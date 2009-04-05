/// \file network.hpp
/// \brief Server network handler
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <net.hpp>
#include <tr1/unordered_map>
#include "msgjob.hpp"


class NetworkInterface;


class RemoteClient : public net::Peer {
    public:
        RemoteClient(MessageSender sendMsg, void* data);
        virtual ~RemoteClient();

        void attachPlayer(PlayerID player);
        PlayerID getAttachedPlayer() const;

    private:
        virtual void handleKeyExchange(uint64_t key);
        virtual void handleLogin(const char* username, uint8_t (&password)[16]);
        virtual void handleDisconnect();
        virtual void handleWhoIsPlayer(uint32_t playerid);
        virtual void handlePlayerInfo(uint32_t playerid, const char* username);
        virtual void handlePlayerInput(uint32_t flags);
        virtual void handlePrivateMsg(uint32_t playerid, const char* text);
        virtual void handleBroadcastMsg(const char* text);
        virtual void handleObjectEnter(uint32_t objectid);
        virtual void handleObjectLeave(uint32_t objectid);
        virtual void handleObjectPos(uint32_t objectid, float x, float y, float z);
        virtual void handleObjectVel(uint32_t objectid, float x, float y, float z);
        virtual void handleObjectRot(uint32_t objectid, float w, float x, float y, float z);
        virtual void handleObjectState(uint32_t objectid, uint8_t ctrl);
        virtual void handleObjectControl(uint32_t objectid, uint8_t ctrl);
        virtual void handleAttachCamera(uint32_t objectid);

        MessageSender _sendMsg;
        PlayerID _player;
};


class NetworkInterface : public MessagableJob, 
                         private net::Interface {
    public:
        friend class RemoteClient;

        NetworkInterface(PostOffice& po);
        virtual ~NetworkInterface();

        virtual RetType main();

    private:
        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        virtual void handleObjectState(ObjectID object, int flags);
        virtual void handleObjectPos(ObjectID object, Vector3 pos);
        virtual void handleObjectVel(ObjectID object, Vector3 vel);
        virtual void handleObjectRot(ObjectID object, Quaternion rot);
        virtual void handleObjectAssoc(ObjectID object, PlayerID player);

        virtual void handlePeerLoginGranted(PeerID peer, PlayerID player);
        virtual void handlePeerLoginDenied(PeerID peer);

        void forEachClient(void (NetworkInterface::*f)(PeerID,RemoteClient*));

        typedef std::tr1::unordered_map<net::PeerID, RemoteClient*> Clients;
        typedef std::tr1::unordered_map<PlayerID, net::PeerID> PlayerToPeer;
        typedef std::pair<const net::PeerID, RemoteClient*> ClientPair;
        typedef std::pair<const PlayerID, net::PeerID> PlayerLookup;

        PlayerToPeer _players;
        Clients _clients;
};


#endif  // NETWORK_HPP

