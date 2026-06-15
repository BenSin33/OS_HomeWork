#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Khai báo các semaphore
sem_t sem_khung_empty; // Số lượng khung có thể sản xuất (giới hạn tối đa)
sem_t sem_banh_empty;  // Số lượng bánh có thể sản xuất cho xe hiện tại (giới hạn tối đa)
sem_t sem_khung;       // Số lượng khung ĐÃ sản xuất xong, chờ lắp ráp
sem_t sem_banh;        // Số lượng bánh ĐÃ sản xuất xong, chờ lắp ráp

// Tổng số xe cần sản xuất để chạy thử
#define TOTAL_CARS 5

void SXKhung() {
    printf("San xuat khung\n");
    sleep(1); // Mô phỏng thời gian sản xuất
}

void SXBanhXe() {
    printf("San xuat banh xe\n");
    usleep(500000); // Mô phỏng thời gian sản xuất bánh (nhanh hơn khung)
}

void LapRapXe(int car_id) {
    printf("--> Lap rap xe thu %d thanh cong!\n\n", car_id);
    sleep(1); // Mô phỏng thời gian lắp ráp
}

// Luồng sản xuất khung xe
void* thread_khung(void* arg) {
    for (int i = 0; i < TOTAL_CARS; i++) {
        sem_wait(&sem_khung_empty); // Xin phép sản xuất khung (block nếu đã có 1 khung chưa lắp)
        SXKhung();
        sem_post(&sem_khung);       // Báo hiệu đã sản xuất xong 1 khung
    }
    return NULL;
}

// Luồng sản xuất bánh xe
void* thread_banh(void* arg) {
    for (int i = 0; i < TOTAL_CARS * 4; i++) {
        sem_wait(&sem_banh_empty);  // Xin phép sản xuất bánh (block nếu đã dư 4 bánh chưa lắp)
        SXBanhXe();
        sem_post(&sem_banh);        // Báo hiệu đã sản xuất xong 1 bánh
    }
    return NULL;
}

// Luồng lắp ráp xe
void* thread_laprap(void* arg) {
    for (int i = 1; i <= TOTAL_CARS; i++) {
        // Đợi có đủ 1 khung
        sem_wait(&sem_khung);
        
        // Đợi có đủ 4 bánh xe
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);

        // Lắp ráp
        LapRapXe(i);

        // Sau khi lắp ráp xong, cấp lại "quota" (chỗ trống) để sản xuất xe tiếp theo
        sem_post(&sem_khung_empty); // Cho phép sản xuất 1 khung mới
        sem_post(&sem_banh_empty);  // Cho phép sản xuất bánh 1
        sem_post(&sem_banh_empty);  // Cho phép sản xuất bánh 2
        sem_post(&sem_banh_empty);  // Cho phép sản xuất bánh 3
        sem_post(&sem_banh_empty);  // Cho phép sản xuất bánh 4
    }
    return NULL;
}

/*
 GIẢI THÍCH TƯ DUY KHỞI TẠO SEMAPHORE TRONG BÀI TOÁN DÂY CHUYỀN:

 1. sem_khung_empty (Khởi tạo = 1): 
    - Semaphore này đại diện cho "không gian trống" (quota) để sản xuất khung.
    - Yêu cầu "tại mỗi thời điểm chỉ cho phép sản xuất tối đa 1 khung xe", do đó ta cho quota ban đầu là 1.
    - Khi luồng sản xuất khung chạy, nó lấy đi 1 quota. Nếu nó chạy nhanh hơn lắp ráp, nó sẽ bị block ở vòng lặp kế tiếp vì quota = 0, tránh ứ đọng khung.

 2. sem_banh_empty (Khởi tạo = 4):
    - Tương tự như khung, ta chỉ muốn bánh xe sản xuất đúng 4 chiếc cho 1 khung hiện tại (tránh tồn kho quá nhiều).
    - Quota ban đầu là 4. Luồng bánh xe sẽ dừng lại nếu nó đã sản xuất đủ 4 bánh mà chưa được lắp ráp.

 3. sem_khung (Khởi tạo = 0):
    - Đại diện cho số khung ĐÃ hoàn thành. Ban đầu chưa có khung nào nên bằng 0.
    - Hàm LapRapXe cần wait semaphore này để đảm bảo khung đã sẵn sàng.

 4. sem_banh (Khởi tạo = 0):
    - Đại diện cho số bánh ĐÃ hoàn thành. Ban đầu bằng 0.
    - Hàm LapRapXe cần wait semaphore này 4 lần, tương đương yêu cầu 4 bánh xe mới được lắp ráp.
    - Nhờ logic này, tỷ lệ luôn được giữ chính xác 1 khung : 4 bánh và không bị deadlock hay lệch nhịp.
*/

int main() {
    pthread_t tid_khung, tid_banh, tid_laprap;

    // Khởi tạo semaphore
    sem_init(&sem_khung_empty, 0, 1);
    sem_init(&sem_banh_empty, 0, 4);
    sem_init(&sem_khung, 0, 0);
    sem_init(&sem_banh, 0, 0);

    printf("===== BAT DAU DAY CHUYEN SAN XUAT (%d XE) =====\n\n", TOTAL_CARS);

    // Tạo các luồng
    pthread_create(&tid_khung, NULL, thread_khung, NULL);
    pthread_create(&tid_banh, NULL, thread_banh, NULL);
    pthread_create(&tid_laprap, NULL, thread_laprap, NULL);

    // Chờ các luồng kết thúc
    pthread_join(tid_khung, NULL);
    pthread_join(tid_banh, NULL);
    pthread_join(tid_laprap, NULL);

    printf("===== HOAN THANH DAY CHUYEN SAN XUAT =====\n");

    // Hủy semaphore
    sem_destroy(&sem_khung_empty);
    sem_destroy(&sem_banh_empty);
    sem_destroy(&sem_khung);
    sem_destroy(&sem_banh);

    return 0;
}
