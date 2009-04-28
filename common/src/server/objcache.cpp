/// \file objcache.cpp
/// \brief Caches information about objects.
/// \author Ben Radford 
/// \date 27th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "objcache.hpp"
#include <core/core.hpp>


////////// ObjectCache //////////


ObjectCache::~ObjectCache()
{
    foreach (ObjectMap::value_type& object, _objects) 
        std::auto_ptr<CachedObjectInfo>(object.second);
}

void ObjectCache::handleZoneSaysObjectEnter(ObjectID object)
{
    getObjectInfo(object);
}

void ObjectCache::handleZoneSaysObjectLeave(ObjectID object)
{
    CachedObjectInfo& info = getObjectInfo(object);

    foreach (ObjectID id, info.getCloseObjects()) {
        CachedObjectInfo& obj = getObjectInfo(id);
        tellPlayerObjectLeave(obj.getAttachedPlayer(), object);
    }

    removeObjectInfo(object);
}

void ObjectCache::handleZoneSaysObjectClearClose(ObjectID object)
{
    getObjectInfo(object).clearCloseObjects();
}

void ObjectCache::handleZoneSaysObjectsClose(ObjectID a, ObjectID b)
{
    getObjectInfo(a).setCloseTo(b);
}

void ObjectCache::handleZoneSaysObjectAttach(ObjectID object, PlayerID player)
{
    getObjectInfo(object).attachPlayer(player);

    tellPlayerObjectAttach(player, object);
}

void ObjectCache::handleZoneSaysObjectPos(ObjectID object, Vector3 pos)
{
    CachedObjectInfo& info = getObjectInfo(object);

    info.setPosition(pos);

    foreach (ObjectID id, info.getCloseObjects()) {
        CachedObjectInfo& obj = getObjectInfo(id);
        tellPlayerObjectPos(obj.getAttachedPlayer(), info);
    }
}

void ObjectCache::handleZoneSaysObjectAll(ObjectID object, Vector3 pos,
     Vector3 vel, Quaternion rot, ControlState state)
{
    CachedObjectInfo& info = getObjectInfo(object);

    info.setPosition(pos);
    info.setVelocity(vel);
    info.setRotation(rot);
    info.setControlState(state);

    foreach (ObjectID id, info.getCloseObjects()) {
        CachedObjectInfo& obj = getObjectInfo(id);
        tellPlayerObjectAll(obj.getAttachedPlayer(), info);
    }
}

CachedObjectInfo& ObjectCache::getObjectInfo(ObjectID id)
{
    ObjectMap::iterator iter = _objects.find(id);
    if (iter != _objects.end()) 
        return *iter->second;

    std::auto_ptr<CachedObjectInfo> info(new CachedObjectInfo(id));
    _objects.insert(std::make_pair(id, info.get()));

    return *info.release();
}

void ObjectCache::removeObjectInfo(ObjectID id)
{
    ObjectMap::iterator iter = _objects.find(id);
    if (iter == _objects.end()) 
        return;

    std::auto_ptr<CachedObjectInfo>(iter->second);

    _objects.erase(id);
}

