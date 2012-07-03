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

#ifndef PLONK_DELAYFORMS_H
#define PLONK_DELAYFORMS_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType, signed Form>
struct DelayFormData
{    
    typedef typename TypeUtility<SampleType>::IndexType DurationType;
    
    ChannelInternalCore::Data base;
    
    DurationType maximumDuration;
    int writePosition;
};      

//------------------------------------------------------------------------------

template<class SampleType, signed Form>
class DelayFormBase
{
public:    
    static Text getName() throw()
    {
        return DelayFormType::getName (Form);
    }        
};

template<class SampleType, signed Form>
class DelayForm : public DelayFormBase<SampleType, Form>
{
};

//------------------------------------------------------------------------------


template<class SampleType>
class DelayForm<SampleType, DelayFormType::Delay>
:   public DelayFormBase<SampleType, DelayFormType::Delay>
{
public:
    typedef SampleType                                          SampleDataType;
    typedef UnitBase<SampleType>                                UnitType;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef DelayFormData<SampleType, DelayFormType::Delay>     Data;
    
    typedef typename TypeUtility<SampleType>::IndexType     DurationType;
    typedef UnitBase<DurationType>                          DurationUnitType;
    typedef NumericalArray<DurationType>                    DurationBufferType;
    typedef InterpLinear<SampleType,DurationType>           InterpType;

    enum Coeffs
    {
        NumCoeffs = 0
    };
    
    static void process (SampleType* outputSamples,
                         const int outputBufferLength,
                         UnitType& inputUnit, 
                         DurationUnitType& durationUnit,
                         Buffer& buffer,
                         Data& data,
                         ProcessInfo& info, 
                         const int channel) throw()
    {
        const double sampleRate = data.base.sampleRate;
        
        const Buffer inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();
        
        const DurationBufferType durationBuffer (durationUnit.process (info, channel));
        const DurationType* durationSamples = durationBuffer.getArray();
        const int durationBufferLength = durationBuffer.length();
                
        SampleType* const bufferSamples = buffer.getArray();
        const int bufferLength = buffer.length();
        const DurationType bufferLengthIndex = DurationType (bufferLength);
        const DurationType buffer0 (0);
        
        int writePosition = data.writePosition;
        
        plonk_assert (inputBufferLength == outputBufferLength);
        int numSamplesToProcess = outputBufferLength;
        
        if (durationBufferLength == outputBufferLength)
        {            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = bufferLength - writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {
                    const DurationType durationValue = *durationSamples++;
                    const DurationType durationInSamples = DurationType (durationValue * sampleRate);
                    plonk_assert (durationInSamples <= bufferLength);

                    const SampleType inputValue = *inputSamples++;
                    
                    DurationType readPosition = DurationType (writePosition) - durationInSamples;
                    if (readPosition < buffer0)
                        readPosition += bufferLengthIndex;
                    const SampleType delayedValue = InterpType::lookup (bufferSamples, readPosition);
                    
                    const SampleType writeValue = inputValue;
                    bufferSamples[writePosition] = writeValue;
                    
                    const SampleType outputValue = delayedValue;
                    *outputSamples++ = outputValue;
                    
                    writePosition++;
                }
                
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }
        }
        else if (durationBufferLength == 1)
        {
            const DurationType durationInSamples = DurationType (durationSamples[0] * sampleRate);
            
            plonk_assert (durationInSamples <= bufferLength);
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = bufferLength - writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {
                    const SampleType inputValue = *inputSamples++;
                    
                    DurationType readPosition = DurationType (writePosition) - durationInSamples;
                    if (readPosition < buffer0)
                        readPosition += bufferLengthIndex;
                    const SampleType delayedValue = InterpType::lookup (bufferSamples, readPosition);
                    
                    const SampleType writeValue = inputValue;
                    bufferSamples[writePosition] = writeValue;
                    
                    const SampleType outputValue = delayedValue;
                    *outputSamples++ = outputValue;
                    
                    writePosition++;
                }

                if (writePosition >= bufferLength)
                    writePosition = 0;
            }
        }
        else
        {                        
            double durationPosition = 0.0;
            const double durationIncrement = double (durationBufferLength) / double (outputBufferLength);

            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = bufferLength - writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {
                    const DurationType durationValue = durationSamples[int (durationPosition)];
                    const DurationType durationInSamples = DurationType (durationValue * sampleRate);
                    plonk_assert (durationInSamples <= bufferLength);
                    
                    const SampleType inputValue = *inputSamples++;
                    
                    DurationType readPosition = DurationType (writePosition) - durationInSamples;
                    if (readPosition < buffer0)
                        readPosition += bufferLengthIndex;
                    const SampleType delayedValue = InterpType::lookup (bufferSamples, readPosition);
                    
                    const SampleType writeValue = inputValue;
                    bufferSamples[writePosition] = writeValue;
                    
                    const SampleType outputValue = delayedValue;
                    *outputSamples++ = outputValue;
                    
                    writePosition++;
                    durationPosition += durationIncrement;
                }
                
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }
        }
        
        data.writePosition = writePosition;
    }
};





#endif // PLONK_DELAYFORMS_H

