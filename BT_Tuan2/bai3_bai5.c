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
        // Tien trinh con
        printf("Day so Collatz tu %d la:\n", n);
        printf("%d", n);
        while (n != 1) {
            if (n % 2 == 0) {
                n = n / 2;
            } else {
                n = 3 * n + 1;
            }
            printf(", %d", n);
        }
        printf("\n");
    } else {
        // Tien trinh cha cho tien trinh con
        wait(NULL);
    }

    return 0;
}
