/// \file zone.hpp
/// \brief Implements zones.
/// \author Ben Radford 
/// \date 29th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef ZONE_HPP
#define ZONE_HPP


#include <sim/sim.hpp>
#include <core/timer.hpp>
#include <core/foreach.hpp>
#include "msgjob.hpp"
#include "quadtree.hpp"
#include "typedefs.hpp"
#include <tr1/unordered_map>
#include <sim/object.hpp>


typedef uint32_t ZoneID;


struct ZoneInfo {
    std::string name;
    bool loaded;
};

class ZoneLoader : public MessagableJob {
    public:
        virtual RetType main();

    private:

};

class Zone : public MessagableJob {
    public:
        Zone(PostOffice& po);
        virtual ~Zone();

        virtual RetType main();

        virtual void handleZoneEnter(PlayerID player, ZoneID zone);
        virtual void handleZoneLeave(PlayerID player, ZoneID zone);
        virtual void handlePlayerInput(PlayerID player, ControlState state);

    private:
        typedef std::tr1::unordered_map<ObjectID, sim::MovableObject*> ObjectMap;
        typedef std::tr1::unordered_map<PlayerID, ObjectID> PlayerMap;

        QuadTree<sim::MovableObject> _quadTree;

        ObjectMap _objectIdMap;
        PlayerMap _playerIdMap;

        ObjectID _nextObjectID;
        ZoneID _thisZone;

        Timer _timer;
};




#endif  // ZONE_HPP

