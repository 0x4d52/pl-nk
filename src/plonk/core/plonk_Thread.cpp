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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "plonk_Thread.h"
#include "../containers/plonk_Atomic.h"

PlankResult Threading::run (PlankThreadRef plankThread) throw()
{
    Threading::Thread* thread = static_cast<Threading::Thread*> (pl_Thread_GetUserData (plankThread));
    
    ResultCode result = thread->run();
    plonk_assert (result == PlankResult_OK);
    
    return result;
}

ResultCode Threading::sleep (const double seconds) throw()
{
    ResultCode result = pl_ThreadSleep (seconds);
    plonk_assert (result == PlankResult_OK);
    return result;
}

ResultCode Threading::yield() throw()
{
    ResultCode result = pl_ThreadYield();
    plonk_assert (result == PlankResult_OK);
    return result;   
}

Threading::ID Threading::getCurrentThreadID() throw()
{
    return pl_ThreadCurrentID();
}

//static AtomicValue<Threading::ID>& getAudioThreadIDRef() throw()
//{
//    static AtomicValue<Threading::ID>* audioThreadID = new AtomicValue<Threading::ID>; // just leak this one...
//    return *audioThreadID;
//}

static AtomicValue<Threading::ID>& getAudioThreadIDRef() throw()
{
    static AtomicValue<Threading::ID> audioThreadID;
    return audioThreadID;
}

Threading::ID Threading::getAudioThreadID() throw()
{
    return getAudioThreadIDRef().getValueUnchecked();
}

void Threading::setAudioThreadID (const Threading::ID theID) throw()
{
    getAudioThreadIDRef().setValue (theID);
}

bool Threading::currentThreadIsAudioThread() throw()
{
    Threading::ID audioThreadID = getAudioThreadID();
    return audioThreadID == 0 ? false : audioThreadID == getCurrentThreadID();
}

Threading::Thread::Thread() throw()
{
    ResultCode result;
    result = pl_Thread_Init (getPeerRef());
    plonk_assert (result == PlankResult_OK);

    result = pl_Thread_SetFunction (getPeerRef(), Threading::run);
    plonk_assert (result == PlankResult_OK);
    
    result = pl_Thread_SetUserData (getPeerRef(), this);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

Threading::Thread::~Thread()
{
    pl_Thread_DeInit (getPeerRef());
}

ResultCode Threading::Thread::start() throw()
{
    ResultCode result = pl_Thread_Start (getPeerRef());
    plonk_assert (result == PlankResult_OK);
    return result;
}

ResultCode Threading::Thread::cancel() throw()
{
    ResultCode result = pl_Thread_Cancel (getPeerRef());
    plonk_assert (result == PlankResult_OK);
    return result;
}

ResultCode Threading::Thread::wait() throw()
{
    ResultCode result = pl_Thread_Wait (getPeerRef());
    plonk_assert (result == PlankResult_OK);
    return result;
}

bool Threading::Thread::isRunning() throw()
{
    return pl_Thread_IsRunning (getPeerRef());
}

ResultCode Threading::Thread::setShouldExit() throw()
{
    ResultCode result = pl_Thread_SetShouldExit (getPeerRef());
    plonk_assert (result == PlankResult_OK);
    return result;
}

ResultCode Threading::Thread::setShouldExitAndWait (const double interval) throw()
{
    ResultCode result = PlankResult_OK;
    
    result = setShouldExit();
    plonk_assert (result == PlankResult_OK);
    
    while (isRunning())
        Threading::sleep (interval);
    
    return result;
}

bool Threading::Thread::getShouldExit() throw()
{
    return pl_Thread_GetShouldExit (getPeerRef());
}

Threading::ID Threading::Thread::getID() throw()
{
    return pl_Thread_GetID (getPeerRef());
}

ResultCode Threading::Thread::sleep (const double seconds) throw()
{
    return Threading::sleep (seconds);
}

ResultCode Threading::Thread::yield() throw()
{
    return Threading::yield();
}

END_PLONK_NAMESPACE
