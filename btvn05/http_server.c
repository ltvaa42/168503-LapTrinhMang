#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void handle_http(int client) {
    char buf[1024];
    recv(client, buf, sizeof(buf), 0);
    printf("Request received by process %d\n", getpid());

    char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</h1></body></html>";
    send(client, msg, strlen(msg), 0);
    close(client);
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr)); 
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 10);

    int num_children = 5; 
    for (int i = 0; i < num_children; i++) {
        if (fork() == 0) {
            while (1) {
                int client = accept(listener, NULL, NULL);
                if (client < 0) continue;
                handle_http(client);
            }
        }
    }

    while(1) sleep(10); 
    return 0;
}