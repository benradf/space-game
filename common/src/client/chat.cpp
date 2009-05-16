/// \file chat.cpp
/// \brief Handles chat system.
/// \author Ben Radford 
/// \date 15th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "chat.hpp"
#include <core/core.hpp>


////////// ChatSystem //////////

ChatSystem::ChatSystem() :
    _updated(false)
{

}

ChatSystem::~ChatSystem()
{

}

bool ChatSystem::haveConsoleText() const
{
    return (!_messages.empty());
}

const std::string& ChatSystem::getConsoleText() const
{
    return _messages.front();
}

void ChatSystem::moveToNextConsoleText()
{
    _messages.pop();
}

void ChatSystem::handleMsgPubChat(const char* text)
{
    _messages.push(text);
}

void ChatSystem::handleMsgPrivChat(const char* text)
{

}

void ChatSystem::handleMsgSystem(const char* text)
{

}

void ChatSystem::handleMsgInfo(const char* text)
{

}

