/// \file ship.hpp
/// \brief Implementation of space ships.
/// \author Ben Radford 
/// \date 28th July 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef SHIP_HPP
#define SHIP_HPP


#include "object.hpp"


namespace physics {


typedef uint32_t ShipID;
typedef uint32_t ControlState;


enum STAT {
    STAT_THRUST,
    STAT_BOOST,
    STAT_ROTSPEED,
    STAT_MAXSPEED,
    STAT_COUNT
};


enum Control {
    CTRL_THRUST = 0x00000001,
    CTRL_BOOST  = 0x00000002,
    CTRL_LEFT   = 0x00000004,
    CTRL_RIGHT  = 0x00000008,
    CTRL_CANNON = 0x00000010,
};


class Ship : public GameObject<Ship> {
    public:
        Ship(ShipID id);
        virtual ~Ship();

        ShipID getID() const;
        virtual void updateState();

        ControlState getControlState() const;
        void setControlState(ControlState control);

        virtual void collision(Ship& ship);
        virtual void collision(Missile& missile);
        virtual void collision(const Vector3& normal);

        void setSystem(Physics& system);

    private:
        ControlState _control;
        Timer _timer;

        ShipID _id;

        float _stats[STAT_COUNT];
};


inline bool controlIsOn(Control control, ControlState state)
{
    return ((state & control) != 0);
}

inline ControlState controlSet(Control control, bool enable, ControlState state)
{
    return (enable ? state | control : state & ~control);
}


}  // namespace physics


#endif  // SHIP_HPP

