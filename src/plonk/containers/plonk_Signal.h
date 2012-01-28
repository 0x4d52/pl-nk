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
    
    inline SignalInternal() throw()
    :   numInterleavedChannels (1)
    {
    }
   
    inline SignalInternal (Buffers const& buffersToUse, 
                           SampleRate const& sampleRateToUse) throw()
    :   buffers (buffersToUse),
        sampleRate (sampleRateToUse),
        numInterleavedChannels (1)
    {
        plonk_assert (buffers.isMatrix()); // each channel must be the same length
    }
    
    inline SignalInternal (Buffer const& buffer, 
                           SampleRate const& sampleRateToUse,
                           const int interleavedChannelsInBuffer) throw()
    :   buffers (buffer),
        sampleRate (sampleRateToUse),
        numInterleavedChannels (interleavedChannelsInBuffer)
    {
        plonk_assert (numInterleavedChannels > 0);
    }
    
    inline int getNumFrames() const throw()
    {   
        const int numFramesPerBuffer = buffers.numColumns();
        return numFramesPerBuffer / numInterleavedChannels;
    }
    
    inline int getNumChannels() const throw()
    {
        if (numInterleavedChannels > 1)
        {
            plonk_assert (buffers.numRows() == 1);
            return numInterleavedChannels;
        }
        else
        {
            return buffers.numRows();
        }
    }
    
    inline Buffer getInterleaved() const throw()
    {
        return (numInterleavedChannels > 1) ? buffers.first() : buffers.interleave();
    }
    
    inline Buffers getDeinterleaved() const throw()
    {        
        if (numInterleavedChannels > 1) 
            return buffers.first().deinterleave (numInterleavedChannels);
        else
            return buffers;
    }
    
    friend class SignalBase<SampleType>;
    
private:
    Buffers buffers;
    SampleRate sampleRate;
    const int numInterleavedChannels;
};

//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class SampleType>
class SignalBase : public SmartPointerContainer< SignalInternal<SampleType> >
{
public:
    typedef SignalInternal<SampleType>          Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef WeakPointerContainer<SignalBase>    Weak;    
    
    typedef typename Internal::Buffer           Buffer;
    typedef typename Internal::Buffers          Buffers;
    
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
            this->setInternal (other.containerCopy().getInternal());
        
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
    
    inline const Buffers& getBuffers() const throw()
    {
        return this->getInternal()->buffers;
    }

    inline Buffers& getBuffers() throw()
    {
        return this->getInternal()->buffers;
    }
    
    inline const SampleRate& getSampleRate() const throw()
    {
        return this->getInternal()->sampleRate;
    }
    
    inline void setSampleRate (SampleRate const& sampleRate) throw()
    {
        this->getInternal()->sampleRate = sampleRate;
    }

    inline int getNumFrames() const throw()
    {
        return this->getInternal()->getNumFrames();
    }
    
    inline int getNumChannels() const throw()
    {
        return this->getInternal()->getNumChannels();
    }
    
    inline bool isInterleaved() const throw()
    {
        return this->getInternal()->numInterleavedChannels > 1;
    }

    inline Buffer getInterleaved() const throw()
    {
        return this->getInternal()->getInterleaved();
    }
    
    inline Buffers getDeinterleaved() const throw()
    {
        return this->getInternal()->getDeinterleaved();
    }

    
};



#endif // PLONK_SIGNAL_H
