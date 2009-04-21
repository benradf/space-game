/// \file network.cpp
/// \brief Server network handler
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "network.hpp"
#include "messages.hpp"
#include <core/core.hpp>
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
    sendKeyExchange(0x19862009);
    Log::log->info("got login message");
    _sendMsg(msg::PeerRequestLogin(getID(), username, 0));
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

void RemoteClient::handlePlayerInput(uint32_t flags)
{
    if (_player == 0) 
        return;

    _sendMsg(msg::PlayerInput(_player, flags));
}

void RemoteClient::handlePrivateMsg(uint32_t playerid, const char* text)
{

}

void RemoteClient::handleBroadcastMsg(const char* text)
{

}

void RemoteClient::handleObjectEnter(uint32_t objectid)
{

}

void RemoteClient::handleObjectLeave(uint32_t objectid)
{

}

void RemoteClient::handleObjectPos(uint32_t objectid, float x, float y, float z)
{

}

void RemoteClient::handleObjectVel(uint32_t objectid, float x, float y, float z)
{

}

void RemoteClient::handleObjectRot(uint32_t objectid, float w, float x, float y, float z)
{

}

void RemoteClient::handleObjectState(uint32_t objectid, uint8_t ctrl)
{

}

void RemoteClient::handleObjectControl(uint32_t objectid, uint8_t ctrl)
{

}

void RemoteClient::handleAttachCamera(uint32_t objectid)
{

}


////////// NetworkInterface //////////

NetworkInterface::NetworkInterface(PostOffice& po) :
    MessagableJob(po, MSG_OBJECT | MSG_PEER), net::Interface(GAMEPORT)
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

    PlayerID player = client->getAttachedPlayer();
    if (player != 0) 
        sendMessage(msg::PeerRequestLogout(player, 1));

    _players.erase(client->getAttachedPlayer());
    _clients.erase(client->getID());

    delete peer;
}

void NetworkInterface::handleObjectState(ObjectID object, int flags)
{
    foreach (ClientPair& client, _clients) 
        client.second->sendObjectState(object, flags);
}

void NetworkInterface::handleObjectPos(ObjectID object, Vector3 pos)
{
    foreach (ClientPair& client, _clients) 
        client.second->sendObjectPos(object, pos.x, pos.y, pos.z);
}

void NetworkInterface::handleObjectVel(ObjectID object, Vector3 vel)
{
    foreach (ClientPair& client, _clients) 
        client.second->sendObjectVel(object, vel.x, vel.y, vel.z);
}

void NetworkInterface::handleObjectRot(ObjectID object, Quaternion rot)
{
    foreach (ClientPair& client, _clients) 
        client.second->sendObjectRot(object, rot.w, rot.x, rot.y, rot.z);
}

void NetworkInterface::handleObjectAssoc(ObjectID object, PlayerID player)
{
    PlayerToPeer::iterator iterPlayer = _players.find(player);
    if (iterPlayer == _players.end()) 
        return;

    Clients::iterator iterClient = _clients.find(iterPlayer->second);
    assert(iterClient != _clients.end());

    iterClient->second->sendAttachCamera(object);
}

void NetworkInterface::handlePeerLoginGranted(PeerID peer, PlayerID player)
{
    Clients::iterator iter = _clients.find(peer);
    if (iter == _clients.end()) 
        return;

    _players.insert(std::make_pair(player, peer));
    iter->second->attachPlayer(player);
}

void NetworkInterface::handlePeerLoginDenied(PeerID peer)
{
    Clients::iterator iter = _clients.find(peer);
    if (iter == _clients.end()) 
        return;

    iter->second->disconnect();
}

void NetworkInterface::forEachClient(void (NetworkInterface::*f)(PeerID,RemoteClient*))
{
    foreach (ClientPair& client, _clients) 
        (this->*f)(client.first, client.second);
}

