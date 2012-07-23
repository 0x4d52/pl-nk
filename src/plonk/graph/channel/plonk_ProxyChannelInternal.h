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

#ifndef PLONK_PROXYCHANNELINTERNAL_H
#define PLONK_PROXYCHANNELINTERNAL_H

#include "../plonk_GraphForwardDeclarations.h"
#include "../utility/plonk_ProcessInfo.h"
#include "../utility/plonk_BlockSize.h"


template<class SampleType>
class ProxyChannelInternal : public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data  Data;

    typedef ChannelBase<SampleType>             ChannelType;
    typedef ObjectArray<ChannelType>            ChannelArrayType;
    typedef ChannelInternal<SampleType,Data>    Internal;
    typedef ChannelInternalBase<SampleType>     InternalBase;
    typedef UnitBase<SampleType>                UnitType;
    typedef InputDictionary                     Inputs;
    typedef NumericalArray<SampleType>          Buffer;
 
    ProxyChannelInternal (ChannelType const& proxyOwner, 
                          Data const& data, 
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate, 
                          const int index) throw()
    :   Internal (Inputs(), data, blockSize, sampleRate),
        owner (proxyOwner),
        proxyIndex (index)
    {
    }
    
    Text getName() const throw()
    {
        const InternalBase* proxyOwner 
            = static_cast<const InternalBase*> (owner.getInternal());

        return "Proxy (" 
                + proxyOwner->getName() 
                + " Channel " 
                + Text (proxyIndex)
                + ")";
    }
    
    IntArray getInputKeys() const throw()
    {
        const InternalBase* proxyOwner 
            = static_cast<const InternalBase*> (owner.getInternal());
        
        return proxyOwner->getInputKeys();
    }    
    
    Text getLabel() const throw() 
    { 
        const InternalBase* proxyOwner 
            = static_cast<const InternalBase*> (owner.getInternal());

        const Text ownerLabel = proxyOwner->getLabel();
        
        if (ownerLabel.length() > 0)
            return proxyOwner->getLabel() + " (Proxy Channel " + Text (proxyIndex) + ")"; 
        else
            return Text::getEmpty();
    }
    
    void setLabel(Text const& newId) throw()
    {
        InternalBase* proxyOwner 
            = static_cast<InternalBase*> (owner.getInternal());
        
        proxyOwner->setLabel (newId);
    }
    
    bool isProxy() const throw() { return true; }
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }    
    
    int getNumChannels() const throw()
    {
        return owner.getNumChannels();
    }
    
    void initChannel (const int channel) throw()
    {        
        InternalBase* proxyOwner 
            = static_cast<InternalBase*> (owner.getInternal());
        
        proxyOwner->initChannel (channel);
    }    
    
//    bool deleteIfOnlyMutualReferencesRemain() throw()
//    {
//        InternalBase* proxyOwner 
//            = static_cast<InternalBase*> (owner.getInternal());
//        
//        if (proxyOwner != 0)
//            return proxyOwner->deleteIfOnlyMutualReferencesRemain();    
//
//		return false;
//    }
            
    void process (ProcessInfo& info, const int channel) throw()
    {        
        owner.process (info, channel);
    }
    
private:
    ChannelType owner;
    const int proxyIndex;
};


#endif // PLONK_PROXYCHANNELINTERNAL_H

