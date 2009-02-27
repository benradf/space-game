#ifndef NET_HPP
#define NET_HPP

#include <stdint.h>
#include <enet/enet.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <memory>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

#include <foreach.hpp>
#define foreach BOOST_FOREACH

typedef char MD5Hash[16];

namespace net {

enum MsgType {
    MSG_PING,
    MSG_KEYEXCHANGE,
    MSG_PLAYERLOGIN,
};

class Peer {
    public:
        Peer(void* data);
        virtual ~Peer();

        void disconnect(bool force = false);

        void sendPing(uint64_t time);
        void sendKeyExchange(uint64_t key);
        void sendPlayerLogin(const char* username, MD5Hash& password);

        virtual void handlePing(uint64_t time);
        virtual void handleKeyExchange(uint64_t key);
        virtual void handlePlayerLogin(const char* username, MD5Hash& password);

    protected:
        const char* getIPAddress() const;
        uint16_t getRemotePort() const;

    private:
        void packetBegin(MsgType type);
        void packetEnd();
        
        template<typename T>
        void packetWrite(T value);
        template<typename T, int N>
        void packetWrite(T (&array)[N]);
        void packetWrite(const char* str);

        typedef boost::iostreams::stream<boost::iostreams::array_sink> PacketWriter;

        PacketWriter _packetWriter;
        ENetPacket* _packet;
        ENetPeer* _peer;
        char _ip[16];
};

typedef uint64_t PeerID;
inline PeerID makePeerID(ENetAddress& address)
{
    return (static_cast<PeerID>(address.port) << 32) | address.host;
}

class Interface {
    public:
        Interface();
        Interface(uint16_t port, uint32_t addr = ENET_HOST_ANY);
        virtual ~Interface();

        void* connect(const char* host, uint16_t port);

        bool connectionInProgress(void* data) const;

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

        ENetHost* _host;
        PeerSet _connecting;
};

}  // namespace net

#endif  // NET_HPP

