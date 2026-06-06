#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    if (n <= 3) {
        printf("n phai lon hon 3\n");
        return 1;
    }

    int fd1[2]; // Cha truyen n cho con
    int fd2[2]; // Con truyen ket qua cho cha

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Tien trinh con
        close(fd1[1]); // Dong dau ghi cua fd1
        close(fd2[0]); // Dong dau doc cua fd2

        int received_n;
        // Đọc số n từ pipe
        if (read(fd1[0], &received_n, sizeof(int)) > 0) {
            long long result = factorial(received_n);
            // Ghi kết quả vào pipe trả về cha
            write(fd2[1], &result, sizeof(long long));
        }
        
        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    } else {
        // Tien trinh cha
        close(fd1[0]); // Dong dau doc cua fd1
        close(fd2[1]); // Dong dau ghi cua fd2

        // Ghi số n vào pipe cho con
        write(fd1[1], &n, sizeof(int));
        close(fd1[1]);

        wait(NULL); // Doi con tinh toan xong

        long long result;
        // Đọc kết quả từ con
        if (read(fd2[0], &result, sizeof(long long)) > 0) {
            printf("%d! = %lld\n", n, result);
        }
        
        close(fd2[0]);
    }

    return 0;
}
