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


class LockInternal : public SmartPointer
{
public:
    LockInternal() throw();
    ~LockInternal();
    
    friend class Lock;
    
private:
    inline PlankLockRef getPeerRef() { return &lock; }
    PlankLock lock;    
};

//------------------------------------------------------------------------------

/** @ingroup PlonkOtherUserClasses */
class Lock : public SmartPointerContainer<LockInternal>
{
public:
    typedef SmartPointerContainer<LockInternal> Base;

    Lock() throw();
    
    explicit Lock (LockInternal* internalToUse) throw();
    
    /** Copy constructor. */
    Lock (Lock const& copy) throw();
    Lock& operator= (Lock const& other) throw();
        
    void lock() throw();
    void unlock() throw();
    bool tryLock() throw();
    void wait() throw();
    void signal() throw();
};

//------------------------------------------------------------------------------

/** @ingroup PlonkOtherUserClasses */
class AutoLock
{
public:
    AutoLock (Lock const& lock) throw();
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
    AutoUnlock (Lock const& lock) throw();
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
    AutoTryLock (Lock const& lock) throw();
    ~AutoTryLock();
    
private:
    Lock theLock;
    bool didLock;
    
    AutoTryLock (AutoTryLock const&);
    AutoTryLock& operator= (AutoTryLock const&);
};



#endif // PLONK_LOCK_H
