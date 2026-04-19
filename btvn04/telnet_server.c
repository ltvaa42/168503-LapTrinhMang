#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define MAX_CLIENTS 64

typedef struct {
    int fd;
    int authenticated;
} client_t;

int check_login(char *user, char *pass) {
    FILE *f = fopen("users.txt", "r");
    if (!f) return 0;
    char u[32], p[32];
    while (fscanf(f, "%s %s", u, p) != EOF) {
        if (strcmp(user, u) == 0 && strcmp(pass, p) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9001);

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
                char *msg = "Nhap user pass: ";
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
                    nfds--; i--; continue;
                }

                buf[n] = 0;
                if (buf[n-1] == '\n') buf[n-1] = 0;
                if (buf[n-2] == '\r') buf[n-2] = 0;

                if (!clients[i].authenticated) {
                    char user[32], pass[32];
                    if (sscanf(buf, "%s %s", user, pass) == 2 && check_login(user, pass)) {
                        clients[i].authenticated = 1;
                        char *msg = "Dang nhap thanh cong. Nhap lenh: ";
                        send(fds[i].fd, msg, strlen(msg), 0);
                    } else {
                        char *msg = "Sai tai khoan. Nhap lai: ";
                        send(fds[i].fd, msg, strlen(msg), 0);
                    }
                } else {
                    char cmd[512];
                    sprintf(cmd, "%s > out.txt 2>&1", buf);
                    system(cmd);

                    FILE *f = fopen("out.txt", "r");
                    char res[1024];
                    while (fgets(res, sizeof(res), f)) {
                        send(fds[i].fd, res, strlen(res), 0);
                    }
                    fclose(f);
                    char *prompt = "\nNhap lenh tiep theo: ";
                    send(fds[i].fd, prompt, strlen(prompt), 0);
                }
            }
        }
    }
    return 0;
}