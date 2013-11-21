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

#include "../core/plank_StandardHeader.h"
#include "plank_SharedPtr.h"

typedef struct PlankSharedPtrCounts
{
    unsigned int refCount:PLANK_HALFWORDBITS;
    unsigned int weakCount:PLANK_HALFWORDBITS;
} PlankSharedPtrCounts;

typedef struct PlankSharedPtrParts
{
    PlankP* ptr;
    PlankSharedPtrCounts counts;
} PlankSharedPtrParts;

typedef struct PlankSharedPtrHalves
{
    PlankP* ptr;
    PlankL  extra;
} PlankSharedPtrHalves;

typedef union PlankSharedPtrElement
{
    PlankSharedPtrParts  parts;
    PlankSharedPtrHalves halves;
} PlankSharedPtrElement;

typedef struct PlankSharedPtrCounter
{
    PlankAtomicPX atom;
} PlankSharedPtrCounter;

typedef struct PlankSharedPtrCounter* PlankSharedPtrCounterRef;

typedef struct PlankSharedPtr
{
    PlankSharedPtrCounterRef sharedCounter;
    PlankWeakPtrRef weakPtr;
    PlankSharedPtrFreeFunction freeFunction;
} PlankSharedPtr;

typedef struct PlankWeakPtr
{
    PlankSharedPtr sharedPtr;
    PlankSharedPtrCounterRef sharedCounter;
} PlankWeakPtr;


static PlankResult pl_SharedPtr_DestroyPtr (PlankSharedPtrRef p, PlankP ptr);
static PlankResult pl_SharedPtr_InitWithPtrAndFreeFunction (PlankSharedPtrRef p, PlankP ptr, void* freeFunction);
static PlankResult pl_SharedPtr_DeInit (PlankSharedPtrRef p);
static PlankResult pl_SharedPtr_Destroy (PlankSharedPtrRef p);
static PlankResult pl_SharedPtr_IncrementWeakCount (PlankSharedPtrRef p);

static PlankSharedPtrCounterRef pl_SharedPtrCounter_CreateAndInitWithPtr (PlankP ptr)
{
    PlankMemoryRef m;
    PlankSharedPtrCounterRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankSharedPtrCounterRef)pl_Memory_AllocateBytes (m, sizeof (PlankSharedPtrCounter));
    
    if (p != PLANK_NULL)
    {
        pl_MemoryZero (p, sizeof (PlankSharedPtrCounter));
        pl_AtomicPX_Init (&p->atom);
        p->atom.ptr = ptr;
        
        printf("pl_SharedPtrCounter_CreateAndInitWithPtr(%p) [%p]\n", p, ptr);
    }
    
    return p;
}

static PlankResult pl_SharedPtrCounter_Destroy (PlankSharedPtrCounterRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_AtomicPX_DeInit (&p->atom)) != PlankResult_OK) goto exit;
    
    result = pl_Memory_Free (m, p);
    
    printf("pl_SharedPtrCounter_Destroy(%p)\n", p);
    
exit:
    return result;
}

PlankSharedPtrRef pl_SharedPtr_CreateAndInitWithPtrAndFreeFunction (PlankP ptr, void* freeFunction)
{
    PlankMemoryRef m;
    PlankSharedPtrRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankSharedPtrRef)pl_Memory_AllocateBytes (m, sizeof (PlankSharedPtr));
    
    if (p != PLANK_NULL)
    {        
        if (pl_SharedPtr_InitWithPtrAndFreeFunction (p, ptr, freeFunction) != PlankResult_OK)
        {
            pl_Memory_Free (m, p);
            return PLANK_NULL;
        }        
    }
        
    return p;
}

static PlankResult pl_SharedPtr_InitWithPtrAndFreeFunction (PlankSharedPtrRef p, PlankP ptr, void* freeFunction)
{
    PlankResult result;
    PlankMemoryRef m;
    PlankWeakPtrRef w;

    result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankSharedPtr));

    p->sharedCounter = pl_SharedPtrCounter_CreateAndInitWithPtr (ptr);
    
    if (!p->sharedCounter)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
            
    if (freeFunction)
    {
        p->freeFunction = freeFunction;

        // a null freeFunction indicates this contains a PlankWeakPtr so don't add another weak
        // otherwise, allocate a PlankWeakPtr
        
        m = pl_MemoryGlobal();
        w = (PlankWeakPtrRef)pl_Memory_AllocateBytes (m, sizeof (PlankWeakPtr));
        
        if (w != PLANK_NULL)
        {
            pl_MemoryZero (w, sizeof (PlankWeakPtr));
            
            pl_SharedPtr_InitWithPtrAndFreeFunction (&w->sharedPtr, w, PLANK_NULL);
            
            w->sharedCounter = p->sharedCounter;
            pl_SharedPtr_IncrementWeakCount (&w->sharedPtr);
//            pl_SharedPtr_IncrementWeakCount (p);
            p->weakPtr = w;
        }
    }
    
    printf("pl_SharedPtr_InitWithPtrAndFreeFunction(%p) [%p] sharedCounter=%p %s\n", p, ptr, p->sharedCounter, p->freeFunction ? "" : "[weak]");

    pl_SharedPtr_IncrementRefCount (p);
    
exit:
    return result;
}

static PlankResult pl_SharedPtr_DestroyPtr (PlankSharedPtrRef p, PlankP ptr)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    
    printf("pl_SharedPtr_DestroyPtr(%p) [%p] sharedCounter=%p %s\n", p, ptr, p->sharedCounter, p->freeFunction ? "" : "[weak]");

    if (ptr != PLANK_NULL)
    {
        if (p->freeFunction != PLANK_NULL)
        {
            (p->freeFunction) (ptr);
            
            if (ptr != p)
            {
                if ((result = pl_SharedPtr_Destroy (p)) != PlankResult_OK) goto exit;
            }
        }
        else
        {
            m = pl_MemoryGlobal();
            result = pl_Memory_Free (m, ptr);
        }
    }
    
exit:
    return PlankResult_OK;
}

static PlankResult pl_SharedPtr_DeInit (PlankSharedPtrRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    PlankWeakPtrRef weakPtr;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    weakPtr = p->weakPtr;
    
    if (weakPtr)
    {
        p->weakPtr = 0;
        pl_SharedPtr_DecrementWeakCount ((PlankSharedPtrRef)weakPtr);
        pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)weakPtr);
    }
    
exit:
    return result;
}

static PlankResult pl_SharedPtr_Destroy (PlankSharedPtrRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_SharedPtr_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
    printf("pl_SharedPtr_Destroy(%p)\n", p);
    
exit:
    return result;
}

PlankResult pl_SharedPtr_DecrementRefCount (PlankSharedPtrRef p)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    PlankResult result;
    PlankSharedPtrCounterRef sharedCounter;
    
    result = PlankResult_OK;
    sharedCounter = p->sharedCounter;
        
    do
    {
        oldElement.halves.ptr   = newElement.halves.ptr   = sharedCounter->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = sharedCounter->atom.extra;
        
        if (--newElement.parts.counts.refCount == 0)
            newElement.parts.ptr = 0;
        
        success = pl_AtomicPX_CompareAndSwap (&sharedCounter->atom,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if (newElement.parts.counts.refCount == 0)
    {
        if ((result = pl_SharedPtr_DestroyPtr (p, oldElement.parts.ptr)) != PlankResult_OK) goto exit;
        
        if (newElement.parts.counts.weakCount == 0)
        {
            p->sharedCounter = PLANK_NULL;
            result = pl_SharedPtrCounter_Destroy (sharedCounter);
        }
    }
    
    printf("pl_SharedPtr_DecrementRefCount(%p) sharedCounter=%p ref=%d weak=%d\n",
           p, sharedCounter,
           newElement.parts.counts.refCount,newElement.parts.counts.weakCount);
    
exit:
    return result;
}

PlankResult pl_SharedPtrDecrementSharedCounterWeakCount (PlankSharedPtrCounterRef *sharedCounterRef)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    PlankResult result;
    PlankSharedPtrCounterRef sharedCounter;
    
    result = PlankResult_OK;
    sharedCounter = *sharedCounterRef;

    do
    {
        oldElement.halves.ptr   = newElement.halves.ptr   = sharedCounter->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = sharedCounter->atom.extra;
        --newElement.parts.counts.weakCount;
        
        success = pl_AtomicPX_CompareAndSwap (&sharedCounter->atom,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if ((newElement.parts.counts.refCount == 0) && (newElement.parts.counts.weakCount == 0))
    {
        *sharedCounterRef = PLANK_NULL;
        result = pl_SharedPtrCounter_Destroy (sharedCounter);
    }
    
    printf("pl_SharedPtrDecrementSharedCounterWeakCount() sharedCounter=%p ref=%d weak=%d\n",
           sharedCounter,
           newElement.parts.counts.refCount,newElement.parts.counts.weakCount);
    
exit:
    return result;
}

PlankResult pl_SharedPtr_DecrementWeakCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrDecrementSharedCounterWeakCount (&p->sharedCounter);
}

static PlankResult pl_SharedPtrIncrementSharedCounterRefCount (PlankSharedPtrCounterRef* sharedCounterRef)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    PlankSharedPtrCounterRef sharedCounter;
    
    sharedCounter = *sharedCounterRef;
    
    do
    {
        oldElement.halves.ptr   = newElement.halves.ptr   = sharedCounter->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = sharedCounter->atom.extra;
        ++newElement.parts.counts.refCount; // in multithreaded, we might be incrementing after the pointer was deleted and zereo'd
        
        success = pl_AtomicPX_CompareAndSwap (&sharedCounter->atom,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    printf("pl_SharedPtrIncrementSharedCounterRefCount() sharedCounter=%p ref=%d weak=%d\n",
           sharedCounter,
           newElement.parts.counts.refCount,newElement.parts.counts.weakCount);

    
    return PlankResult_OK;
}

PlankResult pl_SharedPtr_IncrementRefCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrIncrementSharedCounterRefCount (&p->sharedCounter);
}

static PlankResult pl_SharedPtrIncrementSharedCounterWeakCount (PlankSharedPtrCounterRef* sharedCounterRef)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    PlankSharedPtrCounterRef sharedCounter;

    sharedCounter = *sharedCounterRef;

    do
    {
        oldElement.halves.ptr   = newElement.halves.ptr   = sharedCounter->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = sharedCounter->atom.extra;
        ++newElement.parts.counts.weakCount;
        
        success = pl_AtomicPX_CompareAndSwap (&sharedCounter->atom,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    printf("pl_SharedPtrIncrementSharedCounterWeakCount() sharedCounter=%p ref=%d weak=%d\n",
           sharedCounter,
           newElement.parts.counts.refCount, newElement.parts.counts.weakCount);
    
    return PlankResult_OK;
}

static PlankResult pl_SharedPtr_IncrementWeakCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrIncrementSharedCounterWeakCount (&p->sharedCounter);
}

PlankP pl_SharedPtr_GetPtr (PlankSharedPtrRef p)
{
    return pl_AtomicPX_Get (&p->sharedCounter->atom);
}

PlankWeakPtrRef pl_SharedPtr_GetWeakPtr (PlankSharedPtrRef p)
{
    if (p->weakPtr == PLANK_NULL)
        return PLANK_NULL;
    
    pl_SharedPtr_IncrementRefCount ((PlankSharedPtrRef)p->weakPtr);
    
    return (PlankWeakPtrRef)p->weakPtr;
}

PlankUI pl_SharedPtr_GetRefCount (PlankSharedPtrRef p)
{
    PlankSharedPtrElement e;
    e.halves.extra = pl_AtomicPX_GetExtra (&p->sharedCounter->atom); // could this be the unchecked version?
    return e.parts.counts.refCount;
}

PlankUI pl_SharedPtr_GetWeakCount (PlankSharedPtrRef p)
{
    PlankSharedPtrElement e;
    e.halves.extra = pl_AtomicPX_GetExtra (&p->sharedCounter->atom); // could this be the unchecked version?
    return e.parts.counts.weakCount;
}

/////----

PlankSharedPtrRef pl_WeakPtr_GetSharedPtr (PlankWeakPtrRef p)
{
    PlankSharedPtrRef sharedPtr = PLANK_NULL;

    if (p != PLANK_NULL)
    {
        pl_SharedPtrIncrementSharedCounterRefCount (&p->sharedCounter);
        pl_SharedPtr_IncrementRefCount ((PlankSharedPtrRef)p);
        sharedPtr = pl_AtomicPX_Get (&p->sharedCounter->atom);
        pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)p);
    }
    
    return sharedPtr;
}

PlankResult pl_WeakPtr_DecrementRefCount (PlankWeakPtrRef p)
{
    return pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)p);
}


