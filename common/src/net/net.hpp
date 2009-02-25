#ifndef NET_HPP
#define NET_HPP

#include <stdint.h>
#include <enet/enet.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <memory>

namespace net {

class Peer {
    public:
        Peer(void* data);
        virtual ~Peer();

        void disconnect();

        void sendPing(uint64_t time);
        void sendKeyExchange(uint64_t key);
        void sendPlayerLogin(const char* username, char (&password)[16]);

    private:
        void packetBegin();
        void packetEnd();
        
        template<typename T>
        void packetWrite(T value);
        template<typename T, int N>
        void packetWrite(T (&array)[N]);
        void packetWrite(const char* str);

        ENetPacket* _packet;
        PacketWriter _packetWriter;

        virtual void handlePing(uint64_t time);
        virtual void handleKeyExchange(uint64_t key);
        virtual void handlePlayerLogin(const char* username, char (&password)[16]);

        typedef boost::iostreams::stream<boost::iostreams::array_sink> PacketWriter;
};

typedef uint64_t PeerID;
inline PeerID makePeerID(ENetAddress& address)
{
    return (static_cast<PeerID>(address.port) << 32) | address.host;
}

class Interface {
    public:
        Interface();
        virtual ~Interface();

        void* connect(const char* host, uint16_t port);

        bool connectionInProgress(void* data) const;

        void doTasks();
        
    private:
        void eventConnect(ENetEvent& event);
        void eventReceive(ENetEvent& event);
        void eventDisconnect(ENetEvent& event);

        virtual std::auto_ptr<Peer> handleConnect(void* data);
        virtual void handleDisconnect(std::auto_ptr<Peer> peer);

        typedef std::tr1::unordered_map<PeerID, Peer*> PeerMap;
        typedef std::tr1::unordered_set<void*> PeerSet;

        ENetHost* _host;
        PeerMap _connected;
        PeerSet _connecting;
};

}  // namespace net

#endif  // NET_HPP

