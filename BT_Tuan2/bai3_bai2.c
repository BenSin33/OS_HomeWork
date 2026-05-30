#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("Truoc khi fork:\n");
    printf("  Hien tai PID: %d, Cha PID: %d\n\n", getpid(), getppid());

    pid_t pid = fork();

    if (pid < 0) {
        printf("Loi fork!\n");
        return 1;
    } else if (pid == 0) {
        // Tien trinh con
        printf("Tien trinh con:\n");
        printf("  PID cua con: %d, Cha PID cua con: %d\n", getpid(), getppid());
    } else {
        // Tien trinh cha
        // wait(NULL); // Thu bo comment de thay su khac biet
        printf("Tien trinh cha:\n");
        printf("  PID cua cha: %d, Cha PID cua cha: %d\n", getpid(), getppid());
    }

    return 0;
}
