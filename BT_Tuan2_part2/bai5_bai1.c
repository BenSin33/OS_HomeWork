#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int average = 0;
int maximum = INT_MIN;
int minimum = INT_MAX;

struct arr {
    int n;
    int *a;
};

void* calc_avg(void* arg) {
    struct arr *ap = (struct arr*) arg;
    int sum = 0;
    for(int i = 0; i < ap->n; i++) {
        sum += ap->a[i];
    }
    if (ap->n > 0) average = sum / ap->n;
    pthread_exit(0);
}

void* calc_max(void* arg) {
    struct arr *ap = (struct arr*) arg;
    for(int i = 0; i < ap->n; i++) {
        if(ap->a[i] > maximum) maximum = ap->a[i];
    }
    pthread_exit(0);
}

void* calc_min(void* arg) {
    struct arr *ap = (struct arr*) arg;
    for(int i = 0; i < ap->n; i++) {
        if(ap->a[i] < minimum) minimum = ap->a[i];
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Vui long nhap cac so nguyen (vi du: ./bai1 90 81 78 95 79 72 85)!\n");
        return -1;
    }
    
    struct arr ar;
    ar.n = argc - 1;
    ar.a = (int*)malloc(ar.n * sizeof(int));
    for (int i = 0; i < ar.n; i++) {
        ar.a[i] = atoi(argv[i+1]);
    }
    
    pthread_t tid[3];
    
    pthread_create(&tid[0], NULL, calc_avg, (void*)&ar);
    pthread_create(&tid[1], NULL, calc_max, (void*)&ar);
    pthread_create(&tid[2], NULL, calc_min, (void*)&ar);
    
    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }
    
    printf("Gia tri trung binh: %d\n", average);
    printf("Gia tri lon nhat: %d\n", maximum);
    printf("Gia tri nho nhat: %d\n", minimum);
    
    free(ar.a);
    return 0;
}
