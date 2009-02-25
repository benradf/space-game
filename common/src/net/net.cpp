#include "net.hpp"
#include <enet/enet.h>
#include <core.hpp>
#include <assert.h>


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

}

void net::Peer::sendPlayerLogin(const char* username, char (&password)[16])
{

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

    _address.host = ENET_HOST_ANY;
    _address.port = 12345;

    if ((_host = enet_host_create(&_address, 1024, 0, 0)) == 0)
        throw NetworkException("failed to initialise enet");


}

net::Interface::~Interface()
{
    enet_host_destroy(_host);
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
    PeerID id = event.peer->address.host;
    std::auto_ptr<Peer> peer = handleConnect(&event);
    _peers.insert(std::make_pair(id, peer.get()));
    peer.release();
}

void net::Interface::eventReceive(ENetEvent& event)
{
    // TODO
}

void net::Interface::eventDisconnect(ENetEvent& event)
{
    PeerID id = event.peer->address.host;
    PeerMap::iterator iter = _peers.find(id);
    assert(iter != _peers.end());

    std::auto_ptr<Peer> peer(iter->second);
    _peers.erase(iter);

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


