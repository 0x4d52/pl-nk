/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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
#include "plank_SimpleLinkedList.h"


PlankSimpleLinkedListRef pl_SimpleLinkedList_CreateAndInit()
{
    PlankSimpleLinkedListRef p;
    p = pl_SimpleLinkedList_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_SimpleLinkedList_Init (p) != PlankResult_OK)
            pl_SimpleLinkedList_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankSimpleLinkedListRef pl_SimpleLinkedList_Create()
{
    PlankMemoryRef m;
    PlankSimpleLinkedListRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankSimpleLinkedListRef)pl_Memory_AllocateBytes (m, sizeof (PlankSimpleLinkedList));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankSimpleLinkedList));
    
    return p;
}

PlankResult pl_SimpleLinkedList_Init (PlankSimpleLinkedListRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankSimpleLinkedList));
            
exit:
    return result;
}

PlankResult pl_SimpleLinkedList_DeInit (PlankSimpleLinkedListRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
     
    if (pl_SimpleLinkedList_GetSize (p) != 0)
        result = PlankResult_ContainerNotEmptyOnDeInit;
    
    pl_MemoryZero (p, sizeof (PlankSimpleLinkedList));

exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_Destroy (PlankSimpleLinkedListRef p)
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
    
    if ((result = pl_SimpleLinkedList_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_Clear (PlankSimpleLinkedListRef p)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef element;
    PlankP data;
    
    result = PlankResult_OK;
    
    if ((result = pl_SimpleLinkedList_RemoveFirst (p, &element)) != PlankResult_OK)
        goto exit;
    
    if (p->freeFunction != PLANK_NULL)
    {
        while (element != PLANK_NULL) 
        {
            data = pl_SimpleLinkedListElement_GetData (element);
            
            if (data != PLANK_NULL)
            {
                if ((result = (p->freeFunction) (data)) != PlankResult_OK)
                    goto exit;
            }
            
            if ((result = pl_SimpleLinkedListElement_Destroy (element)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_SimpleLinkedList_RemoveFirst (p, &element)) != PlankResult_OK)
                goto exit;
        }
    }
    else
    {
        while (element != PLANK_NULL) 
        {
            if ((result = pl_SimpleLinkedListElement_Destroy (element)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_SimpleLinkedList_RemoveFirst (p, &element)) != PlankResult_OK)
                goto exit;
        }
    }
    
exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_SetFreeElementDataFunction (PlankSimpleLinkedListRef p, 
                                                            PlankSimpleLinkedListFreeElementDataFunction freeFunction)
{
    PlankResult result = PlankResult_OK;
    p->freeFunction = freeFunction;
    return result;
}

PlankResult pl_SimpleLinkedList_Add (PlankSimpleLinkedListRef p, const PlankSimpleLinkedListElementRef element)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef last;

    result = PlankResult_OK;
    
    if (p->first == PLANK_NULL)
    {
        p->first = element;
    }
    else
    {
        pl_SimpleLinkedList_GetLast (p, &last);
        pl_SimpleLinkedListElement_SetNext (last, element);
    }

    return result;    
}

PlankResult pl_SimpleLinkedList_InsertAtIndex (PlankSimpleLinkedListRef p, const PlankLL index, const PlankSimpleLinkedListElementRef element)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef prev, curr, next;
    PlankLL count;
    
    result = PlankResult_OK;
    count = 0;
    
    if (index == 0)
    {
        pl_SimpleLinkedListElement_SetNext (element, p->first);
        p->first = element;
        goto exit;
    }
    else
    {
        prev = PLANK_NULL;
        curr = p->first;
        next = pl_SimpleLinkedListElement_GetNext (curr);
        count++;
        
        while (next != PLANK_NULL)
        {            
            prev = curr;
            curr = next;
            next = pl_SimpleLinkedListElement_GetNext (curr);
            
            if (count == index)
            {
                pl_SimpleLinkedListElement_SetNext (prev, element);
                pl_SimpleLinkedListElement_SetNext (element, curr);
                goto exit;
            }
            
            count++;
        }
        
        // add to end
        pl_SimpleLinkedListElement_SetNext (curr, element);
        pl_SimpleLinkedListElement_SetNext (element, PLANK_NULL);
    }
        
exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_AtIndex (PlankSimpleLinkedListRef p, const PlankLL index, PlankSimpleLinkedListElementRef* element)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef prev, curr, next;
    PlankLL count;
    
    result = PlankResult_OK;
    count = 0;
    
    if (p->first == PLANK_NULL)
    {
        *element = PLANK_NULL;
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    else if (index == 0)
    {
        *element = p->first;
        goto exit;
    }
    else
    {
        prev = PLANK_NULL;
        curr = p->first;
        next = pl_SimpleLinkedListElement_GetNext (curr);
        count++;
        
        while (next != PLANK_NULL)
        {            
            prev = curr;
            curr = next;
            next = pl_SimpleLinkedListElement_GetNext (curr);
            
            if (count == index)
            {
                *element = curr;
                goto exit;
            }
            
            count++;
        }
    }
        
    *element = PLANK_NULL;
    result = PlankResult_IndexOutOfRange;
    
exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_GetFirst (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element)
{
    PlankResult result = PlankResult_OK;
    *element = p->first;
    return result;    
}

PlankResult pl_SimpleLinkedList_GetLast (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef curr, next;
    
    result = PlankResult_OK;
    curr = p->first;
    
    if (curr == PLANK_NULL)
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
        
    next = pl_SimpleLinkedListElement_GetNext (curr);
    
    while (next != PLANK_NULL)
    {
        curr = next;
        next = pl_SimpleLinkedListElement_GetNext (curr);
    }
    
    *element = curr;
    
exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_RemoveAtIndex (PlankSimpleLinkedListRef p, const PlankLL index, PlankSimpleLinkedListElementRef* element)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef prev, curr, next;
    PlankLL count;
    
    result = PlankResult_OK;
    count = 0;
    
    if (index == 0)
    {
        return pl_SimpleLinkedList_RemoveFirst (p, element);
    }
    else if (p->first != PLANK_NULL)
    {
        prev = PLANK_NULL;
        curr = p->first;
        next = pl_SimpleLinkedListElement_GetNext (curr);
        count++;
        
        while (next != PLANK_NULL)
        {            
            prev = curr;
            curr = next;
            next = pl_SimpleLinkedListElement_GetNext (curr);
            
            if (count == index)
            {
                pl_SimpleLinkedListElement_SetNext (prev, next);
                *element = curr;
                goto exit;
            }
            
            count++;
        }
    }
    
    *element = PLANK_NULL;
    result = PlankResult_IndexOutOfRange;
    
exit:
    return result;    
}

PlankResult pl_SimpleLinkedList_RemoveFirst (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element)
{
    PlankResult result = PlankResult_OK;
        
    *element = p->first;
    
    if (p->first != PLANK_NULL)
        p->first = pl_SimpleLinkedListElement_GetNext (p->first);
    
    return result;    
}

PlankResult pl_SimpleLinkedList_RemoveLast (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef prev, curr, next;
    
    result = PlankResult_OK;
    
    if (p->first == PLANK_NULL)
    {
        *element = PLANK_NULL;
    }
    else
    {
        prev = PLANK_NULL;
        curr = p->first;
        next = pl_SimpleLinkedListElement_GetNext (curr);
        
        while (next != PLANK_NULL)
        {
            prev = curr;
            curr = next;
            next = pl_SimpleLinkedListElement_GetNext (curr);
        }
        
        if (prev == PLANK_NULL)
            p->first = PLANK_NULL;
        else
            pl_SimpleLinkedListElement_SetNext (prev, PLANK_NULL);
        
        *element = curr;
    }

    return result;    
}

PlankResult pl_SimpleLinkedList_IndexOf (PlankSimpleLinkedListRef p, const PlankSimpleLinkedListElementRef element, PlankLL* index)
{
    PlankResult result;
    PlankSimpleLinkedListElementRef curr, next;
    PlankLL count;
    
    result = PlankResult_OK;
    curr = p->first;
    count = 0;
    
    if (curr != PLANK_NULL)
    {
        if (curr == element)
        {
            *index = count;
            goto exit;
        }
        
        next = pl_SimpleLinkedListElement_GetNext (curr);
        count++;
        
        while (next != PLANK_NULL)
        {
            curr = next;
            
            if (curr == element)
            {
                *index = count;
                goto exit;
            }
            
            next = pl_SimpleLinkedListElement_GetNext (curr);
            count++;
        }
    }
    
    *index = -1;
    
exit:
    return result;    
}


PlankLL pl_SimpleLinkedList_GetSize (PlankSimpleLinkedListRef p)
{
    PlankSimpleLinkedListElementRef curr, next;
    PlankLL count;
    
    curr = p->first;
    count = 0;

    if (curr != PLANK_NULL)
    {
        next = pl_SimpleLinkedListElement_GetNext (curr);
        count++;

        while (next != PLANK_NULL)
        {
            curr = next;
            next = pl_SimpleLinkedListElement_GetNext (curr);
            count++;
        }
    }
    
    return count;
}


