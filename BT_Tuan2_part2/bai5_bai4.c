#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct arr {
    int n;
    int *a;
};

struct arr global_ar;
int sum_primes = 0;
int *sorted_a = NULL;
int prime_count = 0;
char* input_filename;

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Thread 1: Đọc file đầu vào
void* thr1_read_file(void* arg) {
    FILE *in = fopen(input_filename, "r");
    if (in == NULL) {
        printf("Khong the mo file %s\n", input_filename);
        pthread_exit(NULL);
    }
    fscanf(in, "%d", &global_ar.n);
    global_ar.a = (int*)malloc(global_ar.n * sizeof(int));
    for (int i = 0; i < global_ar.n; i++) {
        fscanf(in, "%d", &global_ar.a[i]);
    }
    fclose(in);
    pthread_exit(0);
}

// Thread 2: Tính tổng các số nguyên tố trong mảng
void* thr2_sum_primes(void* arg) {
    for (int i = 0; i < global_ar.n; i++) {
        if (is_prime(global_ar.a[i])) {
            sum_primes += global_ar.a[i];
        }
    }
    pthread_exit(0);
}

// Thread 3: Sắp xếp mảng tăng dần (theo ví dụ là mảng các số nguyên tố)
void* thr3_sort_primes(void* arg) {
    // Đếm số lượng số nguyên tố
    for (int i = 0; i < global_ar.n; i++) {
        if (is_prime(global_ar.a[i])) {
            prime_count++;
        }
    }
    
    sorted_a = (int*)malloc(prime_count * sizeof(int));
    int idx = 0;
    for (int i = 0; i < global_ar.n; i++) {
        if (is_prime(global_ar.a[i])) {
            sorted_a[idx++] = global_ar.a[i];
        }
    }
    
    // Sắp xếp tăng dần
    for (int i = 0; i < prime_count - 1; i++) {
        for (int j = i + 1; j < prime_count; j++) {
            if (sorted_a[i] > sorted_a[j]) {
                int temp = sorted_a[i];
                sorted_a[i] = sorted_a[j];
                sorted_a[j] = temp;
            }
        }
    }
    pthread_exit(0);
}

// Thread 4: Ghi file result
void* thr4_write_file(void* arg) {
    FILE *out = fopen("result.txt", "w");
    if (out == NULL) {
        printf("Khong the tao file result\n");
        pthread_exit(NULL);
    }
    fprintf(out, "So phan tu mang: %d\n", global_ar.n);
    for (int i = 0; i < global_ar.n; i++) {
        fprintf(out, "%d ", global_ar.a[i]);
    }
    fprintf(out, "\n");
    
    fprintf(out, "Mang cac so nguyen to:\n");
    for (int i = 0; i < global_ar.n; i++) {
        if (is_prime(global_ar.a[i])) {
            fprintf(out, "%d ", global_ar.a[i]);
        }
    }
    fprintf(out, "\n");
    
    fprintf(out, "Tong cac so nguyen to: %d\n", sum_primes);
    
    fprintf(out, "Mang cac so nguyen to da duoc sap xep\n");
    for (int i = 0; i < prime_count; i++) {
        fprintf(out, "%d ", sorted_a[i]);
    }
    fprintf(out, "\n");
    
    fclose(out);
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Vui long truyen ten file dau vao! (vi du: ./bai4 input.txt)\n");
        return -1;
    }
    
    input_filename = argv[1];
    pthread_t tid[4];
    
    // 1. Thread 1 đọc file
    pthread_create(&tid[0], NULL, thr1_read_file, NULL);
    pthread_join(tid[0], NULL); // Cần đồng bộ: phải đọc file xong mới có data để xử lý
    
    if (global_ar.a != NULL) {
        // 2. Thread 2 và Thread 3 có thể chạy song song để tính tổng và sắp xếp
        pthread_create(&tid[1], NULL, thr2_sum_primes, NULL);
        pthread_create(&tid[2], NULL, thr3_sort_primes, NULL);
        
        pthread_join(tid[1], NULL);
        pthread_join(tid[2], NULL);
        
        // 3. Thread 4 ghi file phụ thuộc vào kết quả của các thread trên
        pthread_create(&tid[3], NULL, thr4_write_file, NULL);
        pthread_join(tid[3], NULL);
        
        free(global_ar.a);
        free(sorted_a);
        printf("Da ghi ket qua vao file result.txt thanh cong!\n");
    }
    
    return 0;
}
