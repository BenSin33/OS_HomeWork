#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void print_process_info(const char *name) {
    printf("Tien trinh %s: PID = %d, PPID = %d\n", name, getpid(), getppid());
}

int main() {
    printf("Khoi tao cay tien trinh:\n");
    print_process_info("A (Goc)");

    pid_t pid_b = fork();
    
    if (pid_b < 0) {
        printf("Loi tao tien trinh B\n");
        exit(1);
    } else if (pid_b == 0) {
        // Day la tien trinh B
        print_process_info("B");

        pid_t pid_d = fork();
        if (pid_d == 0) {
            // Day la tien trinh D
            print_process_info("D");
            exit(0); // D hoan thanh
        } else {
            pid_t pid_e = fork();
            if (pid_e == 0) {
                // Day la tien trinh E
                print_process_info("E");
                exit(0); // E hoan thanh
            } else {
                // B cho D va E
                wait(NULL);
                wait(NULL);
                exit(0); // B hoan thanh
            }
        }
    } else {
        // Day la tien trinh A
        pid_t pid_c = fork();
        
        if (pid_c < 0) {
            printf("Loi tao tien trinh C\n");
            exit(1);
        } else if (pid_c == 0) {
            // Day la tien trinh C
            print_process_info("C");

            pid_t pid_h = fork();
            if (pid_h == 0) {
                // Day la tien trinh H
                print_process_info("H");
                exit(0); // H hoan thanh
            } else {
                // C cho H
                wait(NULL);
                exit(0); // C hoan thanh
            }
        } else {
            // A cho B va C
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
