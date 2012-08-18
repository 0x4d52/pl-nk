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


/*
 Optimised Lock-Free FIFO Queue
 Dominique Fober, Yann Orlarey, Stéphane Letz
 2001
 
 Optimized Lock-Free FIFO Queue continued
 Dominique Fober, Yann Orlarey, St ́ephane Letz
 2005
 */

#include "../core/plank_StandardHeader.h"
#include "plank_LockFreeQueue.h"

PlankLockFreeQueueRef pl_LockFreeQueue_CreateAndInit()
{
    PlankLockFreeQueueRef p;
    p = pl_LockFreeQueue_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_LockFreeQueue_Init (p) != PlankResult_OK)
            pl_LockFreeQueue_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankLockFreeQueueRef pl_LockFreeQueue_Create()
{
    PlankMemoryRef m;
    PlankLockFreeQueueRef p;
    
    m = pl_MemoryGlobal(); // OK, creation of the queue isn't itself lock free
    p = (PlankLockFreeQueueRef)pl_Memory_AllocateBytes (m, sizeof (PlankLockFreeQueue));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankLockFreeQueue));
    
    return p;
}

PlankResult pl_LockFreeQueue_Init (PlankLockFreeQueueRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankLockFreeQueue));
    
    pl_AtomicPX_Init (&p->head);
    pl_AtomicPX_Init (&p->tail);
        
    pl_LockFreeQueueElement_Init (&p->dummyElement);
    
    p->head.ptr = &p->dummyElement;
    p->tail.ptr = &p->dummyElement;
    pl_LockFreeQueueElement_SetNext (&p->dummyElement, (PlankLockFreeQueueElementRef)p);
    
exit:
    return result;    
}

PlankResult pl_LockFreeQueue_DeInit (PlankLockFreeQueueRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
        
    if (pl_LockFreeQueue_GetSize (p) != 0)
    {
        result = PlankResult_ContainerNotEmptyOnDeInit;
        goto exit;
    }
    
    pl_LockFreeQueueElement_DeInit (&p->dummyElement);
    
    pl_AtomicPX_DeInit (&p->head);
    pl_AtomicPX_DeInit (&p->tail);
    
exit:
    return result;    
}

PlankResult pl_LockFreeQueue_Destroy (PlankLockFreeQueueRef p)
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

    if ((result = pl_LockFreeQueue_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);   
    
exit:
    return result;    
}

PlankResult pl_LockFreeQueue_Clear (PlankLockFreeQueueRef p)
{
    PlankResult result;
    PlankLockFreeQueueElementRef element;
    PlankP data;
    
    result = PlankResult_OK;
    
    if ((result = pl_LockFreeQueue_Pop (p, &element)) != PlankResult_OK)
        goto exit;
    
    if (p->freeFunction != PLANK_NULL)
    {
        while (element != PLANK_NULL) 
        {
            data = pl_LockFreeQueueElement_GetData (element);
            
            if (data != PLANK_NULL)
            {
                if ((result = (p->freeFunction) (data)) != PlankResult_OK)
                    goto exit;
            }
            
            if ((result = pl_LockFreeQueueElement_Destroy (element)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_LockFreeQueue_Pop (p, &element)) != PlankResult_OK)
                goto exit;
        }
    }
    else
    {
        while (element != PLANK_NULL) 
        {
            if ((result = pl_LockFreeQueueElement_Destroy (element)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_LockFreeQueue_Pop (p, &element)) != PlankResult_OK)
                goto exit;
        }
    }
    
exit:
    return result;    
}

PlankResult pl_LockFreeQueue_SetFreeElementDataFunction (PlankLockFreeQueueRef p, PlankLockFreeQueueFreeElementDataFunction freeFunction)
{
    PlankResult result = PlankResult_OK;    
    p->freeFunction = freeFunction;
    return result;
}

PlankResult pl_LockFreeQueue_Push (PlankLockFreeQueueRef p, const PlankLockFreeQueueElementRef element)
{
    PlankResult result;
    PlankL tailExtra;
    PlankLockFreeQueueElementRef tailElement;
    PlankAtomicPXRef tailElementNextAtom;
    PlankB success;
    
    result = PlankResult_OK;
    success = PLANK_FALSE;
    
	tailExtra = 0;
	tailElement = 0;

    pl_LockFreeQueueElement_SetNext (element, (PlankLockFreeQueueElementRef)p);
    
    while (!success)
    {
        tailExtra = pl_AtomicPX_GetExtra ((PlankAtomicPXRef)&(p->tail));
        tailElement = (PlankLockFreeQueueElementRef)pl_AtomicPX_Get ((PlankAtomicPXRef)&(p->tail));
        
        tailElementNextAtom = pl_LockFreeQueueElement_GetNextAtom (tailElement);
        success = pl_AtomicP_CompareAndSwap ((PlankAtomicPRef)tailElementNextAtom, 
                                             (PlankLockFreeQueueElementRef)p, element);
        
        if (!success)
        {
            pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)&(p->tail), 
                                        tailElement, tailExtra, 
                                        pl_LockFreeQueueElement_GetNext (tailElement), tailExtra + 1);
        }
    }
    
    pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)&(p->tail), 
                                tailElement, tailExtra, 
                                element, tailExtra + 1);    
    
    pl_AtomicLL_Increment (&p->count);
    
    return result;
}

PlankResult pl_LockFreeQueue_Pop (PlankLockFreeQueueRef p, PlankLockFreeQueueElementRef* element)
{
    PlankResult result;
    PlankL headExtra, tailExtra;
	PlankLockFreeQueueElementRef headElement, nextElement;
    PlankB success;
    
    result = PlankResult_OK;
	headElement = 0;
    success = PLANK_FALSE;
    
    while (!success)
    {
        headExtra = pl_AtomicPX_GetExtra ((PlankAtomicPXRef)&(p->head));
        tailExtra = pl_AtomicPX_GetExtra ((PlankAtomicPXRef)&(p->tail));
        headElement = pl_AtomicPX_Get ((PlankAtomicPXRef)&(p->head));
        nextElement = pl_LockFreeQueueElement_GetNext (headElement);
        
        if (headExtra == pl_AtomicPX_GetExtra ((PlankAtomicPXRef)&(p->head)))
        {
            if (headElement == pl_AtomicPX_Get ((PlankAtomicPXRef)&(p->tail)))
            {
                if (nextElement == (PlankLockFreeQueueElementRef)p)
                {
                    *element = (PlankLockFreeQueueElementRef)PLANK_NULL;
                    goto exit;
                }
                
                pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)&(p->tail), 
                                            headElement, tailExtra, 
                                            nextElement, tailExtra + 1);
            }
            else if (nextElement != (PlankLockFreeQueueElementRef)p)
            {                
                success = pl_AtomicPX_CompareAndSwap ((PlankAtomicPXRef)&(p->head), 
                                                      headElement, headExtra, 
                                                      nextElement, headExtra + 1);
            }
        }
    }
    
    pl_AtomicLL_Decrement (&p->count);
    
    if (headElement == &p->dummyElement)
    {
        if ((result = pl_LockFreeQueue_Push (p, headElement)) != PlankResult_OK)
            goto exit;
        
        if ((result = pl_LockFreeQueue_Pop (p, &headElement)) != PlankResult_OK)
            goto exit;
    }
    
    *element = headElement;
    
exit:
    return result;    
}

PlankLL pl_LockFreeQueue_GetSize (PlankLockFreeQueueRef p)
{
    return pl_AtomicLL_Get (&p->count);
}