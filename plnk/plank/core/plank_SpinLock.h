/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLANK_SPINLOCK_H
#define PLANK_SPINLOCK_H

#include "../containers/atomic/plank_Atomic.h"

#define PLANK_SPINLOCK_UNLOCKED 0
#define PLANK_SPINLOCK_LOCKED 1

PLANK_BEGIN_C_LINKAGE

/** A crossplatform synchronisation utiltiy.
  
 @defgroup PlankSpinLockClass Plank SpinLock class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %SpinLock</i> object. */
typedef struct PlankSpinLock* PlankSpinLockRef; 

/** Create and intitialise a <i>Plank %SpinLock</i> object and return an oqaque reference to it.
 @return A <i>Plank %SpinLock</i> object as an opaque reference or PLANK_NULL. */
PlankSpinLockRef pl_SpinLock_CreateAndInit();

/** Create a <i>Plank %SpinLock</i> object and return an oqaque reference to it.
 @return A <i>Plank %SpinLock</i> object as an opaque reference or PLANK_NULL. */
PlankSpinLockRef pl_SpinLock_Create();

/** Initialise a <i>Plank %SpinLock</i> object. 
 @param p The <i>Plank %SpinLock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_SpinLock_Init (PlankSpinLockRef p);

/** Deinitialise a <i>Plank %SpinLock</i> object. 
 @param p The <i>Plank %SpinLock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_SpinLock_DeInit (PlankSpinLockRef p);

/** Destroy a <i>Plank %SpinLock</i> object. 
 @param p The <i>Plank %SpinLock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_SpinLock_Destroy (PlankSpinLockRef p);

/** Obtain the lock. 
 This will block until the lock can be obtained. Be sure to release the lock
 using pl_SpinLock_Unlock() otherwise your application will freeze.
 @param p The <i>Plank %SpinLock</i> object. */
void pl_SpinLock_Lock (PlankSpinLockRef p);

/** Release the lock. 
 @param p The <i>Plank %SpinLock</i> object. */
void pl_SpinLock_Unlock (PlankSpinLockRef p);

/** Tries to obtain the lock but doesn't block if this fails. 
 @param p The <i>Plank %SpinLock</i> object. 
 @return @c true if the lock was obtained, otherwise @c false. */
static PlankB pl_SpinLock_TryLock (PlankSpinLockRef p);

/** Wait on the lock. 
 [todo docs]
 @param p The <i>Plank %SpinLock</i> object. */
void pl_SpinLock_Wait (PlankSpinLockRef p);

void pl_SpinLock_WaitTimeout (PlankSpinLockRef p, double time);

/** Signal the lock. 
 [todo docs]
 @param p The <i>Plank %SpinLock</i> object. */
void pl_SpinLock_Signal (PlankSpinLockRef p);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankSpinLock
{
    PLANK_ALIGN(4) PlankAtomicI flag;
} PlankSpinLock;
#endif

static PLANK_INLINE_LOW PlankB pl_SpinLock_TryLock (PlankSpinLockRef p)
{
    return pl_AtomicI_CompareAndSwap (&p->flag, PLANK_SPINLOCK_UNLOCKED, PLANK_SPINLOCK_LOCKED);
}

#endif // PLANK_SPINLOCK_H
