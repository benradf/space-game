/// \file postoffice.hpp
/// \brief Message passing system.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef POSTOFFICE_HPP
#define POSTOFFICE_HPP


#include "fifo.hpp"
#include "autolock.hpp"
#include "messages.hpp"
#include "concurrency.hpp"


typedef fifo::Get<msg::Message> Inbox;
typedef fifo::Put<msg::Message> Outbox;


class PostOffice : public Job {
    public:
        PostOffice();
        virtual ~PostOffice();

        virtual RetType run();

        virtual void registerOutbox(Outbox& outbox);
        virtual void registerInbox(Inbox& inbox, int subscription);

    private:
        struct Src {
            Inbox inbox;
        };

        struct Dst {
            Outbox outbox;
            int subscription;
        };

        static const int NUMBOX = 3;

        Src _srcs[NUMBOX];
        Dst _dsts[NUMBOX];

        Lock<Src> _srcsLock;
        Lock<Dst> _dstsLock;
};



#endif  // POSTOFFICE_HPP

