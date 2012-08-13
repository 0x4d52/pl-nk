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

#include "../core/plonk_Headers.h"

#define PLONK_OBJECTMEMORYDEFERFREE_DEBUG 1

void* ObjectMemoryDeferFree::staticAlloc (void* userData, PlankUL size)
{
    ObjectMemoryDeferFree& om = *static_cast<ObjectMemoryDeferFree*> (userData);
    return om.allocateBytes (size);
}

void ObjectMemoryDeferFree::staticFree (void* userData, void* ptr)
{
    ObjectMemoryDeferFree& om = *static_cast<ObjectMemoryDeferFree*> (userData);
    om.free (ptr);
}

static inline void staticDoFree (void* userData, void* ptr) throw()
{
#if PLONK_OBJECTMEMORYDEFERFREE_DEBUG
    plonk_assert (!Threading::currentThreadIsAudioThread());
#endif
    pl_MemoryDefaultFree (userData, ptr);
}

ObjectMemoryDeferFree::ObjectMemoryDeferFree (Memory& m) throw()
:   ObjectMemoryBase (m),
    Threading::Thread ("plonk::ObjectMemoryDeferFree::Threading::Thread")
{    
    getMemory().resetUserData();
    getMemory().resetFunctions();
    
    AtomicOps::memoryBarrier();
    queue = new LockFreeQueue<Element>;
    AtomicOps::memoryBarrier();
    
    getMemory().setUserData (this);
    getMemory().setFunctions (staticAlloc, staticFree); 
}

ObjectMemoryDeferFree::~ObjectMemoryDeferFree()
{    
    setShouldExitAndWait(); // the thread clears the queues
    //<-- something could happen here on another thread but we should be shut down by now..?
    getMemory().resetUserData();
    getMemory().resetFunctions(); 
    delete queue;
}

void* ObjectMemoryDeferFree::allocateBytes (PlankUL size)
{
    return pl_MemoryDefaultAllocateBytes (this, size);
}

void ObjectMemoryDeferFree::free (void* ptr)
{
    if (ptr != 0)
    {        
        if (Threading::getCurrentThreadID() == getID())
        {
            staticDoFree (this, ptr); // already triggered by a call on the background thread.
        }
        else
        {
            Element d (ptr);
            queue->push (d);
        }
    }
}

//void ObjectMemoryDeferFree::increaseQueueCache (const int count) throw()
//{
//    queue.increaseCache (count);
//}

ResultCode ObjectMemoryDeferFree::run() throw()
{
    const double minDuration = 0.000001;
    const double maxDuration = 0.1;
    double duration = minDuration;
    
    while (!getShouldExit())
    {
        plonk_assert (getMemory().getUserData() == this);

        Element d = queue->pop();
        
        if (d.ptr != 0)
        {
            duration = minDuration; // reset back to high speed
            staticDoFree (this, d.ptr);
        }
        else 
        {
            // gradually increase the amount of sleep if the queue is empty
            duration = plonk::min (duration * 2.0, maxDuration);
        }
        
        Threading::sleep (duration);
    }
    
    getMemory().resetFunctions();
    queue->clearAll();
    
    return 0;
}



END_PLONK_NAMESPACE
