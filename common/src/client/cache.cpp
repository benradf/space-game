/// \file cache.cpp
/// \brief Stores information received from server.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "cache.hpp"
#include <core.hpp>


using namespace sim;


////////// ObjectCache //////////

ObjectCache::ObjectCache()
{

}

ObjectCache::~ObjectCache()
{
    foreach (ObjectMap::value_type& objPair, _objects) 
        std::auto_ptr<VisibleObject>(objPair.second);
}

void ObjectCache::updateCachedObjects()
{
    foreach (ObjectMap::value_type& objPair, _objects) 
        objPair.second->update();
}

void ObjectCache::handleObjectEnter(uint32_t objectid)
{
    Log::log->debug("protocol message 'ObjectEnter' is unhandled");
}

void ObjectCache::handleObjectLeave(uint32_t objectid)
{
    Log::log->debug("protocol message 'ObjectLeave' is unhandled");
}

void ObjectCache::handleObjectPos(uint32_t objectid, float x, float y, float z)
{
    getObject(objectid).setPosition(Vector3(x, y, z));
}

void ObjectCache::handleObjectVel(uint32_t objectid, float x, float y, float z)
{
    getObject(objectid).setVelocity(Vector3(x, y, z));
}

void ObjectCache::handleObjectRot(uint32_t objectid, float w, float x, float y, float z)
{
    getObject(objectid).setRotation(Quaternion(w, x, y, z));
}

void ObjectCache::handleObjectState(uint32_t objectid, uint8_t ctrl)
{
    Log::log->debug("protocol message 'ObjectState' is unhandled");
}

void ObjectCache::handleObjectControl(uint32_t objectid, uint8_t ctrl)
{
    getObject(objectid).setControlState(ctrl);
}

VisibleObject& ObjectCache::getObject(ObjectID objectID)
{
    ObjectMap::iterator iter = _objects.find(objectID);
    if (iter != _objects.end()) 
        return *iter->second;

    std::auto_ptr<VisibleObject> object(createVisibleObject(objectID));
    _objects.insert(std::make_pair(objectID, object.get()));

    return *object.release();
}

