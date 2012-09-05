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

#ifndef PLONK_UNARYOPCHANNEL_H
#define PLONK_UNARYOPCHANNEL_H

#include "../../maths/plonk_InlineUnaryOps.h"
#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType>
class UnaryOpUtility
{
private:
    typedef SampleType (*Function)(SampleType const&);
    
    UnaryOpUtility() throw()
    {
        names.put (&plonk::move<SampleType>,        "move");
        names.put (&plonk::inc<SampleType>,         "inc");
        names.put (&plonk::dec<SampleType>,         "dec");
        names.put (&plonk::abs<SampleType>,         "abs");
        names.put (&plonk::log2<SampleType>,        "log2");
        names.put (&plonk::neg<SampleType>,         "neg");
        names.put (&plonk::reciprocal<SampleType>,  "reciprocal");
        names.put (&plonk::sin<SampleType>,         "sin");
        names.put (&plonk::cos<SampleType>,         "cos");
        names.put (&plonk::tan<SampleType>,         "tan");
        names.put (&plonk::asin<SampleType>,        "asin");
        names.put (&plonk::acos<SampleType>,        "acos");
        names.put (&plonk::atan<SampleType>,        "atan");
        names.put (&plonk::sinh<SampleType>,        "sinh");
        names.put (&plonk::cosh<SampleType>,        "cosh");
        names.put (&plonk::tanh<SampleType>,        "tanh");
        names.put (&plonk::sqrt<SampleType>,        "sqrt");
        names.put (&plonk::log<SampleType>,         "log");
        names.put (&plonk::log10<SampleType>,       "log10");
        names.put (&plonk::exp<SampleType>,         "exp");
        names.put (&plonk::squared<SampleType>,     "squared");
        names.put (&plonk::cubed<SampleType>,       "cubed");
        names.put (&plonk::ceil<SampleType>,        "ceil");
        names.put (&plonk::floor<SampleType>,       "floor");
        names.put (&plonk::frac<SampleType>,        "frac");
        names.put (&plonk::sign<SampleType>,        "sign");
        names.put (&plonk::m2f<SampleType>,         "MIDI to frequency");
        names.put (&plonk::f2m<SampleType>,         "Frequency to MIDI");
        names.put (&plonk::a2dB<SampleType>,        "Amplitude to dB");
        names.put (&plonk::dB2a<SampleType>,        "dB to Amplitude");
        names.put (&plonk::d2r<SampleType>,         "Degrees to Radians");
        names.put (&plonk::r2d<SampleType>,         "Radians to Degrees");
        names.put (&plonk::distort<SampleType>,     "distort");
        names.put (&plonk::zap<SampleType>,         "zap");
    }
    
public:
    inline Text getName (Function function) const throw()
    {
        return names[function];
    }
    
    inline void add (Function function, Text const& name) throw()
    {
        names.put (function, name);
    }
        
    static UnaryOpUtility& global() throw()
    {
        static UnaryOpUtility util;
        return util;
    }
    
private:
    Dictionary<Text, Function> names;
};


//------------------------------------------------------------------------------


/** For applying unary operators. */
template<class SampleType, PLONK_UNARYOPFUNCTION(SampleType, op)>
class UnaryOpChannelInternal
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data  Data;

    typedef ChannelBase<SampleType>                 ChannelType;
    typedef UnaryOpChannelInternal<SampleType,op>   UnaryOpInternal;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternalBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    typedef UnaryOpUtility<SampleType>              UtilityType;
    
    UnaryOpChannelInternal (Inputs const& inputs, 
                            Data const& data, 
                            BlockSize const& blockSize,
                            SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
        
    Text getName() const throw()
    {
        Text variant = UtilityType::global().getName (op);
        
        if (variant.length() < 1)
            variant = "unknown type";
        
        return "Unary Operator (" + variant + ")";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new UnaryOpInternal (channelInputs, 
                                    this->getState(), 
                                    this->getBlockSize(), 
                                    this->getSampleRate());
    }    
    
    void initChannel (const int channel) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        const SampleType sourceValue = input.getValue (channel);
        
        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getSampleRate (channel),
                                                 this->getSampleRate()));    
        
        this->setOverlap (input.getOverlap (channel));
        
        this->initValue (op (sourceValue));
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        UnitType& operandUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& operandBuffer (operandUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const SampleType* const operandSamples = operandBuffer.getArray();
        const int operandBufferLength = operandBuffer.length();
        
        int i;
        
        if (operandBufferLength == outputBufferLength)
        {
            NumericalArrayUnaryOp<SampleType,op>::calc (outputSamples, operandSamples, outputBufferLength);
        }
        else if (operandBufferLength == 1)
        {
            NumericalArrayFiller<SampleType>::fill (outputSamples, operandSamples[0], outputBufferLength);
        }
        else
        {
            double operandPosition = 0.0;
            const double operandIncrement = double (operandBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = op (operandSamples[int (operandPosition)]);
                operandPosition += operandIncrement;
            }        
        }
    }
};


#endif // PLONK_UNARYOPCHANNEL_H
