/// \file msghandler.hpp
/// \brief Auto-generated message handler.
/// \author Ben Radford
/// \date 28th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef MSGHANDLER_HPP
#define MSGHANDLER_HPP


#include "typedefs.hpp"


namespace msg {


class MessageHandler {
    public:
        virtual ~MessageHandler();
        virtual void handleZoneTellObjectPos(PlayerID player, ObjectID object, Vector3 pos);
        virtual void handleZoneTellObjectAll(PlayerID player, ObjectID object, Vector3 pos, Vector3 vel, Quaternion rot, ControlState state);
        virtual void handleZoneSaysObjectEnter(ObjectID object);
        virtual void handleZoneSaysObjectLeave(ObjectID object);
        virtual void handleZoneSaysObjectClearClose(ObjectID object);
        virtual void handleZoneSaysObjectsClose(ObjectID a, ObjectID b);
        virtual void handleZoneSaysObjectAttach(ObjectID object, PlayerID player);
        virtual void handleZoneSaysObjectPos(ObjectID object, Vector3 pos);
        virtual void handleZoneSaysObjectAll(ObjectID object, Vector3 pos, Vector3 vel, Quaternion rot, ControlState state);
        virtual void handlePlayerRequestZoneSwitch(PlayerID player, ZoneID zone);
        virtual void handlePlayerEnterZone(PlayerID player, ZoneID zone);
        virtual void handlePlayerLeaveZone(PlayerID player, ZoneID zone);
        virtual void handlePeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password);
        virtual void handlePeerRequestLogout(PeerID peer, PlayerID player);
        virtual void handlePeerLoginGranted(PeerID peer, PlayerID player);
        virtual void handlePeerLoginDenied(PeerID peer);
};


}  // namespace msg


#endif  // MSGHANDLER_HPP

