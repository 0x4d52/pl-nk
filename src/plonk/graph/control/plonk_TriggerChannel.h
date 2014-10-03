/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_TRIGGERCHANNEL_H
#define PLONK_TRIGGERCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class TriggerChannelInternal;

PLONK_CHANNELDATA_DECLARE(TriggerChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;
    
    int durationRemainingInSamples;
    SampleType prevValue;
};

/** Trigger. */
template<class SampleType>
class TriggerChannelInternal
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(TriggerChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(TriggerChannelInternal,SampleType)   Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef TriggerChannelInternal<SampleType>                          TriggerInternal;
    typedef ChannelInternal<SampleType,Data>                            Internal;
    typedef ChannelInternalBase<SampleType>                             InternalBase;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
    typedef typename TypeUtility<SampleType>::IndexType                 IndexType;
    
    typedef typename TypeUtility<SampleType>::IndexType                 DurationType;
    typedef UnitBase<DurationType>                                      DurationUnitType;
    typedef NumericalArray<DurationType>                                DurationBufferType;
        
    TriggerChannelInternal (Inputs const& inputs, 
                            Data const& data,
                            BlockSize const& blockSize,
                            SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Trigger";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new TriggerInternal (channelInputs, 
                                    this->getState(), 
                                    this->getBlockSize(),
                                    this->getSampleRate());
    }    
    
    void initChannel (const int channel) throw()
    {        
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);

        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getSampleRate (channel),
                                                 this->getSampleRate()));
        this->setOverlap (input.getOverlap (channel));
    
        this->initValue (0);
    }    
        
    void process (ProcessInfo& info, const int channel) throw()
    {
        Data& data = this->getState();
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();

        DurationUnitType& durationUnit = ChannelInternalCore::getInputAs<DurationUnitType> (IOKey::Duration);
        const DurationBufferType& durationBuffer (durationUnit.process (info, channel));
        const DurationType* durationSamples = durationBuffer.getArray();
        const int durationBufferLength = durationBuffer.length();

        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        SampleType prevValue = data.prevValue;
        DurationType durationRemainingInSamples = data.durationRemainingInSamples;
        
        const SampleType sampleZero = Math<SampleType>::get0();
        const SampleType sampleOne = Math<SampleType>::get1();

        int i;
        
        if (inputBufferLength == outputBufferLength)
        {
            if (inputBufferLength == durationBufferLength)
            {
                for (i = 0; i < outputBufferLength; ++i)
                {
                    const SampleType currValue = inputSamples[i];
                    
                    if (prevValue <= sampleZero && currValue > sampleZero)
                    {
                        durationRemainingInSamples = plonk::max (1, int (durationSamples[i] * data.base.sampleRate));
                        this->update (Text::getMessageTrigger(), Dynamic::getNull());
                    }
                    
                    outputSamples[i] = durationRemainingInSamples > 0 ? sampleOne : sampleZero;
                        
                    --durationRemainingInSamples;
                    prevValue = currValue;
                }
            }
            else // if (durationBufferLength == 1)
            {
                const DurationType durationInSamples = plonk::max (1, int (durationSamples[0] * data.base.sampleRate));
                
                for (i = 0; i < outputBufferLength; ++i)
                {
                    const SampleType currValue = inputSamples[i];
                    
                    if (prevValue <= sampleZero && currValue > sampleZero)
                    {
                        durationRemainingInSamples = durationInSamples;
                        this->update (Text::getMessageTrigger(), Dynamic::getNull());
                    }
                    
                    outputSamples[i] = durationRemainingInSamples > 0 ? sampleOne : sampleZero;
                    
                    --durationRemainingInSamples;
                    prevValue = currValue;
                }
            }
        }
        else
        {
            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
            
            double durationPosition = 0.0;
            const double durationIncrement = double (durationBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i)
            {
                const SampleType currValue = inputSamples[int (inputPosition)];
                
                if (prevValue <= sampleZero && currValue > sampleZero)
                {
                    durationRemainingInSamples = plonk::max (1, int (durationSamples[int (durationPosition)] * data.base.sampleRate));
                    this->update (Text::getMessageTrigger(), Dynamic::getNull());
                }
                
                outputSamples[i] = durationRemainingInSamples > 0 ? sampleOne : sampleZero;
                    
                --durationRemainingInSamples;
                inputPosition += inputIncrement;
                durationPosition += durationIncrement;
                prevValue = currValue;
            }

        }
        
        // prevent overflow
        if (durationRemainingInSamples < 0)
            durationRemainingInSamples = 0;
        
        data.prevValue = prevValue;
        data.durationRemainingInSamples = durationRemainingInSamples;
    }
};

//------------------------------------------------------------------------------

/** Trigger detects signal transitions from zero or less to greater than zero.
 
 Trigger outputs 1 for the specified duration when the trigger is detected. Then 
 goes back to 0.
 
 @par Factory functions:
 - ar (input, duration=0.005, preferredBlockSize=default, preferredSampleRate=default)
 - kr (input, duration=0.005)
 
 @par Inputs:
 - input: (unit, multi) the input unit within which triggers are detected
 - duration: (unit, multi) the durations of the output trigger signal
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

 @ingroup ConverterUnits */
template<class SampleType>
class TriggerUnit
{
public:    
    typedef TriggerChannelInternal<SampleType>              TriggerInternal;
    typedef typename TriggerInternal::Data                  Data;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;

    typedef typename TriggerInternal::DurationType          DurationType;
    typedef typename TriggerInternal::DurationUnitType      DurationUnitType;
    typedef typename TriggerInternal::DurationBufferType    DurationBufferType;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Trigger", "Trigger detects signal transitions from zero or less to greater than zero.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,      IOKey::End,

                         // inputs
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::Duration,   Measure::Seconds,  0.005,               IOLimit::Minimum,   Measure::Seconds,   0.0,
                         IOKey::BlockSize,  Measure::Samples,  blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate, Measure::Hertz,    sampleRate,          IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate sample rate converter. */
    static UnitType ar (UnitType const& input,
                        DurationUnitType const& duration = DurationType (0.005),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {                                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Duration, duration);
        
        Data data = { { -1.0, -1.0 }, 0, 0 };
                
        return UnitType::template createFromInputs<TriggerInternal> (inputs, 
                                                                     data,
                                                                     preferredBlockSize,
                                                                     preferredSampleRate);
    }
    
    static PLONK_INLINE_LOW UnitType kr (UnitType const& input,
                               DurationUnitType const& duration = DurationType (0.005)) throw()
    {
        return ar (input,
                   duration,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
    
};

typedef TriggerUnit<PLONK_TYPE_DEFAULT> Trigger;



#endif // PLONK_TRIGGERCHANNEL_H


