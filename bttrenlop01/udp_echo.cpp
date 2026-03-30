#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(7777);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) return 1;

    char buffer[1024];
    socklen_t len = sizeof(cliaddr);

    std::cout << "UDP Echo Server is running..." << std::endl;

    while (true) {

        int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        sendto(sockfd, buffer, n, 0, (const struct sockaddr *)&cliaddr, len);
    }
    return 0;
}