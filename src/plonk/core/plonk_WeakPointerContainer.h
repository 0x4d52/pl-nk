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

#ifndef PLONK_WEAKPOINTERCONTAINER_H
#define PLONK_WEAKPOINTERCONTAINER_H

#include "plonk_CoreForwardDeclarations.h"
#include "plonk_SmartPointer.h"
#include "plonk_SmartPointerContainer.h"

/** This stores a weak reference using a WeakPointer. 
 The orginal object can be obtained using fromWeak() but if the object has been 
 since deleted the object return will be a new, default object of the specified 
 type rather than the original. This is useful for avoiding circular 
 dependencies in reference counted situations. 
 @see WeakPointer */
template<class OriginalType>
class WeakPointerContainer : public SmartPointerContainer<WeakPointer,false>
{
public:
    typedef SmartPointerContainer<WeakPointer,false>    Base;
    typedef typename OriginalType::Internal             OriginalInternal;

    WeakPointerContainer() throw()
    :   Base (0)
    {
    }        
    
    ~WeakPointerContainer()
    {
        decrementWeakCount (this->getInternal());
    }
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
	WeakPointerContainer (WeakPointerContainer const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
        incrementWeakCount (this->getInternal());
	}
    
    /** Assignment operator. */
    WeakPointerContainer& operator= (WeakPointerContainer const& other) throw()
	{
		if (this != &other)
        {
            WeakPointer* const weakPointer = this->getInternal();
            WeakPointer* const otherWeakPointer = other.getInternal();

            incrementWeakCount (otherWeakPointer);
            decrementWeakCount (weakPointer);

            this->setInternal (otherWeakPointer);
        }
        
        return *this;
    }
    
    WeakPointerContainer (OriginalType const& original) throw()
    :   Base (original.getWeakPointer())
    {
        incrementWeakCount (this->getInternal());
    }
        
    OriginalInternal* getWeakPointer() const throw()
    {
        OriginalInternal* smartPointer = 0;
        const WeakPointer* const weakPointer = this->getInternal();
        
        if (weakPointer != 0)
            smartPointer = reinterpret_cast<OriginalInternal*> (weakPointer->getWeakPointer());
        
        return smartPointer;
    }
    
    bool isAlive() const throw()
    {
        return this->getWeakPointer() != 0;
    }
    
    OriginalType fromWeak() const throw()
    {
        OriginalInternal* smartPointer = this->getWeakPointer();
        
        if (smartPointer != 0)
            return OriginalType (smartPointer);
        else
            return OriginalType();
    }
    
private:
    inline static void incrementWeakCount (WeakPointer* const weakPointer) throw()
    {
        if (weakPointer != 0)
            weakPointer->incrementWeakCount();
    }
    
    inline static void decrementWeakCount (WeakPointer* const weakPointer) throw()
    {
        if (weakPointer != 0)
            weakPointer->decrementWeakCount();
    }
};


#endif // PLONK_WEAKPOINTERCONTAINER_H
