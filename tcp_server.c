#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Sử dụng: %s <Cổng> <File chào> <File lưu>\n", argv[0]);
        return 1;
    }

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    printf("Đang đợi client kết nối tại cổng %s...\n", argv[1]);
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

    FILE *f_hello = fopen(argv[2], "r");
    if (f_hello) {
        char hello_msg[256];
        fgets(hello_msg, sizeof(hello_msg), f_hello);
        send(client, hello_msg, strlen(hello_msg), 0);
        fclose(f_hello);
    }

    FILE *f_log = fopen(argv[3], "w");
    char buf[256];
    while (1) {
        int len = recv(client, buf, sizeof(buf) - 1, 0);
        if (len <= 0) break;
        buf[len] = 0;
        fprintf(f_log, "%s", buf);
        fflush(f_log);
    }

    fclose(f_log);
    close(client);
    close(listener);
    return 0;
}