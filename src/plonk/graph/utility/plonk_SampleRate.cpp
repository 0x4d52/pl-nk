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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"


SampleRate::SampleRate() throw()
:   Base (-1.0)
{
}

SampleRate::SampleRate (BlockSize const& blockSize) throw()
:   Base (-1.0)
{
    // you must have got your arguments mixed up, perhaps you got
    // sample rate and block size arguments the wrong way round?
    (void)blockSize;
    plonk_assertfalse;
}

SampleRate::SampleRate (const double initValue) throw()
:   Base (initValue)
{
}

SampleRate::SampleRate (DoubleVariable const& initValue) throw()
:   Base (initValue)
{
}

SampleRate::SampleRate (SampleRate const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}        

const double SampleRate::getSampleDurationInTicks() const throw()
{
    return TimeStamp::getTicks() / this->getValue();
}

SampleRate SampleRate::getDefault() throw()
{
    static SampleRate sampleRate (44100.0);
    return sampleRate;
}

SampleRate SampleRate::getControlRate() throw()
{
    static SampleRate sampleRate ((getDefault() / BlockSize::getDefault()) * BlockSize::getControlRateBlockSize());   
    return sampleRate;
}

SampleRate SampleRate::getDefaultBlockRate() throw()
{
    static SampleRate sampleRate (getDefault() / BlockSize::getDefault());
    return sampleRate;
}

const SampleRate SampleRate::getFractionOfDefault(IntVariable const& divisor) throw()
{
    plonk_assert (divisor.getValue() > 0);
    return getDefault() / divisor;
}

const SampleRate SampleRate::getMultipleOfDefault(IntVariable const& factor) throw()
{
    return getDefault() * factor;
}

const SampleRate SampleRate::getMultipleOfDefault(DoubleVariable const& factor) throw()
{
    return getDefault() * factor;
}

const SampleRate& SampleRate::noPreference() throw()
{
    static const SampleRate sentinel; // -1.0
    return sentinel;
}

const SampleRate& SampleRate::getSampleRate0() throw()
{
    static const SampleRate sampleRate (0.0);
    return sampleRate;
}

SampleRate SampleRate::decide (SampleRate const& inputSampleRate, 
                               SampleRate const& preferredSampleRate) throw()
{
    SampleRate sampleRate ((preferredSampleRate.getValue() < 0.0) ? inputSampleRate : preferredSampleRate);
    return (sampleRate.getValue() < 0.0) ? getDefault() : sampleRate;
}



END_PLONK_NAMESPACE
