#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Khai báo 2 semaphore để đồng bộ hóa
sem_t sem_odd;
sem_t sem_even;

#define MAX_NUM 11

// Hàm cho luồng in số lẻ
void* print_odd(void* arg) {
    for (int i = 1; i <= MAX_NUM; i += 2) {
        // Đợi đến lượt in số lẻ (sem_odd > 0)
        sem_wait(&sem_odd);
        
        printf("%d ", i);
        fflush(stdout);
        
        // Kích hoạt luồng in số chẵn (tăng sem_even lên 1)
        sem_post(&sem_even);
    }
    return NULL;
}

// Hàm cho luồng in số chẵn
void* print_even(void* arg) {
    for (int i = 2; i <= MAX_NUM; i += 2) {
        // Đợi đến lượt in số chẵn (sem_even > 0)
        sem_wait(&sem_even);
        
        printf("%d ", i);
        fflush(stdout);
        
        // Kích hoạt luồng in số lẻ (tăng sem_odd lên 1)
        sem_post(&sem_odd);
    }
    return NULL;
}

/*
 Giải thích tư duy khởi tạo Semaphore:
 - Chúng ta muốn số 1 (số lẻ) được in đầu tiên. Do đó, `sem_odd` được khởi tạo bằng 1.
 - Khi luồng lẻ bắt đầu, nó gọi `sem_wait(&sem_odd)`, giảm `sem_odd` xuống 0 và tiếp tục thực thi.
 - Trong khi đó, `sem_even` được khởi tạo bằng 0. Luồng chẵn gọi `sem_wait(&sem_even)` sẽ bị block (chờ) cho đến khi luồng lẻ chạy xong vòng lặp đầu tiên.
 - Sau khi in số 1, luồng lẻ gọi `sem_post(&sem_even)`, tăng `sem_even` lên 1, đánh thức luồng chẵn.
 - Quá trình lặp lại tương tự theo dạng "ping-pong" cho đến khi in hết số 11.
*/

int main() {
    pthread_t tid_odd, tid_even;

    // Khởi tạo semaphore
    // sem_init(con trỏ sem, pshared, giá trị khởi tạo)
    // pshared = 0 nghĩa là semaphore dùng giữa các thread trong cùng 1 process
    sem_init(&sem_odd, 0, 1);  // Luồng lẻ chạy trước
    sem_init(&sem_even, 0, 0); // Luồng chẵn phải chờ luồng lẻ

    printf("Bat dau in day so tu 1 den 11:\n");

    // Tạo 2 luồng
    pthread_create(&tid_odd, NULL, print_odd, NULL);
    pthread_create(&tid_even, NULL, print_even, NULL);

    // Chờ 2 luồng kết thúc
    pthread_join(tid_odd, NULL);
    pthread_join(tid_even, NULL);

    printf("\nHoan thanh!\n");

    // Hủy semaphore
    sem_destroy(&sem_odd);
    sem_destroy(&sem_even);

    return 0;
}
