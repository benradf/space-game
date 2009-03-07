/// \file msgjob.hpp
/// \brief Implements jobs that can send and receive messages.
/// \author Ben Radford 
/// \date 6th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef MSGJOB_HPP
#define MSGJOB_HPP


#include "msghandler.hpp"
#include "postoffice.hpp"
#include "concurrency.hpp"


class MessagableJob : public Job, public msg::MessageHandler {
    public:
        MessagableJob(PostOffice& po, int subscription);
        virtual ~MessagableJob();

        virtual RetType run();
        virtual RetType main() = 0;

    private:
        Inbox _inbox;
        Outbox _outbox;
};


#endif  // MSGJOB_HPP

