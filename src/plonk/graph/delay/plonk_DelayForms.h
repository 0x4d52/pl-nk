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
#include "plonk_DelayForwardDeclarations.h"

template<class SampleType, signed Form, signed NumInParams, signed NumOutParams>
struct DelayFormData
{    
    typedef typename TypeUtility<SampleType>::IndexType IndexType;
    
    ChannelInternalCore::Data base;
    
    IndexType maximumDuration;
    int writePosition;
    
    struct DelayState
    {
        const ChannelInternalCore::Data base;
        
        SampleType* outputSamples;
        const int outputBufferLength;
        const SampleType* inputSamples;
        SampleType* const bufferSamples;
        const int bufferLength;
        const IndexType bufferLengthIndex;
        const IndexType buffer0;

        int writePosition;
        
        SampleType inputValue;
        SampleType readValue;
        SampleType writeValue;
        SampleType outputValue;
        
        IndexType paramsIn[NumInParams];
        IndexType paramsOut[NumOutParams];
    };
};      

//------------------------------------------------------------------------------

template<class SampleType, signed Form, signed NumInParams, signed NumOutParams>
class DelayFormBase
{
public:    
    static inline Text getName() throw()
    {
        return DelayFormType::getName (Form);
    }        
};

template<class SampleType, signed Form, signed NumInParams, signed NumOutParams>
class DelayForm : public DelayFormBase<SampleType, Form, NumInParams, NumOutParams>
{
};

//------------------------------------------------------------------------------


template<class SampleType>
class DelayForm<SampleType, DelayFormType::Delay, 1, 1>
:   public DelayFormBase<SampleType, DelayFormType::Delay, 1, 1>
{
public:
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

    typedef DelayFormData<SampleType, DelayFormType::Delay, NumInParams, NumOutParams>  Data;
    typedef typename Data::DelayState                                                   DelayState;
    typedef DelayForm<SampleType, DelayFormType::Delay, NumInParams, NumOutParams>      FormType;
 
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

    typedef void (*InputFunction)  (DelayState&);
    typedef void (*ReadFunction)   (DelayState&);
    typedef void (*WriteFunction)  (DelayState&);
    typedef void (*OutputFunction) (DelayState&);
    typedef void (*Param1Function) (DelayState&, Param1Type const&);

    static inline IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration);
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
        DurationType readPosition = DurationType (data.writePosition) - data.paramsOut[DurationInSamples];
        if (readPosition < data.buffer0)
            readPosition += data.bufferLengthIndex;
        plonk_assert (readPosition >= 0 && readPosition <= data.bufferLengthIndex);
        data.readValue = InterpType::lookup (data.bufferSamples, readPosition);
    }
        
    static inline void writeIgnore (DelayState&) throw() { }
    static inline void writeWrite (DelayState& data) throw()
    {
        plonk_assert (data.writePosition >= 0 && data.writePosition < data.bufferLength);
        data.writeValue = data.inputValue;
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
        data.paramsIn[Duration] = duration;
        data.paramsOut[DurationInSamples] = DurationType (duration * data.base.sampleRate);
        plonk_assert (data.paramsOut[DurationInSamples] >= 0 && data.paramsOut[DurationInSamples] <= data.bufferLengthIndex);
    }
    
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction>
    static inline void tick (DelayState& data) throw()
    {
        inputFunction (data);
        writeFunction (data);
        readFunction (data);  
        outputFunction (data);
    }
    
    static inline UnitType ar (UnitType const& input,
                               DurationUnitType const& duration,
                               const DurationType maximumDuration,
                               UnitType const& mul,
                               UnitType const& add,
                               BlockSize const& preferredBlockSize,
                               SampleRate const& preferredSampleRate) throw()
    {             
        Data data = { { -1.0, -1.0 }, maximumDuration, 0, };
        
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
            
            return UnitType::template createFromInputs<DelayInternal> (inputs, 
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


template<class SampleType>
class DelayForm<SampleType, DelayFormType::CombDecay, 2, 2>
:   public DelayFormBase<SampleType, DelayFormType::CombDecay, 2, 2>
{
public:
    enum InParams
    {
        Duration,
        Decay,
        NumInParams
    };
    
    enum OutParams
    {
        DurationInSamples,
        Feedback,
        NumOutParams
    };

    typedef DelayFormData<SampleType, DelayFormType::CombDecay, NumInParams, NumOutParams>  Data;
    typedef typename Data::DelayState                                                       DelayState;

    typedef SampleType                                  SampleDataType;
    typedef UnitBase<SampleType>                        UnitType;
    typedef NumericalArray<SampleType>                  Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType DurationType;    
    typedef typename TypeUtility<SampleType>::IndexType DecayType;
    
    typedef DurationType                                Param1Type;    
    typedef DecayType                                   Param2Type;
    
    typedef InterpLinear<SampleType,DurationType>       InterpType;

    typedef void (*InputFunction)  (DelayState&);
    typedef void (*ReadFunction)   (DelayState&);
    typedef void (*WriteFunction)  (DelayState&);
    typedef void (*OutputFunction) (DelayState&);
    typedef void (*Param1Function) (DelayState&, Param1Type const&);
    typedef void (*Param2Function) (DelayState&, Param2Type const&);
    
    static inline IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration, IOKey::Decay);
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
        DurationType readPosition = DurationType (data.writePosition) - data.paramsOut[DurationInSamples];
        if (readPosition < data.buffer0)
            readPosition += data.bufferLengthIndex;
        plonk_assert (readPosition >= 0 && readPosition <= data.bufferLengthIndex);
        data.readValue = InterpType::lookup (data.bufferSamples, readPosition);
    }
    
    static inline void writeIgnore (DelayState&) throw() { }
    static inline void writeWrite (DelayState& data) throw()
    {
        plonk_assert (data.writePosition >= 0 && data.writePosition < data.bufferLength);
        data.writeValue = data.readValue * data.paramsOut[Feedback] + data.inputValue;
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
        data.paramsIn[Duration] = duration;
        data.paramsOut[DurationInSamples] = DurationType (duration * data.base.sampleRate);
        plonk_assert (data.paramsOut[DurationInSamples] >= 0 && data.paramsOut[DurationInSamples] <= data.bufferLengthIndex);
    }
    
    static inline void param2Ignore (DelayState& data, DecayType const& decay) throw() { }
    static inline void param2Process (DelayState& data, DecayType const& decay) throw()
    {                                
        data.paramsIn[Decay] = decay;
        data.paramsOut[Feedback] = plonk::decayFeedback (data.paramsIn[Duration], decay);
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
};


//------------------------------------------------------------------------------


#endif // PLONK_DELAYFORMS_H

