/// \file postoffice.cpp
/// \brief Message passing system.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include <assert.h>
#include <core/core.hpp>
#include "postoffice.hpp"


////////// PostOffice //////////

PostOffice::PostOffice() :
    _srcsLock(_srcs[0]), _dstsLock(_dsts[0])
{
    Log::log->info("PostOffice: message routing: startup");
}

PostOffice::~PostOffice()
{
    Log::log->info("PostOffice: message routing: shutdown");
}

Job::RetType PostOffice::run()
{
    typedef std::auto_ptr<msg::Message> MsgPtr;

    AutoWriteLock<Src> srcLock(_srcsLock);
    AutoWriteLock<Dst> dstLock(_dstsLock);

    for (int i = 0; i < NUMBOX; i++) {
        if (_srcs[i].inbox.closed()) 
            continue;

        _srcs[i].inbox.transfer();

        while (!_srcs[i].inbox.empty()) {
            MsgPtr message(_srcs[i].inbox.get());

            for (int j = 0; j < NUMBOX; j++) {
                if (message->matches(_dsts[j].subscription)) 
                    _dsts[j].outbox.put(*message);
            }
        }
    }

    for (int i = 0; i < NUMBOX; i++) {
        if (_dsts[i].outbox.closed()) 
            continue;

        _dsts[i].outbox.transfer();
    }

    return YIELD;
}

void PostOffice::registerOutbox(Outbox& outbox)
{
    AutoWriteLock<Src> srcLock(_srcsLock);

    for (int i = 0; i < NUMBOX; i++) {
        if (_srcs[i].inbox.closed()) {
            _srcs[i].inbox.connectTo(outbox);
            return;
        }
    }

    Log::log->warn("PostOffice: registered outbox limit exceeded.");

    assert(false);
}

void PostOffice::registerInbox(Inbox& inbox, int subscription)
{
    AutoWriteLock<Dst> dstLock(_dstsLock);

    for (int i = 0; i < NUMBOX; i++) {
        if (_dsts[i].outbox.closed()) {
            _dsts[i].outbox.connectTo(inbox);
            _dsts[i].subscription = subscription;
            return;
        }
    }

    Log::log->warn("PostOffice: registered inbox limit exceeded.");

    assert(false);
}

