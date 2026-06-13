#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_SIZE 100

// Thiết kế một Struct rõ ràng để quản lý trạng thái, tránh giá trị rác của RAM
typedef struct {
    int n;
    int data[MAX_SIZE];
    int sum;
    int status; // 0: Con đang tạo mảng, 1: Cha đang tính tổng, 2: Con đọc kết quả
} SharedBuffer;

int main() {
    // 1. Tạo 2 key và xin cấp phát 2 vùng nhớ chia sẻ
    key_t key0 = ftok("bai2.c", 65);
    key_t key1 = ftok("bai2.c", 66);

    int shmid0 = shmget(key0, sizeof(SharedBuffer), 0666 | IPC_CREAT);
    int shmid1 = shmget(key1, sizeof(int), 0666 | IPC_CREAT);

    if (shmid0 == -1 || shmid1 == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Gắn vùng nhớ ở cha trước để thiết lập trạng thái ban đầu ổn định
    SharedBuffer *shared_init = (SharedBuffer*) shmat(shmid0, NULL, 0);
    shared_init->status = 0; // Đặt status = 0 để báo hiệu con chạy trước
    shared_init->n = 0;
    shmdt(shared_init);

    int *sm1_init = (int*) shmat(shmid1, NULL, 0);
    sm1_init[0] = 0;
    shmdt(sm1_init);

    // 2. Tạo tiến trình con
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) { // === TIẾN TRÌNH CON ===
        // Tự gắn vùng nhớ vào không gian địa chỉ của con
        SharedBuffer *shared = (SharedBuffer*) shmat(shmid0, NULL, 0);
        int *sm1 = (int*) shmat(shmid1, NULL, 0);
        
        srand(time(NULL) ^ getpid());
        int count = rand() % 10 + 5; // Sinh mảng từ 5 đến 14 phần tử
        shared->n = count;
        
        printf("Tiến trình con: Tạo mảng ngẫu nhiên gồm %d phần tử:\n", count);
        for(int i = 0; i < count; i++) {
            shared->data[i] = rand() % 100;
            printf("%d ", shared->data[i]);
        }
        printf("\n");
        
        // Đổi trạng thái sang 1 để báo cho cha biết con đã ghi xong mảng
        shared->status = 1;
        
        // Chờ tiến trình cha tính toán xong (Đợi sm1[0] có kết quả khác 0)
        while (sm1[0] == 0) {
            usleep(10000); // Ngủ 10ms để nhường CPU cho cha tính
        }
        
        printf("Tiến trình con: Nhận được kết quả tổng từ cha = %d\n", sm1[0]);
        
        // Tháo các vùng nhớ trước khi thoát
        shmdt(shared);
        shmdt(sm1);
        exit(0);
        
    } else { // === TIẾN TRÌNH CHA ===
        SharedBuffer *shared = (SharedBuffer*) shmat(shmid0, NULL, 0);
        
        // Đợi cho đến khi status chuyển sang 1 (Con ghi xong mảng)
        while (shared->status != 1) {
            usleep(10000);
        }
        
        int total_elements = shared->n;
        int current_sum = 0;
        
        // Tính tổng
        for(int i = 0; i < total_elements; i++) {
            current_sum += shared->data[i];
        }
        printf("Tiến trình cha: Đã tính xong tổng mảng = %d. Đang gửi sang SM_1...\n", current_sum);
        
        // Tháo SM_0 sau khi dùng xong
        shmdt(shared);
        
        // Gắn vùng nhớ SM_1 để ghi kết quả cho con nhận
        int *sm1 = (int*) shmat(shmid1, NULL, 0);
        sm1[0] = current_sum; 
        shmdt(sm1); // Tháo SM_1
        
        // Đợi tiến trình con in kết quả và kết thúc hoàn toàn
        wait(NULL);
        
        // 6. Hủy phân vùng Shared Memory khỏi hệ thống RAM
        shmctl(shmid0, IPC_RMID, NULL);
        shmctl(shmid1, IPC_RMID, NULL);
        printf("Tiến trình cha: Đã hủy cả 2 vùng nhớ dùng chung và kết thúc hệ thống.\n");
    }

    return 0;
}