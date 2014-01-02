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
    typedef typename UnaryOpFunctionsHelper<SampleType>::UnaryOpFunctionsType UnaryOpFunctionsType;

    UnaryOpUtility() throw()
    {
        names.put (&UnaryOpFunctionsType::move,        "move");
        names.put (&UnaryOpFunctionsType::inc,         "inc");
        names.put (&UnaryOpFunctionsType::dec,         "dec");
        names.put (&UnaryOpFunctionsType::abs,         "abs");
        names.put (&UnaryOpFunctionsType::log2,        "log2");
        names.put (&UnaryOpFunctionsType::neg,         "neg");
        names.put (&UnaryOpFunctionsType::reciprocal,  "reciprocal");
        names.put (&UnaryOpFunctionsType::sin,         "sin");
        names.put (&UnaryOpFunctionsType::cos,         "cos");
        names.put (&UnaryOpFunctionsType::tan,         "tan");
        names.put (&UnaryOpFunctionsType::asin,        "asin");
        names.put (&UnaryOpFunctionsType::acos,        "acos");
        names.put (&UnaryOpFunctionsType::atan,        "atan");
        names.put (&UnaryOpFunctionsType::sinh,        "sinh");
        names.put (&UnaryOpFunctionsType::cosh,        "cosh");
        names.put (&UnaryOpFunctionsType::tanh,        "tanh");
        names.put (&UnaryOpFunctionsType::sqrt,        "sqrt");
        names.put (&UnaryOpFunctionsType::log,         "log");
        names.put (&UnaryOpFunctionsType::log10,       "log10");
        names.put (&UnaryOpFunctionsType::exp,         "exp");
        names.put (&UnaryOpFunctionsType::squared,     "squared");
        names.put (&UnaryOpFunctionsType::cubed,       "cubed");
        names.put (&UnaryOpFunctionsType::ceil,        "ceil");
        names.put (&UnaryOpFunctionsType::floor,       "floor");
        names.put (&UnaryOpFunctionsType::frac,        "frac");
        names.put (&UnaryOpFunctionsType::sign,        "sign");
        names.put (&UnaryOpFunctionsType::m2f,         "MIDI to frequency");
        names.put (&UnaryOpFunctionsType::f2m,         "Frequency to MIDI");
        names.put (&UnaryOpFunctionsType::a2dB,        "Amplitude to dB");
        names.put (&UnaryOpFunctionsType::dB2a,        "dB to Amplitude");
        names.put (&UnaryOpFunctionsType::d2r,         "Degrees to Radians");
        names.put (&UnaryOpFunctionsType::r2d,         "Radians to Degrees");
        names.put (&UnaryOpFunctionsType::distort,     "distort");
        names.put (&UnaryOpFunctionsType::zap,         "zap");
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
    typedef typename UnaryOpFunctionsHelper<SampleType>::UnaryOpFunctionsType UnaryOpFunctionsType;

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
