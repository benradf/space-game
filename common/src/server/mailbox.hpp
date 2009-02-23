#ifndef MAILBOX_HPP
#define MAILBOX_HPP


#include "autolock.hpp"

class Message;

class Mailbox {
    public:
        Mailbox();
        Mailbox(const Mailbox& other);
        Mailbox& operator=(const Mailbox& other);
        ~Mailbox();

        void put(Message* m);
        Message* get();

    private:
        struct MailboxStorage {
            MailboxStorage();

            typedef std::vector<Message*> MsgVec;

            MsgVec _inbox;
            MsgVec _processing;
            unsigned int _processingIndex;

            Lock<MsgVec> _lock;
            unsigned int _refs;
            bool _closePending;
        };

        MailboxStorage* _storage;
};


#endif  // MAILBOX_HPP

