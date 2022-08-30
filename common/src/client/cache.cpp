/// \file cache.cpp
/// \brief Stores information received from server.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "cache.hpp"
#include <core/core.hpp>
#include <net/compress.hpp>


using namespace sim;
using namespace net;


////////// ObjectCache //////////

ObjectCache::ObjectCache() :
    _lastState(0), _attachedObject(0), _haveAttachedObject(false)
{

}

ObjectCache::~ObjectCache()
{
    for (auto& objPair : _objects) 
        std::unique_ptr<VisibleObject>(objPair.second);
}

void ObjectCache::updateCachedObjects()
{
    for (auto& objPair : _objects) 
        objPair.second->update();

    updateAttachedObject();
}

void ObjectCache::setControlState(sim::ControlState state)
{
    if (!_haveAttachedObject || (state == _lastState))
        return;

    VisibleObject& object = getObject(_attachedObject);

    object.setControlState(state);
    sendFullObjectUpdate(object);

    _partialUpdateTimer.reset();
    _fullUpdateTimer.reset();
    _lastState = state;
}

const Vector3& ObjectCache::getAttachedObjectPosition() const
{
    assert(hasAttachedObject());

    const VisibleObject* object = getObject(_attachedObject);

    if (object == 0) 
        return Vector3::ZERO;

    return object->getApparentPosition();
}

const Vector3& ObjectCache::getAttachedObjectVelocity() const
{
    assert(hasAttachedObject());

    const VisibleObject* object = getObject(_attachedObject);

    if (object == 0) 
        return Vector3::ZERO;

    return object->getVelocity();
}

bool ObjectCache::hasAttachedObject() const
{
    return _haveAttachedObject;
}

void ObjectCache::handleObjectEnter(uint16_t objectid)
{
    getObject(objectid);
}

void ObjectCache::handleObjectLeave(uint16_t objectid)
{
    removeObject(objectid);
}

void ObjectCache::handleObjectAttach(uint16_t objectid)
{
    _attachedObject = objectid;
    _haveAttachedObject = true;
    getObject(_attachedObject);
}

void ObjectCache::handleObjectName(uint16_t objectid, const char* name)
{
    getObject(objectid).setName(name);
}

void ObjectCache::handleObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y)
{
    getObject(objectid).setPosition(unpackPos(makeVec2(s_x, s_y)));
}

void ObjectCache::handleObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y, 
    int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl)
{
    assert((objectid != _attachedObject) || !_haveAttachedObject); // temporary assert
    VisibleObject& object = getObject(objectid);
    object.setPosition(unpackPos(makeVec2(s_x, s_y)));
    object.setVelocity(unpackVel(makeVec2(v_x, v_y)));
    object.setRotation(unpackRot(rot));
    object.setControlState(ctrl);
}

const VisibleObject* ObjectCache::getObject(sim::ObjectID objectID) const
{
    ObjectMap::const_iterator iter = _objects.find(objectID);
    if (iter == _objects.end()) 
        return 0;

    return iter->second;
}

VisibleObject& ObjectCache::getObject(ObjectID objectID)
{
    ObjectMap::iterator iter = _objects.find(objectID);
    if (iter != _objects.end()) 
        return *iter->second;

    auto object = createVisibleObject(objectID);
    _objects.insert(std::make_pair(objectID, object.get()));

    sendGetObjectName(objectID);

    return *object.release();
}

void ObjectCache::removeObject(sim::ObjectID objectID)
{
    ObjectMap::iterator iter = _objects.find(objectID);
    if (iter == _objects.end()) 
        return;

    std::unique_ptr<VisibleObject>(iter->second);

    _objects.erase(iter);
}

void ObjectCache::sendPartialObjectUpdate(const VisibleObject& object)
{
    Vec2<int16_t> pos(packPos(object.getPosition()));

    sendObjectUpdatePartial(_attachedObject, pos.x, pos.y);
}

void ObjectCache::sendFullObjectUpdate(const VisibleObject& object)
{
    Vec2<int16_t> pos(packPos(object.getPosition()));
    Vec2<int16_t> vel(packVel(object.getVelocity()));
    uint8_t rot = packRot(object.getRotation());

    sendObjectUpdateFull(_attachedObject, pos.x, pos.y, 
        vel.x, vel.y, rot, object.getControlState());
}

void ObjectCache::updateAttachedObject()
{
    if (!hasAttachedObject()) 
        return;

    VisibleObject& object = getObject(_attachedObject);

    if (_fullUpdateTimer.elapsed() >= FULL_UPDATE_PERIOD) {
        sendFullObjectUpdate(object);
        _partialUpdateTimer.reset();
        _fullUpdateTimer.reset();
    }

    if (_partialUpdateTimer.elapsed() >= PARTIAL_UPDATE_PERIOD) {
        sendPartialObjectUpdate(object);
        _partialUpdateTimer.reset();
    }
}

