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

#ifndef PLONK_SIGNAL_H
#define PLONK_SIGNAL_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "plonk_ObjectArray.h"
#include "plonk_SimpleArray.h"

#include "../graph/utility/plonk_SampleRate.h"



template<class SampleType>
class SignalInternal : public SmartPointer
{
public:
    typedef NumericalArray<SampleType>      Buffer;
    typedef NumericalArray2D<SampleType>    Buffers;
    typedef Variable<SampleType>            SampleVariable;
    typedef NumericalArray<SampleVariable>  SampleVariableArray;
    
    PLONK_INLINE_LOW SignalInternal() throw()
    :   numInterleavedChannels (1),
        channel (-1),
        offset (0),
        numFrames (-1)
    {
        init();
    }
   
    PLONK_INLINE_LOW SignalInternal (Buffers const& buffersToUse, 
                           SampleRate const& sampleRateToUse) throw()
    :   buffers (buffersToUse),
        sampleRate (sampleRateToUse),
        numInterleavedChannels (1),
        channel (-1),
        offset (0),
        numFrames (-1)
    {
        plonk_assert ((buffers.length() == 1) || buffers.isMatrix()); // each channel must be the same length
        init();
    }
    
    PLONK_INLINE_LOW SignalInternal (Buffer const& buffer, 
                           SampleRate const& sampleRateToUse,
                           const int interleavedChannelsInBuffer) throw()
    :   buffers (buffer),
        sampleRate (sampleRateToUse),
        numInterleavedChannels (interleavedChannelsInBuffer),
        channel (-1),
        offset (0),
        numFrames (-1)
    {
        plonk_assert (numInterleavedChannels > 0);
        init();
    }
    
    PLONK_INLINE_LOW SignalInternal (Buffers const& buffersToUse,
                           SampleRate const& sampleRateToUse,
                           const int interleavedChannelsInBuffer,
                           const int channelToUse,
                           const int offsetToUse,
                           const int numFramesToUse) throw()
    :   buffers (buffersToUse),
        sampleRate (sampleRateToUse),
        numInterleavedChannels (interleavedChannelsInBuffer),
        channel (channelToUse),
        offset (offsetToUse),
        numFrames (numFramesToUse)
    {
        plonk_assert (numInterleavedChannels > 0);
        init();
    }
    
    PLONK_INLINE_LOW int getNumFrames() const throw()
    {   
        const int numFramesPerBuffer = buffers.numColumns();
        const int bufferFrames = (numFramesPerBuffer / numInterleavedChannels) - offset;
        return numFrames < 0 ? bufferFrames : plonk::min (numFrames, bufferFrames);
    }
    
    PLONK_INLINE_LOW int getNumChannels() const throw()
    {
        if (channel >= 0)
        {
            return 1;
        }
        else if (numInterleavedChannels > 1)
        {
            plonk_assert (buffers.numRows() == 1);
            return numInterleavedChannels;
        }
        else
        {
            return buffers.numRows();
        }
    }
    
//    PLONK_INLINE_LOW int getFrameStride() const throw()
//    {
//        return isInterleaved() ? getNumChannels() : 1;
//    }

    PLONK_INLINE_LOW int getFrameStride() const throw()
    {
        return isInterleaved() ? numInterleavedChannels : 1;
    }
        
    SampleType* getSamples (const int channel) throw()
    {
        const int wrappedChannel = channel < 0 ? channel : plonk::wrap (channel, 0, getNumChannels());
        return isInterleaved() ? buffers.atUnchecked (0).getArray() + offset * numInterleavedChannels + wrappedChannel
                                 :
                                 buffers.atUnchecked (wrappedChannel).getArray() + offset;
    }
    
    const SampleType* getSamples (const int channel) const throw()
    {
        const int wrappedChannel = channel < 0 ? channel : plonk::wrap (channel, 0, getNumChannels());
        return isInterleaved() ? buffers.atUnchecked (0).getArray() + offset * numInterleavedChannels + wrappedChannel
                                 :
                                 buffers.atUnchecked (wrappedChannel).getArray() + offset;
    }

    PLONK_INLINE_LOW bool isInterleaved() const throw()
    {
        plonk_assert(buffers.length() > 0);
        return (numInterleavedChannels > 1) || (buffers.numRows() == 1);
    }
        
    void normalise() throw()
    {
        buffers.normalise();
    }
        

    
//    PLONK_INLINE_LOW Buffer getInterleaved() const throw()
//    {
//        plonk_assert(buffers.length() > 0);
//        return (isInterleaved()) ? buffers.atUnchecked (0) : buffers.interleave();
//    }
//    
//    PLONK_INLINE_LOW Buffers getDeinterleaved() const throw()
//    {        
//        plonk_assert(buffers.length() > 0);
//        if (isInterleaved()) 
//            return buffers.atUnchecked (0).deinterleave (numInterleavedChannels);
//        else
//            return buffers;
//    }
    
    friend class SignalBase<SampleType>;
    
private:
    Buffers buffers;
    SampleRate sampleRate;
    const int numInterleavedChannels;
    int channel;
    int offset;
    int numFrames;
        
    SampleVariableArray sampleVariables;
    IntVariableArray intVariables;
        
    void init() throw()
    {
        sampleVariables.setSize (buffers.length(), false);
        intVariables.setSize (buffers.length(), false);
    }
};

//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class SampleType>
class SignalBase : public SmartPointerContainer< SignalInternal<SampleType> >
{
public:
    typedef SignalInternal<SampleType>              Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<SignalBase>        Weak;    
    
    typedef typename Internal::Buffer               Buffer;
    typedef typename Internal::Buffers              Buffers;
    typedef typename Internal::SampleVariable       SampleVariable;
    typedef typename Internal::SampleVariableArray  SampleVariableArray;
    
    SignalBase() throw()
    :   Base (new Internal ())
    {
    }
    
    SignalBase (Buffers const& buffers, 
                SampleRate const& sampleRate = SampleRate::getDefault()) throw()
    :   Base (new Internal (buffers, sampleRate))
    {
    }
    
    SignalBase (Buffer const& buffer, 
                SampleRate const& sampleRate = SampleRate::getDefault(),
                const int interleavedChannelsInBuffer = 1) throw()
    :   Base (new Internal (buffer, sampleRate, interleavedChannelsInBuffer))
    {
    }
	
    explicit SignalBase (Internal* internalToUse) throw() 
    :   Base (internalToUse)
    {
    } 
    
    SignalBase (SignalBase const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }    
    
    SignalBase (Dynamic const& other) throw()
    :   Base (other.as<SignalBase>().getInternal())
    {
    }
    
    /** Assignment operator. */
    SignalBase& operator= (SignalBase const& other) throw()
	{
        if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static SignalBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    static const SignalBase& getNull() throw()
    {
        static SignalBase null;        
        return null;
    }
    
    SignalBase getSelection (const int offset, const int numFrames = -1) const throw()
    {
        int offsetToUse = plonk::clip (offset, 0, this->getNumFrames());

        Internal* internal = new Internal (this->getInternal()->buffers,
                                           this->getInternal()->sampleRate,
                                           this->getInternal()->numInterleavedChannels,
                                           this->getInternal()->channel,
                                           plonk::clip (this->getInternal()->offset + offsetToUse,
                                                        this->getInternal()->offset,
                                                        this->getInternal()->offset + this->getNumFrames()),
                                           plonk::clip (numFrames, -1, this->getNumFrames() - offsetToUse));
        return SignalBase (internal);
    }
    
    SignalBase getChannel (const int channel) const throw()
    {
        Internal* internal = new Internal (this->getInternal()->buffers,
                                           this->getInternal()->sampleRate,
                                           this->getInternal()->numInterleavedChannels,
                                           this->getInternal()->channel < 0 ? channel : this->getInternal()->channel,
                                           this->getInternal()->offset,
                                           this->getInternal()->numFrames);
        return SignalBase (internal);
    }
    
//    PLONK_INLINE_LOW const Buffers& getBuffers() const throw()
//    {
//        return this->getInternal()->buffers;
//    }
//
//    PLONK_INLINE_LOW Buffers& getBuffers() throw()
//    {
//        return this->getInternal()->buffers;
//    }

    PLONK_INLINE_LOW const SampleRate& getSampleRate() const throw()
    {
        return this->getInternal()->sampleRate;
    }
    
    PLONK_INLINE_LOW SampleRate& getSampleRate() throw()
    {
        return this->getInternal()->sampleRate;
    }
    
    PLONK_INLINE_LOW int getNumFrames() const throw()
    {
        return this->getInternal()->getNumFrames();
    }
    
    PLONK_INLINE_LOW double getDuration() const throw()
    {
        return this->getInternal()->getNumFrames() / this->getInternal()->sampleRate.getValue();
    }
    
    PLONK_INLINE_LOW int getNumChannels() const throw()
    {
        return this->getInternal()->getNumChannels();
    }
    
    PLONK_INLINE_LOW int getFrameStride() const throw()
    {
        return this->getInternal()->getFrameStride();
    }
    
    SampleType* getSamples (const int channel) throw()
    {
        return this->getInternal()->getSamples (channel);
    }
    
    const SampleType* getSamples (const int channel) const throw()
    {
        return this->getInternal()->getSamples (channel);
    }
    
    PLONK_INLINE_LOW bool isInterleaved() const throw()
    {
        return this->getInternal()->isInterleaved();
    }
    
    SignalBase normalise() throw()
    {
        this->getInternal()->normalise();
        return *this;
    }


//    PLONK_INLINE_LOW Buffer getInterleaved() const throw()
//    {
//        return this->getInternal()->getInterleaved();
//    }
//    
//    PLONK_INLINE_LOW Buffers getDeinterleaved() const throw()
//    {
//        return this->getInternal()->getDeinterleaved();
//    }
    
    PLONK_INLINE_LOW SampleVariableArray& getSampleVariables() throw()
    {
        return this->getInternal()->sampleVariables;
    }

    PLONK_INLINE_LOW const SampleVariableArray& getSampleVariables() const throw()
    {
        return this->getInternal()->sampleVariables;
    }

    PLONK_INLINE_LOW IntVariableArray& getIntVariables() throw()
    {
        return this->getInternal()->intVariables;
    }
    
    PLONK_INLINE_LOW const IntVariableArray& getIntVariables() const throw()
    {
        return this->getInternal()->intVariables;
    }
    
    SampleType getMaxAbsSum() const
    {
        Internal* const internal = this->getInternal();
        SampleType maxSum (Math<SampleType>::get0());

        for (int i = 0; i < internal->getNumChannels(); ++i)
            maxSum = plonk::max (maxSum, this->getAbsSum (i));
        
        return maxSum;
    }
    
    SampleType getAbsSum (const int channel) const
    {
        Internal* const internal  = this->getInternal();
        const SampleType* samples = internal->getSamples (channel);
        const int frameStride     = internal->getFrameStride();
        
        SampleType sum (Math<SampleType>::get0());
        
        for (int i = 0; i < internal->getNumFrames(); ++i)
        {
            sum     += plonk::abs (*samples);
            samples += frameStride;
        }
        
        return sum;
    }


    PLONK_OBJECTARROWOPERATOR(SignalBase);

};



#endif // PLONK_SIGNAL_H
