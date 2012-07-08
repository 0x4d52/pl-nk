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

#ifndef PLONK_DELAYFORMCOMBFB_H
#define PLONK_DELAYFORMCOMBFB_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_DelayForwardDeclarations.h"

template<class SampleType>
class DelayFormCombFB
:   public DelayForm<SampleType, DelayFormType::CombFB, 2, 2>
{
public:
    typedef DelayForm<SampleType, DelayFormType::CombFB, 2, 2>      Base;
    
    enum InParams
    {
        DurationIn,
        FeedbackIn,
        NumInParams
    };
    
    enum OutParams
    {
        DurationInSamplesOut,
        FeedbackOut,
        NumOutParams
    };

    typedef typename Base::Data                                     Data;
    typedef typename Data::DelayState                               DelayState;
    typedef DelayFormCombFB                                         FormType;
    
    typedef SampleType                                              SampleDataType;
    typedef Delay2ParamChannelInternal<FormType>                    DelayInternal;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType             Param1Type;  
    typedef Param1Type                                              DurationType;    
    typedef UnitBase<DurationType>                                  DurationUnitType;

    typedef typename TypeUtility<SampleType>::IndexType             Param2Type;  
    typedef Param2Type                                              FeedbackType;    
    typedef UnitBase<FeedbackType>                                  FeedbackUnitType;
    
    typedef InterpLinear<SampleType,DurationType>                   InterpType;
    
    typedef void (*InputFunction)  (DelayState&);
    typedef void (*ReadFunction)   (DelayState&);
    typedef void (*WriteFunction)  (DelayState&);
    typedef void (*OutputFunction) (DelayState&);
    typedef void (*Param1Function) (DelayState&, Param1Type const&);
    typedef void (*Param2Function) (DelayState&, Param2Type const&);
    
    static inline IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration, IOKey::Feedback);
        return keys;
    }    
    
    static inline void inputIgnore (DelayState&) throw() { }
    static inline void inputRead (DelayState& data) throw()
    {
        data.inputValue = *data.inputSamples++;
    }
    
    static inline void readIgnore (DelayState&) throw() { }
    static inline void readRead (DelayState& data) throw()
    {
        DurationType readPosition = DurationType (data.writePosition) - data.paramsOut[DurationInSamplesOut];
        if (readPosition < data.buffer0)
            readPosition += data.bufferLengthIndex;
        plonk_assert (readPosition >= 0 && readPosition <= data.bufferLengthIndex);
        data.readValue = InterpType::lookup (data.bufferSamples, readPosition);
    }
    
    static inline void writeIgnore (DelayState&) throw() { }
    static inline void writeWrite (DelayState& data) throw()
    {
        plonk_assert (data.writePosition >= 0 && data.writePosition < data.bufferLength);
        data.writeValue = data.inputValue + data.paramsOut[FeedbackOut] * data.readValue;
        data.bufferSamples[data.writePosition] = data.writeValue;
        if (data.writePosition == 0)
            data.bufferSamples[data.bufferLength] = data.writeValue; // for interpolation
    }
    
    static inline void outputIgnore (DelayState&) throw() { }
    static inline void outputWrite (DelayState& data) throw()
    {
        data.outputValue = data.readValue;
        *data.outputSamples++ = data.outputValue;
        data.writePosition++;
    }
    
    static inline void param1Ignore (DelayState& data, DurationType const& duration) throw() { }
    static inline void param1Process (DelayState& data, DurationType const& duration) throw()
    {
        data.paramsIn[DurationIn] = duration;
        data.paramsOut[DurationInSamplesOut] = DurationType (duration * data.base.sampleRate);
        plonk_assert (data.paramsOut[DurationInSamplesOut] >= 0 && data.paramsOut[DurationInSamplesOut] <= data.bufferLengthIndex);
    }
    
    static inline void param2Ignore (DelayState& data, FeedbackType const& feedback) throw() { }
    static inline void param2Process (DelayState& data, FeedbackType const& feedback) throw()
    {                                
        data.paramsIn[FeedbackIn] = data.paramsOut[FeedbackOut] = feedback;
    }
    
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction>
    static inline void tick (DelayState& data) throw()
    {
        inputFunction (data);
        readFunction (data);
        writeFunction (data);
        outputFunction (data);
    }
    
    /** Create an audio rate wavetable oscillator. */
    static inline UnitType ar (UnitType const& input,
                               DurationUnitType const& duration,
                               FeedbackUnitType const& feedback,
                               const DurationType maximumDuration,
                               UnitType const& mul,
                               UnitType const& add,
                               BlockSize const& preferredBlockSize,
                               SampleRate const& preferredSampleRate) throw()
    {             
        const Data data = { { -1.0, -1.0 }, maximumDuration, 0 };
        
        const int numInputChannels = input.getNumChannels();
        const int numDurationChannels = duration.getNumChannels();
        const int numFeedbackChannels = feedback.getNumChannels();
        const int numChannels = plonk::max (numInputChannels, plonk::max (numDurationChannels, numFeedbackChannels));
        
        UnitType mainUnit = UnitType::emptyChannels (numChannels);
        
        for (int i = 0; i < numChannels; ++i)
        {
            Inputs inputs;
            inputs.put (IOKey::Generic, input[i]);
            inputs.put (IOKey::Duration, duration[i]);
            inputs.put (IOKey::Feedback, feedback[i]);
            
            UnitType unit = UnitType::template createFromInputs<DelayInternal> (inputs, 
                                                                                data, 
                                                                                preferredBlockSize, 
                                                                                preferredSampleRate);
            mainUnit.put (i, unit);
        }
        
        return UnitType::applyMulAdd (mainUnit, mul, add);
    }
};


//------------------------------------------------------------------------------


/** A comb filter setting the feedback amount directly. */
template<class SampleType>
class CombFBUnit
{
public:    
    typedef DelayFormCombFB<SampleType>             FormType;
    
    typedef Delay2ParamChannelInternal<FormType>    DelayInternal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
    typedef typename DelayInternal::Param1Type      DurationType;
    typedef UnitBase<DurationType>                  DurationUnitType;
    
    typedef typename DelayInternal::Param2Type      FeedbackType;
    typedef UnitBase<FeedbackType>                  FeedbackUnitType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("CombFB", "A comb filter setting the feedback amount directly.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,                IOLimit::None,                         
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Duration,           Measure::Seconds,   0.5,                IOLimit::Minimum,   Measure::Seconds,   0.0,
                         IOKey::Feedback,           Measure::Factor,    0.5,                IOLimit::None,
                         IOKey::MaximumDuration,    Measure::Seconds,   1.0,                IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate wavetable oscillator. */
    static UnitType ar (UnitType const& input,
                        DurationUnitType const& duration = DurationType (0.5),
                        FeedbackUnitType const& feedback = FeedbackType (0.5),
                        const DurationType maximumDuration = DurationType (1.0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        return FormType::ar (input, duration, feedback, maximumDuration, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
};

typedef CombFBUnit<PLONK_TYPE_DEFAULT> CombFB;


#endif // PLONK_DELAYFORMCOMBFB_H

