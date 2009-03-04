/// \file messages.cpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 4th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "messages.hpp"
#include "msghandler.hpp"


msg::Message::~Message()
{

}


////////// msg::MsgUnitFar //////////

msg::MsgUnitFar::MsgUnitFar(int unit1, int unit2) :
    _unit1(unit1), _unit2(unit2)
{

}
msg::MsgUnitFar::~MsgUnitFar()
{

}

void msg::MsgUnitFar::dispatch(MessageHandler& handler)
{
    handler.handleMsgUnitFar(_unit1, _unit2);
}


////////// msg::MsgUnitNear //////////

msg::MsgUnitNear::MsgUnitNear(int unit1, int unit2) :
    _unit1(unit1), _unit2(unit2)
{

}
msg::MsgUnitNear::~MsgUnitNear()
{

}

void msg::MsgUnitNear::dispatch(MessageHandler& handler)
{
    handler.handleMsgUnitNear(_unit1, _unit2);
}


////////// msg::MsgUnitMove //////////

msg::MsgUnitMove::MsgUnitMove(int unit, int& pos) :
    _unit(unit), _pos(pos)
{

}
msg::MsgUnitMove::~MsgUnitMove()
{

}

void msg::MsgUnitMove::dispatch(MessageHandler& handler)
{
    handler.handleMsgUnitMove(_unit, _pos);
}


////////// msg::MsgUnitWarp //////////

msg::MsgUnitWarp::MsgUnitWarp(int unit, int& pos) :
    _unit(unit), _pos(pos)
{

}
msg::MsgUnitWarp::~MsgUnitWarp()
{

}

void msg::MsgUnitWarp::dispatch(MessageHandler& handler)
{
    handler.handleMsgUnitWarp(_unit, _pos);
}


////////// msg::MsgUnitEnter //////////

msg::MsgUnitEnter::MsgUnitEnter(int unit) :
    _unit(unit)
{

}
msg::MsgUnitEnter::~MsgUnitEnter()
{

}

void msg::MsgUnitEnter::dispatch(MessageHandler& handler)
{
    handler.handleMsgUnitEnter(_unit);
}


////////// msg::MsgUnitLeave //////////

msg::MsgUnitLeave::MsgUnitLeave(int unit) :
    _unit(unit)
{

}
msg::MsgUnitLeave::~MsgUnitLeave()
{

}

void msg::MsgUnitLeave::dispatch(MessageHandler& handler)
{
    handler.handleMsgUnitLeave(_unit);
}


