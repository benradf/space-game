#include <assert.h>
#include "mailbox.hpp"




Mailbox::Mailbox() :
    _storage(new MailboxStorage)
{

}

Mailbox::Mailbox(const Mailbox& other)
{

}

Mailbox& Mailbox::operator=(const Mailbox& other)
{

}

Mailbox::~Mailbox()
{

}

void Mailbox::put(Message* m)
{
    assert(_storage != 0);

    AutoWriteLock<MailboxStorage::MsgVec> lock(_storage->_lock);
    _storage->_inbox.push_back(m);
}

Message* Mailbox::get()
{
    assert(_storage != 0);    

    AutoWriteLock<MailboxStorage::MsgVec> lock(_storage->_lock);
    //Message* m = _storage->_inbox.
    //_storage->_inbox.
}

Mailbox::MailboxStorage::MailboxStorage() :
    _refs(0), _closePending(false), _lock(_inbox)
{

}

