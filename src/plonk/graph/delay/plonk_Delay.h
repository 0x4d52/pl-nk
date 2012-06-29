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

#ifndef PLONK_DELAY_H
#define PLONK_DELAY_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class DelayChannelInternal;

PLONK_CHANNELDATA_DECLARE(DelayChannelInternal,SampleType)
{        
    ChannelInternalCore::Data base;
    int writePosition;
};      

//------------------------------------------------------------------------------

/** Delay processor. */
template<class SampleType>
class DelayChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(DelayChannelInternal,SampleType)>
{
public:
    
    typedef PLONK_CHANNELDATA_NAME(DelayChannelInternal,SampleType)     Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef DelayChannelInternal<SampleType>                            DelayInternal;
    typedef ChannelInternal<SampleType,Data>                            Internal;
    typedef ChannelInternalBase<SampleType>                             InternalBase;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType     DurationType;
    typedef UnitBase<DurationType>                          DurationUnitType;
    typedef NumericalArray<DurationType>                    DurationBufferType;
    
    
    DelayChannelInternal (Inputs const& inputs, 
                          Data const& data, 
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Delay";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration, IOKey::Buffer);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new DelayInternal (channelInputs, 
                                  this->getState(), 
                                  this->getBlockSize(), 
                                  this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {        
        const UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        
        this->setBlockSize (BlockSize::decide (inputUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (inputUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (inputUnit.getOverlap (channel));
        
        this->initValue (SampleType (0));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {        
        Data& data = this->getState();
        const double sampleRate = data.base.sampleRate;
        const double sampleDuration = data.base.sampleDuration;
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();

        DurationUnitType& durationUnit = ChannelInternalCore::getInputAs<DurationUnitType> (IOKey::Duration);
        const DurationBufferType frequencyBuffer (durationUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const DurationType* const durationSamples = durationBuffer.getArray();
        const int durationBufferLength = durationBuffer.length();
        
        const Buffer& buffer (this->getInputAsBuffer (IOKey::Buffer));
        const SampleType* const bufferSamples = buffer.getArray();
        const int bufferLength = buffer.length();
        const DurationType bufferLengthIndex = DurationType (bufferLength);
        const DurationType buffer0 (0);
        const DurationType bufferLengthOverSampleRate = DurationType (bufferLength * sampleDuration); // was double, could be precision issue?
        
        int writePosition = data.writePosition;
        int i;
        
        plonk_assert (inputBufferLength == outputBufferLength);
        
        if (durationBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i) 
            {
                const DurationType durationInSamples = DurationType (durationSamples[i] * sampleRate);

                const SampleType inputValue = inputSamples[i];
                
                const DurationType readPosition = DurationType (writePosition) - durationInSamples;
                if (readPosition < buffer0)
                    readPosition += bufferLengthIndex;
                const SampleType delayedValue = plonk::lookup (buffer, readPosition);
                
                const SampleType writeValue = inputValue;
                bufferSamples[writePosition] = writeValue;
                
                const SampleType outputValue = delayedValue;
                
                outputSamples[i] = outputValue;
                
                writePosition++;
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }            
        }
        else if (durationBufferLength == 1)
        {
            const DurationType durationInSamples = DurationType (durationSamples[0] * sampleRate);
            
            plonk_assert (durationInSamples <= bufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                const SampleType inputValue = inputSamples[i];
                
                const DurationType readPosition = DurationType (writePosition) - durationInSamples;
                if (readPosition < buffer0)
                    readPosition += bufferLengthIndex;
                const SampleType delayedValue = plonk::lookup (buffer, readPosition);
                
                const SampleType writeValue = inputValue;
                bufferSamples[writePosition] = writeValue;
                
                const SampleType outputValue = delayedValue;
                
                outputSamples[i] = outputValue;
                
                writePosition++;
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }            
        }
        else
        {            
            double durationPosition = 0.0;
            const double durationIncrement = double (durationBufferLength) / double (outputBufferLength);

            for (i = 0; i < outputBufferLength; ++i) 
            {
                const DurationType durationInSamples = DurationType (durationSamples[int (durationPosition)] * sampleRate);
                
                const SampleType inputValue = inputSamples[i];
                
                const DurationType readPosition = DurationType (writePosition) - durationInSamples;
                if (readPosition < buffer0)
                    readPosition += bufferLengthIndex;
                const SampleType delayedValue = plonk::lookup (buffer, readPosition);
                
                const SampleType writeValue = inputValue;
                bufferSamples[writePosition] = writeValue;
                
                const SampleType outputValue = delayedValue;
                
                outputSamples[i] = outputValue;
                
                writePosition++;
                if (writePosition >= bufferLength)
                    writePosition = 0;
                
                durationPosition += durationIncrement;
            }            
        }
        
        data.writePosition = writePosition;
    }
};






#endif // PLONK_DELAY_H

