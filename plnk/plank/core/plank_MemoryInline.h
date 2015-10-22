/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

//#define PLANK_MEMORY_DEBUG 1

// help prevent accidental inclusion other than via the intended header
#if PLANK_INLINING_FUNCTIONS

#include "../core/plank_Lock.h"

#if !DOXYGEN
typedef struct PlankMemory
{
    PlankMemoryAllocateBytesFunction allocFunction;
    PlankMemoryFreeFunction freeFunction;
    PlankP userData;
    PlankLock lock;
} PlankMemory; 
#endif

static PLANK_INLINE_LOW PlankB pl_MemoryCompare (PlankConstantP ptr1, PlankConstantP ptr2, const PlankUL numBytes)
{
    return memcmp (ptr1, ptr2, numBytes) == 0 ? PLANK_TRUE : PLANK_FALSE;
}

static PLANK_INLINE_LOW PlankResult pl_MemoryZero (PlankP ptr, const PlankUL numBytes)
{
    PlankUL i;
    PlankUC *dstp;

    if (ptr == PLANK_NULL || numBytes == 0)
        return PlankResult_MemoryError;
    
//    memset (ptr, 0, numBytes);
    
    dstp = (PlankUC*)ptr;

    for (i = 0; i < numBytes; ++i)
        dstp[i] = 0;

    return PlankResult_OK;
}

static PLANK_INLINE_LOW PlankResult pl_MemoryCopy (PlankP dst, PlankConstantP src, const PlankUL numBytes)
{    
    PlankUL i;
    PlankUC *srcp, *dstp;
    
    if (src == PLANK_NULL || dst == PLANK_NULL || numBytes == 0)
        return PlankResult_MemoryError;
    
//    memcpy (dst, src, numBytes);
    
    srcp = (PlankUC*)src;
    dstp = (PlankUC*)dst;
    
    // alignment issue?
    for (i = 0; i < numBytes; ++i)
        dstp[i] = srcp[i];
    
    return PlankResult_OK;    
}

static PLANK_INLINE_LOW PlankP pl_Memory_AllocateBytes (PlankMemoryRef p, PlankUL numBytes)
{
    PlankP ptr;
    
    ptr = (p->allocFunction) (p->userData, numBytes);

#if defined(PLANK_DEBUG) && PLANK_MEMORY_DEBUG
    printf ("pl_Memory_AllocateBytes(%p, %ld) = %p\n", p, numBytes, ptr);
#endif
    
    return ptr;
}

static PLANK_INLINE_LOW PlankResult pl_Memory_Free (PlankMemoryRef p, PlankP ptr)
{
#if defined(PLANK_DEBUG) && PLANK_MEMORY_DEBUG
    printf ("pl_Memory_Free(%p) = %p\n", p, ptr);
#endif    
    
    (p->freeFunction) (p->userData, ptr);

    return PlankResult_OK;
}

#endif //PLANK_INLINING_FUNCTIONS

