#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>

long long count = 0;       
long long niter = 0;       
int num_threads = 1;       // Để 1 luồng con theo đúng nguyên bản đề bài
sem_t sem_count;           

void* monte_carlo_pi(void* arg) {
    long long iterations = *(long long*)arg;
    long long local_count = 0;
    
    // Seed ngẫu nhiên cục bộ cho mỗi luồng dựa trên Thread ID
    unsigned int seed = time(NULL) ^ pthread_self();

    for (long long i = 0; i < iterations; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;

        if (x * x + y * y <= 1.0) {
            local_count++;
        }
    }

    // ĐỒNG BỘ: Khóa quyền truy cập để cộng dồn vào biến toàn cục count
    sem_wait(&sem_count);
    count += local_count;
    sem_post(&sem_count);

    // GIẢI PHÓNG BỘ NHỚ: Giải phóng biến 'iters' được malloc từ luồng cha
    free(arg); 

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Su dung: %s <so_luong_diem_ngau_nhien>\n", argv[0]);
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

    // KHỞI TẠO SEMAPHORE: Đặt giá trị bằng 1 (đóng vai trò như một Mutex)
    sem_init(&sem_count, 0, 1);

    printf("Dang uoc tinh so Pi voi %lld diem ngau nhien bang %d luong...\n", niter, num_threads);

    for (int i = 0; i < num_threads; i++) {
        long long* iters = malloc(sizeof(long long));
        *iters = iter_per_thread;
        if (i == 0) {
            *iters += remaining_iters;
        }
        pthread_create(&threads[i], NULL, monte_carlo_pi, iters);
    }

    // ĐỒNG BỘ: Chờ luồng con hoàn thành nhiệm vụ
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Tiến trình cha thực hiện tính toán số Pi từ kết quả thu được
    double pi_estimate = 4.0 * (double)count / (double)niter;
    double pi_actual = acos(-1.0); 

    printf("\n--- KET QUA KHAO SAT ---\n");
    printf("So diem trong hinh tron: %lld\n", count);
    printf("Tong so diem tao ra   : %lld\n", niter);
    printf("Gia tri Pi uoc tinh   : %.10f\n", pi_estimate);
    printf("Gia tri Pi chuan      : %.10f\n", pi_actual);
    printf("Do lech tuyet doi     : %.10f\n", fabs(pi_estimate - pi_actual));

    // GIẢI PHÓNG TÀI NGUYÊN: Hủy semaphore sau khi dùng xong
    sem_destroy(&sem_count);

    return 0;
}