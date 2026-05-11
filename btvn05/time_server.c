#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void process_client(int client_sock) {
    char buf[256];
    while (1) {
        int ret = recv(client_sock, buf, sizeof(buf) - 1, 0);
        if (ret <= 0) break;
        buf[ret] = 0;

        char cmd[16], format[32];
        int n = sscanf(buf, "%s %s", cmd, format);

        if (n == 2 && strcmp(cmd, "GET_TIME") == 0) {
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            char res[64];

            if (strcmp(format, "dd/mm/yyyy") == 0) strftime(res, sizeof(res), "%d/%m/%Y\n", tm_info);
            else if (strcmp(format, "dd/mm/yy") == 0) strftime(res, sizeof(res), "%d/%m/%y\n", tm_info);
            else if (strcmp(format, "mm/dd/yyyy") == 0) strftime(res, sizeof(res), "%m/%d/%Y\n", tm_info);
            else if (strcmp(format, "mm/dd/yy") == 0) strftime(res, sizeof(res), "%m/%d/%y\n", tm_info);
            else strcpy(res, "Dinh dang khong hop le\n");

            send(client_sock, res, strlen(res), 0);
        } else {
            char *msg = "Sai cu phap. Dung: GET_TIME [format]\n";
            send(client_sock, msg, strlen(msg), 0);
        }
    }
    close(client_sock);
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8888);
    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    while (1) {
        int client = accept(listener, NULL, NULL);
        if (fork() == 0) { 
            close(listener);
            process_client(client);
            exit(0);
        }
        close(client);
    }
    return 0;
}