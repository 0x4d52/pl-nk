/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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
		return internal.getPtr();
	}
    
    inline SmartPointerType* operator->() throw() 
	{
		return internal.getPtr();
	}
    
	inline const SmartPointerType* operator->() const throw() 
	{
		return internal.getPtr();
	}
        
	inline bool isNull() const throw()
	{
		return internal.getPtr() == getNullSmartPointer();
	}
	
	inline bool isNotNull() const throw()
	{
		return internal.getPtr() != getNullSmartPointer();
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
        SmartPointerType* copyPtr = copy.internal.getPtr();
        
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
	:	SmartPointerContainerBase<SmartPointerType> (static_cast<Base const&> (copy))
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

    ScopedPointerContainer() throw()
    :   internal (0)
    {
    }
    
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

    inline ScopedPointerContainer& operator= (ScopedPointerType* const raw) throw()
	{
		ScopedPointerContainer tmp (raw);
        this->swapWith (tmp);                
		return *this;		
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
    
    inline ScopedPointerType* getInternal() throw() 
    {
        return internal.getPtr(); 
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
    
    template<class OtherType>
    inline bool operator== (OtherType const& other) const throw()
	{
		return internal.getPtrUnchecked() == other;
	}
	
    template<class OtherType>
	inline bool operator!= (OtherType const& other) const throw()
	{
		return internal.getPtrUnchecked() != other;
	}
        
private:
    AtomicPointer internal;    
};

//------------------------------------------------------------------------------

#define PLONKSMARTPOINTERCONTAINER_DEEPCOPY(CONTAINERTYPE,SMARTPOINTERTYPE)\
    inline CONTAINERTYPE deepCopy() const throw()\
    {\
        SMARTPOINTERTYPE* const theCopy = static_cast<SMARTPOINTERTYPE*> (this->getInternal()->deepCopy());\
        plonk_assert (theCopy != 0);\
        return CONTAINERTYPE (theCopy);\
    }
    
//------------------------------------------------------------------------------
    
    
template<class PlankSharedPtrType> class PlankWeakPtrContainer;
    
template<class PlankSharedPtrType>
class PlankSharedPtrContainer : public PlonkBase
{
public:
    typedef PlankSharedPtrType              Internal;
    typedef AtomicValue<Internal>           AtomicPointer;
    typedef PlankWeakPtrContainer<Internal> Weak;

    inline static Internal getNullSharedPtr() throw()
    {
        return static_cast<Internal> (0);
    }
    
    inline static PlankSharedPtrContainer getNull() throw()
    {
        static PlankSharedPtrContainer null (getNullSharedPtr());
        return null;
    }
    
    PlankSharedPtrContainer (Internal p) throw()
    :   internal (p ? reinterpret_cast<Internal> (pl_SharefPtr_IncrementRefCountAndGetPtr (reinterpret_cast<PlankSharedPtrRef> (p))) : getNullSharedPtr())
    {
    }
    
    PlankSharedPtrContainer (Weak const& weak) throw()
    :   internal ()
    {
    }
    
    inline PlankSharedPtrContainer (PlankSharedPtrContainer const& copy) throw()
    :   internal (getNullSharedPtr())
	{
        Internal const copyPtr = copy.internal.getPtr();
        
        if (copyPtr != getNullSharedPtr())
        {
            AtomicPointer temp (reinterpret_cast<Internal> (pl_SharefPtr_IncrementRefCountAndGetPtr (reinterpret_cast<PlankSharedPtrRef> (copyPtr))));
            internal.swapWith (temp);
        }
	}
    
    ~PlankSharedPtrContainer()
    {
        AtomicPointer temp (getNullSharedPtr());
        internal.swapWith (temp);
        
        Internal const tempPtr = temp.getPtrUnchecked(); // unchecked version is fine here
        
		if (tempPtr != getNullSharedPtr())
			pl_SharedPtr_DecrementRefCount (reinterpret_cast<PlankSharedPtrRef> (tempPtr));
    }

    inline PlankSharedPtrContainer& operator= (PlankSharedPtrContainer const& other) throw()
    {
        if (this != &other)
            this->setInternal (other.getInternal());
            
        return *this;
    }
    
    inline void setInternal (Internal newInternal) throw()
	{
        PlankSharedPtrContainer temp (newInternal);
        internal.swapWith (temp.internal);
	}
    
    inline void swapWith (PlankSharedPtrContainer& other) throw()
    {
        internal.swapWith (other.internal);
    }

    inline Internal getInternal() throw()
    {
        return internal.getPtr();
    }

    inline Internal getInternal() const throw()
    {
        return internal.getPtr();
    }

    inline Internal operator->() throw()
	{
		return internal.getPtr();
	}
    
	inline const Internal operator->() const throw()
	{
		return internal.getPtr();
	}
    
	inline bool isNull() const throw()
	{
		return internal.getPtr() == getNullSharedPtr();
	}
	
	inline bool isNotNull() const throw()
	{
		return internal.getPtr() != getNullSharedPtr();
	}
    
    Weak getWeak() throw()
    {
        return Weak (pl_SharedPtr_GetWeakPtr (reinterpret_cast<PlankSharedPtrRef> (internal.getPtr())));
    }
    
protected:
    /** To use this you must be sure that the container object cannot be contended e.g., in a derived class constructor. */
    template<typename InitFunction>
    inline void initInternalWithFunction (InitFunction initFunction) throw()
	{
        plonk_assert (internal.getPtrUnchecked() == PlankSharedPtrContainer::getNullSharedPtr());
        Internal newInternal;
        initFunction (&newInternal);
        internal.getAtomicRef()->ptr = newInternal;
	}
    
    template<typename InitFunction, typename ArgType>
    inline void initInternalWithFunction (InitFunction initFunction, ArgType arg) throw()
	{
        plonk_assert (internal.getPtrUnchecked() == PlankSharedPtrContainer::getNullSharedPtr());
        Internal newInternal;
        initFunction (&newInternal, arg);
        internal.getAtomicRef()->ptr = newInternal;
	}

    
private:
    AtomicPointer internal;
};
    
//------------------------------------------------------------------------------

template<class PlankSharedPtrType>
class PlankWeakPtrContainer : public PlankSharedPtrContainer<PlankWeakPtrRef>
{
public:
    typedef PlankSharedPtrContainer<PlankWeakPtrRef> Base;
    typedef PlankSharedPtrContainer<PlankSharedPtrType> OriginalType;
    
    PlankWeakPtrContainer() throw()
    :   Base (Base::getNullSharedPtr())
    {
    }
    
    PlankWeakPtrContainer (PlankWeakPtrRef p) throw()
    :   Base (p)
    {
        if (p)
        {
            pl_WeakPtr_DecrementRefCount (p);
        }
    }
    
    PlankWeakPtrContainer (PlankWeakPtrContainer const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }

    PlankWeakPtrContainer& operator= (PlankWeakPtrContainer const& other) throw()
    {
        if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
    }
    
    OriginalType fromWeak() const throw()
    {
        PlankSharedPtrType sharedPtr = reinterpret_cast<PlankSharedPtrType> (pl_WeakPtr_GetSharedPtr (this->getInternal()));
        OriginalType result (sharedPtr);
        pl_SharedPtr_DecrementRefCount (reinterpret_cast<PlankSharedPtrRef> (sharedPtr));
        return result;
    }
    
};

//------------------------------------------------------------------------------

template<class PlankSharePtrContainterType>
class PlankSharedPtrArrayContainer : public PlankSharedPtrContainer<PlankSharedPtrArrayRef>
{
public:
    typedef PlankSharedPtrContainer<PlankSharedPtrArrayRef> Base;
    typedef PlankSharePtrContainterType                     ElementType;
    typedef typename ElementType::Internal                  ElementInternalType;
    typedef Base::Weak                                      Weak;

    PlankSharedPtrArrayContainer() throw()
    :   Base (Base::getNullSharedPtr())
    {
        initInternalWithFunction (pl_SharedPtrArray_CreateSharedPtr, 0);
    }
    
    PlankSharedPtrArrayContainer (FourCharCode const& elementType) throw()
    :   Base (Base::getNullSharedPtr())
    {
        initInternalWithFunction (pl_SharedPtrArray_CreateSharedPtr, elementType);
    }
    
    PlankSharedPtrArrayContainer (PlankSharedPtrArrayRef p) throw()
    :   Base (p)
    {
    }
    
    PlankSharedPtrArrayContainer (PlankSharedPtrArrayContainer const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    PlankSharedPtrArrayContainer (Base const& base) throw()
    :   Base (base)
    {
    }
    
    PlankSharedPtrArrayContainer (Weak const& weak) throw()
    :   Base (weak.fromWeak())
    {
    }
    
    PlankSharedPtrArrayContainer& operator= (PlankSharedPtrArrayContainer const& other) throw()
    {
        if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
    }
    
    void clear() throw()
    {
        Internal const internal = this->getInternal();
        
        if (internal != ElementType::getNullSharedPtr())
            pl_SharedPtrArray_Clear (internal);
    }
    
    Long length() const throw()
    {
        Internal const internal = this->getInternal();
        return internal ? pl_SharedPtrArray_GetLength (internal) : 0;
    }

    inline ElementType operator[] (const Long index) const throw()
    {
        Internal const internal = this->getInternal();
        plonk_assert (internal != Base::getNullSharedPtr());
        return internal ? ElementType (reinterpret_cast<ElementInternalType> (pl_SharedPtrArray_GetSharedPtr (internal, index))) : ElementType::getNull();
    }

    inline ElementType at (const Long index) const throw()
    {
        return at (index);
    }
    
    inline ElementType wrapAt (const Long index) const throw()
    {
        Internal const internal = this->getInternal();

        if (internal == Base::getNullSharedPtr())
            return ElementType::getNull();
        else plonk_assertfalse;
        
        UnsignedLong length = (unsigned long)this->length();
        
        if (length == 0)
            return ElementType::getNull();

        int indexToUse = index;
		while (indexToUse < 0)
			indexToUse += length;
		
        return ElementType (reinterpret_cast<ElementInternalType> (pl_SharedPtrArray_GetSharedPtr (internal,
                                                                                                   (unsigned long)indexToUse % length)));
        
		return this->getArray()[(unsigned long)indexToUse % (unsigned long)this->length()];
    }

    PlankSharedPtrArrayContainer& add (ElementType const& item) throw()
    {
        Internal const internal = this->getInternal();
        
        if (internal != Base::getNullSharedPtr())
            pl_SharedPtrArray_AddSharedPtr (internal, reinterpret_cast<PlankSharedPtrRef> (item.getInternal()));
        else plonk_assertfalse;
        
        return *this;
    }
    
    PlankSharedPtrArrayContainer& remove (const Long index) throw()
    {
        Internal const internal = this->getInternal();
        
        if (internal != Base::getNullSharedPtr())
            pl_SharedPtrArray_RemoveSharedPtr (internal, index);
        else plonk_assertfalse;
        
        return *this;
    }
    
    PlankSharedPtrArrayContainer& put (const Long index, ElementType const& item) throw()
    {
        Internal const internal = this->getInternal();
        
        if (internal != Base::getNullSharedPtr())
            pl_SharedPtrArray_PutSharedPtr (internal, index, static_cast<PlankSharedPtrRef> (item.getInternal()));
        else plonk_assertfalse;
        
        return *this;
    }

};


#define PLANKSHAREDPTRCONTAINER_DEFINE(NAME)\
    NAME NAME::getNull() throw() { static NAME null (Base::getNullSharedPtr()); return null; }\
    NAME::NAME() throw() : Base (Base::getNullSharedPtr()) { initInternalWithFunction (pl_##NAME##_CreateSharedPtr); }\
    NAME::NAME (Plank##NAME##Ref p) throw() : Base (p) { }\
    NAME::NAME (NAME const& copy) throw() : Base (static_cast<PlankSharedPtrContainer<Plank##NAME##Ref> const&> (copy)) { }\
    NAME::NAME (Base const& base) throw() : Base (base) { }\
    NAME::NAME (Weak const& weak) throw() : Base (weak.fromWeak()) { }\
    NAME& NAME::operator= (NAME const &other) throw() { if (this != &other) this->setInternal (other.getInternal()); return *this; }\
    Plank##NAME##Ref NAME::incrementRefCountAndGetPeer() throw() { return pl_##NAME##_IncrementRefCountAndGet (this->getInternal()); }


#endif // PLONK_SMARTPOINTERCONTAINER_H
