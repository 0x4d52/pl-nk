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

LockInternal::LockInternal() throw()
:   SmartPointer (false) // no weak ref needed
{
    pl_Lock_Init (&lock);
}

LockInternal::~LockInternal()
{
    pl_Lock_DeInit (&lock);
}

//------------------------------------------------------------------------------

Lock::Lock() throw()
:   Base (new LockInternal())
{
}

Lock::Lock (LockInternal* internalToUse) throw() 
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
        this->setInternal (other.containerCopy().getInternal());
    
    return *this;
}

void Lock::lock() throw()
{
    pl_Lock_Lock (getInternal()->getPeerRef());
}

void Lock::unlock() throw()
{
    pl_Lock_Unlock (getInternal()->getPeerRef());
}

bool Lock::tryLock() throw()
{
    return pl_Lock_TryLock (getInternal()->getPeerRef()) != 0;
}

void Lock::wait() throw()
{
    pl_Lock_Wait (getInternal()->getPeerRef());
}

void Lock::signal() throw()
{
    pl_Lock_Signal (getInternal()->getPeerRef());
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

AutoTryLock::AutoTryLock (Lock const& lock) throw()
:   theLock (lock),
    didLock (theLock.tryLock())
{
}

AutoTryLock::~AutoTryLock()
{
    if (didLock)
        theLock.unlock();
}

END_PLONK_NAMESPACE
