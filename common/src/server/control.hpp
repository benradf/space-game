/// \file control.hpp
/// \brief Provides a means to control server.
/// \author Ben Radford 
/// \date 8nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef CONTROL_HPP
#define CONTROL_HPP


#include "msgjob.hpp"


class ControlCentre : public MessagableJob {
    public:
        ControlCentre();
        virtual ~ControlCentre();

        virtual RetType main();

    private:
        void parseCommand(const char* string);
};


#endif  // CONTROL_HPP

