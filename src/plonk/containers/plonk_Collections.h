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

#ifndef PLONK_COLLECTIONS_H
#define PLONK_COLLECTIONS_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "../core/plonk_Thread.h"
#include "plonk_ObjectArray.h"
#include "plonk_SimpleArray.h"


template<class ValueType, class KeyType>
class DictionaryInternal : public SmartPointer
{
public:
    typedef Dictionary<ValueType,KeyType> Container;
    
	DictionaryInternal() throw()
	{
	}
	
    DictionaryInternal (const int initialCapacity) throw()
    :   values (ObjectArray<ValueType>::emptyWithAllocatedSize (initialCapacity)),
        keys (ObjectArray<KeyType>::emptyWithAllocatedSize (initialCapacity))
	{
	}
    
	~DictionaryInternal()
	{
	}
	
	ObjectArray<ValueType>& getValues() throw()
	{
		return values;
	}
    	
	ObjectArray<KeyType>& getKeys() throw()
	{
		return keys;
	}
	
	const ObjectArray<ValueType>& getValues() const throw()
	{
		return values;
	}
	
	const ObjectArray<KeyType>& getKeys() const throw()
	{
		return keys;
	}
	
private:
	ObjectArray<ValueType> values;
	ObjectArray<KeyType> keys;
};


//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType, class KeyType>
class KeyValuePair
{
public:
	KeyValuePair() throw()
	{
	}
	
	KeyValuePair (KeyType const& key, ValueType const& value) throw()
	:	v (value), 
        k (key)
	{
	}
    
    KeyValuePair (KeyValuePair const& copy) throw()
    :   v (copy.v),
        k (copy.k)
    {
    }
    
    KeyValuePair operator= (KeyValuePair const& other) throw()
    {
        if (this != &other)
        {
            v = other.v;
            k = other.k;
        }
        
        return *this;
    }
	
	const KeyType& getKey() const throw() { return k; }
	const ValueType& getValue() const throw() { return v; }
	
private:
	ValueType v;
	KeyType k;
};


//------------------------------------------------------------------------------


/** A dictionary class for storing key/value pairs.
 Similar to std::map. Holds objects against a key. Items are stored in an array 
 and accessed via their key. By default the key is a Text string but can be any 
 appropriate type. 
 @ingroup PlonkContainerClasses */
template<class ValueType, class KeyType>
class Dictionary : public SmartPointerContainer< DictionaryInternal<ValueType,KeyType> >
{
public:
	typedef KeyValuePair<ValueType,KeyType>         KeyValuePairType;
	typedef ObjectArray<KeyValuePairType>           KeyValuePairArrayType;

    typedef DictionaryInternal<ValueType,KeyType>   Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<Dictionary>        Weak;    
    
	/** Creates an empty dictionary. */
	Dictionary() throw()
	:	Base (new Internal())
	{
	}
	
    explicit Dictionary (const int initialCapacity) throw()
	:	Base (new Internal (initialCapacity))
	{
	}
    
    explicit Dictionary (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	} 
    
    Dictionary (Dictionary const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}        
    
    Dictionary (Dynamic const& other) throw()
    :   Base (other.as<Dictionary>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    Dictionary& operator= (Dictionary const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static Dictionary fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    

    static const Dictionary& getNull() throw()
	{
		static Dictionary null;
		return null;
	}	                                
    
	/** Creates a dictionary initialise with an array of key/value pairs. */
	Dictionary (KeyValuePairArrayType const& pairs) throw()
	:	Base (new Internal())
	{
		put (pairs);
	}
    
	/** Returns the array of values. */
	const ObjectArray<ValueType>& getValues() const throw()
	{ 
		return this->getInternal()->getValues(); 
	}
	
	/** Returns the array of keys. */
	const ObjectArray<KeyType>& getKeys() const throw()
	{
		return this->getInternal()->getKeys(); 
	}
	
	/** Returns an array of the key/value pairs. */
	KeyValuePairArrayType getPairs() const throw()
	{
		KeyValuePairArrayType pairs = KeyValuePairArrayType::withSize (length());
		
		for (int i = 0; i < length(); ++i)
			pairs[i] = KeyValuePairType (key (i), value (i));
		
		return pairs;
	}
	
	/** Put an item into the dicionary associated with the specified key. 
	 If an item is already stored agains that key the old value is returned.
	 Otherwise a "null" version of the value is returned. In many case you will
	 not need to use the return value. */
	ValueType put (KeyType const& key, ValueType const& value) throw()
	{
		ObjectArray<ValueType>& values = this->getInternal()->getValues();
		ObjectArray<KeyType>& keys = this->getInternal()->getKeys();

		int index = keys.indexOf (key);
		
		if (index >= 0)
		{
			ValueType oldValue = values[index];
			values.put (index, value);
			return oldValue;
		}
		else
		{
			keys.add (key);
			values.add (value);
			return ObjectArray<ValueType>::getNullObject();
		}
	}
	
	ValueType put (KeyValuePairType const& pair) throw()
	{
		return put (pair.getKey(), pair.getValue());
	}
	
	void put (KeyValuePairArrayType const& pairs) throw()
	{
		for (int i = 0; i < pairs.length(); ++i)
			put (pairs[i]);
	}
	
    bool containsKey (KeyType const& key) throw()
	{
		return this->getInternal()->getKeys().contains (key);
	}

    bool containsValue (ValueType const& value) throw()
	{
		return this->getInternal()->getValues().contains (value);
	}
    
	/** Return a value at the specified key.
	 If the key is not found then a "null" version of the value is returned. */
	ValueType& at (KeyType const& key) throw()
	{
		ObjectArray<ValueType>& values = this->getInternal()->getValues();
		ObjectArray<KeyType>& keys = this->getInternal()->getKeys();
		
		const int index = keys.indexOf (key);
		return values[index];
	}
	
	/** Return a value at the specified key.
	 If the key is not found then a "null" version of the value is returned. */
	const ValueType& at (KeyType const& key) const throw()
	{
		ObjectArray<ValueType> const& values = getValues();
		ObjectArray<KeyType> const& keys = getKeys();
		
		const int index = keys.indexOf (key);
		return values[index];
	}
	
    ValueType& atIndex (const int index) throw()
	{
		ObjectArray<ValueType>& values = this->getInternal()->getValues();		
		return values[index];
	}    
    
    const ValueType& atIndex (const int index) const throw()
	{
		ObjectArray<ValueType> const& values = getValues();		
		return values[index];
	}
    
    ValueType& atIndexUnchecked (const int index) throw()
	{
		ObjectArray<ValueType>& values = this->getInternal()->getValues();		
		return values.atUnchecked (index);
	}    
    
    const ValueType& atIndexUnchecked (const int index) const throw()
	{
		ObjectArray<ValueType> const& values = getValues();		
		return values.atUnchecked (index);
	}
    
	/** Return a value at the specified key.
	 If the key is not found then a "null" version of the value is returned. */
	ValueType& operator[] (KeyType const& key) throw()
	{
		ObjectArray<ValueType>& values = this->getInternal()->getValues();
		ObjectArray<KeyType>& keys = this->getInternal()->getKeys();
		
		const int index = keys.indexOf (key);
		return values[index];
	}
	
	/** Return a value at the specified key.
	 If the key is not found then a "null" version of the value is returned. */
	const ValueType& operator[] (KeyType const& key) const throw()
	{
		ObjectArray<ValueType> const& values = getValues();
		ObjectArray<KeyType> const& keys = getKeys();
		
		const int index = keys.indexOf (key);
		return values[index];
	}
	
	/** Remove and return an item at the specified key.
	 If the key is not found then a "null" version of the value is returned. */
	ValueType remove (KeyType const& key) throw()
	{
		ObjectArray<ValueType>& values = this->getInternal()->getValues();
		ObjectArray<KeyType>& keys = this->getInternal()->getKeys();
		
		const int index = keys.indexOf (key);
		
		if (index >= 0)
		{
			ValueType removed = values[index];
			keys.remove (index);
			values.remove (index);
			return removed;
		}
		else
		{
			return ObjectArray<ValueType>::getNullObject();
		}
	}

	/** Return the key for a particular value. */
	const KeyType& keyForValue (ValueType const& value) const
	{
		ObjectArray<ValueType> const& values = getValues();
		ObjectArray<KeyType> const& keys = getKeys();
		
		const int index = values.indexOf (value);
		return keys[index];
	}
	
	/** Get a key at a particular index. */
	KeyType& key (const int index) throw()
	{
		return getKeys()[index];
	}
	
	/** Get a value at a particular index. */
	ValueType& value (const int index) throw()
	{
		return getValues()[index];
	}
	
	/** Get a key at a particular index. */
	const KeyType& key (const int index) const throw()
	{
		return getKeys()[index];
	}
	
	/** Get a value at a particular index. */
	const ValueType& value (const int index) const throw()
	{
		return getValues()[index];
	}
	
	/** Get the number of items stored in the dictionary. */
	int length() const throw()
	{
		ObjectArray<ValueType> const& values = getValues();
		
		plonk_assert (values.length() == getKeys().length()); // these should be the same length!
		
		return values.length();
	}
	
	/** Get the number of items stored in the dictionary. */
	int size() const throw()
	{
		ObjectArray<ValueType> const& values = getValues();
		ObjectArray<KeyType> const& keys = getKeys();
		
		plonk_assert (values.size() == keys.size()); // these should be the same size!
		
		return values.size();
	}
    
    PLONK_OBJECTARROWOPERATOR(Dictionary)

};

///-----------------------------------------------------------------------------

/** Thread-safe access to another container.
 The SmartPointerContainer and ScopedPointerContainer are thread-safe in terms
 of copying the object but not actions on the object. This class holds another
 container object and ensures thread-safe manipulations on the object. 
 The container must respond to the -> operator and implement copy() which
 should return a deep object of the object. */
template<class Container, int NumThreads>
class ThreadSafeAccess
{
public:
    typedef typename Container::Internal ContainerInternal;
    typedef ObjectArray<Container> Containers;
    
    ThreadSafeAccess() throw()
    :   containers (Containers::withSize (NumThreads))
    {
        clearThreads();
        threads[0] = Threading::getCurrentThreadID();
    }
    
    ~ThreadSafeAccess()
    {
    }
    
    explicit ThreadSafeAccess (Container const& initialValue) throw()
    :   containers (Containers::withSize (NumThreads))
    {        
        for (int i = 0; i < NumThreads; ++i)
            containers.atUnchecked (i) = initialValue.copy();
        
        clearThreads();
        threads[0] = Threading::getCurrentThreadID();
    }
    
    Container operator->() throw() 
    { 
        return containers[0];
    }
    
private:
    Containers containers;
    Threading::ID threads[NumThreads];
    
    void clearThreads() throw()
    {
        for (int i = 0; i < NumThreads; ++i)
            threads[i] = 0;
    }
};



#endif // PLONK_COLLECTIONS_H
