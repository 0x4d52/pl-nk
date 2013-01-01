/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#ifndef PLANK_LOCK_H
#define PLANK_LOCK_H

PLANK_BEGIN_C_LINKAGE

/** A crossplatform synchronisation utiltiy.
 
 This uses a pthread mutex on supported platforms and a CriticalSection on Windows.
 
 @defgroup PlankLockClass Plank Lock class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %Lock</i> object. */
typedef struct PlankLock* PlankLockRef; 

/** Create and intitialise a <i>Plank %Lock</i> object and return an oqaque reference to it.
 @return A <i>Plank %Lock</i> object as an opaque reference or PLANK_NULL. */
PlankLockRef pl_Lock_CreateAndInit();

/** Create a <i>Plank %Lock</i> object and return an oqaque reference to it.
 @return A <i>Plank %Lock</i> object as an opaque reference or PLANK_NULL. */
PlankLockRef pl_Lock_Create();

/** Initialise a <i>Plank %Lock</i> object. 
 @param p The <i>Plank %Lock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Lock_Init (PlankLockRef p);

/** Deinitialise a <i>Plank %Lock</i> object. 
 @param p The <i>Plank %Lock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Lock_DeInit (PlankLockRef p);

/** Destroy a <i>Plank %Lock</i> object. 
 @param p The <i>Plank %Lock</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Lock_Destroy (PlankLockRef p);

/** Obtain the lock. 
 This will block until the lock can be obtained. Be sure to release the lock
 using pl_Lock_Unlock() otherwise your application will freeze.
 @param p The <i>Plank %Lock</i> object. */
void pl_Lock_Lock (PlankLockRef p);

/** Release the lock. 
 @param p The <i>Plank %Lock</i> object. */
void pl_Lock_Unlock (PlankLockRef p);

/** Tries to obtain the lock but doesn't block if this fails. 
 @param p The <i>Plank %Lock</i> object. 
 @return @c true if the lock was obtained, otherwise @c false. */
PlankB pl_Lock_TryLock (PlankLockRef p);

/** Wait on the lock. 
 [todo docs]
 @param p The <i>Plank %Lock</i> object. */
void pl_Lock_Wait (PlankLockRef p);

void pl_Lock_WaitTimeout (PlankLockRef p, double time);

/** Signal the lock. 
 [todo docs]
 @param p The <i>Plank %Lock</i> object. */
void pl_Lock_Signal (PlankLockRef p);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
#if PLANK_APPLE
typedef struct PlankLock
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    PlankB flag;
} PlankLock;
#endif

#if PLANK_WIN
typedef struct PlankLock
{
    CRITICAL_SECTION mutex;
    HANDLE condition;
} PlankLock;
#endif
#endif

#endif // PLANK_LOCK_H
