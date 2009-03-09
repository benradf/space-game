/// \file protocol.hpp
/// \brief Network protocol.
/// \author 
/// \date 9th March 2009
///
/// Copyright (c) 2009 . All rights reserved.
///


#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP




#include <enet/enet.h>
#include <stdint.h>


namespace net {


static const size_t MAXSTRLEN = 1024;


class ProtocolUser {
    public:
        virtual ~ProtocolUser();

        virtual void sendPacket(ENetPacket* packet) = 0;
        void handlePacket(ENetPacket* packet);

        void sendKeyExchange(uint64_t key);
        virtual void handleKeyExchange(uint64_t key) = 0;

        void sendLogin(const char* username, uint8_t (&password)[16]);
        virtual void handleLogin(const char* username, uint8_t (&password)[16]) = 0;

        void sendDisconnect();
        virtual void handleDisconnect() = 0;

        void sendWhoIsPlayer(uint32_t playerid);
        virtual void handleWhoIsPlayer(uint32_t playerid) = 0;

        void sendPlayerInfo(uint32_t playerid, const char* username);
        virtual void handlePlayerInfo(uint32_t playerid, const char* username) = 0;

        void sendPrivateMsg(uint32_t playerid, const char* text);
        virtual void handlePrivateMsg(uint32_t playerid, const char* text) = 0;

        void sendBroadcastMsg(const char* text);
        virtual void handleBroadcastMsg(const char* text) = 0;

};


}  // namespace net


#endif  // PROTOCOL_HPP

