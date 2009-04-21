/// \file net.cpp
/// \brief Network module
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "net.hpp"
#include <core/core.hpp>
#include <assert.h>
#include <enet/enet.h>


/// Initialise network module.
/// This function must be called before creating any Interface objects.
void net::initialise()
{
    enet_initialize();
}

/// Cleanup network module.
/// This function must be called before the program terminates and after all 
/// Interface objects have been deleted.
void net::cleanup()
{
    enet_deinitialize();
}


////////// net::Peer //////////

/// Construct peer base object.
/// \param data This should be the data passed to the connect handler.
net::Peer::Peer(void* data) :
    _peer(static_cast<ENetPeer*>(data))
{
    enet_address_get_host_ip(&_peer->address, _ip, sizeof(_ip));
}

/// Clean up peer base object.
net::Peer::~Peer()
{
    disconnect(true);
}

/// \return Unique identifier for peer.
net::PeerID net::Peer::getID() const
{
    return makePeerID(_peer->address);
}

/// \return Port used on remote peer.
uint16_t net::Peer::getRemotePort() const
{
    return _peer->address.port;
}

/// \return Remote IP address in dotted quad notation.
const char* net::Peer::getIPAddress() const
{
    return _ip;
}

/// \return Mean round trip time in milliseconds.
int net::Peer::getRoundTripTime() const
{
    return _peer->roundTripTime;
}

/// Terminates the connection.
/// In normal operation this function begins the disconnect process. When the
/// disconnect is complete the Interface controlling the peer is notified by 
/// the Interface::handleDisconnect function being called. However if the
/// disconnect is forced no such notification is given and the caller is 
/// responsible for freeing the Peer.
/// \param force If set the disconnect is immediate.
void net::Peer::disconnect(bool force)
{
    if (force) {
        enet_peer_disconnect_now(_peer, 0);
    } else {
        enet_peer_disconnect(_peer, 0);
    }
}

/// Called by ProtocolUser to send packet to peer.
/// \param packet Packet to send to peer.
void net::Peer::sendPacket(ENetPacket* packet)
{
    enet_peer_send(_peer, 0, packet);
}


////////// net::Interface //////////

/// Construct network Interface object.
/// This constructor is intended for "client" interfaces. These interfaces only
/// need to connect to remote peers, not listen for connections themselves.
net::Interface::Interface()
{
    if ((_host = enet_host_create(0, 1, 0, 0)) == 0)
        throw NetworkException("enet_host_create failed");
}

/// Construct network Interface object to listen on specified address.
/// This constructor should be used by "server" interfaces that need to listen
/// and accept remote connections.
/// \param port Local port to listen on.
/// \param addr Local IP address to listen on (defaults to all).
net::Interface::Interface(uint16_t port, uint32_t addr)
{
    ENetAddress address;
    address.host = addr;
    address.port = port;

    if ((_host = enet_host_create(&address, 1024, 0, 0)) == 0)
        throw NetworkException("enet_host_create failed");
}

/// Cleanup network Interface object.
net::Interface::~Interface()
{
    enet_host_destroy(_host);
}

/// Initiate a remote connection on this Interface.
/// This function returns immediately. When the connection is successfully
/// established the Interface::handleConnect function will be called. While the
/// connection is still being negotiated the Interface::connectionInProgress
/// function will return true when passed the handle returned by this function.
/// If the connection fails to be established Interface::connectionInProgress
/// will cease returning true. Use this to detected failed connection attempts.
/// \param host Hostname or dotted quad IP of remote host to connect to.
/// \param port Remote port to connect to.
/// \return Connection handle that can be used to check progress.
void* net::Interface::connect(const char* host, uint16_t port)
{
    ENetAddress address;
    enet_address_set_host(&address, host);
    address.port = port;

    ENetPeer* peer = 0;
    if ((peer = enet_host_connect(_host, &address, 1)) == 0) 
        throw NetworkException("enet_host_connect failed");

    _connecting.insert(peer);
    peer->data = 0;

    return peer;
}

/// \param handle Handle to connection returned by Interface::connect.
/// \return Whether the connection is still in progress.
bool net::Interface::connectionInProgress(void* handle) const
{
    return (_connecting.find(handle) != _connecting.end());
}

/// Process incoming/outgoing messages and handle connection requests.
void net::Interface::doNetworkTasks()
{
    ENetEvent event;

    if (enet_host_service(_host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                return eventConnect(event);
            case ENET_EVENT_TYPE_RECEIVE:
                return eventReceive(event);
            case ENET_EVENT_TYPE_DISCONNECT:
                return eventDisconnect(event);
        }
    }
}

/// Process an ENet connect event.
/// \param event ENet event object.
void net::Interface::eventConnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    event.peer->data = handleConnect(event.peer);
}

/// Process an ENet receive event.
/// \param event ENet event object.
void net::Interface::eventReceive(ENetEvent& event)
{
    assert(event.peer->data != 0);
    Peer& peer = *reinterpret_cast<Peer*>(event.peer->data);
    peer.handlePacket(event.packet);
    enet_packet_destroy(event.packet);
}

/// Process an ENet disconnect event.
/// \param event ENet event object.
void net::Interface::eventDisconnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    if (event.peer->data != 0) 
        handleDisconnect(reinterpret_cast<Peer*>(event.peer->data));
}

