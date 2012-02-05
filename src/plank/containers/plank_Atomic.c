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
#include "plank_Atomic.h"

//------------------------------------------------------------------------------

PlankAtomicIRef pl_AtomicI_CreateAndInit()
{
    PlankAtomicIRef p = pl_AtomicI_Create();
    if (p != PLANK_NULL) pl_AtomicI_Init (p);
    return p;
}

PlankAtomicIRef pl_AtomicI_Create()
{
    PlankMemoryRef m;
    PlankAtomicIRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicIRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicI));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicI));
    
    return p;
}

PlankResult pl_AtomicI_Init (PlankAtomicIRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_AtomicI_Set (p, (PlankI)0);
    
    return PlankResult_OK;
}

PlankResult pl_AtomicI_DeInit (PlankAtomicIRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicI_Destroy (PlankAtomicIRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    return pl_Memory_Free (m, p);
}

PlankI pl_AtomicI_GetExtra (PlankAtomicIRef p)
{
    (void)p;
    return 0;
}

//------------------------------------------------------------------------------

PlankAtomicLRef pl_AtomicL_CreateAndInit()
{
    PlankAtomicLRef p = pl_AtomicL_Create();
    if (p != PLANK_NULL) pl_AtomicL_Init (p);
    return p;
}

PlankAtomicLRef pl_AtomicL_Create()
{
    PlankMemoryRef m;
    PlankAtomicLRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicLRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicL));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicL));

    return p;
}

PlankResult pl_AtomicL_Init (PlankAtomicLRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_AtomicL_Set (p, (PlankL)0);
    
    return PlankResult_OK;
}

PlankResult pl_AtomicL_DeInit (PlankAtomicLRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicL_Destroy (PlankAtomicLRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    return pl_Memory_Free (m, p);
}

PlankL pl_AtomicL_GetExtra (PlankAtomicLRef p)
{
    (void)p;
    return (PlankL)0;
}

//------------------------------------------------------------------------------

PlankAtomicLLRef pl_AtomicLL_CreateAndInit()
{
    PlankAtomicLLRef p = pl_AtomicLL_Create();
    if (p != PLANK_NULL) pl_AtomicLL_Init (p);
    return p;
}

PlankAtomicLLRef pl_AtomicLL_Create()
{
    PlankMemoryRef m;
    PlankAtomicLLRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicLLRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicLL));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicLL));

    return p;
}

PlankResult pl_AtomicLL_Init (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    p->lock = pl_Lock_Create();
    
    if (p->lock == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }    
#endif    
    
    pl_AtomicLL_Set (p, (PlankLL)0);
    
exit:
    return result;
}

PlankResult pl_AtomicLL_DeInit (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    if ((result = pl_Lock_Destroy (p->lock)) != PlankResult_OK)
        goto exit;
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicLL_Destroy (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();

    if ((result = pl_AtomicLL_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
exit:
    return result;
}

PlankLL pl_AtomicLL_GetExtra (PlankAtomicLLRef p)
{
    (void)p;
    return (PlankLL)0;
}


#if PLANK_APPLE && PLANK_PPC
 PlankB pl_AtomicLL_CompareAndSwap (PlankAtomicLLRef p, PlankLL oldValue, PlankLL newValue)
{    
    if (! pl_Lock_TryLock (p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_Lock_Unlock (p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_Lock_Unlock (p->lock);

    return PLANK_TRUE;
}
#endif // PLANK_PPC && PLANK_APPLE


//------------------------------------------------------------------------------

PlankAtomicFRef pl_AtomicF_CreateAndInit()
{
    PlankAtomicFRef p = pl_AtomicF_Create();
    if (p != PLANK_NULL) pl_AtomicF_Init (p);
    return p;
}

PlankAtomicFRef pl_AtomicF_Create()
{
    PlankMemoryRef m;
    PlankAtomicFRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicFRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicF));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicF));

    return p;
}

PlankResult pl_AtomicF_Init (PlankAtomicFRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_AtomicF_Set (p, 0.f);
    
    return PlankResult_OK;
}

PlankResult pl_AtomicF_DeInit (PlankAtomicFRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicF_Destroy (PlankAtomicFRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    pl_Memory_Free (m, p);
    return PlankResult_OK;
}

PlankI pl_AtomicF_GetExtra (PlankAtomicFRef p)
{
    (void)p;
    return 0;
}

//------------------------------------------------------------------------------

PlankAtomicDRef pl_AtomicD_CreateAndInit()
{
    PlankAtomicDRef p = pl_AtomicD_Create();
    if (p != PLANK_NULL) pl_AtomicD_Init (p);
    return p;
}

PlankAtomicDRef pl_AtomicD_Create()
{
    PlankMemoryRef m;
    PlankAtomicDRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicDRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicD));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicD));
    
    return p;
}

PlankResult pl_AtomicD_Init (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    p->lock = pl_Lock_Create();
    
    if (p->lock == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }    
#endif    
    
    pl_AtomicD_Set (p, 0.0);
    
exit:
    return result;
}

PlankResult pl_AtomicD_DeInit (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    if ((result = pl_Lock_Destroy (p->lock)) != PlankResult_OK)
        goto exit;
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicD_Destroy (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if ((result = pl_AtomicD_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
exit:
    return result;
}

PlankLL pl_AtomicD_GetExtra (PlankAtomicDRef p)
{
    (void)p;
    return (PlankLL)0;
}

//------------------------------------------------------------------------------

PlankAtomicPRef pl_AtomicP_CreateAndInit()
{
    PlankAtomicPRef p = pl_AtomicP_Create();
    if (p != PLANK_NULL) pl_AtomicP_Init (p);
    return p;
}

PlankAtomicPRef pl_AtomicP_Create()
{
    PlankMemoryRef m;
    PlankAtomicPRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicPRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicP));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicP));
    
    return p;
}

PlankResult pl_AtomicP_Init (PlankAtomicPRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    pl_AtomicP_Set (p, PLANK_NULL);
    
    return PlankResult_OK;
}

PlankResult pl_AtomicP_DeInit (PlankAtomicPRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicP_Destroy (PlankAtomicPRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    pl_Memory_Free (m, p);
    return PlankResult_OK;
}

PlankL pl_AtomicP_GetExtra (PlankAtomicPRef p)
{
	(void)p;
    return (PlankL)0;
}

//------------------------------------------------------------------------------

PlankAtomicPXRef pl_AtomicPX_CreateAndInit()
{
    PlankAtomicPXRef p = pl_AtomicPX_Create();
    if (p != PLANK_NULL) pl_AtomicPX_Init (p);
    return p;
}

PlankAtomicPXRef pl_AtomicPX_Create()
{
    PlankMemoryRef m;
    PlankAtomicPXRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicPXRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicPX));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicPX));
    
    return p;
}

PlankResult pl_AtomicPX_Init (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    p->lock = pl_Lock_Create();
    
    if (p->lock == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
#endif        
    
    pl_AtomicPX_SetAll (p, (PlankP)0, (PlankL)0);
    
exit:
    return result;
}

PlankResult pl_AtomicPX_DeInit (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    if ((result = pl_Lock_Destroy (p->lock)) != PlankResult_OK)
        goto exit;
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicPX_Destroy (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();

    if ((result = pl_AtomicPX_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

PlankP pl_AtomicPX_Get (PlankAtomicPXRef p)
{
    return pl_AtomicP_Get ((PlankAtomicPRef)p);
}

PlankL pl_AtomicPX_GetExtra (PlankAtomicPXRef p)
{
    return pl_AtomicL_Get ((PlankAtomicLRef)&(p->extra));
}

PlankP pl_AtomicPX_SwapAll (PlankAtomicPXRef p, PlankP newPtr, PlankL newExtra, PlankL* oldExtraPtr)
{
    PlankP oldPtr;
    PlankL oldExtra;
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

PlankP pl_AtomicPX_Swap (PlankAtomicPXRef p, PlankP newPtr)
{
    PlankP oldPtr;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, oldExtra + 1);
    } while (!success);
    
    return oldPtr;            
}

void pl_AtomicPX_SetAll (PlankAtomicPXRef p, PlankP newPtr, PlankL newExtra)
{
    pl_AtomicPX_SwapAll (p, newPtr, newExtra, (PlankL*)PLANK_NULL);
}

void pl_AtomicPX_Set (PlankAtomicPXRef p, PlankP newPtr)
{
    PlankP oldPtr;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, oldExtra + 1);
    } while (!success);
}

PlankP pl_AtomicPX_Add (PlankAtomicPXRef p, PlankL operand)
{
    PlankP newPtr, oldPtr;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldPtr = p->ptr;
        oldExtra = p->extra;
        newPtr = (PlankUC*)oldPtr + operand;
        success = pl_AtomicPX_CompareAndSwap (p, oldPtr, oldExtra, newPtr, oldExtra + 1);
    } while (!success);
    
    return newPtr;    
}

PlankP pl_AtomicPX_Subtract (PlankAtomicPXRef p, PlankL operand)
{
    return pl_AtomicPX_Add (p, -operand);
}

PlankP pl_AtomicPX_Increment (PlankAtomicPXRef p)
{
    return pl_AtomicPX_Add (p, (PlankL)1);
}

PlankP pl_AtomicPX_Decrement (PlankAtomicPXRef p)
{
    return pl_AtomicPX_Add (p, (PlankL)(-1));
}


#if PLANK_APPLE
#if PLANK_X86
#if PLANK_32BIT
inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    char success;
#if __PIC__
    /* If PIC is turned on, we can't use %ebx as it is reserved for the
     GOT pointer.  We can save and restore %ebx because GCC won't be
     using it for anything else (such as any of the m operands) */
    __asm__ __volatile__("pushl %%ebx;"   /* save ebx used for PIC GOT ptr */
                         "movl %6,%%ebx;" /* move new_val2 to %ebx */
                         "lock; cmpxchg8b %0; setz %1;"
                         "pop %%ebx;"     /* restore %ebx */
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldPtr),
                           "c" (newExtra), "m" (newPtr) : "memory");
#else // !__PIC__
    /* We can't just do the same thing in non-PIC mode, because GCC
     * might be using %ebx as the memory operand.  We could have ifdef'd
     * in a clobber, but there's no point doing the push/pop if we don't
     * have to. */
    __asm__ __volatile__("lock; cmpxchg8b %0; setz %1;"
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldPtr),
                           "c" (newExtra), "b" (newPtr) : "memory");
#endif // !__PIC__
    return success;
}
#endif // PLANK_32BIT

#if PLANK_64BIT
inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    char success;
    __asm__ __volatile__("lock; cmpxchg16b %0; setz %1"
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldPtr),
                         "c" (newExtra), "b" (newPtr) : "memory");
    return success;    
}
#endif //PLANK_64BIT
#endif //PLANK_X86

#if PLANK_PPC // and PLANK_APPLE
inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    PlankAtomicPX oldAll = { oldPtr, oldExtra, p->lock };
    PlankAtomicPX newAll = { newPtr, newExtra, p->lock };
    
    return pl_AtomicLL_CompareAndSwap ((PlankAtomicLLRef)p, 
                                       *(PlankLL*)&oldAll, 
                                       *(PlankLL*)&newAll);  
}
#endif // PLANK_PPC and PLANK_APPLE

#if PLANK_ARM // and PLANK_APPLE
inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    PlankAtomicPX oldAll = { oldPtr, oldExtra };
    PlankAtomicPX newAll = { newPtr, newExtra };
        
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldAll, 
                                            *(int64_t*)&newAll, 
                                            (volatile int64_t*)p);    
}
#endif // PLANK_ARM and PLANK_APPLE

#endif // PLANK_APPLE

#if PLANK_WIN
#if PLANK_32BIT
PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    PlankAtomicPX oldAll; // = { oldPtr, oldExtra };
    PlankAtomicPX newAll; // = { newPtr, newExtra };
    PlankULL oldAllValue; // = *(PlankULL*)&oldAll;
    PlankULL newAllValue; // = *(PlankULL*)&newAll;
    
	oldAll.ptr   = oldPtr; 
	oldAll.extra = oldExtra;
	newAll.ptr   = newPtr; 
	newAll.extra = newExtra;
    oldAllValue  = *(PlankULL*)&oldAll;
	newAllValue  = *(PlankULL*)&newAll;

	return oldAllValue == pl_InterlockedCompareExchange64 ((volatile PlankULL*)p, newAllValue, oldAllValue);
}
#endif

#if PLANK_64BIT
PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    // Can't check this as I only have the MSVC 32-bit version, 
    // I might have newPtr and newExtra the wrong way round.
    
    PlankAtomicPX oldAll; // = { oldPtr, oldExtra };
	oldAll.ptr = oldPtr;
	oldAll.extra = oldExtra;
    return _InterlockedCompareExchange128 ((volatile __int64*)p,
                                           *(__int64*)&newPtr,
                                           *(__int64*)&newExtra,
                                           (__int64*)&oldAll);    
}
#endif
#endif

//------------------------------------------------------------------------------

PlankAtomicLXRef pl_AtomicLX_CreateAndInit()
{
    PlankAtomicLXRef p = pl_AtomicLX_Create();
    if (p != PLANK_NULL) pl_AtomicLX_Init (p);
    return p;
}

PlankAtomicLXRef pl_AtomicLX_Create()
{
    PlankMemoryRef m;
    PlankAtomicLXRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicLXRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicLX));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicLX));
    
    return p;
}

PlankResult pl_AtomicLX_Init (PlankAtomicLXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    p->lock = pl_Lock_Create();
    
    if (p->lock == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
#endif        
    
    pl_AtomicLX_SetAll (p, (PlankL)0, (PlankL)0);
    
exit:
    return result;
}

PlankResult pl_AtomicLX_DeInit (PlankAtomicLXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    if ((result = pl_Lock_Destroy (p->lock)) != PlankResult_OK)
        goto exit;
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicLX_Destroy (PlankAtomicLXRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if ((result = pl_AtomicLX_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

PlankL pl_AtomicLX_Get (PlankAtomicLXRef p)
{
    return pl_AtomicL_Get ((PlankAtomicLRef)p);
}

PlankL pl_AtomicLX_GetExtra (PlankAtomicLXRef p)
{
    return pl_AtomicL_Get ((PlankAtomicLRef)&(p->extra));
}

PlankL pl_AtomicLX_SwapAll (PlankAtomicLXRef p, PlankL newValue, PlankL newExtra, PlankL* oldExtraPtr)
{
    PlankL oldValue;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldValue = p->value;
        oldExtra = p->extra;
        success = pl_AtomicLX_CompareAndSwap (p, oldValue, oldExtra, newValue, newExtra);
    } while (!success);
    
    if (oldExtraPtr != PLANK_NULL)
        *oldExtraPtr = oldExtra;
    
    return oldValue;        
}

PlankL pl_AtomicLX_Swap (PlankAtomicLXRef p, PlankL newValue)
{
    PlankL oldValue;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldValue = p->value;
        oldExtra = p->extra;
        success = pl_AtomicLX_CompareAndSwap (p, oldValue, oldExtra, newValue, oldExtra + 1);
    } while (!success);
    
    return oldValue;            
}

void pl_AtomicLX_SetAll (PlankAtomicLXRef p, PlankL newValue, PlankL newExtra)
{
    pl_AtomicLX_SwapAll (p, newValue, newExtra, (PlankL*)PLANK_NULL);
}

void pl_AtomicLX_Set (PlankAtomicLXRef p, PlankL newValue)
{
    PlankL oldValue;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldValue = p->value;
        oldExtra = p->extra;
        success = pl_AtomicLX_CompareAndSwap (p, oldValue, oldExtra, newValue, oldExtra + 1);
    } while (!success);
}

PlankL pl_AtomicLX_Add (PlankAtomicLXRef p, PlankL operand)
{
    PlankL newValue, oldValue;
    PlankL oldExtra;
    PlankB success;
    
    do {
        oldValue = p->value;
        oldExtra = p->extra;
        newValue = oldValue + operand;
        success = pl_AtomicLX_CompareAndSwap (p, oldValue, oldExtra, newValue, oldExtra + 1);
    } while (!success);
    
    return newValue;    
}

PlankL pl_AtomicLX_Subtract (PlankAtomicLXRef p, PlankL operand)
{
    return pl_AtomicLX_Add (p, -operand);
}

PlankL pl_AtomicLX_Increment (PlankAtomicLXRef p)
{
    return pl_AtomicLX_Add (p, (PlankL)1);
}

PlankL pl_AtomicLX_Decrement (PlankAtomicLXRef p)
{
    return pl_AtomicLX_Add (p, (PlankL)(-1));
}


#if PLANK_APPLE
#if PLANK_X86
#if PLANK_32BIT
inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    char success;
#if __PIC__
    /* If PIC is turned on, we can't use %ebx as it is reserved for the
     GOT pointer.  We can save and restore %ebx because GCC won't be
     using it for anything else (such as any of the m operands) */
    __asm__ __volatile__("pushl %%ebx;"   /* save ebx used for PIC GOT ptr */
                         "movl %6,%%ebx;" /* move new_val2 to %ebx */
                         "lock; cmpxchg8b %0; setz %1;"
                         "pop %%ebx;"     /* restore %ebx */
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldValue),
                         "c" (newExtra), "m" (newValue) : "memory");
#else // !__PIC__
    /* We can't just do the same thing in non-PIC mode, because GCC
     * might be using %ebx as the memory operand.  We could have ifdef'd
     * in a clobber, but there's no point doing the push/pop if we don't
     * have to. */
    __asm__ __volatile__("lock; cmpxchg8b %0; setz %1;"
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldValue),
                         "c" (newExtra), "b" (newValue) : "memory");
#endif // !__PIC__
    return success;
}
#endif // PLANK_32BIT

#if PLANK_64BIT
inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    char success;
    __asm__ __volatile__("lock; cmpxchg16b %0; setz %1"
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldValue),
                         "c" (newExtra), "b" (newValue) : "memory");
    return success;    
}
#endif //PLANK_64BIT
#endif //PLANK_X86

#if PLANK_PPC // and PLANK_APPLE
inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankP newValue, PlankL newExtra)
{
    PlankAtomicLX oldAll = { oldValue, oldExtra, p->lock };
    PlankAtomicLX newAll = { newValue, newExtra, p->lock };
    
    return pl_AtomicLL_CompareAndSwap ((PlankAtomicLLRef)p, 
                                       *(PlankLL*)&oldAll, 
                                       *(PlankLL*)&newAll);  
}
#endif // PLANK_PPC and PLANK_APPLE

#if PLANK_ARM // and PLANK_APPLE
inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    PlankAtomicLX oldAll = { oldValue, oldExtra };
    PlankAtomicLX newAll = { newValue, newExtra };
    
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldAll, 
                                            *(int64_t*)&newAll, 
                                            (volatile int64_t*)p);    
}
#endif // PLANK_ARM and PLANK_APPLE

#endif // PLANK_APPLE

#if PLANK_WIN
#if PLANK_32BIT
PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    PlankAtomicLX oldAll; // = { oldValue, oldExtra };
    PlankAtomicLX newAll; // = { newValue, newExtra };
    PlankULL oldAllValue; // = *(PlankULL*)&oldAll;
    PlankULL newAllValue; // = *(PlankULL*)&newAll;

	oldAll.value = oldValue; 
	oldAll.extra = oldExtra;
    newAll.value = newValue; 
	newAll.extra = newExtra;
	oldAllValue  = *(PlankULL*)&oldAll;
    newAllValue  = *(PlankULL*)&newAll;

	return oldAllValue == pl_InterlockedCompareExchange64 ((volatile PlankULL*)p, newAllValue, oldAllValue);
}
#endif

#if PLANK_64BIT
PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    // Can't check this as I only have the MSVC 32-bit version, 
    // I might have newPtr and newExtra the wrong way round.
    
    PlankAtomicLX oldAll; // = { oldValue, oldExtra };
	oldAll.value = oldValue;
	oldAll.extra = oldExtra;
    return _InterlockedCompareExchange128 ((volatile __int64*)p,
                                           *(__int64*)&newValue,
                                           *(__int64*)&newExtra,
                                           (__int64*)&oldAll);    
}
#endif
#endif