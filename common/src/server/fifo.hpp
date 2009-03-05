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


static const int FIFOSIZE = 10;


template<typename T>
class Put {
    public:
        friend class Get<T>;

        Put();
        virtual ~Put();

        void clear();
        void transfer();
        void put(T& value);
        void connectTo(Get<T>& get);
        bool closed() const;
        bool empty() const;

    private:
        void disconnect();

        typedef typename Lockable<T*>::Vector Vector;
        typedef typename Vector::LockForWrite AutoLock;

        Get<T>* _get;
        Vector _vec;

};


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

        Put<T>* _put;
        Vector _vec;
        int _index;

};


}  // namespace fifo


#endif  // FIFO_HPP

