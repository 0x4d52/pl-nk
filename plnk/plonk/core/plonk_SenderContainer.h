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

#ifndef PLONK_SENDERCONTAINER_H
#define PLONK_SENDERCONTAINER_H

#include "plonk_CoreForwardDeclarations.h"
#include "plonk_SmartPointer.h"
#include "plonk_WeakPointer.h"


template<class SenderInternalType>
class SenderContainer : public SmartPointerContainer<SenderInternalType>
{
public:
    typedef SenderInternalType              Internal;
    typedef SmartPointerContainer<Internal> Base;
    
    SenderContainer (SenderInternalType* internal = 0) throw()
    :   Base (internal)
    {
    }
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
	SenderContainer (SenderContainer const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}
    
    /** Assignment operator. */
    SenderContainer& operator= (SenderContainer const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    template<class Receiver>
    void addReceiver (Receiver* receiver) throw()
    {
        this->getInternal()->addReceiver (receiver);
    }
    
    template<class Receiver>
    void removeReceiver (Receiver* receiver) throw()
    {
        this->getInternal()->removeReceiver (receiver);
    }
    
    template<class SmartPointerType>
    void addReceiver (SmartPointerContainer<SmartPointerType>& receiver) throw()
    {        
        this->getInternal()->addReceiver (receiver.getWeakPointer(), receiver.getInternal());
    }
    
    template<class SmartPointerType>
    void removeReceiver (SmartPointerContainer<SmartPointerType>& receiver) throw()
    {
        this->getInternal()->removeReceiver (receiver.getWeakPointer(), receiver.getInternal());
    }
    
};

#endif // PLONK_SENDER_H