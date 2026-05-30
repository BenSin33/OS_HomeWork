#include <stdio.h>
#include <stdlib.h>

int main() {
    int re;
    
    printf("Tao thu muc BaiTap tai Desktop...\n");
    re = system("mkdir -p ~/Desktop/BaiTap");
    if (re != 0) {
        printf("Loi tao thu muc BaiTap!\n");
        return 1;
    }

    printf("Tao thu muc LyThuyet va ThucHanh trong BaiTap...\n");
    re = system("mkdir -p ~/Desktop/BaiTap/LyThuyet ~/Desktop/BaiTap/ThucHanh");
    if (re != 0) {
        printf("Loi tao thu muc LyThuyet va ThucHanh!\n");
        return 1;
    }

    printf("Tao file ranh rong 'test' trong ThucHanh...\n");
    re = system("touch ~/Desktop/BaiTap/ThucHanh/test");
    if (re != 0) {
        printf("Loi tao file test!\n");
        return 1;
    }

    printf("Da hoan thanh thanh cong viec tao thu muc va file!\n");

    return 0;
}
