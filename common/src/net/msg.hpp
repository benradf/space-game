#ifndef MSG_HPP
#define MSG_HPP


class MessageLayer {
    public:
        bool handleMessage(void* buffer, size_t size) const;
        
    private:
        enum MsgType {
            KEYEXCHANGE,
            PLAYERLOGIN,
        };
};




#endif  // MSG_HPP

