#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    int code;
    if (argc < 3)
    {
        printf("Usage: client.exe ip port\n");
        return 0;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        code = WSAGetLastError();
        printf("Failed to initialize, code: %d.\n", code);
        return code;
    }

    auto client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client == INVALID_SOCKET)
    {
        code = WSAGetLastError();
        printf("Failed to create socket, code: %d.\n", code);
        return code;
    }

    sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(argc > 1 ? atoi(argv[2]) : 23333);
    socket_addr.sin_addr.S_un.S_addr = inet_addr(argv[1]);

    int cnt = 0;
    auto str = "Hello, this is a message %d from client.";
    char buffer[1024];
    int len = sizeof(socket_addr);
    while (cnt++ < 100)
    {
        sprintf(buffer, str, cnt);
        sendto(client, buffer, static_cast<int>(strlen(buffer)), 0, reinterpret_cast<sockaddr *>(&socket_addr), len);
        printf("Sent %d message.\n", cnt);
    }

    closesocket(client);
    WSACleanup();

    return 0;
}