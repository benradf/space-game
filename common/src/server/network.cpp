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
#include <net/compress.hpp>
#include <memory>


using namespace msg;
using namespace net;


////////// RemoteClient //////////

RemoteClient::RemoteClient(NetworkInterface& net, MessageSender sendMsg, void* data) :
    net::Peer(data), _net(net), _sendMsg(sendMsg), _player(0)
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
    Log::log->warn("unexpected message: KeyExchange");
}

void RemoteClient::handleLogin(const char* username, uint8_t (&password)[16])
{
    sendKeyExchange(0x19862009);
    Log::log->info("got login message");
    _sendMsg(msg::PeerRequestLogin(getID(), username, 0));
}

void RemoteClient::handleDisconnect()
{

    Log::log->warn("unexpected message: Disconnect");
}

void RemoteClient::handleWhoIsPlayer(uint32_t playerid)
{

    Log::log->warn("unexpected message: WhoIsPlayer");
}

void RemoteClient::handleGetObjectName(uint16_t objectid)
{
    const CachedObjectInfo* objectInfo = _net.getCachedObjectInfo(objectid);
    if (objectInfo == 0) 
        return;

    sendObjectName(objectid, objectInfo->getName().c_str());
}

void RemoteClient::handlePlayerInfo(uint32_t playerid, const char* username)
{

    Log::log->warn("unexpected message: PlayerInfo");
}

void RemoteClient::handlePlayerInput(uint32_t flags)
{

    Log::log->warn("unexpected message: PlayerInput");
}

void RemoteClient::handlePrivateMsg(uint32_t playerid, const char* text)
{

    Log::log->warn("unexpected message: PrivateMsg");
}

void RemoteClient::handleBroadcastMsg(const char* text)
{

    Log::log->warn("unexpected message: BroadcastMsg");
}


void RemoteClient::handleObjectEnter(uint16_t objectid)
{

    Log::log->warn("unexpected message: ObjectEnter");
}

void RemoteClient::handleObjectLeave(uint16_t objectid)
{
    Log::log->warn("unexpected message: ObjectLeave");
}

void RemoteClient::handleObjectName(uint16_t objectid, const char* name)
{
    Log::log->warn("unexpected message: ObjectName");
}

void RemoteClient::handleObjectAttach(uint16_t objectid)
{
    Log::log->warn("unexpected message: ObjectAttach");
}

void RemoteClient::handleObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y)
{
    _sendMsg(msg::ZoneTellObjectPos(_player, objectid, unpackPos(s_x, s_y)));
}

void RemoteClient::handleObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y, 
    int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl)
{
    _sendMsg(msg::ZoneTellObjectAll(_player, objectid, unpackPos(s_x, s_y), 
        unpackVel(v_x, v_y), unpackRot(rot), ctrl));
}

void RemoteClient::handleMsgPubChat(const char* text)
{
    _sendMsg(msg::ChatSayPublic(_player, text));
}

void RemoteClient::handleMsgPrivChat(const char* text)
{
    Log::log->warn("unexpected message: MsgPrivChat");
}

void RemoteClient::handleMsgSystem(const char* text)
{
    Log::log->warn("unexpected message: MsgSystem");
}

void RemoteClient::handleMsgInfo(const char* text)
{
    Log::log->warn("unexpected message: MsgInfo");
}


////////// NetworkInterface //////////

NetworkInterface::NetworkInterface(PostOffice& po) :
    MessagableJob(po, MSG_ZONESAYS | MSG_PEER | MSG_CHAT), net::Interface(GAMEPORT)
{
    Log::log->info("NetworkInterface: startup");
}

NetworkInterface::~NetworkInterface()
{
    Log::log->info("NetworkInterface: shutdown");

    for (auto& client : _clients) 
        std::unique_ptr<net::Peer>(client.second);
}

Job::RetType NetworkInterface::main()
{
    doNetworkTasks();

    return YIELD;
}

void NetworkInterface::tellPlayerObjectPos(PlayerID player, const CachedObjectInfo& object)
{
    RemoteClient* client = getClientByPlayer(player);

    if (client == 0) 
        return;

    Vec2<int16_t> pos(packPos(object.getPosition()));

    client->sendObjectUpdatePartial(object.getID(), pos.x, pos.y);
}

void NetworkInterface::tellPlayerObjectAll(PlayerID player, const CachedObjectInfo& object)
{
    RemoteClient* client = getClientByPlayer(player);

    if (client == 0) 
        return;

    Vec2<int16_t> pos(packPos(object.getPosition()));
    Vec2<int16_t> vel(packVel(object.getVelocity()));
    uint8_t rot = packRot(object.getRotation());
    sim::ControlState state = object.getControlState();

    client->sendObjectUpdateFull(object.getID(), 
        pos.x, pos.y, vel.x, vel.y, rot, state);
}

void NetworkInterface::tellPlayerObjectAttach(PlayerID player, ObjectID object)
{
    RemoteClient* client = getClientByPlayer(player);

    if (client == 0) 
        return;

    client->sendObjectAttach(object);
}

void NetworkInterface::tellPlayerObjectLeave(PlayerID player, ObjectID object)
{
    RemoteClient* client = getClientByPlayer(player);

    if (client == 0) 
        return;

    client->sendObjectLeave(object);
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

void NetworkInterface::handleChatBroadcast(const std::string& text)
{
    for (auto& client : _clients) 
        client.second->sendMsgPubChat(text.c_str());
}

net::Peer* NetworkInterface::handleConnect(void* data)
{
    auto peer = std::make_unique<RemoteClient>(*this, newMessageSender(), data);
    _clients.insert(std::make_pair(peer->getID(), peer.get()));
    return peer.release();
}

void NetworkInterface::handleDisconnect(net::Peer* peer)
{
    RemoteClient* client = dynamic_cast<RemoteClient*>(peer);

    PlayerID player = client->getAttachedPlayer();
    if (player != 0) 
        sendMessage(msg::PeerRequestLogout(peer->getID(), player));

    _players.erase(client->getAttachedPlayer());
    _clients.erase(client->getID());

    delete peer;
}

RemoteClient* NetworkInterface::getClientByPlayer(PlayerID player)
{
    PlayerToPeer::iterator iterPlayer = _players.find(player);
    if (iterPlayer == _players.end()) 
        return 0;

    Clients::iterator iterClient = _clients.find(iterPlayer->second);
    assert(iterClient != _clients.end());

    return iterClient->second;
}

