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

#ifndef PLONK_SCHMIDTCHANNEL_H
#define PLONK_SCHMIDTCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class SchmidtChannelInternal;

PLONK_CHANNELDATA_DECLARE(SchmidtChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;
    SampleType currentState;
};

/** Schmidt. */
template<class SampleType>
class SchmidtChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(SchmidtChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(SchmidtChannelInternal,SampleType)   Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef SchmidtChannelInternal<SampleType>                          SchmidtInternal;
    typedef ChannelInternal<SampleType,Data>                            Internal;
    typedef ChannelInternalBase<SampleType>                             InternalBase;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
            
    SchmidtChannelInternal (Inputs const& inputs, 
                            Data const& data,
                            BlockSize const& blockSize,
                            SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Schmidt";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Minimum, IOKey::Maximum);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new SchmidtInternal (channelInputs, 
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

        UnitType& minimumUnit (this->getInputAsUnit (IOKey::Minimum));
        const Buffer& minimumBuffer (minimumUnit.process (info, channel));
        const SampleType* minimumSamples = minimumBuffer.getArray();
        const int minimumBufferLength = minimumBuffer.length();

        UnitType& maximumUnit (this->getInputAsUnit (IOKey::Maximum));
        const Buffer& maximumBuffer (maximumUnit.process (info, channel));
        const SampleType* maximumSamples = maximumBuffer.getArray();
        const int maximumBufferLength = maximumBuffer.length();

        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const SampleType sampleZero = Math<SampleType>::get0();
        const SampleType sampleOne = Math<SampleType>::get1();
        SampleType currentState = data.currentState;
        
        int i = 0;
        
        if (inputBufferLength == outputBufferLength)
        {
            if ((inputBufferLength == minimumBufferLength) &&
                (inputBufferLength == maximumBufferLength))
            {
                for (i = 0; i < outputBufferLength; ++i)
                {
                    if (currentState == sampleZero)
                    {
                        if (inputSamples[i] > maximumSamples[i])
                            currentState = sampleOne;
                    }
                    else
                    {
                        if (inputSamples[i] < minimumSamples[i])
                            currentState = sampleZero;
                    }

                    outputSamples[i] = currentState;
                }
            }
            else if ((minimumBufferLength == 1) &&
                     (maximumBufferLength == 1))
            {
                const SampleType minimum = minimumSamples[0];
                const SampleType maximum = maximumSamples[0];
                
                for (i = 0; i < outputBufferLength; ++i)
                {
                    if (currentState == sampleZero)
                    {
                        if (inputSamples[i] > maximum)
                            currentState = sampleOne;
                    }
                    else
                    {
                        if (inputSamples[i] < minimum)
                            currentState = sampleZero;
                    }
                    
                    outputSamples[i] = currentState;
                }
            }
        }
        else if (i == 0) // none of the other routines executed..
        {
            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
            double minimumPosition = 0.0;
            const double minimumIncrement = double (minimumBufferLength) / double (outputBufferLength);
            double maximumPosition = 0.0;
            const double maximumIncrement = double (maximumBufferLength) / double (outputBufferLength);
   
            for (i = 0; i < outputBufferLength; ++i)
            {
                if (currentState == sampleZero)
                {
                    if (inputSamples[int (inputPosition)] > maximumSamples[int (maximumPosition)])
                        currentState = sampleOne;
                }
                else
                {
                    if (inputSamples[int (inputPosition)] < minimumSamples[int (minimumPosition)])
                        currentState = sampleZero;
                }
                
                outputSamples[i] = currentState;
                
                inputPosition += inputIncrement;
                minimumPosition += minimumIncrement;
                maximumPosition += maximumIncrement;
            }
        }
                            
                        
        data.currentState = currentState;
    }
};

//------------------------------------------------------------------------------

/** Schmidt trigger.
 
 @par Factory functions:
 - ar (input, minimum, maximum, preferredBlockSize=default, preferredSampleRate=default)
 - kr (input, minimum, maximum)
 
 @par Inputs:
 - input: (unit, multi) the input unit within which triggers are detected
 - minimum: (unit, multi) the lower threshold
 - maximum: (unit, multi) the upper threshold
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

 @ingroup ConverterUnits */
template<class SampleType>
class SchmidtUnit
{
public:    
    typedef SchmidtChannelInternal<SampleType>      SchmidtInternal;
    typedef typename SchmidtInternal::Data          Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;    
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Schmidt", "Trigger with hysteresis.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,      IOKey::End,

                         // inputs
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::Minimum,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::Maximum,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,  blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate, Measure::Hertz,    sampleRate,          IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate sample rate converter. */
    static UnitType ar (UnitType const& input,
                        UnitType const& minimum,
                        UnitType const& maximum,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {                                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Minimum, minimum);
        inputs.put (IOKey::Maximum, maximum);
        
        Data data = { { -1.0, -1.0 }, 0 };
                
        return UnitType::template createFromInputs<SchmidtInternal> (inputs, 
                                                                     data,
                                                                     preferredBlockSize,
                                                                     preferredSampleRate);
    }
    
    static PLONK_INLINE_LOW UnitType kr (UnitType const& input,
                               UnitType const& minimum,
                               UnitType const& maximum) throw()
    {
        return ar (input, minimum, maximum,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
    
};

typedef SchmidtUnit<PLONK_TYPE_DEFAULT> Schmidt;



#endif // PLONK_SCHMIDTCHANNEL_H


