#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// Struct để truyền dữ liệu
typedef struct {
    int a;
    int b;
    char op;
} CalculationData;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <a> <b> <op>\n", argv[0]);
        printf("Note: for multiplication, use 'x' instead of '*' in terminal\n");
        return 1;
    }

    CalculationData data;
    data.a = atoi(argv[1]);
    data.b = atoi(argv[2]);
    data.op = argv[3][0];

    const char *fifo_req = "/tmp/calc_req";
    const char *fifo_res = "/tmp/calc_res";

    mkfifo(fifo_req, 0666);
    mkfifo(fifo_res, 0666);

    int fd1 = open(fifo_req, O_WRONLY);
    if (fd1 == -1) {
        perror("open req error");
        return 1;
    }

    write(fd1, &data, sizeof(CalculationData));
    close(fd1);

    int fd2 = open(fifo_res, O_RDONLY);
    if (fd2 == -1) {
        perror("open res error");
        return 1;
    }

    double result;
    int error;
    read(fd2, &result, sizeof(double));
    read(fd2, &error, sizeof(int));
    close(fd2);

    FILE *f = fopen("ketqua.txt", "w");
    if (f) {
        if (error == 1) {
            fprintf(f, "Loi chia cho 0\n");
        } else if (error == 2) {
            fprintf(f, "Loi phep toan khong hop le\n");
        } else {
            fprintf(f, "%d %c %d = %g\n", data.a, data.op, data.b, result);
        }
        fclose(f);
        printf("Da ghi ket qua vao file ketqua.txt\n");
    } else {
        perror("fopen error");
    }

    unlink(fifo_req);
    unlink(fifo_res);

    return 0;
}
