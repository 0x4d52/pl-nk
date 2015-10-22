/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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
#include "plank_LockFreeDynamicArray.h"
#include "atomic/plank_Atomic.h"
#include "../maths/plank_Maths.h"

// unlikely to implement now... 

///*
// Steps to make this lock-free...
// 
// - change the usedItems and data to atomic ABA preventing types
// - change algos here to check for changes before committing
// - implmement a lock-free memory alloc!!!
// */
//
//PlankLockFreeDynamicArrayRef pl_LockFreeDynamicArray_CreateAndInit()
//{
//    PlankLockFreeDynamicArrayRef p;
//    p = pl_LockFreeDynamicArray_Create();
//    
//    if (p != PLANK_NULL)
//    {
//        if (pl_LockFreeDynamicArray_Init (p) != PlankResult_OK)
//            pl_LockFreeDynamicArray_Destroy (p);
//        else
//            return p;
//    }
//    
//    return PLANK_NULL;
//}
//
//PlankLockFreeDynamicArrayRef pl_LockFreeDynamicArray_Create()
//{
//    PlankMemoryRef m;
//    PlankLockFreeDynamicArrayRef p;
//    
//    m = pl_MemoryGlobal();
//    p = (PlankLockFreeDynamicArrayRef)pl_Memory_AllocateBytes (m, sizeof (PlankLockFreeDynamicArray));
//    
//    if (p != PLANK_NULL)
//        pl_MemoryZero (p, sizeof (PlankLockFreeDynamicArray));
//
//    return p;    
//}
//
//PlankResult pl_LockFreeDynamicArray_Init (PlankLockFreeDynamicArrayRef p)
//{
//    return pl_LockFreeDynamicArray_InitWithItemSize (p, PLANKLOCKFREEDYNAMICARRAY_DEFAULTITEMSIZE);
//}
//
//PlankResult pl_LockFreeDynamicArray_InitWithItemSize (PlankLockFreeDynamicArrayRef p, const PlankL itemSize)
//{
//    return pl_LockFreeDynamicArray_InitWithItemSizeAndCapacity (p, itemSize, PLANKLOCKFREEDYNAMICARRAY_DEFAULTGRANULARITY);
//}
//
//PlankResult pl_LockFreeDynamicArray_InitWithItemSizeAndCapacity (PlankLockFreeDynamicArrayRef p, const PlankL itemSize, const PlankL initialCapacity)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m;
//
//    p->itemSize = itemSize;
//    p->allocatedItems = initialCapacity;
//    p->usedItems = 0;
//    
//    m = pl_MemoryGlobal();
//    p->data = pl_Memory_AllocateBytes (m, p->itemSize * p->allocatedItems);
//    
//    if (p->data == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//exit:
//    return result;    
//}
//
//PlankResult pl_LockFreeDynamicArray_DeInit (PlankLockFreeDynamicArrayRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m = pl_MemoryGlobal();
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    if (p->data)
//    {
//        if ((result = pl_Memory_Free (m, p->data)) != PlankResult_OK)
//            goto exit;
//    }
//    
//exit:
//    return result;
//}
//
//PlankResult pl_LockFreeDynamicArray_Destroy (PlankLockFreeDynamicArrayRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m = pl_MemoryGlobal();
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    if ((result = pl_LockFreeDynamicArray_DeInit (p)) != PlankResult_OK)
//        goto exit;
//    
//    result = pl_Memory_Free (m, p);
//    
//exit:
//    return result;
//}
//
//PlankB pl_LockFreeDynamicArray_IsIndexInRange (PlankLockFreeDynamicArrayRef p, const PlankL index)
//{
//    return (index >= 0) && (index < p->usedItems);
//}
//
//PlankLL pl_LockFreeDynamicArray_GetSize (PlankLockFreeDynamicArrayRef p)
//{
//    return p->usedItems;
//}
//
//PlankResult pl_LockFreeDynamicArray_AddItem (PlankLockFreeDynamicArrayRef p, const PlankP item)
//{
//    PlankResult result = PlankResult_OK;
//    PlankL index;
//    
//    index = p->usedItems;
//    p->usedItems++;
//    
//    if (p->usedItems > p->allocatedItems)
//    {        
//        result = pl_LockFreeDynamicArray_Grow (p, PLANKLOCKFREEDYNAMICARRAY_DEFAULTGRANULARITY);
//        
//        if (result != PlankResult_OK)
//            goto exit;
//    }
//    
//    memcpy ((unsigned char*)p->data + index * p->itemSize, item, p->itemSize);
//    
//exit:
//    return result;
//}
//
//PlankResult pl_LockFreeDynamicArray_SetItem (PlankLockFreeDynamicArrayRef p, const PlankL index, const PlankP item)
//{
//    PlankResult result = PlankResult_OK;
//    
//    if ((index < 0) || (index >= p->usedItems))
//    {
//        result = PlankResult_ArrayParameterError;
//        goto exit;
//    }
//    
//    memcpy ((unsigned char*)p->data + index * p->itemSize, item, p->itemSize);
//    
//exit:
//    return result;        
//}
//
//PlankResult pl_LockFreeDynamicArray_InsertItem (PlankLockFreeDynamicArrayRef p, const PlankL index, const PlankP item)
//{
//    PlankResult result = PlankResult_OK;
//    PlankP src, dst;
//    PlankL size;
//    
//    if (index < 0)
//    {
//        result = PlankResult_ArrayParameterError;
//        goto exit;
//    }
//    
//    if ((result = pl_LockFreeDynamicArray_EnsureSize (p, p->usedItems + 1)) != PlankResult_OK) goto exit;
//    
//    if (index < p->usedItems)
//    {
//        src = (unsigned char*)p->data + index * p->itemSize;
//        dst = (unsigned char*)p->data + (index + 1) * p->itemSize;
//        size = p->itemSize * (p->usedItems - index);
//        memcpy (dst, src, size);
//    
//        dst = src;
//        src = item;
//        size = p->itemSize;
//        memcpy (dst, src, size);
//    }
//    else
//    {
//        dst = (unsigned char*)p->data + p->usedItems * p->itemSize;
//        src = item;
//        size = p->itemSize;
//        memcpy (dst, src, size);        
//    }
//    
//    p->usedItems++;
//    
//exit:
//    return result;
//}
//
//PlankResult pl_LockFreeDynamicArray_GetItem (PlankLockFreeDynamicArrayRef p, const PlankL index, PlankP item)
//{
//    PlankResult result = PlankResult_OK;
//
//    if ((index < 0) || (index >= p->usedItems))
//    {
//        result = PlankResult_ArrayParameterError;
//        goto exit;
//    }
//    
//    memcpy (item, (unsigned char*)p->data + index * p->itemSize, p->itemSize);
//    
//exit:
//    return result;    
//}
//
//PlankResult pl_LockFreeDynamicArray_RemoveItem (PlankLockFreeDynamicArrayRef p, const PlankL index)
//{
//    PlankResult result = PlankResult_OK;
//    PlankP src, dst;
//    PlankL size;
//    
//    if ((index < 0) || (index >= p->usedItems))
//    {
//        result = PlankResult_ArrayParameterError;
//        goto exit;
//    }
//    
//    p->usedItems--;
//    
//    if (p->usedItems > 0)
//    {
//        dst = (unsigned char*)p->data + index * p->itemSize;
//        src = (unsigned char*)p->data + (index + 1) * p->itemSize;
//        size = p->itemSize * (p->usedItems - index);
//        memcpy (dst, src, size);
//    }
//    
//exit:
//    return result;            
//}
//
//PlankResult pl_LockFreeDynamicArray_EnsureSize (PlankLockFreeDynamicArrayRef p, const PlankL capacity)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m;
//    PlankL newAllocatedItems;
//    PlankP newData;
//    
//    if (capacity <= p->allocatedItems)
//        goto exit;
//    
//    m = pl_MemoryGlobal();
//    
//    newAllocatedItems = pl_MaxL (capacity, p->allocatedItems + PLANKLOCKFREEDYNAMICARRAY_DEFAULTGRANULARITY);
//    newData = pl_Memory_AllocateBytes (m, p->itemSize * newAllocatedItems);
//    
//    if (newData == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    memcpy (newData, p->data, p->itemSize * p->allocatedItems);
//    
//    pl_Memory_Free (m, p->data);
//    p->data = newData;
//    p->allocatedItems = newAllocatedItems;
//    
//exit:
//    return result;    
//}
//
//PlankResult pl_LockFreeDynamicArray_Grow (PlankLockFreeDynamicArrayRef p, const int amount)
//{
//    PlankResult result = PlankResult_OK;
//
//    if (amount < 1)
//    {
//        result = PlankResult_ArrayParameterError;
//        goto exit;
//    }
//
//    result = pl_LockFreeDynamicArray_EnsureSize (p, p->allocatedItems + amount);
//    
//exit:
//    return result;
//}
//
//PlankResult pl_LockFreeDynamicArray_Purge (PlankLockFreeDynamicArrayRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m;
//    PlankL newAllocatedItems;
//    PlankP newData;
//    
//    if (p->usedItems == p->allocatedItems)
//        goto exit;
//    
//    m = pl_MemoryGlobal();
//    
//    newAllocatedItems = p->usedItems;
//    newData = pl_Memory_AllocateBytes (m, p->itemSize * newAllocatedItems);
//    
//    if (newData == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    memcpy (newData, p->data, p->itemSize * p->usedItems);
//    
//    pl_Memory_Free (m, p->data);
//    p->data = newData;
//    p->allocatedItems = newAllocatedItems;
//    
//exit:
//    return result;    
//}
//

