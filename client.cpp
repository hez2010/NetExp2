#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    int code;
    if (argc != 3)
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

    auto client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
    if (connect(client, reinterpret_cast<sockaddr *>(&socket_addr), sizeof(socket_addr)) == SOCKET_ERROR)
    {
        code = WSAGetLastError();
        printf("Failed to connect to server, code: %d.\n", code);
        return code;
    }

    auto t = std::thread([client]() {
        char buffer[1024];
        int len;
        do
        {
            len = recv(client, buffer, 1023, 0);
            if (len > 0)
            {
                buffer[len] = 0;
                printf("Message received from server: %s\n", buffer);
            }
        } while (len > 0);
    });
    t.detach();

    while (true)
    {
        std::string str;
        std::getline(std::cin, str);
        if (str.size() <= 0)
            continue;
        send(client, str.c_str(), static_cast<int>(str.size()), 0);
    }

    return 0;
}