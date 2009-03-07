/// \file msgjob.cpp
/// \brief Implements jobs that can send and receive messages.
/// \author Ben Radford 
/// \date 6th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
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

