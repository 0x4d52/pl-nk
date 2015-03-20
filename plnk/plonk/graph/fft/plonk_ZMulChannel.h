/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLONK_ZMULCHANNEL_H
#define PLONK_ZMULCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Complex multiplication channel. */
template<class SampleType>
class ZMulChannelInternal
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef ObjectArray<ChannelType>                            ChannelArrayType;
    typedef ZMulChannelInternal<SampleType>                     ZMulInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    
    ZMulChannelInternal (Inputs const& inputs,
                         Data const& data,
                         BlockSize const& blockSize,
                         SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "ZMul";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::LeftOperand, IOKey::RightOperand);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new ZMulInternal (channelInputs,
                                 this->getState(),
                                 this->getBlockSize(),
                                 this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {
        const UnitType& leftOperand = this->getInputAsUnit (IOKey::LeftOperand);
        this->setBlockSize (leftOperand.getBlockSize (channel));
        this->setSampleRate (leftOperand.getSampleRate (channel));
        this->setOverlap (leftOperand.getOverlap (channel));

#ifdef PLONK_DEBUG
        const UnitType& rightOperand = this->getInputAsUnit (IOKey::RightOperand);
        plonk_assert (leftOperand.getBlockSize (channel) == rightOperand.getBlockSize (channel));
        plonk_assert (leftOperand.getSampleRate (channel) == rightOperand.getSampleRate (channel));
        plonk_assert (leftOperand.getOverlap (channel) == rightOperand.getOverlap (channel));
#endif
        
        this->initValue (SampleType (0)); // todo: should really calculate this...
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        UnitType& leftUnit (this->getInputAsUnit (IOKey::LeftOperand));
        UnitType& rightUnit (this->getInputAsUnit (IOKey::RightOperand));
        
        const Buffer& leftBuffer (leftUnit.process (info, channel));
        const Buffer& rightBuffer (rightUnit.process (info, channel));
        
        const int outputBufferLength = this->getOutputBuffer().length();
        const int outputBufferLengthHalved = (unsigned int)outputBufferLength / 2;
        SampleType* const realOutputSamples = this->getOutputSamples();
        SampleType* const imagOutputSamples = realOutputSamples + outputBufferLengthHalved;
        
        plonk_assert ((outputBufferLength == leftBuffer.length()) &&
                      (outputBufferLength == rightBuffer.length()));
        
        const SampleType* const leftRealSamples = leftBuffer.getArray();
        const SampleType* const rightRealSamples = rightBuffer.getArray();
        const SampleType* const leftImagSamples = leftRealSamples + outputBufferLengthHalved;
        const SampleType* const rightImagSamples = rightRealSamples + outputBufferLengthHalved;

        const SampleType leftDC = leftRealSamples[0];
        const SampleType rightDC = rightRealSamples[0];
        const SampleType leftNyquist = leftImagSamples[0];
        const SampleType rightNyquist = rightImagSamples[0];
        
        NumericalArrayComplex<SampleType>::zmul (realOutputSamples, imagOutputSamples,
                                                 leftRealSamples, leftImagSamples,
                                                 rightRealSamples, rightImagSamples,
                                                 outputBufferLengthHalved);
        
        realOutputSamples[0] = leftDC * rightDC;
        imagOutputSamples[0] = leftNyquist * rightNyquist;  
    }
};



//------------------------------------------------------------------------------

/** Complex multiplication Unit. 
 
 @par Factory functions:
 - ar (leftOperand, rightOperand)
 
 @par Inputs:
 - leftOperand: (unit, multi) the left operand in FFT packed complex format
 - rightOperand: (unit, multi) the right operand in FFT packed complex format
 

 @ingroup ConverterUnits FFTUnits */
template<class SampleType>
class ZMulUnit
{
public:    
    typedef ZMulChannelInternal<SampleType>         ZMulInternal;
    typedef typename ZMulInternal::Data             Data;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        return UnitInfo ("ZMul", "Complex multiplication.",
                         
                         // output
                         ChannelCount::VariableChannelCount,
                         IOKey::FFTPacked,       Measure::FFTPacked,          IOInfo::NoDefault,     IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::LeftOperand,     Measure::FFTPacked,          IOInfo::NoDefault,      IOLimit::None,
                         IOKey::RightOperand,    Measure::FFTPacked,          IOInfo::NoDefault,      IOLimit::None,
                         IOKey::End);
    }    
    
    /** Complex multiplication of signals. */
    static PLONK_INLINE_LOW UnitType ar (UnitType const& leftOperand, UnitType const& rightOperand) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::LeftOperand, leftOperand);
        inputs.put (IOKey::RightOperand, rightOperand);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<ZMulInternal> (inputs,
                                                                  data,
                                                                  BlockSize::noPreference(),
                                                                  SampleRate::noPreference());
    }

};

typedef ZMulUnit<PLONK_TYPE_DEFAULT> ZMul;

#endif // PLONK_ZMULCHANNEL_H


