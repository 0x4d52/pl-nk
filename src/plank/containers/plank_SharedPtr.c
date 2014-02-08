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

#include "../core/plank_StandardHeader.h"
#include "plank_SharedPtr.h"

#ifndef PLANKSHAREDPTR_DEBUG
#define PLANKSHAREDPTR_DEBUG 0
#endif

#if PLANKSHAREDPTR_DEBUG
PlankAtomicL* pl_SharedPtrGlobalCount()
{
    static volatile PlankB firstTime = PLANK_TRUE;
    static PLANK_ALIGN(PLANK_WIDESIZE) PlankAtomicL counter;
    
    if (firstTime)
    {
        firstTime = PLANK_FALSE;
        pl_AtomicMemoryBarrier();
        pl_AtomicL_Init (&counter);
    }
    
    return &counter;
}
#endif

//////////////////////////////// Helpers ///////////////////////////////////////

typedef struct PlankSharedPtrCounter
{
    PLANK_ALIGN(PLANK_WIDESIZE) PlankAtomicPX atom;
} PlankSharedPtrCounter PLANK_ALIGN(PLANK_WIDESIZE);

///////////////////////////////// Weak /////////////////////////////////////////

typedef struct PlankWeakPtr
{
    PlankSharedPtr sharedPtr;
    PlankSharedPtrCounterRef sharedCounter;
} PlankWeakPtr PLANK_ALIGN(PLANK_WIDESIZE);

/////////////////////////////// Functions //////////////////////////////////////

static PlankSharedPtrCounterRef pl_SharedPtrCounter_CreateAndInitWithSharedPtr (PlankSharedPtrRef ptr);
static PlankResult pl_SharedPtrCounter_Destroy (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_IncrementRefCount (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_DecrementRefCount (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_IncrementWeakCount (PlankSharedPtrCounterRef p);
static PlankResult pl_SharedPtrCounter_DecrementWeakCount (PlankSharedPtrCounterRef p);

static PlankResult pl_SharedPtr_Destroy (PlankSharedPtrRef p);
static PlankResult pl_SharedPtr_IncrementRefCount (PlankSharedPtrRef p);
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
        pl_AtomicPX_SetAllUnchecked (&p->atom, ptr, 0);
        
#if PLANKSHAREDPTR_DEBUG
        printf("Create Counter  %p (ptr=%p) TOTAL=%ld ", p, ptr, pl_AtomicL_Increment (pl_SharedPtrGlobalCount()));
#endif
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
    
#if PLANKSHAREDPTR_DEBUG
    printf("Destroy Counter %p          TOTAL=%ld\n", p, pl_AtomicL_Decrement (pl_SharedPtrGlobalCount()));
#endif
    if ((result = pl_AtomicPX_DeInit (&p->atom)) != PlankResult_OK)
        goto exit;
    
//    pl_MemoryZero (p, sizeof (PlankSharedPtrCounter));
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

static PlankResult pl_SharedPtrCounter_IncrementRefCount (PlankSharedPtrCounterRef p)
{
    PlankP ptr;
    PlankUL oldExtra, refCount;
    PlankB success;
    
    do
    {
        ptr       = pl_AtomicPX_GetUnchecked (&p->atom);
        oldExtra  = pl_AtomicPX_GetExtraUnchecked (&p->atom);
        refCount  = PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) + 1;
        
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              ptr, oldExtra,
                                              ptr, PLANK_SHAREDPTR_XREFCOUNT(oldExtra, refCount));
    } while (!success);
    
#if PLANKSHAREDPTR_DEBUG
    printf("Counter Ref  ++ %p ptr=%p refCount=%lu weakCount=%lu\n", p, ptr, refCount, PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra));
#endif
    
    return PlankResult_OK;
}

static PlankResult pl_SharedPtrCounter_DecrementRefCount (PlankSharedPtrCounterRef p)
{
    PlankP oldPtr, newPtr;
    PlankUL oldExtra, refCount;
    PlankB success;
    PlankResult result;
    
    result = PlankResult_OK;
    
    do
    {
        oldPtr    = pl_AtomicPX_GetUnchecked (&p->atom);
        oldExtra  = pl_AtomicPX_GetExtraUnchecked (&p->atom);
        refCount  = PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) - 1;
        newPtr    = refCount == 0 ? PLANK_NULL : oldPtr;
        
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              oldPtr, oldExtra,
                                              newPtr, PLANK_SHAREDPTR_XREFCOUNT(oldExtra, refCount));
    } while (!success);
    
#if PLANKSHAREDPTR_DEBUG
    printf("Counter Ref  -- %p ptr=%p refCount=%lu weakCount=%lu\n", p, oldPtr, refCount, PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra));
#endif

    if (refCount == 0)
    {        
        if ((result = pl_SharedPtr_Destroy (oldPtr)) != PlankResult_OK) goto exit;
        
        if (PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) == 0)
            result = pl_SharedPtrCounter_Destroy (p);
    }
    
exit:
    return result;
}

static PlankResult pl_SharedPtrCounter_IncrementWeakCount (PlankSharedPtrCounterRef p)
{
    PlankP ptr;
    PlankUL oldExtra, weakCount;
    PlankB success;
    
    do
    {
        ptr       = pl_AtomicPX_GetUnchecked (&p->atom);
        oldExtra  = pl_AtomicPX_GetExtraUnchecked (&p->atom);
        weakCount  = PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) + 1;
        
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              ptr, oldExtra,
                                              ptr, PLANK_SHAREDPTR_XWEAKCOUNT(oldExtra, weakCount));
    } while (!success);
    
#if PLANKSHAREDPTR_DEBUG
    printf("Counter Weak ++ %p ptr=%p refCount=%lu weakCount=%lu\n", p, ptr, PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra), weakCount);
#endif
    
    return PlankResult_OK;
}

static PlankResult pl_SharedPtrCounter_DecrementWeakCount (PlankSharedPtrCounterRef p)
{
    PlankP ptr;
    PlankUL oldExtra, weakCount;
    PlankB success;
    PlankResult result;
    
    result = PlankResult_OK;
    
    do
    {
        ptr       = pl_AtomicPX_GetUnchecked (&p->atom);
        oldExtra  = pl_AtomicPX_GetExtraUnchecked (&p->atom);
        weakCount  = PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) - 1;
        
        success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)p,
                                              ptr, oldExtra,
                                              ptr, PLANK_SHAREDPTR_XWEAKCOUNT(oldExtra, weakCount));
    } while (!success);
    
#if PLANKSHAREDPTR_DEBUG
    printf("Counter Weak -- %p ptr=%p refCount=%lu weakCount=%lu\n", p, ptr, PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra), weakCount);
#endif

    if ((PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) == 0) && (weakCount == 0))
        result = pl_SharedPtrCounter_Destroy (p);    
    
//exit:
    return result;
}


/////////////////////////////// SharedPtr //////////////////////////////////////

PlankResult pl_SharedPtr_CreateAndInitWithSizeAndFunctions (PlankSharedPtrRef* pp, const PlankL size, const PlankFourCharCode type,  PlankM initFunction, PlankM deInitFunction)
{
    PlankMemoryRef m;
    PlankSharedPtrRef p;
    PlankWeakPtrRef w;
    PlankSharedPtrCounterRef sharedCounter;
    PlankResult result;
    
    if (!pp) return PlankResult_NullPointerError;

    p = PLANK_NULL;
    result = PlankResult_OK;
    
    if (size > 0)
    {
        if (size > 0xFFFFFFFF)
        {
            result = PlankResult_MemoryError;
            goto exit;
        }
        
        m = pl_MemoryGlobal();
        p = (PlankSharedPtrRef)pl_Memory_AllocateBytes (m, size);
        
        if (p != PLANK_NULL)
        {
            pl_MemoryZero (p, size);
            p->size = (PlankUI)size;
            p->type = type;
            sharedCounter = pl_SharedPtrCounter_CreateAndInitWithSharedPtr (p);
            
            if (sharedCounter == PLANK_NULL)
            {
                pl_Memory_Free (m, p);
                p = PLANK_NULL;
                goto exit;
            }
            
            p->sharedCounter = sharedCounter;
            
            if (deInitFunction != PLANK_NULL)
            {
#if PLANKSHAREDPTR_DEBUG
                printf("\n");
#endif
                // not a WeakPtr
                p->deInitFunction = (PlankSharedPtrFunction)deInitFunction;
                
                // allocate a WeakPtr
                pl_SharedPtr_CreateAndInitWithSizeAndFunctions ((PlankSharedPtrRef*)&w, sizeof (PlankWeakPtr), PLANKSHAREDPTR_TYPE_WEAKPTR, PLANK_NULL, PLANK_NULL);
                
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
#if PLANKSHAREDPTR_DEBUG
            else
            {
                printf("[weak]\n");
            }
#endif

            pl_SharedPtr_IncrementRefCount (p);

            if (initFunction != PLANK_NULL)
            {
                result = ((PlankSharedPtrFunction)initFunction) (p);
            }
            
        }
    }
    
exit:
    *pp = p;
    return result;
}

PlankSharedPtrRef pl_SharefPtr_IncrementRefCountAndGetPtr (PlankSharedPtrRef p)
{
    return pl_SharedPtr_IncrementRefCount (p) == PlankResult_OK ? p : PLANK_NULL;
}

static PlankResult pl_SharedPtr_Destroy (PlankSharedPtrRef p)
{
    PlankMemoryRef m;
    PlankResult result;

    if (p->weakPtr != PLANK_NULL)
    {
        pl_SharedPtr_DecrementWeakCount ((PlankSharedPtrRef)p->weakPtr);
        pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)p->weakPtr);
        p->weakPtr = PLANK_NULL;
    }
    
    if (p->deInitFunction)    
        result = (p->deInitFunction) (p);
    
    pl_MemoryZero (p, p->size);

    m = pl_MemoryGlobal();
    return pl_Memory_Free (m, p);
}

PlankResult pl_SharedPtr_DecrementRefCount (PlankSharedPtrRef p)
{
    return p ? (p->sharedCounter ? pl_SharedPtrCounter_DecrementRefCount (p->sharedCounter) : PlankResult_NullPointerError) : PlankResult_NullPointerError;
}

PlankResult pl_SharedPtr_DecrementWeakCount (PlankSharedPtrRef p)
{
    return p ? (p->sharedCounter ? pl_SharedPtrCounter_DecrementWeakCount (p->sharedCounter) : PlankResult_NullPointerError) : PlankResult_NullPointerError;
}

PlankFourCharCode pl_SharedPtr_GetType (PlankSharedPtrRef p)
{
    return p->type;
}

static PlankResult pl_SharedPtr_IncrementRefCount (PlankSharedPtrRef p)
{
    return p ? (p->sharedCounter ? pl_SharedPtrCounter_IncrementRefCount (p->sharedCounter) : PlankResult_NullPointerError) : PlankResult_NullPointerError;
}

static PlankResult pl_SharedPtr_IncrementWeakCount (PlankSharedPtrRef p)
{
    return p ? (p->sharedCounter ? pl_SharedPtrCounter_IncrementWeakCount (p->sharedCounter) : PlankResult_NullPointerError) : PlankResult_NullPointerError;
}

PlankWeakPtrRef pl_SharedPtr_GetWeakPtr (PlankSharedPtrRef p)
{
    PlankResult result;
    PlankWeakPtrRef w;
    
    w = PLANK_NULL;
    
    if (p->weakPtr != PLANK_NULL)
    {
        if ((result = pl_SharedPtr_IncrementWeakCount (p)) != PlankResult_OK)
            goto exit;
            
        if ((result = pl_SharedPtr_IncrementRefCount ((PlankSharedPtrRef)p->weakPtr)) != PlankResult_OK)
            goto exit;
            
        w = p->weakPtr;
    }
    
exit:
    return w;
}

/////----

PlankSharedPtrRef pl_WeakPtr_GetSharedPtr (PlankWeakPtrRef p)
{
    return pl_SharedPtrCounter_IncrementRefCount (p->sharedCounter) == PlankResult_OK ? (PlankSharedPtrRef)pl_AtomicPX_GetUnchecked(&p->sharedCounter->atom) : PLANK_NULL;
}

PlankResult pl_WeakPtr_DecrementRefCount (PlankWeakPtrRef p)
{
    PlankResult result;
    result = pl_SharedPtrCounter_DecrementWeakCount (p->sharedCounter);
    return result == PlankResult_OK ? pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)p) : result;
}

PlankResult pl_SharedPtrSwap (PlankSharedPtrRef* p1, PlankSharedPtrRef* p2)
{
    PlankSharedPtrRef temp;
    pl_AtomicMemoryBarrier();
    temp = *p2;
    *p2 = *p1;
    *p1 = temp;
    return PlankResult_OK;
}

/////----

static PlankResult pl_SharedPtrArray_Init (PlankSharedPtrArrayRef p);
static PlankResult pl_SharedPtrArray_DeInit (PlankSharedPtrArrayRef p);

static PlankResult pl_SharedPtrArray_Init (PlankSharedPtrArrayRef p)
{
#if PLANKSHAREDPTR_DEBUG
    printf("pl_SharedPtrArray_Init (%p)\n",p);
#endif
    return pl_DynamicArray_InitWithItemSize (&p->array, sizeof (PlankAtomicP));
}

static PlankResult pl_SharedPtrArray_DeInit (PlankSharedPtrArrayRef p)
{
#if PLANKSHAREDPTR_DEBUG
    printf("pl_SharedPtrArray_DeInit (%p)\n",p);
#endif
    pl_SharedPtrArray_Clear (p);
    return pl_DynamicArray_DeInit (&p->array);
}

PlankResult pl_SharedPtrArray_CreateSharedPtr (PlankSharedPtrArrayRef* pp, const PlankFourCharCode elementType)
{
    PlankResult result;
    result = pl_SharedPtr_CreateAndInitWithSizeAndFunctions ((PlankSharedPtrRef*)pp,
                                                             sizeof (PlankSharedPtrArray),
                                                             PLANKSHAREDPTR_TYPE_SHAREDPTRARRAY,
                                                             (PlankM)pl_SharedPtrArray_Init,
                                                             (PlankM)pl_SharedPtrArray_DeInit);
    if (result == PlankResult_OK)
        (*pp)->elementType = elementType;
    
    return result;
}

PlankSharedPtrArrayRef pl_SharefPtrArray_IncrementRefCountAndGetPtr (PlankSharedPtrArrayRef p)
{
    return (PlankSharedPtrArrayRef)pl_SharefPtr_IncrementRefCountAndGetPtr ((PlankSharedPtrRef)p);
}

PlankResult pl_SharedPtrArray_DecrementRefCount (PlankSharedPtrArrayRef p)
{
    return pl_SharedPtr_DecrementRefCount((PlankSharedPtrRef)p);
}

PlankResult pl_SharedPtrArray_SetElementType (PlankSharedPtrArrayRef p, const PlankFourCharCode type)
{
    p->elementType = type;
    return PlankResult_OK;
}

PlankFourCharCode pl_SharedPtrArray_GetElementType (PlankSharedPtrArrayRef p)
{
    return p->elementType;
}

PlankResult pl_SharedPtrArray_Clear (PlankSharedPtrArrayRef p)
{
    PlankResult result;
    PlankL size, i;
    PlankAtomicP* array;
    PlankAtomicP temp;
    
    pl_AtomicP_Init (&temp);
    
    result = PlankResult_OK;
    size = pl_DynamicArray_GetSize (&p->array);
    array = (PlankAtomicP*)pl_DynamicArray_GetArray (&p->array);
    
    for (i = 0; i < size; ++i)
    {
        temp.ptr = PLANK_NULL;
        pl_AtomicP_SwapOther (&array[i], &temp);
        pl_SharedPtr_DecrementRefCount (temp.ptr);
        pl_AtomicP_DeInit (&array[i]);
    }
    
    pl_DynamicArray_SetSize (&p->array, 0);
    
//exit:
    pl_AtomicP_DeInit (&temp);
    
    return result;
}

PlankL pl_SharedPtrArray_GetLength (PlankSharedPtrArrayRef p)
{
    return pl_DynamicArray_GetSize (&p->array);
}

PlankAtomicP* pl_SharedPtrArray_GetArray (PlankSharedPtrArrayRef p)
{
    return (PlankAtomicP*)pl_DynamicArray_GetArray (&p->array);
}

PlankResult pl_SharedPtrArray_AddSharedPtr (PlankSharedPtrArrayRef p, PlankSharedPtrRef item)
{
    PlankResult result;
    PlankAtomicP atom;
    
    result = PlankResult_OK;
    
    pl_AtomicP_Init (&atom);
    if ((result = pl_SharedPtr_IncrementRefCount (item)) != PlankResult_OK) goto exit;
    
    atom.ptr = item;
    if ((result = pl_DynamicArray_AddItem (&p->array, &item)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_SharedPtrArray_PutSharedPtr (PlankSharedPtrArrayRef p, const PlankL index, PlankSharedPtrRef item)
{
    PlankResult result;
    PlankAtomicP atom;

    result = PlankResult_OK;
    
    if ((index < 0) || (index >= p->array.usedItems))
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }
    
    if (p->elementType && (p->elementType != item->type))
    {
        result = PlankResult_SharedPtrTypeError;
        goto exit;
    }
    
    pl_AtomicP_Init (&atom);
    if ((result = pl_SharedPtr_IncrementRefCount (item)) != PlankResult_OK) goto exit;
    
    atom.ptr = item;
    if ((result = pl_DynamicArray_PutItem (&p->array, index, &item)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_SharedPtrArray_RemoveSharedPtr (PlankSharedPtrArrayRef p, const PlankL index)
{
    PlankAtomicP atom;
    PlankResult result;
    result = PlankResult_OK;

    if ((index < 0) || (index >= p->array.usedItems))
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }

    if ((result = pl_DynamicArray_GetItem (&p->array, index, &atom)) != PlankResult_OK) goto exit;
    if ((result = pl_SharedPtr_DecrementRefCount ((PlankSharedPtrRef)atom.ptr)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_RemoveItem (&p->array, index)) != PlankResult_OK) goto exit;

    pl_AtomicP_DeInit (&atom);
    
exit:
    return result;
}

//PlankSharedPtrRef pl_SharedPtrArray_GetSharedPtr (PlankSharedPtrArrayRef p, const PlankL index)
//{
//    PlankAtomicP atom;
//    
//    pl_AtomicP_Init (&atom);
//    pl_DynamicArray_GetItem (&p->array, index, &atom);
//    
//    return (PlankSharedPtrRef)atom.ptr;
//}

PlankSharedPtrRef pl_SharedPtrArray_GetSharedPtr (PlankSharedPtrArrayRef p, const PlankL index)
{
    PlankAtomicP* array;
    PlankSharedPtr* sharedPtr;
    
    if ((index < 0) || (index >= p->array.usedItems))
        return PLANK_NULL;

    array = (PlankAtomicP*)pl_DynamicArray_GetArray (&p->array);
    
    sharedPtr = (PlankSharedPtrRef)array[index].ptr;
    
    return (p->elementType && (p->elementType != sharedPtr->type)) ? PLANK_NULL : sharedPtr;
}


PlankResult pl_SharedPtrArray_Swap (PlankSharedPtrArrayRef p, const PlankL indexA, const PlankL indexB)
{
    PlankAtomicP* array;
    
    if ((indexA < 0) || (indexA >= p->array.usedItems) || (indexB < 0) || (indexB >= p->array.usedItems) )
    {
        return PlankResult_ArrayParameterError;
    }
    
    if (indexA != indexB)
    {
        array = (PlankAtomicP*)pl_DynamicArray_GetArray (&p->array);
        pl_AtomicP_SwapOther (&array[indexA], &array[indexB]);
    }
    
    return PlankResult_OK;
}

//PlankResult pl_SharedPtrArray_Sort (PlankSharedPtrArrayRef p, PlankSharedPtrArrayCompareFunction comparator)
//{
//    return pl_DynamicArray_Sort (&p->array, (PlankDynamicArrayCompareFunction)comparator);
//}

PlankResult pl_SharedPtrArray_Sort (PlankSharedPtrArrayRef p, PlankSharedPtrArrayCompareFunction comparator)
{
    // yikes - a bubble sort - must improve this...
    PlankResult result = PlankResult_OK;
    PlankL i, j;
    PlankB swapped;
    PlankAtomicP* array;

    array = (PlankAtomicP*)pl_DynamicArray_GetArray (&p->array);
    
    if (comparator == PLANK_NULL)
    {
        result = PlankResult_FunctionsInvalid;
        goto exit;
    }
    
    for (i = p->array.usedItems; --i >= 0;)
    {
        swapped = PLANK_FALSE;
        
        for (j = 0; j < i; j++)
        {            
            if (((PlankDynamicArrayCompareFunction)comparator) (array[j].ptr, array[j + 1].ptr))
            {
                pl_AtomicP_SwapOther (&array[j], &array[j + 1]);
                swapped = PLANK_TRUE;
            }
        }
        
        if (!swapped)
            goto exit;
    }
    
exit:
    return result;
}
