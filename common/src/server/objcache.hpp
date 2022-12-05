/// \file objcache.hpp
/// \brief Caches object updates from zone.
/// \author Ben Radford 
/// \date 27th April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef OBJCACHE_HPP
#define OBJCACHE_HPP


#include "typedefs.hpp"
#include "msghandler.hpp"
#include <tr1/unordered_set>
#include <tr1/unordered_map>


typedef std::tr1::unordered_set<ObjectID> ObjectSet;
typedef std::tr1::unordered_map<PlayerID, ObjectID> ObjectMap;


class CachedObjectInfo {
    public:
        explicit CachedObjectInfo(ObjectID id);

        void setRotation(float rotation);
        void setPosition(const Vector3& pos);
        void setVelocity(const Vector3& vel);
        void setControlState(sim::ControlState state);

        float getRotation() const;
        const Vector3& getPosition() const;
        const Vector3& getVelocity() const;
        sim::ControlState getControlState() const;

        void clearCloseObjects();
        void setCloseTo(ObjectID object);
        const ObjectSet& getCloseObjects() const;

        void attachPlayer(PlayerID player);
        PlayerID getAttachedPlayer() const;

        void setName(const std::string& name);
        const std::string& getName() const;

        ObjectID getID() const;

    private:
        ObjectID _id;
        std::string _name;

        float _rotation;
        Vector3 _position;
        Vector3 _velocity;
        sim::ControlState _state;

        ObjectSet _closeObjects;
        PlayerID _attachedPlayer;
};


class ObjectCache : public virtual msg::MessageHandler {
    public:
        virtual ~ObjectCache();

        const CachedObjectInfo* getCachedObjectInfo(ObjectID id) const;

    private:
        typedef std::tr1::unordered_map<ObjectID, CachedObjectInfo*> ObjectMap;

        virtual void tellPlayerObjectPos(PlayerID player, const CachedObjectInfo& object) = 0;
        virtual void tellPlayerObjectAll(PlayerID player, const CachedObjectInfo& object) = 0;
        virtual void tellPlayerObjectAttach(PlayerID player, ObjectID object) = 0;
        virtual void tellPlayerObjectLeave(PlayerID player, ObjectID object) = 0;

        virtual void handleZoneSaysObjectEnter(ObjectID object);
        virtual void handleZoneSaysObjectLeave(ObjectID object);
        virtual void handleZoneSaysObjectClearClose(ObjectID object);
        virtual void handleZoneSaysObjectsClose(ObjectID a, ObjectID b);
        virtual void handleZoneSaysObjectAttach(ObjectID object, PlayerID player);
        virtual void handleZoneSaysObjectName(ObjectID object, const std::string& name);
        virtual void handleZoneSaysObjectPos(ObjectID object, Vector3 pos);
        virtual void handleZoneSaysObjectAll(ObjectID object, Vector3 pos,
            Vector3 vel, float rot, ControlState state);

        CachedObjectInfo& getObjectInfo(ObjectID id);
        void removeObjectInfo(ObjectID id);

        ObjectMap _objects;
};


////////// CachedObjectInfo //////////

inline CachedObjectInfo::CachedObjectInfo(ObjectID id) :
    _id(id), _rotation(0.0f), _position(Vector3::ZERO), 
    _velocity(Vector3::ZERO), _state(0), _attachedPlayer(0)
{

}

inline void CachedObjectInfo::setRotation(float rotation)
{
    _rotation = rotation;
}

inline void CachedObjectInfo::setPosition(const Vector3& pos)
{
    _position = pos;
}

inline void CachedObjectInfo::setVelocity(const Vector3& vel)
{
    _velocity = vel;
}

inline void CachedObjectInfo::setControlState(sim::ControlState state)
{
    _state = state;
}

inline float CachedObjectInfo::getRotation() const
{
    return _rotation;
}

inline const Vector3& CachedObjectInfo::getPosition() const
{
    return _position;
}

inline const Vector3& CachedObjectInfo::getVelocity() const
{
    return _velocity;
}

inline sim::ControlState CachedObjectInfo::getControlState() const
{
    return _state;
}

inline void CachedObjectInfo::clearCloseObjects()
{
    _closeObjects.clear();
}

inline void CachedObjectInfo::setCloseTo(ObjectID object)
{
    _closeObjects.insert(object);
}

inline const ObjectSet& CachedObjectInfo::getCloseObjects() const
{
    return _closeObjects;
}

inline void CachedObjectInfo::attachPlayer(PlayerID player)
{
    _attachedPlayer = player;
}

inline PlayerID CachedObjectInfo::getAttachedPlayer() const
{
    return _attachedPlayer;
}

inline void CachedObjectInfo::setName(const std::string& name)
{
    _name = name;
}

inline const std::string& CachedObjectInfo::getName() const
{
    return _name;
}

inline ObjectID CachedObjectInfo::getID() const
{
    return _id;
}


#endif  // OBJCACHE_HPP

