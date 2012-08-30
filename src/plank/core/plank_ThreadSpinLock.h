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

#ifndef PLANK_THREADSPINLOCK_H
#define PLANK_THREADSPINLOCK_H

#include "../containers/plank_Atomic.h"
#include "plank_Thread.h"

#define PLANK_THREADSPINLOCK_UNLOCKED 0 // assumes 0 is an invalid thread ID, should be OK on posix & windows

PLANK_BEGIN_C_LINKAGE

/** A crossplatform synchronisation utiltiy.
  
 @defgroup PlankThreadSpinLockClass Plank ThreadSpinLock class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %ThreadSpinLock</i> object. */
typedef struct PlankThreadSpinLock* PlankThreadSpinLockRef; 

/** Create and intitialise a <i>Plank %ThreadSpinLock</i> object and return an oqaque reference to it.
 @return A <i>Plank %ThreadSpinLock</i> object as an opaque reference or PLANK_NULL. */
PlankThreadSpinLockRef pl_ThreadSpinLock_CreateAndInit();

/** Create a <i>Plank %ThreadSpinLock</i> object and return an oqaque reference to it.
 @return A <i>Plank %ThreadSpinLock</i> object as an opaque reference or PLANK_NULL. */
PlankThreadSpinLockRef pl_ThreadSpinLock_Create();

/** Initialise a <i>Plank %ThreadSpinLock</i> object. 
 @param p The <i>Plank %ThreadSpinLock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_ThreadSpinLock_Init (PlankThreadSpinLockRef p);

/** Deinitialise a <i>Plank %ThreadSpinLock</i> object. 
 @param p The <i>Plank %ThreadSpinLock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_ThreadSpinLock_DeInit (PlankThreadSpinLockRef p);

/** Destroy a <i>Plank %ThreadSpinLock</i> object. 
 @param p The <i>Plank %ThreadSpinLock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_ThreadSpinLock_Destroy (PlankThreadSpinLockRef p);

/** Obtain the lock. 
 This will block until the lock can be obtained. Be sure to release the lock
 using pl_ThreadSpinLock_Unlock() otherwise your application will freeze.
 @param p The <i>Plank %ThreadSpinLock</i> object. */
void pl_ThreadSpinLock_Lock (PlankThreadSpinLockRef p);

/** Release the lock. 
 @param p The <i>Plank %ThreadSpinLock</i> object. 
 @return Returns PLANK_TRUE if the lock was unlocked. */
PlankB pl_ThreadSpinLock_Unlock (PlankThreadSpinLockRef p);

/** Tries to obtain the lock but doesn't block if this fails. 
 @param p The <i>Plank %ThreadSpinLock</i> object. 
 @return @c true if the lock was obtained, otherwise @c false. */
static PlankB pl_ThreadSpinLock_TryLock (PlankThreadSpinLockRef p);

/** Wait on the lock. 
 [todo docs]
 @param p The <i>Plank %ThreadSpinLock</i> object. */
void pl_ThreadSpinLock_Wait (PlankThreadSpinLockRef p);

/** Signal the lock. 
 [todo docs]
 @param p The <i>Plank %ThreadSpinLock</i> object. */
void pl_ThreadSpinLock_Signal (PlankThreadSpinLockRef p);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankThreadSpinLock
{
    PLANK_ALIGN(8) PlankAtomicL flag;
} PlankThreadSpinLock;
#endif

static inline PlankB pl_ThreadSpinLock_TryLock (PlankThreadSpinLockRef p)
{
    PlankThreadID threadID = pl_ThreadCurrentID();
    
    if (pl_AtomicL_CompareAndSwap (&p->flag, PLANK_THREADSPINLOCK_UNLOCKED, threadID))
        return PLANK_TRUE;
    else
        return pl_AtomicL_CompareAndSwap (&p->flag, threadID, threadID); // stay locked on same thread
}


#endif // PLANK_ThreadSpinLock_H
