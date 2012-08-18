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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "plonk_Lock.h"
#include "plonk_Thread.h"

LockInternal::LockInternal() throw()
{
    pl_Lock_Init (getPeerRef());
}

LockInternal::~LockInternal()
{
    pl_Lock_DeInit (getPeerRef());
}

void LockInternal::lock() throw()
{
    pl_Lock_Lock (getPeerRef());
}

void LockInternal::unlock() throw()
{
    pl_Lock_Unlock (getPeerRef());
}

bool LockInternal::tryLock() throw()
{
    return pl_Lock_TryLock (getPeerRef()) != 0;
}

void LockInternal::wait() throw()
{
    pl_Lock_Wait (getPeerRef());
}

void LockInternal::signal() throw()
{
    pl_Lock_Signal (getPeerRef());
}

//------------------------------------------------------------------------------

SpinLockInternal::SpinLockInternal() throw()
{
    pl_SpinLock_Init (getPeerRef());
}

SpinLockInternal::~SpinLockInternal()
{
    pl_SpinLock_DeInit (getPeerRef());
}

void SpinLockInternal::lock() throw()
{
    pl_SpinLock_Lock (getPeerRef());
}

void SpinLockInternal::unlock() throw()
{
    pl_SpinLock_Unlock (getPeerRef());
}

bool SpinLockInternal::tryLock() throw()
{
    return pl_SpinLock_TryLock (getPeerRef()) != 0;
}

void SpinLockInternal::wait() throw()
{
    pl_SpinLock_Wait (getPeerRef());
}

void SpinLockInternal::signal() throw()
{
    // nothing
}

//------------------------------------------------------------------------------

ThreadSpinLockInternal::ThreadSpinLockInternal() throw()
{
    pl_ThreadSpinLock_Init (getPeerRef());
}

ThreadSpinLockInternal::~ThreadSpinLockInternal()
{
    pl_ThreadSpinLock_DeInit (getPeerRef());
}

void ThreadSpinLockInternal::lock() throw()
{
    plonk_assert (Threading::getCurrentThreadID() != 0); // zero is used to signal unlocked!
    pl_ThreadSpinLock_Lock (getPeerRef());
}

void ThreadSpinLockInternal::unlock() throw()
{
    plonk_assert (Threading::getCurrentThreadID() != 0); // zero is used to signal unlocked!
    pl_ThreadSpinLock_Unlock (getPeerRef());
}

bool ThreadSpinLockInternal::tryLock() throw()
{
    plonk_assert (Threading::getCurrentThreadID() != 0); // zero is used to signal unlocked!
    return pl_ThreadSpinLock_TryLock (getPeerRef()) != 0;
}

void ThreadSpinLockInternal::wait() throw()
{
    plonk_assert (Threading::getCurrentThreadID() != getPeerRef()->flag.value); // you're asking the thread that locked to wait! how is that going to work?
    pl_ThreadSpinLock_Wait (getPeerRef());
}

void ThreadSpinLockInternal::signal() throw()
{
    // nothing
}

//------------------------------------------------------------------------------

Lock::Lock (const Lock::Type lockType) throw()
:   Base (lockInternalFromType (lockType))
{
}

LockInternalBase* Lock::lockInternalFromType (const int lockType) throw()
{
    plonk_assert (lockType < NumTypes);
    
    switch (lockType) 
    {
        case NoLock:            return new NoLockInternal();
        case MutexLock:         return new LockInternal();
        case SpinLock:          return new SpinLockInternal();
        case ThreadSpinLock:    return new ThreadSpinLockInternal();
        default:                return new NoLockInternal();
    }
}

Lock::Lock (LockInternalBase* internalToUse) throw() 
:	Base (internalToUse)
{
}    

Lock::Lock (Lock const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}            

Lock& Lock::operator= (Lock const& other) throw()
{
    if (this != &other)
        this->setInternal (other.getInternal());
    
    return *this;
}


//------------------------------------------------------------------------------

AutoLock::AutoLock (Lock const& lock) throw()
:   theLock (lock)
{
    theLock.lock();
}

AutoLock::~AutoLock()
{
    theLock.unlock();
}

//------------------------------------------------------------------------------

AutoUnlock::AutoUnlock (Lock const& lock) throw()
:   theLock (lock)
{
    theLock.unlock();
}

AutoUnlock::~AutoUnlock()
{
    theLock.lock();
}

//------------------------------------------------------------------------------

AutoTryLock::AutoTryLock (Lock const& lock, bool* didLockToReturn) throw()
:   theLock (lock),
    didLock (theLock.tryLock())
{
    if (didLock != 0)
        *didLockToReturn = didLock;
}

AutoTryLock::~AutoTryLock()
{
    if (didLock)
        theLock.unlock();
}

END_PLONK_NAMESPACE
