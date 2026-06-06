# Hướng Dẫn Bài Tập 8: Giao Tiếp Qua Pipe

Thư mục này (`BT_Tuan3`) chứa các tệp mã nguồn C giải bài 1 và bài 2 về giao tiếp qua ống (Pipe) và ống có tên (Named Pipe/FIFO).

## Cấu trúc dữ liệu cho Bài 2
Trong Bài 2, để truyền tải đồng thời 2 số nguyên và 1 phép tính (cộng, trừ, nhân, chia) giữa tiến trình cha và tiến trình con (hoặc writer và reader), tôi đã định nghĩa một `struct` như sau:
```c
typedef struct {
    int a;
    int b;
    char op;
} CalculationData;
```
Việc này giúp đóng gói dữ liệu và truyền đi trong một lệnh `write()`, thay vì phải truyền lắt nhắt nhiều biến, đảm bảo tính đồng bộ của pipe. Tiến trình nhận chỉ cần dùng `read(fd, &recv_data, sizeof(CalculationData))` để lấy đầy đủ toàn bộ đối số.

---

## Hướng Dẫn Biên Dịch và Chạy

Để tạo file thực thi, sử dụng 2 bước biên dịch cho từng file theo yêu cầu:
1. `gcc -c <file_code.c>` : Biên dịch mã nguồn thành file object (`.o`)
2. `gcc -o <file_code.out> <file_code.o>` : Liên kết file object thành file thực thi

---
### Cách Biên Dịch Hàng Loạt Nhanh Nhất (Dành Cho Linux/Ubuntu)
Nếu bạn không muốn gõ lệnh cho từng file, bạn có thể chạy 2 dòng lệnh sau trực tiếp trên Terminal để biên dịch và tạo file thực thi `.out` cho **toàn bộ** các file `.c` cùng lúc:

1. **Lệnh biên dịch toàn bộ ra `.o`:**
   ```bash
   for file in *.c; do gcc -c "$file"; done
   ```
2. **Lệnh tạo file thực thi `.out` toàn bộ từ `.o`:**
   ```bash
   for file in *.o; do gcc -o "${file%.o}.out" "$file"; done
   ```
---

### BÀI 1: TÍNH GIAI THỪA N!

#### Cách 1: Unnamed Pipe (1 file)
Sử dụng `pipe()` và `fork()` để tạo đường ống nội bộ.
- **Biên dịch:**
  ```bash
  gcc -c bai1_unnamed.c
  gcc -o bai1_unnamed.out bai1_unnamed.o
  ```
- **Chạy lệnh:** Truyền `n` là đối số từ dòng lệnh (n > 3).
  ```bash
  ./bai1_unnamed.out 4
  ```

#### Cách 2: Named Pipe (2 file độc lập)
Sử dụng `mkfifo()` và 2 file hoàn toàn độc lập chạy trên 2 terminal khác nhau.
- **Biên dịch:**
  ```bash
  gcc -c bai1_named_reader.c
  gcc -o bai1_named_reader.out bai1_named_reader.o
  
  gcc -c bai1_named_writer.c
  gcc -o bai1_named_writer.out bai1_named_writer.o
  ```
- **Chạy lệnh:**
  - Mở Terminal 1 (chạy reader trước để đợi dữ liệu):
    ```bash
    ./bai1_named_reader.out
    ```
  - Mở Terminal 2 (chạy writer gửi dữ liệu):
    ```bash
    ./bai1_named_writer.out 5
    ```

---

### BÀI 2: MÁY TÍNH ĐƠN GIẢN

*Lưu ý:* Phép nhân nên truyền kí tự `x` thay vì `*` ở dòng lệnh (vì dấu `*` trong Linux bị Shell nhận nhầm là danh sách toàn bộ các file trong thư mục hiện tại).

#### Cách 1: Unnamed Pipe (1 file)
- **Biên dịch:**
  ```bash
  gcc -c bai2_unnamed.c
  gcc -o bai2_unnamed.out bai2_unnamed.o
  ```
- **Chạy lệnh:** Truyền `a`, `b` và phép toán từ dòng lệnh.
  ```bash
  ./bai2_unnamed.out 4 6 +
  ./bai2_unnamed.out 10 5 /
  ```
  *Kết quả sẽ được ghi vào file `ketqua.txt` trong cùng thư mục.* Cần mở file đó để xem: `cat ketqua.txt`

#### Cách 2: Named Pipe (2 file độc lập)
- **Biên dịch:**
  ```bash
  gcc -c bai2_named_reader.c
  gcc -o bai2_named_reader.out bai2_named_reader.o
  
  gcc -c bai2_named_writer.c
  gcc -o bai2_named_writer.out bai2_named_writer.o
  ```
- **Chạy lệnh:**
  - Mở Terminal 1 (chạy reader trước):
    ```bash
    ./bai2_named_reader.out
    ```
  - Mở Terminal 2 (chạy writer truyền số và phép toán):
    ```bash
    ./bai2_named_writer.out 4 6 +
    ```
  *Tiến trình writer sẽ nhận kết quả và ghi vào file `ketqua.txt`.*
