/// \file protocol.hpp
/// \brief Network protocol.
/// \author 
/// \date 16th May 2009
///
/// Copyright (c) 2009 .
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

        void sendGetObjectName(uint16_t objectid);
        virtual void handleGetObjectName(uint16_t objectid) = 0;

        void sendPlayerInfo(uint32_t playerid, const char* username);
        virtual void handlePlayerInfo(uint32_t playerid, const char* username) = 0;

        void sendPlayerInput(uint32_t flags);
        virtual void handlePlayerInput(uint32_t flags) = 0;

        void sendPrivateMsg(uint32_t playerid, const char* text);
        virtual void handlePrivateMsg(uint32_t playerid, const char* text) = 0;

        void sendBroadcastMsg(const char* text);
        virtual void handleBroadcastMsg(const char* text) = 0;

        void sendObjectEnter(uint16_t objectid);
        virtual void handleObjectEnter(uint16_t objectid) = 0;

        void sendObjectLeave(uint16_t objectid);
        virtual void handleObjectLeave(uint16_t objectid) = 0;

        void sendObjectAttach(uint16_t objectid);
        virtual void handleObjectAttach(uint16_t objectid) = 0;

        void sendObjectName(uint16_t objectid, const char* name);
        virtual void handleObjectName(uint16_t objectid, const char* name) = 0;

        void sendObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y);
        virtual void handleObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y) = 0;

        void sendObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y, int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl);
        virtual void handleObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y, int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl) = 0;

        void sendMsgPubChat(const char* text);
        virtual void handleMsgPubChat(const char* text) = 0;

        void sendMsgPrivChat(const char* text);
        virtual void handleMsgPrivChat(const char* text) = 0;

        void sendMsgSystem(const char* text);
        virtual void handleMsgSystem(const char* text) = 0;

        void sendMsgInfo(const char* text);
        virtual void handleMsgInfo(const char* text) = 0;

};


}  // namespace net


#endif  // PROTOCOL_HPP

