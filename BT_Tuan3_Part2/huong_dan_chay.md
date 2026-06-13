# Hướng Dẫn Biên Dịch và Thực Thi - Bài Tập Message Queue (Tuần 3 - Part 2)

Tài liệu này hướng dẫn cách biên dịch và chạy 3 bài tập liên lạc giữa các tiến trình thông qua cơ chế **System V Message Queue** trên hệ điều hành Linux/Ubuntu.

## Yêu cầu chung
- Đảm bảo bạn đang mở terminal tại đúng thư mục `BT_Tuan3_Part2`.
- Mỗi bài tập yêu cầu chạy 2 tiến trình độc lập. Do đó, bạn cần mở **2 cửa sổ Terminal** (hoặc 2 tab Terminal) riêng biệt.

---

## BÀI 1: Tính Giai Thừa n!
Hệ thống gồm Server lắng nghe yêu cầu và Client gửi số nguyên `n` (n > 3) để tính n!.

### 1. Biên dịch
```bash
gcc bai1_server.c -o bai1_server
gcc bai1_client.c -o bai1_client
```

### 2. Cách chạy
- **Terminal 1 (Chạy Server trước để lắng nghe):**
  ```bash
  ./bai1_server
  ```
- **Terminal 2 (Chạy Client với tham số n):**
  ```bash
  ./bai1_client 5
  ```
  *(Thay `5` bằng số nguyên bất kỳ lớn hơn 3)*

---

## BÀI 2: Máy Tính Số Học
Hệ thống nhận 2 số nguyên và 1 phép toán (`+`, `-`, `x`, `/`) từ Client, Server sẽ tính toán và trả về kết quả. Đã xử lý lỗi chia cho 0.

### 1. Biên dịch
```bash
gcc bai2_server.c -o bai2_server
gcc bai2_client.c -o bai2_client
```

### 2. Cách chạy
- **Terminal 1 (Chạy Server trước):**
  ```bash
  ./bai2_server
  ```
- **Terminal 2 (Chạy Client với tham số là 2 số và phép toán):**
  ```bash
  ./bai2_client 4 6 +
  ./bai2_client 10 2 /
  ./bai2_client 5 3 x
  ```
  *Lưu ý:* Phép nhân nên sử dụng chữ `x`. Nếu sử dụng dấu `*`, bạn cần bọc trong dấu nháy kép `'*'` (Ví dụ: `./bai2_client 5 3 '*'`) để bash shell không hiểu nhầm là ký tự đại diện (wildcard).

---

## BÀI 3: Sắp Xếp Dãy Số Đọc Từ File
Tiến trình `P0` đọc dữ liệu từ file `data`, gửi từng dãy số qua Message Queue cho `P1`. Tiến trình `P1` sắp xếp và tính tổng rồi gửi trả lại. Sau cùng, `P0` sắp xếp các dãy theo tổng và ghi ra file `result`.

### 1. Chuẩn bị dữ liệu
Tạo một file văn bản tên là `data` (không có phần mở rộng .txt) cùng cấp với file code, chứa các số nguyên. Mỗi dòng là một dãy số. Ví dụ:
```text
5 1 4 2 8
9 3 6
10 5 12 1 0 7
```

### 2. Biên dịch
```bash
gcc bai3_p1.c -o bai3_p1
gcc bai3_p0.c -o bai3_p0
```

### 3. Cách chạy
- **Terminal 1 (Chạy P1 trước để chờ dữ liệu):**
  ```bash
  ./bai3_p1
  ```
- **Terminal 2 (Chạy P0 để đọc file và điều phối):**
  ```bash
  ./bai3_p0
  ```

### 4. Kiểm tra kết quả
Sau khi `P0` thông báo hoàn tất, bạn mở file `result` mới được tạo ra trong thư mục để xem kết quả. Message Queue dùng chung cũng sẽ được tự động giải phóng (xoá) bởi `P0` khi kết thúc chương trình.
