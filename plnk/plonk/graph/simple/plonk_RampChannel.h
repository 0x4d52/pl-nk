/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_RAMPCHANNEL_H
#define PLONK_RAMPCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class RampChannelInternal;

PLONK_CHANNELDATA_DECLARE(RampChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;
    SampleType prevValue;
};

template<class SampleType>
class RampChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(RampChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(RampChannelInternal,SampleType)                      Data;
    typedef ChannelBase<SampleType>                                                     ChannelType;
    typedef RampChannelInternal<SampleType>                                             RampChannelInternalType;
    typedef ChannelInternal<SampleType,Data>                                            Internal;
    typedef ChannelInternalBase<SampleType>                                             InternalBase;
    typedef UnitBase<SampleType>                                                        UnitType;
    typedef InputDictionary                                                             Inputs;
    typedef NumericalArray<SampleType>                                                  Buffer;
    
    typedef typename BinaryOpFunctionsHelper<SampleType>::BinaryOpFunctionsType         BinaryOpFunctionsType;
    typedef NumericalArrayBinaryOp<SampleType, BinaryOpFunctionsType::ramp>             ArrayBinaryOpType;
    typedef NumericalArrayFiller<SampleType>                                            ArrayFillType;
    
    RampChannelInternal (Inputs const& inputs,
                         Data const& data,
                         BlockSize const& blockSize,
                         SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "Ramp";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }    
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }        
    
    void initChannel (const int channel) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getSampleRate (channel),
                                                 this->getSampleRate()));
        this->setOverlap (input.getOverlap (channel));
        
        Data& data = this->getState();
        data.prevValue = input.getValue (channel);
        this->initValue (data.prevValue);
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        Data& data = this->getState();
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        
        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const SampleType currentMean = inputBuffer.findMean();
        
        if (currentMean != data.prevValue)
        {
            const SampleType delta = (currentMean - data.prevValue) / outputBufferLength;
            ArrayBinaryOpType::calc11 (outputSamples, data.prevValue, delta, (UnsignedLong) outputBufferLength);
        }
        else
        {
            ArrayFillType::fill (outputSamples, currentMean, (UnsignedLong) outputBufferLength);
        }

        data.prevValue = currentMean;
    }
};



//------------------------------------------------------------------------------

/** Ramp unit.
 
 @par Factory functions:
 - ar (input, preferredBlockSize=default)
 - kr (input)

 @par Inputs:
 - input (unit, multi)

 @ingroup ControlUnits */
template<class SampleType>
class RampUnit
{
public:    
    typedef RampChannelInternal<SampleType>            RampChannelInternalType;
    typedef typename RampChannelInternalType::Data     Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef ChannelInternalBase<SampleType>             InternaBase;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;
    typedef NumericalArray<SampleType>                  Buffer;
    typedef Variable<SampleType>                        VariableType;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();

        return UnitInfo ("Ramp", "A linear ramp to the mean",
                         
                         // output
                         1, 
                         IOKey::Generic,        Measure::None,      IOInfo::NoDefault,  IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,        Measure::None,      0.0,                IOLimit::None,
                         IOKey::BlockSize,      Measure::Samples,   blockSize,          IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::End);
    }    
    
    /** Create audio rate parameter. */
    static UnitType ar (Unit const& input,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        
        Data data = { { -1.0, -1.0 } };
        
        return UnitType::template createFromInputs<RampChannelInternalType> (inputs, 
                                                                             data,
                                                                             preferredBlockSize,
                                                                             preferredSampleRate);
    }
    
    static UnitType kr (Unit const& input) throw()
    {
        return ar (input);
    }
};
    
typedef RampUnit<PLONK_TYPE_DEFAULT> Ramp;



#endif // PLONK_RAMPCHANNEL_H


