/// \file msghandler.cpp
/// \brief Auto-generated message handler.
/// \author Ben Radford
/// \date 25th August 2022
///
/// Copyright (c) 2022 Ben Radford.
///


#include "msghandler.hpp"


////////// msg::MessageHandler //////////

msg::MessageHandler::~MessageHandler()
{

}

void msg::MessageHandler::handleZoneTellObjectPos(PlayerID player, ObjectID object, Vector3 pos)
{

}

void msg::MessageHandler::handleZoneTellObjectAll(PlayerID player, ObjectID object, Vector3 pos, Vector3 vel, float rot, ControlState state)
{

}

void msg::MessageHandler::handleZoneSaysObjectEnter(ObjectID object)
{

}

void msg::MessageHandler::handleZoneSaysObjectLeave(ObjectID object)
{

}

void msg::MessageHandler::handleZoneSaysObjectClearClose(ObjectID object)
{

}

void msg::MessageHandler::handleZoneSaysObjectsClose(ObjectID a, ObjectID b)
{

}

void msg::MessageHandler::handleZoneSaysObjectAttach(ObjectID object, PlayerID player)
{

}

void msg::MessageHandler::handleZoneSaysObjectName(ObjectID object, const std::string& name)
{

}

void msg::MessageHandler::handleZoneSaysObjectPos(ObjectID object, Vector3 pos)
{

}

void msg::MessageHandler::handleZoneSaysObjectAll(ObjectID object, Vector3 pos, Vector3 vel, float rot, ControlState state)
{

}

void msg::MessageHandler::handlePlayerRequestZoneSwitch(PlayerID player, ZoneID zone)
{

}

void msg::MessageHandler::handlePlayerEnterZone(PlayerID player, ZoneID zone)
{

}

void msg::MessageHandler::handlePlayerLeaveZone(PlayerID player, ZoneID zone)
{

}

void msg::MessageHandler::handlePlayerName(PlayerID player, const std::string& username)
{

}

void msg::MessageHandler::handlePeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password)
{

}

void msg::MessageHandler::handlePeerRequestLogout(PeerID peer, PlayerID player)
{

}

void msg::MessageHandler::handlePeerLoginGranted(PeerID peer, PlayerID player)
{

}

void msg::MessageHandler::handlePeerLoginDenied(PeerID peer)
{

}

void msg::MessageHandler::handleChatSayPublic(PlayerID player, const std::string& text)
{

}

void msg::MessageHandler::handleChatBroadcast(const std::string& text)
{

}

