/// \file fifo.hpp
/// Generic FIFO queue.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef FIFO_HPP
#define FIFO_HPP


#include <memory>
#include <assert.h>
#include <core.hpp>
#include "autolock.hpp"


namespace fifo {


template<typename T> class Put;
template<typename T> class Get;


static const int FIFOSIZE = 10;  ///< Space to reserve on each end of FIFO.


/// Writable end of a FIFO pipe.
/// Each end of a FIFO pipe exists independently of the other. This allows 
/// separate ownership and lifetimes. An actual FIFO pipe is formed by 
/// connecting a fifo::Put and a fifo::Get together (see Put::connectTo). When 
/// one end of an established FIFO pipe is destroyed the pipe is automatically 
/// broken and all objects in transit are deleted. The remaining end may be 
/// reused as part of another FIFO pipe.
template<typename T>
class Put {
    public:
        friend class Get<T>;

        Put();
        virtual ~Put();

        void clear();
        void transfer();
        void put(T& object);
        void connectTo(Get<T>& get);
        bool closed() const;
        bool empty() const;

    private:
        void disconnect();

        typedef typename Lockable<T*>::Vector Vector;
        typedef typename Vector::LockForRead AutoReadLock;
        typedef typename Vector::LockForWrite AutoWriteLock;

        Get<T>* _get;  ///< Pointer to other end of pipe.
        Vector _vec;   ///< Holds objects written to pipe.

};


/// Readable end of a FIFO pipe.
/// Each end of a FIFO pipe exists independently of the other. This allows 
/// separate ownership and lifetimes. An actual FIFO pipe is formed by 
/// connecting a fifo::Put and a fifo::Get together (see Get::connectTo). When 
/// one end of an established FIFO pipe is destroyed the pipe is automatically 
/// broken and all objects in transit are deleted. The remaining end may be 
/// reused as part of another FIFO pipe.
template<typename T>
class Get {
    public:
        friend class Put<T>;

        Get();
        virtual ~Get();

        void clear();
        void transfer();
        std::auto_ptr<T> get();
        void connectTo(Put<T>& put);
        bool closed() const;
        bool empty() const;

    private:
        void disconnect();

        typedef typename Lockable<T*>::Vector Vector;
        typedef typename Vector::LockForRead AutoReadLock;
        typedef typename Vector::LockForWrite AutoWriteLock;

        Put<T>* _put;  ///< Pointer to other end of pipe.
        Vector _vec;   ///< Holds objects to be read from pipe.
        int _index;    ///< Index of next object to be read.

};


}  // namespace fifo


////////// fifo::Put //////////

/// Create the writable end of a FIFO pipe.
template<typename T>
fifo::Put<T>::Put() :
    _get(0)
{
    AutoWriteLock(_vec)->reserve(FIFOSIZE);
}

/// Destroy the writable end of a FIFO pipe.
template<typename T>
fifo::Put<T>::~Put()
{
    disconnect();
}

/// Delete all objects just written to FIFO pipe.
template<typename T>
void fifo::Put<T>::clear()
{
    foreach (T*& p, *AutoWriteLock(_vec)) {
        delete p;
        p = 0;
    }
}

/// Transfer objects to other end of FIFO pipe.
/// Note that this function does nothing if the readable end of the pipe has
/// objects still waiting to be read.
template<typename T>
void fifo::Put<T>::transfer()
{
    assert(!closed());
    assert(_get->_put == this);

    _get->transfer();
}

/// Puts an object into the FIFO pipe.
/// Simply placing an object into the pipe will not allow it to be read at the
/// opposite end. Objects must be transferred through the pipe by regular calls
/// to the transfer function (on either end).
/// \param object The object to put into the pipe.
template<typename T>
void fifo::Put<T>::put(T& object)
{
    assert(!closed());
    std::auto_ptr<T> p(object.clone());
    AutoWriteLock(_vec)->push_back(p.get());
    p.release();
}

/// Form a FIFO pipe between two ends.
/// If either end is part of an existing pipe that pipe will be broken and any
/// objects it has in transit will be deleted.
/// \param get The readable end of the pipe.
template<typename T>
void fifo::Put<T>::connectTo(Get<T>& get)
{
    get.connectTo(*this);
}

/// \return Whether this end is part of a FIFO pipe.
template<typename T>
inline bool fifo::Put<T>::closed() const
{
    return (_get == 0);
}

/// \return Whether any objects have just been written.
template<typename T>
inline bool fifo::Put<T>::empty() const
{
    return AutoReadLock(_vec)->empty();
}

/// Disconnect FIFO pipe and free any objects that are in transit.
template<typename T>
void fifo::Put<T>::disconnect()
{
    assert((_get == 0) || (_get->_put == this));

    if (_get != 0) 
        _get->disconnect();
}


////////// fifo::Get //////////

/// Create the readable end of a FIFO pipe.
template<typename T>
fifo::Get<T>::Get() :
    _put(0), _index(0)
{
    AutoWriteLock(_vec)->reserve(FIFOSIZE);
}

/// Destroy the readable end of a FIFO pipe.
template<typename T>
fifo::Get<T>::~Get()
{
    disconnect();
}

/// Delete all objects waiting to be read from FIFO pipe.
template<typename T>
void fifo::Get<T>::clear()
{
    foreach (T*& p, *AutoWriteLock(_vec)) {
        delete p;
        p = 0;
    }
}

/// Transfer objects from other end of FIFO pipe.
/// Note that this function does nothing if the readable end of the pipe has
/// objects still waiting to be read.
template<typename T>
void fifo::Get<T>::transfer()
{
    assert(_put != 0);
    assert(_put->_get == this);

    if (!empty()) 
        return;

    AutoWriteLock(_put->_vec)->swap(*AutoWriteLock(_vec));
}

/// Gets an object from the FIFO pipe.
/// Objects put into the pipe will not automatically appear at this end. 
/// Objects must be transferred through the pipe by regular calls to the 
/// transfer function (on either end).
/// \return The object got from the pipe.
/// \pre !empty()
template<typename T>
std::auto_ptr<T> fifo::Get<T>::get()
{
    assert(!empty());

    AutoWriteLock vec(_vec);

    std::auto_ptr<T> p((*vec)[_index]);
    (*vec)[_index++] = 0;

    return p;
}

/// Form a FIFO pipe between two ends.
/// If either end is part of an existing pipe that pipe will be broken and any
/// objects it has in transit will be deleted.
/// \param get The writable end of the pipe.
template<typename T>
void fifo::Get<T>::connectTo(Put<T>& put)
{
    disconnect();
    put.disconnect();
    
    _put = &put;
    put._get = this;
}

/// \return Whether this end is part of a FIFO pipe.
template<typename T>
inline bool fifo::Get<T>::closed() const
{
    return (_put == 0);
}

/// \return Whether any objects are waiting to be read.
template<typename T>
inline bool fifo::Get<T>::empty() const
{
    return (_index == AutoReadLock(_vec)->size());
}

/// Disconnect FIFO pipe and free any objects that are in transit.
template<typename T>
void fifo::Get<T>::disconnect()
{
    assert((_put == 0) || (_put->_get == this));

    clear();

    if (_put != 0) {
        _put->clear();
        _put->_get = 0;
    }

    _put = 0;
}


#endif  // FIFO_HPP

