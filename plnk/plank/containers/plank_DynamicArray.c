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
#include "plank_DynamicArray.h"
#include "../maths/plank_Maths.h"

PlankDynamicArrayRef pl_DynamicArray_CreateAndInit()
{
    PlankDynamicArrayRef p;
    p = pl_DynamicArray_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_DynamicArray_Init (p) != PlankResult_OK)
            pl_DynamicArray_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankDynamicArrayRef pl_DynamicArray_Create()
{
    PlankMemoryRef m;
    PlankDynamicArrayRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankDynamicArrayRef)pl_Memory_AllocateBytes (m, sizeof (PlankDynamicArray));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankDynamicArray));
    
    return p;    
}

PlankResult pl_DynamicArray_Init (PlankDynamicArrayRef p)
{    
    pl_MemoryZero (p, sizeof (PlankDynamicArray));
    return PlankResult_OK;
}

PlankResult pl_DynamicArray_InitCopy (PlankDynamicArrayRef p, PlankDynamicArrayRef original)
{
    PlankMemoryRef m;
    
    pl_MemoryCopy (p, original, sizeof (PlankDynamicArray));
    
    m = pl_MemoryGlobal();
    
    if (!p->data || !p->itemSize || !p->allocatedItems)
    {
        p->data = PLANK_NULL;
        return PlankResult_OK;
    }
    
    p->data = pl_Memory_AllocateBytes (m, p->itemSize * p->allocatedItems);
    
    if (!p->data)
    {
        pl_MemoryZero (p, sizeof (PlankDynamicArray));
        return PlankResult_MemoryError;
    }
    
    pl_MemoryCopy (p->data, original->data, p->itemSize * p->usedItems);

    return PlankResult_OK;
}

PlankResult pl_DynamicArray_InitWithItemSize (PlankDynamicArrayRef p, const PlankL itemSize)
{
    return pl_DynamicArray_InitWithItemSizeAndCapacity (p, itemSize, PLANKDYNAMICARRAY_DEFAULTGRANULARITY);
}

PlankResult pl_DynamicArray_InitWithItemSizeAndCapacity (PlankDynamicArrayRef p, const PlankL itemSize, const PlankL initialCapacity)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m;

    pl_DynamicArray_Init (p);

    p->itemSize = itemSize;
    p->allocatedItems = initialCapacity;
    p->usedItems = 0;
    
    m = pl_MemoryGlobal();
    p->data = pl_Memory_AllocateBytes (m, p->itemSize * p->allocatedItems);
    
    if (p->data == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
exit:
    return result;    
}

PlankResult pl_DynamicArray_InitWithItemSizeAndSize (PlankDynamicArrayRef p, const PlankL itemSize, const PlankL initialCapacity, PlankB zero)
{
    PlankResult result;
    
    if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (p, itemSize, initialCapacity)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_SetSize (p, initialCapacity)) != PlankResult_OK) goto exit;

    if (zero)
        result = pl_DynamicArray_Zero (p);
    
exit:
    return result;
}


PlankResult pl_DynamicArray_DeInit (PlankDynamicArrayRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if (p->data)
    {
        if ((result = pl_Memory_Free (m, p->data)) != PlankResult_OK)
            goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankDynamicArray));
    
exit:
    return result;
}

PlankResult pl_DynamicArray_Destroy (PlankDynamicArrayRef p)
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
    
    if ((result = pl_DynamicArray_DeInit (p)) != PlankResult_OK)
        goto exit;
        
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

PlankB pl_DynamicArray_IsIndexInRange (PlankDynamicArrayRef p, const PlankL index)
{
    return (index >= 0) && (index < p->usedItems);
}

PlankL pl_DynamicArray_GetSize (PlankDynamicArrayRef p)
{
    return p->usedItems;
}

PlankL pl_DynamicArray_GetItemSize (PlankDynamicArrayRef p)
{
    return p->itemSize;
}

PlankResult pl_DynamicArray_SetSize (PlankDynamicArrayRef p, const PlankL capacity)
{
    PlankResult result;
    
    if ((result = pl_DynamicArray_EnsureSize (p, capacity)) != PlankResult_OK) goto exit;
    
    p->usedItems = capacity;
    
exit:
    return result;
}

PlankResult pl_DynamicArray_Zero (PlankDynamicArrayRef p)
{
    return pl_MemoryZero (p->data, p->itemSize * p->allocatedItems);
}

PlankP pl_DynamicArray_GetArray (PlankDynamicArrayRef p)
{
    return p->data;
}

PlankResult pl_DynamicArray_AddItem (PlankDynamicArrayRef p, const PlankP item)
{
    PlankResult result = PlankResult_OK;
    PlankL index;
    
    index = p->usedItems;
    p->usedItems++;
    
    if (p->usedItems > p->allocatedItems)
    {        
        if ((result = pl_DynamicArray_Grow (p, PLANKDYNAMICARRAY_DEFAULTGRANULARITY)) != PlankResult_OK) goto exit;
    }
    
    pl_MemoryCopy ((unsigned char*)p->data + index * p->itemSize, item, p->itemSize);
    
exit:
    return result;
}

static PLANK_INLINE_LOW void pl_DynamicArraySwapItems (PlankP itemA, PlankP itemB, PlankP temp, const PlankL itemSize)
{
    pl_MemoryCopy (temp,  itemA, itemSize);
    pl_MemoryCopy (itemA, itemB, itemSize);
    pl_MemoryCopy (itemB, temp,  itemSize);
}

#ifndef PLANKDYNAMICARRAY_SORTTEMPSIZE
#define PLANKDYNAMICARRAY_SORTTEMPSIZE 64
#endif

PlankResult pl_DynamicArray_Sort (PlankDynamicArrayRef p, PlankDynamicArrayCompareFunction comparator)
{
    // yikes - a bubble sort - must improve this...
    PLANK_ALIGN(16) PlankUC tempData[PLANKDYNAMICARRAY_SORTTEMPSIZE];
    PlankResult result = PlankResult_OK;
    PlankL i, j, itemSize, end;
    PlankB swapped;
    PlankP itemA, itemB, temp;
    PlankUC* data;

    if (comparator == PLANK_NULL)
    {
        result = PlankResult_FunctionsInvalid;
        goto exit;
    }
    
    itemSize = p->itemSize;

    if (itemSize <= PLANKDYNAMICARRAY_SORTTEMPSIZE)
    {
        data = (PlankUC*)p->data;
        temp = (PlankP)tempData;
    }
    else 
    {
		if (p->usedItems == p->allocatedItems)
		{
			// we need space for the temp location
			if ((result = pl_DynamicArray_Grow (p, PLANKDYNAMICARRAY_DEFAULTGRANULARITY)) != PlankResult_OK) goto exit;
		}

        data = (PlankUC*)p->data;
        temp = data + p->usedItems * itemSize;
    }

        
    for (i = p->usedItems; --i >= 0;)
    {
        swapped = PLANK_FALSE;
        end = i * itemSize;
        
        for (j = 0; j < end; j += itemSize)
        {
            itemA = data + j;
            itemB = (PlankUC*)itemA + itemSize;
            
            if (comparator (itemA, itemB))
            {
                pl_DynamicArraySwapItems (itemA, itemB, temp, itemSize);
                swapped = PLANK_TRUE;
            }
        }
        
        if (!swapped)
            goto exit;
    }
    
exit:
    return result;
}

static PLANK_INLINE_LOW PlankResult pl_DynamicArray_AddItemsInternal (PlankDynamicArrayRef p, PlankConstantP items, const PlankL numItems, const PlankB isText)
{
    PlankResult result = PlankResult_OK;
    PlankL index;
    
    index = isText ? p->usedItems - 1 : p->usedItems;
    p->usedItems += isText ? numItems - 1 : numItems;
    
    if (p->usedItems > p->allocatedItems)
    {
        result = pl_DynamicArray_EnsureSize (p, p->usedItems + PLANKDYNAMICARRAY_DEFAULTGRANULARITY);
        
        if (result != PlankResult_OK)
            goto exit;
    }
    
    pl_MemoryCopy ((unsigned char*)p->data + index * p->itemSize, items, p->itemSize * numItems);
    
exit:
    return result;
}


PlankResult pl_DynamicArray_AddItems (PlankDynamicArrayRef p, PlankConstantP items, const PlankL numItems)
{
    return pl_DynamicArray_AddItemsInternal (p, items, numItems, PLANK_FALSE);
}

PlankResult pl_DynamicArray_SetItem (PlankDynamicArrayRef p, const PlankL index, const PlankP item)
{
    PlankResult result = PlankResult_OK;
    
    if ((index < 0) || (index >= p->usedItems))
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }
    
    pl_MemoryCopy ((unsigned char*)p->data + index * p->itemSize, item, p->itemSize);
    
exit:
    return result;        
}

PlankResult pl_DynamicArray_SetAsClearText (PlankDynamicArrayRef p, const PlankL length)
{
    PlankResult result = PlankResult_OK;
    
    if (p->itemSize == 0)
        pl_DynamicArray_InitWithItemSize (p, 1);
    
    if (p->itemSize != 1)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_SetSize (p, length + 1)) != PlankResult_OK) goto exit;
    
    pl_MemoryZero (pl_DynamicArray_GetArray (p), length + 1);
    
exit:
    return result;
}

PlankResult pl_DynamicArray_SetAsText (PlankDynamicArrayRef p, const char* text)
{
    PlankResult result = PlankResult_OK;
    PlankL length;
    
    if (!text)
        return pl_DynamicArray_SetAsClearText (p, 0);
    
    if (text[0] == '\0')
        return pl_DynamicArray_SetAsClearText (p, 0);
    
    if (p->itemSize == 0)
        pl_DynamicArray_InitWithItemSize (p, 1);
    
    if (p->itemSize != 1)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }

    length = strlen (text);

    if ((result = pl_DynamicArray_SetSize (p, length + 1)) != PlankResult_OK) goto exit;
    
    pl_MemoryCopy (pl_DynamicArray_GetArray (p), text, length + 1);
    
exit:
    return result;
}

PlankResult pl_DynamicArray_AppendText (PlankDynamicArrayRef p, const char* text)
{
    PlankResult result = PlankResult_OK;
    PlankL length;
    
    if (!text)
        goto exit;
    
    if (text[0] == '\0')
        goto exit;
    
    if (p->itemSize == 0)
        pl_DynamicArray_InitWithItemSize (p, 1);
    
    if (p->itemSize != 1)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    length = strlen (text);
    
    if ((result = pl_DynamicArray_AddItemsInternal (p, text, length + 1, PLANK_TRUE)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_DynamicArray_AppendTextLine (PlankDynamicArrayRef p, const char* text, const char* seps)
{
    PlankResult result = PlankResult_OK;
    
    if (p->usedItems == 0)
        return pl_DynamicArray_SetAsText (p, text);
    
    if ((result = pl_DynamicArray_AppendText (p, seps)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_AppendText (p, text)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_DynamicArray_AppendTextLineLF (PlankDynamicArrayRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLine (p, text, "\n");
}

PlankResult pl_DynamicArray_AppendTextLineCRLF (PlankDynamicArrayRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLine (p, text, "\r\n");
}

PlankResult pl_DynamicArray_InsertItem (PlankDynamicArrayRef p, const PlankL index, const PlankP item)
{
    PlankResult result = PlankResult_OK;
    PlankP src, dst;
    PlankL size;
    
    if (index < 0)
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_EnsureSize (p, p->usedItems + 1)) != PlankResult_OK) goto exit;
    
    if (index < p->usedItems)
    {
        src = (unsigned char*)p->data + index * p->itemSize;
        dst = (unsigned char*)p->data + (index + 1) * p->itemSize;
        size = p->itemSize * (p->usedItems - index);
        pl_MemoryCopy (dst, src, size);
    
        dst = src;
        src = item;
        size = p->itemSize;
        pl_MemoryCopy (dst, src, size);
    }
    else
    {
        dst = (unsigned char*)p->data + p->usedItems * p->itemSize;
        src = item;
        size = p->itemSize;
        pl_MemoryCopy (dst, src, size);        
    }
    
    p->usedItems++;
    
exit:
    return result;
}

PlankResult pl_DynamicArray_GetItem (PlankDynamicArrayRef p, const PlankL index, PlankP item)
{
    PlankResult result = PlankResult_OK;

    if ((index < 0) || (index >= p->usedItems))
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }
    
    pl_MemoryCopy (item, (unsigned char*)p->data + index * p->itemSize, p->itemSize);
    
exit:
    return result;    
}

PlankResult pl_DynamicArray_PutItem (PlankDynamicArrayRef p, const PlankL index, PlankP item)
{
    PlankResult result = PlankResult_OK;
    
    if ((index < 0) || (index >= p->usedItems))
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }
        
    pl_MemoryCopy ((unsigned char*)p->data + index * p->itemSize, item, p->itemSize);
        
exit:
    return result;

}

PlankResult pl_DynamicArray_RemoveItem (PlankDynamicArrayRef p, const PlankL index)
{
    PlankResult result = PlankResult_OK;
    PlankP src, dst;
    PlankL size;
    
    if ((index < 0) || (index >= p->usedItems))
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }
    
    p->usedItems--;
    
    if (p->usedItems > 0)
    {
        dst = (unsigned char*)p->data + index * p->itemSize;
        src = (unsigned char*)p->data + (index + 1) * p->itemSize;
        size = p->itemSize * (p->usedItems - index);
        pl_MemoryCopy (dst, src, size);
    }
    
exit:
    return result;            
}

PlankResult pl_DynamicArray_EnsureSize (PlankDynamicArrayRef p, const PlankL capacity)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m;
    PlankL newAllocatedItems;
    PlankP newData;
    
    if (capacity <= p->allocatedItems)
        goto exit;
    
    m = pl_MemoryGlobal();
    
    newAllocatedItems = pl_MaxL (capacity, p->allocatedItems + PLANKDYNAMICARRAY_DEFAULTGRANULARITY);
    newData = pl_Memory_AllocateBytes (m, p->itemSize * newAllocatedItems);
    
    if (newData == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryCopy (newData, p->data, p->itemSize * p->allocatedItems);
    
    pl_Memory_Free (m, p->data);
    p->data = newData;
    p->allocatedItems = newAllocatedItems;
    
exit:
    return result;    
}

PlankResult pl_DynamicArray_Grow (PlankDynamicArrayRef p, const int amount)
{
    PlankResult result = PlankResult_OK;

    if (amount < 1)
    {
        result = PlankResult_ArrayParameterError;
        goto exit;
    }

    result = pl_DynamicArray_EnsureSize (p, p->allocatedItems + amount);
    
exit:
    return result;
}

PlankResult pl_DynamicArray_Purge (PlankDynamicArrayRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m;
    PlankL newAllocatedItems;
    PlankP newData;
    
    if (p->usedItems == p->allocatedItems)
        goto exit;
    
    m = pl_MemoryGlobal();
    
    newAllocatedItems = p->usedItems;
    newData = pl_Memory_AllocateBytes (m, p->itemSize * newAllocatedItems);
    
    if (newData == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryCopy (newData, p->data, p->itemSize * p->usedItems);
    
    pl_Memory_Free (m, p->data);
    p->data = newData;
    p->allocatedItems = newAllocatedItems;
    
exit:
    return result;    
}


