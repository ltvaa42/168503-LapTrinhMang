#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int check_login(char *user, char *pass) {
    FILE *f = fopen("databases.txt", "r");
    if (!f) return 0;
    char u[32], p[32];
    while (fscanf(f, "%s %s", u, p) != EOF) {
        if (strcmp(user, u) == 0 && strcmp(pass, p) == 0) {
            fclose(f); return 1;
        }
    }
    fclose(f); return 0;
}

void handle_telnet(int client) {
    char buf[256], user[32], pass[32];
    send(client, "Username Password: ", 19, 0);
    int ret = recv(client, buf, sizeof(buf) - 1, 0);
    buf[ret] = 0;
    sscanf(buf, "%s %s", user, pass);

    if (check_login(user, pass)) {
        send(client, "Dang nhap thanh cong!\n", 22, 0);
        while (1) {
            ret = recv(client, buf, sizeof(buf) - 1, 0);
            if (ret <= 0) break;
            buf[ret - 1] = 0; 
            char command[300];
            sprintf(command, "%s > out.txt", buf);
            system(command);

            FILE *f = fopen("out.txt", "r");
            while (fgets(buf, sizeof(buf), f)) send(client, buf, strlen(buf), 0);
            fclose(f);
        }
    } else {
        send(client, "Sai tai khoan!\n", 15, 0);
    }
    close(client);
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(9999), .sin_addr.s_addr = INADDR_ANY };
    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    while (1) {
        int client = accept(listener, NULL, NULL);
        if (fork() == 0) {
            close(listener);
            handle_telnet(client);
            exit(0);
        }
        close(client);
    }
}