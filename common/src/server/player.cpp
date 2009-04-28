/// \file player.cpp
/// \brief Handles player related stuff.
/// \author Ben Radford 
/// \date 2nd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "player.hpp"
#include "messages.hpp"


using namespace msg;


LoginManager::LoginManager(PostOffice& po) :
    MessagableJob(po, MSG_PEER), _nextPlayerID(1000)
{

}

LoginManager::~LoginManager()
{

}

LoginManager::RetType LoginManager::main()
{
    return YIELD;
}

void LoginManager::handlePeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password)
{
    sendMessage(msg::PeerLoginGranted(peer, _nextPlayerID));
    sendMessage(msg::PlayerEnterZone(_nextPlayerID, 1));
    _nextPlayerID++;

    Log::log->debug("handlePeerRequestLogin");
}

void LoginManager::handlePeerRequestLogout(PeerID peer, PlayerID player)
{
    sendMessage(msg::PlayerLeaveZone(player, 1));

    Log::log->debug("handlePeerRequestLogout");
}

