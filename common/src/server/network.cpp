/// \file network.cpp
/// \brief Server network handler
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "network.hpp"


////////// RemoteClient //////////

RemoteClient::RemoteClient(void* data) :
    net::Peer(data)
{

}

RemoteClient::~RemoteClient()
{

}


////////// ServerInterface //////////

ServerInterface::ServerInterface()
{

}

ServerInterface::~ServerInterface()
{

}

net::Peer* ServerInterface::handleConnect(void* data)
{
    new RemoteClient(data);
}

void ServerInterface::handleDisconnect(net::Peer* peer)
{

}

