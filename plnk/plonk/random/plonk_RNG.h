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

#ifndef PLONK_RNG_H
#define PLONK_RNG_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "../containers/plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"


class RNGInternal : public SmartPointer
{
public:
    RNGInternal() throw();
    ~RNGInternal();
    
    friend class RNG;
    
private:
    PLONK_INLINE_HIGH PlankRNGRef getRNGRef() { return &rng; }

    PlankRNG rng;
};

/** Random number generator. 
 @ingroup PlonkOtherUserClasses */
class RNG : SmartPointerContainer<RNGInternal>
{
public:
    typedef RNGInternal                         Internal;
    typedef SmartPointerContainer<RNGInternal>  Base;
    
    RNG() throw();
    RNG (RNG const& copy) throw();
    RNG& operator= (RNG const& other) throw();

    /** Get the global random number generator. */
    static RNG& global() throw();
    
    /** Get the audio thread random number generator. 
     NB This will be no longer sufficient when multicore. */
    static RNG& audio() throw();
    
    /** Seed this random number generator. */
    void seed (const unsigned int value) throw();
    
    /** Generate a random integer. */
    unsigned int uniformInt() throw();
    
    /** Generate a uniformly distributed random integer between 0 and max-1. */
    unsigned int uniform (const unsigned int max) throw();
    
    /** Generate a uniformly distributed random integer between min and max-1. */
    unsigned int uniform (const unsigned int min, const unsigned int max) throw();
    
    /** Generate a uniformly distributed random integer between 0 and max-1. */
    int uniform (const int max) throw();
    
    /** Generate a uniformly distributed random integer between min and max-1. */
    int uniform (const int min, const int max) throw();
    
    /** Generate a uniformly distributed random float between 0 and 1. */
    float uniformFloat() throw();
    
    /** Generate a uniformly distributed random float between 0 and max. */
    float uniform (const float max) throw();
    
    /** Generate a uniformly distributed random float between min and max. */
    float uniform (const float min, const float max) throw();
    
    /** Generate a uniformly distributed random double between 0 and 1. */
    double uniformDouble() throw();
    
    /** Generate a uniformly distributed random double between 0 and max. */
    double uniform (const double max) throw();
    
    /** Generate a uniformly distributed random double between min and max. */
    double uniform (const double min, const double max) throw();
    
    /** Generate a exponentially distributed random float between min and max. */
    float exponential (const float min, const float max) throw();
    
    /** Generate a exponentially distributed random double between min and max. */
    double exponential (const double min, const double max) throw();     
    
    PLONK_OBJECTARROWOPERATOR(RNG);
};

//------------------------------------------------------------------------------

PLONK_INLINE_HIGH double rand (const double scale) throw()
{
	return RNG::global().uniform (scale);
}

PLONK_INLINE_HIGH double rand (const double min, const double max) throw()
{
	return RNG::global().uniform (min, max);
}

PLONK_INLINE_HIGH double rand2 (const double scale) throw()
{
	return RNG::global().uniform (scale);
}

PLONK_INLINE_HIGH double exprand (const double min, const double max) throw()
{
	return RNG::global().exponential (min, max);
}

PLONK_INLINE_HIGH float rand (const float scale) throw()
{
	return RNG::global().uniform (scale);
}

PLONK_INLINE_HIGH float rand (const float min, const float max) throw()
{
	return RNG::global().uniform (min, max);
}

PLONK_INLINE_HIGH float rand2 (const float scale) throw()
{
	return RNG::global().uniform (-scale, scale);
}

PLONK_INLINE_HIGH float exprand (const float min, const float max) throw()
{
	return RNG::global().exponential (min, max);
}

PLONK_INLINE_HIGH int rand (const int scale) throw()
{
	return RNG::global().uniform (scale);
}

PLONK_INLINE_HIGH int rand (const int min, const int max) throw()
{
	return RNG::global().uniform (min, max);
}

PLONK_INLINE_HIGH int rand2 (const int scale) throw()
{
	return RNG::global().uniform (-scale, scale);
}

PLONK_INLINE_HIGH Int24 rand (const Int24 scale) throw()
{
	return RNG::global().uniform (int (scale));
}

PLONK_INLINE_HIGH Int24 rand (const Int24 min, const Int24 max) throw()
{
	return RNG::global().uniform (int (min), int (max));
}

PLONK_INLINE_HIGH Int24 rand2 (const Int24 scale) throw()
{
    const int intScale (scale);
	return RNG::global().uniform (-intScale, intScale);
}

PLONK_INLINE_HIGH double exprand (const int min, const int max) throw()
{
	return RNG::global().exponential (double (min), double (max));
}


#endif // PLONK_RNG_H
