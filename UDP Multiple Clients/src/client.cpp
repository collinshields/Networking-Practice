#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "udppractice.h"

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
    serverAddr.sin_port = htons(SERVER_PORT);
    InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Non-blocking
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    // ---- Send JOIN ----
    JoinPacket join{};
    join.type = PACKET_JOIN;

    sendto(sock,
        reinterpret_cast<const char*>(&join),
        static_cast<int>(sizeof(join)),
        0,
        reinterpret_cast<sockaddr*>(&serverAddr),
        static_cast<int>(sizeof(serverAddr)));

    uint32_t myId = 0;

    char buffer[BUFFER_SIZE];

    while (true)
    {
        sockaddr_in from{};
        int fromLen = static_cast<int>(sizeof(from));

        int bytes = recvfrom(sock,
            buffer,
            BUFFER_SIZE,
            0,
            reinterpret_cast<sockaddr*>(&from),
            &fromLen);
        if (bytes == SOCKET_ERROR)
            bytes = 0;

        if (bytes > 0)
        {
            PacketType type = *reinterpret_cast<PacketType*>(buffer);

            if (type == PACKET_WELCOME)
            {
                WelcomePacket* welcome = reinterpret_cast<WelcomePacket*>(buffer);
                myId = welcome->clientId;
                std::cout << "Connected with ID: " << myId << "\n";
            }

            if (type == PACKET_MESSAGE)
            {
                MessagePacket* msg = reinterpret_cast<MessagePacket*>(buffer);
                std::cout << "Broadcast: " << msg->message << "\n";
            }
        }

        // Send test message once connected
        if (myId != 0)
        {
            MessagePacket msg{};
            msg.type = PACKET_MESSAGE;
            msg.clientId = myId;
            // Use secure CRT function on MSVC
            strcpy_s(msg.message, sizeof(msg.message), "Hello from client");

            sendto(sock,
                reinterpret_cast<const char*>(&msg),
                static_cast<int>(sizeof(msg)),
                0,
                reinterpret_cast<sockaddr*>(&serverAddr),
                static_cast<int>(sizeof(serverAddr)));

            Sleep(2000);
        }

        // prevent CPU spin
        Sleep(1);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}