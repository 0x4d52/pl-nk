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

#ifndef PLONK_BINARYOPPLINK_H
#define PLONK_BINARYOPPLINK_H

#include "plonk_BinaryOpChannel.h"


#define PLONK_PLINK_UNARYOPCHANNEL_COMMON_START(PLONKOP) \
    template<>\
    class UnaryOpChannelInternal<float, PLONKOP> : public ChannelInternal<float, ChannelInternalCore::Data>\
    {\
    public:\
        typedef ChannelInternalCore::Data               Data;\
        typedef ChannelBase<float>                      ChannelType;\
        typedef UnaryOpChannelInternal<float,PLONKOP>   UnaryOpInternal;\
        typedef ChannelInternal<float,Data>             Internal;\
        typedef ChannelInternalBase<float>              InternalBase;\
        typedef UnitBase<float>                         UnitType;\
        typedef InputDictionary                         Inputs;\
        typedef NumericalArray<float>                   Buffer;\
        typedef UnaryOpUtility<float>                   UtilityType;\
        \
        enum InputIndices { Operand, NumInputs };\
        enum Outputs { Output, NumOutputs };\
        enum Buffers { OutputBuffer, OperandBuffer, NumBuffers };\
        typedef PlinkProcess<NumBuffers> Process;\
        \
        UnaryOpChannelInternal (Inputs const& inputs, Data const& data,\
                                BlockSize const& blockSize, SampleRate const& sampleRate) throw()\
        :   Internal (inputs, data, blockSize, sampleRate)\
        {\
            plonk_assert (NumBuffers == (NumInputs + NumOutputs));\
            Process::init (&p, this, NumOutputs, NumInputs);\
        }
        

#define PLONK_PLINK_UNARYOPCHANNEL_COMMON_END\
    private:\
        Process p;\
    };

#define PLONK_PLINK_UNARYOPCHANNEL_FUNCTION(PLANKOP,TYPECODE) plink_UnaryOpProcess##PLANKOP##TYPECODE

#define PLONK_PLINK_UNARYOPCHANNEL_PROCESS(PLONKOP, PLANKOP) \
    Text getName() const throw() {\
        Text variant = UtilityType::global().getName (PLONKOP);\
        if (variant.length() < 1) variant = "unknown type";\
        return "Unary Operator (" + variant + ")";\
    }\
    \
    IntArray getInputKeys() const throw() {\
        const IntArray keys (IOKey::Signal);\
        return keys;\
    }\
    \
    InternalBase* getChannel (const int index) throw() {\
        const Inputs channelInputs = this->getInputs().getChannel (index);\
        return new UnaryOpInternal (channelInputs, this->getState(), this->getBlockSize(), this->getSampleRate());\
    }\
    \
    void initChannel (const int channel) throw() {\
        const UnitType& input = this->getInputAsUnit (IOKey::Signal);\
        const float sourceValue = input.getValue (channel);\
        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel), this->getBlockSize()));\
        this->setSampleRate (SampleRate::decide (input.getSampleRate (channel), this->getSampleRate()));\
        this->setOverlap (input.getOverlap (channel));\
        this->initValue (pl_##PLANKOP##F (sourceValue));\
    }\
    \
    void process (ProcessInfo& info, const int channel) throw() {\
        UnitType& operandUnit (this->getInputAsUnit (IOKey::Signal));\
        const Buffer operandBuffer (operandUnit.process (info, channel));\
        \
        p.buffers[0].bufferSize = this->getOutputBuffer().length();\
        p.buffers[0].buffer = this->getOutputSamples();\
        p.buffers[1].bufferSize = operandBuffer.length();\
        p.buffers[1].buffer = operandBuffer.getArray();\
        \
        PLONK_PLINK_UNARYOPCHANNEL_FUNCTION(PLANKOP,F) (&p,0);\
    }

#define PLONK_PLINK_UNARYOPCHANNEL(PLONKOP, PLANKOP)\
    PLONK_PLINK_UNARYOPCHANNEL_COMMON_START(PLONKOP)\
    PLONK_PLINK_UNARYOPCHANNEL_PROCESS(PLONKOP, PLANKOP)\
    PLONK_PLINK_UNARYOPCHANNEL_COMMON_END

PLONK_PLINK_UNARYOPCHANNEL(plonk::move, Move)
PLONK_PLINK_UNARYOPCHANNEL(plonk::neg, Neg)
PLONK_PLINK_UNARYOPCHANNEL(plonk::abs, Abs)
PLONK_PLINK_UNARYOPCHANNEL(plonk::log2, Log2)
PLONK_PLINK_UNARYOPCHANNEL(plonk::reciprocal, Reciprocal)
PLONK_PLINK_UNARYOPCHANNEL(plonk::sin, Sin)
PLONK_PLINK_UNARYOPCHANNEL(plonk::cos, Cos)
PLONK_PLINK_UNARYOPCHANNEL(plonk::tan, Tan)
PLONK_PLINK_UNARYOPCHANNEL(plonk::asin, Asin)
PLONK_PLINK_UNARYOPCHANNEL(plonk::acos, Acos)
PLONK_PLINK_UNARYOPCHANNEL(plonk::atan, Atan)
PLONK_PLINK_UNARYOPCHANNEL(plonk::sinh, Sinh)
PLONK_PLINK_UNARYOPCHANNEL(plonk::cosh, Cosh)
PLONK_PLINK_UNARYOPCHANNEL(plonk::tanh, Tanh)
PLONK_PLINK_UNARYOPCHANNEL(plonk::sqrt, Sqrt)
PLONK_PLINK_UNARYOPCHANNEL(plonk::log, Log)
PLONK_PLINK_UNARYOPCHANNEL(plonk::log10, Log10)
PLONK_PLINK_UNARYOPCHANNEL(plonk::exp, Exp)
PLONK_PLINK_UNARYOPCHANNEL(plonk::squared, Squared)
PLONK_PLINK_UNARYOPCHANNEL(plonk::cubed, Cubed)
PLONK_PLINK_UNARYOPCHANNEL(plonk::ceil, Ceil)
PLONK_PLINK_UNARYOPCHANNEL(plonk::floor, Floor)
PLONK_PLINK_UNARYOPCHANNEL(plonk::frac, Frac)
PLONK_PLINK_UNARYOPCHANNEL(plonk::sign , Sign)
PLONK_PLINK_UNARYOPCHANNEL(plonk::m2f, M2F)
PLONK_PLINK_UNARYOPCHANNEL(plonk::f2m, F2M)
PLONK_PLINK_UNARYOPCHANNEL(plonk::a2dB, A2dB)
PLONK_PLINK_UNARYOPCHANNEL(plonk::dB2a, dB2A)
PLONK_PLINK_UNARYOPCHANNEL(plonk::d2r, D2R)
PLONK_PLINK_UNARYOPCHANNEL(plonk::r2d, R2D)
PLONK_PLINK_UNARYOPCHANNEL(plonk::distort, Distort)
PLONK_PLINK_UNARYOPCHANNEL(plonk::zap, Zap)

#endif // PLONK_BINARYOPPLINK_H
