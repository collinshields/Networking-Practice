#include <iostream>
#include <unordered_map>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "udppractice.h"

struct ClientInfo
{
    sockaddr_in address;
    uint32_t id;
};

bool addressesEqual(const sockaddr_in& a, const sockaddr_in& b)
{
    return a.sin_addr.s_addr == b.sin_addr.s_addr &&
        a.sin_port == b.sin_port;
}

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(sock, reinterpret_cast<sockaddr*>(&serverAddr), static_cast<int>(sizeof(serverAddr))) == SOCKET_ERROR)
    {
        std::cerr << "bind() failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Set non-blocking
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    std::cout << "UDP Server started on port " << SERVER_PORT << "\n";

    ClientInfo clients[MAX_CLIENTS];
    int clientCount = 0;
    uint32_t nextClientId = 1;

    char buffer[BUFFER_SIZE];

    while (true)
    {
        sockaddr_in clientAddr{};
        int addrLen = static_cast<int>(sizeof(clientAddr));

        int bytes = recvfrom(sock,
            buffer,
            BUFFER_SIZE,
            0,
            reinterpret_cast<sockaddr*>(&clientAddr),
            &addrLen);
        if (bytes == SOCKET_ERROR)
            bytes = 0;

        if (bytes > 0)
        {
            PacketType type = *reinterpret_cast<PacketType*>(buffer);

            // ---- JOIN HANDLING ----
            if (type == PACKET_JOIN)
            {
                if (clientCount >= MAX_CLIENTS)
                {
                    std::cout << "Server full\n";
                    continue;
                }

                // Check if already connected
                bool exists = false;
                for (int i = 0; i < clientCount; i++)
                {
                    if (addressesEqual(clients[i].address, clientAddr))
                    {
                        exists = true;
                        break;
                    }
                }

                if (!exists)
                {
                    clients[clientCount].address = clientAddr;
                    clients[clientCount].id = nextClientId++;

                    WelcomePacket welcome{};
                    welcome.type = PACKET_WELCOME;
                    welcome.clientId = clients[clientCount].id;

                    sendto(sock,
                        reinterpret_cast<const char*>(&welcome),
                        static_cast<int>(sizeof(welcome)),
                        0,
                        reinterpret_cast<sockaddr*>(&clientAddr),
                        static_cast<int>(sizeof(clientAddr)));

                    std::cout << "Client connected. ID: " << clients[clientCount].id << "\n";

                    clientCount++;
                }
            }

            // ---- MESSAGE HANDLING ----
            if (type == PACKET_MESSAGE)
            {
                MessagePacket* msg = reinterpret_cast<MessagePacket*>(buffer);

                std::cout << "Client " << msg->clientId << ": " << msg->message << "\n";

                // Echo to all connected clients
                for (int i = 0; i < clientCount; i++)
                {
                    sendto(sock,
                        reinterpret_cast<const char*>(msg),
                        static_cast<int>(sizeof(MessagePacket)),
                        0,
                        reinterpret_cast<sockaddr*>(&clients[i].address),
                        static_cast<int>(sizeof(sockaddr_in)));
                }
            }
        }

        // prevent CPU spinning
        Sleep(1);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}