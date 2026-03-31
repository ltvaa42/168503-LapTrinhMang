#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <port_s> <ip_d> <port_d>" << endl;
        return 1;
    }

    int port_s = stoi(argv[1]);
    string ip_d = argv[2];
    int port_d = stoi(argv[3]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port_s);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    struct sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(port_d);
    inet_pton(AF_INET, ip_d.c_str(), &destaddr.sin_addr);

    fd_set readfds;
    char buffer[1024];

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            break;
        }

        if (FD_ISSET(sockfd, &readfds)) {
            struct sockaddr_in src_addr;
            socklen_t addr_len = sizeof(src_addr);
            int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&src_addr, &addr_len);
            if (n > 0) {
                buffer[n] = '\0';
                cout << "Received: " << buffer << endl;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            string msg;
            if (!getline(cin, msg)) break;
            sendto(sockfd, msg.c_str(), msg.length(), 0, (const struct sockaddr *)&destaddr, sizeof(destaddr));
        }
    }

    close(sockfd);
    return 0;
}