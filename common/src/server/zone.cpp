/// \file zone.cpp
/// \brief Implements zones.
/// \author Ben Radford 
/// \date 29th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "zone.hpp"
#include "messages.hpp"


using namespace msg;
using namespace sim;


Zone::Zone(PostOffice& po) :
    MessagableJob(po, MSG_ZONE), _nextObjectID(1), _thisZone(1),
    _quadTree(vol::AABB(Vector3(-500.0f, -500.0f, -10.0f), Vector3(500.0f, 500.0f, 10.0f)))
{
    Log::log->info("creating zone");
}

Zone::~Zone()
{
    Log::log->info("freeing zone");
}

Zone::RetType Zone::main()
{
    foreach (ObjectMap::value_type& pair, _objectIdMap) {
        ObjectID objectID = pair.first;
        MovableObject* object = pair.second;

        object->update();

        sendMessage(msg::ObjectPos(objectID, object->getPosition()));
        sendMessage(msg::ObjectVel(objectID, object->getVelocity()));
        sendMessage(msg::ObjectRot(objectID, object->getRotation()));
    }

    return YIELD;
}

void Zone::handleZoneEnter(PlayerID player, ZoneID zone)
{
    if (zone != _thisZone) 
        return;

    std::auto_ptr<Ship> ship(new Ship);
    MovableObject* object = ship.get();
    _objectIdMap.insert(std::make_pair(_nextObjectID, object));
    ship.release();

    _playerIdMap.insert(std::make_pair(player, _nextObjectID));
    _quadTree.insert(object);
    _nextObjectID++;

    object->setControlState(CTRL_LEFT);
    Log::log->debug("player enters zone");
}

void Zone::handleZoneLeave(PlayerID player, ZoneID zone)
{
    if (zone != _thisZone) 
        return;

    PlayerMap::iterator playerIter = _playerIdMap.find(player);

    if (playerIter == _playerIdMap.end()) 
        return;

    if (playerIter->second != 0) {
        ObjectMap::iterator objectIter = 
            _objectIdMap.find(playerIter->second);

        if (objectIter != _objectIdMap.end()) {
            _quadTree.remove(objectIter->second);
            delete objectIter->second;
        }
    }

    _playerIdMap.erase(playerIter);

    Log::log->debug("player leaves zone");
}

void Zone::handlePlayerInput(PlayerID player, ControlState state)
{
    PlayerMap::iterator playerIter = _playerIdMap.find(player);
    if (playerIter == _playerIdMap.end()) 
        return;

    if (playerIter->second == 0) 
        return;

    ObjectMap::iterator objectIter = _objectIdMap.find(playerIter->second);
    assert(objectIter != _objectIdMap.end());

    objectIter->second->setControlState(state);

    Log::log->debug("player sends input");
}






















