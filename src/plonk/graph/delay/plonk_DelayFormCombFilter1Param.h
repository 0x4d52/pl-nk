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

#ifndef PLONK_DELAYFORMCOMBFILTER1PARAM_H
#define PLONK_DELAYFORMCOMBFILTER1PARAM_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_DelayForwardDeclarations.h"

template<class FilterShape, Interp::TypeCode InterpTypeCode>
class DelayFormCombFilter1Param
:   public DelayForm<typename FilterShape::SampleDataType, 
                     DelayFormType::CombFilter1Param, 
                     3, 3>
{
public:
    typedef typename FilterShape::SampleDataType                            SampleType;
    typedef DelayForm<SampleType, DelayFormType::CombFilter1Param, 3, 3>    Base;
    
    enum InParams
    {
        DurationIn,
        FeedbackIn,
        FrequencyIn,
        NumInParams
    };
    
    enum OutParams
    {
        DurationInSamplesOut,
        FeedbackOut,
        FrequencyOut,
        NumOutParams
    };
    
    typedef typename Base::Data                                     Data;
    typedef typename Data::DelayState                               DelayStateBase;
    typedef DelayFormCombFilter1Param                               FormType;
    typedef typename FilterShape::Data                              FilterShapeData;
    typedef typename FilterShape::FormType                          FilterForm;
    typedef typename FilterForm::Data                               FilterFormData;
    typedef typename TypeUtility<SampleType>::IndexType             IndexType;

    struct DelayState : DelayStateBase
    {
        FilterShapeData filterShapeData;
        FilterFormData filterFormData;
    };
    
    typedef SampleType                                              SampleDataType;
    typedef Delay3ParamChannelInternal<FormType>                    DelayInternal;
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

    typedef typename TypeUtility<SampleType>::IndexType             Param3Type;  
    typedef Param3Type                                              FrequencyType;    
    typedef UnitBase<FrequencyType>                                 FrequencyUnitType;

    typedef InterpSelect<SampleType,DurationType,InterpTypeCode>    InterpSelect;
    typedef typename InterpSelect::InterpType                       InterpType;
    typedef typename InterpType::ExtensionBuffer                    ExtensionBuffer;
    
    typedef void (*InputFunction)  (Data&, DelayState&);
    typedef void (*ReadFunction)   (Data&, DelayState&);
    typedef void (*WriteFunction)  (Data&, DelayState&);
    typedef void (*OutputFunction) (Data&, DelayState&);
    typedef void (*Param1Function) (Data&, DelayState&, Param1Type const&);
    typedef void (*Param2Function) (Data&, DelayState&, Param2Type const&);
    typedef void (*Param3Function) (Data&, DelayState&, Param3Type const&);
    
    static inline IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration, IOKey::Feedback, IOKey::Frequency);
        return keys;
    }    
    
    static inline void inputIgnore (Data&, DelayState&) throw() { }
    static inline void inputRead (Data&, DelayState& state) throw()
    {
        state.inputValue = *state.inputSamples++;
    }
    
    static inline void readIgnore (Data&, DelayState&) throw() { }
    static inline void readRead (Data&, DelayState& state) throw()
    {
        DurationType readPosition = DurationType (state.writePosition) - state.paramsOut[DurationInSamplesOut];
        if (readPosition < state.buffer0)
            readPosition += state.bufferLengthIndex;
        plonk_assert (readPosition >= 0 && readPosition <= state.bufferLengthIndex);
        state.readValue = InterpType::lookup (state.bufferSamples, readPosition);
    }
    
    static inline void writeIgnore (Data&, DelayState&) throw() { }
    static inline void writeWrite (Data&, DelayState& state) throw()
    {
        plonk_assert (state.writePosition >= 0 && state.writePosition < state.bufferLength);
                
        state.writeValue = state.inputValue + FilterShape::process (SampleType (state.paramsOut[FeedbackOut] * state.readValue),
                                                                    state.filterShapeData,
                                                                    state.filterFormData);
        
        state.bufferSamples[state.writePosition] = state.writeValue;
       
        if (state.writePosition == 0)
            state.bufferSamples[state.bufferLength] = state.writeValue; // for interpolation
    }
    
    static inline void outputIgnore (Data&, DelayState&) throw() { }
    static inline void outputWrite (Data&, DelayState& state) throw()
    {
        state.outputValue = state.readValue;
        *state.outputSamples++ = state.outputValue;
        state.writePosition++;
    }
    
    static inline void param1Ignore (Data&, DelayState&, DurationType const&) throw() { }
    static inline void param1Process (Data& data, DelayState& state, DurationType const& duration) throw()
    {
        if (state.paramsIn[DurationIn] != duration)
        {
            state.paramsIn[DurationIn] = duration;
            state.paramsOut[DurationInSamplesOut] = DurationType (duration * data.base.sampleRate);
            plonk_assert (state.paramsOut[DurationInSamplesOut] >= 0 && 
                          state.paramsOut[DurationInSamplesOut] <= state.bufferLengthIndex);
        }
    }
    
    static inline void param2Ignore (Data&, DelayState&, FeedbackType const&) throw() { }
    static inline void param2Process (Data&, DelayState& state, FeedbackType const& feedback) throw()
    {                                
        state.paramsIn[FeedbackIn] = state.paramsOut[FeedbackOut] = feedback; // probably faster not to check
    }

    static inline void param3Ignore (Data&, DelayState&, FrequencyType const&) throw() { }
    static inline void param3Process (Data& data, DelayState& state, FrequencyType const& frequency) throw()
    {   
        if (state.paramsIn[FrequencyIn] != frequency)
        {
            state.paramsIn[FrequencyIn] = 
            state.paramsOut[FrequencyOut] = 
            state.filterShapeData.params[FilterShape::Frequency] = frequency;
            state.filterShapeData.filterSampleRate = data.base.sampleRate;
            state.filterShapeData.filterSampleDuration = data.base.sampleDuration;
            FilterShape::calculate (state.filterShapeData);
        }
    }

    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction>
    static inline void tick (Data& data, DelayState& state) throw()
    {
        inputFunction (data, state);
        readFunction (data, state);
        writeFunction (data, state);
        outputFunction (data, state);
    }
    
    static inline UnitType ar (UnitType const& input,
                               DurationUnitType const& duration,
                               FeedbackUnitType const& feedback,
                               FrequencyUnitType const& frequency,
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
        const int numFrequencyChannels = frequency.getNumChannels();
        const int numChannels = plonk::max (numInputChannels, numDurationChannels, 
                                            numFeedbackChannels, numFrequencyChannels);
        
        UnitType mainUnit = UnitType::emptyChannels (numChannels);
        
        for (int i = 0; i < numChannels; ++i)
        {
            Inputs inputs;
            inputs.put (IOKey::Generic, input[i]);
            inputs.put (IOKey::Duration, duration[i]);
            inputs.put (IOKey::Feedback, feedback[i]);
            inputs.put (IOKey::Frequency, frequency[i]);
            
            UnitType unit = UnitType::template proxiesFromInputs<DelayInternal> (inputs,
                                                                                 data,
                                                                                 preferredBlockSize,
                                                                                 preferredSampleRate);
            mainUnit.put (i, unit);
        }
        
        return UnitType::applyMulAdd (mainUnit, mul, add);
    }
};


//------------------------------------------------------------------------------


/** A comb filter with a filter in the feedback loop (where the filter takes one parameter). 
 
 @par Factory functions:
 - ar (input, duration=0.5, feedback=0.5, maximumDuration=1, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 
 @par Inputs:
 - input: (unit, multi) the unit to which delay is applied
 - duration: (unit, multi) the delay duration in seconds
 - feedback: (unit, multi) the feedback amount multiplier
 - frequency: (unit, multi) the filter frequency (the use depends on the filter type.
 - maximumDuration: (real) the maximum delay time in seconds (this can be equal to duration if duration is a constant)
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset aded to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)
 
 @ingroup DelayUnits */
template<class FilterShape, Interp::TypeCode InterpTypeCode>
class CombFilter1ParamUnit
{
public:    
    typedef typename FilterShape::SampleDataType                    SampleType;
    typedef DelayFormCombFilter1Param<FilterShape,InterpTypeCode>   FormType;
    
    typedef Delay3ParamChannelInternal<FormType>                    DelayInternal;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    
    typedef typename DelayInternal::Param1Type                      DurationType;
    typedef UnitBase<DurationType>                                  DurationUnitType;
    
    typedef typename DelayInternal::Param2Type                      FeedbackType;
    typedef UnitBase<FeedbackType>                                  FeedbackUnitType;

    typedef typename DelayInternal::Param3Type                      FrequencyType;
    typedef UnitBase<FrequencyType>                                 FrequencyUnitType;

//    typedef CombFilter1ParamUnit<FilterShape, Interp::Lagrange3>    HQ;

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("CombFilter1Param", "A comb filter with a filter in the feedback loop (where the filter takes one parameter).",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,                IOLimit::None,                         
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Duration,           Measure::Seconds,   0.5,                IOLimit::Minimum,   Measure::Seconds,           0.0,
                         IOKey::Feedback,           Measure::Factor,    0.5,                IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::MaximumDuration,    Measure::Seconds,   1.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate comb filter. */
    static UnitType ar (UnitType const& input,
                        DurationUnitType const& duration = DurationType (0.5),
                        FeedbackUnitType const& feedback = FeedbackType (0.5),
                        FrequencyUnitType const& frequency = FrequencyType (1200.0),
                        const DurationType maximumDuration = DurationType (1.0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        return FormType::ar (input, duration, feedback, frequency, maximumDuration, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
};

/** A comb filter with a one-pole low-pass filter in the feedback loop. @ingroup DelayUnits */
typedef CombFilter1ParamUnit<FilterShapeLPFP1Base<PLONK_TYPE_DEFAULT> >     CombLPFP1;

/** A comb filter with a Butterworth low-pass filter in the feedback loop. @ingroup DelayUnits */
typedef CombFilter1ParamUnit<FilterShapeLPFBase<PLONK_TYPE_DEFAULT> >       CombLPF;

/** A comb filter with a one-pole high-pass filter in the feedback loop. @ingroup DelayUnits */
typedef CombFilter1ParamUnit<FilterShapeHPFP1Base<PLONK_TYPE_DEFAULT> >     CombHPFP1;

/** A comb filter with a Butterworth high-pass filter in the feedback loop. @ingroup DelayUnits */
typedef CombFilter1ParamUnit<FilterShapeHPFBase<PLONK_TYPE_DEFAULT> >       CombHPF;


#endif // PLONK_DELAYFORMCOMBFILTER1PARAM_H

