/// \file messages.hpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 3th April 2009
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
    MSG_OBJECT = 0x0001,
    MSG_PEER   = 0x0002,
    MSG_PLAYER = 0x0004,
    MSG_ZONE   = 0x0008,
};


class Message {
    public:
        virtual ~Message();
        virtual std::auto_ptr<Message> clone() const = 0;
        virtual void dispatch(MessageHandler& handler) = 0;
        virtual bool matches(int subscription) = 0;

    private:

};


class ZoneEnter : public Message {
    public:
        ZoneEnter(PlayerID player, ZoneID zone);
        virtual ~ZoneEnter();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ZoneID _zone;
};


class ZoneLeave : public Message {
    public:
        ZoneLeave(PlayerID player, ZoneID zone);
        virtual ~ZoneLeave();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ZoneID _zone;
};


class ObjectState : public Message {
    public:
        ObjectState(ObjectID object, int flags);
        virtual ~ObjectState();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        int _flags;
};


class ObjectPos : public Message {
    public:
        ObjectPos(ObjectID object, Vector3 pos);
        virtual ~ObjectPos();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        Vector3 _pos;
};


class ObjectVel : public Message {
    public:
        ObjectVel(ObjectID object, Vector3 vel);
        virtual ~ObjectVel();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        Vector3 _vel;
};


class ObjectRot : public Message {
    public:
        ObjectRot(ObjectID object, Quaternion rot);
        virtual ~ObjectRot();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        Quaternion _rot;
};


class PlayerInput : public Message {
    public:
        PlayerInput(PlayerID player, ControlState state);
        virtual ~PlayerInput();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ControlState _state;
};


class PlayerRequestZoneSwitch : public Message {
    public:
        PlayerRequestZoneSwitch(PlayerID player, ZoneID zone);
        virtual ~PlayerRequestZoneSwitch();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ZoneID _zone;
};


class PeerRequestLogin : public Message {
    public:
        PeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password);
        virtual ~PeerRequestLogin();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PeerID _peer;
        const std::string _username;
        const MD5Hash _password;
};


class PeerRequestLogout : public Message {
    public:
        PeerRequestLogout(PeerID peer, PlayerID player);
        virtual ~PeerRequestLogout();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PeerID _peer;
        PlayerID _player;
};


class PeerLoginGranted : public Message {
    public:
        PeerLoginGranted(PeerID peer, PlayerID player);
        virtual ~PeerLoginGranted();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PeerID _peer;
        PlayerID _player;
};


class PeerLoginDenied : public Message {
    public:
        PeerLoginDenied(PeerID peer);
        virtual ~PeerLoginDenied();
        virtual std::auto_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PeerID _peer;
};


}  // namespace msg


#endif  // MESSAGES_HPP

