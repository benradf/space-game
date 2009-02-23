#ifndef PEER_HPP
#define PEER_HPP


class RemotePeer {
    public:

    private:
       ENetPeer* _peer; 
};


class NetworkInterface {
    public:

    private:
        ENetHost* _host;
};


#endif  // PEER_HPP

