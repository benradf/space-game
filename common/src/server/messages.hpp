/// \file messages.hpp
/// \brief Auto-generated message definitions.
/// \author Ben Radford
/// \date 25th August 2022
///
/// Copyright (c) 2022 Ben Radford.
///


#ifndef MESSAGES_HPP
#define MESSAGES_HPP


#include <memory>
#include "typedefs.hpp"


namespace msg {


class MessageHandler;


enum MsgType {
    MSG_CHAT     = 0x0001,
    MSG_PEER     = 0x0002,
    MSG_PLAYER   = 0x0004,
    MSG_ZONESAYS = 0x0008,
    MSG_ZONETELL = 0x0010,
};


class Message {
    public:
        virtual ~Message();
        virtual std::unique_ptr<Message> clone() const = 0;
        virtual void dispatch(MessageHandler& handler) = 0;
        virtual bool matches(int subscription) = 0;

    private:

};


class ZoneTellObjectPos : public Message {
    public:
        ZoneTellObjectPos(PlayerID player, ObjectID object, Vector3 pos);
        virtual ~ZoneTellObjectPos();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ObjectID _object;
        Vector3 _pos;
};


class ZoneTellObjectAll : public Message {
    public:
        ZoneTellObjectAll(PlayerID player, ObjectID object, Vector3 pos, Vector3 vel, float rot, ControlState state);
        virtual ~ZoneTellObjectAll();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ObjectID _object;
        Vector3 _pos;
        Vector3 _vel;
        float _rot;
        ControlState _state;
};


class ZoneSaysObjectEnter : public Message {
    public:
        ZoneSaysObjectEnter(ObjectID object);
        virtual ~ZoneSaysObjectEnter();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
};


class ZoneSaysObjectLeave : public Message {
    public:
        ZoneSaysObjectLeave(ObjectID object);
        virtual ~ZoneSaysObjectLeave();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
};


class ZoneSaysObjectClearClose : public Message {
    public:
        ZoneSaysObjectClearClose(ObjectID object);
        virtual ~ZoneSaysObjectClearClose();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
};


class ZoneSaysObjectsClose : public Message {
    public:
        ZoneSaysObjectsClose(ObjectID a, ObjectID b);
        virtual ~ZoneSaysObjectsClose();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _a;
        ObjectID _b;
};


class ZoneSaysObjectAttach : public Message {
    public:
        ZoneSaysObjectAttach(ObjectID object, PlayerID player);
        virtual ~ZoneSaysObjectAttach();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        PlayerID _player;
};


class ZoneSaysObjectName : public Message {
    public:
        ZoneSaysObjectName(ObjectID object, const std::string& name);
        virtual ~ZoneSaysObjectName();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        const std::string _name;
};


class ZoneSaysObjectPos : public Message {
    public:
        ZoneSaysObjectPos(ObjectID object, Vector3 pos);
        virtual ~ZoneSaysObjectPos();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        Vector3 _pos;
};


class ZoneSaysObjectAll : public Message {
    public:
        ZoneSaysObjectAll(ObjectID object, Vector3 pos, Vector3 vel, float rot, ControlState state);
        virtual ~ZoneSaysObjectAll();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        ObjectID _object;
        Vector3 _pos;
        Vector3 _vel;
        float _rot;
        ControlState _state;
};


class PlayerRequestZoneSwitch : public Message {
    public:
        PlayerRequestZoneSwitch(PlayerID player, ZoneID zone);
        virtual ~PlayerRequestZoneSwitch();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ZoneID _zone;
};


class PlayerEnterZone : public Message {
    public:
        PlayerEnterZone(PlayerID player, ZoneID zone);
        virtual ~PlayerEnterZone();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ZoneID _zone;
};


class PlayerLeaveZone : public Message {
    public:
        PlayerLeaveZone(PlayerID player, ZoneID zone);
        virtual ~PlayerLeaveZone();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        ZoneID _zone;
};


class PlayerName : public Message {
    public:
        PlayerName(PlayerID player, const std::string& username);
        virtual ~PlayerName();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        const std::string _username;
};


class PeerRequestLogin : public Message {
    public:
        PeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password);
        virtual ~PeerRequestLogin();
        virtual std::unique_ptr<Message> clone() const;
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
        virtual std::unique_ptr<Message> clone() const;
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
        virtual std::unique_ptr<Message> clone() const;
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
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PeerID _peer;
};


class ChatSayPublic : public Message {
    public:
        ChatSayPublic(PlayerID player, const std::string& text);
        virtual ~ChatSayPublic();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        PlayerID _player;
        const std::string _text;
};


class ChatBroadcast : public Message {
    public:
        ChatBroadcast(const std::string& text);
        virtual ~ChatBroadcast();
        virtual std::unique_ptr<Message> clone() const;
        virtual void dispatch(MessageHandler& handler);
        virtual bool matches(int subscription);

    private:
        const std::string _text;
};


}  // namespace msg


#endif  // MESSAGES_HPP

