# Hướng Dẫn Bài Tập: POSIX Semaphore (Bài 11)

Thư mục này (`BT_Tuan4_Part 2`) chứa các tệp mã nguồn C giải quyết 3 bài tập về đồng bộ hóa đa luồng sử dụng Semaphore trên môi trường POSIX (Linux/Ubuntu).

## Kiến thức cốt lõi
- Tất cả các bài tập sử dụng thư viện `<pthread.h>` và `<semaphore.h>`.
- Quá trình biên dịch bắt buộc phải liên kết với thư viện pthread (`-lpthread`). Riêng bài 2 cần liên kết thêm thư viện toán học (`-lm`).

---

## Hướng Dẫn Biên Dịch và Chạy

Để tạo file thực thi, bạn có thể biên dịch thành file object (`.o`) trước, sau đó liên kết thành file thực thi (`.out`). 

### BÀI 1: Đồng bộ hóa 2 luồng in dãy số tuần tự
Chương trình tạo 2 luồng in số chẵn và số lẻ xen kẽ nhau từ 1 đến 11.
- **Biên dịch:**
  ```bash
  gcc -c bai1.c
  gcc -o bai1.out bai1.o -lpthread
  ```
- **Chạy lệnh:**
  ```bash
  ./bai1.out
  ```

---

### BÀI 2: Ước tính số Pi bằng thuật toán Monte Carlo
Chương trình tính xấp xỉ số Pi thông qua việc tạo ra các điểm ngẫu nhiên trên hệ trục tọa độ đa luồng.
- **Biên dịch:**
  ```bash
  gcc -c bai2.c
  # Lưu ý: Cần liên kết thêm -lm cho thư viện math.h
  gcc -o bai2.out bai2.o -lpthread -lm
  ```
- **Chạy lệnh:** Truyền `niter` (số lượng điểm ngẫu nhiên) là đối số từ dòng lệnh. Số càng lớn thì ước tính số Pi càng chính xác.
  ```bash
  ./bai2.out 10000000
  ```

---

### BÀI 3: Đồng bộ dây chuyền sản xuất xe ô tô
Chương trình mô phỏng dây chuyền sản xuất với 3 bộ phận: Khung, Bánh xe và Lắp ráp. Semaphore giúp đảm bảo tỷ lệ 1 khung - 4 bánh trước khi tiến hành lắp ráp chiếc xe hoàn chỉnh.
- **Biên dịch:**
  ```bash
  gcc -c bai3.c
  gcc -o bai3.out bai3.o -lpthread
  ```
- **Chạy lệnh:**
  ```bash
  ./bai3.out
  ```

---

### Cách Biên Dịch Hàng Loạt Nhanh Nhất (Dành Cho Linux/Ubuntu)
Nếu bạn muốn biên dịch toàn bộ thư mục một lần mà không cần gõ từng lệnh, hãy chạy các dòng sau (đã tích hợp tự động các thư viện cần thiết):

1. **Lệnh biên dịch ra các file `.o`:**
   ```bash
   for file in *.c; do gcc -c "$file"; done
   ```
2. **Lệnh tạo file thực thi `.out` (có kèm -lpthread và -lm):**
   ```bash
   for file in *.o; do gcc -o "${file%.o}.out" "$file" -lpthread -lm; done
   ```
