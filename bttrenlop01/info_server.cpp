#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int count_occurrences(const std::string& str, const std::string& target) {
    int count = 0;
    size_t pos = str.find(target, 0);
    while (pos != std::string::npos) {
        count++;
        pos = str.find(target, pos + 1);
    }
    return count;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));    
    addr.sin_family = AF_INET;           
    addr.sin_addr.s_addr = INADDR_ANY;   
    addr.sin_port = htons(9999);         
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 3);

    int client_socket = accept(server_fd, NULL, NULL);
    char buffer[1024];
    std::string leftover = ""; 
    std::string target = "0123456789";
    int total_count = 0;

    while (true) {
        int bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;

        std::string current_data(buffer, bytes);
        std::string full_data = leftover + current_data;

        total_count += count_occurrences(full_data, target);
        std::cout << "Số lần xuất hiện hiện tại: " << total_count << std::endl;

        if (full_data.length() >= target.length()) {
            leftover = full_data.substr(full_data.length() - (target.length() - 1));
        } else {
            leftover = full_data;
        }
    }
    close(client_socket);
    return 0;
}