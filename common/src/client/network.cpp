/// \file network.cpp
/// \brief Network interface to server.
/// \author Ben Radford 
/// \date 8nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "network.hpp"
#include <core.hpp>


RemoteServer::RemoteServer(void* data) :
    net::Peer(data)
{

}

RemoteServer::~RemoteServer()
{

}

void RemoteServer::handleKeyExchange(uint64_t key)
{

}

void RemoteServer::handleLogin(const char* username, uint8_t (&password)[16])
{

}

void RemoteServer::handleDisconnect()
{

}

void RemoteServer::handleWhoIsPlayer(uint32_t playerid)
{

}

void RemoteServer::handlePlayerInfo(uint32_t playerid, const char* username)
{

}

void RemoteServer::handlePrivateMsg(uint32_t playerid, const char* text)
{

}

void RemoteServer::handleBroadcastMsg(const char* text)
{

}


////////// NetworkInterface //////////

NetworkInterface::NetworkInterface() :
    _maintainConnection(false), _connectingHandle(0)
{
    Log::log->info("NetworkInterface: startup");
}

NetworkInterface::~NetworkInterface()
{
    Log::log->info("NetworkInterface: shutdown");
}

void NetworkInterface::main()
{
    if (_maintainConnection && (_server.get() == 0)) {
        if (!connectionInProgress(_connectingHandle)) {
            Log::log->info("attempting to connect to server");
            try {
                _connectingHandle = connect(_hostname.c_str(), GAMEPORT);
            } catch (NetworkException& e) {
                Log::log->info("network exception");
                throw;
            }
        }
    }

    doNetworkTasks();
}

void NetworkInterface::setServer(const std::string& hostname)
{
    _hostname = hostname;
}

void NetworkInterface::maintainServerConnection(bool yes)
{
    _maintainConnection = yes;
}

net::Peer* NetworkInterface::handleConnect(void* data)
{
    Log::log->info("Network: now connected to server");

    _server.reset(new RemoteServer(data));
    _connectingHandle = 0;
}

void NetworkInterface::handleDisconnect(net::Peer* peer)
{
    Log::log->info("Network: disconnecting from server");

    _server.reset(0);
    _connectingHandle = 0;
}

