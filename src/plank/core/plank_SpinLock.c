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

#include "plank_StandardHeader.h"
#include "plank_SpinLock.h"
#include "plank_Thread.h"

#define PLANK_SPINLOCK_ITERS 20

PlankSpinLockRef pl_SpinLock_CreateAndInit()
{
    PlankSpinLockRef p;
    p = pl_SpinLock_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_SpinLock_Init (p) != PlankResult_OK)
            pl_SpinLock_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}
 
PlankSpinLockRef pl_SpinLock_Create()
{
    PlankMemoryRef m;
    PlankSpinLockRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankSpinLockRef)pl_Memory_AllocateBytes (m, sizeof (PlankSpinLock));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankSpinLock));
    
    return p;
}

PlankResult pl_SpinLock_Destroy (PlankSpinLockRef p)
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
    
    if ((result = pl_SpinLock_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

PlankResult pl_SpinLock_Init (PlankSpinLockRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }    
    
    pl_AtomicI_Init (&p->flag);
    
exit:
    return result;    
}

PlankResult pl_SpinLock_DeInit (PlankSpinLockRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_AtomicI_DeInit (&p->flag);
    pl_MemoryZero (p, sizeof (PlankSpinLock));

exit:
    return result;
}

void pl_SpinLock_Lock (PlankSpinLockRef p)
{
    int i;
    
    for (i = 0; i < PLANK_SPINLOCK_ITERS; ++i)
        if (pl_SpinLock_TryLock (p))
            return;
    
    while (!pl_SpinLock_TryLock (p))
        pl_ThreadYield();
}

void pl_SpinLock_Unlock (PlankSpinLockRef p)
{
    pl_AtomicMemoryBarrier();
    p->flag.value = PLANK_SPINLOCK_UNLOCKED;
}

void pl_SpinLock_Wait (PlankSpinLockRef p)
{
    int i;
    
    for (i = 0; i < PLANK_SPINLOCK_ITERS; ++i)
        if (p->flag.value == PLANK_SPINLOCK_UNLOCKED)
            return;
    
    while (p->flag.value != PLANK_SPINLOCK_UNLOCKED)
        pl_ThreadYield();

}

void pl_SpinLock_WaitTimeout (PlankSpinLockRef p, double time)
{
    
}

void pl_SpinLock_Signal (PlankSpinLockRef p)
{
    pl_SpinLock_Unlock (p);
}


