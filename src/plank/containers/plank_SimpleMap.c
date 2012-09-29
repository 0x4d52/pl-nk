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
#include "plank_SimpleMap.h"


#if !DOXYGEN
typedef struct PlankSimpleMapElement* PlankSimpleMapElementRef; 
typedef struct PlankSimpleMapElement
{
    PlankLL key;
    PlankP ptr;
} PlankSimpleMapElement;
#endif

// private functions
PlankSimpleMapElementRef pl_SimpleMapElement_CreateWithData (const PlankLL key, PlankP ptr);
PlankResult pl_SimpleMapElement_Destroy (PlankSimpleMapElementRef p);

PlankSimpleMapElementRef pl_SimpleMapElement_CreateWithData (const PlankLL key, PlankP ptr)
{
    PlankSimpleMapElementRef p;
    PlankMemoryRef m;

    m = pl_MemoryGlobal();
    p = (PlankSimpleMapElementRef)pl_Memory_AllocateBytes (m, sizeof (PlankSimpleMapElement));
    
    if (p != PLANK_NULL)
    {
        p->key = key;
        p->ptr = ptr;
    }
    
    return p;
}

PlankResult pl_SimpleMapElement_Destroy (PlankSimpleMapElementRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

//------------------------------------------------------------------------------

PlankSimpleMapRef pl_SimpleMap_CreateAndInit()
{
    PlankSimpleMapRef p;
    p = pl_SimpleMap_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_SimpleMap_Init (p) != PlankResult_OK)
            pl_SimpleMap_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankSimpleMapRef pl_SimpleMap_Create()
{
    PlankMemoryRef m;
    PlankSimpleMapRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankSimpleMapRef)pl_Memory_AllocateBytes (m, sizeof (PlankSimpleMap));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankSimpleMap));
    
    return p;
}

PlankResult pl_SimpleMap_Init (PlankSimpleMapRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankSimpleMap));
    result = pl_SimpleLinkedList_Init (&p->list);
            
exit:
    return result;
}

PlankResult pl_SimpleMap_DeInit (PlankSimpleMapRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
        
    if (pl_SimpleMap_GetSize (p) != 0)
        result = PlankResult_ContainerNotEmptyOnDeInit;
    
    if ((result = pl_SimpleLinkedList_DeInit (&p->list)) != PlankResult_OK)
        goto exit;
            
    pl_MemoryZero (p, sizeof (PlankSimpleMap));

exit:
    return result;    
}

PlankResult pl_SimpleMap_Destroy (PlankSimpleMapRef p)
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
    
    if ((result = pl_SimpleMap_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

PlankResult pl_SimpleMap_Clear (PlankSimpleMapRef p)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleLinkedListElementRef listElement;
    PlankSimpleMapElementRef mapElement;
    
    if ((result = pl_SimpleLinkedList_RemoveFirst (&p->list, &listElement)) != PlankResult_OK)
        goto exit;
    
    if (p->freeFunction != PLANK_NULL)
    {
        while (listElement != PLANK_NULL) 
        {
            mapElement = pl_SimpleLinkedListElement_GetData (listElement);
            
            if (mapElement != PLANK_NULL)
            {
                if (mapElement->ptr != PLANK_NULL)
                    if ((result = (p->freeFunction) (mapElement->ptr)) != PlankResult_OK)
                        goto exit;

                if ((result = pl_SimpleMapElement_Destroy (mapElement)) != PlankResult_OK)
                    goto exit;
            }
            
            if ((result = pl_SimpleLinkedListElement_Destroy (listElement)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_SimpleLinkedList_RemoveFirst (&p->list, &listElement)) != PlankResult_OK)
                goto exit;
        }
    }
    else
    {
        while (listElement != PLANK_NULL) 
        {
            mapElement = pl_SimpleLinkedListElement_GetData (listElement);
            
            if (mapElement != PLANK_NULL)
            {                
                if ((result = pl_SimpleMapElement_Destroy (mapElement)) != PlankResult_OK)
                    goto exit;
            }
            
            if ((result = pl_SimpleLinkedListElement_Destroy (listElement)) != PlankResult_OK)
                goto exit;
            
            if ((result = pl_SimpleLinkedList_RemoveFirst (&p->list, &listElement)) != PlankResult_OK)
                goto exit;
        }
    }

    
exit:
    return result;    
}

PlankResult pl_SimpleMap_SetFreeElementDataFunction (PlankSimpleMapRef p, 
                                                     PlankSimpleMapFreeElementDataFunction freeFunction)
{
    PlankResult result = PlankResult_OK;
    p->freeFunction = freeFunction;
    return result;
}

PlankResult pl_SimpleMap_ContainsKey (PlankSimpleMapRef p, const PlankLL key, PlankB* flag)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleLinkedListElementRef listElement;
    PlankSimpleMapElementRef mapElement;
    
    *flag = PLANK_FALSE;
    
    if ((result = pl_SimpleLinkedList_GetFirst (&p->list, &listElement)) != PlankResult_OK)
        goto exit;
    
    while (listElement != PLANK_NULL) 
    {
        mapElement = pl_SimpleLinkedListElement_GetData (listElement);
        
        if (mapElement != PLANK_NULL)
        {
            if (mapElement->key == key)
            {
                *flag = PLANK_TRUE;
                goto exit;
            }
        }
        
        listElement = pl_SimpleLinkedListElement_GetNext (listElement);
    }
    
exit:
    return result;
}

PlankResult pl_SimpleMap_SetKey (PlankSimpleMapRef p, const PlankLL key, PlankP data)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleLinkedListElementRef listElement;
    PlankSimpleMapElementRef mapElement;
    PlankB containsKey;
    
    if ((result = pl_SimpleMap_ContainsKey (p, key, &containsKey)) != PlankResult_OK)
        goto exit;
    
    if (containsKey)
    {
        if ((result = pl_SimpleLinkedList_GetFirst (&p->list, &listElement)) != PlankResult_OK)
            goto exit;
        
        while (listElement != PLANK_NULL) 
        {
            mapElement = pl_SimpleLinkedListElement_GetData (listElement);
            
            if (mapElement != PLANK_NULL)
            {                
                if (mapElement->key == key)
                {
                    if ((mapElement->ptr != PLANK_NULL) &&
                        (mapElement->ptr != data) &&
                        (p->freeFunction != PLANK_NULL))
                        if ((result = (p->freeFunction) (mapElement->ptr)) != PlankResult_OK)
                            goto exit;
                    
                    mapElement->ptr = data;
                    goto exit;
                }
            }
            
            listElement = pl_SimpleLinkedListElement_GetNext (listElement);
        }
        
        result = PlankResult_UnknownError;
    }
    else
    {
        mapElement = pl_SimpleMapElement_CreateWithData (key, data);
        listElement = pl_SimpleLinkedListElement_CreateAndInit();
        pl_SimpleLinkedListElement_SetData (listElement, mapElement);
        pl_SimpleLinkedList_InsertAtIndex (&p->list, 0, listElement);
    }

exit:
    return result;
}

PlankResult pl_SimpleMap_GetKey (PlankSimpleMapRef p, const PlankLL key, PlankP* data)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleLinkedListElementRef listElement;
    PlankSimpleMapElementRef mapElement;
    
    *data = PLANK_NULL;
    
    if ((result = pl_SimpleLinkedList_GetFirst (&p->list, &listElement)) != PlankResult_OK)
        goto exit;
    
    while (listElement != PLANK_NULL) 
    {
        mapElement = pl_SimpleLinkedListElement_GetData (listElement);
        
        if (mapElement != PLANK_NULL)
        {
            if (mapElement->key == key)
            {
                *data = mapElement->ptr;
                goto exit;
            }
        }
        
        listElement = pl_SimpleLinkedListElement_GetNext (listElement);
    }
    
exit:
    return result;
}

PlankResult pl_SimpleMap_RemoveKey (PlankSimpleMapRef p, const PlankLL key, PlankP* data)
{
    PlankResult result = PlankResult_OK;
    PlankSimpleLinkedListElementRef listElement;
    PlankSimpleMapElementRef mapElement;
    PlankLL index;
    
    index = 0;
    *data = PLANK_NULL;
    
    if ((result = pl_SimpleLinkedList_GetFirst (&p->list, &listElement)) != PlankResult_OK)
        goto exit;
    
    while (listElement != PLANK_NULL) 
    {
        mapElement = pl_SimpleLinkedListElement_GetData (listElement);
        
        if (mapElement != PLANK_NULL)
        {
            if (mapElement->key == key)
            {
                result = pl_SimpleLinkedList_RemoveAtIndex (&p->list, index, &listElement);
                goto exit;
            }
        }
        
        listElement = pl_SimpleLinkedListElement_GetNext (listElement);        
        index++;
    }
    
exit:
    return result;
}

PlankLL pl_SimpleMap_GetSize (PlankSimpleMapRef p)
{
    return pl_SimpleLinkedList_GetSize (&p->list);
}


