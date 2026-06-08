#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Định nghĩa cấu trúc thông điệp
struct msgbuf {
    long mtype;       // Loại thông điệp (Bắt buộc phải là số dương)
    int n;            // Dữ liệu: số nguyên n cần tính giai thừa
    long long result; // Dữ liệu: kết quả n!
};

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng đối số
    if (argc != 2) {
        printf("Su dung: %s <so_nguyen_n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 3) {
        printf("Loi: So nguyen phai lon hon 3.\n");
        return 1;
    }

    // 1. Tạo hoặc lấy ID của Message Queue dùng chung
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

    // 2. Chuẩn bị thông điệp gửi đi
    struct msgbuf msg;
    msg.mtype = 1; // Loại 1 dành cho chiều Client -> Server
    msg.n = n;
    msg.result = 0;

    // Gửi thông điệp
    if (msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0) == -1) {
        perror("Loi msgsnd");
        return 1;
    }
    printf("Client: Da gui n = %d. Dang cho ket qua...\n", n);

    // 3. Chờ nhận phản hồi từ Server
    // Lọc lấy các thông điệp có mtype = 2 (Server trả về)
    if (msgrcv(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 2, 0) == -1) {
        perror("Loi msgrcv");
        return 1;
    }

    // In kết quả theo đúng định dạng yêu cầu
    printf("%d! = %lld\n", msg.n, msg.result);

    return 0;
}
