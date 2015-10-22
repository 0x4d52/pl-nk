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

#ifndef PLANK_RNG_H
#define PLANK_RNG_H

PLANK_BEGIN_C_LINKAGE

/** A simple, fast, cross-platform random number generator.
 
 The following code snippet creates a <i>Plank %RNG</i> object, generates a random 
 float, then destroys the <i>Plank %RNG</i> object.
 
 @code
 PlankRNGRef rng;
 float value;
 rng = pl_RNG_CreateAndInit();
 value = pl_RNG_NextFloat (rng);
 pl_RNG_Destroy (rng);
 @endcode
 
 @defgroup PlankRNGClass Plank RNG class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %RNG</i> object. */
typedef struct PlankRNG* PlankRNGRef; 

PlankRNGRef pl_RNGGlobal();

/** Create and initialise a <i>Plank %RNG</i> object and return an oqaque reference to it.
 @return A <i>Plank %RNG</i> object as an opaque reference or NULL. */
PlankRNGRef pl_RNG_CreateAndInit();

/** Create a <i>Plank %RNG</i> object and return an oqaque reference to it.
@return A <i>Plank %RNG</i> object as an opaque reference or NULL. */
PlankRNGRef pl_RNG_Create();

/** Initialse a <i>Plank %RNG</i> object.
 @param p The <i>Plank %RNG</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_RNG_Init(PlankRNGRef p);

/** Deinitialse a <i>Plank %RNG</i> object.
 @param p The <i>Plank %RNG</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_RNG_DeInit(PlankRNGRef p);

/** Destroy a Plank RNG object. 
 @param p The <i>Plank %RNG</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_RNG_Destroy (PlankRNGRef p);

/** Seed a <i>Plank %RNG</i> object. 
 @param p The <i>Plank %RNG</i> object. 
 @param seed The new seed. */
void pl_RNG_Seed (PlankRNGRef p, unsigned int seed);

/** Generate a random integer. 
 @param p The <i>Plank %RNG</i> object. 
 @return The random integer. */
static int pl_RNG_Next (PlankRNGRef p);

/** Generate a random unsigned integer over a given range. 
 @param p The <i>Plank %RNG</i> object. 
 @param max Random numbers between 0 and max-1 will be generated. 
 @return The random unsigned integer. */
static unsigned int pl_RNG_NextInt (PlankRNGRef p, unsigned int max);

/** Generate a random float between 0 and 1. 
 @param p The <i>Plank %RNG</i> object. 
 @return The random float. */
static float pl_RNG_NextFloat (PlankRNGRef p);

/** Generate a random double between 0 and 1. 
 @param p The <i>Plank %RNG</i> object. 
 @return The random double. */
static double pl_RNG_NextDouble (PlankRNGRef p);

/// @} // End group PlankRNGClass

PLANK_END_C_LINKAGE

#define PLANK_INLINING_FUNCTIONS 1
#include "plank_RNGInline.h"
#undef PLANK_INLINING_FUNCTIONS

#endif // PLANK_RNG_H
