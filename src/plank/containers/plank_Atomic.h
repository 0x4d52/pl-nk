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

#ifndef PLANK_ATOMIC_H
#define PLANK_ATOMIC_H

#if PLANK_NOATOMIC64BIT
    #include "../core/plank_Lock.h"
#endif

PLANK_BEGIN_C_LINKAGE

/** Crossplatform atomic operation classes.
 
 @defgroup PlankAtomicClasses Plank Atomic operation classes
 @ingroup PlankClasses
 @{
 */

/** A crossplatform read/write memory barrier. */
static void pl_AtomicMemoryBarrier();

/** @} */

//------------------------------------------------------------------------------

/** Atomic integer.
 
 This will be a 32-bit integer on all Plank-supported platforms.
  
 @defgroup PlankAtomicIClass Plank AtomicI class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicI</i> object (atomic 32-bit integer). */
typedef struct PlankAtomicI* PlankAtomicIRef;

/** Creates and initialises a <i>Plank %AtomicI</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicI</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicIRef pl_AtomicI_CreateAndInit();

/** Create a <i>Plank %AtomicI</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicI</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicIRef pl_AtomicI_Create();

/** Initialise a <i>Plank %AtomicI</i> object. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicI_Init (PlankAtomicIRef p);

/** Deinitialise a <i>Plank %AtomicI</i> object. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicI_DeInit (PlankAtomicIRef p);

/** Destroy a <i>Plank %AtomicI</i> object. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicI_Destroy (PlankAtomicIRef p);

/** Get the current value. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return The value. */
static PlankI pl_AtomicI_Get (PlankAtomicIRef p);

/** Not used for this class. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return Always returns 0. */
PlankI pl_AtomicI_GetExtra (PlankAtomicIRef p);

/** Swap the current value with a new value. 
 @param p The <i>Plank %AtomicI</i> object. 
 @param newValue The new value to store.
 @return The previously stored value. */
static PlankI pl_AtomicI_Swap (PlankAtomicIRef p, PlankI newValue);

/** Set the current value to a new value. 
 @param p The <i>Plank %AtomicI</i> object. 
 @param newValue The new value to store. */
static void pl_AtomicI_Set (PlankAtomicIRef p, PlankI newValue);

/** Add a value to the current value. 
 @param p The <i>Plank %AtomicI</i> object. 
 @param operand The value to add. 
 @return The new value. */
static PlankI pl_AtomicI_Add (PlankAtomicIRef p, PlankI operand);

/** Subtract a value from the current value. 
 @param p The <i>Plank %AtomicI</i> object. 
 @param operand The value to subtract. 
 @return The new value. */
static PlankI pl_AtomicI_Subtract (PlankAtomicIRef p, PlankI operand);

/** Increment the current value by 1. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return The new value. */
static PlankI pl_AtomicI_Increment (PlankAtomicIRef p);

/** Decrement the current value by 1. 
 @param p The <i>Plank %AtomicI</i> object. 
 @return The new value. */
static PlankI pl_AtomicI_Decrement (PlankAtomicIRef p);

/** Swap the current value with a new value if a specified old value is still the current value.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old value again
 and retrying to write the new value (e.g., in a loop) until successful. If there
 are no other threads contending to write the value the operation should succeed without
 blocking.
 @param p The <i>Plank %AtomicI</i> object. 
 @param oldValue The expected old value being currently stored.
 @param newValue The new value to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
static PlankB pl_AtomicI_CompareAndSwap (PlankAtomicIRef p, PlankI oldValue, PlankI newValue);

/** @} */

//------------------------------------------------------------------------------

/** Atomic long.
 
 This will be a 32-bit integer on 32-bit systems and a 64-bit integer on 64-bt
 systems on all Plank-supported platforms (even on Windows where a long is 
 normally 32 bits on both architectures).
 
 @defgroup PlankAtomicLClass Plank AtomicL class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicL</i> object (atomic long integer). */
typedef struct PlankAtomicL* PlankAtomicLRef;

/** Creates and initialises a <i>Plank %AtomicL</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicL</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicLRef pl_AtomicL_CreateAndInit();

/** Create a <i>Plank %AtomicL</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicL</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicLRef pl_AtomicL_Create();

/** Initialise a <i>Plank %AtomicL</i> object. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicL_Init (PlankAtomicLRef p);

/** Deinitialise a <i>Plank %AtomicL</i> object. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicL_DeInit (PlankAtomicLRef p);

/** Destroy a <i>Plank %AtomicL</i> object. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicL_Destroy (PlankAtomicLRef p);

/** Get the current value. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return The value. */
static PlankL pl_AtomicL_Get (PlankAtomicLRef p);

/** Not used for this class. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return Always returns 0. */
PlankL pl_AtomicL_GetExtra (PlankAtomicLRef p);

/** Swap the current value with a new value. 
 @param p The <i>Plank %AtomicL</i> object. 
 @param newValue The new value to store.
 @return The previously stored value. */
static PlankL pl_AtomicL_Swap (PlankAtomicLRef p, PlankL newValue);

/** Set the current value to a new value. 
 @param p The <i>Plank %AtomicL</i> object. 
 @param newValue The new value to store. */
static void pl_AtomicL_Set (PlankAtomicLRef p, PlankL newValue);

/** Add a value to the current value. 
 @param p The <i>Plank %AtomicL</i> object. 
 @param operand The value to add. 
 @return The new value. */
static PlankL pl_AtomicL_Add (PlankAtomicLRef p, PlankL operand);

/** Subtract a value from the current value. 
 @param p The <i>Plank %AtomicL</i> object. 
 @param operand The value to subtract. 
 @return The new value. */
static PlankL pl_AtomicL_Subtract (PlankAtomicLRef p, PlankL operand);

/** Increment the current value by 1. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return The new value. */
static PlankL pl_AtomicL_Increment (PlankAtomicLRef p);

/** Decrement the current value by 1. 
 @param p The <i>Plank %AtomicL</i> object. 
 @return The new value. */
static PlankL pl_AtomicL_Decrement (PlankAtomicLRef p);

/** Swap the current value with a new value if a specified old value is still the current value.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old value again
 and retrying to write the new value (e.g., in a loop) until successful. If there
 are no other threads contending to write the value the operation should succeed without
 blocking.
 @param p The <i>Plank %AtomicL</i> object. 
 @param oldValue The expected old value being currently stored.
 @param newValue The new value to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
static PlankB pl_AtomicL_CompareAndSwap (PlankAtomicLRef p, PlankL oldValue, PlankL newValue);

/** @} */

//------------------------------------------------------------------------------

/** Atomic long long.
 
 This will be a 64-bit integer on all Plank-supported platforms.
 
 On PPC on Mac OS this is not a truly lock-free operations, it
 uses a lock due to the absence of 64-bit atomic operations.
 
 @defgroup PlankAtomicLLClass Plank AtomicLL class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicLL</i> object (atomic 64-bit integer). */
typedef struct PlankAtomicLL* PlankAtomicLLRef;

/** Creates and initialises a <i>Plank %AtomicLL</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicLL</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicLLRef pl_AtomicLL_CreateAndInit();

/** Create a <i>Plank %AtomicLL</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicLL</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicLLRef pl_AtomicLL_Create();

/** Initialise a <i>Plank %AtomicLL</i> object. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicLL_Init (PlankAtomicLLRef p);

/** Deinitialise a <i>Plank %AtomicLL</i> object. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicLL_DeInit (PlankAtomicLLRef p);

/** Destroy a <i>Plank %AtomicLL</i> object. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicLL_Destroy (PlankAtomicLLRef p);

/** Get the current value. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return The value. */
static PlankLL pl_AtomicLL_Get (PlankAtomicLLRef p);

/** Not used for this class. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return Always returns 0. */
PlankLL pl_AtomicLL_GetExtra (PlankAtomicLLRef p);

/** Swap the current value with a new value. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @param newValue The new value to store.
 @return The previously stored value. */
static PlankLL pl_AtomicLL_Swap (PlankAtomicLLRef p, PlankLL newValue);

/** Set the current value to a new value. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @param newValue The new value to store. */
static void pl_AtomicLL_Set (PlankAtomicLLRef p, PlankLL newValue);

/** Add a value to the current value. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @param operand The value to add. 
 @return The new value. */
static PlankLL pl_AtomicLL_Add (PlankAtomicLLRef p, PlankLL operand);

/** Subtract a value from the current value. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @param operand The value to subtract. 
 @return The new value. */
static PlankLL pl_AtomicLL_Subtract (PlankAtomicLLRef p, PlankLL operand);

/** Increment the current value by 1. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return The new value. */
static PlankLL pl_AtomicLL_Increment (PlankAtomicLLRef p);

/** Decrement the current value by 1. 
 @param p The <i>Plank %AtomicLL</i> object. 
 @return The new value. */
static PlankLL pl_AtomicLL_Decrement (PlankAtomicLLRef p);

/** Swap the current value with a new value if a specified old value is still the current value.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old value again
 and retrying to write the new value (e.g., in a loop) until successful. If there
 are no other threads contending to write the value the operation should succeed without
 blocking.
 @param p The <i>Plank %AtomicLL</i> object. 
 @param oldValue The expected old value being currently stored.
 @param newValue The new value to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
#if !PLANK_NOATOMIC64BIT || DOXYGEN
static
#endif
PlankB pl_AtomicLL_CompareAndSwap (PlankAtomicLLRef p, PlankLL oldValue, PlankLL newValue);

/** @} */

//------------------------------------------------------------------------------

/** Atomic float.
 
 This will be a 32-bit float on all Plank-supported platforms.
 
 @defgroup PlankAtomicFClass Plank AtomicF class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicF</i> object (atomic float). */
typedef struct PlankAtomicF* PlankAtomicFRef;

/** Creates and initialises a <i>Plank %AtomicF</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicF</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicFRef pl_AtomicF_CreateAndInit();

/** Create a <i>Plank %AtomicF</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicF</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicFRef pl_AtomicF_Create();

/** Initialise a <i>Plank %AtomicF</i> object. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicF_Init (PlankAtomicFRef p);

/** Deinitialise a <i>Plank %AtomicF</i> object. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicF_DeInit (PlankAtomicFRef p);

/** Destroy a <i>Plank %AtomicF</i> object. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicF_Destroy (PlankAtomicFRef p);

/** Get the current value. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return The value. */
static PlankF pl_AtomicF_Get (PlankAtomicFRef p);

/** Not used for this class. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return Always returns 0. */
PlankI pl_AtomicF_GetExtra (PlankAtomicFRef p);

/** Swap the current value with a new value. 
 @param p The <i>Plank %AtomicF</i> object. 
 @param newValue The new value to store.
 @return The previously stored value. */
static PlankF pl_AtomicF_Swap (PlankAtomicFRef p, PlankF newValue);

/** Set the current value to a new value. 
 @param p The <i>Plank %AtomicF</i> object. 
 @param newValue The new value to store. */
static void pl_AtomicF_Set (PlankAtomicFRef p, PlankF newValue);

/** Add a value to the current value. 
 @param p The <i>Plank %AtomicF</i> object. 
 @param operand The value to add. 
 @return The new value. */
static PlankF pl_AtomicF_Add (PlankAtomicFRef p, PlankF operand);

/** Subtract a value from the current value. 
 @param p The <i>Plank %AtomicF</i> object. 
 @param operand The value to subtract. 
 @return The new value. */
static PlankF pl_AtomicF_Subtract (PlankAtomicFRef p, PlankF operand);

/** Increment the current value by 1. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return The new value. */
static PlankF pl_AtomicF_Increment (PlankAtomicFRef p);

/** Decrement the current value by 1. 
 @param p The <i>Plank %AtomicF</i> object. 
 @return The new value. */
static PlankF pl_AtomicF_Decrement (PlankAtomicFRef p);

/** Swap the current value with a new value if a specified old value is still the current value.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old value again
 and retrying to write the new value (e.g., in a loop) until successful. If there
 are no other threads contending to write the value the operation should succeed without
 blocking. 
 
 NB A bit-level comparison is performed. e.g., -0.f and 0.f would not be considered equal but
 two identical NaNs would be. In both of these cases this is different behaviour than comparing
 the two floating point values (i.e., -0.f and 0.f are considered equal and NaNs are never
 equal to anything else even if there's another identical NaN with which you are comparing).
 
 @param p The <i>Plank %AtomicF</i> object. 
 @param oldValue The expected old value being currently stored.
 @param newValue The new value to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
static PlankB pl_AtomicF_CompareAndSwap (PlankAtomicFRef p, PlankF oldValue, PlankF newValue);

/** @} */

//------------------------------------------------------------------------------

/** Atomic double.
 
 This will be a 64-bit float on all Plank-supported platforms.
 
 On PPC on Mac OS these are not a truly lock-free operations, it
 uses a lock due to the absence of 64-bit atomic operations.

 @defgroup PlankAtomicDClass Plank AtomicD class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicD</i> object (atomic double). */
typedef struct PlankAtomicD* PlankAtomicDRef;

/** Creates and initialises a <i>Plank %AtomicD</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicD</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicDRef pl_AtomicD_CreateAndInit();

/** Create a <i>Plank %AtomicF</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicF</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicDRef pl_AtomicD_Create();

/** Initialise a <i>Plank %AtomicD</i> object. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicD_Init (PlankAtomicDRef p);

/** Deinitialise a <i>Plank %AtomicD</i> object. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicD_DeInit (PlankAtomicDRef p);

/** Destroy a <i>Plank %AtomicD</i> object. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicD_Destroy (PlankAtomicDRef p);

/** Get the current value. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return The value. */
static PlankD pl_AtomicD_Get (PlankAtomicDRef p);

/** Not used for this class. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return Always returns 0. */
PlankLL pl_AtomicD_GetExtra (PlankAtomicDRef p);

/** Swap the current value with a new value. 
 @param p The <i>Plank %AtomicD</i> object. 
 @param newValue The new value to store.
 @return The previously stored value. */
static PlankD pl_AtomicD_Swap (PlankAtomicDRef p, PlankD newValue);

/** Set the current value to a new value. 
 @param p The <i>Plank %AtomicD</i> object. 
 @param newValue The new value to store. */
static void pl_AtomicD_Set (PlankAtomicDRef p, PlankD newValue);

/** Add a value to the current value. 
 @param p The <i>Plank %AtomicD</i> object. 
 @param operand The value to add. 
 @return The new value. */
static PlankD pl_AtomicD_Add (PlankAtomicDRef p, PlankD operand);

/** Subtract a value from the current value. 
 @param p The <i>Plank %AtomicD</i> object. 
 @param operand The value to subtract. 
 @return The new value. */
static PlankD pl_AtomicD_Subtract (PlankAtomicDRef p, PlankD operand);

/** Increment the current value by 1. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return The new value. */
static PlankD pl_AtomicD_Increment (PlankAtomicDRef p);

/** Decrement the current value by 1. 
 @param p The <i>Plank %AtomicD</i> object. 
 @return The new value. */
static PlankD pl_AtomicD_Decrement (PlankAtomicDRef p);

/** Swap the current value with a new value if a specified old value is still the current value.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old value again
 and retrying to write the new value (e.g., in a loop) until successful. If there
 are no other threads contending to write the value the operation should succeed without
 blocking. 
 
 NB A bit-level comparison is performed. e.g., -0.0 and 0.0 would not be considered equal but
 two identical NaNs would be. In both of these cases this is different behaviour than comparing
 the two floating point values (i.e., -0.0 and 0.0 are considered equal and NaNs are never
 equal to anything else even if there's another identical NaN with which you are comparing).
 
 @param p The <i>Plank %AtomicD</i> object. 
 @param oldValue The expected old value being currently stored.
 @param newValue The new value to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
static PlankB pl_AtomicD_CompareAndSwap (PlankAtomicDRef p, PlankD oldValue, PlankD newValue);

/** @} */

//------------------------------------------------------------------------------

/** Atomic pointer.
 
 This will be the natural width of the pointer on the runtime platform.
 
 Use @link PlankAtomicPXClass AtomicPX @endlink for safer pointer storage.
 
 @defgroup PlankAtomicPClass Plank AtomicP class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicP</i> object (atomic pointer). */
typedef struct PlankAtomicP* PlankAtomicPRef;

/** Creates and initialises a <i>Plank %AtomicP</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicP</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicPRef pl_AtomicP_CreateAndInit();

/** Create a <i>Plank %AtomicD</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicD</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicPRef pl_AtomicP_Create();

/** Initialise a <i>Plank %AtomicP</i> object. 
 @param p The <i>Plank %AtomicP</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicP_Init (PlankAtomicPRef p);

/** Deinitialise a <i>Plank %AtomicP</i> object. 
 @param p The <i>Plank %AtomicP</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicP_DeInit (PlankAtomicPRef p);

/** Destroy a <i>Plank %AtomicP</i> object. 
 @param p The <i>Plank %AtomicP</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicP_Destroy (PlankAtomicPRef p);

/** Get the current value. 
 @param p The <i>Plank %AtomicP</i> object. 
 @return The value. */
static PlankP pl_AtomicP_Get (PlankAtomicPRef p);

/** Not used for this class. 
 Use @link PlankAtomicPXClass AtomicPX @endlink for safer pointer storage.
 @param p The <i>Plank %AtomicI</i> object. 
 @return Always returns 0. */
PlankL pl_AtomicP_GetExtra (PlankAtomicPRef p);

/** Swap the current pointer with a new pointer. 
 @param p The <i>Plank %AtomicP</i> object. 
 @param newPtr The new value to store.
 @return The previously stored pointer. */
static PlankP pl_AtomicP_Swap (PlankAtomicPRef p, PlankP newPtr);

/** Set the current pointer to a new pointer. 
 @param p The <i>Plank %AtomicP</i> object. 
 @param newPtr The new pointer to store. */
static void pl_AtomicP_Set (PlankAtomicPRef p, PlankP newPtr);

/** Offset current pointer. 
 @param p The <i>Plank %AtomicP</i> object. 
 @param operand The number of bytes by which to offset. 
 @return The new pointer. */
static PlankP pl_AtomicP_Add (PlankAtomicPRef p, PlankL operand);

/** Offset current pointer in a negative direct. 
 @param p The <i>Plank %AtomicP</i> object. 
 @param operand The number of bytes to subtract from the pointer. 
 @return The new pointer. */
static PlankP pl_AtomicP_Subtract (PlankAtomicPRef p, PlankL operand);

/** Increment the current pointer by 1 byte. 
 @param p The <i>Plank %AtomicP</i> object. 
 @return The new pointer. */
static PlankP pl_AtomicP_Increment (PlankAtomicPRef p);

/** Decrement the current pointer by 1 byte. 
 @param p The <i>Plank %AtomicP</i> object. 
 @return The new pointer. */
static PlankP pl_AtomicP_Decrement (PlankAtomicPRef p);

/** Swap the current pointer with a new pointer if a specified old pointer is still current.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old pointer again
 and retrying to write the new pointer (e.g., in a loop) until successful. If there
 are no other threads contending to write the pointer the operation should succeed without
 blocking.
 @param p The <i>Plank %AtomicP</i> object. 
 @param oldPtr The expected old pointer being currently stored.
 @param newPtr The new pointer to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
static PlankB pl_AtomicP_CompareAndSwap (PlankAtomicPRef p, PlankP oldPtr, PlankP newPtr);

/** @} */

//------------------------------------------------------------------------------

/** Extended atomic pointer.
 
 This combines a pointer and an extra "tag" which can be written in a single atomic
 operation. This can help avoid the "ABA problem" in certain lock-free data
 structures. 
 
 The default is to use the extra tag as a counter such that the counter is incremented
 each time the stored pointer us set.
 
 On PPC on Mac OS these are not a truly lock-free operations, it
 uses a lock due to the absence of 64-bit atomic operations.
 
 @defgroup PlankAtomicPXClass Plank AtomicPX class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicPX</i> object 
 (atomic pointer with an extended tag of data). */
typedef struct PlankAtomicPX* PlankAtomicPXRef;

/** Creates and initialises a <i>Plank %AtomicPX</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicPX</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicPXRef pl_AtomicPX_CreateAndInit();

/** Create a <i>Plank %AtomicPX</i> object and return an oqaque reference to it.
 In this case both the pointer and the extra tag are set to 0.
 @return A <i>Plank %AtomicPX</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicPXRef pl_AtomicPX_Create();

/** Initialise a <i>Plank %AtomicPX</i> object. 
 @param p The <i>Plank %AtomicPX</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicPX_Init (PlankAtomicPXRef p);

/** Deinitialise a <i>Plank %AtomicPX</i> object. 
 @param p The <i>Plank %AtomicPX</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicPX_DeInit (PlankAtomicPXRef p);

/** Destroy a <i>Plank %AtomicPX</i> object. 
 @param p The <i>Plank %AtomicPX</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicPX_Destroy (PlankAtomicPXRef p);

/** Get the current pointer. 
 The extra tag can be obtained using pl_AtomicPX_GetExtra() although for many
 algorithms the user doesn't need to know the value of the tag as it is simply
 used to make each write commit unique.
 @param p The <i>Plank %AtomicPX</i> object. 
 @return The value. */
PlankP pl_AtomicPX_Get (PlankAtomicPXRef p);

/** Get the current value of the extra "tag". 
 Many algorithms the user doesn't need to know the value of the tag as it is 
 simply used to make each write commit unique.
 @param p The <i>Plank %AtomicPX</i> object. 
 @return The extra tag. */
PlankL pl_AtomicPX_GetExtra (PlankAtomicPXRef p);

/** Swap the current pointer with a new pointer and change the tag. 
 @param p The <i>Plank %AtomicPX</i> object. 
 @param newPtr The new value to store.
 @param newExtra The new extra tag to store.
 @param oldExtra The old tage will be place here (you can pass PLANK_NULL if you don't need this).
 @return The previously stored value. */
PlankP pl_AtomicPX_SwapAll (PlankAtomicPXRef p, PlankP newPtr, PlankL newExtra, PlankL* oldExtra);

/** Swap the current pointer with a new pointer. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicPX</i> object. 
 @param newPtr The new pointer to store.
 @return The previously stored value. */
PlankP pl_AtomicPX_Swap (PlankAtomicPXRef p, PlankP newPtr);

/** Set the current pointer to a new pointer and change the tag. 
 @param p The <i>Plank %AtomicPX</i> object. 
 @param newPtr The new value to store.
 @param newExtra The new extra tag to store. */
void pl_AtomicPX_SetAll (PlankAtomicPXRef p, PlankP newPtr, PlankL newExtra);

/** Set the current pointer to a new pointer. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicPX</i> object. 
 @param newPtr The new pointer to store. */
void pl_AtomicPX_Set (PlankAtomicPXRef p, PlankP newPtr);

/** Offset current pointer. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicPX</i> object. 
 @param operand The number of bytes by which to offset. 
 @return The new pointer. */
PlankP pl_AtomicPX_Add (PlankAtomicPXRef p, PlankL operand);

/** Offset current pointer in a negative direct. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicPX</i> object. 
 @param operand The number of bytes to subtract from the pointer. 
 @return The new pointer. */
PlankP pl_AtomicPX_Subtract (PlankAtomicPXRef p, PlankL operand);

/** Increment the current pointer by 1 byte. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicPX</i> object. 
 @return The new pointer. */
PlankP pl_AtomicPX_Increment (PlankAtomicPXRef p);

/** Decrement the current pointer by 1 byte. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicPX</i> object. 
 @return The new pointer. */
PlankP pl_AtomicPX_Decrement (PlankAtomicPXRef p);

/** Swap the current values with new values if specified old values are still current.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old pointer again
 and retrying to write the new pointer (e.g., in a loop) until successful. If there
 are no other threads contending to write the pointer the operation should succeed without
 blocking.
 
 This swaps both the pointer AND the extra tag but importantly both "old" values
 must match the currenty stored values for the swap to take place. This is the most
 important function for helping avoid the ABA problem.
 
 @param p The <i>Plank %AtomicPX</i> object. 
 @param oldPtr The expected old pointer being currently stored.
 @param oldExtra The expected old extra tag being currently stored.
 @param newPtr The new pointer to attempt to store.
 @param newExtra The new extra tag to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
PlankB pl_AtomicPX_CompareAndSwap (PlankAtomicPXRef p, PlankP oldPtr, PlankL oldExtra, PlankP newPtr, PlankL newExtra);

/** @} */

//------------------------------------------------------------------------------

/** Extended atomic pointer.
 
 This combines a pointer and an extra "tag" which can be written in a single atomic
 operation. This can help avoid the "ABA problem" in certain lock-free data
 structures. 
 
 The default is to use the extra tag as a counter such that the counter is incremented
 each time the stored pointer us set.
 
 On PPC on Mac OS these are not a truly lock-free operations, it
 uses a lock due to the absence of 64-bit atomic operations.
 
 @defgroup PlankAtomicPXClass Plank AtomicPX class
 @ingroup PlankAtomicClasses
 @{
 */

/** An opaque reference to the <i>Plank %AtomicLX</i> object 
 (atomic long with an extended tag of data). */
typedef struct PlankAtomicLX* PlankAtomicLXRef;

/** Creates and initialises a <i>Plank %AtomicLX</i> object and return an oqaque reference to it.
 @return A <i>Plank %AtomicLX</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicLXRef pl_AtomicLX_CreateAndInit();

/** Create a <i>Plank %AtomicLX</i> object and return an oqaque reference to it.
 In this case both the pointer and the extra tag are set to 0.
 @return A <i>Plank %AtomicLX</i> object as an opaque reference or PLANK_NULL. */
PlankAtomicLXRef pl_AtomicLX_Create();

/** Initialise a <i>Plank %AtomicLX</i> object. 
 @param p The <i>Plank %AtomicLX</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicLX_Init (PlankAtomicLXRef p);

/** Deinitialise a <i>Plank %AtomicLX</i> object. 
 @param p The <i>Plank %AtomicLX</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicLX_DeInit (PlankAtomicLXRef p);

/** Destroy a <i>Plank %AtomicLX</i> object. 
 @param p The <i>Plank %AtomicLX</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_AtomicLX_Destroy (PlankAtomicLXRef p);

/** Get the current VALUE. 
 The extra tag can be obtained using pl_AtomicLX_GetExtra() although for many
 algorithms the user doesn't need to know the value of the tag as it is simply
 used to make each write commit unique.
 @param p The <i>Plank %AtomicLX</i> object. 
 @return The value. */
PlankL pl_AtomicLX_Get (PlankAtomicLXRef p);

/** Get the current value of the extra "tag". 
 Many algorithms the user doesn't need to know the value of the tag as it is 
 simply used to make each write commit unique.
 @param p The <i>Plank %AtomicLX</i> object. 
 @return The extra tag. */
PlankL pl_AtomicLX_GetExtra (PlankAtomicLXRef p);

/** Swap the current pointer with a new pointer and change the tag. 
 @param p The <i>Plank %AtomicLX</i> object. 
 @param newValue The new value to store.
 @param newExtra The new extra tag to store.
 @param oldExtra The old tage will be place here (you can pass PLANK_NULL if you don't need this).
 @return The previously stored value. */
PlankL pl_AtomicLX_SwapAll (PlankAtomicLXRef p, PlankL newValue, PlankL newExtra, PlankL* oldExtra);

/** Swap the current pointer with a new pointer. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicLX</i> object. 
 @param newValue The new pointer to store.
 @return The previously stored value. */
PlankL pl_AtomicLX_Swap (PlankAtomicLXRef p, PlankL newValue);

/** Set the current value to a new value and change the tag. 
 @param p The <i>Plank %AtomicLX</i> object. 
 @param newValue The new value to store.
 @param newExtra The new extra tag to store. */
void pl_AtomicLX_SetAll (PlankAtomicLXRef p, PlankL newValue, PlankL newExtra);

/** Set the current value to a new value. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicLX</i> object. 
 @param newValue The new value to store. */
void pl_AtomicLX_Set (PlankAtomicLXRef p, PlankL newValue);

/** Offset current value. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicLX</i> object. 
 @param operand The amount by which to offset. 
 @return The new value. */
PlankL pl_AtomicLX_Add (PlankAtomicLXRef p, PlankL operand);

/** Offset current value in a negative direction. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicLX</i> object. 
 @param operand The amount to add to the value. 
 @return The new pointer. */
PlankL pl_AtomicLX_Subtract (PlankAtomicLXRef p, PlankL operand);

/** Increment the current value by 1. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicLX</i> object. 
 @return The new value. */
PlankL pl_AtomicLX_Increment (PlankAtomicLXRef p);

/** Decrement the current value by 1 byte. 
 This also increments the extra tag by 1.
 @param p The <i>Plank %AtomicLX</i> object. 
 @return The new value. */
PlankL pl_AtomicLX_Decrement (PlankAtomicLXRef p);

/** Swap the current values with new values if specified old values are still current.
 This is the most important atomic operation for each atomic class, many other operations are based on this.
 If the operation fails the user would generally try again by getting the old value again
 and retrying to write the new value (e.g., in a loop) until successful. If there
 are no other threads contending to write the value the operation should succeed without
 blocking.
 
 This swaps both the value AND the extra tag but importantly both "old" values
 must match the currenty stored values for the swap to take place. This is the most
 important function for helping avoid the ABA problem.
 
 @param p The <i>Plank %AtomicLX</i> object. 
 @param oldValue The expected old value being currently stored.
 @param oldExtra The expected old extra tag being currently stored.
 @param newValue The new value to attempt to store.
 @param newExtra The new extra tag to attempt to store.
 @return @c true if the swap was successful, otherwise @c false. */
PlankB pl_AtomicLX_CompareAndSwap (PlankAtomicLXRef p, PlankL oldValue, PlankL oldExtra, PlankL newValue, PlankL newExtra);

/** @} */


//------------------------------------------------------------------------------

PLANK_END_C_LINKAGE

#define PLANK_INLINING_FUNCTIONS 1
#include "plank_AtomicInline.h"
#undef PLANK_INLINING_FUNCTIONS

#endif // PLANK_ATOMIC_H
