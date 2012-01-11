/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
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
 -------------------------------------------------------------------------------
 */

#ifndef PLANK_LOCKFREELINKEDLISTELEMENT_H
#define PLANK_LOCKFREELINKEDLISTELEMENT_H

#include "plank_Atomic.h"

PLANK_BEGIN_C_LINKAGE

typedef struct PlankLockFreeLinkedListElement*   PlankLockFreeLinkedListElementRef;
typedef PlankLockFreeLinkedListElementRef PlankLockFreeQueueElementRef;
typedef PlankLockFreeLinkedListElementRef PlankLockFreeStackElementRef;

PlankLockFreeLinkedListElementRef pl_LockFreeLinkedListElement_CreateAndInit();
PlankLockFreeLinkedListElementRef pl_LockFreeLinkedListElement_Create();
PlankResult pl_LockFreeLinkedListElement_Init (PlankLockFreeLinkedListElementRef p);
PlankResult pl_LockFreeLinkedListElement_DeInit (PlankLockFreeLinkedListElementRef p);
PlankResult pl_LockFreeLinkedListElement_Destroy (PlankLockFreeLinkedListElementRef p);
PlankP pl_LockFreeLinkedListElement_GetData (PlankLockFreeLinkedListElementRef p);
void pl_LockFreeLinkedListElement_SetData (PlankLockFreeLinkedListElementRef p, const PlankP data);
PlankLockFreeLinkedListElementRef pl_LockFreeLinkedListElement_GetNext (PlankLockFreeLinkedListElementRef p);
void pl_LockFreeLinkedListElement_SetNext (PlankLockFreeLinkedListElementRef p, const PlankLockFreeLinkedListElementRef next);

PlankAtomicPXRef pl_LockFreeLinkedListElement_GetDataAtom (PlankLockFreeLinkedListElementRef p);
PlankAtomicPXRef pl_LockFreeLinkedListElement_GetNextAtom (PlankLockFreeLinkedListElementRef p);

PLANK_END_C_LINKAGE

#define PLANK_INLINING_FUNCTIONS 1
#include "plank_LockFreeLinkedListElementInline.h"
#undef PLANK_INLINING_FUNCTIONS

// make these inline at some point..

#define pl_LockFreeQueueElement_CreateAndInit  pl_LockFreeLinkedListElement_CreateAndInit
#define pl_LockFreeQueueElement_Create  pl_LockFreeLinkedListElement_Create
#define pl_LockFreeQueueElement_Init  pl_LockFreeLinkedListElement_Init
#define pl_LockFreeQueueElement_DeInit  pl_LockFreeLinkedListElement_DeInit
#define pl_LockFreeQueueElement_Destroy pl_LockFreeLinkedListElement_Destroy
#define pl_LockFreeQueueElement_GetData pl_LockFreeLinkedListElement_GetData
#define pl_LockFreeQueueElement_SetData pl_LockFreeLinkedListElement_SetData
#define pl_LockFreeQueueElement_GetNext pl_LockFreeLinkedListElement_GetNext
#define pl_LockFreeQueueElement_SetNext pl_LockFreeLinkedListElement_SetNext
#define pl_LockFreeQueueElement_GetDataAtom pl_LockFreeLinkedListElement_GetDataAtom
#define pl_LockFreeQueueElement_GetNextAtom pl_LockFreeLinkedListElement_GetNextAtom

#define pl_LockFreeStackElement_CreateAndInit  pl_LockFreeLinkedListElement_CreateAndInit
#define pl_LockFreeStackElement_Create  pl_LockFreeLinkedListElement_Create
#define pl_LockFreeStackElement_Init  pl_LockFreeLinkedListElement_Init
#define pl_LockFreeStackElement_DeInit  pl_LockFreeLinkedListElement_DeInit
#define pl_LockFreeStackElement_Destroy pl_LockFreeLinkedListElement_Destroy
#define pl_LockFreeStackElement_GetData pl_LockFreeLinkedListElement_GetData
#define pl_LockFreeStackElement_SetData pl_LockFreeLinkedListElement_SetData
#define pl_LockFreeStackElement_GetNext pl_LockFreeLinkedListElement_GetNext
#define pl_LockFreeStackElement_SetNext pl_LockFreeLinkedListElement_SetNext
#define pl_LockFreeStackElement_GetDataAtom pl_LockFreeLinkedListElement_GetDataAtom
#define pl_LockFreeQueueElement_GetNextAtom pl_LockFreeLinkedListElement_GetNextAtom

#endif // PLANK_LOCKFREELINKEDLISTELEMENT_H
