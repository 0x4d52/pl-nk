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

#ifndef PLONK_MULADDCHANNEL_H
#define PLONK_MULADDCHANNEL_H

#include "../../maths/plonk_InlineBinaryOps.h"
#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** For applying mul and add in one go. */
template<class SampleType>
class MulAddChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data      Data;

    typedef ChannelBase<SampleType>                 ChannelType;
    typedef MulAddChannelInternal<SampleType>       MulAddInternal;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternalBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
        
    MulAddChannelInternal (Inputs const& inputs, 
                           Data const& data, 
                           BlockSize const& blockSize,
                           SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "MulAdd";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic,
                             IOKey::Multiply,
                             IOKey::Add);
        return keys;
    }
    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new MulAddChannelInternal (channelInputs, 
                                     this->getState(), 
                                     this->getBlockSize(), 
                                     this->getSampleRate());
    }        
    
    void initChannel(const int channel) throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const SampleType inputValue = inputUnit.getValue (channel);
        UnitType& multiplyUnit (this->getInputAsUnit (IOKey::Multiply));
        const SampleType multiplyValue = multiplyUnit.getValue (channel);
        UnitType& addUnit (this->getInputAsUnit (IOKey::Add));
        const SampleType addValue = addUnit.getValue (channel);


        const BlockSize tempBlockSize = inputUnit.getBlockSize (channel)
                                        .selectMax (multiplyUnit.getBlockSize (channel))
                                        .selectMax (addUnit.getBlockSize (channel));
        const SampleRate tempSampleRate = inputUnit.getSampleRate (channel)
                                          .selectMax (multiplyUnit.getSampleRate (channel))
                                          .selectMax (addUnit.getSampleRate (channel));
        
        this->setBlockSize (BlockSize::decide (tempBlockSize, this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (tempSampleRate, this->getSampleRate()));                        
            
        const DoubleVariable inputOverlap = inputUnit.getOverlap (channel);
        const DoubleVariable multiplyOverlap = multiplyUnit.getOverlap (channel);
        const DoubleVariable addOverlap = addUnit.getOverlap (channel);

        if ((inputOverlap == multiplyOverlap) && (inputOverlap == addOverlap))
            this->setOverlap (inputOverlap);
        else if ((inputUnit.isConstant (channel)) && (multiplyUnit.isConstant (channel)))
            this->setOverlap (addOverlap);
        else if ((inputUnit.isConstant (channel)) && (addUnit.isConstant (channel)))
            this->setOverlap (multiplyOverlap);
        else if ((addUnit.isConstant (channel)) && (multiplyUnit.isConstant (channel)))
            this->setOverlap (inputOverlap);
        else
            plonk_assertfalse;
        
        this->initValue (inputValue * multiplyValue + addValue);
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        UnitType& multiplyUnit (this->getInputAsUnit (IOKey::Multiply));
        UnitType& addUnit (this->getInputAsUnit (IOKey::Add));

        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const Buffer& multiplyBuffer (multiplyUnit.process (info, channel));
        const Buffer& addBuffer (addUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const SampleType* const inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();
        const SampleType* const multiplySamples = multiplyBuffer.getArray();
        const int multiplyBufferLength = multiplyBuffer.length();
        const SampleType* const addSamples = addBuffer.getArray();
        const int addBufferLength = addBuffer.length();

        int i;
        
        if ((inputBufferLength == outputBufferLength) && 
            (multiplyBufferLength == outputBufferLength) &&
            (addBufferLength == outputBufferLength))
        {
            // NNNN
            for (i = 0; i < outputBufferLength; ++i) 
                outputSamples[i] = inputSamples[i] * multiplySamples[i] + addSamples[i];
        }
        else if ((inputBufferLength == outputBufferLength) && 
                 (multiplyBufferLength == 1) &&
                 (addBufferLength == 1))
        {
            // NN11
            const SampleType multiplyValue (multiplyBuffer[0]);
            const SampleType addValue (addBuffer[0]);
            
            for (i = 0; i < outputBufferLength; ++i) 
                outputSamples[i] = inputSamples[i] * multiplyValue + addValue;
  
        }
        else if ((inputBufferLength == outputBufferLength) && 
                 (multiplyBufferLength == outputBufferLength) &&
                 (addBufferLength == 1))
        {
            // NNN1
            const SampleType addValue (addBuffer[0]);

            for (i = 0; i < outputBufferLength; ++i) 
                outputSamples[i] = inputSamples[i] * multiplySamples[i] + addValue;
        }
        else if ((inputBufferLength == outputBufferLength) && 
                 (multiplyBufferLength == 1) &&
                 (addBufferLength == outputBufferLength))
        {
            // NN1N
            const SampleType multiplyValue (multiplyBuffer[0]);

            for (i = 0; i < outputBufferLength; ++i) 
                outputSamples[i] = inputSamples[i] * multiplyValue + addSamples[i];
        }
        else goto fallback;
        
        return;
        
    fallback:
        {
            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
            double multiplyPosition = 0.0;
            const double multiplyIncrement = double (multiplyBufferLength) / double (outputBufferLength);
            double addPosition = 0.0;
            const double addIncrement = double (addBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = inputSamples[int (inputPosition)] * 
                                   multiplySamples[int (multiplyPosition)] + 
                                   addSamples[int (addPosition)];
                
                inputPosition += inputIncrement;
                multiplyPosition += multiplyIncrement;
                addPosition += addIncrement;
            }     
        }
    }
};

//------------------------------------------------------------------------------

#ifdef PLONK_USEPLINK

template<>
class MulAddChannelInternal<float>
:   public ChannelInternal<float, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data               Data;    
    typedef ChannelBase<float>                      ChannelType;
    typedef MulAddChannelInternal<float>            MulAddInternal;
    typedef ChannelInternal<float,Data>             Internal;
    typedef ChannelInternalBase<float>              InternalBase;
    typedef UnitBase<float>                         UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<float>                   Buffer;
    
    enum Outputs { Output, NumOutputs };
    enum InputIndices  { Input, Multiply, Add, NumInputs };
    enum Buffers { OutputBuffer, InputBuffer, MultiplyBuffer, AddBuffer, NumBuffers };
    
    typedef PlinkProcess<NumBuffers>                Process;

    MulAddChannelInternal (Inputs const& inputs, 
                           Data const& data, 
                           BlockSize const& blockSize,
                           SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
        plonk_staticassert (NumBuffers == (NumInputs + NumOutputs));
        Process::init (&p, this, NumOutputs, NumInputs);
    }
    
    Text getName() const throw()
    {        
        return "MulAdd";
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic,
                             IOKey::Multiply,
                             IOKey::Add);
        return keys;
    }
    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new MulAddChannelInternal (channelInputs, 
                                          this->getState(), 
                                          this->getBlockSize(), 
                                          this->getSampleRate());
    }        
    
    void initChannel(const int channel) throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const float inputValue = inputUnit.getValue (channel);
        UnitType& multiplyUnit (this->getInputAsUnit (IOKey::Multiply));
        const float multiplyValue = multiplyUnit.getValue (channel);
        UnitType& addUnit (this->getInputAsUnit (IOKey::Add));
        const float addValue = addUnit.getValue (channel);
        
        const BlockSize tempBlockSize = inputUnit.getBlockSize (channel)
                                        .selectMax (multiplyUnit.getBlockSize (channel))
                                        .selectMax (addUnit.getBlockSize (channel));
        const SampleRate tempSampleRate = inputUnit.getSampleRate (channel)
                                          .selectMax (multiplyUnit.getSampleRate (channel))
                                          .selectMax (addUnit.getSampleRate (channel));
        
        this->setBlockSize (BlockSize::decide (tempBlockSize, this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (tempSampleRate, this->getSampleRate()));                        
        
        const DoubleVariable inputOverlap = inputUnit.getOverlap (channel);
        const DoubleVariable multiplyOverlap = multiplyUnit.getOverlap (channel);
        const DoubleVariable addOverlap = addUnit.getOverlap (channel);
        
        if ((inputOverlap == multiplyOverlap) && (inputOverlap == addOverlap))
            this->setOverlap (inputOverlap);
        else if ((inputUnit.isConstant (channel)) && (multiplyUnit.isConstant (channel)))
            this->setOverlap (addOverlap);
        else if ((inputUnit.isConstant (channel)) && (addUnit.isConstant (channel)))
            this->setOverlap (multiplyOverlap);
        else if ((addUnit.isConstant (channel)) && (multiplyUnit.isConstant (channel)))
            this->setOverlap (inputOverlap);
        else
            plonk_assertfalse;
        
        this->initValue (inputValue * multiplyValue + addValue);
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        UnitType& multiplyUnit (this->getInputAsUnit (IOKey::Multiply));
        UnitType& addUnit (this->getInputAsUnit (IOKey::Add));
        
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const Buffer& multiplyBuffer (multiplyUnit.process (info, channel));
        const Buffer& addBuffer (addUnit.process (info, channel));
                
        p.buffers[0].bufferSize = this->getOutputBuffer().length();;
        p.buffers[0].buffer     = this->getOutputSamples();
        p.buffers[1].bufferSize = inputBuffer.length();
        p.buffers[1].buffer     = inputBuffer.getArray();
        p.buffers[2].bufferSize = multiplyBuffer.length();
        p.buffers[2].buffer     = multiplyBuffer.getArray();
        p.buffers[3].bufferSize = addBuffer.length();
        p.buffers[3].buffer     = addBuffer.getArray();

        plink_MulAddProcessF (&p, 0);
    }
    
private:
    Process p;
};

#endif // PLONK_USEPLINK

//------------------------------------------------------------------------------

template<class SampleType>
class MulAddUnit
{
public:    
    typedef MulAddChannelInternal<SampleType>       MulAddInternal;
    typedef typename MulAddInternal::Data           Data;
    typedef InputDictionary                         Inputs;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         ChannelInternalType;
    typedef UnitBase<SampleType>                    UnitType;
        
    /** Create an audio rate multiply-add unit. */
    static UnitType ar (UnitType const& input, 
                        UnitType const& mul,
                        UnitType const& add,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
        
        Data data = { -1.0, -1.0 };
        
        const int numChannels = inputs.getMaxNumChannels();
        UnitType result (UnitType::withSize (numChannels));
        
        for (int i = 0; i < numChannels; ++i) 
        {
            ChannelInternalType* internal = new MulAddInternal (inputs, 
                                                                data, 
                                                                preferredBlockSize, 
                                                                preferredSampleRate);
            internal->initChannel (i);
            result.put (i, ChannelType (internal));
        }
        
        return result;
    }
    
    /** Create a control rate multiply-add unit. */
    static inline UnitType kr (UnitType const& input, 
                               UnitType const& mul,
                               UnitType const& add) throw()
    {
        return ar (input, mul, add, 
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
    
};

typedef MulAddUnit<PLONK_TYPE_DEFAULT> MulAdd;



#endif // PLONK_MULADDCHANNEL_H
