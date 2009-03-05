/// \file postoffice.cpp
/// \brief Message passing system.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include <assert.h>
#include <core.hpp>
#include "postoffice.hpp"


////////// PostOffice //////////

PostOffice::PostOffice()
    //_srcs(NUMBOX)//, _dsts(NUMBOX)
{
    Src src;
}

PostOffice::~PostOffice()
{

}

Job::RetType PostOffice::run()
{
    typedef std::auto_ptr<msg::Message> MsgPtr;

    foreach (Src& src, *SrcVec::LockForWrite(_srcs)) {
        if (src.inbox.empty()) 
            src.inbox.transfer();

        while (!src.inbox.empty()) {
            MsgPtr message(src.inbox.get());
            foreach (Dst& dst, *DstVec::LockForWrite(_dsts)) {
                if (message->matches(dst.subscription)) 
                    dst.outbox.put(*message);
            }
        }
    }

    return YIELD;
}

void PostOffice::registerOutbox(Outbox& outbox)
{
    foreach (Src& src, *SrcVec::LockForWrite(_srcs)) {
        if (src.inbox.closed()) {
            src.inbox.connectTo(outbox);
            return;
        }
    }

    Log::log->warn("PostOffice: registered outbox limit exceeded.");

    assert(false);
}

void PostOffice::registerInbox(Inbox& inbox, int subscription)
{
    foreach (Dst& dst, *DstVec::LockForWrite(_dsts)) {
        if (dst.outbox.closed()) {
            dst.outbox.connectTo(inbox);
            dst.subscription = subscription;
            return;
        }
    }

    Log::log->warn("PostOffice: registered inbox limit exceeded.");

    assert(false);
}

