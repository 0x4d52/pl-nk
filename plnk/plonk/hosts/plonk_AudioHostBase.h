/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

#ifndef PLONK_AUDIOHOSTBASE_H
#define PLONK_AUDIOHOSTBASE_H


template<class SampleType>
class AudioHostBase;

/** An abstract class to interface with audio devices.
 @see PortAudioAudioHost, IOSAudioHost, JuceAudioHost */
template<class SampleType>
class AudioHostBase : public PlonkBase
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
    AudioHostBase() throw()
    :   preferredHostSampleRate (0.0),
        preferredHostBlockSize (0),
        preferredGraphBlockSize (0),
        isRunning (false),
        isPaused (false)
    { 
    }
        
    /** Destructor */
    virtual ~AudioHostBase() 
    {
    }
        
    /** Determine whether the audio device is running. */
    PLONK_INLINE_LOW bool getIsRunning() const throw() { return isRunning.getValue(); }
    
    /** Determine whether the audio device is paused. */
    PLONK_INLINE_LOW bool getIsPaused() const throw() { return isPaused.getValue(); }

    /** Get a reference to the output unit. */
    PLONK_INLINE_LOW const UnitType& getOutputUnit() const throw() { return outputUnit; }
    
    /** Get the number of audio inputs. */
    PLONK_INLINE_LOW int getNumInputs() const throw()  { return this->inputs.length(); }
    
    /** Get the number of audio outputs. */
    PLONK_INLINE_LOW int getNumOutputs() const throw() { return this->outputs.length(); }
    
    /** Get the current preferred sample rate for the host. */
    PLONK_INLINE_LOW double getPreferredHostSampleRate() const throw() { return preferredHostSampleRate; }
    
    /** Get the current preferred block size for the host. */
    PLONK_INLINE_LOW int getPreferredHostBlockSize() const throw() { return preferredHostBlockSize; }
    
    /** Set the host's preferred sample rate.
     This must be called before startHost() to have any effect. */
    PLONK_INLINE_LOW void setPreferredHostSampleRate (const double newRate) throw() { preferredHostSampleRate = newRate; }
    
    /** Set the host's preferred block size.
     This must be called before startHost() to have any effect. */
    PLONK_INLINE_LOW void setPreferredHostBlockSize (const int newSize) throw() {  preferredHostBlockSize = newSize; }
    
    /** Get the current preferred block size for the graph. */
    PLONK_INLINE_LOW int getPreferredGraphBlockSize() const throw() { return preferredGraphBlockSize; }
        
    /** Set the graph's preferred block size.
     This must be called before startHost() to have any effect. */
    PLONK_INLINE_LOW void setPreferredGraphBlockSize (const int newSize) throw() {  preferredGraphBlockSize = newSize; }
    
    /** Set the number of audio inputs required.
     This must be called before startHost() to have any effect. */
    void setNumInputs (const int numInputs) throw();
    
    /** Set the number of audio inputs required.
     This must be called before startHost() to have any effect. */
    void setNumOutputs (const int numOutputs) throw();
    
    /** Get other (normally platform dependent) options. */
    OptionDictionary getOtherOptions() const throw() { return otherOptions; }
    
    virtual void pauseHost() throw() { plonk_assertfalse; }     // the host implementation needs to support pausing
    virtual void resumeHost() throw() { plonk_assertfalse; }    // the host implementation needs to support pausing
    
    void process (UnitType& unit) throw() { unit.process (this->info); }

protected:
    /** Get the input buffers. @internal */
    PLONK_INLINE_LOW const ConstBufferArray& getInputs() const throw()    { return this->inputs; }
    
    /** Get the input buffers. @internal */
    PLONK_INLINE_LOW ConstBufferArray& getInputs() throw()                { return this->inputs; }

    /** Get the output buffers. @internal */
    PLONK_INLINE_LOW const BufferArray& getOutputs() const throw()        { return this->outputs; }
    
    /** Get the output buffers. @internal */
    PLONK_INLINE_LOW BufferArray& getOutputs() throw()                    { return this->outputs; }

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
    virtual void hostPaused() throw()   { }
    virtual void hostResuming() throw() { }

    /** This must be implemented by your application.
     It should return the audio graph that is rendered to the host. */
    virtual UnitType constructGraph() = 0;

    /** @internal */
    PLONK_INLINE_LOW void process() throw()
    {
        int i;

#ifdef PLONK_DEBUG
        Threading::ID currentThreadID = Threading::getCurrentThreadID();
        if (currentThreadID != Threading::getAudioThreadID())
            Threading::setAudioThreadID (Threading::getCurrentThreadID());
#endif
        const int numInputs = this->inputs.length();
        const int numOutputs = this->outputs.length();
        plonk_assert (this->busses.length() == numInputs);

        int blockRemain = preferredHostBlockSize;        
        const int graphBlockSize = BlockSize::getDefault().getValue();
        
        // must do more checks in case the block sizes are not compatible on this run
        

        // push all the input samples for this hardware frame onto the busses
        for (i = 0; i < numInputs; ++i)
        {
            BusType& bus = this->busses.atUnchecked (i);
            bus.getWriteBlockSize().setValue (blockRemain);
            bus.write (this->info.getTimeStamp(), blockRemain, this->inputs.atUnchecked (i));
        }

        // write the hardware frame in possible smaller blocks
        if (this->outputUnit.isNotNull())
        {
            while (blockRemain > 0)
            {            
                this->outputUnit.process (this->info);
                
                for (i = 0; i < numOutputs; ++i)
                {
                    const SampleType* const unitOutput = this->outputUnit.getOutputSamples (i);
                    BufferType::copyData (this->outputs.atUnchecked (i), unitOutput, graphBlockSize);   
                    this->outputs.atUnchecked (i) += graphBlockSize;
                }
                
                this->info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * graphBlockSize);
                
                blockRemain -= graphBlockSize;
            }
        }
        else if (numOutputs > 0)
        {
            for (i = 0; i < numOutputs; ++i)
                BufferType::zeroData (this->outputs.atUnchecked (i), blockRemain);
            
            this->info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * blockRemain);
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
        Unit graphUnit = constructGraph();
        outputUnit = graphUnit.getBlockSize (0).getValue() == 1 ? graphUnit.ar() : graphUnit;
        hostStarting();
        
        setIsRunning (true);
    }
    
protected:
    /** Set a flag to indicate the audio host is running.
     NB This does not start or stop the host use startHost() and stopHost() 
     respectively. */
    PLONK_INLINE_LOW void setIsRunning (const bool state) throw() { isRunning = state; }

    PLONK_INLINE_LOW void setIsPaused (const bool state) throw() { isPaused = state; }
    
private:
    double preferredHostSampleRate;
    int preferredHostBlockSize;
    int preferredGraphBlockSize;
	AtomicInt isRunning;
    AtomicInt isPaused;
    OptionDictionary otherOptions;

    ProcessInfo info;
    UnitType outputUnit;
    BussesType busses;
    ConstBufferArray inputs;
    BufferArray outputs;    
    
    PLONK_INLINE_LOW void initFormat() throw()
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
            inputs.setSize (numInputs, false);
            
            for (int i = 0; i < numInputs; ++i)
            {
                this->inputs.atUnchecked (i) = 0;
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
            outputs.setSize (numOutputs, false);
            
            for (int i = 0; i < numOutputs; ++i)
                this->outputs.atUnchecked (i) = 0;
        }
        else this->outputs.clear();
    }
}


#endif // PLONK_AUDIOHOSTBASE_H
