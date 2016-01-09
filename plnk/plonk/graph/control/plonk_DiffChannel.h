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

#ifndef PLONK_DIFFCHANNEL_H
#define PLONK_DIFFCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class DiffChannelInternal;

PLONK_CHANNELDATA_DECLARE(DiffChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;    
    SampleType prevValue;
};

/** Difference. */
template<class SampleType>
class DiffChannelInternal
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(DiffChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(DiffChannelInternal,SampleType)      Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef DiffChannelInternal<SampleType>                             DiffInternal;
    typedef ChannelInternal<SampleType,Data>                            Internal;
    typedef ChannelInternalBase<SampleType>                             InternalBase;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
    
    DiffChannelInternal (Inputs const& inputs, 
                         Data const& data,
                         BlockSize const& blockSize,
                         SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Diff";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new DiffInternal (channelInputs,
                                 this->getState(), 
                                 this->getBlockSize(),
                                 this->getSampleRate());
    }    
    
    void initChannel (const int channel) throw()
    {        
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);

        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getSampleRate (channel),
                                                 this->getSampleRate()));
        this->setOverlap (input.getOverlap (channel));
    
        this->initValue (0);
    }    
        
    void process (ProcessInfo& info, const int channel) throw()
    {
        Data& data = this->getState();
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();

        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        SampleType prevValue = data.prevValue;
        
        int i;
        
        if (inputBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i)
            {
                const SampleType currValue = inputSamples[i];
                outputSamples[i] = currValue - prevValue;
                prevValue = currValue;
            }
        }
        else
        {
            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
                        
            for (i = 0; i < outputBufferLength; ++i)
            {
                const SampleType currValue = inputSamples[int (inputPosition)];
                outputSamples[i] = currValue - prevValue;
                
                inputPosition += inputIncrement;
                prevValue = currValue;
            }

        }
                
        data.prevValue = prevValue;
    }
};

//------------------------------------------------------------------------------

/** Diff outputs the difference between successive samples.
 
 
 @par Factory functions:
 - ar (input, preferredBlockSize=noPreference, preferredSampleRate=noPreference)
 - kr (input)
 
 @par Inputs:
 - input: (unit, multi) the input unit
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

 @ingroup FilterUnits */
template<class SampleType>
class DiffUnit
{
public:    
    typedef DiffChannelInternal<SampleType>                 DiffInternal;
    typedef typename DiffInternal::Data                     Data;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Diff", "Diff outputs the difference between successive samples.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,      IOKey::End,

                         // inputs
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,  blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate, Measure::Hertz,    sampleRate,          IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate sample rate differentiator. */
    static UnitType ar (UnitType const& input,
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {                                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        
        Data data = { { -1.0, -1.0 }, 0 };
                
        return UnitType::template createFromInputs<DiffInternal> (inputs,
                                                                  data,
                                                                  preferredBlockSize,
                                                                  preferredSampleRate);
    }
    
    static inline UnitType kr (UnitType const& input) throw()
    {
        return ar (input,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
    
};

typedef DiffUnit<PLONK_TYPE_DEFAULT> Diff;



#endif // PLONK_DIFFCHANNEL_H


