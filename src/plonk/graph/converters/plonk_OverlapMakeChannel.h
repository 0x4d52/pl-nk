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

#ifndef PLONK_OVERLAPMAKECHANNEL_H
#define PLONK_OVERLAPMAKECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


/** Output overlapped block. */
template<class SampleType>
class OverlapMakeChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                               Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef OverlapMakeChannelInternal<SampleType>                  OverlapMakeInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    
    OverlapMakeChannelInternal (Inputs const& inputs, 
                                Data const& data, 
                                BlockSize const& blockSize,
                                SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate),
        tempBufferPos (0),
        tempBufferFill (0),
        nextInputTimeStamp (TimeStamp::getZero())
    {
    }
    
    Text getName() const throw()
    {
        return "Overlap Make";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new OverlapMakeChannelInternal (channelInputs, 
                                               this->getState(), 
                                               this->getBlockSize(), 
                                               this->getSampleRate());
    }    
    
    void initChannel(const int channel) throw()
    {        
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        this->setBlockSize (input.getBlockSize (channel));
        this->setSampleRate (input.getSampleRate (channel));

        this->initValue (SampleType (0));        
        
        tempBuffer.setSize (this->getBlockSize().getValue() * 2, false);
        tempBuffer.zero();
        tempBufferPos = 0;
        tempBufferFill = 0;
    }    
        
    void process (ProcessInfo& info, const int channel) throw()
    {                
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
                
        const int outputBufferLength = this->getOutputBuffer().length();
        SampleType* const outputSamples = this->getOutputSamples();
        
        const int tempBufferLength = this->tempBuffer.length();
        SampleType* const tempBufferSamples = this->tempBuffer.getArray();
        
        const int overlapHop = int (this->getOverlap().getValue() * outputBufferLength + 0.5);
        int i;
        
        if (overlapHop < outputBufferLength)
        {
            const TimeStamp infoTimeStamp = info.getTimeStamp();
            
            // ensure we have enough buffered to output the buffer
            while ((tempBufferPos + outputBufferLength) >= tempBufferFill)
            {
                info.setTimeStamp (nextInputTimeStamp);
                const Buffer& inputBuffer (inputUnit.process (info, channel));
                nextInputTimeStamp = inputUnit.getNextTimeStamp (channel);
                
                const SampleType* const inputSamples = inputBuffer.getArray();
                const int inputBufferLength = inputBuffer.length();
                
                plonk_assert (outputBufferLength == inputBufferLength);
                
                for (i = 0; i < inputBufferLength; ++i)
                    tempBufferSamples[tempBufferFill++] = inputSamples[i];
                
                plonk_assert (tempBufferFill <= tempBufferLength);
            }
            
            // output one buffer
            for (i = 0; i < outputBufferLength; ++i)
                outputSamples[i] = tempBufferSamples[tempBufferPos++];
            
            // adjust the position pointer
            tempBufferPos -= outputBufferLength;
            tempBufferPos += overlapHop;
            
            // if we're over 1/2 way move the 2nd half to the start 
            // (the 2nd half will be re-filled next time if needed)
            if (tempBufferPos >= outputBufferLength)
            {
                tempBufferFill = 0;
                
                for (i = outputBufferLength; i < tempBufferLength; ++i)
                    tempBufferSamples[tempBufferFill++] = tempBufferSamples[i];
                
                tempBufferPos -= outputBufferLength;
            }
                        
            info.setTimeStamp (infoTimeStamp); // reset for the parent graph        
        }
        else
        {
            const Buffer& inputBuffer (inputUnit.process (info, channel));            
            const SampleType* const inputSamples = inputBuffer.getArray();
            
            plonk_assert (outputBufferLength == inputBuffer.length());
            
            for (i = 0; i < outputBufferLength; ++i)
                outputSamples[i] = inputSamples[i];
            
        }
    }
    
private:
    Buffer tempBuffer;
    int tempBufferPos;
    int tempBufferFill;
    TimeStamp nextInputTimeStamp;
};

//------------------------------------------------------------------------------

/** Create and overlapped processing stream.
 @ingroup ConverterUnits */
template<class SampleType>
class OverlapMakeUnit
{
public:    
    typedef OverlapMakeChannelInternal<SampleType>      OverlapMakeInternal;
    typedef typename OverlapMakeInternal::Data          Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;    
    
    static inline UnitInfos getInfo() throw()
    {        
        const double minOverlap = TypeUtility<double>::getTypeEpsilon();

        return UnitInfo ("OverlapMake", "Resamples an incoming signal into overlapping blocks.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,         Measure::None,      IOInfo::NoDefault,   IOLimit::None,      IOKey::End,
                         
                         // inputs
                         IOKey::Generic,         Measure::None,      IOInfo::NoDefault,   IOLimit::None,
                         IOKey::OverlapMake,     Measure::Factor,    0.5,                 IOLimit::Clipped,   Measure::Factor,     minOverlap, 1.0,
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
        inputs.put (IOKey::OverlapMake, overlap);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<OverlapMakeInternal> (inputs, 
                                                                         data, 
                                                                         BlockSize::noPreference(), 
                                                                         SampleRate::noPreference());
    }
        
};

typedef OverlapMakeUnit<PLONK_TYPE_DEFAULT> OverlapMake;



#endif // PLONK_OVERLAPMAKECHANNEL_H
