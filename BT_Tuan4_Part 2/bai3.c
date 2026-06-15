#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem_khung_empty; // Điều khiển việc sản xuất khung mới (Khởi tạo = 1)
sem_t sem_banh_empty;  // Điều khiển việc sản xuất bánh xe (Khởi tạo = 0)
sem_t sem_khung;        // Báo hiệu đã có khung sẵn sàng (Khởi tạo = 0)
sem_t sem_banh;         // Báo hiệu đã có bánh xe sẵn sàng (Khởi tạo = 0)

#define TOTAL_CARS 5

void SXKhung() {
    printf("San xuat khung\n");
    sleep(1);
}

void SXBanhXe() {
    printf("San xuat banh xe\n");
    usleep(500000);
}

void LapRapXe(int car_id) {
    printf("--> Lap rap xe thu %d thanh cong!\n\n", car_id);
    sleep(1);
}

void* thread_khung(void* arg) {
    for (int i = 0; i < TOTAL_CARS; i++) {
        sem_wait(&sem_khung_empty); // Chờ tín hiệu cho phép sản xuất khung tiếp theo
        
        SXKhung();
        
        sem_post(&sem_khung);       // Báo cho bộ phận lắp ráp là đã có 1 khung
        
        // Kích hoạt quota sản xuất đúng 4 bánh xe cho khung hiện tại này
        for (int j = 0; j < 4; j++) {
            sem_post(&sem_banh_empty);
        }
    }
    return NULL;
}

void* thread_banh(void* arg) {
    for (int i = 0; i < TOTAL_CARS * 4; i++) {
        sem_wait(&sem_banh_empty);  // Chỉ được sản xuất khi khung hiện tại yêu cầu bánh xe
        
        SXBanhXe();
        
        sem_post(&sem_banh);        // Báo cho bộ phận lắp ráp là đã có thêm 1 bánh xe
    }
    return NULL;
}

void* thread_laprap(void* arg) {
    for (int i = 1; i <= TOTAL_CARS; i++) {
        // Chờ có đủ 1 khung và 4 bánh xe
        sem_wait(&sem_khung);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);

        LapRapXe(i);

        // Lắp ráp xong xuôi mới cho phép chu kỳ sản xuất khung tiếp theo bắt đầu
        sem_post(&sem_khung_empty);
    }
    return NULL;
}

int main() {
    pthread_t tid_khung, tid_banh, tid_laprap;

    // Khởi tạo các Semaphore
    sem_init(&sem_khung_empty, 0, 1); // Cho phép làm khung đầu tiên ngay lập tức
    sem_init(&sem_banh_empty, 0, 0);  // Ban đầu chưa có khung, chưa được làm bánh
    sem_init(&sem_khung, 0, 0);
    sem_init(&sem_banh, 0, 0);

    printf("===== BAT DAU DAY CHUYEN SAN XUAT (%d XE) =====\n\n", TOTAL_CARS);

    pthread_create(&tid_khung, NULL, thread_khung, NULL);
    pthread_create(&tid_banh, NULL, thread_banh, NULL);
    pthread_create(&tid_laprap, NULL, thread_laprap, NULL);

    pthread_join(tid_khung, NULL);
    pthread_join(tid_banh, NULL);
    pthread_join(tid_laprap, NULL);

    printf("===== HOAN THANH DAY CHUYEN SAN XUAT =====\n");

    sem_destroy(&sem_khung_empty);
    sem_destroy(&sem_banh_empty);
    sem_destroy(&sem_khung);
    sem_destroy(&sem_banh);

    return 0;
}