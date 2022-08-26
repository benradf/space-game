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
    MessagableJob(po, MSG_ZONETELL | MSG_PLAYER),
    _quadTree(vol::AABB(Vector3(-500.0f, -500.0f, -10.0f), Vector3(500.0f, 500.0f, 10.0f))),
    _physicsSystem(vol::AABB(Vector3(-500.0f, -500.0f, -10.0f), Vector3(500.0f, 500.0f, 10.0f)), "maps/base03.dat"),
    _nextObjectID(1),
    _thisZone(1)
{
    Log::log->info("creating zone");
}

Zone::~Zone()
{
    Log::log->info("freeing zone");
}

struct SendCloseMsg {
    SendCloseMsg(ObjectID id, MessageSender sender) :
        sendMessage(sender), objectID(id) {}
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

    for (auto& pair : _objectIdMap) {
        ObjectID objectID = pair.first;
        MovableObject* object = pair.second;

        sendMessage(msg::ZoneSaysObjectClearClose(objectID));

        Vector3 offset(150.0f, 150.0f, 0.0f);
        //const Vector3& pos = object->getPosition();
        SendCloseMsg visitor(objectID, newMessageSender());
        _quadTree.process(visitor/*, vol::AABB(pos - offset, pos + offset)*/);
    }

    for (auto& pair : _objectIdMap) {
        ObjectID objectID = pair.first;
        MovableObject* object = pair.second;

        object->update();

        if (sendUpdates) {
            sendMessage(msg::ZoneSaysObjectAll(objectID, 
                object->getPosition(), object->getVelocity(), 
                object->getRotation(), object->getControlState()));
        }
    }

    _physicsSystem.accumulateAndIntegrate();

    return YIELD;
}

void Zone::handlePlayerEnterZone(PlayerID player, ZoneID zone)
{
    if (zone != _thisZone) 
        return;

    ObjectID objectID = _nextObjectID++;

    auto ship = std::make_unique<Ship>(objectID);
    ship->setSystem(_physicsSystem);
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
            std::unique_ptr<sim::MovableObject>(objectIter->second);
            _objectIdMap.erase(objectIter);
        }

        sendMessage(msg::ZoneSaysObjectLeave(playerIter->second));
    }

    _playerIdMap.erase(playerIter);

    Log::log->debug("player leaves zone");
}

void Zone::handlePlayerName(PlayerID player, const std::string& username)
{
    PlayerMap::iterator playerIter = _playerIdMap.find(player);
    if (playerIter == _playerIdMap.end()) 
        return;

    sendMessage(msg::ZoneSaysObjectName(playerIter->second, username));
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
    Vector3 vel, float rot, ControlState state)
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

