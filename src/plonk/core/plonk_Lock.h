/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-12
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of University of the West of England, Bristol nor 
   the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF THE WEST OF ENGLAND, BRISTOL BE 
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 
 This software makes use of third party libraries. For more information see:
 doc/license.txt included in the distribution.
 -------------------------------------------------------------------------------
 */

#ifndef PLONK_LOCK_H
#define PLONK_LOCK_H

#include "plonk_CoreForwardDeclarations.h"
#include "plonk_SmartPointer.h"
#include "plonk_WeakPointer.h"
#include "plonk_SmartPointerContainer.h"

class LockInternalBase : public SmartPointer
{
public:
    LockInternalBase() throw() : SmartPointer (false) { } // no weak ref needed
    ~LockInternalBase() { }
    
    virtual void lock() throw() = 0;
    virtual void unlock() throw() = 0;
    virtual bool tryLock() throw() = 0;
    virtual void wait (const double time) throw() = 0;
    virtual void signal() throw() = 0;

};

class NoLockInternal : public LockInternalBase
{
public:
    NoLockInternal() throw() { }
    ~NoLockInternal() { }
    
    void lock() throw() { }
    void unlock() throw() { }
    bool tryLock() throw() { return true; }
    void wait (const double time) throw() { (void)time; }
    void signal() throw() { }
    
private:
};

class LockInternal : public LockInternalBase
{
public:
    LockInternal() throw();
    ~LockInternal();
    
    void lock() throw();
    void unlock() throw();
    bool tryLock() throw();
    void wait (const double time) throw();
    void signal() throw();

private:
    inline PlankLockRef getPeerRef() { return &l; }
    PlankLock l;    
};

class SpinLockInternal : public LockInternalBase
{
public:
    SpinLockInternal() throw();
    ~SpinLockInternal();
    
    void lock() throw();
    void unlock() throw();
    bool tryLock() throw();
    void wait (const double time) throw();
    void signal() throw();
    
private:
    inline PlankSpinLockRef getPeerRef() { return &l; }
    PlankSpinLock l;    
};

class ThreadSpinLockInternal : public LockInternalBase
{
public:
    ThreadSpinLockInternal() throw();
    ~ThreadSpinLockInternal();
    
    void lock() throw();
    void unlock() throw();
    bool tryLock() throw();
    void wait (const double time) throw();
    void signal() throw();
    
private:
    inline PlankThreadSpinLockRef getPeerRef() { return &l; }
    PlankThreadSpinLock l;    
};


//------------------------------------------------------------------------------

/** @ingroup PlonkOtherUserClasses */
class Lock : public SmartPointerContainer<LockInternalBase>
{
public:
    typedef SmartPointerContainer<LockInternalBase> Base;

    enum Type
    {
        NoLock,             ///< Doesn't actually lock.
        MutexLock,          ///< Uses a mutex to lock.
        SpinLock,           ///< Uses a simple spin lock.
        ThreadSpinLock,     ///< Uses a spin lock that can be locked multiple times from the same thread.
        NumTypes
    };
    
    Lock (const Lock::Type lockType = MutexLock) throw();
    
    explicit Lock (LockInternalBase* internalToUse) throw();
    
    /** Copy constructor. */
    Lock (Lock const& copy) throw();
    Lock& operator= (Lock const& other) throw();
    
    inline void lock() throw()                      { getInternal()->lock(); }
    inline void unlock() throw()                    { getInternal()->unlock(); }
    inline bool tryLock() throw()                   { return getInternal()->tryLock(); }
    inline void wait() throw()                      { getInternal()->wait (0.0); }
    inline void wait (const double time) throw()    { getInternal()->wait (time); }
    inline void signal() throw()                    { getInternal()->signal(); }
    
    PLONK_OBJECTARROWOPERATOR(Lock);
    
private:
    static LockInternalBase* lockInternalFromType (const int lockType) throw();
};

//------------------------------------------------------------------------------

/** @ingroup PlonkOtherUserClasses */
class AutoLock
{
public:
    AutoLock (Lock const& lock = Lock::MutexLock) throw();
    ~AutoLock();
    
private:
    Lock theLock;
    
    AutoLock (AutoLock const&);
    AutoLock& operator= (AutoLock const&);
};

//------------------------------------------------------------------------------

/** @ingroup PlonkOtherUserClasses */
class AutoUnlock
{
public:
    AutoUnlock (Lock const& lock = Lock::MutexLock) throw();
    ~AutoUnlock();
    
private:
    Lock theLock;
    
    AutoUnlock (AutoUnlock const&);
    AutoUnlock& operator= (AutoUnlock const&);
};

//------------------------------------------------------------------------------

/** @ingroup PlonkOtherUserClasses */
class AutoTryLock
{
public:
    AutoTryLock (Lock const& lock = Lock::MutexLock, bool* didLock = 0) throw();
    ~AutoTryLock();
    
    bool getDidLock() const throw() { return didLock; }
        
private:
    Lock theLock;
    bool didLock;
    
    AutoTryLock (AutoTryLock const&);
    AutoTryLock& operator= (AutoTryLock const&);
};

//------------------------------------------------------------------------------

template<class ValueType>
class LockedValueInternal : public SmartPointer
{
public:
    LockedValueInternal (ValueType const& initialValue, Lock const& lockToUse)
    :   value (initialValue),
        lock (lockToUse)
    {
    }
    
    void setValue (ValueType const& newValue) throw()
    {
        const AutoLock l (lock);
        value = newValue;
    }
    
    bool trySetValue (ValueType const& newValue) throw()
    {
        const AutoTryLock l (lock);
        
        if (l.getDidLock())
        {
            value = newValue;
            return true;
        }
        
        return false;
    }
    
    ValueType getValue() const throw()
    {
        ValueType tmp;
        
        const AutoLock l (lock);
        tmp = value;
        return tmp;
    }
    
    inline Lock& getLock() throw() { return lock; }
    
private:
    ValueType value;
    Lock lock;
};

/** A class that stores a value that is stored and retieved using a lock.
 Generally the AtomicValue<> and the atomic Variable classes are best for 
 built-in types but for data/structures larger than 32/64 bits then this class may 
 be useful.
 @ingroup PlonkOtherUserClasses */
template<class Type>
class LockedValue : public SmartPointerContainer< LockedValueInternal<Type> >
{
public:
    typedef LockedValueInternal<Type>             Internal;
    typedef SmartPointerContainer<Internal>       Base;
    typedef Type                                  ValueType;   

    LockedValue (Type const& value = Type(), 
                 Lock const& lock = Lock::SpinLock)
    :   Base (new Internal (value, lock))
    {
    }
    
    LockedValue (LockedValue const& copy) throw()
    :   Base (static_cast<Base> (copy))
    {
    }
    
    LockedValue& operator= (LockedValue const& other) throw()
    {
        if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
    }
    
    inline LockedValue& operator= (Type const& other) throw()
    {
        this->setValue (other);
        return *this;
    }
    
    inline void setValue (Type const& newValue) throw()
    {
        this->getInternal()->setValue (newValue);
    }
    
    inline bool trySetValue (Type const& newValue) throw()
    {
        return this->getInternal()->trySetValue (newValue);
    }
    
    inline Type getValue() const throw()
    {
        return this->getInternal()->getValue();
    }
        
    inline operator Type() const throw()
    {
        return this->getInternal()->getValue();
    }
    
    PLONK_OBJECTARROWOPERATOR(LockedValue);
    
    template<typename Function>
    inline void operator() (VoidReturn::Type ret, Function function)
    {
        (void)ret;
        Type value = this->getValue();
        const AutoLock l (this->getInternal()->getLock());
        (value.*function)();
    }

    template<typename Function, class Arg1Type>
    inline void operator() (VoidReturn::Type ret, Function function, Arg1Type arg1)
    {
        (void)ret;
        Type value = this->getValue();
        const AutoLock l (this->getInternal()->getLock());
        (value.*function)(arg1);
    }
    
    template<typename Function, class Arg1Type, class Arg2Type>
    inline void operator() (VoidReturn::Type ret, Function function, Arg1Type arg1, Arg2Type arg2)
    {
        (void)ret;
        Type value = this->getValue();
        const AutoLock l (this->getInternal()->getLock());
        (value.*function)(arg1, arg2);
    }

    template<class ReturnType, typename Function, class Arg1Type, class Arg2Type, class Arg3Type>
    inline void operator() (VoidReturn::Type ret, Function function, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
    {
        (void)ret;
        Type value = this->getValue();
        const AutoLock l (this->getInternal()->getLock());
        (value.*function)(arg1, arg2, arg3);
    }
    
    template<class ReturnType, typename Function>
    inline void operator() (ReturnType* ret, Function function)
    {
        Type value = this->getValue();
        
        if (ret)
        {
            const AutoLock l (this->getInternal()->getLock());
            *ret = (value.*function)();
        }
        else
        {
            (*this)(VoidReturn::Pass, function);
        }   
    }

    template<class ReturnType, typename Function, class Arg1Type>
    inline void operator() (ReturnType* ret, Function function, Arg1Type arg1)
    {
        Type value = this->getValue();

        if (ret)
        {
            const AutoLock l (this->getInternal()->getLock());
            *ret = (value.*function)(arg1);
        }
        else
        {
            (*this)(VoidReturn::Pass, function, arg1);
        }   
    }
    
    template<class ReturnType, typename Function, class Arg1Type, class Arg2Type>
    inline void operator() (ReturnType* ret, Function function, Arg1Type arg1, Arg2Type arg2)
    {
        Type value = this->getValue();

        if (ret)
        {
            const AutoLock l (this->getInternal()->getLock());
            *ret = (value.*function)(arg1, arg2);
        }
        else
        {
            (*this)(VoidReturn::Pass, function, arg1, arg2);
        }   
    }
    
    template<class ReturnType, typename Function, class Arg1Type, class Arg2Type, class Arg3Type>
    inline void operator() (ReturnType* ret, Function function, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
    {
        Type value = this->getValue();

        if (ret)
        {
            const AutoLock l (this->getInternal()->getLock());
            *ret = (value.*function)(arg1, arg2, arg3);
        }
        else
        {
            (*this)(VoidReturn::Pass, function, arg1, arg2, arg3);
        }   
    }
    
    inline Lock& getLock() throw() { return this->getInternal()->getLock(); }
    
    inline void lock() throw() { this->getLock().lock(); } 
    inline void unlock() throw() { this->getLock().unlock(); } 
    inline bool tryLock() throw() { return this->getLock().tryLock(); } 

};

#define PLONK_LOCKEDVALUE_CALL(VALUETYPE, LOCKEDVALUE, FUNCTION, ARGS) \
    do {\
        VALUETYPE value = LOCKEDVALUE.getValue();\
        const AutoLock l (LOCKEDVALUE.getLock());\
        value.FUNCTION ARGS;\
    } while (false);\

#define PLONK_LOCKEDVALUE_CALLRETURN(VALUETYPE, RETURNEXPR, LOCKEDVALUE, FUNCTION, ARGS) \
    do {\
        VALUETYPE value = LOCKEDVALUE.getValue();\
        const AutoLock l (LOCKEDVALUE.getLock());\
        RETURNEXPR value.FUNCTION ARGS;\
    } while (false);\


#endif // PLONK_LOCK_H
