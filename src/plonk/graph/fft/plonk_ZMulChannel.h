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

#ifndef PLONK_ZMULCHANNEL_H
#define PLONK_ZMULCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Complex multiplication channel. */
template<class SampleType>
class ZMulChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef ObjectArray<ChannelType>                            ChannelArrayType;
    typedef ZMulChannelInternal<SampleType>                     ZMulInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>          Internal;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;

    enum InputIndices { RealInput, ImagInput, NumInputs };
    enum OutputIndices { RealOutput, ImagOutput, NumOutputs };
    
    ZMulChannelInternal (Inputs const& inputs,
                         Data const& data,
                         BlockSize const& blockSize,
                         SampleRate const& sampleRate,
                         ChannelArrayType& channels) throw()
    :   Internal (NumOutputs, inputs, data, blockSize, sampleRate, channels)
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
            
    void initChannel (const int channel) throw()
    {
        if ((channel % this->getNumChannels()) == 0)
        {
            const UnitType& leftOperand = this->getInputAsUnit (IOKey::LeftOperand);
            this->setBlockSize (leftOperand.getBlockSize (0));
            this->setSampleRate (leftOperand.getSampleRate (0));    
            this->setOverlap (leftOperand.getOverlap (0));

#ifdef PLONK_DEBUG
            const UnitType& rightOperand = this->getInputAsUnit (IOKey::RightOperand);
            plonk_assert (leftOperand.getBlockSize (0) == leftOperand.getBlockSize (1));
            plonk_assert (leftOperand.getSampleRate (0) == leftOperand.getSampleRate (1));
            plonk_assert (leftOperand.getOverlap (0) == leftOperand.getOverlap (1));
            plonk_assert (leftOperand.getBlockSize (0) == rightOperand.getBlockSize (0));
            plonk_assert (leftOperand.getSampleRate (0) == rightOperand.getSampleRate (0));
            plonk_assert (leftOperand.getOverlap (0) == rightOperand.getOverlap (0));
#endif
        }
        
        this->initProxyValue (channel, SampleType (0)); // todo: should really calculate this...
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        UnitType& leftUnit (this->getInputAsUnit (IOKey::LeftOperand));
        UnitType& rightUnit (this->getInputAsUnit (IOKey::RightOperand));
        
        const Buffer& leftRealBuffer (leftUnit.process (info, RealInput));
        const Buffer& rightRealBuffer (rightUnit.process (info, RealInput));
        const Buffer& leftImagBuffer (leftUnit.process (info, ImagInput));
        const Buffer& rightImagBuffer (rightUnit.process (info, ImagInput));
        
        SampleType* const realOutputSamples = this->getOutputSamples (RealOutput);
        SampleType* const imagOutputSamples = this->getOutputSamples (ImagOutput);
        const int outputBufferLength = this->getOutputBuffer (0).length();
        
        plonk_assert ((outputBufferLength == leftRealBuffer.length()) &&
                      (outputBufferLength == rightRealBuffer.length()) &&
                      (outputBufferLength == leftImagBuffer.length()) &&
                      (outputBufferLength == rightImagBuffer.length()));
        
        const SampleType* const leftRealSamples = leftRealBuffer.getArray();
        const SampleType* const rightRealSamples = rightRealBuffer.getArray();
        const SampleType* const leftImagSamples = leftImagBuffer.getArray();
        const SampleType* const rightImagSamples = rightImagBuffer.getArray();

        NumericalArrayComplex<SampleType>::zmul (realOutputSamples, imagOutputSamples,
                                                 leftRealSamples, rightRealSamples,
                                                 leftImagSamples, rightImagSamples,
                                                 outputBufferLength);
    }
};



//------------------------------------------------------------------------------

/** Complex multiplication Unit. 
 
 @par Factory functions:
 - ar (leftOperand, rightOperand)
 
 @par Inputs:
 - leftOperand: (unit, multi) the left operand in FFT complex format with real data in even channels and imag data in odd channels
 - rightOperand: (unit, multi) the right operand in FFT complex format with real data in even channels and imag data in odd channels
 

 @ingroup ConverterUnits FFTUnits */
template<class SampleType>
class ZMulUnit
{
public:    
    typedef ZMulChannelInternal<SampleType>         ZMulInternal;
    typedef typename ZMulInternal::Data             Data;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
    static inline UnitInfos getInfo() throw()
    {
        return UnitInfo ("ZMul", "Complex multiplication.",
                         
                         // output
                         2, 
                         IOKey::Real,            Measure::Real,          IOInfo::NoDefault,     IOLimit::None,
                         IOKey::Imaginary,       Measure::Imaginary,     IOInfo::NoDefault,     IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::LeftOperand,     Measure::FFTUnpacked,          IOInfo::NoDefault,      IOLimit::None,
                         IOKey::RightOperand,    Measure::FFTUnpacked,          IOInfo::NoDefault,      IOLimit::None,
                         IOKey::End);
    }    
    
    /** Complex multiplication of signals. */
    static inline UnitType ar (UnitType const& leftOperand, UnitType const& rightOperand) throw()
    {                     
        const int numLeftOperandChannels = leftOperand.getNumChannels();
        const int numRightOperandChannels = rightOperand.getNumChannels();

        plonk_assert ((numLeftOperandChannels % 2) == 0);
        plonk_assert ((numRightOperandChannels % 2) == 0);
        
        const int numChannels = plonk::max (numLeftOperandChannels, numRightOperandChannels);
        
        UnitType result (UnitType::emptyWithAllocatedSize (numChannels));
        Data data = { -1.0, -1.0 };

        for (int i = 0; i < numChannels; i += 2)
        {
            Inputs inputs;
            inputs.put (IOKey::LeftOperand,  UnitType (leftOperand[i],  leftOperand[i + 1]));
            inputs.put (IOKey::RightOperand, UnitType (rightOperand[i], rightOperand[i + 1]));
            
            result.add (UnitType::template proxiesFromInputs<ZMulInternal> (inputs,
                                                                            data,
                                                                            BlockSize::noPreference(),
                                                                            SampleRate::noPreference()));
        }
        
        return result;
    }
};

typedef ZMulUnit<PLONK_TYPE_DEFAULT> ZMul;

#endif // PLONK_ZMULCHANNEL_H


