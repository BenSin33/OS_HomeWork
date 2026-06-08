#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Định nghĩa cấu trúc thông điệp (Khớp với Client)
struct msgbuf {
    long mtype;
    int a;
    int b;
    char op;
    double result;
    int error;
};

int main() {
    // Tạo / liên kết Message Queue
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

    printf("Server May Tinh (Bai 2) dang chay...\n");

    struct msgbuf msg;
    // Chạy ngầm chờ đợi tin nhắn
    while (1) {
        // Nhận gói tin có mtype = 1
        if (msgrcv(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 1, 0) == -1) {
            perror("Loi msgrcv");
            continue;
        }

        printf("Server: Nhan yeu cau tinh %d %c %d\n", msg.a, msg.op, msg.b);
        msg.error = 0; // Khởi tạo cờ không lỗi

        // Bóc tách và xử lý toán học
        switch (msg.op) {
            case '+': msg.result = msg.a + msg.b; break;
            case '-': msg.result = msg.a - msg.b; break;
            case 'x':
            case '*': msg.result = msg.a * msg.b; break;
            case '/': 
                if (msg.b == 0) {
                    msg.error = 1; // Lỗi chia cho 0
                } else {
                    msg.result = (double)msg.a / msg.b;
                }
                break;
            default:
                msg.error = 1; // Toán tử không hợp lệ
        }

        // Đóng gói kết quả và gửi phản hồi (mtype = 2)
        msg.mtype = 2;
        if (msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0) == -1) {
            perror("Loi msgsnd");
        } else {
            printf("Server: Da gui ket qua tra lai Client.\n");
        }
    }

    return 0;
}
