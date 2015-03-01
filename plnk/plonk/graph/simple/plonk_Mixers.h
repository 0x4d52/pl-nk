/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_MIXERS_H
#define PLONK_MIXERS_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class ChannelMixerChannelInternal;

PLONK_CHANNELDATA_DECLARE(ChannelMixerChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    bool allowAutoDelete:1;
};      


/** Mix all channels in a single unit to a single channel. */
template<class SampleType>
class ChannelMixerChannelInternal
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(ChannelMixerChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(ChannelMixerChannelInternal,SampleType)      Data;
    typedef typename BinaryOpFunctionsHelper<SampleType>::BinaryOpFunctionsType BinaryOpFunctionsType;    
    typedef ChannelBase<SampleType>                                             ChannelType;
    typedef ChannelMixerChannelInternal<SampleType>                             ChannelMixerInternal;
    typedef ChannelInternal<SampleType,Data>                                    Internal;
    typedef ChannelInternalBase<SampleType>                                     InternalBase;
    typedef UnitBase<SampleType>                                                UnitType;
    typedef InputDictionary                                                     Inputs;
    typedef NumericalArray<SampleType>                                          Buffer;
    
    ChannelMixerChannelInternal (Inputs const& inputs, 
                                 Data const& data, 
                                 BlockSize const& blockSize,
                                 SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "Channel Mixer";
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
    
    void initChannel (const int /*channel*/) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        
        this->setBlockSize (BlockSize::decide (input.getMaxBlockSize(),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getMaxSampleRate(),
                                                 this->getSampleRate()));      
                
        const int numChannels = input.getNumChannels();
        SampleType value (0);
        
        for (int i = 0; i < numChannels; ++i)
            value += input.getValue (i);
        
        this->initValue (value);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        int i;
        
        this->getOutputBuffer().zero();
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();

        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        
        const int numChannels = inputUnit.getNumChannels();
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            plonk_assert (inputUnit.getOverlap (channel) == Math<DoubleVariable>::get1());
            
            const Buffer& inputBuffer (inputUnit.process (info, channel));
            const SampleType* const inputSamples = inputBuffer.getArray();
            const int inputBufferLength = inputBuffer.length();
            
            if (inputBufferLength == outputBufferLength)
            {
                NumericalArrayBinaryOp<SampleType,BinaryOpFunctionsType::addop>::calcNN (outputSamples, outputSamples, inputSamples, outputBufferLength);                
            }
            else if (inputBufferLength == 1)
            {
                NumericalArrayBinaryOp<SampleType,BinaryOpFunctionsType::addop>::calcN1 (outputSamples, outputSamples, inputSamples[0], outputBufferLength);                
            }
            else
            {
                double inputPosition = 0.0;
                const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
                
                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] += inputSamples[int (inputPosition)];
                    inputPosition += inputIncrement;
                }        
            }
        }
        
        const Data& data = this->getState();
        
        if (data.allowAutoDelete == false)
            info.resetShouldDelete();
    }
};

//------------------------------------------------------------------------------

template<class SampleType> class UnitMixerChannelInternal;

PLONK_CHANNELDATA_DECLARE(UnitMixerChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    int preferredNumChannels;
    bool allowAutoDelete:1;
    bool purgeExpiredUnits:1;
};      


/** Mix an array of units to a multichannel unit. */
template<class SampleType>
class UnitMixerChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(UnitMixerChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(UnitMixerChannelInternal,SampleType)         Data;
    typedef typename BinaryOpFunctionsHelper<SampleType>::BinaryOpFunctionsType BinaryOpFunctionsType;    
    typedef ChannelBase<SampleType>                                             ChannelType;
    typedef ObjectArray<ChannelType>                                            ChannelArrayType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                          Internal;
    typedef UnitBase<SampleType>                                                UnitType;
    typedef InputDictionary                                                     Inputs;
    typedef NumericalArray<SampleType>                                          Buffer;
    typedef NumericalArray2D<ChannelType,UnitType>                              UnitsType;
        
    UnitMixerChannelInternal (Inputs const& inputs, 
                              Data const& data, 
                              BlockSize const& blockSize,
                              SampleRate const& sampleRate,
                              ChannelArrayType& channels) throw()
    :   Internal (data.preferredNumChannels > 0 ? data.preferredNumChannels : inputs.getMaxNumChannels(),
                  inputs, data, blockSize, sampleRate, channels)
    {
    }

    Text getName() const throw()
    {        
        return "Unit Mixer";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Units);
        return keys;
    }    
        
    void initChannel (const int channel) throw()
    {        
        if ((channel % this->getNumChannels()) == 0)
        {
            // should look at the initial array in case no preference was given really... 
            this->setBlockSize (BlockSize::decide (BlockSize::getDefault(),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (SampleRate::getDefault(),
                                                     this->getSampleRate()));      
        }
        
        const Units& units = this->getInputAsUnits (IOKey::Units);

        const int numUnits = units.length();
        SampleType value (0);
        
        for (int i = 0; i < numUnits; ++i)
            value += units.atUnchecked (i).getValue (channel);
        
        this->initProxyValue (channel, value);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        const Data& data = this->getState();

        int i, channel, unit;
        
        UnitsType& units = this->getInputAsUnits (IOKey::Units);
        
        if (data.purgeExpiredUnits)
        {
            // remove nulls...
            for (unit = units.length(); --unit >= 0;)
                if (units.atUnchecked (unit).shouldBeDeletedNow (info))
                    units.remove (unit);
        }
        
        const int numChannels = this->getNumChannels();
        const int numUnits = units.length();
        
        // ..and process.
        for (channel = 0; channel < numChannels; ++channel)
        {
            Buffer& outputBuffer = this->getOutputBuffer (channel);
            outputBuffer.zero();
            SampleType* const outputSamples = outputBuffer.getArray();
            const int outputBufferLength = outputBuffer.length();
            
            for (unit = 0; unit < numUnits; ++unit)
            {
                UnitType& inputUnit (units.atUnchecked (unit));
                
                if (!inputUnit.wrapAt (channel).shouldBeDeletedNow (info.getTimeStamp()))
                {
                    plonk_assert (inputUnit.getOverlap (channel) == Math<DoubleVariable>::get1());
                    
                    const Buffer& inputBuffer (inputUnit.process (info, channel));
                    const SampleType* const inputSamples = inputBuffer.getArray();
                    const int inputBufferLength = inputBuffer.length();
                    
                    if (inputBufferLength == outputBufferLength)
                    {
                        NumericalArrayBinaryOp<SampleType,BinaryOpFunctionsType::addop>::calcNN (outputSamples, outputSamples, inputSamples, outputBufferLength);                
                    }
                    else if (inputBufferLength == 1)
                    {
                        NumericalArrayBinaryOp<SampleType,BinaryOpFunctionsType::addop>::calcN1 (outputSamples, outputSamples, inputSamples[0], outputBufferLength);                
                    }
                    else
                    {
                        double inputPosition = 0.0;
                        const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
                        
                        for (i = 0; i < outputBufferLength; ++i) 
                        {
                            outputSamples[i] += inputSamples[int (inputPosition)];
                            inputPosition += inputIncrement;
                        }        
                    }
                    
                    if (data.allowAutoDelete == false)
                        info.resetShouldDelete();    
                }
            }
        }
    }    
};

//------------------------------------------------------------------------------

template<class SampleType> class QueueMixerChannelInternal;

PLONK_CHANNELDATA_DECLARE(QueueMixerChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;
    int preferredNumChannels;
    bool allowAutoDelete:1;
    bool purgeExpiredUnits:1;
};


/** Mix a queue of units to a multichannel unit. */
template<class SampleType>
class QueueMixerChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(QueueMixerChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(QueueMixerChannelInternal,SampleType)        Data;
    typedef typename BinaryOpFunctionsHelper<SampleType>::BinaryOpFunctionsType BinaryOpFunctionsType;
    typedef ChannelBase<SampleType>                                             ChannelType;
    typedef ObjectArray<ChannelType>                                            ChannelArrayType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                          Internal;
    typedef UnitBase<SampleType>                                                UnitType;
    typedef InputDictionary                                                     Inputs;
    typedef NumericalArray<SampleType>                                          Buffer;
    typedef NumericalArray2D<ChannelType,UnitType>                              UnitsType;
    typedef LockFreeQueue<UnitType>                                             QueueType;
    
    QueueMixerChannelInternal (Inputs const& inputs,
                               Data const& data,
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate,
                               ChannelArrayType& channels) throw()
    :   Internal (data.preferredNumChannels > 0 ? data.preferredNumChannels : inputs.getMaxNumChannels(),
                  inputs, data, blockSize, sampleRate, channels)
    {
    }
    
    Text getName() const throw()
    {
        return "Queue Mixer";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::UnitQueue);
        return keys;
    }
    
    static PLONK_INLINE_LOW const UnitType& getDummy() throw()
    {
        // dummy is a marker so we know we've done the whole queue up to the poiunt that we add this dummy marker
        static UnitType dummy (0); 
        return dummy;
    }
    
    void initChannel (const int channel) throw()
    {
        if ((channel % this->getNumChannels()) == 0)
        {
            // should look at the initial array in case no preference was given really...
            this->setBlockSize (BlockSize::decide (BlockSize::getDefault(),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (SampleRate::getDefault(),
                                                     this->getSampleRate()));
        }
        
        QueueType& queue = this->getInputAsUnitQueue (IOKey::UnitQueue);        
        SampleType value (0);

        if (queue.length() > 0)
        {
            UnitType inputUnit;
            queue.push (getDummy());
            
            while ((inputUnit = queue.pop()) != getDummy())
            {
                value += inputUnit.getValue (channel);
                queue.push (inputUnit);
            }
        }

        this->initProxyValue (channel, value);
    }
        
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        const Data& data = this->getState();
        
        QueueType& queue = this->getInputAsUnitQueue (IOKey::UnitQueue);
        
        const int numChannels = this->getNumChannels();
        int i, channel;
        
        for (channel = 0; channel < numChannels; ++channel)
            this->getOutputBuffer (channel).zero();
        
        if (queue.length() > 0)
        {
            UnitType inputUnit;
            queue.push (getDummy());

            while ((inputUnit = queue.pop()) != getDummy())
            {
                if (!inputUnit.shouldBeDeletedNow (info))
                {
                    plonk_assert (inputUnit.getOverlap (channel) == Math<DoubleVariable>::get1());

                    for (channel = 0; channel < numChannels; ++channel)
                    {
                        const Buffer& inputBuffer (inputUnit.process (info, channel));
                        const SampleType* const inputSamples = inputBuffer.getArray();
                        const int inputBufferLength = inputBuffer.length();
                        
                        Buffer& outputBuffer = this->getOutputBuffer (channel);
                        SampleType* const outputSamples = outputBuffer.getArray();
                        const int outputBufferLength = outputBuffer.length();

                        if (inputBufferLength == outputBufferLength)
                        {
                            NumericalArrayBinaryOp<SampleType,BinaryOpFunctionsType::addop>::calcNN (outputSamples, outputSamples, inputSamples, outputBufferLength);
                        }
                        else if (inputBufferLength == 1)
                        {
                            NumericalArrayBinaryOp<SampleType,BinaryOpFunctionsType::addop>::calcN1 (outputSamples, outputSamples, inputSamples[0], outputBufferLength);
                        }
                        else
                        {
                            double inputPosition = 0.0;
                            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
                            
                            for (i = 0; i < outputBufferLength; ++i)
                            {
                                outputSamples[i] += inputSamples[int (inputPosition)];
                                inputPosition += inputIncrement;
                            }
                        }
                        
                        if (data.allowAutoDelete == false)
                            info.resetShouldDelete();
                    }
                    
                    queue.push (inputUnit);
                }
                else if (!data.purgeExpiredUnits)
                {
                    queue.push (inputUnit);
                }
            }
        }
    }    
};

//------------------------------------------------------------------------------

#ifdef PLONK_USEPLINK
#include "plonk_BinaryOpPlink.h"
#include "plonk_UnaryOpPlink.h"

template<>
class ChannelMixerChannelInternal<float>
:   public ChannelInternal<float, PLONK_CHANNELDATA_NAME(ChannelMixerChannelInternal,float)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(ChannelMixerChannelInternal,float)   Data;
    typedef ChannelBase<float>                                          ChannelType;
    typedef ChannelMixerChannelInternal<float>                          ChannelMixerInternal;
    typedef ChannelInternal<float,Data>                                 Internal;
    typedef ChannelInternalBase<float>                                  InternalBase;
    typedef UnitBase<float>                                             UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<float>                                       Buffer;
    
    typedef BinaryOpFunctionsHelper<float>::BinaryOpFunctionsType       BinaryOpFunctionsType;    
    typedef UnaryOpFunctionsHelper<float>::UnaryOpFunctionsType         UnaryOpFunctionsType;    

    typedef BinaryOpChannelInternal<float,BinaryOpFunctionsType::addop> BinaryOpChannel;
    typedef BinaryOpChannel::Process                                    Process;
    typedef UnaryOpChannelInternal<float,UnaryOpFunctionsType::move>    UnaryOpChannel;
    typedef UnaryOpChannel::Process                                     UnaryProcess;
    
    ChannelMixerChannelInternal (Inputs const& inputs, 
                                 Data const& data, 
                                 BlockSize const& blockSize,
                                 SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
        plonk_staticassert (BinaryOpChannel::NumBuffers == (BinaryOpChannel::NumInputs + BinaryOpChannel::NumOutputs));
        Process::init (&p, this, BinaryOpChannel::NumOutputs, BinaryOpChannel::NumInputs);
    }
    
    Text getName() const throw()
    {        
        return "Channel Mixer";
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
    
    void initChannel (const int /*channel*/) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        
        this->setBlockSize (BlockSize::decide (input.getMaxBlockSize(),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getMaxSampleRate(),
                                                 this->getSampleRate()));      
        
        const int numChannels = input.getNumChannels();
        float value = 0.f;
        
        for (int i = 0; i < numChannels; ++i)
            value += input.getValue (i);
        
        this->initValue (value);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {        
        float* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        pl_VectorClearF_N (outputSamples, outputBufferLength);
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const int numChannels = inputUnit.getNumChannels();
        
        p.buffers[0].bufferSize = outputBufferLength;
        p.buffers[0].buffer     = outputSamples;
        
        // channel 0
        {
            plonk_assert (inputUnit.getOverlap (0) == Math<DoubleVariable>::get1());
            
            const Buffer& inputBuffer (inputUnit.process (info, 0));
            const float* const inputSamples = inputBuffer.getArray();
            const int inputBufferLength = inputBuffer.length();
            
            if (outputBufferLength == inputBufferLength)
            {
                pl_VectorMoveF_NN (outputSamples, inputSamples, outputBufferLength);
            }
            else
            {
                p.buffers[1].bufferSize = inputBuffer.length();
                p.buffers[1].buffer     = inputBuffer.getArray();
                plink_UnaryOpProcessMoveF_Nn (reinterpret_cast<UnaryProcess*>(&p), 0);
            }
        }
        
        p.buffers[1].bufferSize = outputBufferLength;
        p.buffers[1].buffer     = outputSamples;
        
        // channels 1+
        for (int channel = 1; channel < numChannels; ++channel)
        {
            plonk_assert (inputUnit.getOverlap (channel) == Math<DoubleVariable>::get1());
            
            const Buffer& inputBuffer (inputUnit.process (info, channel));
            const float* const inputSamples = inputBuffer.getArray();
            const int inputBufferLength = inputBuffer.length();
            
            if (outputBufferLength == inputBufferLength)
            {
                pl_VectorAddF_NNN (outputSamples, outputSamples, inputSamples, outputBufferLength);
            }
            else
            {
                p.buffers[2].bufferSize = inputBuffer.length();
                p.buffers[2].buffer     = inputBuffer.getArray();
                plink_BinaryOpProcessAddF_NNn (&p, 0);
            }
        }
        
        const Data& data = this->getState();
        
        if (data.allowAutoDelete == false)
            info.resetShouldDelete();
    }
    
private:
    Process p;
};


template<>
class UnitMixerChannelInternal<float>
:   public ProxyOwnerChannelInternal<float, PLONK_CHANNELDATA_NAME(UnitMixerChannelInternal,float)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(UnitMixerChannelInternal,float)      Data;
    typedef ChannelBase<float>                                          ChannelType;
    typedef ObjectArray<ChannelType>                                    ChannelArrayType;
    typedef ProxyOwnerChannelInternal<float,Data>                       Internal;
    typedef UnitBase<float>                                             UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<float>                                       Buffer;
    typedef NumericalArray2D<ChannelType,UnitType>                      UnitsType;
    typedef BinaryOpFunctionsHelper<float>::BinaryOpFunctionsType       BinaryOpFunctionsType;    

    typedef BinaryOpChannelInternal<float,BinaryOpFunctionsType::addop> BinaryOpChannel;
    typedef BinaryOpChannel::Process                                    Process;

    UnitMixerChannelInternal (Inputs const& inputs, 
                              Data const& data, 
                              BlockSize const& blockSize,
                              SampleRate const& sampleRate,
                              ChannelArrayType& channels) throw()
    :   Internal (data.preferredNumChannels > 0 ? data.preferredNumChannels : inputs.getMaxNumChannels(),
                  inputs, data, blockSize, sampleRate, channels)
    {
        plonk_staticassert (BinaryOpChannel::NumBuffers == (BinaryOpChannel::NumInputs + BinaryOpChannel::NumOutputs));
        Process::init (&p, this, BinaryOpChannel::NumOutputs, BinaryOpChannel::NumInputs);
    }
    
    Text getName() const throw()
    {        
        return "Unit Mixer";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Units);
        return keys;
    }    
    
    void initChannel (const int channel) throw()
    {        
        if ((channel % this->getNumChannels()) == 0)
        {
            // should look at the initial array in case no preference was given really... 
            this->setBlockSize (BlockSize::decide (BlockSize::getDefault(),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (SampleRate::getDefault(),
                                                     this->getSampleRate()));      
        }
        
        const Units& units = this->getInputAsUnits (IOKey::Units);
        
        const int numUnits = units.length();
        float value (0.f);
        
        for (int i = 0; i < numUnits; ++i)
            value += units.atUnchecked (i).getValue (channel);
        
        this->initProxyValue (channel, value);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        int channel, unit;
        const Data& data = this->getState();
        UnitsType& units = this->getInputAsUnits (IOKey::Units);
        
        if (data.purgeExpiredUnits)
        {
            // remove nulls...
            for (unit = units.length(); --unit >= 0;)
                if (units.atUnchecked (unit).shouldBeDeletedNow (info))
                    units.remove (unit);
        }
        
        const int numChannels = this->getNumChannels();
        const int numUnits = units.length();
        
        // ..and process.
        for (channel = 0; channel < numChannels; ++channel)
        {
            Buffer& outputBuffer = this->getOutputBuffer (channel);
            float* const outputSamples = outputBuffer.getArray();
            const int outputBufferLength = outputBuffer.length();
            pl_VectorClearF_N (outputSamples, outputBufferLength);
            
            p.buffers[0].bufferSize = outputBufferLength;
            p.buffers[0].buffer     = outputSamples;
            p.buffers[1].bufferSize = outputBufferLength;
            p.buffers[1].buffer     = outputSamples;

            for (unit = 0; unit < numUnits; ++unit)
            {
                UnitType& inputUnit (units.atUnchecked (unit));
                
                if (!inputUnit.wrapAt (channel).shouldBeDeletedNow (info.getTimeStamp()))
                {
                    plonk_assert (inputUnit.getOverlap (channel) == Math<DoubleVariable>::get1());
                    
                    const Buffer& inputBuffer (inputUnit.process (info, channel));
                    const float* const inputSamples = inputBuffer.getArray();
                    const int inputBufferLength = inputBuffer.length();
                    
                    if (outputBufferLength == inputBufferLength)
                    {
                        pl_VectorAddF_NNN (outputSamples, outputSamples, inputSamples, outputBufferLength);
                    }
                    else
                    {
                        p.buffers[2].bufferSize = inputBuffer.length();
                        p.buffers[2].buffer     = inputBuffer.getArray();
                        plink_BinaryOpProcessAddF_NNn (&p, 0);
                    }
                    
                    if (data.allowAutoDelete == false)
                        info.resetShouldDelete();    
                }
            }
        }
    }    
    
private:
    Process p;
};



#endif //PLONK_USEPLINK


//------------------------------------------------------------------------------

/** Mixer. 
 
 @par Factory functions:
 - ar (input, allowAutoDelete=true, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - ar (array, allowAutoDelete=true, purgeNullUnits=true, preferredNumChannels=0, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - ar (queue, allowAutoDelete=true, purgeNullUnits=true, preferredNumChannels=0, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 
 @par Inputs:
 - input: (unit) the input unit to mix
 - array: (units, multi) the array of units to mix
 - queue: (unit queue) the queue of units to mix 
 - allowAutoDelete: (bool) whether this unit can be casued to be deleted by the unit(s) it contains
 - purgeNullUnits: (bool) whether null units are removed from the array during processing
 - preferredNumChannels: (int) force this unit to have a certain number of channels (0= the maximum channel count in array)
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @ingroup MathsUnits */
template<class SampleType>
class MixerUnit
{
public:    
    typedef ChannelMixerChannelInternal<SampleType>     ChannelMixerInternal;
    typedef UnitMixerChannelInternal<SampleType>        UnitMixerInternal;
    typedef QueueMixerChannelInternal<SampleType>       QueueMixerInternal;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef UnitBase<SampleType>                        UnitType;
    typedef NumericalArray2D<ChannelType,UnitType>      UnitsType;
    typedef InputDictionary                             Inputs;
    typedef LockFreeQueue<UnitType>                     QueueType;

    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfos (UnitInfo ("Channel Mixer", "Mixes all channels in an input unit down to a single channel.",
                                    
                                    // output
                                    1, 
                                    IOKey::Generic,          Measure::None,      IOInfo::NoDefault,  IOLimit::None,      IOKey::End,
                                    
                                    // inputs
                                    IOKey::Generic,          Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                                    IOKey::AutoDeleteFlag,   Measure::Bool,      IOInfo::True,       IOLimit::None,
                                    IOKey::Multiply,         Measure::Factor,    1.0,                IOLimit::None,
                                    IOKey::Add,              Measure::None,      0.0,                IOLimit::None,
                                    IOKey::BlockSize,        Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,   1.0,
                                    IOKey::SampleRate,       Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                                    IOKey::End),
                          
                          UnitInfo ("Unit Mixer", "Mixes a queue of units down to a multichannel unit.",
                                    
                                    // output
                                    ChannelCount::VariableChannelCount,
                                    IOKey::Generic,          Measure::None,      IOInfo::NoDefault,  IOLimit::None,      IOKey::End,
                                    
                                    // inputs
                                    IOKey::UnitQueue,        Measure::None,
                                    IOKey::AutoDeleteFlag,   Measure::Bool,      IOInfo::True,       IOLimit::None,
                                    IOKey::Multiply,         Measure::Factor,    1.0,                IOLimit::None,
                                    IOKey::Add,              Measure::None,      0.0,                IOLimit::None,
                                    IOKey::BlockSize,        Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,   1.0,
                                    IOKey::SampleRate,       Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                                    IOKey::End),
                          
                          UnitInfo ("Queue Mixer", "Mixes multichannel units down to a multichannel unit.",
                                    
                                    // output
                                    ChannelCount::VariableChannelCount,     
                                    IOKey::Generic,                 Measure::None,      IOInfo::NoDefault,  IOLimit::None,      IOKey::End,

                                    // inputs
                                    IOKey::Units,                   Measure::None,
                                    IOKey::AutoDeleteFlag,          Measure::Bool,      IOInfo::True,       IOLimit::None,
                                    IOKey::PurgeExpiredUnitsFlag,   Measure::Bool,      IOInfo::True,       IOLimit::None,
                                    IOKey::PreferredNumChannels,    Measure::Count,     0.0,                IOLimit::None,
                                    IOKey::Multiply,                Measure::Factor,    1.0,                IOLimit::None,
                                    IOKey::Add,                     Measure::None,      0.0,                IOLimit::None,
                                    IOKey::BlockSize,               Measure::Samples,   blockSize,          IOLimit::Minimum, Measure::Samples,     1.0,
                                    IOKey::SampleRate,              Measure::Hertz,     sampleRate,         IOLimit::Minimum, Measure::Hertz,       0.0,
                                    IOKey::End));
    }        
    
    
    /** Create an audio rate channel mixer. */
    static UnitType ar (UnitType const& input, 
                        const bool allowAutoDelete = true,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {           
        typedef PLONK_CHANNELDATA_NAME(ChannelMixerChannelInternal,SampleType) Data;

        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
        
        Data data = { { -1.0, -1.0 }, allowAutoDelete };
        
        return UnitType::template createFromInputs<ChannelMixerInternal> (inputs, 
                                                                          data, 
                                                                          preferredBlockSize, 
                                                                          preferredSampleRate);
    }
    
    /** Create an audio rate unit mixer. */
    static UnitType ar (UnitsType const& array, 
                        const bool allowAutoDelete = true,
                        const bool purgeExpiredUnits = true,
                        const int preferredNumChannels = 0,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        typedef PLONK_CHANNELDATA_NAME(UnitMixerChannelInternal,SampleType) Data;

        Inputs inputs;
        inputs.put (IOKey::Units, array);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
        
        Data data = { { -1.0, -1.0 }, preferredNumChannels, allowAutoDelete, purgeExpiredUnits };
        
        return UnitType::template proxiesFromInputs<UnitMixerInternal> (inputs, 
                                                                        data, 
                                                                        preferredBlockSize, 
                                                                        preferredSampleRate);
    }
    
    /** Create an audio rate unit queue mixer. */
    static UnitType ar (QueueType const& queue,
                        const bool allowAutoDelete = true,
                        const bool purgeExpiredUnits = true,
                        const int preferredNumChannels = 0,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        typedef PLONK_CHANNELDATA_NAME(QueueMixerChannelInternal,SampleType) Data;
        
        Inputs inputs;
        inputs.put (IOKey::UnitQueue, queue);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
        
        Data data = { { -1.0, -1.0 }, preferredNumChannels, allowAutoDelete, purgeExpiredUnits };
        
        return UnitType::template proxiesFromInputs<QueueMixerInternal> (inputs,
                                                                         data,
                                                                         preferredBlockSize,
                                                                         preferredSampleRate);
    }

};

typedef MixerUnit<PLONK_TYPE_DEFAULT> Mixer;

#endif // PLONK_MIXERS_H
