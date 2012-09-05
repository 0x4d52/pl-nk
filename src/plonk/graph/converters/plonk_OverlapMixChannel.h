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

#ifndef PLONK_OVERLAPMIXCHANNEL_H
#define PLONK_OVERLAPMIXCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


/** Output overlapped block. */
template<class SampleType>
class OverlapMixChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                               Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef OverlapMixChannelInternal<SampleType>                   OverlapMixInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    
    OverlapMixChannelInternal (Inputs const& inputs, 
                               Data const& data, 
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate),
        overlapMix (ChannelInternalCore::getInputAs<DoubleVariable> (IOKey::OverlapMix)),
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
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new OverlapMixChannelInternal (channelInputs, 
                                              this->getState(), 
                                              this->getBlockSize(), 
                                              this->getSampleRate());
    }    
    
    void initChannel (const int channel) throw()
    {                
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);

        this->setBlockSize (input.getBlockSize (channel));
        this->setSampleRate (input.getSampleRate (channel));
        
        this->initValue (SampleType (0));        
        
        tempBuffer.setSize (this->getBlockSize().getValue() * 2, false);
        tempBuffer.zero();
        tempBufferPos = 0;
        tempBufferStartOffset = 0;        
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {                
        /* Be careful optimising this with the new NumericalArray vector stuff */

        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        
        const int outputBufferLength = this->getOutputBuffer().length();
        SampleType* const outputSamples = this->getOutputSamples();        
        SampleType* const tempBufferSamples = this->tempBuffer.getArray();        

        const int overlapLength = int (this->overlapMix.getValue() * outputBufferLength + 0.5);
        int i;

        if (overlapLength < outputBufferLength)
        {
            const TimeStamp infoTimeStamp = info.getTimeStamp();
            
            // copy remaining overlap from last time to the output
            for (i = 0; i < outputBufferLength; ++i)
                outputSamples[i] = tempBufferSamples[tempBufferPos++];
                        
            // build the overlapping material in the temp buffer 
            tempBuffer.zero();
            tempBufferPos = tempBufferStartOffset;

            while (tempBufferPos < outputBufferLength)
            {
                const int tempBufferStartPos = tempBufferPos;
                
                info.setTimeStamp (nextInputTimeStamp);
                const Buffer& inputBuffer (inputUnit.process (info, channel));            
                nextInputTimeStamp = inputUnit.getNextTimeStamp (channel);
                
                const SampleType* const inputSamples = inputBuffer.getArray();
                const int inputBufferLength = inputBuffer.length();

                plonk_assert (inputBufferLength == outputBufferLength);
                                
                for (i = 0; i < inputBufferLength; ++i)
                    tempBufferSamples[tempBufferPos++] += inputSamples[i];
                
                tempBufferPos = tempBufferStartPos + overlapLength;
            }
            
            // in case the overlap is not an integer divsion of the block size
            tempBufferStartOffset = tempBufferPos - outputBufferLength;
            
            // accumulate the first part of the temp buffer to the output
            // leaving the rest there for next time
            tempBufferPos = 0;
            for (i = 0; i < outputBufferLength; ++i)
                outputSamples[i] += tempBufferSamples[tempBufferPos++];
                        
            info.setTimeStamp (infoTimeStamp); // reset for the parent graph        
        }
        else
        {
            const Buffer& inputBuffer (inputUnit.process (info, channel));            
            const SampleType* const inputSamples = inputBuffer.getArray();
            
            plonk_assert (outputBufferLength == inputBuffer.length());
            
            Buffer::copyData (outputSamples, inputSamples, outputBufferLength);

//            for (i = 0; i < outputBufferLength; ++i)
//                outputSamples[i] = inputSamples[i];
        }
    }
    
private:
    DoubleVariable overlapMix;
    Buffer tempBuffer;
    int tempBufferPos;
    int tempBufferStartOffset;
    TimeStamp nextInputTimeStamp;
};

//------------------------------------------------------------------------------

/** Mix down an overlapped processing stream to a continuous one. 
 
 Factory functions:
 - ar (input, overlap=0.5)
 
 Inputs:
 - input: (unit, multi) the unit to de-overlap
 - overlap: (doublevariable) the source overlap (1= is no overlap, 0.5= blocks overlap by half their length)

 
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
    
    static inline UnitInfos getInfo() throw()
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
        
        return UnitType::template createFromInputs<OverlapMixInternal> (inputs, 
                                                                        data, 
                                                                        BlockSize::noPreference(), 
                                                                        SampleRate::noPreference());
    }
    
    
};

typedef OverlapMixUnit<PLONK_TYPE_DEFAULT> OverlapMix;


#endif // PLONK_OVERLAPMIXCHANNEL_H
