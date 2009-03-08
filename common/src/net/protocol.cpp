/// \file protocol.cpp
/// \brief Network protocol.
/// \author 
/// \date 8th March 2009
///
/// Copyright (c) 2009 . All rights reserved.
///


#include "protocol.hpp"
#include <core.hpp>
#include <memory.h>


#define htonq(x) x  // needs implementing

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
        if ( offset + 0x08 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise KeyExchange: malformed packet");
            return;
        }
        uint64_t key = *reinterpret_cast<uint64_t*>(offset);
        offset += sizeof(uint64_t);
        handleKeyExchange(htonq(key));
        } break;
    case 0x02: {
        if ( offset + 0x12 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise Login: malformed packet");
            return;
        }
        len = *reinterpret_cast<uint16_t*>(offset);
        offset += 0x02;
        if (offset + len + 0x10 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise Login: malformed packet");
            return;
        }
        char* username = reinterpret_cast<char*>(offset - 1);
        memmove(username, offset, len);
        username[len] = '\0';
        offset += len;
        uint8_t* password = reinterpret_cast<uint8_t*>(offset);
        offset += sizeof(password);
        handleLogin(username, reinterpret_cast<uint8_t(&)[16]>(*password));
        } break;
    case 0x03: {
        if ( offset + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise Disconnect: malformed packet");
            return;
        }
        handleDisconnect();
        } break;
    case 0x04: {
        if ( offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise WhoIsPlayer: malformed packet");
            return;
        }
        uint32_t playerid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        handleWhoIsPlayer(htonl(playerid));
        } break;
    case 0x05: {
        if ( offset + 0x06 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PlayerInfo: malformed packet");
            return;
        }
        uint32_t playerid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        len = *reinterpret_cast<uint16_t*>(offset);
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PlayerInfo: malformed packet");
            return;
        }
        char* username = reinterpret_cast<char*>(offset - 1);
        memmove(username, offset, len);
        username[len] = '\0';
        offset += len;
        handlePlayerInfo(htonl(playerid), username);
        } break;
    case 0x06: {
        if ( offset + 0x06 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PrivateMsg: malformed packet");
            return;
        }
        uint32_t playerid = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        len = *reinterpret_cast<uint16_t*>(offset);
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PrivateMsg: malformed packet");
            return;
        }
        char* text = reinterpret_cast<char*>(offset - 1);
        memmove(text, offset, len);
        text[len] = '\0';
        offset += len;
        handlePrivateMsg(htonl(playerid), text);
        } break;
    case 0x07: {
        if ( offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise BroadcastMsg: malformed packet");
            return;
        }
        len = *reinterpret_cast<uint16_t*>(offset);
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise BroadcastMsg: malformed packet");
            return;
        }
        char* text = reinterpret_cast<char*>(offset - 1);
        memmove(text, offset, len);
        text[len] = '\0';
        offset += len;
        handleBroadcastMsg(text);
        } break;
    }
}

void net::ProtocolUser::sendKeyExchange(uint64_t key)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x01;
    uint16_t len = 0;
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
    len = std::min(strlen(username), MAXSTRLEN);
    if ((offset + len + 0x10 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x10) * 2) != 0)) {
        Log::log->warn("Network: serialise Login: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = len;
    memcpy(offset += 0x02, username, len);
    offset += len;
    for (int i = 0; i < 16; i++)
        reinterpret_cast<uint8_t*>(offset)[i] = password[i];
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
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendWhoIsPlayer(uint32_t playerid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x04;
    uint16_t len = 0;
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
    *reinterpret_cast<uint32_t*>(offset) = htonl(playerid);
    offset += sizeof(uint32_t);
    len = std::min(strlen(username), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise PlayerInfo: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = len;
    memcpy(offset += 0x02, username, len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendPrivateMsg(uint32_t playerid, const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x06;
    uint16_t len = 0;
    *reinterpret_cast<uint32_t*>(offset) = htonl(playerid);
    offset += sizeof(uint32_t);
    len = std::min(strlen(text), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise PrivateMsg: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = len;
    memcpy(offset += 0x02, text, len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendBroadcastMsg(const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x07;
    uint16_t len = 0;
    len = std::min(strlen(text), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise BroadcastMsg: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = len;
    memcpy(offset += 0x02, text, len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

