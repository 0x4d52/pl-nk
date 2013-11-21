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

//////////////////////////////// Helpers ///////////////////////////////////////

typedef struct PlankSharedPtrCounts
{
    unsigned int refCount:PLANK_HALFWORDBITS;
    unsigned int weakCount:PLANK_HALFWORDBITS;
} PlankSharedPtrCounts;

typedef struct PlankSharedPtrParts
{
    PlankSharedPtrRef ptr;
    PlankSharedPtrCounts counts;
} PlankSharedPtrParts;

typedef struct PlankSharedPtrHalves
{
    PlankSharedPtrRef ptr;
    PlankL extra;
} PlankSharedPtrHalves;

typedef union PlankSharedPtrElement
{
    PlankSharedPtrParts  parts;
    PlankSharedPtrHalves halves;
} PlankSharedPtrElement;

/////////////////////////////// Functions //////////////////////////////////////

static PlankSharedPtrCounterRef pl_SharedPtrCounter_CreateAndInitWithSharedPtr (PlankSharedPtrRef ptr);
static PlankResult pl_SharedPtrCounter_Destroy (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_IncrementRefCount (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_DecrementRefCount (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_IncrementWeakCount (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_DecrementWeakCount (PlankSharedPtrCounterRef p);

static PlankResult pl_SharedPtr_Destroy (PlankSharedPtrRef p);
static PlankResult pl_SharedPtr_IncrementWeakCount (PlankSharedPtrRef p);

//////////////////////////////// Counter ///////////////////////////////////////

static PlankSharedPtrCounterRef pl_SharedPtrCounter_CreateAndInitWithSharedPtr (PlankSharedPtrRef ptr)
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
        
        printf("Create Counter  %p ptr=%p", p, ptr);
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
    
    printf("Destroy Counter %p ptr=%p\n", p, p->atom.ptr);
    
    if ((result = pl_AtomicPX_DeInit (&p->atom)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

static PlankResult pl_SharedPtrCounter_IncrementRefCount (PlankSharedPtrCounterRef p)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = p->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = p->atom.extra;
        ++newElement.parts.counts.refCount; // in multithreaded, we might be incrementing after the pointer was deleted and zereo'd
                
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    return PlankResult_OK;
}

static PlankResult pl_SharedPtrCounter_DecrementRefCount (PlankSharedPtrCounterRef p)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    PlankResult result;
    
    result = PlankResult_OK;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = p->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = p->atom.extra;
        
        if (--newElement.parts.counts.refCount == 0)
            newElement.parts.ptr = 0;
        
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if (newElement.parts.counts.refCount == 0)
    {        
        if ((result = pl_SharedPtr_Destroy (oldElement.parts.ptr)) != PlankResult_OK) goto exit;
        
        if (newElement.parts.counts.weakCount == 0)
            result = pl_SharedPtrCounter_Destroy (p);
    }

exit:
    return result;
}

static PlankResult pl_SharedPtrCounter_IncrementWeakCount (PlankSharedPtrCounterRef p)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = p->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = p->atom.extra;
        ++newElement.parts.counts.weakCount;
                
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    return PlankResult_OK;
}

static PlankResult pl_SharedPtrCounter_DecrementWeakCount (PlankSharedPtrCounterRef p)
{
    PlankSharedPtrElement oldElement, newElement;
    PlankB success;
    PlankResult result;
    
    result = PlankResult_OK;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = p->atom.ptr;
        oldElement.halves.extra = newElement.halves.extra = p->atom.extra;
        --newElement.parts.counts.weakCount;
        
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              oldElement.halves.ptr, oldElement.halves.extra,
                                              newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if ((newElement.parts.counts.refCount == 0) && (newElement.parts.counts.weakCount == 0))
        result = pl_SharedPtrCounter_Destroy (p);
        
exit:
    return result;
}


/////////////////////////////// SharedPtr //////////////////////////////////////

PlankSharedPtrRef pl_SharedPtr_CreateAndInitWithSizeAndFunctions (const PlankL size, void* initFunction, void* deInitFunction)
{
    PlankMemoryRef m;
    PlankSharedPtrRef p;
    PlankWeakPtrRef w;
    PlankSharedPtrCounterRef sharedCounter;
    
    p = PLANK_NULL;
    
    if (size > 0)
    {
        m = pl_MemoryGlobal();
        p = (PlankSharedPtrRef)pl_Memory_AllocateBytes (m, size);
        
        if (p != PLANK_NULL)
        {
            pl_MemoryZero (p, size);
            p->size = size;
            sharedCounter = pl_SharedPtrCounter_CreateAndInitWithSharedPtr (p);
            
            if (sharedCounter == PLANK_NULL)
            {
                pl_Memory_Free (m, p);
                p = PLANK_NULL;
                goto exit;
            }
            
            p->sharedCounter = sharedCounter;
            
            if (deInitFunction == PLANK_NULL)
            {
                printf("[weak]\n");
            }
            else
            {
                printf("\n");

                // not a WeakPtr
                p->deInitFunction = deInitFunction;
                
                // allocate a WeakPtr
                w = (PlankWeakPtrRef)pl_SharedPtr_CreateAndInitWithSizeAndFunctions (sizeof (PlankWeakPtr), PLANK_NULL, PLANK_NULL); // nulls say "I'm a WeakPtr"
                
                if (w == PLANK_NULL)
                {
                    pl_SharedPtr_Destroy (p);
                    p = PLANK_NULL;
                    goto exit;
                }
                
                w->sharedCounter = sharedCounter;
                pl_SharedPtr_IncrementWeakCount ((PlankSharedPtrRef)w);
                p->weakPtr = w;
            }
            
            pl_SharedPtr_IncrementRefCount (p);

            if (initFunction != PLANK_NULL)
                ((PlankSharedPtrFunction)initFunction) (p);
        }
    }
    
exit:
    return p;
}

static PlankResult pl_SharedPtr_Destroy (PlankSharedPtrRef p)
{
    PlankMemoryRef m;

    if (p->weakPtr != PLANK_NULL)
    {
        pl_SharedPtr_DecrementWeakCount ((PlankSharedPtrRef)p->weakPtr);
        pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)p->weakPtr);
        p->weakPtr = PLANK_NULL;
    }
    
    if (p->deInitFunction)    
        (p->deInitFunction) (p);
    
    m = pl_MemoryGlobal();
    pl_Memory_Free (m, p);
    
    return PlankResult_OK;
}

PlankResult pl_SharedPtr_DecrementRefCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrCounter_DecrementRefCount (p->sharedCounter);
}

PlankResult pl_SharedPtr_DecrementWeakCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrCounter_DecrementWeakCount (p->sharedCounter);
}

PlankResult pl_SharedPtr_IncrementRefCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrCounter_IncrementRefCount (p->sharedCounter);
}

static PlankResult pl_SharedPtr_IncrementWeakCount (PlankSharedPtrRef p)
{
    return pl_SharedPtrCounter_IncrementWeakCount (p->sharedCounter);
}

PlankWeakPtrRef pl_SharedPtr_GetWeakPtr (PlankSharedPtrRef p)
{
    PlankResult result;
    PlankWeakPtrRef w;
    
    w = PLANK_NULL;
    
    if ((result = pl_SharedPtr_IncrementWeakCount (p)) == PlankResult_OK)
        w = p->weakPtr;

    return w;
}

/////----

PlankSharedPtrRef pl_WeakPtr_GetSharedPtr (PlankWeakPtrRef p)
{
    pl_SharedPtrCounter_IncrementRefCount (p->sharedCounter);
    return (PlankSharedPtrRef)p->sharedCounter->atom.ptr;
}

PlankResult pl_WeakPtr_DecrementRefCount (PlankWeakPtrRef p)
{
    return pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)p);
}


