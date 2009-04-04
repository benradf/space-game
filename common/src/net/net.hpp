/// \file net.hpp
/// \brief Network module
/// \author Ben Radford 
/// \date 2nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef NET_HPP
#define NET_HPP


#include <memory>
#include <stdint.h>
#include <enet/enet.h>
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include "protocol.hpp"


namespace net {


typedef uint64_t PeerID;
typedef char MD5Hash[16];


enum MsgType {
    MSG_PING,
    MSG_KEYEXCHANGE,
    MSG_PLAYERLOGIN,
};


void initialise();
void cleanup();


/// Base object for remote Peer.
/// Users of this network module should derive their own peer objects from 
/// this one. That way they can override the message handler functions this
/// class provides and receive notification of the messages they care about.
class Peer : public virtual ProtocolUser {
    public:
        Peer(void* data);
        virtual ~Peer();

        PeerID getID() const;
        uint16_t getRemotePort() const;
        const char* getIPAddress() const;

        void disconnect(bool force = false);

    private:
        virtual void sendPacket(ENetPacket* packet);

        ENetPeer* _peer;  ///< ENet peer object.
        char _ip[16];     ///< Buffer for IP in dotted quad form.
};


/// Interface to network module.
/// This object handles messages for existing peers and remote and local 
/// connection requests. The Interface::doTasks function should be called 
/// regularly. Users should derive their own network interfaces from this
/// Interface object. This way they can receive notification of important 
/// events by implementing the pure virtual functions this class has.
class Interface {
    public:
        Interface();
        Interface(uint16_t port, uint32_t addr = ENET_HOST_ANY);
        virtual ~Interface();

        void* connect(const char* host, uint16_t port);

        bool connectionInProgress(void* handle) const;

        void doNetworkTasks();

    private:
        void eventConnect(ENetEvent& event);
        void eventReceive(ENetEvent& event);
        void eventDisconnect(ENetEvent& event);

        virtual Peer* handleConnect(void* data) = 0;
        virtual void handleDisconnect(Peer* peer) = 0;

        typedef std::tr1::unordered_set<void*> PeerSet;

        ENetHost* _host;      ///< ENetHost object for this network interface.
        PeerSet _connecting;  ///< Set of ENetPeerS that are connecting.
};


/// \return Unique ID for peer based on its address.
inline PeerID makePeerID(ENetAddress& address)
{
    return (static_cast<PeerID>(address.port) << 32) | address.host;
}


}  // namespace net


#endif  // NET_HPP

