/// \file network.cpp
/// \brief Server network handler
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "network.hpp"
#include "messages.hpp"


using namespace msg;


////////// RemoteClient //////////

RemoteClient::RemoteClient(void* data) :
    net::Peer(data)
{

}

RemoteClient::~RemoteClient()
{

}


////////// NetworkInterface //////////

NetworkInterface::NetworkInterface(PostOffice& po) :
    MessagableJob(po, MSG_UNIT), net::Interface(LOCALPORT)
{
    Log::log->info("NetworkInterface: startup");
}

NetworkInterface::~NetworkInterface()
{
    Log::log->info("NetworkInterface: shutdown");
}

Job::RetType NetworkInterface::main()
{

    return YIELD;
}

net::Peer* NetworkInterface::handleConnect(void* data)
{
    new RemoteClient(data);
}

void NetworkInterface::handleDisconnect(net::Peer* peer)
{

}

void NetworkInterface::handleUnitNear(int unit1, int unit2)
{

}

