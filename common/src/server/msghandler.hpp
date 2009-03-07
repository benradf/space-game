/// \file msghandler.hpp
/// \brief Auto-generated message handler.
/// \author Ben Radford
/// \date 7th March 2009
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
        virtual void handleUnitFar(int unit1, int unit2);
        virtual void handleUnitNear(int unit1, int unit2);
        virtual void handleUnitMove(int unit, int& pos);
        virtual void handleUnitWarp(int unit, int& pos);
        virtual void handleUnitEnter(int unit);
        virtual void handleUnitLeave(int unit);
};


}  // namespace msg


#endif  // MSGHANDLER_HPP

