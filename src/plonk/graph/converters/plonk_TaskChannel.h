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

#ifndef PLONK_THREADEDCHANNEL_H
#define PLONK_THREADEDCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType, Interp::TypeCode InterpTypeCode> class InputTaskChannelInternal;

template<class SampleType,Interp::TypeCode InterpTypeCode>
struct ChannelData< InputTaskChannelInternal<SampleType,InterpTypeCode> >
{
    ChannelInternalCore::Data base;
    int numChannels;
    int numBuffers;
    int priority;
    bool resampleInput;
};      

//------------------------------------------------------------------------------

class TaskMessageInternal : public SmartPointer
{
public:
    TaskMessageInternal() throw()
    {
    }
    
    TaskMessageInternal (Text const& t)
    :   message (t)
    {
    }
    
    TaskMessageInternal (Text const& t, Dynamic const& d)
    :   message (t), payload (d)
    {
    }
    
    friend class TaskMessage;
    
private:
    const Text message;
    const Dynamic payload;
};

class TaskMessage : public SmartPointerContainer<TaskMessageInternal>
{
public:
    TaskMessage() throw()
    :   SmartPointerContainer<TaskMessageInternal> (static_cast<TaskMessageInternal*> (0))
    {
    }
    
    TaskMessage (Text const& t)
    :   SmartPointerContainer<TaskMessageInternal> (new TaskMessageInternal (t, Dynamic::getNull()))
    {
    }
    
    TaskMessage (Text const& t, Dynamic const& d)
    :   SmartPointerContainer<TaskMessageInternal> (new TaskMessageInternal (t, d))
    {
    }
    
    inline const Text& getMessage() const throw()    { return this->getInternal()->message; }
    inline const Dynamic& getPayload() const throw() { return this->getInternal()->payload; }
};

template<class SampleType>
class TaskBufferInternal : public SmartPointer
{
public:
    typedef NumericalArray<SampleType> Buffer;
    typedef LockFreeQueue<TaskMessage> TaskMessages;
    
    TaskBufferInternal (const int size) throw()
    :   buffer (Buffer::newClear (size))
    {
    }
    
    Buffer buffer;
    TaskMessages messages;
};

template<class SampleType>
class TaskBufferBase : public SmartPointerContainer< TaskBufferInternal<SampleType> >
{
public:
    TaskBufferBase() throw()
    :   SmartPointerContainer< TaskBufferInternal<SampleType> > (static_cast< TaskBufferInternal<SampleType>* > (0))
    {
    }
    
    TaskBufferBase (const int size) throw()
    :   SmartPointerContainer< TaskBufferInternal<SampleType> > (new TaskBufferInternal<SampleType> (size))
    {
    }
    
    static const TaskBufferBase& getNull() throw()
    {
        static TaskBufferBase null;
        return null;
    }
};


/** Defer a unit's processing to a separate task, thread, process or core. */
template<class SampleType, Interp::TypeCode InterpTypeCode>
class InputTaskChannelInternal
:  public ProxyOwnerChannelInternal<SampleType, ChannelData< InputTaskChannelInternal<SampleType,InterpTypeCode> > >
{
public:   
    typedef InputTaskChannelInternal<SampleType,InterpTypeCode>         TaskInternal;
    typedef ChannelData<TaskInternal>                                   Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef ChannelInternalBase<SampleType>                             ChannelInternalType;
    typedef typename ChannelType::Weak                                  WeakChannelType;
    typedef ObjectArray<ChannelType>                                    ChannelArrayType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                  Internal;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
    typedef TaskBufferBase<SampleType>                                  TaskBuffer;
    typedef ResampleUnit<SampleType,InterpTypeCode>                     ResampleType;
    
    //--------------------------------------------------------------------------
    
    class InputTask :  public Threading::Thread, public Channel::Receiver
    {
    public:
        typedef LockFreeQueue<TaskBuffer> TaskBufferQueue;
        
        InputTask (InputTaskChannelInternal* o) throw()
        :   Threading::Thread (Text ("InputTaskChannelInternal::Thread[" + Text (*(LongLong*)&o) + Text ("]"))),
            weakOwner (ChannelType (static_cast<ChannelInternalType*> (o))),
            event (Lock::MutexLock),
            inputEnded (0)
        {
        }
        
        void changed (ChannelType const& source, Text const& message, Dynamic const& payload) throw()
        {
            TaskMessage tm (message, payload);
            currentTaskBuffer.getInternal()->messages.push (tm);
        }
        
        void fillBuffers() throw()
        {
            ChannelType ownerChannel (weakOwner.fromWeak());
            
            plonk_assert (ownerChannel.isNotNull());
            
            InputTaskChannelInternal* owner = static_cast<InputTaskChannelInternal*> (ownerChannel.getInternal());
            
            Data& data = owner->getState();
            setPriority (data.priority);
            
            const int numBuffers = owner->getState().numBuffers;
            
            plonk_assert (numBuffers > 0);
            plonk_assert (owner->getBlockSize().getValue() > 0);
            
            const int bufferSize = owner->getNumChannels() * owner->getBlockSize().getValue();
            
            for (int i = 0; i < numBuffers; ++i)
                activeBuffers.push (TaskBuffer (bufferSize));
        }
        
        ResultCode run() throw()
        {
            fillBuffers();
            
            Threading::yield();
                
            while (!getShouldExit())
            {
                ChannelType ownerChannel (weakOwner.fromWeak());
                
                if (ownerChannel.isNull())
                    break;
                
                InputTaskChannelInternal* owner = static_cast<InputTaskChannelInternal*> (ownerChannel.getInternal());
                ProcessInfo& info (owner->getProcessInfo());

                UnitType& inputUnit (owner->getInputAsUnit (IOKey::Generic));
                
                if (inputUnit.shouldBeDeletedNow (info))
                {
                    inputEnded.setValue (1);
                    Threading::sleep (0.01);
                    continue;
                }
                
                const int numChannels = owner->getNumChannels();
                const int blockSize = owner->getBlockSize().getValue();
                const int numFreeBuffers = freeBuffers.length();
                
                plonk_assert (inputUnit.channelsHaveSameBlockSize());

                if (numFreeBuffers > 0)
                {                                        
                    currentTaskBuffer = freeBuffers.pop(); // nothing else pops so must be still available
                    Buffer& buffer = currentTaskBuffer.getInternal()->buffer;
                    buffer.setSize (blockSize * numChannels, false);
                    
                    SampleType* bufferSamples = buffer.getArray();

                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        const Buffer& inputBuffer (inputUnit.process (info, channel));                    
                        const SampleType* inputSamples = inputBuffer.getArray();
                        const int inputBufferLength = inputBuffer.length();
                                                
                        if (buffer.length() == (numChannels * inputBufferLength))
                        {
                            NumericalArray<SampleType>::copyData (bufferSamples, inputSamples, inputBufferLength);
                            bufferSamples += inputBufferLength;
                        }
                        else
                        {
                            // probably got deleted..?
                            buffer.zero();
                            break;
                        }
                    }
                    
                    activeBuffers.push (currentTaskBuffer);
                    currentTaskBuffer = TaskBuffer::getNull();

                    plonk_assert (inputUnit.channelsHaveSameSampleRate());
                    info.offsetTimeStamp (owner->getSampleRate().getSampleDurationInTicks() * blockSize);
                }
                
                if (numFreeBuffers == 1)
                {
                    event.wait (0.5);
                }
                else if (numFreeBuffers < (activeBuffers.length() * 2))
                {
                    Threading::sleep (owner->getBlockDurationInTicks() * TimeStamp::getReciprocalTicks());
                }
                else
                {
                    Threading::yield();
                }
            }
            
            freeBuffers.clearAll();
            activeBuffers.clearAll();
            
            delete this;
            
            return PlankResult_ThreadWasDeleted;
        }
                
        void end() throw()
        {
            Lock e = this->event; // take a shared copy of the event
            setShouldExit();      // this might delete this object before the next line
            e.signal();           // or this will proceed past the wait, or be harmless if the thread died already
        }
    
        inline bool pop (TaskBuffer& buffer) throw()
        {
            return activeBuffers.pop (buffer);
        }
    
        inline void push (TaskBuffer const& buffer) throw()
        {
            buffer.getInternal()->messages.clear();
            freeBuffers.push (buffer);
            event.signal();
        }
        
        inline bool inputHasEnded() const throw()
        {
            return inputEnded.getValueUnchecked() != 0;
        }
    
    private:
        WeakChannelType weakOwner;
        TaskBufferQueue activeBuffers;
        TaskBufferQueue freeBuffers;
        TaskBuffer currentTaskBuffer;
        Lock event;
        AtomicInt inputEnded;
    };
    
    //--------------------------------------------------------------------------
    
    InputTaskChannelInternal (Inputs const& inputs,
                              Data const& data,
                              BlockSize const& blockSize,
                              SampleRate const& sampleRate,
                              ChannelArrayType& channels) throw()
    :   Internal (numChannelsInSource (inputs), 
                  inputs, data, blockSize, sampleRate,
                  channels),
        task (new InputTask (this))
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        inputUnit.addReceiverToChannels (task);
        
        if (data.resampleInput)
            inputUnit = ResampleType::ar (inputUnit, 1, blockSize, sampleRate);
        
        task->start();
    }
    
    ~InputTaskChannelInternal()
    {
        if (task)
            endTask();
    }
    
    void endTask() throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        inputUnit.removeReceiverFromChannels (task);
        task->end(); // will delete the task when its thread exits
        task = 0;
    }
            
    Text getName() const throw()
    {
        return "Task";
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
        
        TaskBuffer taskBuffer;
        
        if (!task)
        {
            zeroOutput (numChannels);
        
        }
        else if (task->inputHasEnded())
        {
            endTask();
            zeroOutput (numChannels);
        }
        else if (task->pop (taskBuffer))
        {
            // could be smarter in here in case the buffer size changes
            
            Buffer& buffer = taskBuffer.getInternal()->buffer;
            SampleType* bufferSamples = buffer.getArray();
            
            for (int channel = 0; channel < numChannels; ++channel)
            {
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                SampleType* const outputSamples = outputBuffer.getArray();
                const int outputBufferLength = outputBuffer.length();  
                
                plonk_assert (buffer.length() == (numChannels * outputBufferLength));
                
                Buffer::copyData (outputSamples, bufferSamples, outputBufferLength);
                bufferSamples += outputBufferLength;                
            }
            
            buffer.zero();
            
            TaskMessage taskMessage;
            while (taskBuffer.getInternal()->messages.pop (taskMessage))
                this->update (taskMessage.getMessage(), taskMessage.getPayload());
            
            task->push (taskBuffer);
        }
        else
        {            
            // buffer underrun or other error
            zeroOutput (numChannels);
        }        
    }
    
    inline void zeroOutput (const int numChannels) throw()
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            Buffer& outputBuffer = this->getOutputBuffer (channel);
            outputBuffer.zero();
        }
    }
    
    ProcessInfo& getProcessInfo() throw() { return info; }
    
private:
    InputTask* task;

    ProcessInfo info; // private info for this object as we're running out of sync with everything else
    
    static inline int numChannelsInSource (Inputs const& inputs) throw()
    {
        return inputs[IOKey::Generic].asUnchecked<UnitType>().getNumChannels();
    }
    
};

//------------------------------------------------------------------------------

/** Defer a unit's processing to a separate task, thread, process or core. 
  
 Can also be used to distribute proessing across threads, processors or cores.
 Very important to use this to wrap units that access files etc
 (e.g., FilePlayUnit).
 
 The latency of this will be equal to: 
 @f$ \frac{preferredBlockSize \times numBuffers}{preferredSampleRate} @f$
 
 @par @par Factory functions:
 - ar (input, numBuffers=16, priority=50, preferredBlockSize=noPref, preferredSampleRate=noPref)
 
 @par @par Inputs:
 - input: (input, multi) the input unit to defer to a separate task
 - numBuffers: (int) the number of buffers to queue, also affects latency
 - priority: (int) the priority of the task (0-100, 100 is highest)
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

  @ingroup ConverterUnits */
template<class SampleType, Interp::TypeCode InterpTypeCode>
class InputTaskUnit
{
public:    
    typedef InputTaskChannelInternal<SampleType,InterpTypeCode>     TaskInternal;
    typedef typename TaskInternal::Data                             Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef ResampleUnit<SampleType,InterpTypeCode>                 ResampleType;
    typedef InputTaskUnit<SampleType,Interp::Lagrange3>             HQ;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("InputTask", "Defer a unit's processing to a separate task, thread, process or core.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::None,
                         IOKey::BufferCount,        Measure::Count,     16.0,       IOLimit::Minimum,   Measure::Count,             1.0,
                         IOKey::Priority,           Measure::Percent,   50.0,       IOLimit::Clipped,   Measure::Percent,           0.0, 100.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    static UnitType ar (UnitType const& input,
                        const int numBuffers = 16,
                        const int priority = 50,
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {                     
        BlockSize blockSize = BlockSize::decide (input.getBlockSize (0), preferredBlockSize);
        SampleRate sampleRate = SampleRate::decide (input.getSampleRate (0), preferredSampleRate);
        
        // could avoid the resample if we added the function the check if all bs/sr are the same in each channel
        
        Inputs inputs;
//        inputs.put (IOKey::Generic, ResampleType::ar (input, 1, blockSize, sampleRate));
        inputs.put (IOKey::Generic, input);
        
        Data data = { { -1.0, -1.0 }, 0, numBuffers, priority, true };
        
        return UnitType::template proxiesFromInputs<TaskInternal> (inputs, 
                                                                   data, 
                                                                   blockSize, 
                                                                   sampleRate);
    }
    
    static UnitType arNoResample (UnitType const& input,
                                  const int numBuffers = 16,
                                  const int priority = 50,
                                  BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                                  SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        BlockSize blockSize = BlockSize::decide (input.getBlockSize (0), preferredBlockSize);
        SampleRate sampleRate = SampleRate::decide (input.getSampleRate (0), preferredSampleRate);
        
        // could avoid the resample if we added the function the check if all bs/sr are the same in each channel
        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        
        Data data = { { -1.0, -1.0 }, 0, numBuffers, priority, false };
        
        return UnitType::template proxiesFromInputs<TaskInternal> (inputs,
                                                                   data,
                                                                   blockSize,
                                                                   sampleRate);
    }

};

typedef InputTaskUnit<PLONK_TYPE_DEFAULT> InputTask;

#endif // PLONK_THREADEDCHANNEL_H

