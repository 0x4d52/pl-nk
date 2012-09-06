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


template<class SampleType>
class AudioHostBase;

/** An abstract class to interface with audio devices.
 @see PortAudioAudioHost, IOSAudioHost, JuceAudioHost */
template<class SampleType>
class AudioHostBase // don't inherit from PlonkBase...!
{
public:    
    typedef NumericalArray<SampleType*>            BufferArray;
    typedef NumericalArray<const SampleType*>      ConstBufferArray;

    typedef UnitBase<SampleType>                UnitType;
    typedef BusBuffer<SampleType>               BusType;
    typedef PLONK_BUSARRAYBASETYPE<BusType>     BussesType;
    typedef Dictionary<Dynamic>                 OptionDictionary;
    typedef NumericalArray<SampleType>          BufferType;
    
    /** Constructor */
    AudioHostBase (ObjectMemoryBase* omb = ObjectMemory<ObjectMemoryDefault>::create()) throw()
    :   om (omb),
        preferredHostSampleRate (0.0),
        preferredHostBlockSize (0),
        preferredGraphBlockSize (0),
        isRunning (false)
    { 
        om->init();
    }
        
    /** Destructor */
    virtual ~AudioHostBase() 
    {
    }
        
    /** Determine whether the audio device is runnging. */
    inline bool getIsRunning() const throw() { return isRunning; }
    
    /** Get a reference to the output unit. */
    inline const UnitType& getOutputUnit() const throw() { return outputUnit; }
    
    /** Get the number of audio inputs. */
    inline int getNumInputs() const throw()  { return this->inputs.length(); }
    
    /** Get the number of audio outputs. */
    inline int getNumOutputs() const throw() { return this->outputs.length(); }
    
    /** Get the current preferred sample rate for the host. */
    inline double getPreferredHostSampleRate() const throw() { return preferredHostSampleRate; }
    
    /** Get the current preferred block size for the host. */
    inline int getPreferredHostBlockSize() const throw() { return preferredHostBlockSize; }
    
    /** Set the host's preferred sample rate.
     This must be called before startHost() to have any effect. */
    inline void setPreferredHostSampleRate (const double newRate) throw() { preferredHostSampleRate = newRate; }
    
    /** Set the host's preferred block size.
     This must be called before startHost() to have any effect. */
    inline void setPreferredHostBlockSize (const int newSize) throw() {  preferredHostBlockSize = newSize; }
    
    /** Get the current preferred block size for the graph. */
    inline int getPreferredGraphBlockSize() const throw() { return preferredGraphBlockSize; }
        
    /** Set the graph's preferred block size.
     This must be called before startHost() to have any effect. */
    inline void setPreferredGraphBlockSize (const int newSize) throw() {  preferredGraphBlockSize = newSize; }
    
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
    inline const ConstBufferArray& getInputs() const throw()  { return this->inputs; }
    
    /** Get the input buffers. @internal */
    inline ConstBufferArray& getInputs() throw()              { return this->inputs; }

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
        const int numInputs = this->inputs.length();
        const int numOutputs = this->outputs.length();
        plonk_assert (this->busses.length() == numInputs);

        int blockRemain = preferredHostBlockSize;
        int i;
        
        const int graphBlockSize = BlockSize::getDefault().getValue();
        
        // must do more checks in case the block sizes are not compatible on this run
        
        while (blockRemain > 0)
        {
            for (i = 0; i < numInputs; ++i)
            {
                this->busses.atUnchecked (i).write (this->info.getTimeStamp(), 
                                                    graphBlockSize, 
                                                    this->inputs.atUnchecked (i)); 
                this->inputs.atUnchecked (i) += graphBlockSize;
            }
            
            this->outputUnit.process (info);
            
            if (this->outputUnit.isNotNull())
            {
                for (i = 0; i < numOutputs; ++i)
                {
                    const SampleType* const unitOutput = this->outputUnit.getOutputSamples (i);
                    BufferType::copyData (this->outputs.atUnchecked (i), unitOutput, graphBlockSize);   
                    this->outputs.atUnchecked (i) += graphBlockSize;
                }
            }
            else if (numOutputs > 0)
            {
                for (i = 0; i < numOutputs; ++i)
                {
                    BufferType::zeroData (this->outputs.atUnchecked (i), graphBlockSize);   
                    this->outputs.atUnchecked (i) += graphBlockSize;
                }
            }
                        
            this->info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * graphBlockSize);
            
            blockRemain -= graphBlockSize;
        }
        
#if PLONK_DEBUG
        // null the pointers to cause crash if buffers are not updated each HW block
        this->inputs.zero();
        this->outputs.zero();
#endif
    }
    
    /** @internal */
    void startHostInternal() throw()
    {
        initFormat();
        outputUnit = constructGraph();
        hostStarting();
        setIsRunning (true);
    }
    
    /** Set a flag to indicate the audio host is running.
     NB This does not start or stop the host use startHost() and stopHost() 
     respectively. */
    inline void setIsRunning (const bool state) throw() { isRunning = state; }

    
private:
    ScopedPointerContainer<ObjectMemoryBase> om;

    double preferredHostSampleRate;
    int preferredHostBlockSize;
    int preferredGraphBlockSize;
	bool isRunning;    
    OptionDictionary otherOptions;

    ProcessInfo info;
    UnitType outputUnit;  
    BussesType busses;
    ConstBufferArray inputs;
    BufferArray outputs;    
    
    inline void initFormat() throw()
    {
        SampleRate::getDefault().setValue (preferredHostSampleRate);
        
        // preferredGraphBlockSize must be less that the hardware size 
        // and divide into the hardware size without a remainder.
        if ((preferredGraphBlockSize == 0) ||
            (preferredHostBlockSize <= preferredGraphBlockSize) ||
            ((preferredHostBlockSize % preferredGraphBlockSize) != 0))
        {
            preferredGraphBlockSize = preferredHostBlockSize;
        }
        
        BlockSize::getDefault().setValue (preferredGraphBlockSize); 
    }

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
            //inputs = BufferArray::withSize (numInputs);
            inputs.setSize (numInputs, false);
            
            for (int i = 0; i < numInputs; ++i)
            {
                this->inputs.atUnchecked (i) = 0;//.referTo (0, 0);
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
            //outputs = BufferArray::withSize (numOutputs);
            outputs.setSize (numOutputs, false);
            
            for (int i = 0; i < numOutputs; ++i)
                this->outputs.atUnchecked (i) = 0;//.referTo (0, 0);
        }
        else this->outputs.clear();
    }
}


#endif // PLONK_AUDIOHOSTBASE_H
