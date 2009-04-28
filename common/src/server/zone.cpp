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
    MessagableJob(po, MSG_ZONETELL | MSG_PLAYER), _nextObjectID(1), _thisZone(1),
    _quadTree(vol::AABB(Vector3(-500.0f, -500.0f, -10.0f), Vector3(500.0f, 500.0f, 10.0f)))
{
    Log::log->info("creating zone");
}

Zone::~Zone()
{
    Log::log->info("freeing zone");
}

struct SendCloseMsg {
    SendCloseMsg(ObjectID id, MessageSender sender) :
        objectID(id), sendMessage(sender) {}
    void visit(const sim::MovableObject* object) {
        if (objectID != object->getID())
            sendMessage(msg::ZoneSaysObjectsClose(objectID, object->getID()));
    }
    MessageSender sendMessage;
    ObjectID objectID;
};

Zone::RetType Zone::main()
{
    bool sendUpdates = (_timer.elapsed() > 500000);
    if (sendUpdates)
        _timer.reset();

    foreach (ObjectMap::value_type& pair, _objectIdMap) {
        ObjectID objectID = pair.first;
        MovableObject* object = pair.second;

        sendMessage(msg::ZoneSaysObjectClearClose(objectID));

        Vector3 offset(150.0f, 150.0f, 0.0f);
        const Vector3& pos = object->getPosition();
        SendCloseMsg visitor(objectID, newMessageSender());
        _quadTree.process(visitor, vol::AABB(pos - offset, pos + offset));
    }

    foreach (ObjectMap::value_type& pair, _objectIdMap) {
        ObjectID objectID = pair.first;
        MovableObject* object = pair.second;

        object->update();

        if (sendUpdates) {
            sendMessage(msg::ZoneSaysObjectAll(objectID, 
                object->getPosition(), object->getVelocity(), 
                object->getRotation(), object->getControlState()));
        }
    }

    return YIELD;
}

void Zone::handlePlayerEnterZone(PlayerID player, ZoneID zone)
{
    if (zone != _thisZone) 
        return;

    ObjectID objectID = _nextObjectID++;

    std::auto_ptr<Ship> ship(new Ship(objectID));
    MovableObject* object = ship.get();
    _objectIdMap.insert(std::make_pair(objectID, object));
    ship.release();

    _playerIdMap.insert(std::make_pair(player, objectID));
    _quadTree.insert(object);

    sendMessage(msg::ZoneSaysObjectAttach(objectID, player));

    //object->setControlState(CTRL_LEFT | CTRL_THRUST | CTRL_BOOST);
    Log::log->debug("player enters zone");
}

void Zone::handlePlayerLeaveZone(PlayerID player, ZoneID zone)
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

void Zone::handleZoneTellObjectPos(PlayerID player, ObjectID object, Vector3 pos)
{
    //TODO: Verify position is within error margin of prediction position.

    PlayerMap::iterator playerIter = _playerIdMap.find(player);
    if (playerIter == _playerIdMap.end()) 
        return;

    if (playerIter->second != object) {
        Log::log->warn("player not authorised to control this object!");
        return;
    }

    ObjectMap::iterator objectIter = _objectIdMap.find(object);
    if (objectIter == _objectIdMap.end()) 
        return;

    objectIter->second->setPosition(pos);
}

void Zone::handleZoneTellObjectAll(PlayerID player, ObjectID object, Vector3 pos, 
    Vector3 vel, Quaternion rot, ControlState state)
{
    //TODO: Verify position is within error margin of prediction position.

    PlayerMap::iterator playerIter = _playerIdMap.find(player);
    if (playerIter == _playerIdMap.end()) 
        return;

    if (playerIter->second != object) {
        Log::log->warn("player not authorised to control this object!");
        return;
    }

    ObjectMap::iterator objectIter = _objectIdMap.find(object);
    if (objectIter == _objectIdMap.end()) 
        return;

    objectIter->second->setPosition(pos);
    objectIter->second->setVelocity(vel);
    objectIter->second->setRotation(rot);
    objectIter->second->setControlState(state);
}

