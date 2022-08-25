/// \file localstore.hpp
/// \brief Provides storage local to each thread.
/// \author Ben Radford 
/// \date 10th November 2007
///
/// Copyright (c) 2007 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):


#ifndef LOCALSTORE_HPP
#define LOCALSTORE_HPP


#include <memory>
#include <core/core.hpp>
#include "lock.hpp"
#include "autolock.hpp"


/// Holds data local to worker threads.
/// 
template<typename T>
class LocalStore {
    public:
        LocalStore();
        ~LocalStore();
        
        T* operator->();
        T& operator*();
        
    private:
        LocalStore(const LocalStore&);
        LocalStore& operator=(const LocalStore&);
        
        static void destructor(void* data);
        T* getObjectPointer();
        
        struct Node {
            T object;
            Node* prev;
            Node* next;
            Node** first;
        };
        
        Node* _first;
        Lock<Node*> _listLock;
        
        pthread_key_t _key;
};


////////// LocalStore //////////

template<typename T>
LocalStore<T>::LocalStore()
    : _first(0), _listLock(_first)
{
    if (pthread_key_create(&_key, destructor) != 0)
        throw MemoryException("failed to create thread local store");
}

template<typename T>
LocalStore<T>::~LocalStore()
{
    pthread_key_delete(_key);
    
    while (_first != 0)
        destructor(_first);
}

template<typename T>
T* LocalStore<T>::operator->()
{
    return getObjectPointer();
}

template<typename T>
T& LocalStore<T>::operator*()
{
    return *getObjectPointer();
}

template<typename T>
void LocalStore<T>::destructor(void* data)
{
    auto node = std::unique_ptr<Node>(reinterpret_cast<Node*>(data));
    
    if (node->prev != 0)
        node->prev->next = node->next;
    if (node->next != 0)
        node->next->prev = node->prev;
    
    if (node.get() == *node->first)
        *node->first = node->next;
}

template<typename T>
T* LocalStore<T>::getObjectPointer()
{
    void* data = pthread_getspecific(_key);
    
    if (data != 0)
        return reinterpret_cast<T*>(data);
    
    auto node = std::make_unique<Node>();
    
    if (pthread_setspecific(_key, node.get()) != 0)
        throw MemoryException("out of thread local memory");
    
    AutoWriteLock<Node*> lock(_listLock);
    
    node->prev = 0;
    node->next = _first;
    node->first = &_first;
    
    if (_first != 0)
        _first->prev = node.get();
    
    _first = node.release();
    
    return &_first->object;
}


#endif  // LOCALSTORE_HPP
