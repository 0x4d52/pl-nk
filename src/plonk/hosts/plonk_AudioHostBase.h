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

#ifndef PLONK_AUDIOHOSTBASE_H
#define PLONK_AUDIOHOSTBASE_H

/** An abstract class to interface with audio devices.
 @see PortAudioAudioHost, IOSAudioHost, JuceAudioHost */
template<class SampleType>
class AudioHostBase
{
public:
    typedef NumericalArray<SampleType>          Buffer;
    typedef NumericalArray2D<SampleType>        BufferArray;
//    typedef NumericalArray<const SampleType>    ConstBuffer;
//    typedef NumericalArray2D<const SampleType>  ConstBufferArray;
    typedef UnitBase<SampleType>                UnitType;
    typedef BusBuffer<SampleType>               BusType;
    typedef PLONK_BUSARRAYBASETYPE<BusType>     BussesType;
    typedef Dictionary<Dynamic>                 OptionDictionary;
    
    /** Constructor */
    AudioHostBase() throw()
    :   om (ObjectMemory::global()),
        preferredSampleRate (0.0),
        preferredBlockSize (0),
        isRunning (false)
    { 
        om.increaseQueueCache (256);
    }
    
    /** Destructor */
    virtual ~AudioHostBase() { }
        
    /** Determine whether the audio device is runnging. */
    inline bool getIsRunning() const throw() { return isRunning; }
    
    /** Get a reference to the output unit. */
    inline const UnitType& getOutputUnit() const throw() { return outputUnit; }
    
    /** Get the number of audio inputs. */
    inline int getNumInputs() const throw()  { return this->inputs.length(); }
    
    /** Get the number of audio outputs. */
    inline int getNumOutputs() const throw() { return this->outputs.length(); }
    
    /** Get the current preferred sample rate for the host. */
    inline double getPreferredSampleRate() const throw() { return preferredSampleRate; }
    
    /** Get the current preferred block size for the host. */
    inline int getPreferredBlockSize() const throw() { return preferredBlockSize; }
    
    /** Set the host's preferred sample rate.
     This must be called before startHost() to have any effect. */
    inline void setPreferredSampleRate (const double newRate) throw() { preferredSampleRate = newRate; }
    
    /** Set the host's preferred block size.
     This must be called before startHost() to have any effect. */
    inline void setPreferredBlockSize (const int newSize) throw() {  preferredBlockSize = newSize; }
    
    /** Set the number of audio inputs required.
     This must be called before startHost() to have any effect. */
    void setNumInputs (const int numInputs) throw();
    
    /** Set the number of audio inputs required.
     This must be called before startHost() to have any effect. */
    void setNumOutputs (const int numOutputs) throw();
    
    /** Get other (normally platform dependent) options. */
    OptionDictionary getOtherOptions() const throw() { return otherOptions; }
                
protected:
    /** Get the input buffers. @internal */
    inline const BufferArray& getInputs() const throw()  { return this->inputs; }
    
    /** Get the input buffers. @internal */
    inline BufferArray& getInputs() throw()              { return this->inputs; }

    /** Get the output buffers. @internal */
    inline const BufferArray& getOutputs() const throw() { return this->outputs; }
    
    /** Get the output buffers. @internal */
    inline BufferArray& getOutputs() throw()             { return this->outputs; }

    /** Get the name of the audio host. */
    virtual Text getHostName() const = 0;
    
    /** Get the name of the native host audio API. */
    virtual Text getNativeHostName() const = 0;
    
    /** Get the name of the audio input device. */
    virtual Text getInputName() const = 0;
    
    /** Get the name of the audio output device. */
    virtual Text getOutputName() const = 0;
    
    /** Get the current CPU usage for the DSP loop. */
    virtual double getCpuUsage() const = 0;

    /** Start the host. */
    virtual void startHost() = 0;
    
    /** Stop the host. */
    virtual void stopHost() = 0;
    
    virtual void hostStopped() throw()  { }
    virtual void hostStarting() throw() { }

    /** This must be implemented by your application.
     It should return the audio graph that is rendered to the host. */
    virtual UnitType constructGraph() = 0;

    /** @internal */
    inline void process() throw()
    {
#ifdef PLONK_DEBUG
        Threading::ID currentThreadID = Threading::getCurrentThreadID();
        if (currentThreadID != Threading::getAudioThreadID())
            Threading::setAudioThreadID (Threading::getCurrentThreadID());
#endif
        
        const int blockSize = BlockSize::getDefault().getValue();
        const int numInputs = this->inputs.length();
        const int numOutputs = this->outputs.length();
        
        plonk_assert (this->busses.length() == numInputs);
        
        int i;
        
        for (i = 0; i < numInputs; ++i)
            this->busses.atUnchecked (i).write (this->info.getTimeStamp(), 
                                                blockSize, 
                                                this->inputs.atUnchecked (i).getArray());
        
        //this->lock.lock();   // perhaps replace with virtual function call?
        this->outputUnit.process (info);
        //this->lock.unlock(); // perhaps replace with virtual function call?
        
        if (this->outputUnit.isNotNull())
        {
            for (i = 0; i < numOutputs; ++i)
            {
                float* const output = this->outputs.atUnchecked (i).getArray();
                const float* const unitOutput = this->outputUnit.getOutputSamples (i);
                Floats::copyData (output, unitOutput, blockSize);            
            }
        }
        else if (numOutputs > 0)
        {
            for (i = 0; i < numOutputs; ++i)
            {
                float* const output = this->outputs.atUnchecked (i).getArray();
                Floats::zeroData (output, blockSize);         
            }
        }
        
        this->info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * blockSize);
    }
    
    /** @internal */
    void startHostInternal() throw()
    {
        outputUnit = constructGraph();
        hostStarting();
        setIsRunning (true);
    }
    
    /** Set a flag to indicate the audio host is running.
     NB This does not start or stop the host use startHost() and stopHost() 
     respectively. */
    inline void setIsRunning (const bool state) throw() { isRunning = state; }

    
private:
    ObjectMemory& om;

    double preferredSampleRate;
    int preferredBlockSize;
	bool isRunning;    
    OptionDictionary otherOptions;

    ProcessInfo info;
    UnitType outputUnit;  
    BussesType busses;
    BufferArray inputs;
    BufferArray outputs;
    //Lock lock; // <-- can get rid of soon!
    
};

//------------------------------------------------------------------------------

template<class SampleType>
void AudioHostBase<SampleType>::setNumInputs (const int numInputs) throw()
{
    plonk_assert (numInputs >= 0);
    
    if (numInputs != this->getNumInputs())
    {
        this->busses.clear();
        
        if (numInputs != 0)
        {
            inputs = BufferArray (numInputs);
            
            for (int i = 0; i < numInputs; ++i)
            {
                this->inputs.atUnchecked (i).referTo (0, 0);
                this->busses.add (BusType (i));
            }
        }
        else this->inputs.clear();
    }
}

template<class SampleType>
void AudioHostBase<SampleType>::setNumOutputs (const int numOutputs) throw()
{
    plonk_assert (numOutputs >= 0);
    
    if (numOutputs != this->getNumOutputs())
    {            
        if (numOutputs != 0)
        {
            outputs = BufferArray (numOutputs);
            
            for (int i = 0; i < numOutputs; ++i)
                this->outputs.atUnchecked (i).referTo (0, 0);
        }
        else this->outputs.clear();
    }
}


#endif // PLONK_AUDIOHOSTBASE_H
