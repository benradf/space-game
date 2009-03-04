/// \file msghandler.hpp
/// \brief Auto-generated message handler.
/// \author Ben Radford
/// \date 4th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef MSGHANDLER_HPP
#define MSGHANDLER_HPP


namespace msg {


class MessageHandler {
    public:
        virtual void handleMsgUnitFar(int unit1, int unit2);
        virtual void handleMsgUnitNear(int unit1, int unit2);
        virtual void handleMsgUnitMove(int unit, int& pos);
        virtual void handleMsgUnitWarp(int unit, int& pos);
        virtual void handleMsgUnitEnter(int unit);
        virtual void handleMsgUnitLeave(int unit);

};


}  // namespace msg


#endif  // MSGHANDLER_HPP

