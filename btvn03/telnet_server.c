#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9999
#define MAX_CLIENTS 10
#define BUF_SIZE 2048

typedef struct {
    int fd;
    int logged_in;
} TelnetClient;

int check_auth(char *input) {
    char user[50], pass[50], fuser[50], fpass[50];
    if (sscanf(input, "%s %s", user, pass) < 2) return 0;
    
    FILE *f = fopen("database.txt", "r");
    if (!f) return 0;
    while (fscanf(f, "%s %s", fuser, fpass) != EOF) {
        if (strcmp(user, fuser) == 0 && strcmp(pass, fpass) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main() {
    int server_fd, max_fd;
    struct sockaddr_in addr;
    fd_set readfds;
    TelnetClient clients[MAX_CLIENTS] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > max_fd) max_fd = clients[i].fd;
            }
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds)) {
            int new_sock = accept(server_fd, NULL, NULL);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd == 0) {
                    clients[i].fd = new_sock;
                    clients[i].logged_in = 0;
                    send(new_sock, "Nhap user pass: ", 16, 0);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0 && FD_ISSET(clients[i].fd, &readfds)) {
                char buffer[BUF_SIZE] = {0};
                int n = recv(clients[i].fd, buffer, BUF_SIZE, 0);
                if (n <= 0) {
                    close(clients[i].fd);
                    clients[i].fd = 0;
                    continue;
                }
                buffer[strcspn(buffer, "\r\n")] = 0;

                if (!clients[i].logged_in) {
                    if (check_auth(buffer)) {
                        clients[i].logged_in = 1;
                        send(clients[i].fd, "Dang nhap thanh cong! Nhap lenh: ", 33, 0);
                    } else {
                        send(clients[i].fd, "Loi dang nhap! Nhap lai: ", 25, 0);
                    }
                } else {
                    char cmd[BUF_SIZE + 20];
                    sprintf(cmd, "%s > out.txt 2>&1", buffer);
                    system(cmd);

                    FILE *f = fopen("out.txt", "r");
                    if (f) {
                        char res[BUF_SIZE];
                        while (fgets(res, BUF_SIZE, f)) {
                            send(clients[i].fd, res, strlen(res), 0);
                        }
                        fclose(f);
                    }
                    send(clients[i].fd, "\nNhap lenh tiep theo: ", 22, 0);
                }
            }
        }
    }
    return 0;
}