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

#ifndef PLONK_THREADEDCHANNEL_H
#define PLONK_THREADEDCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class ThreadedChannelInternal;

PLONK_CHANNELDATA_DECLARE(ThreadedChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    int numChannels;
    int numBuffers;
};      

//------------------------------------------------------------------------------


/** File player generator. */
template<class SampleType>
class ThreadedChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(ThreadedChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(ThreadedChannelInternal,SampleType)  Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef ObjectArray<ChannelType>                                    ChannelArrayType;
    typedef ThreadedChannelInternal<SampleType>                         ThreadedInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                  Internal;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;

    //--------------------------------------------------------------------------
    
    class Thread : public Threading::Thread
    {
    public:
        typedef LockFreeQueue<Buffer> BufferQueue;
        
        Thread (ThreadedChannelInternal* o) throw()
        :   Threading::Thread (Text ("ThreadedChannelInternal::Thread[" + Text (*(LongLong*)&o) + Text ("]"))),
            owner (o),
            info (owner->getProcessInfo())
        {
        }
        
        BufferQueue& getActiveBufferQueue() throw() { return activeBuffers; }
        BufferQueue& getFreeBufferQueue() throw() { return freeBuffers; }
                
        ResultCode run() throw()
        {
            const int numChannels = owner->getNumChannels();
            const int numBuffers = owner->getState().numBuffers;;
            
            plonk_assert (numBuffers > 0);
            plonk_assert (owner->getBlockSize().getValue() > 0);
            
            const int bufferSize = numChannels * owner->getBlockSize().getValue();
            
            for (int i = 0; i < numBuffers; ++i)
                activeBuffers.push (Buffer::newClear (bufferSize));
            
            while (!getShouldExit())
            {                     
                UnitType& inputUnit (owner->getInputAsUnit (IOKey::Generic));

                if (freeBuffers.length() > 0)
                {
                    plonk_assert (inputUnit.channelsHaveSameBlockSize());
                    const int blockSize = inputUnit.getBlockSize (0).getValue();
                    
                    Buffer buffer = freeBuffers.pop();
                    buffer.setSize (blockSize * numChannels, false);
                    
                    SampleType* bufferSamples = buffer.getArray();

                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        const Buffer& inputBuffer (inputUnit.process (info, channel));                    
                        const SampleType* const inputSamples = inputBuffer.getArray();
                        const int inputBufferLength = inputBuffer.length();

                        plonk_assert (buffer.length() == (numChannels * inputBufferLength));
                        
                        for (int i = 0; i < inputBufferLength; ++i)
                            *bufferSamples++ = inputSamples[i];
                    }
                    activeBuffers.push (buffer);

                    plonk_assert (inputUnit.channelsHaveSameSampleRate());
                    info.offsetTimeStamp (inputUnit.getSampleRate (0).getSampleDurationInTicks() * blockSize);
                                        
                    Threading::yield();
                }
                
                Threading::sleep (inputUnit.getBlockSize (0).getValue() / inputUnit.getSampleRate (0).getValue() * 0.5);
            }
            
            freeBuffers.clearAll();
            activeBuffers.clearAll();
            
            return 0;
        }
        
    private:
        ThreadedChannelInternal* owner;
        ProcessInfo& info;
        BufferQueue activeBuffers;
        BufferQueue freeBuffers;
    };
    
    //--------------------------------------------------------------------------
    
    ThreadedChannelInternal (Inputs const& inputs, 
                             Data const& data, 
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate,
                             ChannelArrayType& channels) throw()
    :   Internal (numChannelsInSource (inputs), 
                  inputs, data, blockSize, sampleRate,
                  channels),
        thread (this)
    {        
        thread.start();
    }
    
    ~ThreadedChannelInternal()
    {
        thread.setShouldExitAndWait (0.000001); // will block though...
    }
            
    Text getName() const throw()
    {
        return "Threaded";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }    
        
    void initChannel (const int channel) throw()
    {       
        this->initProxyValue (channel, 0);
    }    
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {                
        const int numChannels = this->getNumChannels();
        
        Buffer buffer;

        if (thread.getActiveBufferQueue().pop (buffer)) 
        {
            // could be smarter in here in case the buffer size changes
            
            SampleType* bufferSamples = buffer.getArray();
            
            for (int channel = 0; channel < numChannels; ++channel)
            {
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                SampleType* const outputSamples = outputBuffer.getArray();
                const int outputBufferLength = outputBuffer.length();  
                
                plonk_assert (buffer.length() == (numChannels * outputBufferLength));
                
                for (int i = 0; i < outputBufferLength; ++i)
                    outputSamples[i] = *bufferSamples++;
            }
            
            buffer.zero();
            thread.getFreeBufferQueue().push (buffer);
        }
        else
        {
            // buffer underrun or other error
            for (int channel = 0; channel < numChannels; ++channel)
            {
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                outputBuffer.zero();
            }
        }
    }
    
    ProcessInfo& getProcessInfo() throw() { return info; }
    
private:
    Thread thread;
    ProcessInfo info; // private info for this object as we're running out of sync with everything else
    
    static inline int numChannelsInSource (Inputs const& inputs) throw()
    {
        return inputs[IOKey::Generic].asUnchecked<UnitType>().getNumChannels();
    }
    
};

//------------------------------------------------------------------------------

/** Defer a unit's processing to a separate. 
  
 Factory functions:
 - ar (input, preferredBlockSize=default, preferredSampleRate=default)
 
 Inputs:
 - input: (input, multi) the input unit to defer to a separate thread
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

  @ingroup GeneratorUnits */
template<class SampleType>
class ThreadedUnit
{
public:    
    typedef ThreadedChannelInternal<SampleType>         ThreadedInternal;
    typedef typename ThreadedInternal::Data             Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;
        
//    static inline UnitInfos getInfo() throw()
//    {
//        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();
//        
//        return UnitInfo ("FilePlay", "A signal player generator.",
//                         
//                         // output
//                         ChannelCount::VariableChannelCount, 
//                         IOKey::Generic,            Measure::None,      0.0,        IOLimit::None,
//                         IOKey::End,
//                         
//                         // inputs
//                         IOKey::AudioFileReader,    Measure::None,
//                         IOKey::Multiply,           Measure::Factor,    1.0,        IOLimit::None,
//                         IOKey::Add,                Measure::None,      0.0,        IOLimit::None,
//                         IOKey::BlockSize,          Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
//                         IOKey::SampleRate,         Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
//                         IOKey::End);
//    }
    
    static UnitType ar (UnitType const& input,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        Inputs inputs;
        inputs.put (IOKey::Generic, Resample::ar (input, preferredBlockSize, preferredSampleRate));
                        
        Data data = { { -1.0, -1.0 }, 0, 16 };
        
        return UnitType::template proxiesFromInputs<ThreadedInternal> (inputs, 
                                                                       data, 
                                                                       preferredBlockSize, 
                                                                       preferredSampleRate);
    }
};

typedef ThreadedUnit<PLONK_TYPE_DEFAULT> Threaded;


#endif // PLONK_THREADEDCHANNEL_H

