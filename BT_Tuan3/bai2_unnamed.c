#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Struct để truyền dữ liệu giữa các tiến trình
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

    int fd1[2], fd2[2];
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe error");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        return 1;
    } else if (pid == 0) {
        // Tiến trình con
        close(fd1[1]);
        close(fd2[0]);

        CalculationData recv_data;
        read(fd1[0], &recv_data, sizeof(CalculationData));
        close(fd1[0]);

        double result = 0;
        int error = 0; // 0: no error, 1: division by zero, 2: invalid op
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

        write(fd2[1], &result, sizeof(double));
        write(fd2[1], &error, sizeof(int));
        close(fd2[1]);
        exit(0);
    } else {
        // Tiến trình cha
        close(fd1[0]);
        close(fd2[1]);

        write(fd1[1], &data, sizeof(CalculationData));
        close(fd1[1]);

        wait(NULL);

        double result;
        int error;
        read(fd2[0], &result, sizeof(double));
        read(fd2[0], &error, sizeof(int));
        close(fd2[0]);

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
    }

    return 0;
}
