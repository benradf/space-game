#include "net.hpp"
#include <enet/enet.h>
#include <core.hpp>
#include <assert.h>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>


net::Peer::Peer(void* data)
{

}

net::Peer::~Peer()
{

}

void net::Peer::sendPing(uint64_t time)
{

}

void net::Peer::sendKeyExchange(uint64_t key)
{
    packetBegin();
    packetWrite(key);
    packetEnd();
}

void net::Peer::sendPlayerLogin(const char* username, char (&password)[16])
{
    packetBegin();
    packetWrite(username);
    packetWrite(password);
    packetEnd();
}

void net::Peer::packetBegin()
{
    _packet = enet_packet_create(0, 1024, ENET_PACKET_FLAG_RELIABLE);
    _packetWriter.open(static_cast<char*>(_packet->data), packet->dataLength);
}

void net::Peer::packetEnd()
{
    size_t size = _packetWriter.tellp();
    assert(size <= _packet.dataLength);
    enet_packet_resize(_packet, size);
    _packet.Writer.close();
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
        writer << array[i];
}

void net::Peer::packetWrite(const char* str)
{
    assert(_packetWriter.is_open());

    uint16_t len = strlen(str);
    _packetWriter << len << str;
}

void net::Peer::handlePing(uint64_t time)
{

}

void net::Peer::handleKeyExchange(uint64_t key)
{

}

void net::Peer::handlePlayerLogin(const char* username, char (&password)[16])
{

}


net::Interface::Interface()
{
    // need to do enet_initialize() outside net::Interface since we may have multiple InterfaceS

    ENetAddress address;
    address.host = ENET_HOST_ANY;
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

    PeerID id = makePeerID(event.peer->address);
    std::auto_ptr<Peer> peer = handleConnect(&event);
    _connected.insert(std::make_pair(id, peer.get()));
    peer.release();
}

void net::Interface::eventReceive(ENetEvent& event)
{
    // TODO
}

void net::Interface::eventDisconnect(ENetEvent& event)
{
    _connecting.erase(event.peer);

    PeerID id = makePeerID(event.peer->address);
    PeerMap::iterator iter = _connected.find(id);
    if (iter == _connected.end()) 
        return;

    std::auto_ptr<Peer> peer(iter->second);
    _connected.erase(iter);
    handleDisconnect(peer);
}

std::auto_ptr<net::Peer> net::Interface::handleConnect(void* data)
{
    return std::auto_ptr<Peer>(new Peer(data));
}

void net::Interface::handleDisconnect(std::auto_ptr<Peer> peer)
{
    peer.reset(0);
}


