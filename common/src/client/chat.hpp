/// \file chat.hpp
/// \brief Handles chat system.
/// \author Ben Radford 
/// \date 15th May 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef CHAT_HPP
#define CHAT_HPP


#include <queue>
#include <string>
#include <net/protocol.hpp>


class ChatSystem : public virtual net::ProtocolUser {
    public:
        ChatSystem();
        virtual ~ChatSystem();

        bool haveConsoleText() const;
        const std::string& getConsoleText() const;
        void moveToNextConsoleText();

        virtual void handleMsgPubChat(const char* text);
        virtual void handleMsgPrivChat(const char* text);
        virtual void handleMsgSystem(const char* text);
        virtual void handleMsgInfo(const char* text);

        static const int MESSAGE_HISTORY = 64;

    private:
        void addMessage(const std::string& message);
        void updateText();

        std::queue<std::string> _messages;
        std::string _text;
        bool _updated;
};


#endif  // CHAT_HPP

