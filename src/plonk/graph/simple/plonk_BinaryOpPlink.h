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

#define PLONK_PLINK_BINARYOPCHANNEL_COMMON_START(PLONKOP) \
    template<>\
    class BinaryOpChannelInternal<float, PLONKOP> : public ChannelInternal<float, ChannelInternalCore::Data>\
    {\
    public:\
        typedef ChannelInternalCore::Data               Data;\
        typedef ChannelBase<float>                      ChannelType;\
        typedef BinaryOpChannelInternal<float,PLONKOP>  BinaryOpInternal;\
        typedef ChannelInternal<float,Data>             Internal;\
        typedef ChannelInternalBase<float>              InternalBase;\
        typedef UnitBase<float>                         UnitType;\
        typedef InputDictionary                         Inputs;\
        typedef NumericalArray<float>                   Buffer;\
        typedef BinaryOpUtility<float>                  UtilityType;\
        \
        enum InputIndices { LeftOperand, RightOperand, NumInputs };\
        enum Outputs { Output, NumOutputs };\
        enum Buffers { OutputBuffer, LeftOperandBuffer, RightOperandBuffer, NumBuffers };\
        typedef PlinkProcess<NumBuffers> Process;\
        \
        BinaryOpChannelInternal (Inputs const& inputs, Data const& data,\
                                 BlockSize const& blockSize, SampleRate const& sampleRate) throw()\
        :   Internal (inputs, data, blockSize, sampleRate)\
        {\
            plonk_staticassert (NumBuffers == (NumInputs + NumOutputs));\
            Process::init (&p, this, NumOutputs, NumInputs);\
        }
        

#define PLONK_PLINK_BINARYOPCHANNEL_COMMON_END\
    private:\
        Process p;\
    };

#define PLONK_PLINK_BINARYOPCHANNEL_FUNCTION(PLANKOP,TYPECODE) plink_BinaryOpProcess##PLANKOP##TYPECODE

#define PLONK_PLINK_BINARYOPCHANNEL_PROCESS(PLONKOP, PLANKOP) \
    Text getName() const throw() {\
        Text variant = UtilityType::global().getName (PLONKOP);\
        if (variant.length() < 1) variant = "unknown type";\
        return "Binary Operator (" + variant + ")";\
    }\
    \
    IntArray getInputKeys() const throw() {\
        const IntArray keys (IOKey::LeftOperand, IOKey::RightOperand);\
        return keys;\
    }\
    \
    InternalBase* getChannel (const int index) throw() {\
        const Inputs channelInputs = this->getInputs().getChannel (index);\
        return new BinaryOpInternal (channelInputs, this->getState(), this->getBlockSize(), this->getSampleRate());\
    }\
    \
    void initChannel (const int channel) throw() {\
        const UnitType& leftUnit = this->getInputAsUnit (IOKey::LeftOperand);\
        const float leftValue = leftUnit.getValue (channel);\
        const UnitType& rightUnit = this->getInputAsUnit (IOKey::RightOperand);\
        const float rightValue = rightUnit.getValue (channel);\
        this->setBlockSize (BlockSize::decide (leftUnit.getBlockSize (channel).selectMax (rightUnit.getBlockSize (channel)),\
                                               this->getBlockSize()));\
        this->setSampleRate (SampleRate::decide (leftUnit.getSampleRate (channel).selectMax (rightUnit.getSampleRate (channel)),\
                                                 this->getSampleRate()));\
        const DoubleVariable leftOverlap = leftUnit.getOverlap (channel);\
        const DoubleVariable rightOverlap = rightUnit.getOverlap (channel);\
        if (leftOverlap == rightOverlap)            this->setOverlap (leftOverlap);\
        else if (leftUnit.isConstant (channel))     this->setOverlap (rightOverlap);\
        else if (rightUnit.isConstant (channel))    this->setOverlap (leftOverlap);\
        else plonk_assertfalse;\
        this->initValue (pl_##PLANKOP##F (leftValue, rightValue));\
    }\
    \
    void process (ProcessInfo& info, const int channel) throw() {\
        UnitType& leftUnit (this->getInputAsUnit (IOKey::LeftOperand));\
        UnitType& rightUnit (this->getInputAsUnit  (IOKey::RightOperand));\
        \
        const Buffer& leftBuffer (leftUnit.process (info, channel));\
        const Buffer& rightBuffer (rightUnit.process (info, channel));\
        \
        p.buffers[0].bufferSize = this->getOutputBuffer().length();\
        p.buffers[0].buffer = this->getOutputSamples();\
        p.buffers[1].bufferSize = leftBuffer.length();\
        p.buffers[1].buffer = leftBuffer.getArray();\
        p.buffers[2].bufferSize = rightBuffer.length();\
        p.buffers[2].buffer = rightBuffer.getArray();\
        \
        PLONK_PLINK_BINARYOPCHANNEL_FUNCTION(PLANKOP,F) (&p,0);\
    }

#define PLONK_PLINK_BINARYOPCHANNEL(PLONKOP, PLANKOP)\
    PLONK_PLINK_BINARYOPCHANNEL_COMMON_START(PLONKOP)\
    PLONK_PLINK_BINARYOPCHANNEL_PROCESS(PLONKOP, PLANKOP)\
    PLONK_PLINK_BINARYOPCHANNEL_COMMON_END

PLONK_PLINK_BINARYOPCHANNEL(plonk::addop, Add);
PLONK_PLINK_BINARYOPCHANNEL(plonk::subop, Sub);
PLONK_PLINK_BINARYOPCHANNEL(plonk::mulop, Mul);
PLONK_PLINK_BINARYOPCHANNEL(plonk::divop, Div);
PLONK_PLINK_BINARYOPCHANNEL(plonk::min, Min);
PLONK_PLINK_BINARYOPCHANNEL(plonk::max, Max);
PLONK_PLINK_BINARYOPCHANNEL(plonk::pow, Pow);
PLONK_PLINK_BINARYOPCHANNEL(plonk::isEqualTo, IsEqualTo);
PLONK_PLINK_BINARYOPCHANNEL(plonk::isNotEqualTo, IsNotEqualTo);
PLONK_PLINK_BINARYOPCHANNEL(plonk::isGreaterThan, IsGreaterThan);
PLONK_PLINK_BINARYOPCHANNEL(plonk::isGreaterThanOrEqualTo, IsGreaterThanOrEqualTo);
PLONK_PLINK_BINARYOPCHANNEL(plonk::isLessThan, IsLessThan);
PLONK_PLINK_BINARYOPCHANNEL(plonk::isLessThanOrEqualTo, IsLessThanOrEqualTo);
PLONK_PLINK_BINARYOPCHANNEL(plonk::hypot, Hypot);
PLONK_PLINK_BINARYOPCHANNEL(plonk::atan2, Atan2);
PLONK_PLINK_BINARYOPCHANNEL(plonk::sumsqr, SumSqr);
PLONK_PLINK_BINARYOPCHANNEL(plonk::difsqr, DifSqr);
PLONK_PLINK_BINARYOPCHANNEL(plonk::sqrsum, SqrSum);
PLONK_PLINK_BINARYOPCHANNEL(plonk::sqrdif, SqrDif);
PLONK_PLINK_BINARYOPCHANNEL(plonk::absdif, AbsDif);
PLONK_PLINK_BINARYOPCHANNEL(plonk::thresh, Thresh);


#endif // PLONK_BINARYOPPLINK_H
