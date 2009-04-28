/// \file network.hpp
/// \brief Network interface to server.
/// \author Ben Radford 
/// \date 8th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <net/net.hpp>
#include "cache.hpp"
#include "input.hpp"


class RemoteServer : public net::Peer, public ObjectCache {
    public:
        RemoteServer(void* data);
        virtual ~RemoteServer();

    private:
        virtual void handleKeyExchange(uint64_t key);
        virtual void handleLogin(const char* username, uint8_t (&password)[16]);
        virtual void handleDisconnect();
        virtual void handleWhoIsPlayer(uint32_t playerid);
        virtual void handlePlayerInfo(uint32_t playerid, const char* username);
        virtual void handlePlayerInput(uint32_t flags);
        virtual void handlePrivateMsg(uint32_t playerid, const char* text);
        virtual void handleBroadcastMsg(const char* text);
};


class NetworkInterface : public net::Interface {
    public:
        NetworkInterface();
        virtual ~NetworkInterface();

        void main();

        void setServer(const std::string& hostname);
        void maintainServerConnection(bool yes = true);

        RemoteServer& getServer();
        bool hasServer() const;

    private:
        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        std::auto_ptr<RemoteServer> _server;

        bool _maintainConnection;
        void* _connectingHandle;
        std::string _hostname;
};



#endif  // NETWORK_HPP

