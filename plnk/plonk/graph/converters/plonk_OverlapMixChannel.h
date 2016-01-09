/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_OVERLAPMIXCHANNEL_H
#define PLONK_OVERLAPMIXCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


/** Output overlapped block. */
template<class SampleType>
class OverlapMixChannelInternal 
:   public ProxyOwnerChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                               Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ObjectArray<ChannelType>                                ChannelArrayType;
    typedef OverlapMixChannelInternal<SampleType>                   OverlapMixInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>              Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef ObjectArray<Buffer>                                     BufferArray;

    OverlapMixChannelInternal (Inputs const& inputs,
                               Data const& data, 
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate,
                               ChannelArrayType& channels) throw()
    :   Internal (inputs.getMaxNumChannels(), inputs, data, blockSize, sampleRate, channels),
        overlapMix (ChannelInternalCore::getInputAs<DoubleVariable> (IOKey::OverlapMix)),
        tempBuffers (BufferArray::withSize (inputs.getMaxNumChannels())),
        tempBufferPos (0),
        tempBufferStartOffset (0),
        nextInputTimeStamp (TimeStamp::getZero())
    {
    }
    
    Text getName() const throw()
    {
        return "Overlap Mix";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }
    
    void initChannel (const int channel) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        const SampleType sourceValue = input.getValue (channel);
        
        if ((channel % this->getNumChannels()) == 0)
        {
            // all need to be the same input BS and SR
            this->setBlockSize (input.getBlockSize (0));
            this->setSampleRate (input.getSampleRate (0));
            
            for (int i = 0; i < this->getNumChannels(); ++i)
            {
                tempBuffers.atUnchecked (i).setSize (this->getBlockSize().getValue() * 2, false);
                tempBuffers.atUnchecked (i).zero();
                tempBufferPos = 0;
                tempBufferStartOffset = 0;
            }
        }
                
        this->initProxyValue (channel, sourceValue);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {                
        /* Be careful optimising this with the new NumericalArray vector stuff */

        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        
        const int numChannels = this->getNumChannels();
        const int outputBufferLength = this->getOutputBuffer (0).length();
        const int overlapHop = int (this->overlapMix.getValue() * outputBufferLength + 0.5);
        const int overlapLength = outputBufferLength - overlapHop;
//        const int ovarlapRemain = plonk::max (overlapHop, overlapLength);
        int i, channel, channelBufferPos;

        if (overlapLength < outputBufferLength)
        {
            const TimeStamp infoTimeStamp = info.getTimeStamp();
            
            for (channel = 0; channel < numChannels; ++channel)
            {
                channelBufferPos = tempBufferPos;
                
                SampleType* const outputSamples = this->getOutputSamples (channel);
                SampleType* const tempBufferSamples = this->tempBuffers.atUnchecked (channel).getArray();

                // copy remaining overlap from last time to the output
                for (i = 0; i < outputBufferLength; ++i)
                    outputSamples[i] = tempBufferSamples[channelBufferPos++];
                    
                tempBuffers.atUnchecked (channel).zero();
            }
                        
            // build the overlapping material in the temp buffer 
            tempBufferPos = tempBufferStartOffset;

            while (tempBufferPos < outputBufferLength)
            {
                const int tempBufferStartPos = tempBufferPos;
                
                info.setTimeStamp (nextInputTimeStamp);
                
                for (channel = 0; channel < numChannels; ++channel)
                {
                    channelBufferPos = tempBufferPos;
                    
                    SampleType* const tempBufferSamples = this->tempBuffers.atUnchecked (channel).getArray();

                    const Buffer& inputBuffer (inputUnit.process (info, channel));
                    const SampleType* const inputSamples = inputBuffer.getArray();
                    const int inputBufferLength = inputBuffer.length();

                    plonk_assert (inputBufferLength == outputBufferLength);
                                    
                    for (i = 0; i < inputBufferLength; ++i)
                        tempBufferSamples[channelBufferPos++] += inputSamples[i];
                
                    channelBufferPos = tempBufferStartPos + overlapHop;
                }
                
                tempBufferPos = channelBufferPos;
                nextInputTimeStamp = inputUnit.getNextTimeStamp (0);
            }
            
            tempBufferStartOffset = tempBufferPos - outputBufferLength;
            
            // accumulate the first part of the temp buffer to the output
            // leaving the rest there for next time
            tempBufferPos = 0;
            
            for (channel = 0; channel < numChannels; ++channel)
            {
                channelBufferPos = tempBufferPos;

                SampleType* const outputSamples = this->getOutputSamples (channel);
                SampleType* const tempBufferSamples = this->tempBuffers.atUnchecked (channel).getArray();

                for (i = 0; i < outputBufferLength; ++i)
                    outputSamples[i] += tempBufferSamples[channelBufferPos++];
            }
            
            tempBufferPos = channelBufferPos;
            
            info.setTimeStamp (infoTimeStamp); // reset for the parent graph
        }
        else
        {
            for (channel = 0; channel < numChannels; ++channel)
            {
                SampleType* const outputSamples = this->getOutputSamples (channel);
                const Buffer& inputBuffer (inputUnit.process (info, channel));
                const SampleType* const inputSamples = inputBuffer.getArray();
                
                plonk_assert (outputBufferLength == inputBuffer.length());
                
                Buffer::copyData (outputSamples, inputSamples, outputBufferLength);
            }
        }
    }
    
private:
    DoubleVariable overlapMix;
    BufferArray tempBuffers;
    int tempBufferPos;
    int tempBufferStartOffset;
    TimeStamp nextInputTimeStamp;
};

//------------------------------------------------------------------------------

/** Mix down an overlapped processing stream to a continuous one. 
 
 @par Factory functions:
 - ar (input, overlap=0.5)
 
 @par Inputs:
 - input: (unit, multi) the unit to de-overlap
 - overlap: (doublevariable) the source overlap hop multiplier (1= is no overlap, 0.5= blocks overlap by half their length, 0.25= block overlap by 3/4 of their length)

 
 @ingroup ConverterUnits */
template<class SampleType>
class OverlapMixUnit
{
public:    
    typedef OverlapMixChannelInternal<SampleType>       OverlapMixInternal;
    typedef typename OverlapMixInternal::Data           Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;    
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {        
        const double minOverlap = TypeUtility<double>::getTypeEpsilon();
        
        return UnitInfo ("OverlapMix", "Mixes down incoming overlapped blocks into a continuous signal.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,         Measure::None,      IOInfo::NoDefault,   IOLimit::None,      IOKey::End,
                         
                         // inputs
                         IOKey::Generic,         Measure::None,      IOInfo::NoDefault,   IOLimit::None,
                         IOKey::OverlapMix,      Measure::Factor,    0.5,                 IOLimit::Clipped,   Measure::Factor,     minOverlap, 1.0,
                         IOKey::End);
    }    
    
    /**  */
    static UnitType ar (UnitType const& input, 
                        DoubleVariable const& overlap = Math<DoubleVariable>::get0_5()) throw()
    {                        
        plonk_assert (overlap.getValue() >= TypeUtility<double>::getTypeEpsilon());
        plonk_assert (overlap.getValue() <= 1.0);
        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::OverlapMix, overlap);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template proxiesFromInputs<OverlapMixInternal> (inputs,
                                                                         data,
                                                                         BlockSize::noPreference(),
                                                                         SampleRate::noPreference());
    }
    
    
};

typedef OverlapMixUnit<PLONK_TYPE_DEFAULT> OverlapMix;


#endif // PLONK_OVERLAPMIXCHANNEL_H
