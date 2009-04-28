/// \file network.hpp
/// \brief Server network handler
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <net/net.hpp>
#include <tr1/unordered_map>
#include "objcache.hpp"
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
        virtual void handleObjectEnter(uint16_t objectid);
        virtual void handleObjectLeave(uint16_t objectid);
        virtual void handleObjectAttach(uint16_t objectid);
        virtual void handleObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y);
        virtual void handleObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y, 
            int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl);

        MessageSender _sendMsg;
        PlayerID _player;
};


class NetworkInterface : public ObjectCache,
                         public MessagableJob, 
                         private net::Interface {
    public:
        friend class RemoteClient;

        NetworkInterface(PostOffice& po);
        virtual ~NetworkInterface();

        virtual RetType main();

    private:
        typedef std::tr1::unordered_map<PlayerID, net::PeerID> PlayerToPeer;
        typedef std::tr1::unordered_map<net::PeerID, RemoteClient*> Clients;

        virtual void tellPlayerObjectPos(PlayerID player, const CachedObjectInfo& object);
        virtual void tellPlayerObjectAll(PlayerID player, const CachedObjectInfo& object);
        virtual void tellPlayerObjectAttach(PlayerID player, ObjectID object);
        virtual void tellPlayerObjectLeave(PlayerID player, ObjectID object);

        virtual void handlePeerLoginGranted(PeerID peer, PlayerID player);
        virtual void handlePeerLoginDenied(PeerID peer);

        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        RemoteClient* getClientByPlayer(PlayerID player);

        PlayerToPeer _players;
        Clients _clients;
};


#endif  // NETWORK_HPP

