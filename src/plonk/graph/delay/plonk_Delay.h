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
        const IntArray keys (IOKey::Buffer, 
                             IOKey::Duration);
        return keys;
    }    
    
//    InternalBase* getChannel (const int index) throw()
//    {
//        const Inputs channelInputs = this->getInputs().getChannel (index);
//        return new TableInternal (channelInputs, 
//                                  this->getState(), 
//                                  this->getBlockSize(), 
//                                  this->getSampleRate());
//    }
    
//    void initChannel (const int channel) throw()
//    {        
//        const FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
//        
//        this->setBlockSize (BlockSize::decide (frequencyUnit.getBlockSize (channel),
//                                               this->getBlockSize()));
//        this->setSampleRate (SampleRate::decide (frequencyUnit.getSampleRate (channel),
//                                                 this->getSampleRate()));
//        
//        this->setOverlap (frequencyUnit.getOverlap (channel));
//        
//        this->initValue (this->getState().currentPosition);
//    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {        
        Data& data = this->getState();
        const double sampleRate = data.base.sampleRate;
        
        DurationUnitType& durationUnit = ChannelInternalCore::getInputAs<DurationUnitType> (IOKey::Duration);
        const DurationBufferType frequencyBuffer (durationUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const DurationType* const durationSamples = durationBuffer.getArray();
        const int durationBufferLength = durationBuffer.length();
        
        const Buffer& buffer (this->getInputAsBuffer (IOKey::Buffer));
        const SampleType* const bufferSamples = buffer.getArray();
        const DurationType bufferLength = DurationType (buffer.length());
        const DurationType buffer0 (0);
        const DurationType bufferLengthOverSampleRate = bufferLength / sampleRate; // was double, could be precision issue?
        
        int writePosition = data.writePosition;
        int i;
        
        if (durationBufferLength == outputBufferLength)
        {
//            for (i = 0; i < outputBufferLength; ++i) 
//            {
//                outputSamples[i] = plonk::lookup (tableSamples, currentPosition);
//                currentPosition += frequencySamples[i] * tableLengthOverSampleRate;
//                
//                if (currentPosition >= tableLength)
//                    currentPosition -= tableLength;
//                else if (currentPosition < table0)	
//                    currentPosition += tableLength;                
//            }                    
        }
        else if (durationBufferLength == 1)
        {
//            const FrequencyType valueIncrement (frequencySamples[0] * tableLengthOverSampleRate);
//            
//            if (valueIncrement > table0)
//            {
//                for (i = 0; i < outputBufferLength; ++i) 
//                {
//                    outputSamples[i] = plonk::lookup (tableSamples, currentPosition);
//                    currentPosition += valueIncrement;
//                    
//                    if (currentPosition >= tableLength)
//                        currentPosition -= tableLength;
//                }            
//            }
//            else
//            {
//                for (i = 0; i < outputBufferLength; ++i) 
//                {
//                    outputSamples[i] = plonk::lookup (tableSamples, currentPosition);
//                    currentPosition += valueIncrement;
//                    
//                    if (currentPosition < table0)	
//                        currentPosition += tableLength;                
//                }            
//            }
        }
        else
        {
//            double frequencyPosition = 0.0;
//            const double frequencyIncrement = double (frequencyBufferLength) / double (outputBufferLength);
//            
//            for (i = 0; i < outputBufferLength; ++i) 
//            {
//                outputSamples[i] = plonk::lookup (tableSamples, currentPosition);
//                currentPosition += frequencySamples[int (frequencyPosition)] * tableLengthOverSampleRate;
//                
//                if (currentPosition >= tableLength)
//                    currentPosition -= tableLength;
//                else if (currentPosition < table0)	
//                    currentPosition += tableLength;                
//                
//                frequencyPosition += frequencyIncrement;
//            }        
        }
        
        data.writePosition = writePosition;
    }
};






#endif // PLONK_DELAY_H

