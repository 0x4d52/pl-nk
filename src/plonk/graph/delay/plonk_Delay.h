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


/** Delay processor. */
template<class FormType>
class DelayBaseChannelInternal 
:   public ProxyOwnerChannelInternal<typename FormType::SampleDataType, 
                                     typename FormType::Data>
{
public:
    typedef typename FormType::SampleDataType               SampleType;
    typedef typename FormType::Data                         Data;
    
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef DelayBaseChannelInternal<FormType>              DelayInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>      Internal;
    typedef ChannelInternalBase<SampleType>                 InternalBase;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType     DurationType;
    typedef UnitBase<DurationType>                          DurationUnitType;
    typedef NumericalArray<DurationType>                    DurationBufferType;
    typedef InterpLinear<SampleType,DurationType>           InterpType;
    
    DelayBaseChannelInternal (Inputs const& inputs, 
                              Data const& data, 
                              BlockSize const& blockSize,
                              SampleRate const& sampleRate) throw()
    :   Internal (1, inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Delay";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration);
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
        
        Data& data = this->getState();
        this->circularBuffer = Buffer::newClear (int (data.maximumDuration * data.base.sampleRate + 0.5));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        Data& data = this->getState();
        const int outputBufferLength = this->getOutputBuffer (0).length();
        
        UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        const Buffer inputBuffer (inputUnit.process (info, channel));
        data.inputSamples = inputBuffer.getArray();
   
        plonk_assert (inputBuffer.length() == outputBufferLength);

        FormType::process (this->getOutputSamples (0),
                           outputBufferLength, 
                           this->getInputAsUnit (IOKey::Duration), 
                           this->circularBuffer,
                           data,
                           info, 
                           channel);        
    }
        
private:
    Buffer circularBuffer;
};

//------------------------------------------------------------------------------

/** Delay processor. */
template<class FormType>
class DelayBaseUnit
{
public:    
    typedef typename FormType::SampleDataType       SampleType;

    typedef DelayBaseChannelInternal<FormType>      DelayInternal;
    typedef typename DelayInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    
    typedef typename DelayInternal::DurationType         DurationType;
    typedef typename DelayInternal::DurationUnitType     DurationUnitType;
    typedef typename DelayInternal::DurationBufferType   DurationBufferType;
    
//    static inline UnitInfos getInfo() throw()
//    {
//        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();
//        
//        return UnitInfo ("Table", "A wavetable oscillator.",
//                         
//                         // output
//                         ChannelCount::VariableChannelCount, 
//                         IOKey::Generic,    Measure::None,      0.0,        IOLimit::None,
//                         IOKey::End,
//                         
//                         // inputs
//                         IOKey::Wavetable,  Measure::None,
//                         IOKey::Frequency,  Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
//                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
//                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
//                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
//                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
//                         IOKey::End);
//    }
    
    /** Create an audio rate wavetable oscillator. */
    static UnitType ar (UnitType const& input,
                        DurationUnitType const& duration = DurationType (0.5),
                        const DurationType maximumDuration = DurationType (1.0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Duration, duration);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
        
        Data data = { { -1.0, -1.0 }, maximumDuration, 0 };
        
        //proxiesFromInputs // <--- can use this anyway for just one channel...
        return UnitType::template createFromInputs<DelayInternal> (inputs, 
                                                                   data, 
                                                                   preferredBlockSize, 
                                                                   preferredSampleRate);
    }
};

typedef DelayForm<PLONK_TYPE_DEFAULT,DelayFormType::Delay> DelayFormDelay;
typedef DelayBaseUnit<DelayFormDelay> Delay;







#endif // PLONK_DELAY_H

