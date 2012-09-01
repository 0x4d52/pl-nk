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

#include "../core/plank_StandardHeader.h"
#include "plank_RNG.h"
#include "../containers/plank_Atomic.h"

PlankRNGRef pl_RNGGlobal()
{
    static PlankAtomicI init = { 0 };
    static PlankRNG rng;
    
    if (pl_AtomicI_CompareAndSwap (&init, 0, 1))
        pl_RNG_Init (&rng);
    
    return &rng;
}

PlankRNGRef pl_RNG_CreateAndInit()
{
    PlankRNGRef p;
    p = pl_RNG_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_RNG_Init (p) != PlankResult_OK)
            pl_RNG_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankRNGRef pl_RNG_Create()
{
    PlankMemoryRef m;
    PlankRNGRef p;

    m = pl_MemoryGlobal();
    p = (PlankRNGRef)pl_Memory_AllocateBytes (m, sizeof (PlankRNG));
    
    if (p != NULL)
        pl_MemoryZero (p, sizeof (PlankRNG));
    
    return p;
}

PlankResult pl_RNG_Init (PlankRNGRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    p->value = (unsigned int)time (NULL);
    return PlankResult_OK;
}

PlankResult pl_RNG_DeInit (PlankRNGRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;

    return PlankResult_OK;
}

PlankResult pl_RNG_Destroy (PlankRNGRef p)
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
    
    if ((result = pl_RNG_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
        
exit:
    return result;
}

void pl_RNG_Seed (PlankRNGRef p, unsigned int seed)
{
    p->value = seed;
}
