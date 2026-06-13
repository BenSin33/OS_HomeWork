#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARRAYS 10
#define MAX_ELEM 50

// Cấu trúc struct trong Shared Memory để lưu trữ dãy số
typedef struct {
    int count;          // Số lượng phần tử trong dãy
    int data[MAX_ELEM]; // Mảng chứa các phần tử
    int sum;            // Biến lưu tổng của dãy
} Sequence;

// Cấu trúc dùng chung cho cả bộ nhớ chia sẻ
typedef struct {
    int num_sequences;            // Tổng số dãy số
    Sequence seqs[MAX_ARRAYS];    // Mảng các dãy số
    // Biến trạng thái để đồng bộ (Pipeline: P0 -> P1 -> P2 -> P0)
    // 0: P0 đang đọc file; 1: P1 đang xử lý; 2: P2 đang xử lý; 3: P0 ghi file
    int state; 
} SharedData;

// Hàm sắp xếp mảng tăng dần
void sort_asc(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] > arr[j]) {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

// Hàm sắp xếp các dãy số theo tổng (tổng nhỏ ở trên, lớn ở dưới)
void sort_seqs(Sequence seqs[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (seqs[i].sum > seqs[j].sum) {
                Sequence temp = seqs[i];
                seqs[i] = seqs[j];
                seqs[j] = temp;
            }
        }
    }
}

int main() {
    // Tạo key dùng chung
    key_t key = ftok("bai3.c", 67);
    
    // Khởi tạo vùng nhớ chia sẻ cho SharedData
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Lỗi shmget");
        exit(1);
    }
    
    // KHÔNG gọi shmat() trước fork() để tránh kế thừa thụ động

    pid_t pid1, pid2;

    // Tạo tiến trình P1
    pid1 = fork();
    if (pid1 == 0) { // Đây là P1
        // Gắn vùng nhớ vào không gian địa chỉ của P1
        SharedData *shared = (SharedData*) shmat(shmid, NULL, 0);
        
        // Chờ P0 hoàn thành việc chép dữ liệu lên SM
        while (shared->state != 1) {
            usleep(10000); // Đợi 10ms thay vì 1s
        }
        
        printf("P1: Đã nhận tín hiệu. Bắt đầu sắp xếp tăng dần và tính tổng từng dãy...\n");
        for (int i = 0; i < shared->num_sequences; i++) {
            // Sắp xếp tăng dần
            sort_asc(shared->seqs[i].data, shared->seqs[i].count);
            
            // Tính tổng
            int sum = 0;
            for (int j = 0; j < shared->seqs[i].count; j++) {
                sum += shared->seqs[i].data[j];
            }
            shared->seqs[i].sum = sum; // Ghi kết quả lên SM
        }
        
        // Chuyển quyền cho P2
        shared->state = 2; 
        shmdt(shared); // Tạm tháo vùng nhớ
        exit(0);
    }

    // Tạo tiến trình P2
    pid2 = fork();
    if (pid2 == 0) { // Đây là P2
        // Gắn vùng nhớ vào không gian địa chỉ của P2
        SharedData *shared = (SharedData*) shmat(shmid, NULL, 0);
        
        // Chờ P1 xử lý xong
        while (shared->state != 2) {
            usleep(10000); // Đợi 10ms thay vì 1s
        }
        
        printf("P2: Đã nhận tín hiệu. Bắt đầu sắp xếp các dãy theo tổng...\n");
        sort_seqs(shared->seqs, shared->num_sequences);
        
        // Chuyển quyền lại cho P0
        shared->state = 3; 
        shmdt(shared); // Tạm tháo vùng nhớ
        exit(0);
    }

    // Tiến trình P0
    // Gắn vùng nhớ vào không gian địa chỉ của P0
    SharedData *shared = (SharedData*) shmat(shmid, NULL, 0);
    
    // Khởi tạo các giá trị ban đầu cho bộ nhớ chia sẻ
    shared->state = 0;
    shared->num_sequences = 0;

    printf("P0: Đang đọc file data.txt và chép lên Shared Memory...\n");
    FILE *fin = fopen("data.txt", "r");
    if (!fin) {
        perror("Lỗi mở file data.txt");
        // Hủy vùng nhớ và kết thúc nếu không có file
        shmctl(shmid, IPC_RMID, NULL);
        shmdt(shared);
        exit(1);
    }

    char line[256];
    int seq_idx = 0;
    while (fgets(line, sizeof(line), fin) && seq_idx < MAX_ARRAYS) {
        char *token = strtok(line, " \t\n");
        int count = 0;
        while (token != NULL && count < MAX_ELEM) {
            shared->seqs[seq_idx].data[count++] = atoi(token);
            token = strtok(NULL, " \t\n");
        }
        if (count > 0) {
            shared->seqs[seq_idx].count = count;
            seq_idx++;
        }
    }
    fclose(fin);
    shared->num_sequences = seq_idx;
    
    // Cho phép P1 bắt đầu
    shared->state = 1; 
    
    // P0: Chờ P2 hoàn thành (state == 3)
    while (shared->state != 3) {
        usleep(10000); // Đợi 10ms
    }
    
    printf("P0: Nhận dữ liệu cuối cùng từ P2. Ghi ra result.txt...\n");
    FILE *fout = fopen("result.txt", "w");
    if (fout) {
        for (int i = 0; i < shared->num_sequences; i++) {
            fprintf(fout, "Sum: %d | Data: ", shared->seqs[i].sum);
            for (int j = 0; j < shared->seqs[i].count; j++) {
                fprintf(fout, "%d ", shared->seqs[i].data[j]);
            }
            fprintf(fout, "\n");
        }
        fclose(fout);
    }
    
    // Chờ 2 tiến trình con kết thúc hoàn toàn
    wait(NULL);
    wait(NULL);
    
    // Hủy vùng nhớ chia sẻ
    shmctl(shmid, IPC_RMID, NULL);
    shmdt(shared);
    printf("P0: Đã ghi xong result.txt, dọn dẹp bộ nhớ và kết thúc!\n");
    
    return 0;
}
