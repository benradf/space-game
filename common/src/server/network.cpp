/// \file network.cpp
/// \brief Server network handler
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "network.hpp"
#include "messages.hpp"
#include <core.hpp>
#include <memory>


using namespace msg;


////////// RemoteClient //////////

RemoteClient::RemoteClient(MessageSender sendMsg, void* data) :
    net::Peer(data), _sendMsg(sendMsg), _player(0)
{
    Log::log->info("NetworkInterface: remote client has connected");
}

RemoteClient::~RemoteClient()
{
    Log::log->info("NetworkInterface: remote client has dropped");
}

void RemoteClient::attachPlayer(PlayerID player)
{
    _player = player;
}

PlayerID RemoteClient::getAttachedPlayer() const
{
    return _player;
}

void RemoteClient::handleKeyExchange(uint64_t key)
{

}

void RemoteClient::handleLogin(const char* username, uint8_t (&password)[16])
{

}

void RemoteClient::handleDisconnect()
{

}

void RemoteClient::handleWhoIsPlayer(uint32_t playerid)
{

}

void RemoteClient::handlePlayerInfo(uint32_t playerid, const char* username)
{

}

void RemoteClient::handlePrivateMsg(uint32_t playerid, const char* text)
{

}

void RemoteClient::handleBroadcastMsg(const char* text)
{

}


////////// NetworkInterface //////////

NetworkInterface::NetworkInterface(PostOffice& po) :
    MessagableJob(po, MSG_UNIT), net::Interface(GAMEPORT)
{
    Log::log->info("NetworkInterface: startup");
}

NetworkInterface::~NetworkInterface()
{
    Log::log->info("NetworkInterface: shutdown");

    foreach (ClientPair& client, _clients) 
        std::auto_ptr<net::Peer>(client.second);
}

Job::RetType NetworkInterface::main()
{
    doNetworkTasks();

    return YIELD;
}

net::Peer* NetworkInterface::handleConnect(void* data)
{
    std::auto_ptr<RemoteClient> peer(new RemoteClient(newMessageSender(), data));
    _clients.insert(std::make_pair(peer->getID(), peer.get()));
    return peer.release();
}

void NetworkInterface::handleDisconnect(net::Peer* peer)
{
    RemoteClient* client = static_cast<RemoteClient*>(peer);
    _players.erase(client->getAttachedPlayer());
    _clients.erase(client->getID());
    delete peer;
}

