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

#ifndef PLONK_LINEARPAN_H
#define PLONK_LINEARPAN_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


/** For linear panning across 2 channels. 
 @ingroup ControlUnits */
template<class SampleType>
class LinearPanChannelInternal 
:   public ProxyOwnerChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data              Data;

    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>      Internal;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;

    enum OutputIndices { LeftOutput, RightOutput, NumOutputs };
    
    LinearPanChannelInternal (Inputs const& inputs, 
                              Data const& data, 
                              BlockSize const& blockSize,
                              SampleRate const& sampleRate) throw()
    :   Internal (NumOutputs, inputs, data, blockSize, sampleRate)
    {
    }
        
    Text getName() const throw()
    {
        return "Linear Pan";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Signal,
                             IOKey::Position);
        return keys;
    }    
    
    void initChannel (const int channel) throw()
    {        
        const UnitType& input = this->getInputAsUnit (IOKey::Signal);
        const UnitType& position = this->getInputAsUnit (IOKey::Position);
                
        if ((channel % this->getNumChannels()) == 0)
        {
            // use sample rate and block size of the input unless another 
            // preference has been indicated on construction
            this->setBlockSize (BlockSize::decide (input.getBlockSize (0),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (input.getSampleRate (0),
                                                     this->getSampleRate()));      
        }
        
        plonk_assert (input.getOverlap (0) == DoubleVariable::getOne());
        plonk_assert (position.getOverlap (0) == DoubleVariable::getOne());

        const SampleType inputValue = input.getValue (0);        
        const SampleType mulAdd (0.5);
        const SampleType rightLevel = position.getValue (0) * mulAdd + mulAdd;
        
        SampleType value (0);
        
        switch (channel)
        {
            case 0: {
                const SampleType leftLevel = SampleType (1.0) - rightLevel;
                value = inputValue * leftLevel;
            } break;
            case 1: {
                value = inputValue * rightLevel;
            } break;
            default: {
                plonk_assertfalse;
            } break;
        }
        
        this->initProxyValue (channel, value);
    }    
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {                
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Signal));
        UnitType& positionUnit (this->getInputAsUnit (IOKey::Position));
        
        const Buffer inputBuffer (inputUnit.process (info, 0));
        const Buffer positionBuffer (positionUnit.process (info, 0));
        
        const SampleType mulAdd (0.5);
        const SampleType rightLevel = positionBuffer.atUnchecked (0) * mulAdd + mulAdd;
        const SampleType leftLevel = SampleType (1.0) - rightLevel;
        
        const SampleType* const inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();

        SampleType* const leftOutputSamples = this->getOutputSamples (LeftOutput);
        SampleType* const rightOutputSamples = this->getOutputSamples (RightOutput);
        const int outputBufferLength = this->getOutputBuffer (0).length();
                
        int i;
        
        if (inputBufferLength == outputBufferLength)
        {        
            for (i = 0; i < outputBufferLength; ++i)
            {
                const SampleType inputValue = inputSamples[i];
                leftOutputSamples[i] = leftLevel * inputValue;
                rightOutputSamples[i] = rightLevel * inputValue;
            }
        }
        else 
        {
            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);

            for (i = 0; i < outputBufferLength; ++i)
            {
                const SampleType inputValue = inputSamples[int(inputPosition)];
                
                leftOutputSamples[i] = leftLevel * inputValue;
                rightOutputSamples[i] = rightLevel * inputValue;
                
                inputPosition += inputIncrement;
            }
        }
    }    
};



//------------------------------------------------------------------------------

template<class SampleType>
class LinearPanUnit
{
public:
    typedef typename ChannelInternalCore::Data      Data;

    typedef LinearPanChannelInternal<SampleType>    LinearPanInternal;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::noPreference().getValue();
        const double sampleRate = SampleRate::noPreference().getValue();
        const double peak = (double)TypeUtility<SampleType>::getTypePeak(); // will be innaccurate for LongLong

        return UnitInfo ("LinearPan", "Pan a mono signal across two channels using a linear pan.",
                        
                         // outputs
                         2, 
                         IOKey::LeftOperand,    Measure::None,          IOInfo::NoDefault,  IOLimit::None,
                         IOKey::RightOperand,   Measure::None,          IOInfo::NoDefault,  IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Signal,     Measure::None,              IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Position,   Measure::NormalisedBipolar, 0.0,                IOLimit::Clipped, Measure::NormalisedBipolar,  -peak, peak,
                         IOKey::BlockSize,  Measure::Samples,           blockSize,          IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::SampleRate, Measure::Hertz,             sampleRate,         IOLimit::Minimum, Measure::Hertz,               0.0,
                         IOKey::End);
    }        
    
    static UnitType ar (UnitType const& input,
                        UnitType const& position = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {               
        /*
         for full templating we'd need to add template params for position unit types possibly not for this type though
         */

        const int numInputChannels = plonk::max (input.getNumChannels(), position.getNumChannels());
        UnitType result (UnitType::emptyWithAllocatedSize (numInputChannels * 2));
        Data data = { -1.0, -1.0 };

        for (int i = 0; i < numInputChannels; ++i)
        {
            Inputs inputs;
            inputs.put (IOKey::Signal, input[i]);
            inputs.put (IOKey::Position, position[i]);

            result.add (UnitType::template proxiesFromInputs<LinearPanInternal> (inputs, 
                                                                                 data, 
                                                                                 preferredBlockSize, 
                                                                                 preferredSampleRate));
        }
        
        return result;
    }
        
};

typedef LinearPanUnit<PLONK_TYPE_DEFAULT> LinearPan;


#endif // PLONK_LINEARPAN_H
