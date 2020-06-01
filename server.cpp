#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <cstdio>
#include <thread>

int main(int argc, char *argv[])
{
    int code;
    if (argc < 2)
    {
        printf("Usage: server.exe port\n");
        return 0;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        code = WSAGetLastError();
        printf("Failed to initialize, code: %d.\n", code);
        return code;
    }

    auto server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server == INVALID_SOCKET)
    {
        code = WSAGetLastError();
        printf("Failed to create socket, code: %d.\n", code);
        return code;
    }

    int port = atoi(argv[1]);
    sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(port);
    socket_addr.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(server, reinterpret_cast<sockaddr *>(&socket_addr), sizeof(socket_addr)) == SOCKET_ERROR)
    {
        code = WSAGetLastError();
        printf("Failed to bind socket, code: %d.\n", code);
        return code;
    }

    printf("Server started, port: %d.\n", port);

    sockaddr_in from_addr;
    int len = sizeof(from_addr);
    int cnt = 0;
    char buffer[1024];

    while (true)
    {
        int recv_len = recvfrom(server, buffer, 1023, 0, reinterpret_cast<sockaddr *>(&from_addr), &len);
        if (recv_len <= 0)
            break;
        buffer[recv_len] = 0;
        printf("[Count = %d] Received from client %s: %s\n", ++cnt, inet_ntoa(from_addr.sin_addr), buffer);
    }

    closesocket(server);
    WSACleanup();
    
    printf("Server closed.\n");

    return 0;
}