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

#ifndef PLANK_DYNAMICARRAY_H
#define PLANK_DYNAMICARRAY_H

#define PLANKDYNAMICARRAY_DEFAULTGRANULARITY 8
#define PLANKDYNAMICARRAY_DEFAULTITEMSIZE 4

PLANK_BEGIN_C_LINKAGE

/** A dynamic array.
 
 @defgroup PlankDynamicArrayClass Plank DynamicArray class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank DynamicArray</i> object. */
typedef struct PlankDynamicArray* PlankDynamicArrayRef; 

/** Creates and intialises a <i>Plank DynamicArray</i> object and return an oqaque reference to it.
 @return A <i>Plank DynamicArray</i> object as an opaque reference or PLANK_NULL. */
PlankDynamicArrayRef pl_DynamicArray_CreateAndInit();

/** Create a <i>Plank DynamicArray</i> object and return an oqaque reference to it.
 @return A <i>Plank DynamicArray</i> object as an opaque reference or PLANK_NULL. */
PlankDynamicArrayRef pl_DynamicArray_Create();

/** Initialise the array.
 @param p The <i>Plank DynamicArray</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_Init (PlankDynamicArrayRef p);

/** Initialise the array.
 @param p The <i>Plank DynamicArray</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_InitWithItemSize (PlankDynamicArrayRef p, const PlankL itemSize);

/** Initialise the array with specified capacity.
 @param p The <i>Plank DynamicArray</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_InitWithItemSizeAndCapacity (PlankDynamicArrayRef p, const PlankL itemSize, const PlankL initialCapacity);

PlankResult pl_DynamicArray_InitWithItemSizeAndSize (PlankDynamicArrayRef p, const PlankL itemSize, const PlankL initialCapacity, PlankB zero);

/** Denitialise the array.
 @param p The <i>Plank DynamicArray</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_DeInit (PlankDynamicArrayRef p);

/** Destroy a <i>Plank DynamicArray</i> object. 
 @param p The <i>Plank DynamicArray</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_Destroy (PlankDynamicArrayRef p);

/** Determines if an index is in range for this array. */
PlankB pl_DynamicArray_IsIndexInRange (PlankDynamicArrayRef p, const PlankL index);

/** Get the number of items stored in the array. */
PlankL pl_DynamicArray_GetSize (PlankDynamicArrayRef p);

/** Get the size of a single item stored in the array. */
PlankL pl_DynamicArray_GetItemSize (PlankDynamicArrayRef p);


PlankResult pl_DynamicArray_SetSize (PlankDynamicArrayRef p, const PlankL capacity);

PlankResult pl_DynamicArray_Zero (PlankDynamicArrayRef p);


/** Get the raw array. */
PlankP pl_DynamicArray_GetArray (PlankDynamicArrayRef p);

/** Add an item to the array.
 This copies the bytes from one memory location to the array data based on the item
 size set during initialisation.
 @param p The <i>Plank DynamicArray</i> object.
 @param item The item to add to the array.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_AddItem (PlankDynamicArrayRef p, const PlankP item);

PlankResult pl_DynamicArray_AddItems (PlankDynamicArrayRef p, PlankConstantP items, const PlankL numItems);

/** Copies an item to the specified index. 
 This copies the bytes from one memory location to the array data based on the item
 size set during initialisation. 
 @param p The <i>Plank DynamicArray</i> object.
 @param index The desired index. This must be between 0 and one less that the current size.  
 @param item The item to add to the array.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_SetItem (PlankDynamicArrayRef p, const PlankL index, const PlankP item);

/** Inserts an item into the specified index. 
 This copies the bytes from one memory location to the array data based on the item
 size set during initialisation. Items above the specified index will be moved one
 index higher.
 @param p The <i>Plank DynamicArray</i> object.
 @param index The desired index. This should be 0 or greater. If this is beyond the 
              size of the array the item will be added to the end. 
 @param item The item to add to the array.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_InsertItem (PlankDynamicArrayRef p, const PlankL index, const PlankP item);

/** Retrieves an item at the specified index.
 @param p The <i>Plank DynamicArray</i> object.
 @param index The desired index. This must be between 0 and one less that the current size.  
 @param item The destination to copy the specified item. This must point to memory of 
             sufficient size to hold the item.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_GetItem (PlankDynamicArrayRef p, const PlankL index, PlankP item);

/** Removes an item from the specified index.
 @param p The <i>Plank DynamicArray</i> object.
 @param index The desired index to remove. This must be between 0 and one less that the current size. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_RemoveItem (PlankDynamicArrayRef p, const PlankL index);

/** Ensures that the array is sufficient to hold a certain number of items.
 This will resize the array if necessary to hold the specified capacity. This only
 resizes the avauilable capacity pl_DynamicArray_GetSize() will still return the
 number of items used.
 @param p The <i>Plank DynamicArray</i> object.
 @param capacity The number of items the array should be set to hold.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_EnsureSize (PlankDynamicArrayRef p, const PlankL capacity);

/** Inreases the array's capcity by a spefcified number of items.
 @param p The <i>Plank DynamicArray</i> object.
 @param amount The number of additional items the array should be set to hold.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_Grow (PlankDynamicArrayRef p, const int amount);

/** Reduces the amount of memory the array uses to the minimum.
 This sets the allocated size to the number of items used.
 @warniong During this operation the array may temporarily require more memory. 
 @param p The <i>Plank DynamicArray</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_DynamicArray_Purge (PlankDynamicArrayRef p);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankDynamicArray
{
    PlankL itemSize;
    PlankL allocatedItems;
    PlankL usedItems;
    PlankP data;
} PlankDynamicArray;
#endif

#endif // PLANK_DYNAMICARRAY_H
