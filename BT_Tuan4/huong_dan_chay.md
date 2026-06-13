# Hướng Dẫn Bài Tập 9: Giao Tiếp Qua Shared Memory

Thư mục này (`BT_Tuan4`) chứa các tệp mã nguồn C giải bài 2 và bài 3 về giao tiếp giữa các tiến trình thông qua bộ nhớ chia sẻ (Shared Memory) trên Linux.

---

## Cấu trúc dữ liệu cho Bài 3
Trong Bài 3, để quản lý nhiều dãy số, lưu trữ số phần tử, giá trị mảng và tổng, cũng như quản lý trạng thái đồng bộ giữa 3 tiến trình P0, P1, và P2, tôi đã định nghĩa một `struct` như sau:
```c
typedef struct {
    int count;          // Số lượng phần tử trong dãy
    int data[MAX_ELEM]; // Mảng chứa các phần tử
    int sum;            // Biến lưu tổng của dãy
} Sequence;

typedef struct {
    int num_sequences;            // Tổng số dãy số
    Sequence seqs[MAX_ARRAYS];    // Mảng các dãy số
    int state; // Biến đồng bộ trạng thái (0: P0 đọc, 1: P1 xử lý, 2: P2 xử lý, 3: P0 ghi)
} SharedData;
```
Việc này giúp đóng gói toàn bộ trạng thái và dữ liệu của cả bài toán lên vùng nhớ Shared Memory duy nhất. Các tiến trình chỉ việc dùng `sleep()` kèm vòng lặp `while` chờ giá trị `state` hợp lệ để lấy quyền điều khiển (cơ chế xoay vòng - Pipeline).

---

## Hướng Dẫn Biên Dịch và Chạy

Để tạo file thực thi, sử dụng 2 bước biên dịch cho từng file:
1. `gcc -c <file_code.c>` : Biên dịch mã nguồn thành file object (`.o`)
2. `gcc -o <file_code.out> <file_code.o>` : Liên kết file object thành file thực thi

---
### Cách Biên Dịch Hàng Loạt Nhanh Nhất (Dành Cho Linux/Ubuntu)
Bạn có thể chạy 2 dòng lệnh sau trực tiếp trên Terminal để biên dịch và tạo file thực thi `.out` cho toàn bộ các file `.c` cùng lúc:

1. **Lệnh biên dịch toàn bộ ra `.o`:**
   ```bash
   for file in *.c; do gcc -c "$file"; done
   ```
2. **Lệnh tạo file thực thi `.out` toàn bộ từ `.o`:**
   ```bash
   for file in *.o; do gcc -o "${file%.o}.out" "$file"; done
   ```

---

### BÀI 2: Mảng và Tính Tổng trên Shared Memory

Sử dụng `shmget()`, `shmat()` để tạo và chia sẻ mảng cũng như giá trị tổng giữa tiến trình cha và con.

- **Biên dịch:**
  ```bash
  gcc -c bai2.c
  gcc -o bai2.out bai2.o
  ```
- **Chạy lệnh:**
  ```bash
  ./bai2.out
  ```
  *Chương trình sẽ tự sinh mảng ngẫu nhiên (kích thước ngẫu nhiên), in ra mảng ban đầu và hiển thị tổng cuối cùng mà cha đã tính.*

---

### BÀI 3: Pipeline P0 -> P1 -> P2 -> P0

Đọc dữ liệu từ `data.txt`, sao chép vào bộ nhớ chia sẻ, lần lượt cho P1 sắp xếp và tính tổng từng dãy, sau đó P2 sắp xếp lại thứ tự các dãy, cuối cùng P0 ghi lại ra `result.txt`.

- **Chuẩn bị:** Trong cùng thư mục đã có file `data.txt` chứa sẵn một số dãy số ví dụ.
- **Biên dịch:**
  ```bash
  gcc -c bai3.c
  gcc -o bai3.out bai3.o
  ```
- **Chạy lệnh:**
  ```bash
  ./bai3.out
  ```
  *Tiến trình P0 sẽ in thông báo tiến trình thực thi, sau khi chương trình kết thúc hãy dùng lệnh `cat result.txt` để xem kết quả.*
  ```bash
  cat result.txt
  ```
