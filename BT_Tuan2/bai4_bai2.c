#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Loi fork!\n");
        return 1;
    } else if (pid == 0) {
        // Tien trinh con
        printf("Tien trinh con (PID: %d) da hoan thanh cong viec va ket thuc.\n", getpid());
        exit(0);
    } else {
        // Tien trinh cha
        printf("Tien trinh cha (PID: %d) dang ngu trong 15 giay...\n", getpid());
        printf("Trong thoi gian nay, tien trinh con (PID: %d) la mot zombie.\n", pid);
        printf("Hay dung lenh 'ps -l' de kiem tra trang thai 'Z' cua tien trinh con.\n");
        printf("Va su dung 'kill -9 %d' de ket lieu zombie thong qua tien trinh cha.\n", getpid());
        
        sleep(15);
        
        printf("Tien trinh cha thuc day va ket thuc.\n");
    }

    return 0;
}
