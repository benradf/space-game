/// \file fifo.hpp
/// \brief Generic FIFO queue.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef FIFO_HPP
#define FIFO_HPP


#include <memory>
#include <assert.h>
#include <core/core.hpp>
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
        void put(const T& object);
        void connectTo(Get<T>& get);
        bool closed() const;
        bool empty() const;

    private:
        void disconnect();

        typedef AutoWriteLock<Put> HalfLockFIFO;
        typedef typename Lockable<T*>::Vector Vector;
        typedef typename Vector::LockForRead ReadLock;
        typedef typename Vector::LockForWrite WriteLock;

        Lock<Put> _lock;  ///< Lock for this half of pipe.
        Get<T>* _get;     ///< Pointer to other end of pipe.
        Vector _vec;      ///< Holds objects written to pipe.

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
        std::unique_ptr<T> get();
        void connectTo(Put<T>& put);
        bool closed() const;
        bool empty() const;

    private:
        void transfer(bool referred);
        void disconnect(bool referred);

        typedef AutoWriteLock<Get> HalfLockFIFO;
        typedef typename Lockable<T*>::Vector Vector;
        typedef typename Vector::LockForRead ReadLock;
        typedef typename Vector::LockForWrite WriteLock;

        Lock<Get> _lock;  ///< Lock for this half of pipe.
        Put<T>* _put;     ///< Pointer to other end of pipe.
        Vector _vec;      ///< Holds objects to be read from pipe.
        size_t _index;    ///< Index of next object to be read.

        class LockFIFO {
            public:
                LockFIFO(Get* get, bool bothEnds);
                ~LockFIFO();
                
                bool halfLocked() const;

            private:
                bool _bothEnds;
                Get<T>* _get;
                Put<T>* _put;
        };
};


}  // namespace fifo


////////// fifo::Put //////////

/// Create the writable end of a FIFO pipe.
template<typename T>
fifo::Put<T>::Put() :
    _lock(*this), _get(0)
{
    WriteLock(_vec)->reserve(FIFOSIZE);
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
    WriteLock vec(_vec);

    for (auto& p : *vec)
        delete p;

    vec->clear();
}

/// Transfer objects to other end of FIFO pipe.
/// This function simply locks this end of the pipe and invokes the transfer 
/// function on the get side.
template<typename T>
void fifo::Put<T>::transfer()
{
    HalfLockFIFO lock(_lock);
    if (_get != 0) 
        _get->transfer(true);
}

/// Puts an object into the FIFO pipe.
/// Simply placing an object into the pipe will not allow it to be read at the
/// opposite end. Objects must be transferred through the pipe by regular calls
/// to the transfer function (on either end).
/// \param object The object to put into the pipe.
template<typename T>
void fifo::Put<T>::put(const T& object)
{
    if (closed()) 
        return;

    auto p = object.clone();
    WriteLock(_vec)->push_back(p.get());
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
    return ReadLock(_vec)->empty();
}

/// Disconnect FIFO pipe and free any objects that are in transit.
/// This function simply locks this end of the pipe and invokes the disconnect
/// function on the get side.
template<typename T>
void fifo::Put<T>::disconnect()
{
    HalfLockFIFO lock(_lock);
    if (_get != 0) 
        _get->disconnect(true);
}


////////// fifo::Get //////////

/// Create the readable end of a FIFO pipe.
template<typename T>
fifo::Get<T>::Get() :
    _lock(*this), _put(0), _index(0)
{
    WriteLock(_vec)->reserve(FIFOSIZE);
}

/// Destroy the readable end of a FIFO pipe.
template<typename T>
fifo::Get<T>::~Get()
{
    disconnect(false);
}

/// Delete all objects waiting to be read from FIFO pipe.
template<typename T>
void fifo::Get<T>::clear()
{
    WriteLock vec(_vec);

    for (auto& p : *vec)
        delete p;

    vec->clear();
    _index = 0;
}

/// Transfer objects from other end of FIFO pipe.
template<typename T>
void fifo::Get<T>::transfer()
{
    transfer(false);
}

/// Gets an object from the FIFO pipe.
/// Objects put into the pipe will not automatically appear at this end. 
/// Objects must be transferred through the pipe by regular calls to the 
/// transfer function (on either end).
/// \return The object got from the pipe.
/// \pre !empty()
template<typename T>
std::unique_ptr<T> fifo::Get<T>::get()
{
    assert(!empty());

    WriteLock vec(_vec);

    auto p = std::unique_ptr<T>((*vec)[_index]);
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
    disconnect(false);
    put.disconnect();
    
    typename Get<T>::HalfLockFIFO getLock(_lock);
    typename Put<T>::HalfLockFIFO putLock(put._lock);

    if (!closed() || !put.closed()) 
        return;

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
    ReadLock vec(_vec);

    return (_index == vec->size());
}

/// Transfer objects from other end of FIFO pipe.
/// Note that this function does nothing if the readable end of the pipe has
/// objects still waiting to be read. The referred parameter is provided for 
/// when this function is invoked from the Put end. When that happens the Put
/// end will already be locked so it should not be locked again.
/// \param referred Whether the Put end should be locked.
template<typename T>
void fifo::Get<T>::transfer(bool referred)
{
    LockFIFO lock(this, !referred);

    if (lock.halfLocked() || !empty())
        return;

    clear();

    WriteLock(_put->_vec)->swap(*WriteLock(_vec));
}

/// Disconnect FIFO pipe and free any objects that are in transit.
/// This function needs to lock both ends of the pipe in order to perform the
/// disconnect. However, if it is invoked from an already locked Put end it 
/// should not attempt to lock that end again. The referred parameter is 
/// provided for this purpose.
/// \param referred Whether the Put end should be locked.
template<typename T>
void fifo::Get<T>::disconnect(bool referred)
{
    LockFIFO lock(this, !referred);

    if (!lock.halfLocked()) {
        _put->_get = 0;
        _put->clear();
    }

    _put = 0;
    clear();
}


////////// fifo::Get::LockFIFO //////////

template<typename T>
fifo::Get<T>::LockFIFO::LockFIFO(Get* get, bool bothEnds) :
    _bothEnds(bothEnds), _get(get), _put(0)
{
    assert(_get != 0);

    while (true) {
        _get->_lock.rwWaitLock();

        _put = _get->_put;

        assert((_get->_put == 0) || (_get->_put->_get == _get));

        if (!_bothEnds || (_put == 0) || _put->_lock.rwTryLock())
            return;

        _get->_lock.rwUnlock();
    }
}

template<typename T>
fifo::Get<T>::LockFIFO::~LockFIFO()
{
    if (_bothEnds && (_put != 0))
        _put->_lock.rwUnlock();

    _get->_lock.rwUnlock();
}

template<typename T>
bool fifo::Get<T>::LockFIFO::halfLocked() const
{
    return (_put == 0);
}


#endif  // FIFO_HPP

