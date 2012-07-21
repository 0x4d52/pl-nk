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

#ifndef PLONK_ATOMICOBJECT_H
#define PLONK_ATOMICOBJECT_H

#define PLONK_ATOMICOBJECT_DEBUG 1

template<class Type> class AtomicObject;

#if PLONK_ATOMICOBJECT_DEBUG
static AtomicValue<int> atomicObjectCount;
#endif 

template<class Type>
class AtomicObjectWrapper
{
public:
    AtomicObjectWrapper (Type const& other) throw()
    :   counter (0),
        object (other)
    {
#if PLONK_ATOMICOBJECT_DEBUG
        ++atomicObjectCount;
        printf ("AtomicObject: atomicObjectCount = %d [thread=%lx]\n", atomicObjectCount.getValueUnchecked(), Threading::getCurrentThreadID());
#endif
    }
    
    ~AtomicObjectWrapper()
    {
#if PLONK_ATOMICOBJECT_DEBUG
        --atomicObjectCount;
        printf ("AtomicObject: atomicObjectCount = %d [thread=%lx]\n", atomicObjectCount.getValueUnchecked(), Threading::getCurrentThreadID());
#endif
    }
    
    friend class AtomicObject<Type>;
            
private:
    AtomicValue<int> counter;
    Type object;        
};

/** Store and retrieve an object atomically.
 This stores an object using a reference counted wrapper. The object must implement
 a getNull() function and should implement operator-> (in order for this operator
 to be usable on the AtomicObject). Subclasses of SmartPointerContainer meet both
 of these requirements and work well with this class.
 
 It is important to note that the storage and retrieval of the object is atomic
 (i.e., threadsafe) but other operations on the object may not be threadsafe.
 This is especially true of subclasses of SmartPointerContainer in Plonk.
 
 For example you could store a FloatArray in an AtomicObject<FloatArray>, access
 items in the array, get the size of the array. But adding items to or removing
 items from the array would not be threadsafe. Instead your algorithm would need 
 to use only one thread to modify such objects but new items could be received from 
 other threads via the AtomicObject<FloatArray> and addded on the designated thread. */
template<class Type>
class AtomicObject
{
public: 
    typedef AtomicObjectWrapper<Type>       Wrapper;
    typedef AtomicExtended<Wrapper*>        AtomicWrapperPointer;

    inline AtomicObject() throw()
    {
        this->init (Type::getNull());
    }
    
    inline AtomicObject (Type const& object) throw()
    {
        this->init (object);
    }
    
    inline ~AtomicObject()
    {
        AtomicWrapperPointer temp (getNullWrapper());
        atom.swapWith (temp);
        decrementRefCount (temp);
    }
    
    inline AtomicObject (AtomicObject const& copy) throw()
    {
        Wrapper* const wrapper (incrementRefCountAndGetPtr (copy.atom));
        AtomicWrapperPointer temp (wrapper);
        atom.swapWith (temp);
    }
    
    inline AtomicObject& operator= (AtomicObject const& other) throw()
    {
        AtomicObject temp (other);
        atom.swapWith (temp.atom);
        return *this;
    }
    
    inline AtomicObject& operator= (Type const& object) throw()
    {
        this->set (object);
        return *this;
    }
    
    inline void set (Type const& object) throw()
    {
        AtomicObject temp (object);
        atom.swapWith (temp.atom);
    }
    
    inline Type get() const throw()
    {
        AtomicObject temp (*this);
        return temp.atom->object;
    }
    
    inline Type operator->() const throw()
    {
        return this->get();
    }
    
    inline Type operator->() throw()
    {
        return this->get();
    }
    
    inline Type operator*() const throw()
    {
        return this->get();
    }
    
    inline Type operator*() throw()
    {
        return this->get();
    }
    
    bool isNull() const throw()
	{
		return atom.getPtrUnchecked() == 0;
	}
	
	bool isNotNull() const throw()
	{
		return atom.getPtrUnchecked() != 0;
	}
    
    bool operator== (AtomicObject const& other) const throw()
	{
        Type temp1 (atom->object);
        Type temp2 (other.atom->object);
		return temp1 == temp2;
	}
	
	bool operator!= (AtomicObject const& other) const throw()
	{
        Type temp1 (atom->object);
        Type temp2 (other.atom->object);
		return temp1 != temp2;
	}
    
    void swapWith (AtomicObject const& other) throw()
    {
        atom.swapWith (other.atom);
    }

    static AtomicObject& getNull() throw()
    {
        static AtomicObject null;
        return null;
    }
    
private:
    AtomicWrapperPointer atom; 
    
    inline AtomicObject (Wrapper* const wrapper) throw()
    {
        init (wrapper);
    }
    
    inline void init (Type const& object) throw()
    {
        init (new Wrapper (object));
    }

    inline void init (Wrapper* const wrapper) throw()
    {
        AtomicWrapperPointer temp (wrapper);
        incrementRefCount (temp);
        atom.swapWith (temp);
    }
    
    static inline Wrapper* getNullWrapper() throw()
    {
        return static_cast<Wrapper*> (0);
    }
    
    static inline void incrementRefCount (AtomicWrapperPointer& atom) throw()
    {
        int counter;
        Wrapper* const wrapper (atom.getPtrUnchecked());
        
        if (wrapper != getNullWrapper()) 
            counter = ++wrapper->counter;
        
#if PLONK_ATOMICOBJECT_DEBUG
        printf ("AtomicObject:  ++%p->counter = %d [extra=%ld] [thread=%lx]\n", 
                wrapper, counter, atom.getExtraUnchecked(), Threading::getCurrentThreadID());
#else
        (void)counter;
#endif
    }
    
    static inline void decrementRefCount (AtomicWrapperPointer& atom) throw()
    {
        int counter;
        Wrapper* const wrapper (atom.getPtrUnchecked());
        
        if ((wrapper != getNullWrapper()) && !(counter = --wrapper->counter))
            delete wrapper;
        
#if PLONK_ATOMICOBJECT_DEBUG
        printf ("AtomicObject:  --%p->counter = %d [extra=%ld] [thread=%lx]\n", 
                wrapper, counter, atom.getExtraUnchecked(), Threading::getCurrentThreadID());
#else
        (void)counter;
#endif
    }
    
    static inline Wrapper* incrementRefCountAndGetPtr (AtomicWrapperPointer const& atom) throw()
    {
        int counter;
        Wrapper* wrapper = getNullWrapper();
        
        do 
        {
            do 
            {
                wrapper = atom.getPtrUnchecked();
                
                if (wrapper == getNullWrapper())
                    goto exit;
                
                counter = wrapper->counter.getValueUnchecked();
                
            } while (wrapper != atom.getPtrUnchecked());
            
        } while ((wrapper != getNullWrapper()) && 
                 !wrapper->counter.compareAndSwap (counter, counter + 1));
        
#if PLONK_ATOMICOBJECT_DEBUG
        printf ("AtomicObject: *++%p->counter = %d [extra=%ld] [thread=%lx]\n", 
                wrapper, counter + 1, atom.getExtraUnchecked(), Threading::getCurrentThreadID());
#endif
        
    exit:
        return wrapper;
    }
};

template<class Type>
void swap (AtomicObject<Type> const& a, 
           AtomicObject<Type> const& b) throw()
{
    a.swapWith (b);
}


#undef PLONK_ATOMICOBJECT_DEBUG

#endif // PLONK_ATOMICOBJECT_H
