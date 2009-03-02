/// \file net.cpp
/// \brief Network module
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///

#include "net.hpp"
#include <core.hpp>
#include <assert.h>
#include <enet/enet.h>


/// \brief Construct peer base object.
/// \param data This should be the data passed to the connect handler.
net::Peer::Peer(void* data) :
    _packet(0), _peer(static_cast<ENetPeer*>(data))
{
    enet_address_get_host_ip(&_peer->address, _ip, sizeof(_ip));
}

/// \brief Clean up peer base object.
net::Peer::~Peer()
{
    disconnect(true);
}

/// \brief Terminates the connection.
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

/// \brief Send a ping message.
/// \param time Time to include with ping.
void net::Peer::sendPing(uint64_t time)
{
    packetBegin(MSG_PING);
    packetWrite(time);
    packetEnd();
}

/// \brief Send our encryption key to remote peer.
/// \param key Encryption key to send.
void net::Peer::sendKeyExchange(uint64_t key)
{
    packetBegin(MSG_KEYEXCHANGE);
    packetWrite(key);
    packetEnd();
}

/// \brief Initiate a player login with remote server.
/// \param username Username of player logging in.
/// \param password Hash of player password.
void net::Peer::sendPlayerLogin(const char* username, char (&password)[16])
{
    packetBegin(MSG_PLAYERLOGIN);
    packetWrite(username);
    packetWrite(password);
    packetEnd();
}

/// \return Remote IP address in dotted quad notation.
const char* net::Peer::getIPAddress() const
{
    return _ip;
}

/// \return Port used on remote peer.
uint16_t net::Peer::getRemotePort() const
{
    return _peer->address.port;
}

/// \brief Indicates the beginning of a new packet.
/// After calling this, the Peer::packetWrite functions may be called to write 
/// the actual contents of the message.
/// \param type Type of message to begin.
void net::Peer::packetBegin(MsgType type)
{
    assert(_packet == 0);
    _packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    _packetWriter.open(reinterpret_cast<char*>(_packet->data), _packet->dataLength);

    assert(_packetWriter.is_open());

    _packetWriter << static_cast<uint8_t>(type);
}

/// \brief Finalise and then transmit packet.
void net::Peer::packetEnd()
{
    size_t size = _packetWriter.tellp();
    assert(size <= _packet->dataLength);
    enet_packet_resize(_packet, size);
    enet_peer_send(_peer, 0, _packet);
    _packetWriter.close();
    _packet = 0;
}

/// \brief Write a generic value to packet.
/// \param value Value to write.
template<typename T>
void net::Peer::packetWrite(T value)
{
    assert(_packetWriter.is_open());

    _packetWriter << value;
}

/// \brief Write an array to packet.
/// \param array Array to write.
template<typename T, int N>
void net::Peer::packetWrite(T (&array)[N])
{
    assert(_packetWriter.is_open());

    for (int i = 0; i < N; i++) 
        _packetWriter << array[i];
}

/// \brief Write a null terminated string.
/// \param str String to write to packet.
void net::Peer::packetWrite(const char* str)
{
    assert(_packetWriter.is_open());

    uint8_t len = strlen(str);
    _packetWriter << len << str;
}

/// \brief Default handler.
void net::Peer::handlePing(uint64_t time)
{

}

/// \brief Default handler.
void net::Peer::handleKeyExchange(uint64_t key)
{

}

/// \brief Default handler.
void net::Peer::handlePlayerLogin(const char* username, MD5Hash& password)
{

}

/// \brief Construct network Interface object.
/// This constructor is intended for "client" interfaces. These interfaces only
/// need to connect to remote peers, not listen for connections themselves.
net::Interface::Interface()
{
    if ((_host = enet_host_create(0, 1, 0, 0)) == 0)
        throw NetworkException("enet_host_create failed");
}

/// \brief Construct network Interface object to listen on specified address.
/// This constructor should be used by "server" interfaces that need to listen
/// and accept remote connections.
/// \param port Local port to listen on.
/// \param addr Local IP address to listen on (defaults to all).
net::Interface::Interface(uint16_t port, uint32_t addr)
{
    ENetAddress address;
    address.host = addr;
    address.port = 12345;

    if ((_host = enet_host_create(&address, 1024, 0, 0)) == 0)
        throw NetworkException("enet_host_create failed");
}

/// \brief Cleanup network Interface object.
net::Interface::~Interface()
{
    enet_host_destroy(_host);
}

/// \brief Initiate a remote connection on this Interface.
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

/// \brief Process incoming/outgoing messages and handle connection requests.
void net::Interface::doTasks()
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

/// \brief Process an ENet connect event.
/// \param event ENet event object.
void net::Interface::eventConnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    event.peer->data = handleConnect(event.peer);
}

/// \brief Process an ENet receive event.
/// \param event ENet event object.
void net::Interface::eventReceive(ENetEvent& event)
{
    assert(event.peer->data != 0);
    Peer& peer = *reinterpret_cast<Peer*>(event.peer->data);

    ENetPacket* packet = event.packet;
    PacketReader reader(reinterpret_cast<char*>(
        packet->data), packet->dataLength);

    uint8_t type;
    reader >> type;

    switch (type) {
        case MSG_PING:
            handlePing(peer, reader);
            break;
        case MSG_KEYEXCHANGE:
            handleKeyExchange(peer, reader);
            break;
        case MSG_PLAYERLOGIN:
            handlePlayerLogin(peer, reader);
            break;
        default:
            // TODO Reply with "unknown msg".
            break;
    }

    enet_packet_destroy(event.packet);
}

/// \brief Process an ENet disconnect event.
/// \param event ENet event object.
void net::Interface::eventDisconnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    if (event.peer->data != 0) 
        handleDisconnect(reinterpret_cast<Peer*>(event.peer->data));
}

/// \brief Deserialise Ping message.
/// \param peer Remote peer message was received from.
/// \param reader Packet stream to read from.
void net::Interface::handlePing(Peer& peer, PacketReader& reader)
{
    uint64_t time;
    reader >> time;

    peer.handlePing(time);
}

/// \brief Deserialise KeyExchange message.
/// \param peer Remote peer message was received from.
/// \param reader Packet stream to read from.
void net::Interface::handleKeyExchange(Peer& peer, PacketReader& reader)
{
    uint64_t key;
    reader >> key;

    peer.handleKeyExchange(key);
}

/// \brief Deserialise PlayerLogin message.
/// \param peer Remote peer message was received from.
/// \param reader Packet stream to read from.
void net::Interface::handlePlayerLogin(Peer& peer, PacketReader& reader)
{
    StringBuf username;
    readString(reader, username);

    MD5Hash password;
    reader.read(password, sizeof(password));

    peer.handlePlayerLogin(username, password);
}

/// \brief Read a null terminated string.
/// \param reader Packet stream to read from.
/// \param buffer Buffer to read string into.
/// \return Length of string.
uint8_t net::Interface::readString(PacketReader& reader, StringBuf& buffer) const
{
    uint8_t len = 0;
    reader >> len;

    reader.read(buffer, len);
    len = reader.gcount();
    buffer[len] = 0;

    return len;
}

