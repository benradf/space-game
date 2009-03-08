/// \file protocol.hpp
/// \brief Network protocol.
/// \author 
/// \date 8th March 2009
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

        void sendPlayerLogin(const char* username, uint8_t (&password)[16]);
        virtual void handlePlayerLogin(const char* username, uint8_t (&password)[16]) = 0;

        void sendKeyExchange(uint64_t key);
        virtual void handleKeyExchange(uint64_t key) = 0;

        void sendQuit();
        virtual void handleQuit() = 0;

        void sendPing(uint64_t (&x)[8], uint8_t a);
        virtual void handlePing(uint64_t (&x)[8], uint8_t a) = 0;

};


}  // namespace net


#endif  // PROTOCOL_HPP

