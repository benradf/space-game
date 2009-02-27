#include "net.hpp"
#include <enet/enet.h>
#include <core.hpp>
#include <assert.h>


net::Peer::Peer(void* data) :
    _packet(0), _peer(static_cast<ENetPeer*>(data))
{
    enet_address_get_host_ip(&_peer->address, _ip, sizeof(_ip));
}

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

void net::Peer::sendPing(uint64_t time)
{
    packetBegin(MSG_PING);
    packetWrite(time);
    packetEnd();
}

void net::Peer::sendKeyExchange(uint64_t key)
{
    packetBegin(MSG_KEYEXCHANGE);
    packetWrite(key);
    packetEnd();
}

void net::Peer::sendPlayerLogin(const char* username, char (&password)[16])
{
    packetBegin(MSG_PLAYERLOGIN);
    packetWrite(username);
    packetWrite(password);
    packetEnd();
}

const char* net::Peer::getIPAddress() const
{
    return _ip;
}

uint16_t net::Peer::getRemotePort() const
{
    return _peer->address.port;
}

void net::Peer::packetBegin(MsgType type)
{
    assert(_packet == 0);
    _packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    _packetWriter.open(reinterpret_cast<char*>(_packet->data), _packet->dataLength);

    assert(_packetWriter.is_open());

    _packetWriter << static_cast<uint8_t>(type);
}

void net::Peer::packetEnd()
{
    size_t size = _packetWriter.tellp();
    assert(size <= _packet->dataLength);
    enet_packet_resize(_packet, size);
    enet_peer_send(_peer, 0, _packet);
    _packetWriter.close();
    _packet = 0;
}

template<typename T>
void net::Peer::packetWrite(T value)
{
    assert(_packetWriter.is_open());

    _packetWriter << value;
}

template<typename T, int N>
void net::Peer::packetWrite(T (&array)[N])
{
    assert(_packetWriter.is_open());

    for (int i = 0; i < N; i++) 
        _packetWriter << array[i];
}

void net::Peer::packetWrite(const char* str)
{
    assert(_packetWriter.is_open());

    uint8_t len = strlen(str);
    _packetWriter << len << str;
}

void net::Peer::handlePing(uint64_t time)
{

}

void net::Peer::handleKeyExchange(uint64_t key)
{

}

void net::Peer::handlePlayerLogin(const char* username, MD5Hash& password)
{

}


net::Interface::Interface()
{
    if ((_host = enet_host_create(0, 1, 0, 0)) == 0)
        throw NetworkException("enet_host_create failed");
}

net::Interface::Interface(uint16_t port, uint32_t addr)
{
    ENetAddress address;
    address.host = addr;
    address.port = 12345;

    if ((_host = enet_host_create(&address, 1024, 0, 0)) == 0)
        throw NetworkException("enet_host_create failed");
}

net::Interface::~Interface()
{
    enet_host_destroy(_host);
}

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

bool net::Interface::connectionInProgress(void* data) const
{
    return (_connecting.find(data) != _connecting.end());
}

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

void net::Interface::eventConnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    event.peer->data = handleConnect(event.peer);
}

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

void net::Interface::eventDisconnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    if (event.peer->data != 0) 
        handleDisconnect(reinterpret_cast<Peer*>(event.peer->data));
}

void net::Interface::handlePing(Peer& peer, PacketReader& reader)
{
    uint64_t time;
    reader >> time;

    peer.handlePing(time);
}

void net::Interface::handleKeyExchange(Peer& peer, PacketReader& reader)
{
    uint64_t key;
    reader >> key;

    peer.handleKeyExchange(key);
}

void net::Interface::handlePlayerLogin(Peer& peer, PacketReader& reader)
{
    StringBuf username;
    readString(reader, username);

    MD5Hash password;
    reader.read(password, sizeof(password));

    peer.handlePlayerLogin(username, password);
}

uint8_t net::Interface::readString(PacketReader& reader, StringBuf& buffer) const
{
    uint8_t len = 0;
    reader >> len;

    reader.read(buffer, len);
    len = reader.gcount();
    buffer[len] = 0;

    return len;
}


