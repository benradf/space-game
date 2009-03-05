/// \file messages.cpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "messages.hpp"
#include "msghandler.hpp"


////////// msg::Message //////////

msg::Message::~Message()
{

}


////////// msg::UnitFar //////////

msg::UnitFar::UnitFar(int unit1, int unit2) :
    _unit1(unit1), _unit2(unit2)
{

}

msg::UnitFar::~UnitFar()
{

}

std::auto_ptr<msg::Message> msg::UnitFar::clone() const
{
    return std::auto_ptr<Message>(new UnitFar(*this));
}

void msg::UnitFar::dispatch(MessageHandler& handler)
{
    handler.handleUnitFar(_unit1, _unit2);
}

bool msg::UnitFar::matches(int subscription)
{
    return ((subscription & MSG_UNIT) != 0);
}


////////// msg::UnitNear //////////

msg::UnitNear::UnitNear(int unit1, int unit2) :
    _unit1(unit1), _unit2(unit2)
{

}

msg::UnitNear::~UnitNear()
{

}

std::auto_ptr<msg::Message> msg::UnitNear::clone() const
{
    return std::auto_ptr<Message>(new UnitNear(*this));
}

void msg::UnitNear::dispatch(MessageHandler& handler)
{
    handler.handleUnitNear(_unit1, _unit2);
}

bool msg::UnitNear::matches(int subscription)
{
    return ((subscription & MSG_UNIT) != 0);
}


////////// msg::UnitMove //////////

msg::UnitMove::UnitMove(int unit, int& pos) :
    _unit(unit), _pos(pos)
{

}

msg::UnitMove::~UnitMove()
{

}

std::auto_ptr<msg::Message> msg::UnitMove::clone() const
{
    return std::auto_ptr<Message>(new UnitMove(*this));
}

void msg::UnitMove::dispatch(MessageHandler& handler)
{
    handler.handleUnitMove(_unit, _pos);
}

bool msg::UnitMove::matches(int subscription)
{
    return ((subscription & MSG_UNIT) != 0);
}


////////// msg::UnitWarp //////////

msg::UnitWarp::UnitWarp(int unit, int& pos) :
    _unit(unit), _pos(pos)
{

}

msg::UnitWarp::~UnitWarp()
{

}

std::auto_ptr<msg::Message> msg::UnitWarp::clone() const
{
    return std::auto_ptr<Message>(new UnitWarp(*this));
}

void msg::UnitWarp::dispatch(MessageHandler& handler)
{
    handler.handleUnitWarp(_unit, _pos);
}

bool msg::UnitWarp::matches(int subscription)
{
    return ((subscription & MSG_UNIT) != 0);
}


////////// msg::UnitEnter //////////

msg::UnitEnter::UnitEnter(int unit) :
    _unit(unit)
{

}

msg::UnitEnter::~UnitEnter()
{

}

std::auto_ptr<msg::Message> msg::UnitEnter::clone() const
{
    return std::auto_ptr<Message>(new UnitEnter(*this));
}

void msg::UnitEnter::dispatch(MessageHandler& handler)
{
    handler.handleUnitEnter(_unit);
}

bool msg::UnitEnter::matches(int subscription)
{
    return ((subscription & MSG_UNIT) != 0);
}


////////// msg::UnitLeave //////////

msg::UnitLeave::UnitLeave(int unit) :
    _unit(unit)
{

}

msg::UnitLeave::~UnitLeave()
{

}

std::auto_ptr<msg::Message> msg::UnitLeave::clone() const
{
    return std::auto_ptr<Message>(new UnitLeave(*this));
}

void msg::UnitLeave::dispatch(MessageHandler& handler)
{
    handler.handleUnitLeave(_unit);
}

bool msg::UnitLeave::matches(int subscription)
{
    return ((subscription & MSG_UNIT) != 0);
}


