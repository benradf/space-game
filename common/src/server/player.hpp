/// \file player.hpp
/// \brief Handles player related stuff.
/// \author Ben Radford 
/// \date 2nd April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef PLAYER_HPP
#define PLAYER_HPP


#include "msgjob.hpp"
#include <tr1/unordered_map>


typedef uint64_t PlayerID;


class LoginManager : public MessagableJob {
    public:
        LoginManager(PostOffice& po);
        virtual ~LoginManager();

        virtual RetType main();

    private:
        virtual void handlePeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password);
        virtual void handlePeerRequestLogout(PeerID peer, PlayerID player);

        virtual void handleChatSayPublic(PlayerID player, const std::string& text);

        typedef std::tr1::unordered_map<PlayerID, std::string> PlayerToUsername;
        typedef std::tr1::unordered_map<std::string, PlayerID> UsernameToPlayer;

        bool lookupPlayerID(const std::string& username, PlayerID& id);
        bool lookupUsername(PlayerID id, std::string& username);
        PlayerID addNewPlayer(const std::string& username);
        void removePlayer(PlayerID& id);

        PlayerToUsername _playerToUsername;
        UsernameToPlayer _usernameToPlayer;

        PlayerID _nextPlayerID;
};



#endif  // PLAYER_HPP

