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

#define PLONK_OBJECTMEMORYPOOLS_DEBUG 1

#if PLONK_OBJECTMEMORYPOOLS_DEBUG
static AtomicLong largestSize;
static AtomicInt blockCounts[64];
#endif

void* ObjectMemoryPools::staticAlloc (PlankUL size)
{
    return ObjectMemoryPools::global().allocateBytes (size);
}

void ObjectMemoryPools::staticFree (void* ptr)
{
    ObjectMemoryPools::global().free (ptr);
}

static inline void staticDoFree (void* ptr) throw()
{
#if PLONK_OBJECTMEMORYPOOLS_DEBUG
    plonk_assert (!Threading::currentThreadIsAudioThread());
#endif
    const PlankUL align = PLONK_WORDSIZE * 2;
    PlankUC* const raw = static_cast<PlankUC*> (ptr) - align;
    ::free (raw);
}

ObjectMemoryPools& ObjectMemoryPools::global() throw()
{
    static ObjectMemoryPools* om = new ObjectMemoryPools (Memory::global()); // just leak
    
    if (!om->isRunning()) // how best to avoid this without calling ->start from the constructor?
        om->start();
    
    return *om;
}

ObjectMemoryPools::ObjectMemoryPools (Memory& m) throw()
:   memory (m)
{
    memory.setFunctions (staticAlloc, staticFree);    
}

ObjectMemoryPools::~ObjectMemoryPools()
{
    //memory.setFunctions (staticAlloc, staticDoFree);
    setShouldExitAndWait();
    //aarggh.. something could happen here on another thread!?
    memory.setFunctions (malloc, ::free);
}

void* ObjectMemoryPools::allocateBytes (PlankUL requestedSize)
{    
    const PlankUL align = PLONK_WORDSIZE * 2;
    const PlankUL size = Bits<PlankUL>::nextPowerOf2 (requestedSize + align);
    const PlankUL sizeLog2 = size > 0 ? Bits<PlankUL>::countTrailingZeroes (size) : 0;
    plonk_assert (sizeLog2 >= 0 && sizeLog2 < NumQueues);
    
    void* rtn = 0;
    PlankUC* raw;
    Element e = queues[sizeLog2].pop();
    
    if (e.ptr != 0)
    {
        rtn = e.ptr;
#if PLONK_DEBUG
        raw = static_cast<PlankUC*> (rtn) - align;
        plonk_assert (*reinterpret_cast<PlankUL*> (raw) == size);
#endif
    }
    else
    {
#if PLONK_OBJECTMEMORYPOOLS_DEBUG
        //plonk_assert (!Threading::currentThreadIsAudioThread());
        blockCounts[sizeLog2]++;
        largestSize.setIfLarger (size);
#endif
    
        raw = static_cast<PlankUC*> (malloc (size));
        *reinterpret_cast<PlankUL*> (raw) = size;
        rtn = raw + align;
    }

    return rtn;
}

void ObjectMemoryPools::free (void* ptr)
{
    if (ptr != 0)
    {
        const PlankUL align = PLONK_WORDSIZE * 2;
        PlankUC* const raw = static_cast<PlankUC*> (ptr) - align;
        const PlankUL size = *reinterpret_cast<PlankUL*> (raw);
        const PlankUL sizeLog2 = size > 0 ? Bits<PlankUL>::countTrailingZeroes (size) : 0;
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
            // could delete some here gradually
            Threading::sleep (duration);
        }
    }
    
    for (i = 0; i < NumQueues; ++i)
    {
        Element e;
        
        do 
        {
            e = queues[i].pop();
            staticDoFree (e.ptr);
        } while (e.ptr != 0);
    }
    
    return 0;
}


END_PLONK_NAMESPACE
