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

#ifndef PLANK_SHAREDPTR_H
#define PLANK_SHAREDPTR_H

#include "plank_DynamicArray.h"

#define PLANKSHAREDPTR_CREATEANDINIT_DEFINE(NAME)\
    PlankResult pl_##NAME##_CreateSharedPtr (Plank##NAME##Ref* pp) {\
        return pl_SharedPtr_CreateAndInitWithSizeAndFunctions ((PlankSharedPtrRef*)pp, sizeof (Plank##NAME),(PlankSharedPtrFunction)pl_##NAME##_Init,(PlankSharedPtrFunction)pl_##NAME##_DeInit);\
    }

#define PLANKSHAREDPTR_INCREMENTREFCOUNTANDGET_DEFINE(NAME)\
    Plank##NAME##Ref pl_##NAME##_IncrementRefCountAndGet (Plank##NAME##Ref p) {\
        return (Plank##NAME##Ref)pl_SharefPtr_IncrementRefCountAndGetPtr ((PlankSharedPtrRef)p);\
    }

#define PLANKSHAREDPTR_DECREMENTREFCOUNT_DEFINE(NAME)\
    PlankResult pl_##NAME##_DecrementRefCount (Plank##NAME##Ref p) {\
        return pl_SharedPtr_DecrementRefCount((PlankSharedPtrRef)p);\
    }

#define PLANKSHAREDPTR_GETWEAKPTR_DEFINE(NAME)\
    PlankWeakPtrRef pl_##NAME##_GetWeakPtr (Plank##NAME##Ref p){\
        return pl_SharedPtr_GetWeakPtr ((PlankSharedPtrRef)p);\
    }

#define PLANKSHAREDPTR_GETFROMWEAKPTR_DEFINE(NAME)\
    Plank##NAME##Ref pl_##NAME##GetFromWeakPtr (PlankWeakPtrRef w){\
        return (Plank##NAME##Ref)pl_WeakPtr_GetSharedPtr (w);\
    }

PLANK_BEGIN_C_LINKAGE

/** A shared pointer.
 
 @defgroup PlankSharedPtrClass Plank SharedPtr class
 @ingroup PlankClasses
 @{
 */

typedef struct PlankSharedPtr* PlankSharedPtrRef;
typedef struct PlankWeakPtr* PlankWeakPtrRef;
typedef struct PlankSharedPtrCounter* PlankSharedPtrCounterRef;
typedef PlankResult (*PlankSharedPtrFunction)(void*);

PlankResult pl_SharedPtr_CreateAndInitWithSizeAndFunctions (PlankSharedPtrRef* pp, const PlankL size, void* initFunction, void* deInitFunction);
PlankSharedPtrRef pl_SharefPtr_IncrementRefCountAndGetPtr (PlankSharedPtrRef p);
PlankResult pl_SharedPtr_DecrementRefCount (PlankSharedPtrRef p);
PlankResult pl_SharedPtr_DecrementWeakCount (PlankSharedPtrRef p);
PlankWeakPtrRef pl_SharedPtr_GetWeakPtr (PlankSharedPtrRef p);

PlankSharedPtrRef pl_WeakPtr_GetSharedPtr (PlankWeakPtrRef p);
PlankResult pl_WeakPtr_DecrementRefCount (PlankWeakPtrRef p);

PlankResult pl_SharedPtrSwap (PlankSharedPtrRef* p1, PlankSharedPtrRef* p2);
/** @} */

/** An array of shared pointers.
 
 @defgroup PlankSharedPtrClass Plank SharedPtr class
 @ingroup PlankClasses
 @{
 */

typedef struct PlankSharedPtrArray* PlankSharedPtrArrayRef;

PlankResult pl_SharedPtrArray_CreateSharedPtr (PlankSharedPtrArrayRef* pp);
PlankSharedPtrArrayRef pl_SharefPtrArray_IncrementRefCountAndGetPtr (PlankSharedPtrArrayRef p);
PlankResult pl_SharedPtrArray_DecrementRefCount (PlankSharedPtrArrayRef p);
PlankResult pl_SharedPtrArray_Clear (PlankSharedPtrArrayRef p);
PlankL pl_SharedPtrArray_GetLength (PlankSharedPtrArrayRef p);
PlankAtomicP* pl_SharedPtrArray_GetArray (PlankSharedPtrArrayRef p);
PlankResult pl_SharedPtrArray_AddSharedPtr (PlankSharedPtrArrayRef p, PlankSharedPtrRef item);
PlankResult pl_SharedPtrArray_PutSharedPtr (PlankSharedPtrArrayRef p, const PlankL index, PlankSharedPtrRef item);
PlankResult pl_SharedPtrArray_RemoveSharedPtr (PlankSharedPtrArrayRef p, const PlankL index);
PlankSharedPtrRef pl_SharedPtrArray_GetSharedPtr (PlankSharedPtrArrayRef p, const PlankL index);
PlankResult pl_SharedPtrArray_Swap (PlankSharedPtrArrayRef p, const PlankL indexA, const PlankL indexB);


/** @} */

PLANK_END_C_LINKAGE

//////////////////////////////// /////// ///////////////////////////////////////

#if !DOXYGEN
typedef struct PlankSharedPtr
{
    PlankL size;
    PlankSharedPtrCounterRef sharedCounter;
    PlankSharedPtrFunction deInitFunction;
    PlankWeakPtrRef weakPtr;
} PlankSharedPtr;

typedef struct PlankSharedPtrArray
{
    PlankSharedPtr sharedHeader;
    PlankDynamicArray array;
} PlankSharedPtrArray;
#endif

#endif // PLANK_SHAREDPTR_H