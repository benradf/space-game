/// \file messages.hpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 4th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef MESSAGES_HPP
#define MESSAGES_HPP


namespace msg {


class MessageHandler;


class Message {
    public:
        virtual ~Message();
        virtual void dispatch(MessageHandler& handler) = 0;

    private:

};


class MsgUnitFar : public Message {
    public:
        MsgUnitFar(int unit1, int unit2);
        virtual ~MsgUnitFar();
        virtual void dispatch(MessageHandler& handler);

    private:
        int _unit1;
        int _unit2;
};


class MsgUnitNear : public Message {
    public:
        MsgUnitNear(int unit1, int unit2);
        virtual ~MsgUnitNear();
        virtual void dispatch(MessageHandler& handler);

    private:
        int _unit1;
        int _unit2;
};


class MsgUnitMove : public Message {
    public:
        MsgUnitMove(int unit, int& pos);
        virtual ~MsgUnitMove();
        virtual void dispatch(MessageHandler& handler);

    private:
        int _unit;
        int _pos;
};


class MsgUnitWarp : public Message {
    public:
        MsgUnitWarp(int unit, int& pos);
        virtual ~MsgUnitWarp();
        virtual void dispatch(MessageHandler& handler);

    private:
        int _unit;
        int _pos;
};


class MsgUnitEnter : public Message {
    public:
        MsgUnitEnter(int unit);
        virtual ~MsgUnitEnter();
        virtual void dispatch(MessageHandler& handler);

    private:
        int _unit;
};


class MsgUnitLeave : public Message {
    public:
        MsgUnitLeave(int unit);
        virtual ~MsgUnitLeave();
        virtual void dispatch(MessageHandler& handler);

    private:
        int _unit;
};


}  // namespace msg


#endif  // MESSAGES_HPP

