/// \file protocol.hpp
/// \brief Network protocol.
/// \author 
/// \date 4th April 2009
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

        void sendPlayerInput(uint32_t flags);
        virtual void handlePlayerInput(uint32_t flags) = 0;

        void sendPrivateMsg(uint32_t playerid, const char* text);
        virtual void handlePrivateMsg(uint32_t playerid, const char* text) = 0;

        void sendBroadcastMsg(const char* text);
        virtual void handleBroadcastMsg(const char* text) = 0;

        void sendObjectEnter(uint32_t objectid);
        virtual void handleObjectEnter(uint32_t objectid) = 0;

        void sendObjectLeave(uint32_t objectid);
        virtual void handleObjectLeave(uint32_t objectid) = 0;

        void sendObjectPos(uint32_t objectid, float x, float y, float z);
        virtual void handleObjectPos(uint32_t objectid, float x, float y, float z) = 0;

        void sendObjectVel(uint32_t objectid, float x, float y, float z);
        virtual void handleObjectVel(uint32_t objectid, float x, float y, float z) = 0;

        void sendObjectRot(uint32_t objectid, float w, float x, float y, float z);
        virtual void handleObjectRot(uint32_t objectid, float w, float x, float y, float z) = 0;

        void sendObjectState(uint32_t objectid, uint8_t ctrl);
        virtual void handleObjectState(uint32_t objectid, uint8_t ctrl) = 0;

        void sendObjectControl(uint32_t objectid, uint8_t ctrl);
        virtual void handleObjectControl(uint32_t objectid, uint8_t ctrl) = 0;

        void sendAttachCamera(uint32_t objectid);
        virtual void handleAttachCamera(uint32_t objectid) = 0;

};


}  // namespace net


#endif  // PROTOCOL_HPP

