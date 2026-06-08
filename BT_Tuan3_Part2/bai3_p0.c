#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_SIZE 100

// Cấu trúc thông điệp truyền qua Queue
struct msgbuf {
    long mtype;
    int id;           // ID/Dòng của dãy số để phân biệt
    int size;         // Số lượng phần tử
    int arr[MAX_SIZE];// Dãy số nguyên
    long long sum;    // Tổng để trả về
};

// Cấu trúc lưu trữ dữ liệu để P0 sắp xếp ở cuối
struct ArrayData {
    int id;
    int size;
    int arr[MAX_SIZE];
    long long sum;
};

int main() {
    // 1. Tạo Message Queue
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

    // 2. Mở file "data" để đọc
    FILE *f_in = fopen("data", "r");
    if (!f_in) {
        perror("Loi mo file 'data'. Vui long tao file 'data' truoc");
        return 1;
    }

    struct msgbuf msg;
    char line[1024];
    int line_count = 0;

    printf("P0: Bat dau doc du lieu tu file 'data'...\n");
    // Đọc từng dòng dãy số
    while (fgets(line, sizeof(line), f_in)) {
        msg.mtype = 1; // Loại 1: Gửi cho P1
        msg.id = line_count;
        msg.size = 0;
        
        // Tách các số trên dòng
        char *token = strtok(line, " \t\n");
        while (token != NULL && msg.size < MAX_SIZE) {
            msg.arr[msg.size++] = atoi(token);
            token = strtok(NULL, " \t\n");
        }

        // Gửi dãy số
        if (msg.size > 0) {
            if (msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0) == -1) {
                perror("Loi msgsnd");
            } else {
                printf("P0: Da gui day so [%d] (%d phan tu) cho P1.\n", msg.id, msg.size);
                line_count++;
            }
        }
    }
    fclose(f_in);

    // Gửi thông điệp size=0 để báo cho P1 biết P0 đã đọc xong file
    msg.mtype = 1;
    msg.size = 0;
    msgsnd(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 0);

    // 3. Chờ nhận lại dữ liệu đã xử lý từ P1 (mtype = 2)
    struct ArrayData *results = malloc(line_count * sizeof(struct ArrayData));
    int received_count = 0;

    printf("P0: Dang cho P1 tra ve ket qua...\n");
    while (received_count < line_count) {
        if (msgrcv(msgid, &msg, sizeof(struct msgbuf) - sizeof(long), 2, 0) == -1) {
            perror("Loi msgrcv");
            continue;
        }

        results[received_count].id = msg.id;
        results[received_count].size = msg.size;
        for (int i = 0; i < msg.size; i++) {
            results[received_count].arr[i] = msg.arr[i];
        }
        results[received_count].sum = msg.sum;
        
        printf("P0: Nhan thanh cong day %d da sap xep (Tong: %lld)\n", msg.id, msg.sum);
        received_count++;
    }

    // 4. Sắp xếp toàn bộ các dãy dựa trên tổng của chúng (Tăng dần)
    for (int i = 0; i < line_count - 1; i++) {
        for (int j = i + 1; j < line_count; j++) {
            if (results[i].sum > results[j].sum) {
                struct ArrayData temp = results[i];
                results[i] = results[j];
                results[j] = temp;
            }
        }
    }

    // 5. Ghi các dãy đã được sắp xếp ra file "result"
    FILE *f_out = fopen("result", "w");
    if (!f_out) {
        perror("Loi mo file result");
    } else {
        printf("P0: Dang ghi ket qua vao file 'result'...\n");
        for (int i = 0; i < line_count; i++) {
            fprintf(f_out, "Tong = %lld | Day so: ", results[i].sum);
            for (int j = 0; j < results[i].size; j++) {
                fprintf(f_out, "%d ", results[i].arr[j]);
            }
            fprintf(f_out, "\n");
        }
        fclose(f_out);
        printf("P0: Ghi file hoan tat.\n");
    }

    free(results);

    // 6. Huỷ bỏ System V Message Queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Loi msgctl IPC_RMID");
    } else {
        printf("P0: Da giai phong Message Queue thanh cong.\n");
    }

    return 0;
}
