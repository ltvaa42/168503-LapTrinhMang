#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

struct SinhVien {
    char mssv[12];
    char hoten[50];
    char ngaysinh[12];
    float diem;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Sử dụng: %s <Cổng>\n", argv[0]);
        return 1;
    }

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

        struct SinhVien sv;
        int ret = recv(client, &sv, sizeof(sv), 0);
        if (ret > 0) {

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm);

            char *ip = inet_ntoa(client_addr.sin_addr);

            printf("%s %s %s %s %s %.2f\n", ip, time_str, sv.mssv, sv.hoten, sv.ngaysinh, sv.diem);
            
            FILE *f = fopen("sv_log.txt", "a");
            fprintf(f, "%s %s %s %s %s %.2f\n", ip, time_str, sv.mssv, sv.hoten, sv.ngaysinh, sv.diem);
            fclose(f);
        }
        close(client);
    }

    close(listener);
    return 0;
}