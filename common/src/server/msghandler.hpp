/// \file msghandler.hpp
/// \brief Auto-generated message handler.
/// \author Ben Radford
/// \date 4th April 2009
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
        virtual void handleZoneEnter(PlayerID player, ZoneID zone);
        virtual void handleZoneLeave(PlayerID player, ZoneID zone);
        virtual void handleObjectState(ObjectID object, int flags);
        virtual void handleObjectPos(ObjectID object, Vector3 pos);
        virtual void handleObjectVel(ObjectID object, Vector3 vel);
        virtual void handleObjectRot(ObjectID object, Quaternion rot);
        virtual void handleObjectAssoc(ObjectID object, PlayerID player);
        virtual void handlePlayerInput(PlayerID player, ControlState state);
        virtual void handlePlayerRequestZoneSwitch(PlayerID player, ZoneID zone);
        virtual void handlePeerRequestLogin(PeerID peer, const std::string& username, const MD5Hash& password);
        virtual void handlePeerRequestLogout(PeerID peer, PlayerID player);
        virtual void handlePeerLoginGranted(PeerID peer, PlayerID player);
        virtual void handlePeerLoginDenied(PeerID peer);
};


}  // namespace msg


#endif  // MSGHANDLER_HPP

