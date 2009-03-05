/// \file fifo.cpp
/// \brief Generic FIFO queue.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include <assert.h>
#include <core.hpp>
#include "fifo.hpp"


////////// fifo::Put //////////

/// \brief Create the writable end of a FIFO pipe.
template<typename T>
fifo::Put<T>::Put() :
    _get(0)
{
    _vec.reserve(FIFOSIZE);
}

/// \brief Destroy the writable end of a FIFO pipe.
template<typename T>
fifo::Put<T>::~Put()
{
    disconnect();
}

/// \brief Delete all objects just written to FIFO pipe.
template<typename T>
void fifo::Put<T>::clear()
{
    foreach (T*& p, Vector::LockForWrite(_vec)) {
        delete p;
        p = 0;
    }
}

/// \brief Transfer objects to other end of FIFO pipe.
/// Note that this function does nothing if the readable end of the pipe has
/// objects still waiting to be read.
template<typename T>
void fifo::Put<T>::transfer()
{
    assert(_get != 0);
    assert(_get->_put == this);

    _get->transfer();
}

/// \brief Puts an object into the FIFO pipe.
/// Simply placing an object into the pipe will not allow it to be read at the
/// opposite end. Objects must be transferred through the pipe by regular calls
/// to the transfer function (on either end).
/// \param object The object to put into the pipe.
template<typename T>
void fifo::Put<T>::put(T& object)
{
    std::auto_ptr<T> p(new T(object));
    AutoLock(_vec)->push_back(p.get());
    p.release();
}

/// \brief Form a FIFO pipe between two ends.
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
bool fifo::Put<T>::closed() const
{
    return (_get == 0);
}

/// \return Whether any objects have just been written.
template<typename T>
bool fifo::Put<T>::empty() const
{
    return AutoLock(_vec)->empty();
}

/// \brief Disconnect FIFO pipe and free any objects that are in transit.
template<typename T>
void fifo::Put<T>::disconnect()
{
    assert((_get == 0) || (_get->_put == this));

    if (_get != 0) 
        _get->disconnect();
}


////////// fifo::Get //////////

/// \brief Create the readable end of a FIFO pipe.
template<typename T>
fifo::Get<T>::Get() :
    _put(0), _index(0)
{
    _vec.reserve(FIFOSIZE);
}

/// \brief Destroy the readable end of a FIFO pipe.
template<typename T>
fifo::Get<T>::~Get()
{
    disconnect();
}

/// \brief Delete all objects waiting to be read from FIFO pipe.
template<typename T>
void fifo::Get<T>::clear()
{
    foreach (T*& p, Vector::LockForWrite(_vec)) {
        delete p;
        p = 0;
    }
}

/// \brief Transfer objects from other end of FIFO pipe.
/// Note that this function does nothing if the readable end of the pipe has
/// objects still waiting to be read.
template<typename T>
void fifo::Get<T>::transfer()
{
    assert(_put != 0);
    assert(_put->_get == this);

    if (!empty()) 
        return;

    AutoLock(_put->_vec)->swap(*AutoLock(_vec));
}

/// \brief Gets an object from the FIFO pipe.
/// Objects put into the pipe will not automatically appear at this end. 
/// Objects must be transferred through the pipe by regular calls to the 
/// transfer function (on either end).
/// \return The object got from the pipe.
/// \pre !empty()
template<typename T>
std::auto_ptr<T> fifo::Get<T>::get()
{
    assert(!empty());

    AutoLock vec(_vec);

    std::auto_ptr<T> p((*vec)[_index]);
    (*vec)[_index++] = 0;

    return p;
}

/// \brief Form a FIFO pipe between two ends.
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
bool fifo::Get<T>::closed() const
{
    return (_put == 0);
}

/// \return Whether any objects are waiting to be read.
template<typename T>
bool fifo::Get<T>::empty() const
{
    return (_index == AutoLock(_vec)->size());
}

/// \brief Disconnect FIFO pipe and free any objects that are in transit.
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

