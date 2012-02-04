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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"


RNGInternal::RNGInternal() throw()
{
    pl_RNG_Init (&rng);
}

RNGInternal::~RNGInternal()
{
    pl_RNG_DeInit (&rng);
}

//------------------------------------------------------------------------------

RNG::RNG() throw()
:   Base (new Internal())
{
}

RNG::RNG (RNG const& copy) throw()
:   Base (static_cast<Base const&> (copy))
{
}

RNG& RNG::operator= (RNG const& other) throw()
{
    if (this != &other)
        this->setInternal (other.containerCopy().getInternal());
    
    return *this;
}

RNG& RNG::global() throw()
{
    static RNG random;
    return random;
}

RNG& RNG::audio() throw()
{
    static RNG random;
    return random;
}

void RNG::seed (const unsigned int value) throw()
{
    pl_RNG_Seed (this->getInternal()->getRNGRef(), value);
}

unsigned int RNG::uniformInt() throw()
{
    return pl_RNG_Next (this->getInternal()->getRNGRef());
}

unsigned int RNG::uniform (const unsigned int max) throw()
{
    return pl_RNG_NextInt (this->getInternal()->getRNGRef(), max);
}

unsigned int RNG::uniform (const unsigned int min, 
                           const unsigned int max) throw()
{
    plonk_assert (max > min);    
    return pl_RNG_NextInt (this->getInternal()->getRNGRef(), max - min) + min;
}

int RNG::uniform (const int max) throw()
{
    plonk_assert (max > 0);
    return pl_RNG_NextInt (this->getInternal()->getRNGRef(), max);
}

int RNG::uniform (const int min, const int max) throw()
{
    plonk_assert (min >= 0);
    plonk_assert (max > 0);
    plonk_assert (max > min);    
    return pl_RNG_NextInt (this->getInternal()->getRNGRef(), max - min) + min;
}

float RNG::uniformFloat() throw()
{
    return pl_RNG_NextFloat (this->getInternal()->getRNGRef());
}

float RNG::uniform (const float max) throw()
{
    return pl_RNG_NextFloat (this->getInternal()->getRNGRef()) * max;
}

float RNG::uniform (const float min, const float max) throw()
{
    return plonk::linlin (pl_RNG_NextFloat (this->getInternal()->getRNGRef()), 
                          0.f, 1.f, 
                          min, max);
}

double RNG::uniformDouble() throw()
{
    return pl_RNG_NextDouble (this->getInternal()->getRNGRef());
}

double RNG::uniform (const double max) throw()
{
    return pl_RNG_NextDouble (this->getInternal()->getRNGRef()) * max;
}

double RNG::uniform (const double min, const double max) throw()
{
    return plonk::linlin (pl_RNG_NextDouble (this->getInternal()->getRNGRef()), 
                          0.0, 1.0, 
                          min, max);
}

float RNG::exponential (const float min, const float max) throw()
{
    return plonk::linexp (pl_RNG_NextFloat (this->getInternal()->getRNGRef()), 
                          0.f, 1.f, 
                          min, max);
}

double RNG::exponential (const double min, const double max) throw()
{
    return plonk::linexp (pl_RNG_NextDouble (this->getInternal()->getRNGRef()), 
                          0.0, 1.0, 
                          min, max);
}



END_PLONK_NAMESPACE
