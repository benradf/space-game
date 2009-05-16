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
    MessagableJob(po, MSG_PEER | MSG_CHAT), _nextPlayerID(1000)
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
    PlayerID tempID;
    if (lookupPlayerID(username, tempID)) {
        sendMessage(msg::PeerLoginDenied(peer));
        Log::log->info(std::string("login denied (") + username + ")");
        return;
    }

    PlayerID id = addNewPlayer(username);
    sendMessage(msg::PeerLoginGranted(peer, id));
    sendMessage(msg::PlayerEnterZone(id, 1));
    sendMessage(msg::PlayerName(id, username));

    sendMessage(msg::ChatBroadcast(std::string("* ") + username + " has joined."));

    Log::log->info(std::string("login granted (") + username + ")");
}

void LoginManager::handlePeerRequestLogout(PeerID peer, PlayerID player)
{
    sendMessage(msg::PlayerLeaveZone(player, 1));

    std::string username;
    if (!lookupUsername(player, username)) 
        return;

    sendMessage(msg::ChatBroadcast(std::string("* ") + username + " has quit."));

    removePlayer(player);

    Log::log->info(std::string("peer logout (") + username + ")");
}

void LoginManager::handleChatSayPublic(PlayerID player, const std::string& text)
{
    std::string username;
    if (!lookupUsername(player, username)) 
        return;

    sendMessage(msg::ChatBroadcast(username + ">\t " + text));

    Log::log->debug(std::string("PUBCHAT: ") + username + ">\t " + text);
}

bool LoginManager::lookupUsername(const PlayerID id, std::string& username)
{
    PlayerToUsername::iterator iter = _playerToUsername.find(id);
    if (iter == _playerToUsername.end()) 
        return false;

    username = iter->second;

    return true;
}

bool LoginManager::lookupPlayerID(const std::string& username, PlayerID& id)
{
    UsernameToPlayer::iterator iter = _usernameToPlayer.find(username);
    if (iter == _usernameToPlayer.end()) 
        return false;

    id = iter->second;

    return true;
}

PlayerID LoginManager::addNewPlayer(const std::string& username)
{
    _playerToUsername.insert(std::make_pair(_nextPlayerID, username));
    _usernameToPlayer.insert(std::make_pair(username, _nextPlayerID));

    return _nextPlayerID++;
}

void LoginManager::removePlayer(PlayerID& id)
{
    std::string username;
    if (!lookupUsername(id, username)) 
        return;

    _usernameToPlayer.erase(username);
    _playerToUsername.erase(id);
}

