#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Định nghĩa cấu trúc thông điệp (Khớp với Client)
struct msgbuf {
    long mtype;
    int n;
    long long result;
};

int main() {
    // 1. Tạo hoặc lấy ID của Message Queue
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

    printf("Server (Bai 1) dang chay. Cho yeu cau...\n");

    struct msgbuf msg;
    // Chạy ngầm liên tục
    while (1) {
        // 2. Chờ nhận thông điệp từ Client
        // Lọc lấy các thông điệp có mtype = 1
        if (msgrcv(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 1, 0) == -1) {
            perror("Loi msgrcv");
            continue; // Bỏ qua nếu lỗi, chờ tin nhắn tiếp theo
        }

        printf("Server: Nhan duoc n = %d. Dang tinh toan...\n", msg.n);

        // 3. Tính toán giá trị giai thừa (Sử dụng long long để tránh tràn số)
        long long fact = 1;
        for (int i = 1; i <= msg.n; i++) {
            fact *= i;
        }

        // 4. Đóng gói kết quả và gửi lại cho Client
        msg.mtype = 2; // Loại 2 dành cho chiều Server -> Client
        msg.result = fact;

        if (msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0) == -1) {
            perror("Loi msgsnd");
        } else {
            printf("Server: Da gui ket qua %lld tro lai Client.\n", fact);
        }
    }

    return 0;
}
