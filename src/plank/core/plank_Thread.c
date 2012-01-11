/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
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
 -------------------------------------------------------------------------------
 */

#include "plank_StandardHeader.h"
#include "plank_Thread.h"
#include "../maths/plank_Maths.h"


typedef PlankThreadNativeReturn (PLANK_THREADCALL *PlankThreadNativeFunction)(PlankP);
PlankThreadNativeReturn PLANK_THREADCALL pl_ThreadNativeFunction (PlankP argument);

void pl_Thread_Reset (PlankThreadRef p)
{
    pl_AtomicI_Set (&p->shouldExitAtom, PLANK_FALSE);
    p->thread = (PlankThreadNativeHandle)0;
    p->threadID = (PlankThreadID)0;
    pl_AtomicI_Set (&p->isRunningAtom, PLANK_FALSE);    
}

PlankResult pl_ThreadSleep (PlankD seconds)
{
    pl_AtomicMemoryBarrier();

#if PLANK_APPLE
    useconds_t useconds = (useconds_t)pl_MaxD (seconds * 1000000.0, 0.0);
    
    if (useconds > 0)
        usleep (useconds);
    else
        sched_yield();

#elif PLANK_WIN    
    Sleep ((int)pl_MaxD (seconds * 1000.0, 0.0));
#endif
    
    return PlankResult_OK;
}

PlankResult pl_ThreadYield()
{
    pl_ThreadSleep (0.0);
    return PlankResult_OK;
}

PlankThreadID pl_ThreadCurrentID()
{
#if PLANK_APPLE
    return (PlankThreadID)pthread_self();
#elif PLANK_WIN
    return (PlankThreadID)GetCurrentThreadId();
#endif
}

PlankThreadNativeReturn PLANK_THREADCALL pl_ThreadNativeFunction (PlankP argument)
{
    PlankResult result;
    PlankThreadRef p;
    
    p = (PlankThreadRef)argument;
    
    if (p->function == 0)
        result = PlankResult_ThreadFunctionInvalid;
    else
        result = (*p->function) (p);
        
    pl_Thread_Reset (p);

    return (result == PlankResult_OK) ? 0 : (PlankThreadNativeReturn)(-1);
}

PlankThreadRef pl_Thread_CreateAndInit()
{
    PlankThreadRef p;
    p = pl_Thread_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_Thread_Init (p) != PlankResult_OK)
            pl_Thread_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankThreadRef pl_Thread_Create()
{
    PlankMemoryRef m;
    PlankThreadRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankThreadRef)pl_Memory_AllocateBytes (m, sizeof (PlankThread));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankThread));
    
    return p;
}

PlankResult pl_Thread_Init (PlankThreadRef p)
{
    PlankResult result = PlankResult_OK;
    
    p->function = (PlankThreadFunction)0;
    
    pl_AtomicI_Init (&p->shouldExitAtom);
    pl_AtomicI_Init (&p->isRunningAtom);
    pl_AtomicPX_Init (&p->userDataAtom);
        
    pl_Thread_Reset (p);
    
exit:
    return result;
}

PlankResult pl_Thread_DeInit (PlankThreadRef p)
{
    PlankResult result = PlankResult_OK;
        
    pl_AtomicI_DeInit (&p->shouldExitAtom);
    pl_AtomicI_DeInit (&p->isRunningAtom);
    pl_AtomicPX_DeInit (&p->userDataAtom);

exit:
    return result;    
}

PlankResult pl_Thread_Destroy (PlankThreadRef p)
{
    PlankResult result = PlankResult_OK;    
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_Thread_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

PlankThreadID pl_Thread_GetID (PlankThreadRef p)
{
    return p->threadID;
}

PlankResult pl_Thread_SetFunction (PlankThreadRef p, PlankThreadFunction function)
{
    if (pl_AtomicI_Get (&p->isRunningAtom))
        return PlankResult_ThreadSetFunctionFailed;
    
    p->function = function;
    return PlankResult_OK;
}

PlankResult pl_Thread_SetUserData (PlankThreadRef p, PlankP userData)
{
    if (pl_AtomicI_Get (&p->isRunningAtom))
        return PlankResult_ThreadSetUserDataFailed;

    pl_AtomicPX_Set (&p->userDataAtom, userData);
    return PlankResult_OK;
}

PlankP pl_Thread_GetUserData (PlankThreadRef p)
{
    return pl_AtomicPX_Get (&p->userDataAtom);
}

PlankResult pl_Thread_Start (PlankThreadRef p)
{
    if (pl_AtomicI_Get (&p->isRunningAtom))
        return PlankResult_ThreadAlreadyRunning;
    
    if (p->function == PLANK_NULL)
        return PlankResult_ThreadFunctionInvalid;
    
#if PLANK_APPLE
    if (pthread_create (&p->thread, NULL, pl_ThreadNativeFunction, p) != 0)
        return PlankResult_ThreadStartFailed;
    
    p->threadID = (PlankThreadID)p->thread;
    
#elif PLANK_WIN    
    if ((p->thread = _beginthreadex (NULL, 0, pl_ThreadNativeFunction, p, 0, (unsigned int*)&p->threadID)) == 0) 
        return PlankResult_ThreadStartFailed;

#endif    
    pl_AtomicI_Set (&p->isRunningAtom, PLANK_TRUE);
    
    return PlankResult_OK;
}

PlankResult pl_Thread_Cancel (PlankThreadRef p)
{
    if (! p->thread)
        return PlankResult_ThreadInvalid;

#if PLANK_APPLE
    if (pthread_cancel (p->thread) == 0)
    {
        pl_Thread_Reset (p);
        return PlankResult_OK;
    }
    
#elif PLANK_WIN   
    TerminateThread ((HANDLE)p->thread, 0);
    return PlankResult_OK;

#endif
    return PlankResult_ThreadCancelFailed;
}

PlankResult pl_Thread_Wait (PlankThreadRef p)
{
    if (! p->thread)
        return PlankResult_ThreadInvalid;
    
#if PLANK_APPLE
    if (pthread_join (p->thread, NULL) == 0) 
    {
        pl_Thread_Reset (p);
        return PlankResult_OK;
    }
    
#elif PLANK_WIN    
    if (WaitForSingleObject ((HANDLE)p->thread, INFINITE) == WAIT_OBJECT_0) 
    {
        CloseHandle ((HANDLE)p->thread);
        pl_Thread_Reset (p);
        return PlankResult_OK;
    }
    
#endif
    return PlankResult_ThreadWaitFailed;    
}

PlankB pl_Thread_IsRunning (PlankThreadRef p)
{
    return pl_AtomicI_Get (&p->isRunningAtom) != 0;
}

PlankResult pl_Thread_SetShouldExit (PlankThreadRef p)
{
    if (pl_AtomicI_Get (&p->shouldExitAtom) != 0)
        return PlankResult_ThreadShouldExitAlreadySet;
        
    pl_AtomicI_Set (&p->shouldExitAtom, PLANK_TRUE);
    return PlankResult_OK;
}

PlankB pl_Thread_GetShouldExit (PlankThreadRef p)
{
    return pl_AtomicI_Get (&p->shouldExitAtom) != 0;
}



