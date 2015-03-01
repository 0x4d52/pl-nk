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

#ifndef PLONK_OBJECTARRAYINTERNAL_H
#define PLONK_OBJECTARRAYINTERNAL_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
//#include "../core/plonk_Sender.h"
#include "../maths/plonk_InlineCommonOps.h"
//#include "plonk_DynamicContainer.h"


template<class ObjectType, class BaseType>
class ObjectArrayInternalBase :  public BaseType
{
public:	
    typedef ObjectArray<ObjectType> Container;
    
	ObjectArrayInternalBase (const int size, const bool isNullTerminated) throw();
	ObjectArrayInternalBase (const int size, ObjectType *dataToUse, const bool isNullTerminated, const bool shouldTakeOwnership) throw();
	~ObjectArrayInternalBase();
	
	PLONK_INLINE_LOW int size() const throw() { return sizeUsed; }
    PLONK_INLINE_LOW int sizeAllocated() const throw() { return allocatedSize; }
	PLONK_INLINE_LOW int length() const throw() 
	{ 
		if (sizeUsed == 0) 
			return 0;
		else
			return arrayIsNullTerminated ? sizeUsed - 1 : sizeUsed; 
	}
	
	PLONK_INLINE_LOW ObjectType* getArray() throw() { return array; }
	PLONK_INLINE_LOW const ObjectType* getArray() const throw() { return array; }
	PLONK_INLINE_LOW bool isNullTerminated() const throw() { return arrayIsNullTerminated; }
	PLONK_INLINE_LOW void setNullTerminated (const bool state) throw() { arrayIsNullTerminated = state; } 
	
    bool setSize (const int newSize, const bool keepContents) throw();
    void add (ObjectType const& item) throw();
    void add (const int numItems, const ObjectType* items) throw();
    void insert (const int index, const int numItems, const ObjectType* items) throw();
    void remove (const int index) throw();
    int indexOf (ObjectType const& itemToSearchFor, const int startIndex = 0) const throw();
	void referTo (const int size, ObjectType *dataToUse, const bool needsNullTermination = false) throw();
    void clear() throw();
        
private:
	int allocatedSize, sizeUsed;
	ObjectType *array;
	bool arrayIsNullTerminated : 1;
	bool ownsTheData : 1;
    
    void setSizeInternal (const int newSize, const bool keepContents) throw();    
    bool setSizeIfNeeded (const int newSize, const bool keepContents) throw();
    
	ObjectArrayInternalBase();
	ObjectArrayInternalBase (const ObjectArrayInternalBase&);
	const ObjectArrayInternalBase& operator= (const ObjectArrayInternalBase&);
};


//------------------------------------------------------------------------------



template<class ObjectType, class BaseType>
ObjectArrayInternalBase<ObjectType,BaseType>
::ObjectArrayInternalBase (const int initSize, 
                           const bool isNullTerminated) throw()
:	allocatedSize (initSize <= 0 ? 0 : initSize), 
    sizeUsed (allocatedSize),
    array (allocatedSize == 0 ? 0 : ArrayAllocator<ObjectType>::allocate (allocatedSize)),
    arrayIsNullTerminated (isNullTerminated),
    ownsTheData (true)
{
}

template<class ObjectType, class BaseType>
ObjectArrayInternalBase<ObjectType,BaseType>
::ObjectArrayInternalBase (const int initSize, 
                           ObjectType *dataToUse, 
                           const bool isNullTerminated,
                           const bool shouldTakeOwnership) throw()
:	allocatedSize (shouldTakeOwnership ? initSize : 0),
    sizeUsed (initSize),
    array (dataToUse),
    arrayIsNullTerminated (isNullTerminated),
    ownsTheData (shouldTakeOwnership)
{
}


template<class ObjectType, class BaseType>
ObjectArrayInternalBase<ObjectType,BaseType>
::~ObjectArrayInternalBase()
{
    if (ownsTheData) 
        ArrayAllocator<ObjectType>::free (array);
}


template<class ObjectType, class BaseType>
bool ObjectArrayInternalBase<ObjectType,BaseType>
::setSize (const int newSize, const bool keepContents) throw()
{
    const bool didResize = setSizeIfNeeded (newSize, keepContents);
    return didResize;
}

template<class ObjectType, class BaseType>
bool ObjectArrayInternalBase<ObjectType,BaseType>
::setSizeIfNeeded (const int newSize, const bool keepContents) throw()
{
    if (newSize != sizeUsed)
    {
        sizeUsed = newSize;
        
        if (newSize > allocatedSize)
            setSizeInternal (newSize, keepContents);
        
        const int newLength = this->length();
        
        if (allocatedSize > newLength)
        {   
            // null items beyond the used size and add null termination if needed..
            // could avoid this for simple numerical types 
            ObjectType null = TypeUtility<ObjectType>::getNull();// ObjectType();
            
            for (int i = newLength; i < allocatedSize; ++i)
                array[i] = null;
        }
        
        return true;
    }
    
    return false;
}


template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::setSizeInternal (const int newSize, const bool keepContents) throw()
{    
    plonk_assert (newSize >= 0);
    plonk_assert (ownsTheData);
    
    ObjectType* oldArray = array;
    
    if (newSize <= 0)
    {
        allocatedSize = 0;
        array = 0;
    }
    else
    {        
        //const int newSizeWithMargin = (newSize + 7) & ~7; // round to chunks of 8
        //..would do this for simple numerical types only.        
        ObjectType *newArray = ArrayAllocator<ObjectType>::allocate (newSize);
        
        if (keepContents)
        {
            const int overlapSize = plonk::min (allocatedSize, newSize);
            
            int i;
            
            // copy existing contents
            for (i = 0; i < overlapSize; ++i)
                newArray[i] = array[i];
            
            // and clear the rest
            if (newSize > overlapSize)
            {
                ObjectType null = TypeUtility<ObjectType>::getNull();// ObjectType();
            
                for (i = overlapSize; i < newSize; ++i)
                    newArray[i] = null;
            }
        }
        
        allocatedSize = newSize;
        array = newArray;   
    }
    
    ArrayAllocator<ObjectType>::free (oldArray);
}


template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::add (ObjectType const& item) throw()
{
    plonk_assert (ownsTheData);
    
    setSizeIfNeeded (sizeUsed + 1, true);
    array[this->length() - 1] = item;
}


template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::add (const int numItems, const ObjectType* items) throw()
{
    plonk_assert (ownsTheData);
    plonk_assert (numItems > 0);
    
    const int originalLength = this->length();    
    const int newSize = sizeUsed + numItems;
    
    setSizeIfNeeded (newSize, true);
    
    for (int i = originalLength; i < newSize; ++i)
        array[i] = *items++;
}

template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::insert (const int index, const int numItems, const ObjectType* items) throw()
{
    plonk_assert (ownsTheData);
    plonk_assert (numItems > 0);
    
    const int originalLength = this->length();
    plonk_assert (index <= originalLength);
    
    const int newSize = sizeUsed + numItems;
    
    setSizeIfNeeded (newSize, true);

    for (int i = originalLength; --i >= index;)
        array[i + numItems]  = array[i];
        
    for (int i = 0; i < numItems; ++i)
        array[index + i] = *items++;
}

template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::remove (const int index) throw()
{		
    plonk_assert (ownsTheData);
    
    if (index < 0 || index >= sizeUsed) 
        return;
    
    if (allocatedSize == 1)
    {        
        sizeUsed = 0;
    }
    else
    {
        const int newSize = sizeUsed - 1;
        
        for (int i = index; i < newSize; ++i)
            array[i] = array[i + 1];
        
        ObjectType null = TypeUtility<ObjectType>::getNull();// ObjectType();
        array[newSize] = null;//ObjectType();
        
        sizeUsed = newSize;
    }    
}

template<class ObjectType, class BaseType>
int ObjectArrayInternalBase<ObjectType,BaseType>
::indexOf (ObjectType const& itemToSearchFor, 
           const int startIndex) const throw()
{
    if (array != 0)
    {
        for (int i = startIndex < 0 ? 0 : startIndex; i < sizeUsed; ++i)
            if (array[i] == itemToSearchFor)
                return i;
    }		
    
    return -1;
}

template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::referTo (const int newSize, 
           ObjectType *dataToUse, 
           const bool needsNullTermination) throw()
{
    bool needsUpdate = false;
    
    if (dataToUse == array)
    {
        if (newSize != sizeUsed)
        {
            sizeUsed = newSize;
            needsUpdate = true;
        }
        
        if (arrayIsNullTerminated != needsNullTermination)
        {
            arrayIsNullTerminated = needsNullTermination;
            needsUpdate = true;
            ObjectType null = TypeUtility<ObjectType>::getNull();// ObjectType();
            array[this->length()] = null;//ObjectType();
        }
    }
    else 
    {
        if (ownsTheData)
            ArrayAllocator<ObjectType>::free (array);
        
        allocatedSize = 0;
        sizeUsed = newSize;
        array = dataToUse;
        
        arrayIsNullTerminated = needsNullTermination;
        ownsTheData = false;
        
        needsUpdate = true;
        
        if (array)
        {
            ObjectType null = TypeUtility<ObjectType>::getNull();// ObjectType();
            array[this->length()] = null;//ObjectType();
        }
    }
}

template<class ObjectType, class BaseType>
void ObjectArrayInternalBase<ObjectType,BaseType>
::clear() throw()
{
    if (allocatedSize == 0)
        return;
        
    if (ownsTheData)
        ArrayAllocator<ObjectType>::free (array);
    
    allocatedSize = 0;
    sizeUsed = 0;
    array = 0;    
    ownsTheData = true;
}

template<class ObjectType, class BaseType>
class ObjectArrayInternal : public ObjectArrayInternalBase<ObjectType,BaseType>
{
public:
    typedef ObjectArrayInternalBase<ObjectType,BaseType> ObjectArrayInternalBaseType;
    
    ObjectArrayInternal (const int size, const bool isNullTerminated) throw()
    :   ObjectArrayInternalBaseType (size, isNullTerminated)
    {
    }
    
	ObjectArrayInternal (const int size, ObjectType *dataToUse, const bool isNullTerminated, const bool shouldTakeOwnership) throw()
    :   ObjectArrayInternalBaseType (size, dataToUse, isNullTerminated, shouldTakeOwnership)
    {
    }

    SmartPointer* deepCopy() const throw()
    {
        const int size = this->size();
        ObjectArrayInternal *theCopy = new ObjectArrayInternal (size, this->isNullTerminated());
        const ObjectType* const src = this->getArray();
        ObjectType* const dst = theCopy->getArray();
        Memory::copy(dst, src, size * sizeof (ObjectType));
        return theCopy;
    }
    
};

#endif // PLONK_OBJECTARRAYINTERNAL_H
