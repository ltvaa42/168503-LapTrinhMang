#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#define MAX_CLIENTS 64

typedef struct {
    int fd;
    char id[32];
    char name[32];
    int authenticated;
} client_t;

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    struct pollfd fds[MAX_CLIENTS];
    client_t clients[MAX_CLIENTS];
    
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    int nfds = 1;

    while (1) {
        int ret = poll(fds, nfds, -1);
        if (ret < 0) break;

        if (fds[0].revents & POLLIN) {
            int client = accept(listener, NULL, NULL);
            if (nfds < MAX_CLIENTS) {
                fds[nfds].fd = client;
                fds[nfds].events = POLLIN;
                clients[nfds].fd = client;
                clients[nfds].authenticated = 0;
                nfds++;
                char *msg = "Hay gui id: name de bat dau\n";
                send(client, msg, strlen(msg), 0);
            } else {
                close(client);
            }
        }

        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                char buf[256];
                int n = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);
                if (n <= 0) {
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    clients[i] = clients[nfds - 1];
                    nfds--;
                    i--;
                    continue;
                }

                buf[n] = 0;
                if (buf[n-1] == '\n') buf[n-1] = 0;
                if (buf[n-2] == '\r') buf[n-2] = 0;

                if (!clients[i].authenticated) {
                    char id[32], name[32];
                    if (sscanf(buf, "%[^:]: %s", id, name) == 2) {
                        strcpy(clients[i].id, id);
                        strcpy(clients[i].name, name);
                        clients[i].authenticated = 1;
                        char *msg = "Dang nhap thanh cong\n";
                        send(fds[i].fd, msg, strlen(msg), 0);
                    } else {
                        char *msg = "Sai cu phap. Gui lai id: name\n";
                        send(fds[i].fd, msg, strlen(msg), 0);
                    }
                } else {
                    time_t rawtime;
                    struct tm *timeinfo;
                    char time_buf[32];
                    time(&rawtime);
                    timeinfo = localtime(&rawtime);
                    strftime(time_buf, sizeof(time_buf), "%Y/%m/%d %I:%M:%S%p", timeinfo);

                    char send_buf[512];
                    sprintf(send_buf, "%s %s: %s\n", time_buf, clients[i].id, buf);

                    for (int j = 1; j < nfds; j++) {
                        if (j != i && clients[j].authenticated) {
                            send(fds[j].fd, send_buf, strlen(send_buf), 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}