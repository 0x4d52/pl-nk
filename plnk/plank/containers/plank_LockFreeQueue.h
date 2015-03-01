/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLANK_LOCKFREEQUEUE_H
#define PLANK_LOCKFREEQUEUE_H

#include "plank_LockFreeLinkedListElement.h"

PLANK_BEGIN_C_LINKAGE

/** A lock-free queue (FIFO).
 
 @defgroup PlankLockFreeQueueClass Plank LockFreeQueue class
 @ingroup PlankClasses
 @{
 */

typedef struct PlankLockFreeQueue* PlankLockFreeQueueRef; 

typedef PlankResult (*PlankLockFreeQueueFreeElementDataFunction)(PlankP);

PlankLockFreeQueueRef pl_LockFreeQueue_CreateAndInit();
PlankLockFreeQueueRef pl_LockFreeQueue_Create();
PlankResult pl_LockFreeQueue_Init (PlankLockFreeQueueRef p);
PlankResult pl_LockFreeQueue_DeInit (PlankLockFreeQueueRef p);
PlankResult pl_LockFreeQueue_Destroy (PlankLockFreeQueueRef p);
PlankResult pl_LockFreeQueue_Clear (PlankLockFreeQueueRef p);
PlankResult pl_LockFreeQueue_SetFreeElementDataFunction (PlankLockFreeQueueRef p, PlankLockFreeQueueFreeElementDataFunction freeFunction);
PlankLockFreeQueueFreeElementDataFunction pl_LockFreeQueue_GetFreeElementDataFunction (PlankLockFreeQueueRef p);
PlankResult pl_LockFreeQueue_Push (PlankLockFreeQueueRef p, const PlankLockFreeQueueElementRef element);
PlankResult pl_LockFreeQueue_Pop (PlankLockFreeQueueRef p, PlankLockFreeQueueElementRef* element);

/** NB the result of this could be invalid by the time it is returned in a multithreaded context. */
PlankI pl_LockFreeQueue_GetSize (PlankLockFreeQueueRef p);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankLockFreeQueue
{
	PLANK_ALIGN(PLANK_WIDESIZE) PlankAtomicPX               head;
    PLANK_ALIGN(PLANK_WIDESIZE) PlankAtomicPX               tail;
    PLANK_ALIGN(4) PlankAtomicI                             count;
    PlankLockFreeQueueFreeElementDataFunction               freeFunction;
    PLANK_ALIGN(PLANK_WIDESIZE) PlankLockFreeQueueElement   dummyElement;

} PlankLockFreeQueue PLANK_ALIGN(PLANK_WIDESIZE);
#endif

#endif // PLANK_LOCKFREEQUEUE_H
