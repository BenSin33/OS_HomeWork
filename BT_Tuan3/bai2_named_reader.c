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

int main() {
    const char *fifo_req = "/tmp/calc_req";
    const char *fifo_res = "/tmp/calc_res";

    mkfifo(fifo_req, 0666);
    mkfifo(fifo_res, 0666);

    printf("Reader dang doi du lieu...\n");

    int fd1 = open(fifo_req, O_RDONLY);
    if (fd1 == -1) {
        perror("open req error");
        return 1;
    }

    CalculationData recv_data;
    read(fd1, &recv_data, sizeof(CalculationData));
    close(fd1);

    double result = 0;
    int error = 0;
    switch (recv_data.op) {
        case '+': result = recv_data.a + recv_data.b; break;
        case '-': result = recv_data.a - recv_data.b; break;
        case 'x':
        case '*': result = recv_data.a * recv_data.b; break;
        case '/': 
            if (recv_data.b != 0) result = (double)recv_data.a / recv_data.b;
            else error = 1;
            break;
        default: error = 2; break;
    }

    int fd2 = open(fifo_res, O_WRONLY);
    if (fd2 == -1) {
        perror("open res error");
        return 1;
    }

    write(fd2, &result, sizeof(double));
    write(fd2, &error, sizeof(int));
    close(fd2);

    printf("Da tinh toan va gui ket qua tra ve.\n");

    return 0;
}
