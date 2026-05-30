#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Day la tien trinh bai 1.\n");
    printf("Dang chay vong lap vo han. Su dung Ctrl+Z de dung va kill %d de dong.\n", getpid());
    
    while(1) {
        // Vong lap vo tan
        sleep(1); 
    }
    
    return 0;
}
