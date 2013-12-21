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

// help prevent accidental inclusion other than via the intended header
#if PLANK_INLINING_FUNCTIONS


static inline PlankP pl_AtomicPX_Get (PlankAtomicPXRef p)
{
    return p->ptr; // should be aligned anyway and volatile so OK // pl_AtomicP_Get ((PlankAtomicPRef)p);
}

static inline PlankP pl_AtomicPX_GetUnchecked (PlankAtomicPXRef p)
{
    return p->ptr;
}

static inline PlankL pl_AtomicPX_GetExtra (PlankAtomicPXRef p)
{
    return p->extra; // should be aligned anyway and volatile so OK // pl_AtomicL_Get ((PlankAtomicLRef)&(p->extra));
}

static inline PlankL pl_AtomicPX_GetExtraUnchecked (PlankAtomicPXRef p)
{
    return p->extra;
}

static inline PlankP pl_AtomicPX_SwapAll (PlankAtomicPXRef p, PlankP newPtr, PlankL newExtra, PlankL* oldExtraPtr)
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

static inline PlankP pl_AtomicPX_Swap (PlankAtomicPXRef p, PlankP newPtr)
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

static inline void pl_AtomicPX_SetAll (PlankAtomicPXRef p, PlankP newPtr, PlankL newExtra)
{
    pl_AtomicPX_SwapAll (p, newPtr, newExtra, (PlankL*)PLANK_NULL);
}

static inline void pl_AtomicPX_Set (PlankAtomicPXRef p, PlankP newPtr)
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

static inline PlankP pl_AtomicPX_Add (PlankAtomicPXRef p, PlankL operand)
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


#if PLANK_APPLE
#if PLANK_X86
#if PLANK_32BIT
static inline  PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
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
static inline  PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
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
static inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    PlankAtomicPX oldAll = { oldPtr, oldExtra, p->lock };
    PlankAtomicPX newAll = { newPtr, newExtra, p->lock };
    
    return pl_AtomicLL_CompareAndSwap ((PlankAtomicLLRef)p, 
                                       *(PlankLL*)&oldAll, 
                                       *(PlankLL*)&newAll);  
}
#endif // PLANK_PPC and PLANK_APPLE

#if PLANK_ARM // and PLANK_APPLE

#if PLANK_64BIT // ARM
#error pl_AtomicPX_CompareAndSwap need to implement 128-bit CAS for this platform
#endif // PLANK_64BIT

#if PLANK_32BIT // ARM
static inline  PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    PlankAtomicPX oldAll = { oldPtr, oldExtra };
    PlankAtomicPX newAll = { newPtr, newExtra };
    
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldAll,
                                            *(int64_t*)&newAll,
                                            (volatile int64_t*)p);
}
#endif // PLANK_32BIT ARM
#endif // PLANK_ARM and PLANK_APPLE

#endif // PLANK_APPLE

#if PLANK_WIN
#if PLANK_32BIT
static inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    // can't use static init as MSVC C is C89
    PlankAtomicPX oldAll;
    PlankAtomicPX newAll;
    PlankULL oldAllValue;
    PlankULL newAllValue;
    
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
static inline PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{ 
    PlankAtomicPX oldAll;
	oldAll.ptr = oldPtr;
	oldAll.extra = oldExtra;
    return _InterlockedCompareExchange128 ((volatile __int64*)p,
                                           *(__int64*)&newExtra,
                                           *(__int64*)&newPtr,
                                           (__int64*)&oldAll);    
}
#endif
#endif // PLANK_WIN

#if PLANK_ANDROID
#if PLANK_32BIT
static inline  PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    PlankAtomicPX oldAll = { oldPtr, oldExtra };
    PlankAtomicPX newAll = { newPtr, newExtra };
    
    return __sync_bool_compare_and_swap ((volatile PlankLL*)p,
                                         *(PlankLL*)&oldAll,
                                         *(PlankLL*)&newAll);
}
#endif // PLANK_32BIT

#if PLANK_64BIT

#if PLANK_ARM
#error pl_AtomicPX_CompareAndSwap need to implement 128-bit CAS for this platform
#endif

#if PLANK_X86
static inline  PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra)
{
    char success;
    __asm__ __volatile__("lock; cmpxchg16b %0; setz %1"
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldPtr),
                         "c" (newExtra), "b" (newPtr) : "memory");
    return success;
}
#endif

#endif // PLANK_64BIT
#endif // PLANK_ANDROID

//--

static inline PlankL pl_AtomicLX_Get (PlankAtomicLXRef p)
{
    return p->value; // should be aligned anyway and volatile so OK // pl_AtomicL_Get ((PlankAtomicLRef)p);
}

static inline PlankL pl_AtomicLX_GetUnchecked (PlankAtomicLXRef p)
{
    return p->value;
}

static inline PlankL pl_AtomicLX_GetExtra (PlankAtomicLXRef p)
{
    return p->extra; // should be aligned anyway and volatile so OK // pl_AtomicL_Get ((PlankAtomicLRef)&(p->extra));
}

static inline PlankL pl_AtomicLX_GetExtraUnchecked (PlankAtomicLXRef p)
{
    return p->extra;
}

static inline PlankL pl_AtomicLX_SwapAll (PlankAtomicLXRef p, PlankL newValue, PlankL newExtra, PlankL* oldExtraPtr)
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

static inline PlankL pl_AtomicLX_Swap (PlankAtomicLXRef p, PlankL newValue)
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

static inline void pl_AtomicLX_SwapOther (PlankAtomicLXRef p1, PlankAtomicLXRef p2)
{
    PlankAtomicLX tmp1, tmp2;
    PlankB success;
    
    do {
        tmp1 = *p1;
        tmp2 = *p2;
        success = pl_AtomicLX_CompareAndSwap (p1, tmp1.value, tmp1.extra, tmp2.value, tmp1.extra + 1);
    } while (!success);
    
    pl_AtomicLX_Set (p2, tmp1.value);
}

static inline void pl_AtomicLX_SetAll (PlankAtomicLXRef p, PlankL newValue, PlankL newExtra)
{
    pl_AtomicLX_SwapAll (p, newValue, newExtra, (PlankL*)PLANK_NULL);
}

static inline void pl_AtomicLX_Set (PlankAtomicLXRef p, PlankL newValue)
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

static inline PlankL pl_AtomicLX_Add (PlankAtomicLXRef p, PlankL operand)
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

static inline PlankL pl_AtomicLX_Subtract (PlankAtomicLXRef p, PlankL operand)
{
    return pl_AtomicLX_Add (p, -operand);
}

static inline PlankL pl_AtomicLX_Increment (PlankAtomicLXRef p)
{
    return pl_AtomicLX_Add (p, (PlankL)1);
}

static inline PlankL pl_AtomicLX_Decrement (PlankAtomicLXRef p)
{
    return pl_AtomicLX_Add (p, (PlankL)(-1));
}


#if PLANK_APPLE
#if PLANK_X86
#if PLANK_32BIT
static inline  PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
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
static inline  PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
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
static inline  PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankP newValue, PlankL newExtra)
{
    PlankAtomicLX oldAll = { oldValue, oldExtra, p->lock };
    PlankAtomicLX newAll = { newValue, newExtra, p->lock };
    
    return pl_AtomicLL_CompareAndSwap ((PlankAtomicLLRef)p,
                                       *(PlankLL*)&oldAll,
                                       *(PlankLL*)&newAll);
}
#endif // PLANK_PPC and PLANK_APPLE

#if PLANK_ARM // and PLANK_APPLE
static inline  PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
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
static inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    PlankAtomicLX oldAll;
    PlankAtomicLX newAll;
    PlankULL oldAllValue;
    PlankULL newAllValue;
    
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
static inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra)
{
    PlankAtomicLX oldAll;
	oldAll.value = oldValue;
	oldAll.extra = oldExtra;
    return _InterlockedCompareExchange128 ((volatile __int64*)p,
                                           *(__int64*)&newExtra,
                                           *(__int64*)&newValue,
                                           (__int64*)&oldAll);
}
#endif
#endif

#if PLANK_ANDROID
#if PLANK_32BIT
static inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldPtr, PlankL oldExtra, PlankL newPtr, PlankL newExtra)
{
    PlankAtomicLX oldAll = { oldPtr, oldExtra };
    PlankAtomicLX newAll = { newPtr, newExtra };
    
    return __sync_bool_compare_and_swap ((volatile PlankLL*)p,
                                         *(PlankLL*)&oldAll,
                                         *(PlankLL*)&newAll);
}
#endif // PLANK_32BIT

#if PLANK_64BIT

#if PLANK_ARM
#error pl_AtomicPX_CompareAndSwap need to implement 128-bit CAS for this platform
#endif

#if PLANK_X86
static inline PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldPtr, PlankL oldExtra, PlankL newPtr, PlankL newExtra)
{
    char success;
    __asm__ __volatile__("lock; cmpxchg16b %0; setz %1"
                         : "=m"(*p), "=a"(success)
                         : "m"(*p), "d" (oldExtra), "a" (oldPtr),
                         "c" (newExtra), "b" (newPtr) : "memory");
    return success;
}
#endif

#endif // PLANK_64BIT
#endif // PLANK_ANDROID


#endif //PLANK_INLINING_FUNCTIONS






