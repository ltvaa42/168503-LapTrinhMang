#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUF_SIZE 1024

typedef struct {
    int fd;
    char id[50];
    int authenticated;
} Client;

int main() {
    int server_fd, max_fd;
    struct sockaddr_in address;
    fd_set readfds;
    Client clients[MAX_CLIENTS] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) exit(1);
    listen(server_fd, 5);
    printf("Chat Server dang chay tren port %d...\n", PORT);

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
                    clients[i].authenticated = 0;
                    send(new_sock, "Hay nhap 'client_id: client_name': ", 36, 0);
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

                if (!clients[i].authenticated) {
                    char id[50], name[50];
                    if (sscanf(buffer, "%[^:]: %s", id, name) == 2) {
                        strcpy(clients[i].id, id);
                        clients[i].authenticated = 1;
                        send(clients[i].fd, "Xac thuc thanh cong!\n", 22, 0);
                    } else {
                        send(clients[i].fd, "Sai cu phap! Nhap lai: ", 23, 0);
                    }
                } else {
                    time_t now = time(0);
                    struct tm *t = localtime(&now);
                    char time_buf[25], send_buf[BUF_SIZE + 100];
                    strftime(time_buf, sizeof(time_buf), "%Y/%m/%d %H:%M:%S", t);
                    sprintf(send_buf, "%s %s: %s\n", time_buf, clients[i].id, buffer);

                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (clients[j].fd > 0 && i != j && clients[j].authenticated) {
                            send(clients[j].fd, send_buf, strlen(send_buf), 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}