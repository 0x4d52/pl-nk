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

#ifndef PLONK_FFTBUFFERS_H
#define PLONK_FFTBUFFERS_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "../containers/plonk_ContainerForwardDeclarations.h"
#include "../containers/plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "../containers/plonk_ObjectArray.h"
#include "../containers/plonk_SimpleArray.h"

static RNG& getConvolveRNG()
{
    static RNG rng;
    return rng;
}

template<class SampleType> class FFTBuffersBase;

template<class SampleType>
class FFTBuffersInternal : public SmartPointer
{
public:
    typedef FFTBuffersBase<SampleType>      Container;
    typedef NumericalArray<SampleType>      BufferType;
    typedef NumericalArray2D<SampleType>    BuffersType;
    typedef FFTEngineBase<SampleType>       FFTEngineType;
    typedef SignalBase<SampleType>          SignalType;

    
    FFTBuffersInternal (FFTEngineType const& fftEngineToUse, BuffersType const& sourceBuffers) throw()
    :   fftEngine (fftEngineToUse),
        originalLength (0),
        numDivisions (0)
    {
        setBuffersInternal (sourceBuffers);
        initProcessBuffers();
        initCountDownStart();
    }
    
    FFTBuffersInternal (FFTEngineType const& fftEngineToUse, SignalType const& sourceSignal) throw()
    :   fftEngine (fftEngineToUse),
        originalLength (0),
        numDivisions (0)
    {
        setSignalInternal (sourceSignal);
        initProcessBuffers();
        initCountDownStart();
    }
    
    void initProcessBuffers() throw()
    {
        if (originalLength > 0)
            for (int i = 0; i < fftBuffers.length(); ++i)
                processBuffers.add (BufferType::newClear (numDivisions * (int) fftEngine.length()));
    }
    
    void initCountDownStart() throw()
    {
        for (int i = 0; i < fftBuffers.length(); ++i)
            countDownStart.add (getConvolveRNG().uniform ((int) (fftEngine.length() / 16)) * 4);
    }
    
    FFTBuffersInternal* getChannel (const int channel) const throw()
    {
        const int numChannels = fftBuffers.length();
        
        if (numChannels == 0)
            return new FFTBuffersInternal (fftEngine, Buffers());
        else
            return new FFTBuffersInternal (fftBuffers.atUnchecked (channel % numChannels),
                                           fftEngine,
                                           originalLength,
                                           numDivisions);
    }
    
    void setSignalInternal (SignalType const& sourceSignal) throw()
    {
        const int numChannels = sourceSignal.getNumChannels();
        
        if (numChannels < 1)
            return;
        
        const int fftSize = (int) fftEngine.length();
        const int fftSizeHalved = (int) fftEngine.halfLength();
        
        originalLength = sourceSignal.getNumFrames();
        numDivisions = originalLength / fftSizeHalved;
        
        if (originalLength % fftSizeHalved != 0)
            ++numDivisions;
        
        BufferType tempBuffer = Buffer::withSize (fftSize);
        SampleType* const tempSamples = tempBuffer.getArray();
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            BufferType fftBuffer = BufferType::withSize (numDivisions * fftSize);
            SampleType* fftSamples = fftBuffer.getArray();
            const SampleType* sourceBuffer = sourceSignal.getSamples (channel);
            const int frameStride = sourceSignal.getFrameStride();
            int sourceRemaining = originalLength;
            
            if (frameStride == 1)
            {
                for (int division = 0; division < numDivisions; ++division)
                {
                    const int dataLength = plonk::min (sourceRemaining, fftSizeHalved);
                    const int zeroLength = fftSize - dataLength;
                    
                    BufferType::copyData (tempSamples, sourceBuffer, dataLength);    // copy
                    BufferType::zeroData (tempSamples + dataLength, zeroLength);     // zero pad
                    
                    fftEngine.forward (fftSamples, tempSamples);
                    
                    fftSamples      += fftSize;
                    sourceBuffer    += fftSizeHalved;
                    sourceRemaining -= fftSizeHalved;
                }
            }
            else // signal is interleaved
            {
                for (int division = 0; division < numDivisions; ++division)
                {
                    const int dataLength = plonk::min (sourceRemaining, fftSizeHalved);
                    const int zeroLength = fftSize - dataLength;
                    
                    for (int sample = 0; sample < dataLength; ++sample)
                    {
                        tempSamples[sample] = *sourceBuffer;
                        sourceBuffer += frameStride;
                    }
                    
                    BufferType::zeroData (tempSamples + dataLength, zeroLength);
                    fftEngine.forward (fftSamples, tempSamples);
                    
                    fftSamples      += fftSize;
                    sourceRemaining -= fftSizeHalved;
                }
            }
            
            fftBuffers.add (fftBuffer);
        }
    }
    
    void setBuffersInternal (BuffersType const& sourceBuffers) throw()
    {
        const int numChannels = sourceBuffers.length();
        
        if (numChannels < 1)
            return;
        
        plonk_assert (numChannels == 1 || sourceBuffers.isMatrix());
        
        const int fftSize = (int) fftEngine.length();
        const int fftSizeHalved = (int) fftEngine.halfLength();
        
        originalLength = sourceBuffers.atUnchecked (0).length();
        numDivisions = originalLength / fftSizeHalved;
        
        if (originalLength % fftSizeHalved != 0)
            ++numDivisions;
        
        BufferType tempBuffer = Buffer::withSize (fftSize);
        SampleType* const tempSamples = tempBuffer.getArray();
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            BufferType fftBuffer = BufferType::withSize (numDivisions * fftSize);
            SampleType* fftSamples = fftBuffer.getArray();
            const SampleType* sourceBuffer = sourceBuffers.atUnchecked (channel).getArray();
            int sourceRemaining = originalLength;
            
            for (int division = 0; division < numDivisions; ++division)
            {
                const int dataLength = plonk::min (sourceRemaining, fftSizeHalved);
                const int zeroLength = fftSize - dataLength;
                
                BufferType::copyData (tempSamples, sourceBuffer, dataLength);    // copy
                BufferType::zeroData (tempSamples + dataLength, zeroLength);     // zero pad
                
                fftEngine.forward (fftSamples, tempSamples);
                
                fftSamples      += fftSize;
                sourceBuffer    += fftSizeHalved;
                sourceRemaining -= fftSizeHalved;
            }
            
            fftBuffers.add (fftBuffer);
        }
    }
    
    PLONK_INLINE_LOW SampleType* getProcessBuffer (const int channel, const int division) throw()
    {
        plonk_assert (division >= 0 && division < numDivisions);
        plonk_assert (processBuffers.length() > 0);
        return getProcessBuffer (channel) + division * fftEngine.length();
    }
    
    PLONK_INLINE_LOW const SampleType* getDivision (const int channel, const int division) const throw()
    {
        plonk_assert (division >= 0 && division < numDivisions);
        plonk_assert (fftBuffers.length() > 0);
        return getBuffer (channel) + division * fftEngine.length();
    }
    
    PLONK_INLINE_LOW int getCountDownStart (const int channel) const throw()
    {
        return countDownStart.atUnchecked (channel);
    }
    
    friend class FFTBuffersBase<SampleType>;
    
private:
    
    PLONK_INLINE_LOW const SampleType* getBuffer (const int channel) const throw()
    {
        return fftBuffers.atUnchecked ((unsigned) channel % (unsigned) fftBuffers.length()).getArray();
    }
    
    PLONK_INLINE_LOW SampleType* getProcessBuffer (const int channel) throw()
    {
        return processBuffers.atUnchecked (channel).getArray();
    }


    FFTBuffersInternal (BufferType const& singleFFTBuffer,
                        FFTEngineType const& fftEngineToUse,
                        const int originalLengthToUse,
                        const int numDivisionsToUse) throw()
    :   fftBuffers (singleFFTBuffer),
        fftEngine (fftEngineToUse),
        originalLength (originalLengthToUse),
        numDivisions (numDivisionsToUse)
    {
    }
    
    BuffersType fftBuffers;
    FFTEngineType fftEngine;
    BuffersType processBuffers;
    Ints countDownStart;
    int originalLength;
    int numDivisions;
};

//------------------------------------------------------------------------------

/** Convert buffers or signals into the frequency domain.
 The original time-domain samples are divided into segments and transformed
 into a series of separate FFTs using an FFTEgine with a specific FFT size.
 @ingroup PlonkContainerClasses */
template<class SampleType>
class FFTBuffersBase : public SmartPointerContainer< FFTBuffersInternal<SampleType> >
{
public:
    typedef FFTBuffersInternal<SampleType>          Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<FFTBuffersBase>    Weak;
    
    typedef NumericalArray<SampleType>              BufferType;
    typedef NumericalArray2D<SampleType>            BuffersType;
    typedef FFTEngineBase<SampleType>               FFTEngineType;
    typedef SignalBase<SampleType>                  SignalType;

    FFTBuffersBase() throw()
    :	Base (new Internal (FFTEngine(), BuffersType()))
    {
    }
    
    FFTBuffersBase (FFTEngineType const& fftEngine, BufferType const& sourceBuffer) throw()
    :	Base (new Internal (fftEngine, BuffersType (sourceBuffer)))
    {
    }
    
    FFTBuffersBase (FFTEngineType const& fftEngine, BuffersType const& sourceBuffers) throw()
	:	Base (new Internal (fftEngine, sourceBuffers))
	{
	}
    
    FFTBuffersBase (FFTEngineType const& fftEngine, SignalType const& sourceSignal) throw()
    :	Base (new Internal (fftEngine, sourceSignal))
    {
    }
	
    explicit FFTBuffersBase (Internal* internalToUse) throw()
	:	Base (internalToUse)
	{
	} 
    
    FFTBuffersBase (FFTBuffersBase const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}    
    
    FFTBuffersBase (Dynamic const& other) throw()
    :   Base (other.as<FFTBuffersBase>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    FFTBuffersBase& operator= (FFTBuffersBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static FFTBuffersBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
        
    static const FFTBuffersBase& getNull() throw()
	{
		static FFTBuffersBase null;
		return null;
	}	                            
    
    /** To be deprecated, wrong name!! */
    static FFTBuffersBase getUnity (FFTEngineType const& fftEngine, const int numChannels) throw()
    {
        return getIdentity (fftEngine, numChannels);
    }
    
    static FFTBuffersBase getIdentity (FFTEngineType const& fftEngine, const int numChannels) throw()
    {
        plonk_assert (numChannels > 0);
        
        BufferType bufferChannel (BufferType::newClear ((int) fftEngine.halfLength()));
        bufferChannel.atUnchecked (0) = TypeUtility<SampleType>::getTypePeak();
        BuffersType buffers;
        
        for (int i = 0; i < numChannels; ++i)
            buffers.add (bufferChannel);
            
        return FFTBuffersBase (fftEngine, buffers);
    }
    
    PLONK_OBJECTARROWOPERATOR(FFTBuffersBase);
    
    /** Get a pointer to the process "scratch" buffer for this set of buffers. */
    SampleType* getProcessBuffer (const int channel, const int division) throw()
    {
        return this->getInternal()->getProcessBuffer (channel, division);
    }
    
    /** Get a point to the samples in the FFT buffer for a particular channel and division. */
    const SampleType* getDivision (const int channel, const int division) const throw()
    {
        return this->getInternal()->getDivision (channel, division);
    }
    
    /** Get the FFTEngine used by this buffer. */
    const FFTEngineType& getFFTEngine() const throw()
    {
        return this->getInternal()->fftEngine;
    }

    /** Get the FFTEngine used by this buffer. */
    FFTEngineType& getFFTEngine() throw()
    {
        return this->getInternal()->fftEngine;
    }
    
    /** Get the number of channels in the buffers. */
    const int getNumChannels() const throw()
    {
        return this->getInternal()->fftBuffers.length();
    }
    
    /** Get the original length of the input signal in sample frames. */
    const int getOriginalLength() const throw()
    {
        return this->getInternal()->originalLength;
    }
    
    /** Get the number of FFT divisions in the buffer.*/
    const int getNumDivisions() const throw()
    {
        return this->getInternal()->numDivisions;
    }
    
    /** Gets a FFTBuffersBase representing only a single channel. */
    FFTBuffersBase getChannel (const int channel) const throw()
    {
        return FFTBuffersBase (this->getInternal()->getChannel (channel));
    }
    
    /** Gets a FFTBuffersBase representing only a single channel. */
    FFTBuffersBase operator[] (const int channel) const throw()
    {
        return getChannel (channel);
    }
    
    /** Get the randomly generated start time for a specific channel.
     This helps amortise the processing load by avoiding heavy processing happening
     in the same i/o block across many convolver instances. */
    int getCountDownStart (const int channel) const throw()
    {
        return this->getInternal()->getCountDownStart (channel);
    }
    
};

//#if PLONK_INSTANTIATE_TEMPLATES
template class FFTBuffersBase<Float>;
//#endif


#endif // PLONK_FFTBUFFERS_H
