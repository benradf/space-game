/// \file network.cpp
/// \brief Network interface to server.
/// \author Ben Radford 
/// \date 8th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "network.hpp"
#include <core.hpp>
#include <iostream>


using namespace std;


////////// RemoteServer //////////

RemoteServer::RemoteServer(void* data) :
    net::Peer(data)
{

}

RemoteServer::~RemoteServer()
{

}

void RemoteServer::handleKeyExchange(uint64_t key)
{
    cout << "received key '" << ((void*)key) << endl;
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

void RemoteServer::handlePlayerInput(uint32_t flags)
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
    Log::log->info("starting network interface");
}

NetworkInterface::~NetworkInterface()
{
    Log::log->info("shutting down network interface");
}

void NetworkInterface::main()
{
    if (_maintainConnection && (_server.get() == 0)) {
        if (!connectionInProgress(_connectingHandle)) {
            try {
                Log::log->info("attempting to connect to server");
                _connectingHandle = connect(_hostname.c_str(), GAMEPORT);
            } catch (NetworkException& e) {
                e.annotate("while connecting to server");
                throw;
            }
        }
    }

    if (_server.get() != 0) 
        _server->updateCachedObjects();

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
    Log::log->info("connected to server");

    _server.reset(new RemoteServer(data));
    _connectingHandle = 0;

    uint8_t password[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    _server->sendLogin("username", password);

    return _server.get();
}

void NetworkInterface::handleDisconnect(net::Peer* peer)
{
    Log::log->info("disconnected from server");

    _server.reset(0);
    _connectingHandle = 0;
}

