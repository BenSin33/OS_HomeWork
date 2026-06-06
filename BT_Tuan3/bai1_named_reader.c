#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    const char *fifo1 = "/tmp/ff.1"; // pipe để nhận n
    const char *fifo2 = "/tmp/ff.2"; // pipe để trả kết quả

    // Tạo FIFO nếu chưa tồn tại
    mkfifo(fifo1, 0666);
    mkfifo(fifo2, 0666);

    printf("Reader dang doi du lieu...\n");

    // Mở fifo1 để đọc
    int fd1 = open(fifo1, O_RDONLY);
    if (fd1 == -1) {
        perror("open fifo1 error");
        return 1;
    }

    int n;
    read(fd1, &n, sizeof(int));
    close(fd1);

    printf("Reader nhan duoc n = %d, dang tinh toan...\n", n);
    long long result = factorial(n);

    // Mở fifo2 để ghi
    int fd2 = open(fifo2, O_WRONLY);
    if (fd2 == -1) {
        perror("open fifo2 error");
        return 1;
    }

    write(fd2, &result, sizeof(long long));
    close(fd2);

    printf("Da gui ket qua tra ve.\n");

    return 0;
}
