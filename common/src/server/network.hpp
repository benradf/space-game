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


class RemoteClient : net::Peer {
    public:
        RemoteClient(void* data);
        virtual ~RemoteClient();

    private:

};


class ServerInterface : public net::Interface {
    public:
        ServerInterface();
        virtual ~ServerInterface();

        bool hasClient(net::PeerID id) const;
        RemoteClient& getClient(net::PeerID id) const;

    private:
        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        typedef int ClientMap;
};


#endif  // NETWORK_HPP

