/// \file autolock.hpp
/// \brief Provides locks using the RAII idiom.
/// \author Ben Radford 
/// \date 1st October 2007
///
/// Copyright (c) 2007 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):
/// - 17/05/08 Refactored code into seperate read and write locks.


#ifndef AUTOLOCK_HPP
#define AUTOLOCK_HPP


#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <list>
#include <set>

#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include "lock.hpp"


// TODO: Implement a lockless queue using CAS algorithms.

// NOTE: Need to be careful of accessing an object while it is being destructed.
// Probably the best solution is to hold the write lock while the object is
// destroyed.

// NOTE: Further to the above, accessing an object after is destroyed is 
// undefined already. Therefore we can just extend this to access while the
// destructor is running. Such a circumstance only arises in a multithreaded
// environment.


template<typename T>
class AutoReadLock {
    public:
        AutoReadLock(Lock<T>& lock);
        ~AutoReadLock();
        
        T* operator->();
        T& operator*();
        
    private:
        AutoReadLock(const AutoReadLock&);
        AutoReadLock& operator=(const AutoReadLock&);
        
        Lock<T>& _lock;
};


template<typename T>
class AutoWriteLock {
    public:
        AutoWriteLock(Lock<T>& lock);
        ~AutoWriteLock();
        
        T* operator->();
        T& operator*();

        void resetLock();
        
    private:
        AutoWriteLock(const AutoWriteLock&);
        AutoWriteLock& operator=(const AutoWriteLock&);
        
        Lock<T>& _lock;
};


template<typename T>
struct Lockable {
    template<typename S>
    class Template {
        public:
            Template();

            struct LockForRead : public AutoReadLock<S> {
                LockForRead(Template& obj);
            };

            struct LockForWrite : public AutoWriteLock<S> {
                LockForWrite(Template& obj);
            };

        private:
            Template(const Template&);
            Template& operator=(const Template&);

            S _struct;
            Lock<S> _lock;
    };

    typedef Template<std::vector<T> > Vector;
    typedef Template<std::deque<T> > Deque;
    typedef Template<std::queue<T> > Queue;
    typedef Template<std::stack<T> > Stack;
    typedef Template<std::list<T> > List;
    typedef Template<std::set<T> > Set;

    typedef Template<std::tr1::unordered_set<T> > UnorderedSet;
};


////////// AutoReadLock //////////

template<typename T>
inline AutoReadLock<T>::AutoReadLock(Lock<T>& lock)
    : _lock(lock)
{
    _lock.roWaitLock();
}

template<typename T>
inline AutoReadLock<T>::~AutoReadLock()
{
    _lock.roUnlock();
}

template<typename T>
inline T* AutoReadLock<T>::operator->()
{
    return &_lock.getObject();
}

template<typename T>
inline T& AutoReadLock<T>::operator*()
{
    return _lock.getObject();
}


////////// AutoWriteLock //////////

template<typename T>
inline AutoWriteLock<T>::AutoWriteLock(Lock<T>& lock)
    : _lock(lock)
{
    _lock.rwWaitLock();
}

template<typename T>
inline AutoWriteLock<T>::~AutoWriteLock()
{
    _lock.rwUnlock();
}

template<typename T>
inline T* AutoWriteLock<T>::operator->()
{
    return &_lock.getObject();
}

template<typename T>
inline T& AutoWriteLock<T>::operator*()
{
    return _lock.getObject();
}


////////// ALQueue //////////

template<typename T>
template<typename S>
inline Lockable<T>::Template<S>::Template()
    : _lock(_struct)
{
    
}

template<typename T>
template<typename S>
inline Lockable<T>::Template<S>::LockForRead::LockForRead(Template& obj)
    : AutoReadLock<S>(obj._lock)
{
    
}

template<typename T>
template<typename S>
inline Lockable<T>::Template<S>::LockForWrite::LockForWrite(Template& obj)
    : AutoWriteLock<S>(obj._lock)
{
    
}


#endif  // AUTOLOCK_HPP

