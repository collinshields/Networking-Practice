#pragma once

#include <cstdint>

constexpr int SERVER_PORT = 40000;
constexpr int MAX_CLIENTS = 16;
constexpr int BUFFER_SIZE = 1024;

enum PacketType : uint8_t
{
    PACKET_JOIN = 0,
    PACKET_WELCOME = 1,
    PACKET_MESSAGE = 2
};

struct JoinPacket
{
    PacketType type;
};

struct WelcomePacket
{
    PacketType type;
    uint32_t clientId;
};

struct MessagePacket
{
    PacketType type;
    uint32_t clientId;
    char message[256];
};