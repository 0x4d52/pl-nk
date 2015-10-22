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

// help prevent accidental inclusion other than via the intended header
#if PLANK_INLINING_FUNCTIONS

#if !DOXYGEN
typedef struct PlankRNG
{
    unsigned int value;
} PlankRNG;
#endif

#define PLANK_RNG_MAGIC1            1664525
#define PLANK_RNG_MAGIC2            1013904223
#define PLANK_RNG_FLOAT_ONE         PLANK_FLOAT_ONE
#define PLANK_RNG_FLOAT_ONEMASK     PLANK_FLOAT_ONEMASK
#define PLANK_RNG_DOUBLE_ONE        PLANK_DOUBLE_ONE
#define PLANK_RNG_DOUBLE_ONEMASK    PLANK_DOUBLE_ONEMASK

static PLANK_INLINE_LOW int pl_RNG_Next (PlankRNGRef p)
{
    p->value = (unsigned int)PLANK_RNG_MAGIC1 * p->value + (unsigned int)PLANK_RNG_MAGIC2;
    return p->value;
}

static PLANK_INLINE_LOW unsigned int pl_RNG_NextInt (PlankRNGRef p, unsigned int max)
{
    unsigned int value = pl_RNG_Next (p);
    return value % max;
}

static PLANK_INLINE_LOW float pl_RNG_NextFloat (PlankRNGRef p)
{    
    unsigned int bits;
	float value;
	bits = PLANK_RNG_FLOAT_ONE | (PLANK_RNG_FLOAT_ONEMASK & pl_RNG_Next (p));
    value = *(float*)(&bits);
    return value - 1.f;
}

static PLANK_INLINE_LOW double pl_RNG_NextDouble (PlankRNGRef p)
{    
    unsigned int bits;
	float value;
	bits = PLANK_RNG_FLOAT_ONE | (PLANK_RNG_FLOAT_ONEMASK & pl_RNG_Next (p));
    value = *(float*)(&bits);
    return (double)value - 1.0;
}

#endif
