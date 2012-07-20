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


template<class Type> class AtomicObject;

static AtomicValue<int> atomicObjectCount;

template<class Type>
class AtomicObjectWrapper
{
public:
    AtomicObjectWrapper (Type const& other) throw()
    :   counter (0),
        object (other)
    {
        ++atomicObjectCount;
        printf ("AtomicObject: atomicObjectCount = %d\n", atomicObjectCount.getValueUnchecked());
    }
    
    ~AtomicObjectWrapper()
    {
        --atomicObjectCount;
        printf ("AtomicObject: atomicObjectCount = %d\n", atomicObjectCount.getValueUnchecked());
    }
    
    friend class AtomicObject<Type>;
            
private:
    AtomicValue<int> counter;
    Type object;        
};


template<class Type>
class AtomicObject
{
public: 
    typedef AtomicObjectWrapper<Type> Wrapper;

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
        AtomicValue<Wrapper*> temp (getNullWrapper());
        atom.swapWith (temp);
        
        this->decrementRefCount (temp);
    }
    
    inline AtomicObject (AtomicObject const& copy) throw()
    {
        AtomicValue<Wrapper*> temp (incrementRefCountAndGetPtr (const_cast<AtomicObject&> (copy).atom));
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
    
    inline Type operator-> () const throw()
    {
        return this->get();
    }
    
    inline Type operator-> () throw()
    {
        return this->get();
    }
    
    inline Type operator* () const throw()
    {
        return this->get();
    }
    
    inline Type operator* () throw()
    {
        return this->get();
    }
    
//    inline AtomicObject containerCopy() const throw()
//    {
//        return *this;
//    }

private:
    AtomicValue<Wrapper*> atom; 
    
    inline void init (Type const& object) throw()
    {
        AtomicValue<Wrapper*> temp (new Wrapper (object));
        this->incrementRefCount (temp);
        atom.swapWith (temp);
    }

    static inline Wrapper* getNullWrapper() throw()
    {
        return static_cast<Wrapper*> (0);
    }
    
    static inline void incrementRefCount (AtomicValue<Wrapper*>& atom) throw()
    {
        Wrapper* const wrapper (atom.getPtrUnchecked());
        int counter;
        
        if (wrapper != getNullWrapper()) 
            counter = ++wrapper->counter;
        
        printf ("AtomicObject: ++%p->counter = %d\n", wrapper, counter);
    }
    
    static inline void decrementRefCount (AtomicValue<Wrapper*>& atom) throw()
    {
        Wrapper* const wrapper (atom.getPtrUnchecked());
        int counter;
        
        if ((wrapper != getNullWrapper()) && ((counter = --wrapper->counter) == 0))
            delete wrapper;
        
        printf ("AtomicObject: ++%p->counter = %d\n", wrapper, counter);
    }
    
    static inline Wrapper* incrementRefCountAndGetPtr (AtomicValue<Wrapper*>& atom) throw()
    {
        int counter;
        Wrapper* wrapper;
        
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
                 (wrapper->counter.compareAndSwap (counter, counter + 1) == false));
        
        printf ("AtomicObject: ++%p->counter = %d\n", wrapper, counter + 1);
        
    exit:
        return wrapper;
    }
};

#endif // PLONK_ATOMICOBJECT_H
