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


class RemoteClient : net::Peer {
    public:
        RemoteClient(void* data);
        virtual ~RemoteClient();

    private:

};


class NetworkInterface : public MessagableJob, 
                        private net::Interface {
    public:
        NetworkInterface(PostOffice& po);
        virtual ~NetworkInterface();

        virtual RetType main();

    private:
        static const int LOCALPORT = 12345;

        virtual net::Peer* handleConnect(void* data);
        virtual void handleDisconnect(net::Peer* peer);

        virtual void handleUnitNear(int unit1, int unit2);
};


#endif  // NETWORK_HPP

