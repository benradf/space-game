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
        virtual void handlePrivateMsg(uint32_t playerid, const char* text);
        virtual void handleBroadcastMsg(const char* text);

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

        typedef std::tr1::unordered_map<net::PeerID, RemoteClient*> Clients;
        typedef std::tr1::unordered_map<PlayerID, net::PeerID> PlayerToPeer;
        typedef std::pair<const net::PeerID, RemoteClient*> ClientPair;
        typedef std::pair<const PlayerID, net::PeerID> PlayerLookup;

        PlayerToPeer _players;
        Clients _clients;
};


#endif  // NETWORK_HPP

