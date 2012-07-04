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

template<class SampleType, signed Form, signed NumParams>
struct DelayFormData
{    
    typedef typename TypeUtility<SampleType>::IndexType DurationType;
    
    ChannelInternalCore::Data base;
    
    DurationType maximumDuration;
    int writePosition;
    
    SampleType inputValue;
    SampleType readValue;
    SampleType writeValue;
    SampleType outputValue;
    
    const SampleType* inputSamples;
    SampleType* bufferSamples;
    SampleType* outputSamples;
    
    DurationType buffer0;
    DurationType bufferLengthIndex;
    DurationType params[NumParams];
};      

//------------------------------------------------------------------------------

template<class SampleType, signed Form, signed NumParams>
class DelayFormBase
{
public:    
    static inline Text getName() throw()
    {
        return DelayFormType::getName (Form);
    }        
};

template<class SampleType, signed Form, signed NumParams>
class DelayForm : public DelayFormBase<SampleType, Form, NumParams>
{
};

//------------------------------------------------------------------------------


template<class SampleType>
class DelayForm<SampleType, DelayFormType::Delay, 1>
:   public DelayFormBase<SampleType, DelayFormType::Delay, 1>
{
public:
    enum Params
    {
        Duration,
        NumParams
    };
    
    typedef SampleType                                                  SampleDataType;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef NumericalArray<SampleType>                                  Buffer;
    typedef DelayFormData<SampleType, DelayFormType::Delay, NumParams>  Data;
    
    typedef typename TypeUtility<SampleType>::IndexType                 DurationType;
    typedef UnitBase<DurationType>                                      DurationUnitType;
    typedef NumericalArray<DurationType>                                DurationBufferType;
    typedef InterpLinear<SampleType,DurationType>                       InterpType;
    
    typedef DurationType                                                Param1Type;
    typedef DurationUnitType                                            Param1UnitType;
    typedef DurationBufferType                                          Param1BufferType;

    static inline IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Duration);
        return keys;
    }    
    
    static inline void inputIgnore (Data&) throw() { }
    static inline void inputRead (Data& data) throw()
    {
        data.inputValue = *data.inputSamples++;
    }
    
    static inline void readIgnore (Data&, const int) throw() { }
    static inline void readRead (Data& data, const int writePosition) throw()
    {
        DurationType readPosition = DurationType (writePosition) - data.params[Duration];
        if (readPosition < data.buffer0)
            readPosition += data.bufferLengthIndex;
        data.readValue = InterpType::lookup (data.bufferSamples, readPosition);
    }
        
    static inline void writeIgnore (Data&, const int) throw() { }
    static inline void writeWrite (Data& data, const int writePosition) throw()
    {
        data.writeValue = data.inputValue;
        data.bufferSamples[writePosition] = data.writeValue;
    }
    
    static inline void outputIgnore (Data&, int&) throw() { }
    static inline void outputWrite (Data& data, int& writePosition) throw()
    {
        data.outputValue = data.readValue;
        *data.outputSamples++ = data.outputValue;
        writePosition++;
    }
    
    static inline void param1Ignore (Data& data, DurationType const duration) throw() { }
    static inline void param1Process (Data& data, DurationType const duration) throw()
    {
        data.params[Duration] = DurationType (duration * data.base.sampleRate);
        plonk_assert (data.params[Duration] <= data.bufferLengthIndex);
    }
};


//------------------------------------------------------------------------------



#endif // PLONK_DELAYFORMS_H

