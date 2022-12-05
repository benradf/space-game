/// \file cache.hpp
/// \brief Stores information received from server.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef CACHE_HPP
#define CACHE_HPP


#include <net/net.hpp>
#include <physics/object.hpp>
#include <tr1/unordered_map>
#include "visobject.hpp"
#include "input.hpp"


class ObjectCache : public virtual net::ProtocolUser {
    public:
        ObjectCache();
        virtual ~ObjectCache();

        void updateCachedObjects();

        void setControlState(sim::ControlState state);
        const Vector3& getAttachedObjectPosition() const;
        const Vector3& getAttachedObjectVelocity() const;
        bool hasAttachedObject() const;

    private:
        static const int FULL_UPDATE_PERIOD = 5000000;
        static const int PARTIAL_UPDATE_PERIOD = 1000000;

        virtual void handleObjectEnter(uint16_t objectid);
        virtual void handleObjectLeave(uint16_t objectid);
        virtual void handleObjectAttach(uint16_t objectid);
        virtual void handleObjectName(uint16_t objectid, const char* name);
        virtual void handleObjectUpdatePartial(uint16_t objectid, int16_t s_x, int16_t s_y);
        virtual void handleObjectUpdateFull(uint16_t objectid, int16_t s_x, int16_t s_y,
            int16_t v_x, int16_t v_y, uint8_t rot, uint8_t ctrl);

        typedef std::tr1::unordered_map<sim::ObjectID, VisibleObject*> ObjectMap;

        const VisibleObject* getObject(sim::ObjectID objectID) const;
        VisibleObject& getObject(sim::ObjectID objectID);
        void removeObject(sim::ObjectID objectID);

        void sendPartialObjectUpdate(const VisibleObject& object);
        void sendFullObjectUpdate(const VisibleObject& object);
        void updateAttachedObject();

        Timer _fullUpdateTimer;
        Timer _partialUpdateTimer;
        sim::ControlState _lastState;
        sim::ObjectID _attachedObject;
        bool _haveAttachedObject;

        ObjectMap _objects;
};


#endif  // CACHE_HPP

