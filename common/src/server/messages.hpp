/// \file messages.hpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 7th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef MESSAGES_HPP
#define MESSAGES_HPP


#include <memory>
#include "typedefs.hpp"


namespace msg {


class MessageHandler;


enum MsgType {
    MSG_UNIT = 0x0001,
};


class Message {
    public:
        virtual ~Message();
        virtual std::auto_ptr<Message> clone() const = 0;
        virtual void dispatch(MessageHandler& handler) = 0;
        virtual bool matches(int subscription) = 0;

    private:

};


class UnitFar : public Message {
    public:
        UnitFar(int unit1, int unit2);
        virtual ~UnitFar();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        int _unit1;
        int _unit2;
};


class UnitNear : public Message {
    public:
        UnitNear(int unit1, int unit2);
        virtual ~UnitNear();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        int _unit1;
        int _unit2;
};


class UnitMove : public Message {
    public:
        UnitMove(int unit, int& pos);
        virtual ~UnitMove();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        int _unit;
        int _pos;
};


class UnitWarp : public Message {
    public:
        UnitWarp(int unit, int& pos);
        virtual ~UnitWarp();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        int _unit;
        int _pos;
};


class UnitEnter : public Message {
    public:
        UnitEnter(int unit);
        virtual ~UnitEnter();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        int _unit;
};


class UnitLeave : public Message {
    public:
        UnitLeave(int unit);
        virtual ~UnitLeave();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        int _unit;
};


}  // namespace msg


#endif  // MESSAGES_HPP

