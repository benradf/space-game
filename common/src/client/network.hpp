/// \file network.hpp
/// \brief Network interface to server.
/// \author Ben Radford 
/// \date 8th March 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <net/net.hpp>
#include "cache.hpp"
#include "input.hpp"
#include "chat.hpp"


class Login;


class RemoteServer : public net::Peer, 
                     public ObjectCache, 
                     public ChatSystem {
    public:
        RemoteServer(void* data);
        virtual ~RemoteServer();

    private:
        virtual void handleKeyExchange(uint64_t key);
        virtual void handleLogin(const char* username, uint8_t (&password)[16]);
        virtual void handleDisconnect();
        virtual void handleWhoIsPlayer(uint32_t playerid);
        virtual void handleGetObjectName(uint16_t objectid);
        virtual void handlePlayerInfo(uint32_t playerid, const char* username);
        virtual void handlePlayerInput(uint32_t flags);
        virtual void handlePrivateMsg(uint32_t playerid, const char* text);
        virtual void handleBroadcastMsg(const char* text);
};


class NetworkInterface : public net::Interface {
    public:
        NetworkInterface(Login& login);
        virtual ~NetworkInterface();

        void main();

        RemoteServer& getServer();
        bool hasServer() const;

        void maintainServerConnection(bool yes = true);

    private:
        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        std::unique_ptr<RemoteServer> _server;

        bool _maintainConnection;
        void* _connectingHandle;
        Login& _login;
};



#endif  // NETWORK_HPP

