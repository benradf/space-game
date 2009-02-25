#ifndef NET_HPP
#define NET_HPP

namespace net {

class Peer {
    public:
        Peer(void* data);
        virtual ~Peer();

        void sendPing(uint64_t time);
        void sendKeyExchange(uint64_t key);
        void sendPlayerLogin(const char* username, char (&password)[16])

    private:
        virtual void handlePing(uint64_t time);
        virtual void handleKeyExchange(uint64_t key);
        virtual void handlePlayerLogin(const char* username, char (&password)[16]);
};

class Interface {
    public:
        void doTasks();
        
    private:
        virtual Peer* connected(void* data);
        virtual void disconnected(Peer* peer);

};

}  // namespace net

#endif  // NET_HPP

