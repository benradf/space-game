/// \file protocol.cpp
/// \brief Network protocol.
/// \author 
/// \date 3th April 2009
///
/// Copyright (c) 2009 . All rights reserved.
///


#include "protocol.hpp"
#include <core.hpp>
#include <memory.h>


#define htonq(x) x  // needs implementing
#define ntohq(x) x  // needs implementing

#define CHECK_WRITE_OFFSET(pkt, off, type) { \
    if ((off + sizeof(type) > pkt->data + pkt->dataLength) && \
            (enet_packet_resize(pkt, pkt->dataLength * 2) != 0)) \
        return; }


#define CHECK_READ_OFFSET(pkt, off, type) { \
    if (off + sizeof(type) > pkt->data + pkt->dataLength) \
        return; }


////////// net::ProtocolUser //////////

net::ProtocolUser::~ProtocolUser()
{

}

void net::ProtocolUser::handlePacket(ENetPacket* packet)
{
    enet_uint8* offset = packet->data;
    uint8_t typecode = *offset++;
    uint16_t len = 0;

    switch (typecode) {
    case 0x01: {
        if (offset + 0x08 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise KeyExchange: malformed packet");
            return;
        }
        uint64_t key = *reinterpret_cast<uint64_t*>(offset);
        offset += sizeof(uint64_t);
        handleKeyExchange(ntohq(key));
        } break;
    case 0x02: {
        if (offset + 0x12 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise Login: malformed packet");
            return;
        }
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x10 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise Login: malformed packet");
            return;
        }
        char* (username) = reinterpret_cast<char*>(offset - 1);
        memmove((username), offset, len);
        (username)[len] = '\0';
        offset += len;
        uint8_t* password = reinterpret_cast<uint8_t*>(offset);
        offset += sizeof(password);
        handleLogin((username), reinterpret_cast<uint8_t(&)[16]>(*password));
        } break;
    case 0x03: {
        if (offset + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise Disconnect: malformed packet");
            return;
        }
        handleDisconnect();
        } break;
    case 0x04: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise WhoIsPlayer: malformed packet");
            return;
        }
        uint32_t playerid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        handleWhoIsPlayer(ntohl(playerid));
        } break;
    case 0x05: {
        if (offset + 0x06 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PlayerInfo: malformed packet");
            return;
        }
        uint32_t playerid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PlayerInfo: malformed packet");
            return;
        }
        char* (username) = reinterpret_cast<char*>(offset - 1);
        memmove((username), offset, len);
        (username)[len] = '\0';
        offset += len;
        handlePlayerInfo(ntohl(playerid), (username));
        } break;
    case 0x06: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PlayerInput: malformed packet");
            return;
        }
        uint32_t flags = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        handlePlayerInput(ntohl(flags));
        } break;
    case 0x07: {
        if (offset + 0x06 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PrivateMsg: malformed packet");
            return;
        }
        uint32_t playerid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PrivateMsg: malformed packet");
            return;
        }
        char* (text) = reinterpret_cast<char*>(offset - 1);
        memmove((text), offset, len);
        (text)[len] = '\0';
        offset += len;
        handlePrivateMsg(ntohl(playerid), (text));
        } break;
    case 0x08: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise BroadcastMsg: malformed packet");
            return;
        }
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise BroadcastMsg: malformed packet");
            return;
        }
        char* (text) = reinterpret_cast<char*>(offset - 1);
        memmove((text), offset, len);
        (text)[len] = '\0';
        offset += len;
        handleBroadcastMsg((text));
        } break;
    case 0x09: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectEnter: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        handleObjectEnter(ntohl(objectid));
        } break;
    case 0x0a: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectLeave: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        handleObjectLeave(ntohl(objectid));
        } break;
    case 0x0b: {
        if (offset + 0x10 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectPos: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        float x = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float y = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float z = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        handleObjectPos(ntohl(objectid), (x), (y), (z));
        } break;
    case 0x0c: {
        if (offset + 0x10 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectVel: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        float x = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float y = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float z = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        handleObjectVel(ntohl(objectid), (x), (y), (z));
        } break;
    case 0x0d: {
        if (offset + 0x14 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectRot: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        float w = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float x = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float y = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        float z = *reinterpret_cast<float*>(offset);
        offset += sizeof(float);
        handleObjectRot(ntohl(objectid), (w), (x), (y), (z));
        } break;
    case 0x0e: {
        if (offset + 0x05 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectState: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        uint8_t ctrl = *reinterpret_cast<uint8_t*>(offset);
        offset += sizeof(uint8_t);
        handleObjectState(ntohl(objectid), (ctrl));
        } break;
    case 0x0f: {
        if (offset + 0x05 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectControl: malformed packet");
            return;
        }
        uint32_t objectid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        uint8_t ctrl = *reinterpret_cast<uint8_t*>(offset);
        offset += sizeof(uint8_t);
        handleObjectControl(ntohl(objectid), (ctrl));
        } break;
    }
}

void net::ProtocolUser::sendKeyExchange(uint64_t key)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x01;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint64_t*>(offset) = htonq(key);
    offset += sizeof(uint64_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendLogin(const char* username, uint8_t (&password)[16])
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x02;
    uint16_t len = 0;
    offset += 0x01;
    len = std::min(strlen((username)), MAXSTRLEN);
    if ((offset + len + 0x10 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x10) * 2) != 0)) {
        Log::log->warn("Network: serialise Login: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (username), len);
    offset += len;
    for (int i = 0; i < 16; i++)
        reinterpret_cast<uint8_t*>(offset)[i] = (password[i]);
    offset += sizeof(password);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendDisconnect()
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x03;
    uint16_t len = 0;
    offset += 0x01;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendWhoIsPlayer(uint32_t playerid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x04;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(playerid);
    offset += sizeof(uint32_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendPlayerInfo(uint32_t playerid, const char* username)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x05;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(playerid);
    offset += sizeof(uint32_t);
    len = std::min(strlen((username)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise PlayerInfo: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (username), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendPlayerInput(uint32_t flags)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x06;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(flags);
    offset += sizeof(uint32_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendPrivateMsg(uint32_t playerid, const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x07;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(playerid);
    offset += sizeof(uint32_t);
    len = std::min(strlen((text)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise PrivateMsg: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (text), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendBroadcastMsg(const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x08;
    uint16_t len = 0;
    offset += 0x01;
    len = std::min(strlen((text)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise BroadcastMsg: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (text), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectEnter(uint32_t objectid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x09;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectLeave(uint32_t objectid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0a;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectPos(uint32_t objectid, float x, float y, float z)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0b;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    *reinterpret_cast<float*>(offset) = (x);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (y);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (z);
    offset += sizeof(float);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectVel(uint32_t objectid, float x, float y, float z)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0c;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    *reinterpret_cast<float*>(offset) = (x);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (y);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (z);
    offset += sizeof(float);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectRot(uint32_t objectid, float w, float x, float y, float z)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0d;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    *reinterpret_cast<float*>(offset) = (w);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (x);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (y);
    offset += sizeof(float);
    *reinterpret_cast<float*>(offset) = (z);
    offset += sizeof(float);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectState(uint32_t objectid, uint8_t ctrl)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0e;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    *reinterpret_cast<uint8_t*>(offset) = (ctrl);
    offset += sizeof(uint8_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectControl(uint32_t objectid, uint8_t ctrl)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0f;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint32_t*>(offset) = htonl(objectid);
    offset += sizeof(uint32_t);
    *reinterpret_cast<uint8_t*>(offset) = (ctrl);
    offset += sizeof(uint8_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

