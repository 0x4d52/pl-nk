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

#ifndef PLONK_DELAYFORMDELAY_H
#define PLONK_DELAYFORMDELAY_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_DelayForwardDeclarations.h"


template<class SampleType>
class DelayFormDelay
:   public DelayForm<SampleType, DelayFormType::Delay, 1, 1>
{
public:
    typedef DelayForm<SampleType, DelayFormType::Delay, 1, 1>       Base;
    
    enum InParams
    {
        Duration,
        NumInParams
    };
    
    enum OutParams
    {
        DurationInSamples,
        NumOutParams
    };

    typedef typename Base::Data                                     Data;
    typedef typename Data::DelayState                               DelayState;
    typedef DelayFormDelay                                          FormType;
 
    typedef SampleType                                              SampleDataType;
    typedef Delay1ParamChannelInternal<FormType>                    DelayInternal;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType             Param1Type;  
    typedef Param1Type                                              DurationType;    
    typedef UnitBase<DurationType>                                  DurationUnitType;
    typedef NumericalArray<DurationType>                            DurationBufferType;
    typedef ChannelBase<DurationType>                               DurationChannelType;
    typedef NumericalArray2D<DurationChannelType,DurationUnitType>  DurationUnitArrayType;
    
    typedef NumericalArray2D<ChannelType,UnitType>                  UnitArrayType;
    typedef InterpLinear<SampleType,DurationType>                   InterpType;

    typedef void (*InputFunction)  (Data&, DelayState&);
    typedef void (*ReadFunction)   (Data&, DelayState&);
    typedef void (*WriteFunction)  (Data&, DelayState&);
    typedef void (*OutputFunction) (Data&, DelayState&);
    typedef void (*Param1Function) (Data&, DelayState&, Param1Type const&);

    static inline IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration);
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
        DurationType readPosition = DurationType (state.writePosition) - state.paramsOut[DurationInSamples];
        if (readPosition < state.buffer0)
            readPosition += state.bufferLengthIndex;
        plonk_assert (readPosition >= 0 && readPosition <= state.bufferLengthIndex);
        state.readValue = InterpType::lookup (state.bufferSamples, readPosition);
    }
        
    static inline void writeIgnore (Data&, DelayState&) throw() { }
    static inline void writeWrite (Data&, DelayState& state) throw()
    {
        plonk_assert (state.writePosition >= 0 && state.writePosition < state.bufferLength);
        state.writeValue = state.inputValue;
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
        if (state.paramsIn[Duration] != duration)
        {
            state.paramsIn[Duration] = duration;
            state.paramsOut[DurationInSamples] = DurationType (duration * data.base.sampleRate);
            plonk_assert (state.paramsOut[DurationInSamples] >= 0 && 
                          state.paramsOut[DurationInSamples] <= state.bufferLengthIndex);
        }
    }
    
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction>
    static inline void tick (Data& data, DelayState& state) throw()
    {
        inputFunction (data, state);
        writeFunction (data, state);
        readFunction (data, state);  
        outputFunction (data, state);
    }
    
    static inline UnitType ar (UnitType const& input,
                               DurationUnitType const& duration,
                               const DurationType maximumDuration,
                               UnitType const& mul,
                               UnitType const& add,
                               BlockSize const& preferredBlockSize,
                               SampleRate const& preferredSampleRate) throw()
    {             
        const Data data = { { -1.0, -1.0 }, maximumDuration, 0, };
        
        const int numInputChannels = input.getNumChannels();
        const int numDurationChannels = duration.getNumChannels();
        const int numChannels = plonk::max (numInputChannels, numDurationChannels);
        
        if (numChannels == 1)
        {
            Inputs inputs;
            inputs.put (IOKey::Generic, input);
            inputs.put (IOKey::Duration, duration);
            inputs.put (IOKey::Multiply, mul);
            inputs.put (IOKey::Add, add);
            
            return UnitType::template proxiesFromInputs<DelayInternal> (inputs,
                                                                        data, 
                                                                        preferredBlockSize, 
                                                                        preferredSampleRate);
        }
        else
        {
            DurationUnitArrayType durationsGrouped = duration.deinterleave (numInputChannels);
            UnitArrayType resultGrouped;
            
            for (int i = 0; i < numInputChannels; ++i)
            {
                Inputs inputs;
                inputs.put (IOKey::Generic, input[i]);
                inputs.put (IOKey::Duration, durationsGrouped.wrapAt (i));
                
                UnitType unit = UnitType::template proxiesFromInputs<DelayInternal> (inputs, 
                                                                                     data, 
                                                                                     preferredBlockSize, 
                                                                                     preferredSampleRate);
                resultGrouped.add (unit);
            }
            
            const UnitType mainUnit (resultGrouped.interleave());
            plonk_assert (mainUnit.getNumChannels() == numChannels);
            return UnitType::applyMulAdd (mainUnit, mul, add);
        }
    }

};

//------------------------------------------------------------------------------

/** Simple delay processor. 
 
 @par Factory functions:
 - ar (input, duration=0.5, maximumDuration=1, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 
 @par Inputs:
 - input: (unit, multi) the unit to which delay is applied
 - duration: (unit, multi) the delay duration in seconds
 - maximumDuration: (real) the maximum delay time in seconds (this can be equal to duration if duration is a constant)
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset aded to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @ingroup DelayUnits */
template<class SampleType>
class DelayUnit
{
public:    
    typedef DelayFormDelay<SampleType>              FormType;
    
    typedef Delay1ParamChannelInternal<FormType>    DelayInternal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
    typedef typename DelayInternal::Param1Type      DurationType;
    typedef UnitBase<DurationType>                  DurationUnitType;
    
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Delay", "A simple delay processor.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,                IOLimit::None,                         
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Duration,           Measure::Seconds,   0.5,                IOLimit::Minimum,   Measure::Seconds,   0.0,
                         IOKey::MaximumDuration,    Measure::Seconds,   1.0,                IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }
    
    static UnitType ar (UnitType const& input,
                        DurationUnitType const& duration = DurationType (0.5),
                        const DurationType maximumDuration = DurationType (1.0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        return FormType::ar (input, duration, maximumDuration, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
};

typedef DelayUnit<PLONK_TYPE_DEFAULT> Delay;



#endif // PLONK_DELAYFORMDELAY_H

