/// \file protocol.cpp
/// \brief Network protocol.
/// \author 
/// \date 16th May 2009
///
/// Copyright (c) 2009 . All rights reserved.
///


#include "protocol.hpp"
#include <core/core.hpp>
#include <memory.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wunused-variable"

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
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise GetObjectName: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        handleGetObjectName(ntohs(objectid));
        } break;
    case 0x06: {
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
    case 0x07: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise PlayerInput: malformed packet");
            return;
        }
        uint32_t flags = *reinterpret_cast<uint32_t*>(offset);
        offset += sizeof(uint32_t);
        handlePlayerInput(ntohl(flags));
        } break;
    case 0x08: {
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
    case 0x09: {
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
    case 0x0a: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectEnter: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        handleObjectEnter(ntohs(objectid));
        } break;
    case 0x0b: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectLeave: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        handleObjectLeave(ntohs(objectid));
        } break;
    case 0x0c: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectAttach: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        handleObjectAttach(ntohs(objectid));
        } break;
    case 0x0d: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectName: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectName: malformed packet");
            return;
        }
        char* (name) = reinterpret_cast<char*>(offset - 1);
        memmove((name), offset, len);
        (name)[len] = '\0';
        offset += len;
        handleObjectName(ntohs(objectid), (name));
        } break;
    case 0x0e: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectUpdatePartial: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        int16_t s_x = *reinterpret_cast<int16_t*>(offset);
        offset += sizeof(int16_t);
        int16_t s_y = *reinterpret_cast<int16_t*>(offset);
        offset += sizeof(int16_t);
        handleObjectUpdatePartial(ntohs(objectid), ntohs(s_x), ntohs(s_y));
        } break;
    case 0x0f: {
        if (offset + 0x04 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise ObjectUpdateFull: malformed packet");
            return;
        }
        uint16_t objectid = *reinterpret_cast<uint16_t*>(offset);
        offset += sizeof(uint16_t);
        int16_t s_x = *reinterpret_cast<int16_t*>(offset);
        offset += sizeof(int16_t);
        int16_t s_y = *reinterpret_cast<int16_t*>(offset);
        offset += sizeof(int16_t);
        int16_t v_x = *reinterpret_cast<int16_t*>(offset);
        offset += sizeof(int16_t);
        int16_t v_y = *reinterpret_cast<int16_t*>(offset);
        offset += sizeof(int16_t);
        uint8_t rot = *reinterpret_cast<uint8_t*>(offset);
        offset += sizeof(uint8_t);
        uint8_t ctrl = *reinterpret_cast<uint8_t*>(offset);
        offset += sizeof(uint8_t);
        handleObjectUpdateFull(ntohs(objectid), ntohs(s_x), ntohs(s_y), ntohs(v_x), ntohs(v_y), (rot), (ctrl));
        } break;
    case 0x10: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgPubChat: malformed packet");
            return;
        }
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgPubChat: malformed packet");
            return;
        }
        char* (text) = reinterpret_cast<char*>(offset - 1);
        memmove((text), offset, len);
        (text)[len] = '\0';
        offset += len;
        handleMsgPubChat((text));
        } break;
    case 0x11: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgPrivChat: malformed packet");
            return;
        }
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgPrivChat: malformed packet");
            return;
        }
        char* (text) = reinterpret_cast<char*>(offset - 1);
        memmove((text), offset, len);
        (text)[len] = '\0';
        offset += len;
        handleMsgPrivChat((text));
        } break;
    case 0x12: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgSystem: malformed packet");
            return;
        }
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgSystem: malformed packet");
            return;
        }
        char* (text) = reinterpret_cast<char*>(offset - 1);
        memmove((text), offset, len);
        (text)[len] = '\0';
        offset += len;
        handleMsgSystem((text));
        } break;
    case 0x13: {
        if (offset + 0x02 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgInfo: malformed packet");
            return;
        }
        len = ntohs(*reinterpret_cast<uint16_t*>(offset));
        offset += 0x02;
        if (offset + len + 0x00 > packet->data + packet->dataLength) {
            Log::log->warn("Network: deserialise MsgInfo: malformed packet");
            return;
        }
        char* (text) = reinterpret_cast<char*>(offset - 1);
        memmove((text), offset, len);
        (text)[len] = '\0';
        offset += len;
        handleMsgInfo((text));
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

void net::ProtocolUser::sendGetObjectName(uint16_t objectid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x05;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendPlayerInfo(uint32_t playerid, const char* username)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x06;
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
    *reinterpret_cast<uint8_t*>(offset) = 0x07;
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
    *reinterpret_cast<uint8_t*>(offset) = 0x08;
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
    *reinterpret_cast<uint8_t*>(offset) = 0x09;
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

void net::ProtocolUser::sendObjectEnter(uint16_t objectid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0a;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectLeave(uint16_t objectid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0b;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectAttach(uint16_t objectid)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0c;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectName(uint16_t objectid, const char* name)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0d;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    len = std::min(strlen((name)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise ObjectName: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (name), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0e;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    *reinterpret_cast<int16_t*>(offset) = htons(s_x);
    offset += sizeof(int16_t);
    *reinterpret_cast<int16_t*>(offset) = htons(s_y);
    offset += sizeof(int16_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y, int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x0f;
    uint16_t len = 0;
    offset += 0x01;
    *reinterpret_cast<uint16_t*>(offset) = htons(objectid);
    offset += sizeof(uint16_t);
    *reinterpret_cast<int16_t*>(offset) = htons(s_x);
    offset += sizeof(int16_t);
    *reinterpret_cast<int16_t*>(offset) = htons(s_y);
    offset += sizeof(int16_t);
    *reinterpret_cast<int16_t*>(offset) = htons(v_x);
    offset += sizeof(int16_t);
    *reinterpret_cast<int16_t*>(offset) = htons(v_y);
    offset += sizeof(int16_t);
    *reinterpret_cast<uint8_t*>(offset) = (rot);
    offset += sizeof(uint8_t);
    *reinterpret_cast<uint8_t*>(offset) = (ctrl);
    offset += sizeof(uint8_t);
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendMsgPubChat(const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x10;
    uint16_t len = 0;
    offset += 0x01;
    len = std::min(strlen((text)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise MsgPubChat: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (text), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendMsgPrivChat(const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x11;
    uint16_t len = 0;
    offset += 0x01;
    len = std::min(strlen((text)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise MsgPrivChat: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (text), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendMsgSystem(const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x12;
    uint16_t len = 0;
    offset += 0x01;
    len = std::min(strlen((text)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise MsgSystem: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (text), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

void net::ProtocolUser::sendMsgInfo(const char* text)
{
    ENetPacket* packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    enet_uint8* offset = packet->data;
    *reinterpret_cast<uint8_t*>(offset) = 0x13;
    uint16_t len = 0;
    offset += 0x01;
    len = std::min(strlen((text)), MAXSTRLEN);
    if ((offset + len + 0x00 > packet->data + packet->dataLength) && 
            (enet_packet_resize(packet, (packet->dataLength + len + 0x00) * 2) != 0)) {
        Log::log->warn("Network: serialise MsgInfo: enet_packet_resize failed");
        return;
    }
    *reinterpret_cast<uint16_t*>(offset) = htons(len);
    memcpy(offset += 0x02, (text), len);
    offset += len;
    enet_packet_resize(packet, offset - packet->data);
    sendPacket(packet);
}

#pragma GCC diagnostic pop
