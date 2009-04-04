/// \file player.hpp
/// \brief Handles player related stuff.
/// \author Ben Radford 
/// \date 2nd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef PLAYER_HPP
#define PLAYER_HPP


#include "msgjob.hpp"


typedef uint64_t PlayerID;


class LoginManager : public MessagableJob {
    public:
        LoginManager(PostOffice& po);
        virtual ~LoginManager();

        virtual RetType main();

    private:
        virtual void handlePeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password);
        virtual void handlePeerRequestLogout(PeerID peer, PlayerID player);

        PlayerID _nextPlayerID;
};



#endif  // PLAYER_HPP

