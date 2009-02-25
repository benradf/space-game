#ifndef NET_HPP
#define NET_HPP

#include <stdint.h>
#include <enet/enet.h>
#include <tr1/unordered_map>
#include <memory>

namespace net {

class Peer {
    public:
        Peer(void* data);
        virtual ~Peer();

        void sendPing(uint64_t time);
        void sendKeyExchange(uint64_t key);
        void sendPlayerLogin(const char* username, char (&password)[16]);

    private:
        virtual void handlePing(uint64_t time);
        virtual void handleKeyExchange(uint64_t key);
        virtual void handlePlayerLogin(const char* username, char (&password)[16]);
};

typedef uint32_t PeerID;

class Interface {
    public:
        Interface();
        virtual ~Interface();

        void doTasks();
        
    private:
        void eventConnect(ENetEvent& event);
        void eventReceive(ENetEvent& event);
        void eventDisconnect(ENetEvent& event);

        virtual std::auto_ptr<Peer> handleConnect(void* data);
        virtual void handleDisconnect(std::auto_ptr<Peer> peer);

        ENetAddress _address;
        ENetHost* _host;

        typedef std::tr1::unordered_map<PeerID, Peer*> PeerMap;
        PeerMap _peers;
};

}  // namespace net

#endif  // NET_HPP

