#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>

// Biến toàn cục
long long count = 0;       // Số điểm nằm trong hình tròn
long long niter = 0;       // Tổng số lượng điểm
int num_threads = 4;       // Số lượng luồng để tính toán
sem_t sem_count;           // Semaphore để bảo vệ biến count

// Hàm thực thi của mỗi luồng
void* monte_carlo_pi(void* arg) {
    long long iterations = *(long long*)arg;
    long long local_count = 0;
    
    // Seed random cục bộ cho mỗi luồng để tránh trùng lặp
    unsigned int seed = time(NULL) ^ pthread_self();

    for (long long i = 0; i < iterations; i++) {
        // Sinh tọa độ x, y ngẫu nhiên trong khoảng [-1.0, 1.0]
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;

        // Kiểm tra điểm có nằm trong hình tròn bán kính r=1
        if (x * x + y * y <= 1.0) {
            local_count++;
        }
    }

    // Bảo vệ vùng critical section khi cập nhật biến đếm toàn cục count
    sem_wait(&sem_count);
    count += local_count;
    sem_post(&sem_count);

    return NULL;
}

/*
 Giải thích tư duy khởi tạo Semaphore:
 - Ở bài này, Semaphore được dùng như một Mutex (Mutual Exclusion) để khóa quyền truy cập vào biến `count`.
 - Ta khởi tạo `sem_count` bằng 1: `sem_init(&sem_count, 0, 1);`
 - Nghĩa là tại một thời điểm, chỉ có 1 luồng duy nhất được phép lấy tài nguyên (semaphore) để cộng dồn `local_count` vào `count`.
 - Nếu khởi tạo bằng 0, không luồng nào có thể vào được vùng critical section (bị deadlock).
*/

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Su dung: %s <so_luong_diem_ngau_nhien>\n", argv[0]);
        printf("Vi du: %s 1000000\n", argv[0]);
        return 1;
    }

    niter = atoll(argv[1]);
    if (niter <= 0) {
        printf("So luong diem phai lon hon 0.\n");
        return 1;
    }

    pthread_t threads[num_threads];
    long long iter_per_thread = niter / num_threads;
    long long remaining_iters = niter % num_threads;

    // Khởi tạo semaphore với giá trị 1 (đóng vai trò như Mutex)
    sem_init(&sem_count, 0, 1);

    printf("Dang uoc tinh so Pi voi %lld diem ngau nhien bang %d luong...\n", niter, num_threads);

    // Tạo các luồng
    for (int i = 0; i < num_threads; i++) {
        long long* iters = malloc(sizeof(long long));
        *iters = iter_per_thread;
        // Gán phần dư cho luồng đầu tiên
        if (i == 0) {
            *iters += remaining_iters;
        }
        pthread_create(&threads[i], NULL, monte_carlo_pi, iters);
    }

    // Chờ các luồng hoàn thành
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Tính toán số Pi
    double pi_estimate = 4.0 * (double)count / (double)niter;
    double pi_actual = acos(-1.0); // Số Pi chuẩn

    // Đánh giá
    printf("So diem trong hinh tron: %lld\n", count);
    printf("Tong so diem tao ra   : %lld\n", niter);
    printf("Gia tri Pi uoc tinh   : %.10f\n", pi_estimate);
    printf("Gia tri Pi chuan      : %.10f\n", pi_actual);
    printf("Do lech tuyet doi     : %.10f\n", fabs(pi_estimate - pi_actual));

    // Hủy semaphore
    sem_destroy(&sem_count);

    return 0;
}
