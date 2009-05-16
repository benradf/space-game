/// \file zone.hpp
/// \brief Implements zones.
/// \author Ben Radford 
/// \date 29th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef ZONE_HPP
#define ZONE_HPP


#include <physics/sim.hpp>
#include <core/timer.hpp>
#include <core/foreach.hpp>
#include "msgjob.hpp"
#include <physics/quadtree.hpp>
#include "typedefs.hpp"
#include <tr1/unordered_map>
#include <physics/object.hpp>


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

    private:
        typedef std::tr1::unordered_map<ObjectID, sim::MovableObject*> ObjectMap;
        typedef std::tr1::unordered_map<PlayerID, ObjectID> PlayerMap;

        virtual void handlePlayerEnterZone(PlayerID player, ZoneID zone);
        virtual void handlePlayerLeaveZone(PlayerID player, ZoneID zone);
        virtual void handlePlayerName(PlayerID player, const std::string& username);

        virtual void handleZoneTellObjectPos(PlayerID player, ObjectID object, Vector3 pos);
        virtual void handleZoneTellObjectAll(PlayerID player, ObjectID object, Vector3 pos, 
            Vector3 vel, float rot, ControlState state);

        QuadTree<sim::MovableObject> _quadTree;

        Physics _physicsSystem;

        ObjectMap _objectIdMap;
        PlayerMap _playerIdMap;

        ObjectID _nextObjectID;
        ZoneID _thisZone;

        Timer _timer;
};




#endif  // ZONE_HPP

