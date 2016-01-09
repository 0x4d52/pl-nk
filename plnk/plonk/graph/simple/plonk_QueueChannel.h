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

#ifndef PLONK_QUEUECHANNEL_H
#define PLONK_QUEUECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


template<class SampleType> class QueueChannelInternal;

PLONK_CHANNELDATA_DECLARE(QueueChannelInternal,SampleType)
{
    ChannelInternalCore::Data base;
    int preferredNumChannels;
};


/** Play a queue of units. */
template<class SampleType>
class QueueChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(QueueChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(QueueChannelInternal,SampleType)             Data;
    typedef ChannelBase<SampleType>                                             ChannelType;
    typedef ObjectArray<ChannelType>                                            ChannelArrayType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                          Internal;
    typedef UnitBase<SampleType>                                                UnitType;
    typedef InputDictionary                                                     Inputs;
    typedef NumericalArray<SampleType>                                          Buffer;
    typedef LockFreeQueue<UnitType>                                             QueueType;
    
    QueueChannelInternal (Inputs const& inputs,
                          Data const& data,
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate,
                          ChannelArrayType& channels) throw()
    :   Internal (data.preferredNumChannels > 0 ? data.preferredNumChannels : inputs.getMaxNumChannels(),
                  inputs, data, blockSize, sampleRate, channels)
    {
        QueueType& queue = this->getInputAsUnitQueue (IOKey::UnitQueue);
        currentUnit = queue.pop();
    }
    
    Text getName() const throw()
    {
        return "Queue";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::UnitQueue);
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
    
        this->initProxyValue (channel, currentUnit.getValue (channel));
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        const Data& data = this->getState();
                
        const int numChannels = this->getNumChannels();
        int i, channel;
        
        for (channel = 0; channel < numChannels; ++channel)
        {
            const Buffer& inputBuffer (currentUnit.process (info, channel));
            const SampleType* const inputSamples = inputBuffer.getArray();
            const int inputBufferLength = inputBuffer.length();
            
            Buffer& outputBuffer = this->getOutputBuffer (channel);
            SampleType* const outputSamples = outputBuffer.getArray();
            const int outputBufferLength = outputBuffer.length();
            
            if (inputBufferLength == outputBufferLength)
            {
                Buffer::copyData (outputSamples, inputSamples, outputBufferLength);
            }
            else if (inputBufferLength == 1)
            {
                NumericalArrayFiller<SampleType>::fill (outputSamples, inputSamples[0], outputBufferLength);
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
            
            if (info.getShouldDelete())
            {
                QueueType& queue = this->getInputAsUnitQueue (IOKey::UnitQueue);
                currentUnit = queue.pop();
                info.resetShouldDelete();
            }
        }
    }
    
private:
    UnitType currentUnit;
};


//------------------------------------------------------------------------------

/** Queue unit.
 Plays a queue of units in sequence.
 
 @par Factory functions:
 - ar (queue, preferredNumChannels, preferredBlockSize=default, preferredSampleRate=default)
 - kr (queue, preferredNumChannels)
 
 @par Inputs:
 - queue: (unit queue) the queue to use
 - preferredNumChannels: (int) force this unit to have a certain number of channels
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @ingroup ControlUnits  */
template<class SampleType>
class QueueUnit
{
public:    
    typedef QueueChannelInternal<SampleType>        QueueInternal;
    typedef typename QueueInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternaBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    typedef NumericalArray2D<SampleType>            BufferArray;
    typedef LockFreeQueue<UnitType>                 QueueType;

    
//    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
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
    static PLONK_INLINE_LOW UnitType ar (QueueType const& queue,
                               const int preferredNumChannels = 0,
                               BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                               SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::UnitQueue, queue);
        
        Data data = { { -1.0, -1.0 }, preferredNumChannels };
        
        return UnitType::template proxiesFromInputs<QueueInternal> (inputs,
                                                                    data,
                                                                    preferredBlockSize,
                                                                    preferredSampleRate);
    }
    
    static PLONK_INLINE_LOW UnitType kr (QueueType const& queue,                                
                               const int preferredNumChannels = 0) throw()
    {
        return ar (queue, 
                   preferredNumChannels,
                   BlockSize::getControlRateBlockSize(),
                   SampleRate::getControlRate());
    }
      
};

typedef QueueUnit<PLONK_TYPE_DEFAULT> Queue;




#endif // PLONK_BLOCKCHANNEL_H


