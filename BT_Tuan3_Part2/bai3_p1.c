#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_SIZE 100

// Cấu trúc thông điệp (Khớp với P0)
struct msgbuf {
    long mtype;
    int id;
    int size;
    int arr[MAX_SIZE];
    long long sum;
};

// Hàm hỗ trợ so sánh cho thuật toán qsort
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    // 1. Tạo/Liên kết Message Queue
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

    printf("P1 (Tien trinh xu ly): Dang chay, cho du lieu tu P0...\n");

    struct msgbuf msg;
    while (1) {
        // 2. Chờ nhận mảng cần xử lý từ P0 (mtype = 1)
        if (msgrcv(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 1, 0) == -1) {
            perror("Loi msgrcv");
            continue;
        }

        // Nếu kích thước gửi sang bằng 0 => Tín hiệu P0 báo đã đọc hết file
        if (msg.size == 0) {
            printf("P1: Da xu ly het du lieu. P1 dang thoat.\n");
            break;
        }

        printf("P1: Da nhan day so [%d] gom %d phan tu. Dang xu ly...\n", msg.id, msg.size);

        // 3. Tính tổng và sắp xếp dãy số nhận được
        msg.sum = 0;
        for (int i = 0; i < msg.size; i++) {
            msg.sum += msg.arr[i];
        }

        // Sắp xếp mảng tăng dần bằng thuật toán QuickSort tích hợp sẵn
        qsort(msg.arr, msg.size, sizeof(int), compare);

        // 4. Gửi kết quả (mảng đã xếp + tổng) ngược lại cho P0 (mtype = 2)
        msg.mtype = 2; // Loại 2: P1 -> P0
        if (msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0) == -1) {
            perror("Loi msgsnd");
        } else {
            printf("P1: Da tra ve ket qua cho day so [%d] toi P0.\n", msg.id);
        }
    }

    return 0;
}
