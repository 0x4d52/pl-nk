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

#ifndef PLANK_LOCKFREEDYNAMICARRAY_H
#define PLANK_LOCKFREEDYNAMICARRAY_H

// unlikely to implement now... 

//#define PLANKLOCKFREEDYNAMICARRAY_DEFAULTGRANULARITY 8
//#define PLANKLOCKFREEDYNAMICARRAY_DEFAULTITEMSIZE 4
//
//PLANK_BEGIN_C_LINKAGE
//
//
///** A lock-free dynamic array.
// 
// @defgroup PlankLockFreeDynamicArrayClass Plank LockFreeDynamicArray class
// @ingroup PlankClasses
// @{
// */
//
///** An opaque reference to the <i>Plank LockFreeDynamicArray</i> object. */
//typedef struct PlankLockFreeDynamicArray* PlankLockFreeDynamicArrayRef; 
//
///** Create and initialise a <i>Plank LockFreeDynamicArray</i> object and return an oqaque reference to it.
// @return A <i>Plank LockFreeDynamicArray</i> object as an opaque reference or PLANK_NULL. */
//PlankLockFreeDynamicArrayRef pl_LockFreeDynamicArray_CreateAndInit();
//
///** Create a <i>Plank LockFreeDynamicArray</i> object and return an oqaque reference to it.
// @return A <i>Plank LockFreeDynamicArray</i> object as an opaque reference or PLANK_NULL. */
//PlankLockFreeDynamicArrayRef pl_LockFreeDynamicArray_Create();
//
///** Initialise the array.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_Init (PlankLockFreeDynamicArrayRef);
//
///** Initialise the array.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_InitWithItemSize (PlankLockFreeDynamicArrayRef p, const PlankL itemSize);
//
///** Initialise the array with specified capacity
// @param p The <i>Plank LockFreeDynamicArray</i> object
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_InitWithItemSizeAndCapacity (PlankLockFreeDynamicArrayRef p, const PlankL itemSize, const PlankL initialCapacity);
//
///** Denitialise the array.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_DeInit (PlankLockFreeDynamicArrayRef p);
//
///** Destroy a <i>Plank LockFreeDynamicArray</i> object. 
// @param p The <i>Plank LockFreeDynamicArray</i> object. */
//PlankResult pl_LockFreeDynamicArray_Destroy (PlankLockFreeDynamicArrayRef p);
//
///** Determines if an index is in range for this array. */
//PlankB pl_LockFreeDynamicArray_IsIndexInRange (PlankLockFreeDynamicArrayRef p, const PlankL index);
//
///** Get the number of items stored in the array. */
//PlankLL pl_LockFreeDynamicArray_GetSize (PlankLockFreeDynamicArrayRef p);
//
///** Add an item to the array.
// This copies the bytes from one memory location to the array data based on the item
// size set during initialisation.
// @param p The <i>Plank LockFreeDynamicArray</i> object
// @param item The item to add to the array.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_AddItem (PlankLockFreeDynamicArrayRef p, const PlankP item);
//
///** Copies an item to the specified index. 
// This copies the bytes from one memory location to the array data based on the item
// size set during initialisation. 
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @param index The desired index. This must be between 0 and one less that the current size.  
// @param item The item to add to the array.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_SetItem (PlankLockFreeDynamicArrayRef p, const PlankL index, const PlankP item);
//
///** Inserts an item into the specified index. 
// This copies the bytes from one memory location to the array data based on the item
// size set during initialisation. Items above the specified index will be moved one
// index higher.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @param index The desired index. This should be 0 or greater. If this is beyond the 
//              size of the array the item will be added to the end. 
// @param item The item to add to the array.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_InsertItem (PlankLockFreeDynamicArrayRef p, const PlankL index, const PlankP item);
//
///** Retrieves an item at the specified index.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @param index The desired index. This must be between 0 and one less that the current size.  
// @param item The destination to copy the specified item. This must point to memory of 
//             sufficient size to hold the item.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_GetItem (PlankLockFreeDynamicArrayRef p, const PlankL index, PlankP item);
//
///** Removes an item from the specified index.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @param index The desired index to remove. This must be between 0 and one less that the current size. 
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_RemoveItem (PlankLockFreeDynamicArrayRef p, const PlankL index);
//
///** Ensures that the array is sufficient to hold a certain number of items.
// This will resize the array if necessary to hold the specified capacity. This only
// resizes the avauilable capacity pl_LockFreeDynamicArray_GetSize() will still return the
// number of items used.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @param capacity The number of items the array should be set to hold.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_EnsureSize (PlankLockFreeDynamicArrayRef p, const PlankL capacity);
//
///** Inreases the array's capcity by a spefcified number of items.
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @param amount The number of additional items the array should be set to hold.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_Grow (PlankLockFreeDynamicArrayRef p, const int amount);
//
///** Reduces the amount of memory the array uses to the minimum.
// This sets the allocated size to the number of items used.
// @warniong During this operation the array may temporarily require more memory. 
// @param p The <i>Plank LockFreeDynamicArray</i> object.
// @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_LockFreeDynamicArray_Purge (PlankLockFreeDynamicArrayRef p);
//
///** @} */
//
//PLANK_END_C_LINKAGE
//
//#if !DOXYGEN
//typedef struct PlankLockFreeDynamicArray
//{
//    PlankL itemSize;
//    PlankL allocatedItems;
//    PlankL usedItems;
//    PlankP data;
//} PlankLockFreeDynamicArray;
//#endif

#endif // PLANK_LOCKFREEDYNAMICARRAY_H
