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
class Peer {
    public:
        Peer(void* data);
        virtual ~Peer();

        PeerID getID() const;
        uint16_t getRemotePort() const;
        const char* getIPAddress() const;

        void disconnect(bool force = false);

        void sendPing(uint64_t time);
        void sendKeyExchange(uint64_t key);
        void sendPlayerLogin(const char* username, MD5Hash& password);

        virtual void handlePing(uint64_t time);
        virtual void handleKeyExchange(uint64_t key);
        virtual void handlePlayerLogin(const char* username, MD5Hash& password);

    private:
        void packetBegin(MsgType type);
        void packetEnd();
        
        template<typename T>
        void packetWrite(T value);
        template<typename T, int N>
        void packetWrite(T (&array)[N]);
        void packetWrite(const char* str);

        typedef boost::iostreams::stream<boost::iostreams::array_sink> PacketWriter;

        PacketWriter _packetWriter;  ///< Packet stream for writing new packet.
        ENetPacket* _packet;         ///< ENet packet object to be sent.
        ENetPeer* _peer;             ///< ENet peer object.
        char _ip[16];                ///< Buffer for IP in dotted quad form.
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

        void doTasks();
        
    private:
        typedef char StringBuf[256];
        typedef std::tr1::unordered_set<void*> PeerSet;
        typedef boost::iostreams::stream<
            boost::iostreams::array_source> PacketReader;

        void eventConnect(ENetEvent& event);
        void eventReceive(ENetEvent& event);
        void eventDisconnect(ENetEvent& event);

        void handlePing(Peer& peer, PacketReader& reader);
        void handleKeyExchange(Peer& peer, PacketReader& reader);
        void handlePlayerLogin(Peer& peer, PacketReader& reader);

        virtual Peer* handleConnect(void* data) = 0;
        virtual void handleDisconnect(Peer* peer) = 0;

        uint8_t readString(PacketReader& reader, StringBuf& buffer) const;

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

