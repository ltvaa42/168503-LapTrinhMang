#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

string generate_hust_email(string fullname, string mssv) {
    mssv.erase(0, mssv.find_first_not_of(" "));
    mssv.erase(mssv.find_last_not_of(" ") + 1);

    stringstream ss(fullname);
    string word;
    vector<string> parts;
    
    while (ss >> word) {
        parts.push_back(word);
    }
    
    if (parts.empty() || mssv.length() < 3) return "error@sis.hust.edu.vn";
    
    string email = parts.back() + ".";
    for (size_t i = 0; i < parts.size() - 1; ++i) {
        email += parts[i][0];
    }
    
    string short_mssv = mssv.substr(2);
    email += short_mssv + "@sis.hust.edu.vn";
    
    return email;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8888);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);
    
    cout << "Server (Non-blocking) dang chay tai port 8888..." << endl;

    fd_set readfds;
    vector<int> client_sockets;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int max_sd = server_fd;

        for (int sd : client_sockets) {
            FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds)) {
            int new_socket = accept(server_fd, NULL, NULL);
            if (new_socket >= 0) {
                fcntl(new_socket, F_SETFL, O_NONBLOCK);
                client_sockets.push_back(new_socket);
                string msg = "Nhap 'Ho ten|MSSV': ";
                send(new_socket, msg.c_str(), msg.length(), 0);
            }
        }

        for (auto it = client_sockets.begin(); it != client_sockets.end(); ) {
            int sd = *it;
            if (FD_ISSET(sd, &readfds)) {
                char buffer[1024] = {0};
                int valread = read(sd, buffer, 1024);
                
                if (valread <= 0) {
                    close(sd);
                    it = client_sockets.erase(it);
                    continue;
                } else {
                    string input(buffer);
                    size_t pos = input.find('|');
                    if (pos != string::npos) {
                        string name = input.substr(0, pos);
                        string mssv = input.substr(pos + 1);
                        
                        mssv.erase(remove(mssv.begin(), mssv.end(), '\n'), mssv.end());
                        mssv.erase(remove(mssv.begin(), mssv.end(), '\r'), mssv.end());

                        string email = generate_hust_email(name, mssv);
                        string response = "Email: " + email + "\n";
                        send(sd, response.c_str(), response.length(), 0);
                    }
                }
            }
            ++it;
        }
    }
    return 0;
}