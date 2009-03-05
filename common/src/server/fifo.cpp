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

template<typename T>
fifo::Put<T>::Put() :
    _get(0)
{
    _vec.reserve(FIFOSIZE);
}

template<typename T>
fifo::Put<T>::~Put()
{
    clear();
}

template<typename T>
void fifo::Put<T>::clear()
{
    foreach (T*& p, Vector::LockForWrite(_vec)) {
        delete p;
        p = 0;
    }
}

template<typename T>
void fifo::Put<T>::transfer()
{
    assert(_get != 0);
    assert(_get->_put == this);

    _get->transfer();
}

template<typename T>
void fifo::Put<T>::put(T& value)
{
    std::auto_ptr<T> p(new T(value));
    AutoLock(_vec)->push_back(p.get());
    p.release();
}

template<typename T>
void fifo::Put<T>::connectTo(Get<T>& get)
{
    get.connectTo(*this);
}

template<typename T>
bool fifo::Put<T>::closed() const
{
    return (_get == 0);
}

template<typename T>
bool fifo::Put<T>::empty() const
{
    return AutoLock(_vec)->empty();
}

template<typename T>
void fifo::Put<T>::disconnect()
{
    assert((_get == 0) || (_get->_put == this));

    if (_get != 0) 
        _get->disconnect();
}


////////// fifo::Get //////////

template<typename T>
fifo::Get<T>::Get() :
    _put(0), _index(0)
{
    _vec.reserve(FIFOSIZE);
}

template<typename T>
fifo::Get<T>::~Get()
{
    clear();
}

template<typename T>
void fifo::Get<T>::clear()
{
    foreach (T*& p, Vector::LockForWrite(_vec)) {
        delete p;
        p = 0;
    }
}

template<typename T>
void fifo::Get<T>::transfer()
{
    assert(_put != 0);
    assert(_put->_get == this);

    if (!empty()) 
        return;

    AutoLock(_put->_vec)->swap(*AutoLock(_vec));
}

template<typename T>
std::auto_ptr<T> fifo::Get<T>::get()
{
    assert(!empty());

    AutoLock vec(_vec);

    std::auto_ptr<T> p((*vec)[_index]);
    (*vec)[_index++] = 0;

    return p;
}

template<typename T>
void fifo::Get<T>::connectTo(Put<T>& put)
{
    disconnect();
    put.disconnect();
    
    _put = &put;
    put._get = this;
}

template<typename T>
bool fifo::Get<T>::closed() const
{
    return (_put == 0);
}

template<typename T>
bool fifo::Get<T>::empty() const
{
    return (_index == AutoLock(_vec)->size());
}

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

