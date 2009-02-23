/// \file lock.hpp
/// \brief Defines a lock object.
/// \author Ben Radford 
/// \date 18th September 2007
///
/// Copyright (c) 2007 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):
/// - 08/10/07 Made the try locks fully atomic using the assembly lock prefix.


#ifndef LOCK_HPP
#define LOCK_HPP


/// Safely manages access to an object in a multi-threaded environment.
/// The %Lock is constructed by with a reference to the object it is to manage.
/// All access to this object must then be done via the lock to guarantee thread
/// safety. Accessing the object directly will result in undefined behaviour.
/// Methods are provided to lock the object for both read-write and read-only 
/// access. The read-write lock may only be held once at any given time and
/// no read-only locks may be held at the same time as it. Multiple read-only 
/// locks may be held at once. This ensures data is in a consistent state when 
/// it is read. 
template<typename T>
class Lock {
    public:
        Lock(T& object);
        
        T* rwTryLock();
        T& rwWaitLock();
        void rwUnlock();
        
        const T* roTryLock();
        const T& roWaitLock();
        void roUnlock();
        
        bool rwLocked() const;
        bool roLocked() const;
        
        T& getObject();
        
    protected:
        
    private:
        unsigned char _rw;  ///< Read-write lock flag.
        unsigned char _ro;  ///< Read-only lock count.
        T& _object;         ///< Object to manage locks for.
};


////////// Lock //////////

/// Create a lock for \em object.
/// \param object The object to create the lock for.
template<typename T>
inline Lock<T>::Lock(T& object)
    : _rw(0), _ro(0), _object(object)
{
    
}

/// Attempt to lock the object for read-write access.
/// If no read-write or read-only locks are held then this method locks the 
/// object for read-write access and returns a pointer to the object. If it 
/// can't lock the object because it is already locked it then it will return
/// zero.
/// \return A pointer to locked object or zero.
template<typename T>
T* Lock<T>::rwTryLock()
{
    unsigned char lock = 0;
    
    // Try and get lock.
    asm volatile (
        "              lock cmpxchgb %%dl, %1;"
        "              cmpb $0, %%al;         "
        "              jne tryrw_done%=;      "
        "              orb %2, %%al;          "
        "              cmpb $0, %%al;         "
        "              je tryrw_done%=;       "
        "              lock andb $0, %1;      "
        "tryrw_done%=:                        "
        : "=a"(lock)
        : "m"(_rw), "m"(_ro), "a"(0), "d"(1)
    );
    
    // Did we get the lock?
    if (lock != 0)
        return 0;
    
    return &_object;
}

/// Lock the object for read-write access.
/// If no read-write or read-only locks are held then this method sets the 
/// read-write lock and returns. Otherwise it spins until the stated condition
/// is true.
/// \return The locked object.
template<typename T>
inline T& Lock<T>::rwWaitLock()
{
    // Spin until we get rw lock and ro == 0.
    asm volatile (
        "rwwait_spin%=: lock cmpxchgb %%dl, %0;"
        "               cmpb $0, %%al;         "
        "               jne rwwait_spin%=;     "
        "               cmpb $0, %1;           "
        "               je rwwait_done%=;      "
        "               movb $0, %0;           "
        "               jmp rwwait_spin%=;     "
        "rwwait_done%=:                        "
        : : "m"(_rw), "m"(_ro), "a"(0), "d"(1)
    );
    
    return _object;
}

/// Release a read-write lock.
/// After calling this method the read-write lock is free to be acquired again.
/// It also makes it possible to lock the object for read-only access which is
/// blocked while the read-write lock is held. Note that behaviour is undefined
/// if a call to this method is not preceded by a call to rwLock().
template<typename T>
inline void Lock<T>::rwUnlock()
{
    asm volatile ("lock andb $0, %0" : : "m"(_rw));
}

/// Attempt to lock the object for read-only access.
/// If the read-write lock is not held then this method locks the object for 
/// read-only access and returns a pointer to the object. Otherwise it returns
/// zero. The caller is allowed to read the object but must not write to it. As
/// long as the object is const correct the compiler will enforce this 
/// condition.
/// \return A pointer to the locked object or zero.
template<typename T>
const T* Lock<T>::roTryLock()
{
    unsigned char lock = 0;
    
    // Try and get lock.
    asm volatile (
        "lock cmpxchgb %%dl, %1;"
        : "=a"(lock)
        : "m"(_rw), "a"(0), "d"(1)
    );
    
    // Did we get the lock?
    if (lock != 0)
        return 0;
    
    // Increment ro lock count.
    _ro++;
    
    // Release rw lock.
    asm volatile ("lock andb $0, %0" : : "m"(_rw));
    
    return &_object;
}

/// Lock the object for read-only access.
/// If the read-write lock is not held then this method locks the object for 
/// read-only access and returns. Otherwise it spins until the stated condition
/// is true. The caller is allowed to read the object but must not write to it. 
/// As long as the object is const correct the compiler will enforce this 
/// condition.
/// \return The locked object.
template<typename T>
inline const T& Lock<T>::roWaitLock()
{
    // Spin until we get rw lock.
    asm volatile (
        "rowait_spin%=: lock cmpxchgb %%dl, %0;"
        "               cmpb $0, %%al;         "
        "               jne rowait_spin%=;     "
        : : "m"(_rw), "a"(0), "d"(1)
    );
    
    // Increment ro lock count.
    _ro++;
    
    // Release rw lock.
    asm volatile ("lock andb $0, %0" : : "m"(_rw));
    
    return _object;
}

/// Release a read-only lock.
/// Depending on whether other read-only locks are held it may be possible to
/// acquire a read-write lock after calling this method. It is certainly 
/// possible if the last read-only lock is released. Note that behaviour is 
/// undefined if a call to this method is not preceded by a call to roLock().
template<typename T>
inline void Lock<T>::roUnlock()
{
    asm volatile ("lock decb %0;" : : "m"(_ro));
}

/// Indicates whether the read-write lock is held.
/// \return Status of read-write lock.
template<typename T>
inline bool Lock<T>::rwLocked() const
{
    return (_rw != 0);
}

/// Indicates whether any read-only locks are held.
/// \return Status of read-only lock.
template<typename T>
inline bool Lock<T>::roLocked() const
{
    return (_ro != 0);
}

/// Used to access the object guarded by this lock.
/// This method provides access to the object in addition to that provided by
/// the returns from the locking methods. However it should only be called while
/// a valid lock is held on the object.
/// \return The object.
template<typename T>
inline T& Lock<T>::getObject()
{
    return _object;
}


#endif  // LOCK_HPP
