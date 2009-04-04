/// \file msghandler.cpp
/// \brief Auto-generated message handler.
/// \author Ben Radford
/// \date 3th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "msghandler.hpp"


////////// msg::MessageHandler //////////

msg::MessageHandler::~MessageHandler()
{

}

void msg::MessageHandler::handleZoneEnter(PlayerID player, ZoneID zone)
{

}

void msg::MessageHandler::handleZoneLeave(PlayerID player, ZoneID zone)
{

}

void msg::MessageHandler::handleObjectState(ObjectID object, int flags)
{

}

void msg::MessageHandler::handleObjectPos(ObjectID object, Vector3 pos)
{

}

void msg::MessageHandler::handleObjectVel(ObjectID object, Vector3 vel)
{

}

void msg::MessageHandler::handleObjectRot(ObjectID object, Quaternion rot)
{

}

void msg::MessageHandler::handlePlayerInput(PlayerID player, ControlState state)
{

}

void msg::MessageHandler::handlePlayerRequestZoneSwitch(PlayerID player, ZoneID zone)
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

