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
#include "plank_SimpleStack.h"

PlankSimpleStackRef pl_SimpleStack_CreateAndInit()
{
    PlankSimpleStackRef p;
    p = pl_SimpleStack_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_SimpleStack_Init (p) != PlankResult_OK)
            pl_SimpleStack_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankSimpleStackRef pl_SimpleStack_Create()
{
    PlankMemoryRef m;
    PlankSimpleStackRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankSimpleStackRef)pl_Memory_AllocateBytes (m, sizeof (PlankSimpleStack));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankSimpleStack));
    
    return p;
}

PlankResult pl_SimpleStack_Init (PlankSimpleStackRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankSimpleStack));
        
exit:
    return result;
}

PlankResult pl_SimpleStack_DeInit (PlankSimpleStackRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if (pl_SimpleStack_GetSize (p) != 0)
        result = PlankResult_ContainerNotEmptyOnDeInit;
    
exit:
    return result;    
}

PlankResult pl_SimpleStack_Destroy (PlankSimpleStackRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_SimpleStack_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

PlankResult pl_SimpleStack_Clear (PlankSimpleStackRef p)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleStackElementRef element;
    PlankP data;
    
    if ((result = pl_SimpleStack_Pop (p, &element)) != PlankResult_OK)
        goto exit;
    
    if (p->freeFunction != PLANK_NULL)
    {
        while (element != PLANK_NULL) 
        {
            data = pl_SimpleStackElement_GetData (element);
            
            if (data != PLANK_NULL)
            {
                if ((result = (p->freeFunction) (data)) != PlankResult_OK)
                    goto exit;
            }
            
            if ((result = pl_SimpleStackElement_Destroy (element)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_SimpleStack_Pop (p, &element)) != PlankResult_OK)
                goto exit;
        }
    }
    else
    {
        while (element != PLANK_NULL) 
        {
            if ((result = pl_SimpleStackElement_Destroy (element)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_SimpleStack_Pop (p, &element)) != PlankResult_OK)
                goto exit;
        }
    }
    
exit:
    return result;    
}

PlankResult pl_SimpleStack_SetFreeElementDataFunction (PlankSimpleStackRef p, 
                                                       PlankSimpleStackFreeElementDataFunction freeFunction)
{
    PlankResult result = PlankResult_OK;    
    p->freeFunction = freeFunction;
    return result;
}

PlankResult pl_SimpleStack_Push (PlankSimpleStackRef p, const PlankSimpleStackElementRef element)
{
    PlankResult result = PlankResult_OK;
    
    pl_SimpleStackElement_SetNext (element, p->head);
    p->head = element;
    p->count++;
    
    return result;
}

PlankResult pl_SimpleStack_Pop (PlankSimpleStackRef p, PlankSimpleStackElementRef* element)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleStackElementRef headElement, nextElement;
    
    headElement = p->head;
    
    if (headElement != PLANK_NULL)
    {
        nextElement = pl_SimpleStackElement_GetNext (headElement);
        p->head = nextElement;
        p->count--;
    }
    
    *element = headElement;
    
    return result;    
}

PlankLL pl_SimpleStack_GetSize (PlankSimpleStackRef p)
{
    return p->count;
}

