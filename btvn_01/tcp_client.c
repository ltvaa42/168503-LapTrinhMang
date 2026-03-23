#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Sử dụng: %s <Địa chỉ IP> <Cổng>\n", argv[0]);
        return 1;
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Kết nối thất bại");
        return 1;
    }

    char buf[256];
    printf("Nhập dữ liệu gửi đến server (gõ 'exit' để thoát):\n");
    while (1) {
        fgets(buf, sizeof(buf), stdin);
        send(client, buf, strlen(buf), 0);
        if (strncmp(buf, "exit", 4) == 0) break;
    }

    close(client);
    return 0;
}