#include "../src/types.h"


int main(int argc, char *argv[]) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    char broadcast = '1';

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        fprintf(stderr, "No broadcast");
        return 0;
    }

    motd_t motd;

    gethostname(motd.hostname, sizeof(motd.hostname));
    struct addrinfo hints, *result;
    hints.ai_family = AF_INET;
    getaddrinfo(motd.hostname, NULL, &hints, &result);

    motd.ipaddr = (((struct sockaddr_in *) result->ai_addr)->sin_addr.S_un.S_addr);
    wcscpy(motd.motd_body, L"Telthar zażółć gęślą jaźń");


    struct sockaddr_in Recv_addr;
    Recv_addr.sin_family       = AF_INET;
    Recv_addr.sin_port         = htons(2005);   

    Recv_addr.sin_addr.s_addr = INADDR_BROADCAST;

    while (1) {
        sendto(sock, (char*)&motd, sizeof(motd_t), 0, (struct sockaddr *)&Recv_addr, sizeof(Recv_addr));
        Sleep(10 * 1000);
    }

}