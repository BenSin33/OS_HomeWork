#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem_khung_empty;
sem_t sem_banh_empty;
sem_t sem_khung;
sem_t sem_banh;

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
        sem_wait(&sem_khung_empty); // Chờ có chỗ trống để sản xuất khung (giới hạn kho)
        SXKhung();
        sem_post(&sem_khung);       // Tăng số lượng khung sẵn sàng cho việc lắp ráp
    }
    return NULL;
}

void* thread_banh(void* arg) {
    for (int i = 0; i < TOTAL_CARS * 4; i++) {
        sem_wait(&sem_banh_empty);  // Chờ có chỗ trống để sản xuất bánh xe (tối đa 4 chiếc)
        SXBanhXe();
        sem_post(&sem_banh);        // Tăng số lượng bánh xe sẵn sàng cho việc lắp ráp
    }
    return NULL;
}

void* thread_laprap(void* arg) {
    for (int i = 1; i <= TOTAL_CARS; i++) {
        // Chờ đủ nguyên liệu: 1 khung và 4 bánh xe có sẵn trong kho
        sem_wait(&sem_khung);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);
        sem_wait(&sem_banh);

        LapRapXe(i);

        // Giải phóng không gian kho để các thread sản xuất tiếp tục làm việc
        sem_post(&sem_khung_empty);
        sem_post(&sem_banh_empty);
        sem_post(&sem_banh_empty);
        sem_post(&sem_banh_empty);
        sem_post(&sem_banh_empty);
    }
    return NULL;
}

int main() {
    pthread_t tid_khung, tid_banh, tid_laprap;

    // Khởi tạo các Semaphore điều phối
    sem_init(&sem_khung_empty, 0, 1); // Cho phép sản xuất tối đa 1 khung chờ lắp ráp
    sem_init(&sem_banh_empty, 0, 4);  // Cho phép sản xuất tối đa 4 bánh xe chờ lắp ráp
    sem_init(&sem_khung, 0, 0);       // Ban đầu chưa có khung nào sẵn sàng
    sem_init(&sem_banh, 0, 0);       // Ban đầu chưa có bánh xe nào sẵn sàng

    printf("===== BAT DAU DAY CHUYEN SAN XUAT (%d XE) =====\n\n", TOTAL_CARS);

    // Kích hoạt các luồng chạy song song
    pthread_create(&tid_khung, NULL, thread_khung, NULL);
    pthread_create(&tid_banh, NULL, thread_banh, NULL);
    pthread_create(&tid_laprap, NULL, thread_laprap, NULL);

    // Chờ tất cả các luồng hoàn thành nhiệm vụ
    pthread_join(tid_khung, NULL);
    pthread_join(tid_banh, NULL);
    pthread_join(tid_laprap, NULL);

    printf("===== HOAN THANH DAY CHUYEN SAN XUAT =====\n");

    // Hủy Semaphore giải phóng tài nguyên hệ thống
    sem_destroy(&sem_khung_empty);
    sem_destroy(&sem_banh_empty);
    sem_destroy(&sem_khung);
    sem_destroy(&sem_banh);

    return 0;
}