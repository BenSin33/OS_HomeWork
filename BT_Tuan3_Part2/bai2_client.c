#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Định nghĩa cấu trúc thông điệp chứa các toán hạng và toán tử
struct msgbuf {
    long mtype;
    int a;
    int b;
    char op; // Phép toán: '+', '-', 'x', '/'
    double result;
    int error; // Cờ báo lỗi (VD: chia cho 0)
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Su dung: %s <a> <b> <phep_toan>\n", argv[0]);
        printf("Vi du: %s 4 6 +\n", argv[0]);
        return 1;
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    char op = argv[3][0];

    // Tạo hoặc liên kết Message Queue
    key_t key = ftok(".", 1);
    if (key == -1) {
        perror("Loi ftok");
        return 1;
    }

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Loi msgget");
        return 1;
    }

    // Đóng gói dữ liệu gửi đi
    struct msgbuf msg;
    msg.mtype = 1; // mtype = 1 (Gửi đi Server)
    msg.a = a;
    msg.b = b;
    msg.op = op;
    msg.error = 0;

    if (msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0) == -1) {
        perror("Loi msgsnd");
        return 1;
    }
    printf("Client: Da gui yeu cau %d %c %d. Dang cho...\n", a, op, b);

    // Chờ nhận kết quả từ Server (mtype = 2)
    if (msgrcv(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 2, 0) == -1) {
        perror("Loi msgrcv");
        return 1;
    }

    // Xử lý và in kết quả
    if (msg.error) {
        printf("Client: Server bao loi (Phep chia cho 0 hoac toan tu khong hop le).\n");
    } else {
        printf("Ket qua: %d %c %d = %.2f\n", msg.a, msg.op, msg.b, msg.result);
    }

    return 0;
}
