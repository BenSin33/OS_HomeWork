#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void* print_primes(void* arg) {
    int n = *((int*)arg);
    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Vui long nhap mot so nguyen! (vi du: ./bai2 20)\n");
        return -1;
    }
    
    int num = atoi(argv[1]);
    if (num < 2) {
        printf("Khong co so nguyen to nao nho hon hoac bang %d\n", num);
        return 0;
    }
    
    pthread_t tid;
    pthread_create(&tid, NULL, print_primes, (void*)&num);
    pthread_join(tid, NULL);
    
    return 0;
}
