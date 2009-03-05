/// \file fifo.hpp
/// \brief Generic FIFO queue.
/// \author Ben Radford 
/// \date 5th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef FIFO_HPP
#define FIFO_HPP


#include <memory>
#include "autolock.hpp"


namespace fifo {


template<typename T> class Put;
template<typename T> class Get;


static const int FIFOSIZE = 10;  ///< Space to reserve on each end of FIFO.


/// \brief Writable end of a FIFO pipe.
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
        typedef typename Vector::LockForWrite AutoLock;

        Get<T>* _get;  ///< Pointer to other end of pipe.
        Vector _vec;   ///< Holds objects written to pipe.

};


/// \brief Readable end of a FIFO pipe.
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
        typedef typename Vector::LockForWrite AutoLock;

        Put<T>* _put;  ///< Pointer to other end of pipe.
        Vector _vec;   ///< Holds objects to be read from pipe.
        int _index;    ///< Index of next object to be read.

};


}  // namespace fifo


#endif  // FIFO_HPP

