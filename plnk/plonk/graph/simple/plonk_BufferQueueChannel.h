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

#ifndef PLONK_BUFFERQUEUECHANNEL_H
#define PLONK_BUFFERQUEUECHANNEL_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../containers/plonk_ContainerForwardDeclarations.h"

#include "../../core/plonk_SmartPointer.h"
#include "../../core/plonk_WeakPointer.h"

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType>
class QueueBufferBase;

template<class SampleType>
class QueueBufferInternal : public SmartPointer
{
public:
    typedef NumericalArray<SampleType>      Buffer;
    typedef NumericalArray2D<SampleType>    BufferArray;
    
    QueueBufferInternal (const int numChannels, const int blockSize) throw()
    :   buffers (numChannels, blockSize, false)
    {
    }
    
    friend class QueueBufferBase<SampleType>;
    
private:
    BufferArray buffers;
    TimeStamp timestamp;
};

template<class SampleType>
class QueueBufferBase : public SmartPointerContainer< QueueBufferInternal<SampleType> >
{
public:
    typedef QueueBufferInternal<SampleType>     Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef WeakPointerContainer<QueueBuffer>   Weak;
    typedef NumericalArray<SampleType>          Buffer;
    typedef NumericalArray2D<SampleType>        BufferArray;

    QueueBufferBase (const int numChannels = 1, const int blockSize = BlockSize::getDefault().getValue()) throw()
    :   Base (new Internal (numChannels, blockSize))
    {
    }
    
    const int getNumFrames() const throw() { return this->getInternal()->buffers.atUnchecked (0).length(); }
    const int getNumChannels() const throw() { return this->getInternal()->buffers.length(); }
    BufferArray& getBuffers() throw() { return this->getInternal()->buffers; }
    TimeStamp getTimeStamp() const throw() { return this->getInternal()->timestamp; }
    void setTimeStamp (TimeStamp const& timeStamp) throw() { this->getInternal()->timestamp = timeStamp; }
};

template<class SampleType> class BufferQueueRecordChannelInternal;

PLONK_CHANNELDATA_DECLARE(BufferQueueRecordChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;
};


/** Send buffers of recorded data */
template<class SampleType>
class BufferQueueRecordChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(BufferQueueRecordChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(BufferQueueRecordChannelInternal,SampleType)     Data;
    typedef ChannelBase<SampleType>                                                 ChannelType;
    typedef ObjectArray<ChannelType>                                                ChannelArrayType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                              Internal;
    typedef UnitBase<SampleType>                                                    UnitType;
    typedef InputDictionary                                                         Inputs;
    typedef NumericalArray<SampleType>                                              Buffer;
    typedef NumericalArray2D<SampleType>                                            BufferArray;
    typedef QueueBufferBase<SampleType>                                             QueueBufferType;
    typedef LockFreeQueue<QueueBufferType>                                          BufferQueueType;
    
    BufferQueueRecordChannelInternal (Inputs const& inputs,
                                      Data const& data,
                                      BlockSize const& blockSize,
                                      SampleRate const& sampleRate,
                                      ChannelArrayType& channels) throw()
    :   Internal (inputs.getMaxNumChannels(), inputs, data, blockSize, sampleRate, channels),
        currentBuffer (BufferQueueType::getNullValue())
    {
    }
    
    Text getName() const throw()
    {
        return "Buffer Queue Record";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::BufferQueue);
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
    
        this->initProxyValue (channel, SampleType (0));
    }

    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        BufferQueueType& bufferQueue (this->getInputAsBufferQueue (IOKey::BufferQueue));

        if (currentBuffer == BufferQueueType::getNullValue())
            currentBuffer = bufferQueue.pop();
        
        const int numChannels = this->getNumChannels();
        int channel;
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        
        if (currentBuffer != BufferQueueType::getNullValue())
        {
            const int numBufferChannels = currentBuffer.getBuffers().length();
            currentBuffer.setTimeStamp (info.getTimeStamp());
            
            for (channel = 0; channel < numChannels; ++channel)
            {
                const Buffer& inputBuffer (inputUnit.process (info, channel));
                const SampleType* const inputSamples = inputBuffer.getArray();
                
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                SampleType* const outputSamples = outputBuffer.getArray();
                const int outputBufferLength = outputBuffer.length();

                plonk_assert (inputBuffer.length() == outputBufferLength);
                
                Buffer::copyData (outputSamples, inputSamples, outputBufferLength);

                if (channel < numBufferChannels)
                {
                    Buffer& recordBuffer = currentBuffer.getBuffers().atUnchecked (channel);
                    recordBuffer.setSize (outputBufferLength, false);
                    SampleType* const bufferSamples = recordBuffer.getArray();
                    Buffer::copyData (bufferSamples, outputSamples, outputBufferLength);
                }
            }
            
            this->update (Text::getMessageQueueBuffer(), currentBuffer);
            currentBuffer = BufferQueueType::getNullValue();
        }
        else
        {
            this->update (Text::getMessageBufferQueueUnderrun(), bufferQueue);

            for (channel = 0; channel < numChannels; ++channel)
            {
                const Buffer& inputBuffer (inputUnit.process (info, channel));
                const SampleType* const inputSamples = inputBuffer.getArray();
                
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                SampleType* const outputSamples = outputBuffer.getArray();
                const int outputBufferLength = outputBuffer.length();
                
                plonk_assert (inputBuffer.length() == outputBufferLength);
                Buffer::copyData (outputSamples, inputSamples, outputBufferLength);
            }
        }
    }
    
private:
    QueueBufferType currentBuffer;
};


//------------------------------------------------------------------------------

///** Queue unit.
// Plays a queue of units in sequence.
// 
// @par Factory functions:
// - ar (queue, preferredNumChannels, preferredBlockSize=default, preferredSampleRate=default)
// - kr (queue, preferredNumChannels)
// 
// @par Inputs:
// - queue: (unit queue) the queue to use
// - preferredNumChannels: (int) force this unit to have a certain number of channels
// - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
// - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)
//
// @ingroup ControlUnits  */
template<class SampleType>
class BufferQueueRecordUnit
{
public:    
    typedef BufferQueueRecordChannelInternal<SampleType>    BufferQueueRecordInternal;
    typedef typename BufferQueueRecordInternal::Data        Data;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef ChannelInternalBase<SampleType>                 InternaBase;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
    typedef NumericalArray2D<SampleType>                    BufferArray;
    typedef QueueBufferBase<SampleType>                     QueueBufferType;
    typedef LockFreeQueue<QueueBufferType>                  BufferQueueType;

    
//    static inline UnitInfos getInfo() throw()
//    {
//        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();
//
//        return UnitInfo ("Queue", "Plays a queue of units in sequence.",
//                         
//                         // output
//                         1, 
//                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None, 
//                         IOKey::End,
//                         
//                         // inputs
//                         IOKey::Buffer,         Measure::None,
//                         IOKey::OverlapMake,    Measure::Factor,    0.5,        IOLimit::Clipped, Measure::NormalisedUnipolar,  0.0, 1.0,
//                         IOKey::BlockSize,      Measure::Samples,   blockSize,  IOLimit::Minimum, Measure::Samples,             1.0,
//                         IOKey::SampleRate,     Measure::Hertz,     sampleRate, IOLimit::Minimum, Measure::Hertz,               0.0,
//                         IOKey::End);
//    }    
    
    /** Create an audio rate block generator. */
    static inline UnitType ar (UnitType const& input,
                               BufferQueueType const& queue,
                               BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                               SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::BufferQueue, queue);
        
        Data data = { { -1.0, -1.0 } };
        
        return UnitType::template proxiesFromInputs<BufferQueueRecordInternal> (inputs,
                                                                                data,
                                                                                preferredBlockSize,
                                                                                preferredSampleRate);
    }
    
    static inline UnitType kr (UnitType const& input,
                               BufferQueueType const& queue) throw()
    {
        return ar (input, queue,
                   BlockSize::getControlRateBlockSize(),
                   SampleRate::getControlRate());
    }
      
};

typedef BufferQueueRecordUnit<PLONK_TYPE_DEFAULT> BufferQueueRecord;




#endif // PLONK_BUFFERQUEUECHANNEL_H


