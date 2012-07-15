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
#include "plank_Lock.h"

PlankLockRef pl_Lock_CreateAndInit()
{
    PlankLockRef p;
    p = pl_Lock_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_Lock_Init (p) != PlankResult_OK)
            pl_Lock_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankLockRef pl_Lock_Create()
{
    PlankMemoryRef m;
    PlankLockRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankLockRef)pl_Memory_AllocateBytes (m, sizeof (PlankLock));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankLock));
    
    return p;
}

PlankResult pl_Lock_Destroy (PlankLockRef p)
{
    PlankResult result = PlankResult_OK;
    
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_Lock_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

//------------------------------------------------------------------------------

#if PLANK_APPLE

PlankResult pl_Lock_Init (PlankLockRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }    
    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init (&attr);
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_INHERIT);
    pthread_mutex_init (&p->mutex, &attr);
    pthread_cond_init (&p->condition, NULL);    
    
exit:
    return result;    
}

PlankResult pl_Lock_DeInit (PlankLockRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pthread_mutex_destroy (&p->mutex);
    pthread_cond_destroy (&p->condition);

exit:
    return result;
}

void pl_Lock_Lock (PlankLockRef p)
{
    pthread_mutex_lock (&p->mutex);
}

void pl_Lock_Unlock (PlankLockRef p)
{
    pthread_mutex_unlock (&p->mutex);
}

PlankB pl_Lock_TryLock (PlankLockRef p)
{
    return pthread_mutex_trylock (&p->mutex) == 0;
}

void pl_Lock_Wait (PlankLockRef p)
{
    pthread_cond_wait (&p->condition, &p->mutex);
}

void pl_Lock_Signal (PlankLockRef p)
{
    pthread_cond_signal(&p->condition);
}

#endif // PLANK_APPLE

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

#if PLANK_WIN
PlankResult pl_Lock_Init (PlankLockRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }    
    
    InitializeCriticalSection (&p->mutex);
    p->condition = CreateEvent (NULL,        // no security
                                PLANK_TRUE,  // manual-reset
                                PLANK_FALSE, // non-signaled initially
                                NULL);       // unnamed
    
    if (!p->condition)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
exit:
    return result;    
}

PlankResult pl_Lock_DeInit (PlankLockRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    DeleteCriticalSection (&p->mutex);
    CloseHandle (p->condition);
    
exit:
    return result;
}

void pl_Lock_Lock (PlankLockRef p)
{
    EnterCriticalSection (&p->mutex);
}

void pl_Lock_Unlock (PlankLockRef p)
{
    LeaveCriticalSection (&p->mutex);
}

PlankB pl_Lock_TryLock (PlankLockRef p)
{
    return TryEnterCriticalSection (&p->mutex) != 0;
}

void pl_Lock_Wait (PlankLockRef p)
{
    WaitForMultipleObjects (1, &p->condition, PLANK_FALSE, INFINITE);
}

void pl_Lock_Signal (PlankLockRef p)
{
    SetEvent (p->condition);
}

#endif // PLANK_WIN

//------------------------------------------------------------------------------


