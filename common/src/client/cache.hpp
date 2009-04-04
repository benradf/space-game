/// \file cache.hpp
/// \brief Stores information received from server.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef CACHE_HPP
#define CACHE_HPP


#include <net.hpp>
#include <object.hpp>
#include <tr1/unordered_map>
#include "visobject.hpp"


class ObjectCache : public virtual net::ProtocolUser {
    public:
        ObjectCache();
        virtual ~ObjectCache();

        void updateCachedObjects();

    private:
        virtual void handleObjectEnter(uint32_t objectid);
        virtual void handleObjectLeave(uint32_t objectid);
        virtual void handleObjectPos(uint32_t objectid, float x, float y, float z);
        virtual void handleObjectVel(uint32_t objectid, float x, float y, float z);
        virtual void handleObjectRot(uint32_t objectid, float w, float x, float y, float z);
        virtual void handleObjectState(uint32_t objectid, uint8_t ctrl);
        virtual void handleObjectControl(uint32_t objectid, uint8_t ctrl);

        typedef std::tr1::unordered_map<sim::ObjectID, VisibleObject*> ObjectMap;

        VisibleObject& getObject(sim::ObjectID objectID);

        ObjectMap _objects;
};


#endif  // CACHE_HPP

