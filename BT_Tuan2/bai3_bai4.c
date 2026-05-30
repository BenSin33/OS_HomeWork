#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Cach su dung: %s <so nguyen duong n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Vui long nhap so nguyen duong.\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        printf("Loi fork!\n");
        return 1;
    } else if (pid == 0) {
        // Tien trinh con tinh tong cac uoc so cua n
        int sum_divisors = 0;
        for (int i = 1; i <= n; i++) {
            if (n % i == 0) {
                sum_divisors += i;
            }
        }
        printf("Tien trinh con (PID: %d): Tong cac uoc so cua %d la %d\n", getpid(), n, sum_divisors);
    } else {
        // Tien trinh cha tinh tong 1 toi n
        long long sum_n = 0;
        for (int i = 1; i <= n; i++) {
            sum_n += i;
        }
        printf("Tien trinh cha (PID: %d): Tong S = 1 + 2 + ... + %d la %lld\n", getpid(), n, sum_n);

        // Cho tien trinh con ket thuc
        wait(NULL);
        printf("Tien trinh cha hoan thanh, tien trinh con da duoc thu hoi (khong mo coi).\n");
    }

    return 0;
}
