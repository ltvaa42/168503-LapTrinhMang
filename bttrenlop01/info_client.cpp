#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8888);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 3);
    
    std::cout << "Server b1.1 dang cho ket noi...\n";
    int client_sock = accept(server_fd, NULL, NULL);
    char buffer[4096];
    int bytes = recv(client_sock, buffer, sizeof(buffer), 0);

    char* ptr = buffer;
    // 1. Lay ten thu muc
    std::string folder = ptr;
    ptr += folder.length() + 1;
    std::cout << "Folder: " << folder << std::endl;

    // 2. Lay danh sach file
    while (ptr < buffer + bytes) {
        std::string fname = ptr;
        ptr += fname.length() + 1;
        long fsize;
        memcpy(&fsize, ptr, sizeof(long));
        ptr += sizeof(long);
        std::cout << fname << " - " << fsize << " bytes" << std::endl;
    }
    close(client_sock); close(server_fd);
    return 0;
}