/// \file msgjob.cpp
/// \brief Implements jobs that can send and receive messages.
/// \author Ben Radford 
/// \date 6th March 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#include "msgjob.hpp"


////////// MessagableJob //////////

MessagableJob::MessagableJob(PostOffice& po, int subscription)
{
    po.registerInbox(_inbox, subscription);
    po.registerOutbox(_outbox);
}

MessagableJob::~MessagableJob()
{

}

Job::RetType MessagableJob::run()
{
    while (!_inbox.empty()) 
        _inbox.get()->dispatch(*this);

    return main();
}

void MessagableJob::sendMessage(const msg::Message& msg)
{
    _outbox.put(msg);
}

MessageSender MessagableJob::newMessageSender()
{
    return MessageSender(*this);
}


////////// MessageSender //////////

MessageSender::MessageSender(MessagableJob& job) :
    _job(job)
{

}

void MessageSender::operator()(const msg::Message& msg)
{
    _job.sendMessage(msg);
}

