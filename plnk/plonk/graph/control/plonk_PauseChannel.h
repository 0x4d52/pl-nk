/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

#ifndef PLONK_PAUSECHANNEL_H
#define PLONK_PAUSECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Pause. */
template<class SampleType>
class PauseChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data              Data;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef PauseChannelInternal<SampleType>                PauseInternal;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef ChannelInternalBase<SampleType>                 InternalBase;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
            
    PauseChannelInternal (Inputs const& inputs, 
                          Data const& data,
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Pause";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Gate);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new PauseInternal (channelInputs, 
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
        UnitType& gateUnit (this->getInputAsUnit (IOKey::Gate));
        const Buffer& gateBuffer (gateUnit.process (info, channel));
        const SampleType* gateSamples = gateBuffer.getArray();
        const int gateBufferLength = gateBuffer.length();

        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const SampleType sampleZero = Math<SampleType>::get0();
        
        bool done = false;
        
        if (gateBufferLength == 1)
        {
            if (gateSamples[0] > sampleZero)
            {
                UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
                const Buffer& inputBuffer (inputUnit.process (info, channel));
                const SampleType* inputSamples = inputBuffer.getArray();
                const int inputBufferLength = inputBuffer.length();

                if (inputBufferLength == outputBufferLength)
                {
                    Buffer::copyData (outputSamples, inputSamples, outputBufferLength);
                    done = true;
                }
            }
            else
            {
                this->getOutputBuffer().zero();
                done = true;
            }
        }
        
        if (! done)
        {
            UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
            const Buffer& inputBuffer (inputUnit.process (info, channel));
            const SampleType* inputSamples = inputBuffer.getArray();
            const int inputBufferLength = inputBuffer.length();

            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
            double gatePosition = 0.0;
            const double gateIncrement = double (gateBufferLength) / double (outputBufferLength);
            
            for (int i = 0; i < outputBufferLength; ++i)
            {
                outputSamples[i] = gateSamples[int (gatePosition)] > sampleZero ? inputSamples[int (inputPosition)] : sampleZero;
                
                inputPosition += inputIncrement;
                gatePosition += gateIncrement;
            }
        }
    }
};

//------------------------------------------------------------------------------

/** Pause.
 
 @par Factory functions:
 - ar (input, gate, preferredBlockSize=noPreference, preferredSampleRate=noPreference)
 - kr (input, gate)
 
 @par Inputs:
 - input: (unit, multi) the input unit
 - gate: (unit) the gate
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

 @ingroup ConverterUnits */
template<class SampleType>
class PauseUnit
{
public:    
    typedef PauseChannelInternal<SampleType>        PauseInternal;
    typedef typename PauseInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;    
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Pause", "Pause sub-graph execution.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,      IOKey::End,

                         // inputs
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::Gate,       Measure::Bool,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,  blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate, Measure::Hertz,    sampleRate,          IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate pause. */
    static UnitType ar (UnitType const& input,
                        UnitType const& gate,
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {                                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Gate, gate);
        
        Data data = { -1.0, -1.0 };
                
        return UnitType::template createFromInputs<PauseInternal> (inputs, 
                                                                   data,
                                                                   preferredBlockSize,
                                                                   preferredSampleRate);
    }
    
    static inline UnitType kr (UnitType const& input,
                               UnitType const& gate) throw()
    {
        return ar (input, gate,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
    
};

typedef PauseUnit<PLONK_TYPE_DEFAULT> Pause;

#endif // PLONK_PAUSECHANNEL_H


