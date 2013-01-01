/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#ifndef PLONK_SENDER_H
#define PLONK_SENDER_H

#include "plonk_CoreForwardDeclarations.h"
#include "plonk_SmartPointer.h"
#include "plonk_WeakPointer.h"

#include "../containers/plonk_ContainerForwardDeclarations.h"
#include "../containers/plonk_SimpleArray.h"


template<class SenderContainerBaseType>
class SenderInternal : public SmartPointer
{
public:    
    typedef typename SenderContainerBaseType::Internal          SenderInternalBaseType;    
    typedef ReceiverInternal<SenderContainerBaseType>           Receiver;
        
    typedef ObjectArrayInternal<Receiver*,SmartPointer>         RawReceiverArrayInternal;
    typedef ObjectArrayInternal<WeakPointer*,SmartPointer>      WeakReceiverOwnerArrayInternal;

    SenderInternal() throw();
    ~SenderInternal();
        
    void addReceiver(Receiver* const receiver) throw();
    void removeReceiver(Receiver* const receiver) throw();
    
    void addReceiver(WeakPointer* const owner, Receiver* const receiver) throw();
    void removeReceiver(WeakPointer* const owner, Receiver* const receiver) throw();
    
    void update (Text const& message, Dynamic const& payload) throw();
    void updateRaw (Text const& message, Dynamic const& payload) throw();
    void updateWeak (Text const& message, Dynamic const& payload) throw();
    
private:
    SimpleArray<Receiver*>      rawReceivers;
    SimpleArray<WeakPointer*>   weakReceiverOwners;
    SimpleArray<Receiver*>      weakReceivers;
};


//------------------------------------------------------------------------------


template<class SenderContainerBaseType>
SenderInternal<SenderContainerBaseType>::SenderInternal() throw()
{
}


template<class SenderContainerBaseType>
SenderInternal<SenderContainerBaseType>::~SenderInternal()
{
}    

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::addReceiver (Receiver* const receiver) throw()
{
    plonk_assert (receiver != 0);
    
    if (rawReceivers.getInternal()->indexOf (receiver) < 0)
        rawReceivers.getInternal()->add (receiver);
}

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::removeReceiver (Receiver* const receiver) throw()
{
    plonk_assert (receiver != 0);
    
    int index = rawReceivers.getInternal()->indexOf (receiver);
    
    if (index >= 0)
        rawReceivers.getInternal()->remove (index);  
}

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::addReceiver (WeakPointer* const owner, Receiver* const receiver) throw()
{    
    plonk_assert (receiver != 0);
    plonk_assert (owner != 0);
    plonk_assert (owner->getWeakPointer() != 0);
            
    if (weakReceivers.getInternal()->indexOf (receiver) < 0)
    {
        plonk_assert (weakReceiverOwners.getInternal()->indexOf (owner) < 0);
        
        owner->incrementRefCount();
        weakReceiverOwners.getInternal()->add (owner);
        weakReceivers.getInternal()->add (receiver);
    }
}

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::removeReceiver (WeakPointer* const owner, Receiver* const receiver) throw()
{
    plonk_assert (receiver != 0);
    plonk_assert (owner != 0);

    int index = weakReceivers.getInternal()->indexOf (receiver);
    
    if (index >= 0)
    {
        plonk_assert (weakReceiverOwners.getInternal()->indexOf (owner) == index);
        
        weakReceivers.getInternal()->remove (index);
        weakReceiverOwners.getInternal()->remove (index);
        owner->decrementRefCount();
    }
}

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::update (Text const& message, Dynamic const& payload) throw()
{    
    this->updateRaw (message, payload);
    this->updateWeak (message, payload);
}    

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::updateRaw (Text const& message, Dynamic const& payload) throw()
{    
    RawReceiverArrayInternal* arrayInternal = rawReceivers.getInternal();
    plonk_assert (arrayInternal != 0);
    
    const int numReceivers = arrayInternal->length();
    if (numReceivers > 0)
    {        
        SenderInternalBaseType* pthis = static_cast<SenderInternalBaseType*> (this);
        SenderContainerBaseType sender (pthis);            
        
        for (int i = numReceivers; --i >= 0;)
        {
            arrayInternal->getArray()[i]->changed (sender, message, payload);
        }
    }    
}    

template<class SenderContainerBaseType>
void SenderInternal<SenderContainerBaseType>::updateWeak (Text const& message, Dynamic const& payload) throw()
{    
    WeakReceiverOwnerArrayInternal* ownerArray = weakReceiverOwners.getInternal();
    plonk_assert (ownerArray != 0);

    const int numReceivers = ownerArray->length();
        
    if (numReceivers > 0)
    {     
        plonk_assert (weakReceivers.getInternal() != 0);
        plonk_assert (weakReceivers.getInternal()->length() == numReceivers);
        
        RawReceiverArrayInternal* reveiverArray = weakReceivers.getInternal();
        
        SenderInternalBaseType* pthis = static_cast<SenderInternalBaseType*> (this);
        SenderContainerBaseType sender (pthis);            
        
        for (int i = numReceivers; --i >= 0;)
        {            
            WeakPointer* owner = ownerArray->getArray()[i];
            Receiver* receiver = reveiverArray->getArray()[i];
            
            if (owner->getWeakPointer() != 0)
                receiver->changed (sender, message, payload);
            else
                this->removeReceiver (owner, receiver);
        }
    }    
}    





#endif // PLONK_SENDER_H