#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

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

    const char *fifo1 = "/tmp/ff.1"; // pipe để gửi n
    const char *fifo2 = "/tmp/ff.2"; // pipe để nhận kết quả

    // Tạo FIFO nếu chưa tồn tại
    mkfifo(fifo1, 0666);
    mkfifo(fifo2, 0666);

    // Mở fifo1 để ghi số n
    int fd1 = open(fifo1, O_WRONLY);
    if (fd1 == -1) {
        perror("open fifo1 error");
        return 1;
    }

    write(fd1, &n, sizeof(int));
    close(fd1);

    // Mở fifo2 để đọc kết quả
    int fd2 = open(fifo2, O_RDONLY);
    if (fd2 == -1) {
        perror("open fifo2 error");
        return 1;
    }

    long long result;
    read(fd2, &result, sizeof(long long));
    close(fd2);

    printf("%d! = %lld\n", n, result);

    // Xóa FIFO
    unlink(fifo1);
    unlink(fifo2);

    return 0;
}
