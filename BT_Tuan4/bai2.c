#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_SIZE 100

int main() {
    // 1. Tiến trình cha khởi tạo 2 vùng nhớ dùng chung
    // ftok() tạo ra các key duy nhất dựa vào tên file và ID (project ID)
    key_t key0 = ftok("bai2.c", 65);
    key_t key1 = ftok("bai2.c", 66);

    // shmget() xin cấp phát vùng nhớ chia sẻ. IPC_CREAT để tạo mới nếu chưa có
    int shmid0 = shmget(key0, (MAX_SIZE + 1) * sizeof(int), 0666 | IPC_CREAT);
    int shmid1 = shmget(key1, sizeof(int), 0666 | IPC_CREAT);

    if (shmid0 == -1 || shmid1 == -1) {
        perror("shmget failed");
        exit(1);
    }
    
    // 2. Dùng fork() tạo tiến trình con
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) { // Tiến trình con
        // Gắn vùng nhớ SM_0 vào không gian bộ nhớ của tiến trình con
        int *sm0 = (int*) shmat(shmid0, NULL, 0);
        
        srand(time(NULL));
        int n = rand() % 10 + 5; // Số phần tử ngẫu nhiên từ 5 đến 14
        sm0[0] = n; // Ghi số lượng phần tử vào vị trí đầu tiên
        
        printf("Tiến trình con: Tạo mảng ngẫu nhiên gồm %d phần tử:\n", n);
        for(int i = 1; i <= n; i++) {
            sm0[i] = rand() % 100; // Giá trị từ 0 đến 99
            printf("%d ", sm0[i]);
        }
        printf("\n");
        
        // Tháo vùng nhớ SM_0 khỏi tiến trình con
        shmdt(sm0);
        
        // Gắn vùng nhớ SM_1 để chờ đọc kết quả
        int *sm1 = (int*) shmat(shmid1, NULL, 0);
        
        // Đợi cha tính toán xong (khi sm1[0] được ghi giá trị khác 0)
        while (sm1[0] == 0) {
            usleep(10000); // Đợi 10ms
        }
        
        printf("Tiến trình con: Nhận được kết quả tổng từ cha = %d\n", sm1[0]);
        shmdt(sm1);
        
        exit(0);
    } else { // Tiến trình cha
        // Gắn vùng nhớ SM_0 để chờ đọc dữ liệu
        int *sm0 = (int*) shmat(shmid0, NULL, 0);
        
        // Đợi tiến trình con ghi dữ liệu vào sm0[0]
        while (sm0[0] == 0) {
            usleep(10000); // Đợi 10 miligiây để tránh Busy Waiting
        }
        
        int n = sm0[0];
        int sum = 0;
        
        // Tính tổng các phần tử
        for(int i = 1; i <= n; i++) {
            sum += sm0[i];
        }
        shmdt(sm0); // Tháo SM_0 sau khi đọc xong
        
        // Gắn vùng nhớ SM_1 để ghi kết quả
        int *sm1 = (int*) shmat(shmid1, NULL, 0);
        sm1[0] = sum; // Ghi tổng vào vị trí đầu tiên
        shmdt(sm1); // Tháo SM_1
        
        // Đợi tiến trình con kết thúc để dọn dẹp
        wait(NULL);
        
        // 6. Dọn dẹp và hủy 2 vùng nhớ chia sẻ
        shmctl(shmid0, IPC_RMID, NULL);
        shmctl(shmid1, IPC_RMID, NULL);
        printf("Tiến trình cha: Đã hủy cả 2 vùng nhớ dùng chung và kết thúc.\n");
    }

    return 0;
}
