/// \file messages.cpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 25th August 2022
///
/// Copyright (c) 2022 Ben Radford.
///


#include "messages.hpp"
#include "msghandler.hpp"


////////// msg::Message //////////

msg::Message::~Message()
{

}


////////// msg::ZoneTellObjectPos //////////

msg::ZoneTellObjectPos::ZoneTellObjectPos(PlayerID player, ObjectID object, Vector3 pos) :
    _player(player), _object(object), _pos(pos)
{

}

msg::ZoneTellObjectPos::~ZoneTellObjectPos()
{

}

std::unique_ptr<msg::Message> msg::ZoneTellObjectPos::clone() const
{
    return std::unique_ptr<Message>(new ZoneTellObjectPos(*this));
}

void msg::ZoneTellObjectPos::dispatch(MessageHandler& handler)
{
    handler.handleZoneTellObjectPos(_player, _object, _pos);
}

bool msg::ZoneTellObjectPos::matches(int subscription)
{
    return ((subscription & MSG_ZONETELL) != 0);
}


////////// msg::ZoneTellObjectAll //////////

msg::ZoneTellObjectAll::ZoneTellObjectAll(PlayerID player, ObjectID object, Vector3 pos, Vector3 vel, float rot, ControlState state) :
    _player(player), _object(object), _pos(pos), _vel(vel), _rot(rot), _state(state)
{

}

msg::ZoneTellObjectAll::~ZoneTellObjectAll()
{

}

std::unique_ptr<msg::Message> msg::ZoneTellObjectAll::clone() const
{
    return std::unique_ptr<Message>(new ZoneTellObjectAll(*this));
}

void msg::ZoneTellObjectAll::dispatch(MessageHandler& handler)
{
    handler.handleZoneTellObjectAll(_player, _object, _pos, _vel, _rot, _state);
}

bool msg::ZoneTellObjectAll::matches(int subscription)
{
    return ((subscription & MSG_ZONETELL) != 0);
}


////////// msg::ZoneSaysObjectEnter //////////

msg::ZoneSaysObjectEnter::ZoneSaysObjectEnter(ObjectID object) :
    _object(object)
{

}

msg::ZoneSaysObjectEnter::~ZoneSaysObjectEnter()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectEnter::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectEnter(*this));
}

void msg::ZoneSaysObjectEnter::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectEnter(_object);
}

bool msg::ZoneSaysObjectEnter::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectLeave //////////

msg::ZoneSaysObjectLeave::ZoneSaysObjectLeave(ObjectID object) :
    _object(object)
{

}

msg::ZoneSaysObjectLeave::~ZoneSaysObjectLeave()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectLeave::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectLeave(*this));
}

void msg::ZoneSaysObjectLeave::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectLeave(_object);
}

bool msg::ZoneSaysObjectLeave::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectClearClose //////////

msg::ZoneSaysObjectClearClose::ZoneSaysObjectClearClose(ObjectID object) :
    _object(object)
{

}

msg::ZoneSaysObjectClearClose::~ZoneSaysObjectClearClose()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectClearClose::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectClearClose(*this));
}

void msg::ZoneSaysObjectClearClose::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectClearClose(_object);
}

bool msg::ZoneSaysObjectClearClose::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectsClose //////////

msg::ZoneSaysObjectsClose::ZoneSaysObjectsClose(ObjectID a, ObjectID b) :
    _a(a), _b(b)
{

}

msg::ZoneSaysObjectsClose::~ZoneSaysObjectsClose()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectsClose::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectsClose(*this));
}

void msg::ZoneSaysObjectsClose::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectsClose(_a, _b);
}

bool msg::ZoneSaysObjectsClose::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectAttach //////////

msg::ZoneSaysObjectAttach::ZoneSaysObjectAttach(ObjectID object, PlayerID player) :
    _object(object), _player(player)
{

}

msg::ZoneSaysObjectAttach::~ZoneSaysObjectAttach()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectAttach::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectAttach(*this));
}

void msg::ZoneSaysObjectAttach::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectAttach(_object, _player);
}

bool msg::ZoneSaysObjectAttach::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectName //////////

msg::ZoneSaysObjectName::ZoneSaysObjectName(ObjectID object, const std::string& name) :
    _object(object), _name(name)
{

}

msg::ZoneSaysObjectName::~ZoneSaysObjectName()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectName::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectName(*this));
}

void msg::ZoneSaysObjectName::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectName(_object, _name);
}

bool msg::ZoneSaysObjectName::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectPos //////////

msg::ZoneSaysObjectPos::ZoneSaysObjectPos(ObjectID object, Vector3 pos) :
    _object(object), _pos(pos)
{

}

msg::ZoneSaysObjectPos::~ZoneSaysObjectPos()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectPos::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectPos(*this));
}

void msg::ZoneSaysObjectPos::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectPos(_object, _pos);
}

bool msg::ZoneSaysObjectPos::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::ZoneSaysObjectAll //////////

msg::ZoneSaysObjectAll::ZoneSaysObjectAll(ObjectID object, Vector3 pos, Vector3 vel, float rot, ControlState state) :
    _object(object), _pos(pos), _vel(vel), _rot(rot), _state(state)
{

}

msg::ZoneSaysObjectAll::~ZoneSaysObjectAll()
{

}

std::unique_ptr<msg::Message> msg::ZoneSaysObjectAll::clone() const
{
    return std::unique_ptr<Message>(new ZoneSaysObjectAll(*this));
}

void msg::ZoneSaysObjectAll::dispatch(MessageHandler& handler)
{
    handler.handleZoneSaysObjectAll(_object, _pos, _vel, _rot, _state);
}

bool msg::ZoneSaysObjectAll::matches(int subscription)
{
    return ((subscription & MSG_ZONESAYS) != 0);
}


////////// msg::PlayerRequestZoneSwitch //////////

msg::PlayerRequestZoneSwitch::PlayerRequestZoneSwitch(PlayerID player, ZoneID zone) :
    _player(player), _zone(zone)
{

}

msg::PlayerRequestZoneSwitch::~PlayerRequestZoneSwitch()
{

}

std::unique_ptr<msg::Message> msg::PlayerRequestZoneSwitch::clone() const
{
    return std::unique_ptr<Message>(new PlayerRequestZoneSwitch(*this));
}

void msg::PlayerRequestZoneSwitch::dispatch(MessageHandler& handler)
{
    handler.handlePlayerRequestZoneSwitch(_player, _zone);
}

bool msg::PlayerRequestZoneSwitch::matches(int subscription)
{
    return ((subscription & MSG_PLAYER) != 0);
}


////////// msg::PlayerEnterZone //////////

msg::PlayerEnterZone::PlayerEnterZone(PlayerID player, ZoneID zone) :
    _player(player), _zone(zone)
{

}

msg::PlayerEnterZone::~PlayerEnterZone()
{

}

std::unique_ptr<msg::Message> msg::PlayerEnterZone::clone() const
{
    return std::unique_ptr<Message>(new PlayerEnterZone(*this));
}

void msg::PlayerEnterZone::dispatch(MessageHandler& handler)
{
    handler.handlePlayerEnterZone(_player, _zone);
}

bool msg::PlayerEnterZone::matches(int subscription)
{
    return ((subscription & MSG_PLAYER) != 0);
}


////////// msg::PlayerLeaveZone //////////

msg::PlayerLeaveZone::PlayerLeaveZone(PlayerID player, ZoneID zone) :
    _player(player), _zone(zone)
{

}

msg::PlayerLeaveZone::~PlayerLeaveZone()
{

}

std::unique_ptr<msg::Message> msg::PlayerLeaveZone::clone() const
{
    return std::unique_ptr<Message>(new PlayerLeaveZone(*this));
}

void msg::PlayerLeaveZone::dispatch(MessageHandler& handler)
{
    handler.handlePlayerLeaveZone(_player, _zone);
}

bool msg::PlayerLeaveZone::matches(int subscription)
{
    return ((subscription & MSG_PLAYER) != 0);
}


////////// msg::PlayerName //////////

msg::PlayerName::PlayerName(PlayerID player, const std::string& username) :
    _player(player), _username(username)
{

}

msg::PlayerName::~PlayerName()
{

}

std::unique_ptr<msg::Message> msg::PlayerName::clone() const
{
    return std::unique_ptr<Message>(new PlayerName(*this));
}

void msg::PlayerName::dispatch(MessageHandler& handler)
{
    handler.handlePlayerName(_player, _username);
}

bool msg::PlayerName::matches(int subscription)
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

std::unique_ptr<msg::Message> msg::PeerRequestLogin::clone() const
{
    return std::unique_ptr<Message>(new PeerRequestLogin(*this));
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

std::unique_ptr<msg::Message> msg::PeerRequestLogout::clone() const
{
    return std::unique_ptr<Message>(new PeerRequestLogout(*this));
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

std::unique_ptr<msg::Message> msg::PeerLoginGranted::clone() const
{
    return std::unique_ptr<Message>(new PeerLoginGranted(*this));
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

std::unique_ptr<msg::Message> msg::PeerLoginDenied::clone() const
{
    return std::unique_ptr<Message>(new PeerLoginDenied(*this));
}

void msg::PeerLoginDenied::dispatch(MessageHandler& handler)
{
    handler.handlePeerLoginDenied(_peer);
}

bool msg::PeerLoginDenied::matches(int subscription)
{
    return ((subscription & MSG_PEER) != 0);
}


////////// msg::ChatSayPublic //////////

msg::ChatSayPublic::ChatSayPublic(PlayerID player, const std::string& text) :
    _player(player), _text(text)
{

}

msg::ChatSayPublic::~ChatSayPublic()
{

}

std::unique_ptr<msg::Message> msg::ChatSayPublic::clone() const
{
    return std::unique_ptr<Message>(new ChatSayPublic(*this));
}

void msg::ChatSayPublic::dispatch(MessageHandler& handler)
{
    handler.handleChatSayPublic(_player, _text);
}

bool msg::ChatSayPublic::matches(int subscription)
{
    return ((subscription & MSG_CHAT) != 0);
}


////////// msg::ChatBroadcast //////////

msg::ChatBroadcast::ChatBroadcast(const std::string& text) :
    _text(text)
{

}

msg::ChatBroadcast::~ChatBroadcast()
{

}

std::unique_ptr<msg::Message> msg::ChatBroadcast::clone() const
{
    return std::unique_ptr<Message>(new ChatBroadcast(*this));
}

void msg::ChatBroadcast::dispatch(MessageHandler& handler)
{
    handler.handleChatBroadcast(_text);
}

bool msg::ChatBroadcast::matches(int subscription)
{
    return ((subscription & MSG_CHAT) != 0);
}


