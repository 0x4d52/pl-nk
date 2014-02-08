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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"

#define PLONK_OBJECTMEMORYPOOLS_DEBUG 1

#if PLONK_OBJECTMEMORYPOOLS_DEBUG
//static AtomicLong largestSize;
//static AtomicInt blockAllocationCounts[ObjectMemoryPools::NumQueues];
//static AtomicInt blockDeallocationCounts[ObjectMemoryPools::NumQueues];
#endif

void* ObjectMemoryPools::staticAlloc (void* userData, UnsignedLong size)
{
    ObjectMemoryPools& om = *static_cast<ObjectMemoryPools*> (userData);
    return om.allocateBytes (size);
}

void ObjectMemoryPools::staticFree (void* userData, void* ptr)
{
    ObjectMemoryPools& om = *static_cast<ObjectMemoryPools*> (userData);
    om.free (ptr);
}

static inline void* staticDoAlloc (void* userData, UnsignedLong requestedSize) throw()
{    
    const UnsignedLong align = PLONK_WORDSIZE * 2;
    const UnsignedLong size = Bits::nextPowerOf2 (requestedSize + align);
    UnsignedChar* raw = static_cast<UnsignedChar*> (pl_MemoryDefaultAllocateBytes (userData, size));
    *reinterpret_cast<UnsignedLong*> (raw) = size;
        
    return raw + align;
}

static inline void staticDoFree (void* userData, void* ptr) throw()
{
#if PLONK_OBJECTMEMORYPOOLS_DEBUG
    plonk_assert (!Threading::currentThreadIsAudioThread());
#endif
    
    if (ptr != 0)
    {
        const UnsignedLong align = PLONK_WORDSIZE * 2;
        UnsignedChar* const raw = static_cast<UnsignedChar*> (ptr) - align;
        pl_MemoryDefaultFree (userData, raw);
    }
}

ObjectMemoryPools::ObjectMemoryPools (Memory& m) throw()
:   ObjectMemoryBase (m),
    Threading::Thread ("plonk::ObjectMemoryPools::Threading::Thread")
{
    getMemory().resetUserData();
    getMemory().setFunctions (staticDoAlloc, staticDoFree); 
    
    AtomicOps::memoryBarrier();
    queues = new LockFreeQueue<Element>[NumQueues];
    AtomicOps::memoryBarrier();
    
    getMemory().setUserData (this);
    getMemory().setFunctions (staticAlloc, staticFree); 
}

ObjectMemoryPools::~ObjectMemoryPools()
{
    setShouldExitAndWait(); // the thread clears the queues
    //<-- something could happen here on another thread but we should be shut down by now..?
    getMemory().resetUserData();
    getMemory().setFunctions (staticDoAlloc, staticDoFree); 
    delete [] queues;
}

void* ObjectMemoryPools::allocateBytes (UnsignedLong requestedSize)
{    
    const UnsignedLong align = PLONK_WORDSIZE * 2;
    const UnsignedLong size = Bits::nextPowerOf2 (requestedSize + align);
    const UnsignedLong sizeLog2 = size > 0 ? Bits::countTrailingZeroes (size) : 0;
    plonk_assert (sizeLog2 >= 0 && sizeLog2 < NumQueues);
    
    void* rtn = 0;
    Element e = queues[sizeLog2].pop();
    
    if (e.ptr != 0)
    {
        rtn = e.ptr;
#if PLONK_DEBUG
        UnsignedChar* raw = static_cast<UnsignedChar*> (rtn) - align;
        plonk_assert (*reinterpret_cast<UnsignedLong*> (raw) == size);
#endif
    }
    else
    {
        rtn = staticDoAlloc (this, size);
    }

    return rtn;
}

void ObjectMemoryPools::free (void* ptr)
{
    if (ptr != 0)
    {
        const UnsignedLong align = PLONK_WORDSIZE * 2;
        UnsignedChar* const raw = static_cast<UnsignedChar*> (ptr) - align;
        const UnsignedLong size = *reinterpret_cast<UnsignedLong*> (raw);
        const UnsignedLong sizeLog2 = size > 0 ? Bits::countTrailingZeroes (size) : 0;
        plonk_assert (sizeLog2 >= 0 && sizeLog2 < NumQueues);

        Element e (ptr);
        queues[sizeLog2].push (e);
    }
}

ResultCode ObjectMemoryPools::run() throw()
{
    int i;
//    const double minDuration = 0.000001;
    const double maxDuration = 0.1;
    double duration = maxDuration;//minDuration;
    
    while (!getShouldExit())
    {
        for (i = 0; i < NumQueues; ++i)
        {
            plonk_assert (getMemory().getUserData() == this);
            // could delete some here gradually
            Threading::sleep (duration);
        }
    }
    
    getMemory().setFunctions (staticDoAlloc, staticDoFree); 
    
    for (i = 0; i < NumQueues; ++i)
        queues[i].clearAll();
    
    return 0;
}


END_PLONK_NAMESPACE
