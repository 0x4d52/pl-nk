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

// help prevent accidental inclusion other than via the intended header
#if PLANK_INLINING_FUNCTIONS

#include "../../../core/plank_Lock.h"

#if PLANK_32BIT
    #define PLANK_ATOMIC_XBITS          32
    #define PLANK_ATOMIC_XREFCOUNTBITS  16
    #define PLANK_ATOMIC_XWEAKCOUNTBITS 16
    #define PLANK_ATOMIC_XREFCOUNTMAX   0x0000FFFFUL
    #define PLANK_ATOMIC_XREFCOUNTMASK  PLANK_ATOMIC_XREFCOUNTMAX
    #define PLANK_ATOMIC_XWEAKCOUNTMAX  0x0000FFFFUL
    #define PLANK_ATOMIC_XWEAKCOUNTMASK 0xFFFF0000UL
    #define PLANK_ATOMIC_XMAX           0xFFFFFFFFUL
    #define PLANK_ATOMIC_PMASK          0xFFFFFFFFUL
#endif

#if PLANK_64BIT
    #define PLANK_ATOMIC_XBITS          64
    #define PLANK_ATOMIC_XREFCOUNTBITS  32
    #define PLANK_ATOMIC_XWEAKCOUNTBITS 32
    #define PLANK_ATOMIC_XREFCOUNTMAX   0x00000000FFFFFFFFUL
    #define PLANK_ATOMIC_XREFCOUNTMASK  PLANK_ATOMIC_XREFCOUNTMAX
    #define PLANK_ATOMIC_XWEAKCOUNTMAX  0x00000000FFFFFFFFUL
    #define PLANK_ATOMIC_XWEAKCOUNTMASK 0xFFFFFFFF00000000UL
    #define PLANK_ATOMIC_XMAX           0xFFFFFFFFFFFFFFFFUL
    #define PLANK_ATOMIC_PMASK          0xFFFFFFFFFFFFFFFFUL
#endif

#if !DOXYGEN
typedef struct PlankAtomicI
{
    PlankI value;
    PlankLock lock;
} PlankAtomicI;

typedef struct PlankAtomicL
{
    PlankL value;
    PlankLock lock;
} PlankAtomicL;

typedef struct PlankAtomicLL
{
    PlankLL value;
    PlankLock lock;
} PlankAtomicLL;

typedef struct PlankAtomicF
{
    PlankF value;
    PlankLock lock;
} PlankAtomicF;

typedef struct PlankAtomicD
{
    PlankD value;
    PlankLock lock;
} PlankAtomicD;

typedef struct PlankAtomicP
{
    PlankP ptr;
    PlankLock lock;
} PlankAtomicP;

typedef struct PlankAtomicPX
{
    PlankP ptr;
    PlankUL extra;
    PlankLock lock;
} PlankAtomicPX;
#endif

static PlankLock* getBarrierLock()
{
    static PlankLock lock;
    static PlankB firstTime = PLANK_TRUE;
    
    if (firstTime)
    {
        pl_Lock_Init (&lock);
        firstTime = PLANK_FALSE;
    }
    
    return &lock;
}

static inline void pl_AtomicMemoryBarrier()
{
    if (pl_Lock_TryLock (getBarrierLock()))
        pl_Lock_Unlock (getBarrierLock());
}

//------------------------------------------------------------------------------
static PlankResult pl_AtomicI_Init (PlankAtomicIRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_MemoryZero (p, sizeof (PlankAtomicI));
    pl_Lock_Init (&p->lock);
    
    return PlankResult_OK;
}

static PlankResult pl_AtomicI_DeInit (PlankAtomicIRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_Lock_DeInit (&p->lock);
    
    return PlankResult_OK;
}

static inline PlankI pl_AtomicI_Get (PlankAtomicIRef p)
{
    PlankI result;
    pl_Lock_Lock (&p->lock);
    result = p->value;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankI pl_AtomicI_GetUnchecked (PlankAtomicIRef p)
{
    return p->value;
}

static inline PlankI pl_AtomicI_Swap (PlankAtomicIRef p, PlankI newValue)
{
    PlankI oldValue;
    PlankB success;
    
    do {
        oldValue = *(PlankI*)p;
        success = pl_AtomicI_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return oldValue;
}

static inline void pl_AtomicI_SwapOther (PlankAtomicIRef p1, PlankAtomicIRef p2)
{
    PlankI value1, value2;
    PlankB success;
    
    do {
        value1 = *(PlankI*)p1;
        value2 = *(PlankI*)p2;
        success = pl_AtomicI_CompareAndSwap (p1, value1, value2);
    } while (!success);
    
    *(PlankI*)p2 = value1;
}

static inline void pl_AtomicI_Set (PlankAtomicIRef p, PlankI newValue)
{
    pl_AtomicI_Swap (p, newValue);
}

static inline PlankI pl_AtomicI_Add (PlankAtomicIRef p, PlankI operand)
{
    PlankI oldValue, newValue;
    PlankB success;
    
    do {
        oldValue = *(PlankI*)p;
        newValue = oldValue + operand;
        success = pl_AtomicI_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return newValue;
}

static inline PlankB  pl_AtomicI_CompareAndSwap (PlankAtomicIRef p, PlankI oldValue, PlankI newValue)
{
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}

static inline PlankI pl_AtomicI_Subtract (PlankAtomicIRef p, PlankI operand)
{
    return pl_AtomicI_Add (p, -operand);
}

static inline PlankI pl_AtomicI_Increment (PlankAtomicIRef p)
{
    return pl_AtomicI_Add (p, 1);
}

static inline PlankI pl_AtomicI_Decrement (PlankAtomicIRef p)
{
    return pl_AtomicI_Add (p, -1);
}

//------------------------------------------------------------------------------
static PlankResult pl_AtomicL_Init (PlankAtomicLRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_MemoryZero (p, sizeof (PlankAtomicL));
    pl_Lock_Init (&p->lock);
    
    return PlankResult_OK;
}

static PlankResult pl_AtomicL_DeInit (PlankAtomicLRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_Lock_DeInit (&p->lock);
    
    return PlankResult_OK;
}

static inline PlankL pl_AtomicL_Get (PlankAtomicLRef p)
{
    PlankL result;
    pl_Lock_Lock (&p->lock);
    result = p->value;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankL pl_AtomicL_GetUnchecked (PlankAtomicLRef p)
{
    return p->value;
}

static inline PlankL pl_AtomicL_Swap (PlankAtomicLRef p, PlankL newValue)
{
    PlankL oldValue;
    PlankB success;
    
    do {
        oldValue = *(PlankL*)p;
        success = pl_AtomicL_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return oldValue;
}

static inline void pl_AtomicL_SwapOther (PlankAtomicLRef p1, PlankAtomicLRef p2)
{
    PlankL value1, value2;
    PlankB success;
    
    do {
        value1 = *(PlankL*)p1;
        value2 = *(PlankL*)p2;
        success = pl_AtomicL_CompareAndSwap (p1, value1, value2);
    } while (!success);
    
    *(PlankL*)p2 = value1;
}

static inline void pl_AtomicL_Set (PlankAtomicLRef p, PlankL newValue)
{
    pl_AtomicL_Swap (p, newValue);
}

static inline PlankL pl_AtomicL_Add (PlankAtomicLRef p, PlankL operand)
{
    PlankL oldValue, newValue;
    PlankB success;
    
    do {
        oldValue = *(PlankL*)p;
        newValue = oldValue + operand;
        success = pl_AtomicL_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return newValue;
}

static inline PlankB pl_AtomicL_CompareAndSwap (PlankAtomicLRef p, PlankL oldValue, PlankL newValue)
{    
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}
        
static inline PlankL pl_AtomicL_Subtract (PlankAtomicLRef p, PlankL operand)
{
    return pl_AtomicL_Add (p, -operand);
}

static inline PlankL pl_AtomicL_Increment (PlankAtomicLRef p)
{
    return pl_AtomicL_Add (p, (PlankL)1);
}

static inline PlankL pl_AtomicL_Decrement (PlankAtomicLRef p)
{
    return pl_AtomicL_Add (p, (PlankL)(-1));
}

//------------------------------------------------------------------------------

static inline PlankResult pl_AtomicLL_Init (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }

    pl_MemoryZero (p, sizeof (PlankAtomicLL));
    pl_Lock_Init (&p->lock);
    
exit:
    return result;
}

static inline PlankResult pl_AtomicLL_DeInit (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_Lock_DeInit (&p->lock);
    
exit:
    return result;
}

static inline PlankLL pl_AtomicLL_Get (PlankAtomicLLRef p)
{
    PlankLL result;
    pl_Lock_Lock (&p->lock);
    result = p->value;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankLL pl_AtomicLL_GetUnchecked (PlankAtomicLLRef p)
{
    return p->value;
}

static inline PlankLL pl_AtomicLL_Swap (PlankAtomicLLRef p, PlankLL newValue)
{
    PlankLL oldValue;
    PlankB success;
    
    do {
        oldValue = *(PlankLL*)p;
        success = pl_AtomicLL_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return oldValue;
}

static inline void pl_AtomicLL_SwapOther (PlankAtomicLLRef p1, PlankAtomicLLRef p2)
{
    PlankLL value1, value2;
    PlankB success;
    
    do {
        value1 = *(PlankLL*)p1;
        value2 = *(PlankLL*)p2;
        success = pl_AtomicLL_CompareAndSwap (p1, value1, value2);
    } while (!success);
    
    *(PlankLL*)p2 = value1;
}

static inline void pl_AtomicLL_Set (PlankAtomicLLRef p, PlankLL newValue)
{
    pl_AtomicLL_Swap (p, newValue);
}

static inline PlankLL pl_AtomicLL_Add (PlankAtomicLLRef p, PlankLL operand)
{
    PlankLL oldValue, newValue;
    PlankB success;
    
    do {
        oldValue = *(PlankLL*)p;
        newValue = oldValue + operand;
        success = pl_AtomicLL_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return newValue;
}

static inline PlankB pl_AtomicLL_CompareAndSwap (PlankAtomicLLRef p, PlankLL oldValue, PlankLL newValue)
{    
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}

static inline PlankLL pl_AtomicLL_Subtract (PlankAtomicLLRef p, PlankLL operand)
{
    return pl_AtomicLL_Add (p, -operand);
}

static inline PlankLL pl_AtomicLL_Increment (PlankAtomicLLRef p)
{
    return pl_AtomicLL_Add (p, (PlankLL)1);
}

static inline PlankLL pl_AtomicLL_Decrement (PlankAtomicLLRef p)
{
    return pl_AtomicLL_Add (p, (PlankLL)(-1));
}

//------------------------------------------------------------------------------

static PlankResult pl_AtomicF_Init (PlankAtomicFRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_MemoryZero (p, sizeof (PlankAtomicF));
    pl_Lock_Init (&p->lock);
    
    return PlankResult_OK;
}

static PlankResult pl_AtomicF_DeInit (PlankAtomicFRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_Lock_DeInit (&p->lock);
    
    return PlankResult_OK;
}

static inline PlankF pl_AtomicF_Get (PlankAtomicFRef p)
{
    PlankF result;
    pl_Lock_Lock (&p->lock);
    result = p->value;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankF pl_AtomicF_GetUnchecked (PlankAtomicFRef p)
{
    return  p->value;
}

static inline PlankF pl_AtomicF_Swap (PlankAtomicFRef p, PlankF newValue)
{
    PlankF oldValue;
    PlankB success;
    
    do
    {
        oldValue = *(PlankF*)p;
        success = pl_AtomicF_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return oldValue;
}

static inline void pl_AtomicF_SwapOther (PlankAtomicFRef p1, PlankAtomicFRef p2)
{
    PlankF value1, value2;
    PlankB success;
    
    do {
        value1 = *(PlankF*)p1;
        value2 = *(PlankF*)p2;
        success = pl_AtomicF_CompareAndSwap (p1, value1, value2);
    } while (!success);
    
    *(PlankF*)p2 = value1;
}

static inline void pl_AtomicF_Set (PlankAtomicFRef p, PlankF newValue)
{
    pl_AtomicF_Swap (p, newValue);
}

static inline PlankF pl_AtomicF_Add (PlankAtomicFRef p, PlankF operand)
{
    PlankF newValue, oldValue;
    PlankB success;
    
    do {
        oldValue = *(PlankF*)p;
        newValue = oldValue + operand;
        success = pl_AtomicF_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return newValue;
}

static inline PlankB pl_AtomicF_CompareAndSwap (PlankAtomicFRef p, PlankF oldValue, PlankF newValue)
{    
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}

static inline PlankF pl_AtomicF_Subtract (PlankAtomicFRef p, PlankF operand)
{
    return pl_AtomicF_Add (p, -operand);
}

static inline PlankF pl_AtomicF_Increment (PlankAtomicFRef p)
{
    return pl_AtomicF_Add (p, 1.f);
}

static inline PlankF pl_AtomicF_Decrement (PlankAtomicFRef p)
{
    return pl_AtomicF_Add (p, -1.f);
}

//------------------------------------------------------------------------------

static inline PlankResult pl_AtomicD_Init (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankAtomicD));
    pl_Lock_Init (&p->lock);
    
exit:
    return result;
}

static inline PlankResult pl_AtomicD_DeInit (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_Lock_DeInit (&p->lock);
    
exit:
    return result;
}

static inline PlankD pl_AtomicD_Get (PlankAtomicDRef p)
{
    PlankD result;
    pl_Lock_Lock (&p->lock);
    result = p->value;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankD pl_AtomicD_GetUnchecked (PlankAtomicDRef p)
{
    return p->value;
}

static inline PlankD pl_AtomicD_Swap (PlankAtomicDRef p, PlankD newValue)
{
    PlankD oldValue;
    PlankB success;
    
    do {
        oldValue = *(PlankD*)p;
        success = pl_AtomicD_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return oldValue;
}

static inline void pl_AtomicD_SwapOther (PlankAtomicDRef p1, PlankAtomicDRef p2)
{
    PlankD value1, value2;
    PlankB success;
    
    do {
        value1 = *(PlankD*)p1;
        value2 = *(PlankD*)p2;
        success = pl_AtomicD_CompareAndSwap (p1, value1, value2);
    } while (!success);
    
    *(PlankD*)p2 = value1;
}

static inline void pl_AtomicD_Set (PlankAtomicDRef p, PlankD newValue)
{
    pl_AtomicD_Swap (p, newValue);
}

static inline PlankD pl_AtomicD_Add (PlankAtomicDRef p, PlankD operand)
{
    PlankD newValue, oldValue;
    PlankB success;
    
    do {
        oldValue = *(PlankD*)p;
        newValue = oldValue + operand;
        success = pl_AtomicD_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return newValue;
}

static inline PlankB pl_AtomicD_CompareAndSwap (PlankAtomicDRef p, PlankD oldValue, PlankD newValue)
{    
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}

static inline PlankD pl_AtomicD_Subtract (PlankAtomicDRef p, PlankD operand)
{
    return pl_AtomicD_Add (p, -operand);
}

static inline PlankD pl_AtomicD_Increment (PlankAtomicDRef p)
{
    return pl_AtomicD_Add (p, 1.0);
}

static inline PlankD pl_AtomicD_Decrement (PlankAtomicDRef p)
{
    return pl_AtomicD_Add (p, -1.0);
}

//------------------------------------------------------------------------------

static PlankResult pl_AtomicP_Init (PlankAtomicPRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_MemoryZero (p, sizeof (PlankAtomicP));
    pl_Lock_Init (&p->lock);
    
    return PlankResult_OK;
}

static PlankResult pl_AtomicP_DeInit (PlankAtomicPRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_Lock_DeInit (&p->lock);
    
    return PlankResult_OK;
}

static inline PlankP pl_AtomicP_Get (PlankAtomicPRef p)
{
    PlankP result;
    pl_Lock_Lock (&p->lock);
    result = p->ptr;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankP pl_AtomicP_GetUnchecked (PlankAtomicPRef p)
{
    return p->ptr;
}

static inline PlankP pl_AtomicP_Swap (PlankAtomicPRef p, PlankP newPtr)
{
    PlankP oldPtr;
    PlankB success;
    
    do {
        oldPtr = *(PlankP*)p;
        success = pl_AtomicP_CompareAndSwap (p, oldPtr, newPtr);
    } while (!success);
    
    return oldPtr;    
}

static inline void pl_AtomicP_SwapOther (PlankAtomicPRef p1, PlankAtomicPRef p2)
{
    PlankP value1, value2;
    PlankB success;
    
    do {
        value1 = *(PlankP*)p1;
        value2 = *(PlankP*)p2;
        success = pl_AtomicP_CompareAndSwap (p1, value1, value2);
    } while (!success);
    
    *(PlankP*)p2 = value1;
}

static inline void pl_AtomicP_Set (PlankAtomicPRef p, PlankP newPtr)
{
    pl_AtomicP_Swap (p, newPtr);
}

static inline PlankP pl_AtomicP_Add (PlankAtomicPRef p, PlankL operand)
{
    PlankP newPtr, oldPtr;
    PlankB success;
    
    do {
        oldPtr = *(PlankP*)p;
        newPtr = (PlankUC*)oldPtr + operand;
        success = pl_AtomicP_CompareAndSwap (p, oldPtr, newPtr);
    } while (!success);
    
    return newPtr;
}

static inline PlankB pl_AtomicP_CompareAndSwap (PlankAtomicPRef p, PlankP oldPtr, PlankP newPtr)
{
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->ptr != oldPtr)
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->ptr = newPtr;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}

static inline PlankP pl_AtomicP_Subtract (PlankAtomicPRef p, PlankL operand)
{
    return pl_AtomicP_Add (p, -operand);
}

static inline PlankP pl_AtomicP_Increment (PlankAtomicPRef p)
{
    return pl_AtomicP_Add (p, (PlankL)1);
}

static inline PlankP pl_AtomicP_Decrement (PlankAtomicPRef p)
{
    return pl_AtomicP_Add (p, (PlankL)(-1));
}

//------------------------------------------------------------------------------

static inline PlankAtomicPXRef pl_AtomicPX_CreateAndInit()
{
    PlankAtomicPXRef p = pl_AtomicPX_Create();
    if (p != PLANK_NULL) pl_AtomicPX_Init (p);
    return p;
}

static inline PlankAtomicPXRef pl_AtomicPX_Create()
{
    PlankMemoryRef m;
    PlankAtomicPXRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicPXRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicPX));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicPX));
    
    return p;
}

static inline PlankResult pl_AtomicPX_Init (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankAtomicPX));
    pl_Lock_Init (&p->lock);
    
exit:
    return result;
}

static inline PlankResult pl_AtomicPX_DeInit (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_Lock_DeInit (&p->lock);
    
exit:
    return result;
}

static inline PlankResult pl_AtomicPX_Destroy (PlankAtomicPXRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if ((result = pl_AtomicPX_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

static inline PlankP pl_AtomicPX_Get (PlankAtomicPXRef p)
{
    PlankP result;
    pl_Lock_Lock (&p->lock);
    result = p->ptr;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankP pl_AtomicPX_GetUnchecked (PlankAtomicPXRef p)
{
    return p->ptr;
}

static inline PlankUL pl_AtomicPX_GetExtra (PlankAtomicPXRef p)
{
    PlankUL result;
    pl_Lock_Lock (&p->lock);
    result = p->extra;
    pl_Lock_Unlock (&p->lock);
    return result;
}

static inline PlankUL pl_AtomicPX_GetExtraUnchecked (PlankAtomicPXRef p)
{
    return p->extra;
}

static inline PlankP pl_AtomicPX_SwapAll (PlankAtomicPXRef p, PlankP newPtr, PlankUL newExtra, PlankUL* oldExtraPtr)
{
    PlankP oldPtr;
    PlankUL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, newExtra);
    } while (!success);
    
    if (oldExtraPtr != PLANK_NULL)
        *oldExtraPtr = oldExtra;
    
    return oldPtr;
}

static inline PlankP pl_AtomicPX_Swap (PlankAtomicPXRef p, PlankP newPtr)
{
    PlankP oldPtr;
    PlankUL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, oldExtra + 1);
    } while (!success);
    
    return oldPtr;
}

static inline void pl_AtomicPX_SwapOther (PlankAtomicPXRef p1, PlankAtomicPXRef p2)
{
    PlankAtomicPX tmp1, tmp2;
    PlankB success;
    
    do {
        tmp1 = *p1;
        tmp2 = *p2;
        success = pl_AtomicPX_CompareAndSwap (p1, tmp1.ptr, tmp1.extra, tmp2.ptr, tmp1.extra + 1);
    } while (!success);
    
    pl_AtomicPX_Set (p2, tmp1.ptr);
}

static inline void pl_AtomicPX_SetAll (PlankAtomicPXRef p, PlankP newPtr, PlankUL newExtra)
{
    pl_AtomicPX_SwapAll (p, newPtr, newExtra, (PlankUL*)PLANK_NULL);
}

static inline void pl_AtomicPX_Set (PlankAtomicPXRef p, PlankP newPtr)
{
    PlankP oldPtr;
    PlankUL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, oldExtra + 1);
    } while (!success);
}

static inline PlankP pl_AtomicPX_Add (PlankAtomicPXRef p, PlankL operand)
{
    PlankP newPtr, oldPtr;
    PlankUL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        newPtr = (PlankUC*)oldPtr + operand;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, oldExtra + 1);
    } while (!success);
    
    return newPtr;
}

static inline PlankP pl_AtomicPX_Subtract (PlankAtomicPXRef p, PlankL operand)
{
    return pl_AtomicPX_Add (p, -operand);
}

static inline PlankP pl_AtomicPX_Increment (PlankAtomicPXRef p)
{
    return pl_AtomicPX_Add (p, (PlankL)1);
}

static inline PlankP pl_AtomicPX_Decrement (PlankAtomicPXRef p)
{
    return pl_AtomicPX_Add (p, (PlankL)(-1));
}

static inline  PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankUL oldExtra, PlankP newPtr, PlankUL newExtra)
{
    if (! pl_Lock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if ((p->ptr != oldPtr) || (p->extra != oldExtra))
    {
        pl_Lock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->ptr = newPtr;
    p->extra = newExtra;
    pl_Lock_Unlock (&p->lock);
    
    return PLANK_TRUE;
}

static inline PlankB pl_AtomicPX_CompareAndSwapP (PlankAtomicPXRef p, PlankP oldPtr, PlankP newPtr)
{
    return pl_AtomicP_CompareAndSwap ((PlankAtomicPRef)p, oldPtr, newPtr);
}

static void pl_AtomicPX_SetAllUnchecked (PlankAtomicPXRef p, PlankP newPtr, PlankUL newExtra)
{
    p->ptr = newPtr;
    p->extra = newExtra;
}

#define PLANK_ATOMICS_DEFINED 1

#endif // PLANK_INLINING_FUNCTIONS

