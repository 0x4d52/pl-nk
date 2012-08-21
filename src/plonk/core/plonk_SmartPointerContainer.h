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
        
        SmartPointerType* const tempPtr = temp.getPtrUnchecked(); // unchecked version is fine here
        
		if (tempPtr != getNullSmartPointer()) 
			tempPtr->decrementRefCount(); 
	}
    
	inline SmartPointerType* getInternal() const throw() 
	{
		return internal.getPtr(); // might need to be this safe version
	}
    
    inline SmartPointerType* operator->() throw() 
	{
		return internal.getPtr(); // might need to be this safe version
	}
    
	inline const SmartPointerType* operator->() const throw() 
	{
		return internal.getPtr(); // might need to be this safe version
	}
    
    inline SmartPointerContainerBase containerCopy() const throw()
    {
        return *this;
    }
    
	inline bool isNull() const throw()
	{
		return internal.getPtrUnchecked() == getNullSmartPointer(); // fine as the result is not reliable anyway!
	}
	
	inline bool isNotNull() const throw()
	{
		return internal.getPtrUnchecked() != getNullSmartPointer(); // fine as the result is not reliable anyway!
	}
	
	inline void setInternal (SmartPointerType* newInternal) throw()
	{
        SmartPointerContainerBase temp (newInternal);
        internal.swapWith (temp.internal);        
	}
    
    inline void swapWith (SmartPointerContainerBase& other) throw()
    {
        // is this a problem is other.internal is contended?
        internal.swapWith (other.internal);
    }
	
	inline SmartPointerContainerBase (SmartPointerContainerBase const& copy) throw()
    :   internal (getNullSmartPointer())
	{
        SmartPointerType* copyPtr = copy.internal.getPtr(); // need be safe version?
        
        if (copyPtr != getNullSmartPointer()) 
        {
            copyPtr->incrementRefCount();
            AtomicPointer temp (copyPtr);
            internal.swapWith (temp);            
        }        
	}
        
	inline SmartPointerContainerBase& operator= (SmartPointerContainerBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
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
    inline SmartPointerContainer (SmartPointerType* internalToUse = 0) throw() 
	:	SmartPointerContainerBase<SmartPointerType> (internalToUse)
	{
	}
    
    inline SmartPointerContainer (SmartPointerContainer const& copy) throw()
	:	SmartPointerContainerBase<SmartPointerType> (copy)
	{
	}
    
    inline SmartPointerContainer& operator= (SmartPointerContainer const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
		return *this;		
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
    
    inline SmartPointerContainer& operator= (SmartPointerContainer const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
		return *this;		
	}    

    WeakPointer* getWeakPointer() const throw()
    {
        const SmartPointerType* const p = this->getInternal();
            
        if (p != Base::getNullSmartPointer())
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

template<class ScopedPointerType>
class ScopedPointerContainer
{
public:
    typedef AtomicValue<ScopedPointerType*> AtomicPointer;

    ScopedPointerContainer (ScopedPointerType* const raw) throw()
    :   internal (raw)
    {
    }
    
    ~ScopedPointerContainer()
    {
        destroy (*this);
    }
    
    inline ScopedPointerContainer (ScopedPointerContainer& copy) throw()
    :   internal (0)
	{
        this->swapWith (copy);        
	}
    
	inline ScopedPointerContainer& operator= (ScopedPointerContainer& other) throw()
	{
		if (this != &other)
        {
            destroy (*this);
            this->swapWith (other);
        }
        
		return *this;		
	}    
    
    inline static void destroy (ScopedPointerContainer& container) throw()
    {
        AtomicPointer tmp (0);
        container.internal.swapWith (tmp);
        delete tmp.getPtrUnchecked(); // unchecked is fine here
    }
    
    inline void swapWith (ScopedPointerContainer& other) throw()
    {
        // other might be contended
        internal.swapWith (other.internal);
    }
    
    inline ScopedPointerType* operator->() throw() 
    { 
        plonk_assert(internal.getPtrUnchecked() != 0);
        return internal.getPtr(); 
    }
    
	inline const ScopedPointerType* operator->() const throw() 
    { 
        plonk_assert(internal.getPtrUnchecked() != 0);
        return internal.getPtr(); 
    }
    
    inline ScopedPointerType& operator*() throw() 
    { 
        plonk_assert(internal.getPtrUnchecked() != 0);
        return *internal.getPtr(); 
    }
    
	inline const ScopedPointerType& operator*() const throw() 
    { 
        plonk_assert(internal.getPtrUnchecked() != 0);
        return *internal.getPtr(); 
    }
    
private:
    AtomicPointer internal;
    
    ScopedPointerContainer& operator= (ScopedPointerContainer const&);
    ScopedPointerContainer (ScopedPointerContainer const&);
};

#endif // PLONK_SMARTPOINTERCONTAINER_H
