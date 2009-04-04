/// \file messages.cpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 3th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "messages.hpp"
#include "msghandler.hpp"


////////// msg::Message //////////

msg::Message::~Message()
{

}


////////// msg::ZoneEnter //////////

msg::ZoneEnter::ZoneEnter(PlayerID player, ZoneID zone) :
    _player(player), _zone(zone)
{

}

msg::ZoneEnter::~ZoneEnter()
{

}

std::auto_ptr<msg::Message> msg::ZoneEnter::clone() const
{
    return std::auto_ptr<Message>(new ZoneEnter(*this));
}

void msg::ZoneEnter::dispatch(MessageHandler& handler)
{
    handler.handleZoneEnter(_player, _zone);
}

bool msg::ZoneEnter::matches(int subscription)
{
    return ((subscription & MSG_ZONE) != 0);
}


////////// msg::ZoneLeave //////////

msg::ZoneLeave::ZoneLeave(PlayerID player, ZoneID zone) :
    _player(player), _zone(zone)
{

}

msg::ZoneLeave::~ZoneLeave()
{

}

std::auto_ptr<msg::Message> msg::ZoneLeave::clone() const
{
    return std::auto_ptr<Message>(new ZoneLeave(*this));
}

void msg::ZoneLeave::dispatch(MessageHandler& handler)
{
    handler.handleZoneLeave(_player, _zone);
}

bool msg::ZoneLeave::matches(int subscription)
{
    return ((subscription & MSG_ZONE) != 0);
}


////////// msg::ObjectState //////////

msg::ObjectState::ObjectState(ObjectID object, int flags) :
    _object(object), _flags(flags)
{

}

msg::ObjectState::~ObjectState()
{

}

std::auto_ptr<msg::Message> msg::ObjectState::clone() const
{
    return std::auto_ptr<Message>(new ObjectState(*this));
}

void msg::ObjectState::dispatch(MessageHandler& handler)
{
    handler.handleObjectState(_object, _flags);
}

bool msg::ObjectState::matches(int subscription)
{
    return ((subscription & MSG_OBJECT) != 0);
}


////////// msg::ObjectPos //////////

msg::ObjectPos::ObjectPos(ObjectID object, Vector3 pos) :
    _object(object), _pos(pos)
{

}

msg::ObjectPos::~ObjectPos()
{

}

std::auto_ptr<msg::Message> msg::ObjectPos::clone() const
{
    return std::auto_ptr<Message>(new ObjectPos(*this));
}

void msg::ObjectPos::dispatch(MessageHandler& handler)
{
    handler.handleObjectPos(_object, _pos);
}

bool msg::ObjectPos::matches(int subscription)
{
    return ((subscription & MSG_OBJECT) != 0);
}


////////// msg::ObjectVel //////////

msg::ObjectVel::ObjectVel(ObjectID object, Vector3 vel) :
    _object(object), _vel(vel)
{

}

msg::ObjectVel::~ObjectVel()
{

}

std::auto_ptr<msg::Message> msg::ObjectVel::clone() const
{
    return std::auto_ptr<Message>(new ObjectVel(*this));
}

void msg::ObjectVel::dispatch(MessageHandler& handler)
{
    handler.handleObjectVel(_object, _vel);
}

bool msg::ObjectVel::matches(int subscription)
{
    return ((subscription & MSG_OBJECT) != 0);
}


////////// msg::ObjectRot //////////

msg::ObjectRot::ObjectRot(ObjectID object, Quaternion rot) :
    _object(object), _rot(rot)
{

}

msg::ObjectRot::~ObjectRot()
{

}

std::auto_ptr<msg::Message> msg::ObjectRot::clone() const
{
    return std::auto_ptr<Message>(new ObjectRot(*this));
}

void msg::ObjectRot::dispatch(MessageHandler& handler)
{
    handler.handleObjectRot(_object, _rot);
}

bool msg::ObjectRot::matches(int subscription)
{
    return ((subscription & MSG_OBJECT) != 0);
}


////////// msg::PlayerInput //////////

msg::PlayerInput::PlayerInput(PlayerID player, ControlState state) :
    _player(player), _state(state)
{

}

msg::PlayerInput::~PlayerInput()
{

}

std::auto_ptr<msg::Message> msg::PlayerInput::clone() const
{
    return std::auto_ptr<Message>(new PlayerInput(*this));
}

void msg::PlayerInput::dispatch(MessageHandler& handler)
{
    handler.handlePlayerInput(_player, _state);
}

bool msg::PlayerInput::matches(int subscription)
{
    return ((subscription & MSG_PLAYER) != 0);
}


////////// msg::PlayerRequestZoneSwitch //////////

msg::PlayerRequestZoneSwitch::PlayerRequestZoneSwitch(PlayerID player, ZoneID zone) :
    _player(player), _zone(zone)
{

}

msg::PlayerRequestZoneSwitch::~PlayerRequestZoneSwitch()
{

}

std::auto_ptr<msg::Message> msg::PlayerRequestZoneSwitch::clone() const
{
    return std::auto_ptr<Message>(new PlayerRequestZoneSwitch(*this));
}

void msg::PlayerRequestZoneSwitch::dispatch(MessageHandler& handler)
{
    handler.handlePlayerRequestZoneSwitch(_player, _zone);
}

bool msg::PlayerRequestZoneSwitch::matches(int subscription)
{
    return ((subscription & MSG_PLAYER) != 0);
}


////////// msg::PeerRequestLogin //////////

msg::PeerRequestLogin::PeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password) :
    _peer(peer), _username(username), _password(password)
{

}

msg::PeerRequestLogin::~PeerRequestLogin()
{

}

std::auto_ptr<msg::Message> msg::PeerRequestLogin::clone() const
{
    return std::auto_ptr<Message>(new PeerRequestLogin(*this));
}

void msg::PeerRequestLogin::dispatch(MessageHandler& handler)
{
    handler.handlePeerRequestLogin(_peer, _username, _password);
}

bool msg::PeerRequestLogin::matches(int subscription)
{
    return ((subscription & MSG_PEER) != 0);
}


////////// msg::PeerRequestLogout //////////

msg::PeerRequestLogout::PeerRequestLogout(PeerID peer, PlayerID player) :
    _peer(peer), _player(player)
{

}

msg::PeerRequestLogout::~PeerRequestLogout()
{

}

std::auto_ptr<msg::Message> msg::PeerRequestLogout::clone() const
{
    return std::auto_ptr<Message>(new PeerRequestLogout(*this));
}

void msg::PeerRequestLogout::dispatch(MessageHandler& handler)
{
    handler.handlePeerRequestLogout(_peer, _player);
}

bool msg::PeerRequestLogout::matches(int subscription)
{
    return ((subscription & MSG_PEER) != 0);
}


////////// msg::PeerLoginGranted //////////

msg::PeerLoginGranted::PeerLoginGranted(PeerID peer, PlayerID player) :
    _peer(peer), _player(player)
{

}

msg::PeerLoginGranted::~PeerLoginGranted()
{

}

std::auto_ptr<msg::Message> msg::PeerLoginGranted::clone() const
{
    return std::auto_ptr<Message>(new PeerLoginGranted(*this));
}

void msg::PeerLoginGranted::dispatch(MessageHandler& handler)
{
    handler.handlePeerLoginGranted(_peer, _player);
}

bool msg::PeerLoginGranted::matches(int subscription)
{
    return ((subscription & MSG_PEER) != 0);
}


////////// msg::PeerLoginDenied //////////

msg::PeerLoginDenied::PeerLoginDenied(PeerID peer) :
    _peer(peer)
{

}

msg::PeerLoginDenied::~PeerLoginDenied()
{

}

std::auto_ptr<msg::Message> msg::PeerLoginDenied::clone() const
{
    return std::auto_ptr<Message>(new PeerLoginDenied(*this));
}

void msg::PeerLoginDenied::dispatch(MessageHandler& handler)
{
    handler.handlePeerLoginDenied(_peer);
}

bool msg::PeerLoginDenied::matches(int subscription)
{
    return ((subscription & MSG_PEER) != 0);
}


