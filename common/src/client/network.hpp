/// \file network.hpp
/// \brief Network interface to server.
/// \author Ben Radford 
/// \date 8nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <net.hpp>


class RemoteServer : public net::Peer {
    public:
        RemoteServer(void* data);
        virtual ~RemoteServer();

    private:
        void handleKeyExchange(uint64_t key);
        void handleLogin(const char* username, uint8_t (&password)[16]);
        void handleDisconnect();
        void handleWhoIsPlayer(uint32_t playerid);
        void handlePlayerInfo(uint32_t playerid, const char* username);
        void handlePrivateMsg(uint32_t playerid, const char* text);
        void handleBroadcastMsg(const char* text);

};


class NetworkInterface : public net::Interface {
    public:
        NetworkInterface();
        virtual ~NetworkInterface();

        void main();

        void setServer(const std::string& hostname);
        void maintainServerConnection(bool yes = true);

    private:
        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        std::auto_ptr<RemoteServer> _server;

        bool _maintainConnection;
        void* _connectingHandle;
        std::string _hostname;
};



#endif  // NETWORK_HPP

