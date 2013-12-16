/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#ifndef PLONK_IMPULSES_H
#define PLONK_IMPULSES_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"
#include "plonk_Saw.h"

template<class SampleType> class ImpulseChannelInternal;

PLONK_CHANNELDATA_DECLARE(ImpulseChannelInternal,SampleType)
{
    typedef typename TypeUtility<SampleType>::IndexType FrequencyType;
    
    ChannelInternalCore::Data base;
    
    FrequencyType currentValue;
    LongLong peak;
    LongLong peak2peak;
};

PLONK_CHANNELDATA_SPECIAL(ImpulseChannelInternal,float)
{
    ChannelInternalCore::Data base;
    
    float currentValue;
    float peak;
    float peak2peak;
};

PLONK_CHANNELDATA_SPECIAL(ImpulseChannelInternal,double)
{
    ChannelInternalCore::Data base;
    
    double currentValue;
    double peak;
    double peak2peak;
};

PLONK_CHANNELDATA_SPECIAL(ImpulseChannelInternal,short)
{
    typedef TypeUtility<short>::IndexType FrequencyType;
    
    ChannelInternalCore::Data base;
    
    FrequencyType currentValue;
    short peak;
    int peak2peak;
};

PLONK_CHANNELDATA_SPECIAL(ImpulseChannelInternal,int)
{
    typedef TypeUtility<int>::IndexType FrequencyType;
    
    ChannelInternalCore::Data base;
    
    FrequencyType currentValue;
    int peak;
    LongLong peak2peak;
};

//------------------------------------------------------------------------------

/** Impulse generator. */
template<class SampleType>
class ImpulseChannelInternal
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(ImpulseChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(ImpulseChannelInternal,SampleType)   Data;
    typedef InputDictionary                                             Inputs;
    
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef ImpulseChannelInternal<SampleType>                  ImpulseInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    
    typedef typename TypeUtility<SampleType>::IndexType         FrequencyType;
    typedef UnitBase<FrequencyType>                             FrequencyUnitType;
    typedef NumericalArray<FrequencyType>                       FrequencyBufferType;
    
    ImpulseChannelInternal (Inputs const& inputs,
                            Data const& data,
                            BlockSize const& blockSize,
                            SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Impulse";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Frequency);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new ImpulseInternal (channelInputs,
                                    this->getState(),
                                    this->getBlockSize(),
                                    this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {
        const FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        
        this->setBlockSize (BlockSize::decide (frequencyUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (frequencyUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (frequencyUnit.getOverlap (channel));
        
        this->initValue (SampleType (1));
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        Data& data = this->getState();
        const double sampleDuration = data.base.sampleDuration;
        const double factor = data.peak2peak * sampleDuration;
        
        FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        
        const FrequencyBufferType& frequencyBuffer (frequencyUnit.process (info, channel));
        const FrequencyType* const frequencySamples = frequencyBuffer.getArray();
        const int frequencyBufferLength = frequencyBuffer.length();
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        int i;
        
        if (frequencyBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i)
            {
                if (data.currentValue >= data.peak)
                {
                    data.currentValue -= data.peak2peak;
                    outputSamples[i] = data.peak;
                }
                else
                {
                    outputSamples[i] = SampleType (0);
                }

                data.currentValue += frequencySamples[i] * factor;                
            }
        }
        else if (frequencyBufferLength == 1)
        {
            const FrequencyType valueIncrement (frequencySamples[0] * factor);
            
            for (i = 0; i < outputBufferLength; ++i)
            {
                if (data.currentValue >= data.peak)
                {
                    data.currentValue -= data.peak2peak;
                    outputSamples[i] = data.peak;
                }
                else
                {
                    outputSamples[i] = SampleType (0);
                }

                data.currentValue += valueIncrement;
            }
        }
        else
        {
            double frequencyPosition = 0.0;
            const double frequencyIncrement = double (frequencyBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i)
            {
                if (data.currentValue >= data.peak)
                {
                    data.currentValue -= data.peak2peak;
                    outputSamples[i] = data.peak;
                }
                else
                {
                    outputSamples[i] = SampleType (0);
                }
                
                frequencyPosition += frequencyIncrement;
                data.currentValue += frequencySamples[int (frequencyPosition)] * factor;
            }
        }
    }
    
private:
};



//------------------------------------------------------------------------------

/** An impulse generator. 
 
 @par Factory functions:
 - ar (frequency=440, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - kr (frequency=440, mul=1, add=0) 
 
 @par Inputs:
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset aded to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @ingroup GeneratorUnits ControlUnits */
template<class SampleType>
class ImpulseUnit
{
public:    
    typedef ImpulseChannelInternal<SampleType>              ImpulseInternal;
    typedef typename ImpulseInternal::Data                  Data;
    typedef InputDictionary                                 Inputs;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef UnitBase<SampleType>                            UnitType;
    
    typedef typename ImpulseInternal::FrequencyType         FrequencyType;
    typedef typename ImpulseInternal::FrequencyUnitType     FrequencyUnitType;
    typedef typename ImpulseInternal::FrequencyBufferType   FrequencyBufferType;

    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        const double peak = (double)TypeUtility<SampleType>::getTypePeak(); // will be innaccurate for LongLong
        
        return UnitInfo ("Impulse", "An impulse generator.",
                         
                         // output
                         ChannelCount::VariableChannelCount,
                         IOKey::Generic,    Measure::None,      0.0,        IOLimit::Clipped,   Measure::NormalisedBipolar, -peak, peak,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Frequency,  Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate impulse oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        Inputs inputs;
        inputs.put (IOKey::Frequency, frequency);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
        
        const LongLong peak = SampleType (TypeUtility<SampleType>::getTypePeak());
        
        Data data = { { -1.0, -1.0 }, 0, peak, peak * 2 };
        
        return UnitType::template createFromInputs<ImpulseInternal> (inputs,
                                                                     data,
                                                                     preferredBlockSize,
                                                                     preferredSampleRate);
    }
    
    /** Create a control rate impulse oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        return ar (frequency, mul, add,
                   BlockSize::getControlRateBlockSize(),
                   SampleRate::getControlRate());
    }
};

typedef ImpulseUnit<PLONK_TYPE_DEFAULT> Impulse;


#endif // PLONK_IMPULSES_H

