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

#ifndef PLONK_BINARYOPCHANNEL_H
#define PLONK_BINARYOPCHANNEL_H

#include "../../maths/plonk_InlineBinaryOps.h"
#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType>
class BinaryOpUtility
{
private:
    typedef SampleType (*Function)(SampleType const&,SampleType const&);
    
    BinaryOpUtility() throw()
    {
        names.put (&plonk::addop<SampleType>,                   "add");
        names.put (&plonk::subop<SampleType>,                   "subtract");
        names.put (&plonk::mulop<SampleType>,                   "multiply");
        names.put (&plonk::divop<SampleType>,                   "divide");
        names.put (&plonk::modop<SampleType>,                   "modulo");
        names.put (&plonk::isEqualTo<SampleType>,               "==");
        names.put (&plonk::isNotEqualTo<SampleType>,            "=");
        names.put (&plonk::isGreaterThan<SampleType>,           ">");
        names.put (&plonk::isGreaterThanOrEqualTo<SampleType>,  ">=");
        names.put (&plonk::isLessThan<SampleType>,              "<");
        names.put (&plonk::isLessThanOrEqualTo<SampleType>,     "<=");
        names.put (&plonk::hypot<SampleType>,                   "hypot");
        names.put (&plonk::pow<SampleType>,                     "pow");
        names.put (&plonk::atan2<SampleType>,                   "atan2");
        names.put (&plonk::min<SampleType>,                     "min");
        names.put (&plonk::max<SampleType>,                     "max");
        names.put (&plonk::sumsqr<SampleType>,                  "sumsqr");
        names.put (&plonk::difsqr<SampleType>,                  "difsqr");
        names.put (&plonk::sqrsum<SampleType>,                  "sqrsum");
        names.put (&plonk::sqrdif<SampleType>,                  "sqrdif");
        names.put (&plonk::absdif<SampleType>,                  "absdif");
        names.put (&plonk::thresh<SampleType>,                  "thresh");
        names.put (&plonk::round<SampleType>,                   "round");
        names.put (&plonk::trunc<SampleType>,                   "trunc");
        names.put (&plonk::clip2<SampleType>,                   "clip2");
        names.put (&plonk::decayFeedback<SampleType>,           "decayFeedback");
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
    
    static BinaryOpUtility& global() throw()
    {
        static BinaryOpUtility util;
        return util;
    }
    
private:
    Dictionary<Text, Function> names;
};


//------------------------------------------------------------------------------


/** For applying binary operators. */
template<class SampleType, PLONK_BINARYOPFUNCTION(SampleType, op)>
class BinaryOpChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data      Data;

    typedef ChannelBase<SampleType>                 ChannelType;
    typedef BinaryOpChannelInternal<SampleType,op>  BinaryOpInternal;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternalBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    typedef BinaryOpUtility<SampleType>             UtilityType;
        
    BinaryOpChannelInternal (Inputs const& inputs, 
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
        
        return "Binary Operator (" + variant + ")";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::LeftOperand,
                             IOKey::RightOperand);
        return keys;
    }
    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new BinaryOpInternal (channelInputs, 
                                     this->getState(), 
                                     this->getBlockSize(), 
                                     this->getSampleRate());
    }        
    
    void initChannel(const int channel) throw()
    {
        const UnitType& leftUnit = this->getInputAsUnit (IOKey::LeftOperand);
        const SampleType leftValue = leftUnit.getValue (channel);
        const UnitType& rightUnit = this->getInputAsUnit (IOKey::RightOperand);
        const SampleType rightValue = rightUnit.getValue (channel);

        this->setBlockSize (BlockSize::decide (leftUnit.getBlockSize (channel).selectMax (rightUnit.getBlockSize (channel)),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (leftUnit.getSampleRate (channel).selectMax (rightUnit.getSampleRate (channel)),
                                                 this->getSampleRate()));                        
            
        const DoubleVariable leftOverlap = leftUnit.getOverlap (channel);
        const DoubleVariable rightOverlap = rightUnit.getOverlap (channel);

        if (leftOverlap == rightOverlap)
            this->setOverlap (leftOverlap);
        else if (leftUnit.isConstant (channel))
            this->setOverlap (rightOverlap);
        else if (rightUnit.isConstant (channel))
            this->setOverlap (leftOverlap);
        else
            plonk_assertfalse;
        
        this->initValue (op (leftValue, rightValue));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        UnitType& leftUnit (this->getInputAsUnit (IOKey::LeftOperand));
        UnitType& rightUnit (this->getInputAsUnit (IOKey::RightOperand));
        
        const Buffer& leftBuffer (leftUnit.process (info, channel));
        const Buffer& rightBuffer (rightUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const SampleType* const leftSamples = leftBuffer.getArray();
        const int leftBufferLength = leftBuffer.length();
        const SampleType* const rightSamples = rightBuffer.getArray();
        const int rightBufferLength = rightBuffer.length();

        int i;
        
        if ((leftBufferLength == outputBufferLength) && (rightBufferLength == outputBufferLength))
        {
            NumericalArrayBinaryOp<SampleType,op>::calcNN (outputSamples, leftSamples, rightSamples, outputBufferLength);
        }
        else if (rightBufferLength == outputBufferLength)
        {
            if (leftBufferLength == 1)
            {
                NumericalArrayBinaryOp<SampleType,op>::calc1N (outputSamples, leftSamples[0], rightSamples, outputBufferLength);
            }
            else
            {
                double leftPosition = 0.0;
                const double leftIncrement = double (leftBufferLength) / double (outputBufferLength);
                
                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] = op (leftSamples[int (leftPosition)],
                                           rightSamples[i]);
                    
                    leftPosition += leftIncrement;
                }                     
            }
        }
        else if (leftBufferLength == outputBufferLength)
        {
            if (rightBufferLength == 1)
            {
                NumericalArrayBinaryOp<SampleType,op>::calcN1 (outputSamples, leftSamples, rightSamples[0], outputBufferLength);
            }
            else
            {
                double rightPosition = 0.0;
                const double rightIncrement = double (rightBufferLength) / double (outputBufferLength);

                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] = op (leftSamples[i],
                                           rightSamples[int (rightPosition)]);
                    
                    rightPosition += rightIncrement;
                }                     
            }
        }
        else
        {
            double leftPosition = 0.0;
            const double leftIncrement = double (leftBufferLength) / double (outputBufferLength);
            
            double rightPosition = 0.0;
            const double rightIncrement = double (rightBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = op (leftSamples[int (leftPosition)],
                                       rightSamples[int (rightPosition)]);
                
                leftPosition += leftIncrement;
                rightPosition += rightIncrement;
            }     
        }
    }
};


#endif // PLONK_BINARYOPCHANNEL_H
