#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <cstdio>
#include <thread>

int main(int argc, char *argv[])
{
    int code;
    if (argc != 2)
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

    auto server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    if (listen(server, SOMAXCONN) == SOCKET_ERROR)
    {
        code = WSAGetLastError();
        printf("Failed to start listen, code: %d.\n", code);
        return code;
    }

    printf("Server started listening at %d.\n", port);

    while (true)
    {
        sockaddr_in client_addr;
        int len = sizeof(client_addr);
        auto client = accept(
            server,
            reinterpret_cast<sockaddr *>(&client_addr),
            &len);
        if (client == INVALID_SOCKET)
        {
            code = WSAGetLastError();
            printf("Failed to accept client, code: %d.\n", code);
            continue;
        }

        printf("Client accepted: %s\n", inet_ntoa(client_addr.sin_addr));
        auto t = std::thread([client, client_addr]() {
            char buffer[1024];
            int len;
            do
            {
                len = recv(client, buffer, 1023, 0);
                if (len > 0)
                {
                    buffer[len] = 0;
                    printf("Message received from client %s: %s\n", inet_ntoa(client_addr.sin_addr), buffer);
                    send(client, buffer, static_cast<int>(strlen(buffer)), 0);
                }
                if (strcmp(buffer, "bye") == 0)
                    break;
            } while (len > 0);
            closesocket(client);
            printf("Connection closed: %s\n", inet_ntoa(client_addr.sin_addr));
        });
        t.detach();
    }

    return 0;
}