#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

struct SinhVien {
    char mssv[12];
    char hoten[50];
    char ngaysinh[12];
    float diem;
};
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Sử dụng: %s <IP> <Cổng>\n", argv[0]);
        return 1;
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    connect(client, (struct sockaddr *)&addr, sizeof(addr));

    struct SinhVien sv;
    printf("MSSV: "); scanf("%s", sv.mssv);
    getchar(); // Xóa bộ nhớ đệm
    printf("Họ tên: "); fgets(sv.hoten, sizeof(sv.hoten), stdin);
    sv.hoten[strcspn(sv.hoten, "\n")] = 0; // Xóa ký tự xuống dòng
    printf("Ngày sinh (YYYY-MM-DD): "); scanf("%s", sv.ngaysinh);
    printf("Điểm trung bình: "); scanf("%f", &sv.diem);

    send(client, &sv, sizeof(sv), 0); 

    close(client);
    return 0;
}