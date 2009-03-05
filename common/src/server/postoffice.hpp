/// \file postoffice.hpp
/// \brief Message passing system.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef POSTOFFICE_HPP
#define POSTOFFICE_CPP


#include "fifo.hpp"
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

        static const int NUMBOX = 32;

        std::vector<Src> _srcs;
        std::vector<Dst> _dsts;
};



#endif  // POSTOFFICE_HPP

