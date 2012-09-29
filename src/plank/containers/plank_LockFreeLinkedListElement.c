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

#include "../core/plank_StandardHeader.h"
#include "plank_LockFreeLinkedListElement.h"

#if PLANK_NOATOMIC64BIT
#include "../core/plank_Lock.h"
#endif

PlankLockFreeLinkedListElementRef pl_LockFreeLinkedListElement_CreateAndInit()
{
    PlankLockFreeLinkedListElementRef p;
    p = pl_LockFreeLinkedListElement_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_LockFreeLinkedListElement_Init (p) != PlankResult_OK)
            pl_LockFreeLinkedListElement_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankLockFreeLinkedListElementRef pl_LockFreeLinkedListElement_Create()
{
    PlankMemoryRef m;
    PlankLockFreeLinkedListElementRef p;
    
    m = pl_MemoryGlobal(); // must use the lock free one when done... nope just live with it and use alogrithms that reuse the memory allocs
    p = (PlankLockFreeLinkedListElementRef)pl_Memory_AllocateBytes (m, sizeof (PlankLockFreeLinkedListElement));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankLockFreeLinkedListElement));
    
    return p;    
}

PlankResult pl_LockFreeLinkedListElement_Init (PlankLockFreeLinkedListElementRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankLockFreeLinkedListElement)); 
        
    pl_AtomicPX_Init (&p->next);
    pl_AtomicPX_Init (&p->data);
    
exit:
    return result;
}

PlankResult pl_LockFreeLinkedListElement_DeInit (PlankLockFreeLinkedListElementRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
        
    pl_AtomicPX_DeInit (&p->next);
    pl_AtomicPX_DeInit (&p->data);
    pl_MemoryZero (p, sizeof (PlankLockFreeLinkedListElement));

exit:
    return result;    
}

PlankResult pl_LockFreeLinkedListElement_Destroy (PlankLockFreeLinkedListElementRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if ((result = pl_LockFreeLinkedListElement_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

PlankP pl_LockFreeLinkedListElement_GetData (PlankLockFreeLinkedListElementRef p)
{
    return pl_AtomicPX_GetUnchecked (&p->data); // changed to unchecked
}

void pl_LockFreeLinkedListElement_SetData (PlankLockFreeLinkedListElementRef p, const PlankP data)
{
    pl_AtomicPX_Set (&p->data, data);
}

PlankLockFreeLinkedListElementRef pl_LockFreeLinkedListElement_GetNext (PlankLockFreeLinkedListElementRef p)
{
    return pl_AtomicPX_GetUnchecked (&p->next); // changed to unchecked
}

void pl_LockFreeLinkedListElement_SetNext (PlankLockFreeLinkedListElementRef p, const PlankLockFreeLinkedListElementRef next)
{
    pl_AtomicPX_Set (&p->next, next);
}

PlankAtomicPXRef pl_LockFreeLinkedListElement_GetDataAtom (PlankLockFreeLinkedListElementRef p)
{
    return &p->data;
}

PlankAtomicPXRef pl_LockFreeLinkedListElement_GetNextAtom (PlankLockFreeLinkedListElementRef p)
{
    return &p->next;
}

