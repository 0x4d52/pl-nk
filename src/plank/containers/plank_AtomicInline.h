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

// help prevent accidental inclusion other than via the intended header
#if PLANK_INLINING_FUNCTIONS

static inline void pl_AtomicMemoryBarrier()
{
#if PLANK_APPLE
    OSMemoryBarrier();
#elif PLANK_WIN
	_ReadWriteBarrier();
#endif
}

#if PLANK_WIN

#if PLANK_32BIT
#pragma warning(disable:4035)
static inline PlankULL pl_InterlockedCompareExchange64 (volatile PlankULL *value, 
                                                        PlankULL newValue, 
                                                        PlankULL oldValue) 
{
    //value returned in eax::edx
    __asm {
        lea esi,oldValue;
        lea edi,newValue;
        
        mov eax,[esi];
        mov edx,4[esi];
        mov ebx,[edi];
        mov ecx,4[edi];
        mov esi,value;
        lock CMPXCHG8B [esi];			
    }
}
#pragma warning(default:4035)
#endif

#if PLANK_64BIT
static inline PlankULL pl_InterlockedCompareExchange64 (volatile PlankULL *value, 
                                                        PlankULL newValue, 
                                                        PlankULL oldValue) 
{
	return (PlankULL)_InterlockedCompareExchange64 ((volatile __int64*)value, 
												    *(__int64*)&newValue, 
												    *(__int64*)&oldValue);
}
#endif

#endif

//------------------------------------------------------------------------------

static inline PlankI pl_AtomicI_Get (PlankAtomicIRef p)
{
    return pl_AtomicI_Add (p, 0);
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

static inline void pl_AtomicI_Set (PlankAtomicIRef p, PlankI newValue)
{
    pl_AtomicI_Swap (p, newValue);
}

#if PLANK_APPLE
static inline PlankI pl_AtomicI_Add (PlankAtomicIRef p, PlankI operand)
{
    return OSAtomicAdd32Barrier (*(int32_t*)&operand, 
                                 (volatile int32_t*)p);
}

static inline PlankB  pl_AtomicI_CompareAndSwap (PlankAtomicIRef p, PlankI oldValue, PlankI newValue)
{    
    return OSAtomicCompareAndSwap32Barrier (*(int32_t*)&oldValue, 
                                            *(int32_t*)&newValue, 
                                            (volatile int32_t*)p);
}
#endif

#if PLANK_WIN
static inline PlankI pl_AtomicI_Add (PlankAtomicIRef p, PlankI operand)
{
    return operand + _InterlockedExchangeAdd ((volatile long*)p, operand);
}

static inline PlankB  pl_AtomicI_CompareAndSwap (PlankAtomicIRef p, PlankI oldValue, PlankI newValue)
{    
    return oldValue == _InterlockedCompareExchange ((volatile long*)p, 
                                                    *(long*)&newValue, 
                                                    *(long*)&oldValue);
}
#endif

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

static inline PlankL pl_AtomicL_Get (PlankAtomicLRef p)
{
    return pl_AtomicL_Add (p, (PlankL)0);
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

static inline void pl_AtomicL_Set (PlankAtomicLRef p, PlankL newValue)
{
    pl_AtomicL_Swap (p, newValue);
}

#if PLANK_APPLE
#if PLANK_32BIT
static inline PlankL pl_AtomicL_Add (PlankAtomicLRef p, PlankL operand)
{
    return OSAtomicAdd32Barrier (*(int32_t*)&operand, 
                                 (volatile int32_t*)p);
}

static inline PlankB pl_AtomicL_CompareAndSwap (PlankAtomicLRef p, PlankL oldValue, PlankL newValue)
{    
    return OSAtomicCompareAndSwap32Barrier (*(int32_t*)&oldValue, 
                                            *(int32_t*)&newValue, 
                                            (volatile int32_t*)p);
}
#endif //PLANK_32BIT

#if PLANK_64BIT
static inline PlankL pl_AtomicL_Add (PlankAtomicLRef p, PlankL operand)
{
    return OSAtomicAdd64Barrier (*(int64_t*)&operand, 
                                 (volatile int64_t*)p);
}

static inline PlankB  pl_AtomicL_CompareAndSwap (PlankAtomicLRef p, PlankL oldValue, PlankL newValue)
{    
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldValue, 
                                            *(int64_t*)&newValue, 
                                            (volatile int64_t*)p);
}
#endif //PLANK_64BIT
#endif //PLANK_APPLE

#if PLANK_WIN
#if PLANK_32BIT
static inline PlankL pl_AtomicL_Add (PlankAtomicLRef p, PlankL operand)
{
    return operand + _InterlockedExchangeAdd ((volatile long*)p, operand);
}

static inline PlankB  pl_AtomicL_CompareAndSwap (PlankAtomicLRef p, PlankL oldValue, PlankL newValue)
{    
    return oldValue == _InterlockedCompareExchange ((volatile long*)p, newValue, oldValue);
}
#endif //PLANK_32BIT

#if PLANK_64BIT
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

static inline PlankB  pl_AtomicL_CompareAndSwap (PlankAtomicLRef p, PlankL oldValue, PlankL newValue)
{    
    return (*(PlankULL*)&oldValue) == pl_InterlockedCompareExchange64 ((volatile PlankULL*)p,
									  					               *(PlankULL*)&newValue, 
                                                                       *(PlankULL*)&oldValue);
}
#endif //PLANK_64BIT
#endif //PLANK_WIN

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

static inline PlankLL pl_AtomicLL_Get (PlankAtomicLLRef p)
{
    return pl_AtomicLL_Add (p, (PlankLL)0);
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

static inline void pl_AtomicLL_Set (PlankAtomicLLRef p, PlankLL newValue)
{
    pl_AtomicLL_Swap (p, newValue);
}

#if PLANK_APPLE
#if PLANK_X86 || PLANK_ARM // and PLANK_APPLE
static inline PlankLL pl_AtomicLL_Add (PlankAtomicLLRef p, PlankLL operand)
{
    return OSAtomicAdd64Barrier (*(int64_t*)&operand, 
                                 (volatile int64_t*)p);
}

static inline PlankB pl_AtomicLL_CompareAndSwap (PlankAtomicLLRef p, PlankLL oldValue, PlankLL newValue)
{    
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldValue, 
                                            *(int64_t*)&newValue, 
                                            (volatile int64_t*)p);
}
#endif // PLANK_X86 and PLANK_APPLE

#if PLANK_PPC // and PLANK_APPLE
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
#endif // PLANK_PPC and PLANK_APPLE
#endif

#if PLANK_WIN
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
    return oldValue == (PlankLL)pl_InterlockedCompareExchange64 ((volatile PlankULL*)p,
							   						             *(PlankULL*)&newValue, 
																 *(PlankULL*)&oldValue);
}
#endif

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

static inline PlankF pl_AtomicF_Get (PlankAtomicFRef p)
{
    PlankI bits = pl_AtomicI_Add ((PlankAtomicIRef)p, 0); // use the I version
    return *(PlankF*)&bits;
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

#if PLANK_APPLE
static inline PlankB pl_AtomicF_CompareAndSwap (PlankAtomicFRef p, PlankF oldValue, PlankF newValue)
{    
    return OSAtomicCompareAndSwap32Barrier (*(int32_t*)&oldValue, 
                                            *(int32_t*)&newValue, 
                                            (volatile int32_t*)p);
}
#endif

#if PLANK_WIN
static inline PlankB pl_AtomicF_CompareAndSwap (PlankAtomicFRef p, PlankF oldValue, PlankF newValue)
{   
	long oldLong = *(long*)&oldValue;
    return oldLong == _InterlockedCompareExchange ((volatile long*)p,
                                                   *(long*)&newValue, 
                                                   oldLong);
}
#endif

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

static inline PlankD pl_AtomicD_Get (PlankAtomicDRef p)
{
    PlankLL bits = pl_AtomicLL_Add ((PlankAtomicLLRef)p, (PlankLL)0); // use the LL version
    return *(PlankD*)&bits;
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

#if PLANK_APPLE
#if PLANK_X86 || PLANK_ARM // and PLANK_APPLE
static inline PlankB pl_AtomicD_CompareAndSwap (PlankAtomicDRef p, PlankD oldValue, PlankD newValue)
{    
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldValue, 
                                            *(int64_t*)&newValue, 
                                            (volatile int64_t*)p);
}
#endif // PLANK_X86 and PLANK_APPLE

#if PLANK_PPC // and PLANK_APPLE
static inline PlankB pl_AtomicD_CompareAndSwap (PlankAtomicDRef p, PlankD oldValue, PlankD newValue)
{    
    // must use an integer compare since comparing floating point 
    // values doesn't do a bitwise comparison
    return pl_AtomicLL_CompareAndSwap ((PlankAtomicLLRef)p, 
                                       *(PlankLL*)&oldValue, 
                                       *(PlankLL*)&newValue);
}
#endif // PLANK_PPC and PLANK_APPLE
#endif // PLANK_APPLE

#if PLANK_WIN
inline PlankB pl_AtomicD_CompareAndSwap (PlankAtomicDRef p, PlankD oldValue, PlankD newValue)
{   
    PlankULL oldBits = *(PlankULL*)&oldValue;
    return oldBits == pl_InterlockedCompareExchange64 ((volatile PlankULL*)p,
                                                       *(PlankULL*)&newValue, 
                                                       oldBits);
}
#endif

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

static inline PlankP pl_AtomicP_Get (PlankAtomicPRef p)
{
    return pl_AtomicP_Add (p, (PlankL)0);
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

static inline void pl_AtomicP_Set (PlankAtomicPRef p, PlankP newPtr)
{
    pl_AtomicP_Swap (p, newPtr);
}

#if PLANK_APPLE
#if PLANK_32BIT // and PLANK_APPLE
static inline PlankP pl_AtomicP_Add (PlankAtomicPRef p, PlankL operand)
{
    return (PlankP)OSAtomicAdd32Barrier (*(int32_t*)&operand, (volatile int32_t*)p);
}

static inline PlankB pl_AtomicP_CompareAndSwap (PlankAtomicPRef p, PlankP oldValue, PlankP newValue)
{
    return OSAtomicCompareAndSwap32Barrier (*(int32_t*)&oldValue, 
                                            *(int32_t*)&newValue, 
                                            (volatile int32_t*)p);
}
#endif // PLANK_32BIT and PLANK_APPLE

#if PLANK_64BIT // and PLANK_APPLE
static inline PlankP pl_AtomicP_Add (PlankAtomicPRef p, PlankL operand)
{
    return (PlankP)OSAtomicAdd64Barrier (*(int64_t*)&operand, (volatile int64_t*)p);
}

static inline PlankB pl_AtomicP_CompareAndSwap (PlankAtomicPRef p, PlankP oldValue, PlankP newValue)
{
    return OSAtomicCompareAndSwap64Barrier (*(int64_t*)&oldValue, 
                                            *(int64_t*)&newValue, 
                                            (volatile int64_t*)p);
}
#endif // PLANK_64BIT and PLANK_APPLE
#endif // PLANK_APPLE

#if PLANK_WIN
#if PLANK_32BIT
static inline PlankP pl_AtomicP_Add (PlankAtomicPRef p, PlankL operand)
{
    return (PlankP)(_InterlockedExchangeAdd ((volatile long*)&p, operand) + operand);
}

static inline PlankB pl_AtomicP_CompareAndSwap (PlankAtomicPRef p, PlankP oldValue, PlankP newValue)
{    
	long oldLong = *(long*)&oldValue;
    return oldLong == _InterlockedCompareExchange ((volatile long*)p, 
                                                   *(long*)&newValue, 
                                                   oldLong);
}
#endif

#if PLANK_64BIT
static inline PlankP pl_AtomicP_Add (PlankAtomicPRef p, PlankL operand)
{
    PlankP oldValue, newValue;
    PlankB success;
    
    do {
        oldValue = *(PlankP*)p;
        newValue = (PlankUC*)oldValue + operand;
        success = pl_AtomicP_CompareAndSwap (p, oldValue, newValue);
    } while (!success);
    
    return newValue;        
}

static inline PlankB pl_AtomicP_CompareAndSwap (PlankAtomicPRef p, PlankP oldValue, PlankP newValue)
{    
    PlankULL oldBits = *(PlankULL*)&oldValue;
    return oldBits == pl_InterlockedCompareExchange64 ((volatile PlankULL*)p, 
                                                       *(PlankULL*)&newValue, 
                                                       *(PlankULL*)&oldValue);    
}
#endif
#endif

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


#if !DOXYGEN
typedef struct PlankAtomicI
{
    volatile PlankI value;
} PlankAtomicI;

typedef struct PlankAtomicL
{
    volatile PlankL value;
} PlankAtomicL;

typedef struct PlankAtomicLL
{
    volatile PlankLL value;
#if PLANK_NOATOMIC64BIT
    PlankLockRef lock;
#endif
} PlankAtomicLL;

typedef struct PlankAtomicF
{
    volatile PlankF value;
} PlankAtomicF;

typedef struct PlankAtomicD
{
    volatile PlankD value;
#if PLANK_NOATOMIC64BIT
    PlankLockRef lock;
#endif    
} PlankAtomicD;

typedef struct PlankAtomicP
{
    volatile PlankP ptr;
} PlankAtomicP;

typedef struct PlankAtomicPX
{
    volatile PlankP ptr;
    volatile PlankL extra;
#if PLANK_NOATOMIC64BIT
    PlankLockRef lock;
#endif    
} PlankAtomicPX;

typedef struct PlankAtomicLX
{
    volatile PlankL value;
    volatile PlankL extra;
#if PLANK_NOATOMIC64BIT
    PlankLockRef lock;
#endif    
} PlankAtomicLX;
#endif


#endif //PLANK_INLINING_FUNCTIONS

