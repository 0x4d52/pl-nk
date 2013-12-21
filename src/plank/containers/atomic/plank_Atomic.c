/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#include "../../core/plank_StandardHeader.h"
#include "plank_Atomic.h"

//------------------------------------------------------------------------------

PlankAtomicIRef pl_AtomicI_CreateAndInit()
{
    PlankAtomicIRef p = pl_AtomicI_Create();
    if (p != PLANK_NULL) pl_AtomicI_Init (p);
    return p;
}

PlankAtomicIRef pl_AtomicI_Create()
{
    PlankMemoryRef m;
    PlankAtomicIRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicIRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicI));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicI));
    
    return p;
}

PlankResult pl_AtomicI_Init (PlankAtomicIRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
//    pl_AtomicI_Set (p, (PlankI)0);
    p->value = 0;
    
    return PlankResult_OK;
}

PlankResult pl_AtomicI_DeInit (PlankAtomicIRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicI_Destroy (PlankAtomicIRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    return pl_Memory_Free (m, p);
}

PlankI pl_AtomicI_GetExtra (PlankAtomicIRef p)
{
    (void)p;
    return 0;
}

PlankI pl_AtomicI_GetExtraUnchecked (PlankAtomicIRef p)
{
    (void)p;
    return 0;
}

//------------------------------------------------------------------------------

PlankAtomicLRef pl_AtomicL_CreateAndInit()
{
    PlankAtomicLRef p = pl_AtomicL_Create();
    if (p != PLANK_NULL) pl_AtomicL_Init (p);
    return p;
}

PlankAtomicLRef pl_AtomicL_Create()
{
    PlankMemoryRef m;
    PlankAtomicLRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicLRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicL));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicL));

    return p;
}

PlankResult pl_AtomicL_Init (PlankAtomicLRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
//    pl_AtomicL_Set (p, (PlankL)0);
    p->value = 0;
    
    return PlankResult_OK;
}

PlankResult pl_AtomicL_DeInit (PlankAtomicLRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicL_Destroy (PlankAtomicLRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    return pl_Memory_Free (m, p);
}

PlankL pl_AtomicL_GetExtra (PlankAtomicLRef p)
{
    (void)p;
    return (PlankL)0;
}

PlankL pl_AtomicL_GetExtraUnchecked (PlankAtomicLRef p)
{
    (void)p;
    return (PlankL)0;
}

//------------------------------------------------------------------------------

PlankAtomicLLRef pl_AtomicLL_CreateAndInit()
{
    PlankAtomicLLRef p = pl_AtomicLL_Create();
    if (p != PLANK_NULL) pl_AtomicLL_Init (p);
    return p;
}

PlankAtomicLLRef pl_AtomicLL_Create()
{
    PlankMemoryRef m;
    PlankAtomicLLRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicLLRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicLL));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicLL));

    return p;
}

PlankResult pl_AtomicLL_Init (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_Init (&p->lock);
#endif    
    
//    pl_AtomicLL_Set (p, (PlankLL)0);
    p->value = 0;
    
exit:
    return result;
}

PlankResult pl_AtomicLL_DeInit (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_DeInit (&p->lock);
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicLL_Destroy (PlankAtomicLLRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();

    if ((result = pl_AtomicLL_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
exit:
    return result;
}

PlankLL pl_AtomicLL_GetExtra (PlankAtomicLLRef p)
{
    (void)p;
    return (PlankLL)0;
}

PlankLL pl_AtomicLL_GetExtraUnchecked (PlankAtomicLLRef p)
{
    (void)p;
    return (PlankLL)0;
}


#if PLANK_APPLE && PLANK_PPC
PlankB pl_AtomicLL_CompareAndSwap (PlankAtomicLLRef p, PlankLL oldValue, PlankLL newValue)
{    
    if (! pl_ThreadSpinLock_TryLock (&p->lock))
        return PLANK_FALSE;
    
    if (p->value != oldValue)
    {
        pl_ThreadSpinLock_Unlock (&p->lock);
        return PLANK_FALSE;
    }
    
    p->value = newValue;
    pl_ThreadSpinLock_Unlock (&p->lock);

    return PLANK_TRUE;
}
#endif // PLANK_PPC && PLANK_APPLE


//------------------------------------------------------------------------------

PlankAtomicFRef pl_AtomicF_CreateAndInit()
{
    PlankAtomicFRef p = pl_AtomicF_Create();
    if (p != PLANK_NULL) pl_AtomicF_Init (p);
    return p;
}

PlankAtomicFRef pl_AtomicF_Create()
{
    PlankMemoryRef m;
    PlankAtomicFRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicFRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicF));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicF));

    return p;
}

PlankResult pl_AtomicF_Init (PlankAtomicFRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
//    pl_AtomicF_Set (p, 0.f);
    p->value = 0.f;
    
    return PlankResult_OK;
}

PlankResult pl_AtomicF_DeInit (PlankAtomicFRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicF_Destroy (PlankAtomicFRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    pl_Memory_Free (m, p);
    return PlankResult_OK;
}

PlankI pl_AtomicF_GetExtra (PlankAtomicFRef p)
{
    (void)p;
    return 0;
}

PlankI pl_AtomicF_GetExtraUnchecked (PlankAtomicFRef p)
{
    (void)p;
    return 0;
}

//------------------------------------------------------------------------------

PlankAtomicDRef pl_AtomicD_CreateAndInit()
{
    PlankAtomicDRef p = pl_AtomicD_Create();
    if (p != PLANK_NULL) pl_AtomicD_Init (p);
    return p;
}

PlankAtomicDRef pl_AtomicD_Create()
{
    PlankMemoryRef m;
    PlankAtomicDRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicDRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicD));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicD));
    
    return p;
}

PlankResult pl_AtomicD_Init (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_Init (&p->lock);
#endif    
    
//    pl_AtomicD_Set (p, 0.0);
    p->value = 0.0;
    
exit:
    return result;
}

PlankResult pl_AtomicD_DeInit (PlankAtomicDRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_DeInit (&p->lock);
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicD_Destroy (PlankAtomicDRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if ((result = pl_AtomicD_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
exit:
    return result;
}

PlankLL pl_AtomicD_GetExtra (PlankAtomicDRef p)
{
    (void)p;
    return (PlankLL)0;
}

PlankLL pl_AtomicD_GetExtraUnchecked (PlankAtomicDRef p)
{
    (void)p;
    return (PlankLL)0;
}

//------------------------------------------------------------------------------

PlankAtomicPRef pl_AtomicP_CreateAndInit()
{
    PlankAtomicPRef p = pl_AtomicP_Create();
    if (p != PLANK_NULL) pl_AtomicP_Init (p);
    return p;
}

PlankAtomicPRef pl_AtomicP_Create()
{
    PlankMemoryRef m;
    PlankAtomicPRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicPRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicP));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicP));
    
    return p;
}

PlankResult pl_AtomicP_Init (PlankAtomicPRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
//    pl_AtomicP_Set (p, PLANK_NULL);
    p->ptr = PLANK_NULL;
    
    return PlankResult_OK;
}

PlankResult pl_AtomicP_DeInit (PlankAtomicPRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    return PlankResult_OK;
}

PlankResult pl_AtomicP_Destroy (PlankAtomicPRef p)
{
    PlankMemoryRef m = pl_MemoryGlobal();
    pl_Memory_Free (m, p);
    return PlankResult_OK;
}

PlankL pl_AtomicP_GetExtra (PlankAtomicPRef p)
{
	(void)p;
    return (PlankL)0;
}

PlankL pl_AtomicP_GetExtraUnchecked (PlankAtomicPRef p)
{
	(void)p;
    return (PlankL)0;
}

//------------------------------------------------------------------------------

PlankAtomicPXRef pl_AtomicPX_CreateAndInit()
{
    PlankAtomicPXRef p = pl_AtomicPX_Create();
    if (p != PLANK_NULL) pl_AtomicPX_Init (p);
    return p;
}

PlankAtomicPXRef pl_AtomicPX_Create()
{
    PlankMemoryRef m;
    PlankAtomicPXRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicPXRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicPX));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicPX));
    
    return p;
}

PlankResult pl_AtomicPX_Init (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_Init (&p->lock);
#endif        
    
//    pl_AtomicPX_SetAll (p, (PlankP)0, (PlankL)0);
    pl_MemoryZero (p, sizeof (PlankAtomicPX));
    
exit:
    return result;
}

PlankResult pl_AtomicPX_DeInit (PlankAtomicPXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_DeInit (&p->lock);
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicPX_Destroy (PlankAtomicPXRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();

    if ((result = pl_AtomicPX_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

//------------------------------------------------------------------------------

PlankAtomicLXRef pl_AtomicLX_CreateAndInit()
{
    PlankAtomicLXRef p = pl_AtomicLX_Create();
    if (p != PLANK_NULL) pl_AtomicLX_Init (p);
    return p;
}

PlankAtomicLXRef pl_AtomicLX_Create()
{
    PlankMemoryRef m;
    PlankAtomicLXRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAtomicLXRef)pl_Memory_AllocateBytes (m, sizeof (PlankAtomicLX));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAtomicLX));
    
    return p;
}

PlankResult pl_AtomicLX_Init (PlankAtomicLXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_Init (&p->lock);
#endif        
    
//    pl_AtomicLX_SetAll (p, (PlankL)0, (PlankL)0);
    pl_MemoryZero (p, sizeof (PlankAtomicLX));
    
exit:
    return result;
}

PlankResult pl_AtomicLX_DeInit (PlankAtomicLXRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
#if PLANK_NOATOMIC64BIT
    pl_ThreadSpinLock_DeInit (&p->lock);
#endif
    
exit:
    return result;
}

PlankResult pl_AtomicLX_Destroy (PlankAtomicLXRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if ((result = pl_AtomicLX_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}





