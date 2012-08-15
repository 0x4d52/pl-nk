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

#ifndef PLONK_SMARTPOINTERCONTAINER_H
#define PLONK_SMARTPOINTERCONTAINER_H

#include "plonk_SmartPointer.h"
#include "plonk_WeakPointer.h"


#define PLONK_OBJECTARROWOPERATOR(Type) \
    inline Type* operator->() throw() { return this; } \
	inline const Type* operator->() const throw() { return this; }


/** This manages a SmartPointer pointer. 
 This class and its subclasses are passed around by value (or reference) and 
 if a copy is made it actually just copies the SmartPointer it contains 
 incrementing its reference count. As SmartPointerContainer objects are deleted 
 the reference count of its contained SmartPointer are decremented (thus 
 deleting them only if the reference count reaches zero). Almost all other 
 Plonk concepts (i.e., arrays, channels, units) comprise an 'internal' 
 (which is a subclass of SmartPointer) and 'external' (which is a subclass of 
 SmartPointerContainer) representation. 
 @see SenderContainer */
template<class SmartPointerType>
class SmartPointerContainerBase : public PlonkBase
{
public: 
    typedef AtomicValue<SmartPointerType*> AtomicPointer;
    
    inline static SmartPointerType* getNullSmartPointer() throw() 
    { 
        return static_cast<SmartPointerType*> (0); 
    }
    
	inline SmartPointerContainerBase() throw() 
	:	internal (getNullSmartPointer())
	{
	}
    
    inline SmartPointerContainerBase (SmartPointerType* internalToUse) throw() 
	:	internal (getNullSmartPointer())
	{
        if (internalToUse != getNullSmartPointer())
        {
            internalToUse->incrementRefCount();
            AtomicPointer temp (internalToUse);
            internal.swapWith (temp);
        }
	}
    
    inline ~SmartPointerContainerBase()
	{        
        AtomicPointer temp (getNullSmartPointer());
        internal.swapWith (temp);
        
		if (temp != getNullSmartPointer()) 
			temp->decrementRefCount();		
	}
    
	inline SmartPointerType* getInternal() const throw() 
	{
		return internal;
	}
    
    inline SmartPointerType* operator->() throw() 
	{
		return internal;
	}
    
	inline const SmartPointerType* operator->() const throw() 
	{
		return internal;
	}
    
    inline SmartPointerContainerBase containerCopy() const throw()
    {
        return *this;
    }
    
	inline bool isNull() const throw()
	{
		return internal == getNullSmartPointer();
	}
	
	inline bool isNotNull() const throw()
	{
		return internal != getNullSmartPointer();
	}
	
	inline void setInternal (SmartPointerType* newInternal) throw()
	{
        SmartPointerContainerBase temp (newInternal);
        internal.swapWith (temp.internal);        
	}
    
    inline void swapWith (SmartPointerContainerBase& other) throw()
    {
        internal.swapWith (other.internal);
    }
	
	inline SmartPointerContainerBase (SmartPointerContainerBase const& copy) throw()
    :   internal (getNullSmartPointer())
	{
        if (copy.internal != getNullSmartPointer()) 
        {
            copy.internal->incrementRefCount();
            AtomicPointer temp (copy.internal.getPtrUnchecked());
            internal.swapWith (temp);
        }        
	}
        
	inline SmartPointerContainerBase& operator= (SmartPointerContainerBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.internal);
        
		return *this;		
	}    
    
	inline bool operator== (SmartPointerContainerBase const& other) const throw()
	{
		return internal == other.internal;
	}
	
	inline bool operator!= (SmartPointerContainerBase const& other) const throw()
	{
		return internal != other.internal;
	}
    
private:
    AtomicPointer internal;
};


template<class SmartPointerType, bool enableWeak>
class SmartPointerContainer : public SmartPointerContainerBase<SmartPointerType>
{
public:
    SmartPointerContainer (SmartPointerType* internalToUse = 0) throw() 
	:	SmartPointerContainerBase<SmartPointerType> (internalToUse)
	{
	}
    
    SmartPointerContainer (SmartPointerContainer const& copy) throw()
	:	SmartPointerContainerBase<SmartPointerType> (copy)
	{
	}
    
};

template<class SmartPointerType>
class SmartPointerContainer<SmartPointerType, true> : public SmartPointerContainerBase<SmartPointerType>
{
public:    
    typedef SmartPointerContainerBase<SmartPointerType> Base;
    
    inline SmartPointerContainer (SmartPointerType* internalToUse = 0) throw() 
	:	SmartPointerContainerBase<SmartPointerType> (internalToUse)
	{
	}
    
    inline SmartPointerContainer (SmartPointerContainer const& copy) throw()
	:	SmartPointerContainerBase<SmartPointerType> (copy)
	{
	}    
    
    WeakPointer* getWeakPointer() const throw()
    {
        const SmartPointerType* const p = this->getInternal();
            
        if ((p != 0) && (p != Base::getNullSmartPointer())) // can't remember why :(
        {
            WeakPointer* w = static_cast<WeakPointer*> (p->getWeak());            
            return w;
        }
        
        return 0;
    }
    
};


template<class SmartPointerContainerType>
void swap (SmartPointerContainerType& a, 
           SmartPointerContainerType& b) throw()
{
    a.swapWith (b);
}


#endif // PLONK_SMARTPOINTERCONTAINER_H
