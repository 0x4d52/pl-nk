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

#ifndef PLONK_PROXYOWNERCHANNELINTERNAL_H
#define PLONK_PROXYOWNERCHANNELINTERNAL_H

#include "../plonk_GraphForwardDeclarations.h"
#include "../utility/plonk_ProcessInfo.h"

/** Where true multiple-input-multiple-output units are needed 
 This is where all outputs depend on all inputs in a matrix configuration a 
 proxy system is used. Here one ProxyOwnerChannelInternal object is created 
 that in turn creates a series of (dumb) ProxyChannelInternal objects (which 
 essentially hold buffers of samples). Subclasses that need to operate this way 
 inherit from ProxyOwnerChannelInternal. In this case the process() function 
 processes the sample data for all channels rather than just one, writing 
 outputs for the 2nd channel and higher to the buffers in the proxies. 
 ProxyOwnerChannelInternal and ProxyChannelInternal keep references to each 
 other in a mutually dependent way so that should the channels be split into 
 separate units (and perhaps recombined later) all the channels stay in memory 
 until only mutual references remain. 
 @see LinearPan */
template<class SampleType, class DataType>
class ProxyOwnerChannelInternal : public ChannelInternal<SampleType, DataType>
{
public:    
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef SimpleArray<ChannelType>                ChannelArrayType;
    typedef ChannelInternalBase<SampleType>         InternalBase;
    typedef ChannelInternal<SampleType,DataType>    Internal;
    typedef ProxyChannelInternal<SampleType>        ProxyInternal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;    
    typedef typename ProxyInternal::Data            ProxyData;

    ProxyOwnerChannelInternal (const int numOutputs, 
                               Inputs const& inputs, 
                               DataType const& data, 
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate),
        ready (false),
        fullyInitialised (false)
    {
        plonk_assert (numOutputs >= 1);
        
        proxies.getInternal()->setSize (numOutputs, false);
        ChannelType* proxiesArray = proxies.getInternal()->getArray();
        
        InternalBase* ownerInternal = this;
        ChannelType ownerChannel = ChannelType (ownerInternal);
        proxiesArray[0] = ownerChannel;
                
        const ProxyData& proxyData (reinterpret_cast<ProxyData const&> (data));
        
        for (int i = 1; i < numOutputs; ++i)
        {
            InternalBase* proxyInternal 
                = new ProxyInternal (ownerChannel, 
                                     proxyData, 
                                     blockSize,
                                     sampleRate,
                                     i);
            
            proxiesArray[i] = ChannelType (proxyInternal);
        }        
        
        ready = true;
    }
        
    bool deleteIfOnlyMutualReferencesRemain() throw();
    
    bool isProxyOwner() const throw() { return true; }
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }
    
    inline const SampleType* getOutputSamples (const int index) const throw() 
    {         
        plonk_assert (index >= 0);
        plonk_assert (index < proxies.getInternal()->length());
        const ChannelType* proxiesArray = proxies.getInternal()->getArray();
        return proxiesArray[index].getOutputSamples();
    }
    
    inline SampleType* getOutputSamples (const int index) throw() 
    {        
        plonk_assert (index >= 0);
        plonk_assert (index < proxies.getInternal()->length());
        ChannelType* proxiesArray = proxies.getInternal()->getArray();
        return proxiesArray[index].getOutputSamples();
    }
    
    inline const Buffer getOutputBuffer (const int index) const throw() 
    {         
        plonk_assert (index >= 0);
        plonk_assert (index < proxies.getInternal()->length());
        const ChannelType* proxiesArray = proxies.getInternal()->getArray();
        return proxiesArray[index].getOutputBuffer();
    }
    
    inline Buffer getOutputBuffer (const int index) throw() 
    {                 
        plonk_assert (index >= 0);
        plonk_assert (index < proxies.getInternal()->length());
        ChannelType* proxiesArray = proxies.getInternal()->getArray();
        return proxiesArray[index].getOutputBuffer();
    }    
    
    inline const int getNumChannels() const throw()
    {
        return proxies.getInternal()->length();
    }
    
    void initProxyValue (const int index, SampleType const& value)
    {
        plonk_assert (index >= 0);
        plonk_assert (index < proxies.getInternal()->length());
        ChannelType* proxiesArray = proxies.getInternal()->getArray();
        return proxiesArray[index].initValue (value);
    }
    
    ChannelType& getProxy (const int index) throw()
    {
        plonk_assert (index >= 0);
        plonk_assert (index < proxies.getInternal()->length());
        ChannelType* proxiesArray = proxies.getInternal()->getArray();
        return proxiesArray[index];
    }
    
    void setBlockSize (BlockSize const& newBlockSize) throw()
    {
        InternalBase::setBlockSize (newBlockSize);
        
        const int numProxies = proxies.getInternal()->length();
        ChannelType* proxiesArray = proxies.getInternal()->getArray();

        for (int i = 1; i < numProxies; ++i)
            proxiesArray[i].setBlockSize (newBlockSize);
    }
    
    void setSampleRate (SampleRate const& newSampleRate) throw()
    {
        Internal::setSampleRate (newSampleRate);
        
        const int numProxies = proxies.getInternal()->length();
        ChannelType* proxiesArray = proxies.getInternal()->getArray();

        for (int i = 1; i < numProxies; ++i)
            proxiesArray[i].setSampleRate (newSampleRate);
    }
    
    void setOverlap (DoubleVariable const& newOverlap) throw()
    {
        Internal::setOverlap (newOverlap);
        
        const int numProxies = proxies.getInternal()->length();
        ChannelType* proxiesArray = proxies.getInternal()->getArray();

        for (int i = 1; i < numProxies; ++i)
            proxiesArray[i].setOverlap (newOverlap);        
    }
    
private:
    ChannelArrayType proxies;
    bool ready : 1;
    bool fullyInitialised : 1;
};


template<class SampleType, class DataType>
bool ProxyOwnerChannelInternal<SampleType,DataType>::deleteIfOnlyMutualReferencesRemain() throw()
{            
    typedef ChannelBase<SampleType> ChannelType;

    if (ready == true)
	{
		if (fullyInitialised == false)
		{
			// refCounts reach the critial state once during normal initialisation
			// this flags that state, the next time it's reached it is time to
			// destory the proxy and owner
			fullyInitialised = true;        
		}
		else
		{
			const int numProxies = proxies.getInternal()->length();
			ChannelType* proxiesArray = proxies.getInternal()->getArray();

			for (int i = 1 ; i < numProxies; ++i)
			{
				SmartPointer* p = proxiesArray[i].getInternal();
				const int proxyRefCount = p->getRefCount();
				if (proxyRefCount > 1)
					return false;
			}
            
			if (this->getRefCount() <= numProxies)
			{
				ready = false;  
                
				// this is to ensure the clean deteleting of all objects
				this->incrementRefCount();
				proxies.getInternal()->clear();

				return true;
			}
		}
	}

	return false;
}


#endif // PLONK_PROXYOWNERCHANNELINTERNAL_H

