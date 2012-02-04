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


ChannelInternalCore::ChannelInternalCore (Inputs const& inputsToUse,
                                          BlockSize const& blockSizeToUse,
                                          SampleRate const& sampleRateToUse) throw()
:   lastTimeStamp (-1, 0.0),
    nextTimeStamp (TimeStamp::getZero()),
    expiryTimeStamp (TimeStamp::getMaximum()),
    inputs (inputsToUse),
    blockSize (blockSizeToUse),
    sampleRate (sampleRateToUse),
    overlap (inputs.containsKey (IOKey::OverlapMake) ? getInputAs<DoubleVariable> (IOKey::OverlapMake) : Math<DoubleVariable>::get1())
{
    cacheSampleDurationTicks();
    
    plonk_assert (blockSize.getValue() >= 0);
    plonk_assert (overlap.getValue() > 0.0);
    plonk_assert (overlap.getValue() <= 1.0);
}

void ChannelInternalCore::setNextTimeStamp (TimeStamp const& time) throw()
{
    plonk_assert ((nextTimeStamp.isInfinite() && time.isInfinite()) || (time > nextTimeStamp)); // gone backwards or wraparound
    nextTimeStamp = time;
}

void ChannelInternalCore::setLastTimeStamp (TimeStamp const& time) throw()
{
    plonk_assert (time.isFinite());
    lastTimeStamp = time;
}

void ChannelInternalCore::setExpiryTimeStamp (TimeStamp const& time) throw()
{
    expiryTimeStamp = time;
}

bool ChannelInternalCore::shouldBeDeletedNow (TimeStamp const& time) const throw()
{
    return time >= expiryTimeStamp;
}

double ChannelInternalCore::getBlockDurationInTicks() const throw()                          
{ 
    return this->getSampleDurationInTicks() * double (this->getBlockSize().getValue()) * overlap.getValue();
}

void ChannelInternalCore::updateTimeStamp() throw()
{
    if (this->lastTimeStamp >= TimeStamp::getZero()) // would like to avoid this condition..
    {
        const double blockDuration = this->getBlockDurationInTicks();
        this->setNextTimeStamp (this->lastTimeStamp + blockDuration);
    }
}

void ChannelInternalCore::setLabel (Text const& newId) throw()
{
    identifier = newId;
}

void ChannelInternalCore::setBlockSizeInternal (BlockSize const& newBlockSize) throw()
{
    blockSize = newBlockSize;
}

void ChannelInternalCore::setSampleRateInternal (SampleRate const& newSampleRate) throw()
{
    sampleRate = newSampleRate;
    cacheSampleDurationTicks();
}

void ChannelInternalCore::setOverlapInternal (DoubleVariable const& newOverlap) throw()
{
    overlap = newOverlap;
}

void ChannelInternalCore::cacheSampleDurationTicks() const throw()
{
    cachedSampleDurationTicks = TimeStamp::getTicks() / sampleRate.getValue(); 
}

TextArray ChannelInternalCore::getInputNames() const throw()
{
    return IOKey::collectNames (this->getInputKeys());
}

IntArray ChannelInternalCore::getInputTypes() const throw()
{
    return IOKey::collectTypes (this->getInputKeys());
}

TextArray ChannelInternalCore::getInputTypeNames() const throw()
{
    return IOKey::collectTypeNames (this->getInputKeys());
}

END_PLONK_NAMESPACE
