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

#ifndef PLONK_OBJECTARRAY_H
#define PLONK_OBJECTARRAY_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointerContainer.h"
#include "../core/plonk_WeakPointerContainer.h"
#include "../core/plonk_SenderContainer.h"
#include "../maths/plonk_InlineCommonOps.h"
#include "../random/plonk_RNG.h"
#include "plonk_ObjectArrayInternal.h"



#define ObjectArrayConcatOperatorsDefine(Base, Type)\
	Base<Type> operator<< (Base<Type> const& other) const throw()\
	{\
		return Base<Type>(*this, other);\
	}\
    \
	Base<Type> operator<< (Type const& other) const throw()\
	{\
		return Base<Type>(*this, other);\
	}\
    \
	Base<Type> operator, (Base<Type> const& other) const throw()\
	{\
		return Base<Type>(*this, other);\
	}\
    \
	Base<Type> operator, (Type const& other) const throw()\
	{\
		return Base<Type>(*this, other);\
	}\
    \
	Base<Type>& operator<<= (Base<Type> const& other) throw()\
	{\
		return operator= (Base<Type>(*this, other));\
	}\
    \
	Base<Type>& operator<<= (Type const& other) throw()\
	{\
		return operator= (Base<Type>(*this, other));\
	}

#define ObjectArrayAssignmentDefinition(DerivedArrayType, ElementType)\
    DerivedArrayType& operator= (ObjectArray<ElementType> const& other) throw()\
    {\
        return operator= (static_cast<DerivedArrayType const&> (other) );\
    }


/** This is the base class for arrays.
 You can pass ObjectArray instances around relatively efficiently since copies
 simply refer to the same (reference counted) internal data.

 This is particulary useful for storing arrays of other reference counted objects
 It should work on all types. For numerical types it  may be more convenient 
 to use NumericalArray (which is a subclass of ObjectArray) since it includes 
 numerical operations on the arrays. Use Text for text strings.
 */
template<class ObjectType>
//class ObjectArray : public SenderContainer< ObjectArrayInternal<ObjectType> >
class ObjectArray : public SmartPointerContainer< ObjectArrayInternal<ObjectType> >
{
public:
    typedef ObjectArrayInternal<ObjectType>             Internal;
    typedef SmartPointerContainer<Internal>             Base;    
    typedef WeakPointerContainer<ObjectArray>           Weak;
    
    typedef ObjectArray<ObjectType>                     ObjectArrayType;
    typedef ObjectArray<ObjectArrayType>                ObjectArray2DType;
        

	class InitialObject
	{
	public:
		InitialObject() throw() : valid(false) { }
        
        template<class OtherType>
		InitialObject (OtherType const& other) throw() 
		:	object (other), valid (true) 
		{ 
		}
        		
		ObjectType object;
		const bool valid;
        
    private:
        InitialObject& operator= (InitialObject const&);
	};
	
	/** Creates an empty array. */
	ObjectArray() throw()
	:	Base (new Internal (0, false))
	{
	}
    
    explicit ObjectArray (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static ObjectArray fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }
    
    	
	/** Creates an array with a particular size/length.
	 @param size The total number of elements in the array.
	 @param needsNullTermination If true the last element will be used as a null terminator 
								 (e.g., for C strings but can be used for other purposes. */
	ObjectArray (const int size, const bool needsNullTermination) throw()
	:	Base (new Internal (size, needsNullTermination))
	{
	}
    
	/** Returns an array with a given size.
	 The array will NOT be a null terminated type. */
	static ObjectArray<ObjectType> withSize (const int size) throw()
	{
		return ObjectArray<ObjectType> (size, false);
	}	
    
    /** Returns an empty array with a given size of allocated storage.
     This will make expanding this array's size upto this allocated size more
     efficient than bulding it up from a zero-size array.
	 The array will NOT be a null terminated type. */
	static ObjectArray<ObjectType> emptyWithAllocatedSize (const int size) throw()
	{
		ObjectArray<ObjectType> result = ObjectArray<ObjectType> (size, false);
        result.setSize (0, false);
        return result;
	}	    
	
	/** Wraps an existing array in an ObjectArray.
	 @warning This does NOT copy the data so take care that the array passed in
			  does not get deallocated before the ObjectArray that uses it. */
	ObjectArray (const int size, ObjectType *dataToUse, const bool needsNullTermination = false, const bool shouldTakeOwnership = false) throw()
	:	Base (new Internal (size, dataToUse, needsNullTermination, shouldTakeOwnership))
	{
	}
	    
	/** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    inline ObjectArray (ObjectArray const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    /** Assignment operator. */
    ObjectArray& operator= (ObjectArray const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    	
	/** Create a copy of another array.
	 You need to be sure that the other array type can be casted to the type of this array. */
	template<class CopyType>
	ObjectArray (ObjectArray<CopyType> const& copy) throw()
	:	Base (new Internal (copy.size(), copy.isNullTerminated()))
	{
		const int size = this->size();
        
		for (int i = 0; i < size; ++i)
			this->put (i, ObjectType (copy[i]));
	}
	
	/** Collect a new array based on the contents of another array.
	 Each element in turn is passed to the new object's constructor. */
	template<class CollectionType>
	static ObjectArray<ObjectType> collect (ObjectArray<CollectionType> const& collection) throw()
	{
		const int length = collection.length();
		ObjectArray<ObjectType> result (length, false);
		
		for (int i = 0; i < length; ++i)
			result.put (i, ObjectType (collection[i]));
		
		return result;
	}
	
	template<class CollectionType1, class CollectionType2>
	static ObjectArray<ObjectType> collect (ObjectArray<CollectionType1> const& collection1,
										    ObjectArray<CollectionType2> const& collection2) throw()
	{
		const int length = plonk::max (collection1.length(), collection2.length());
		ObjectArray<ObjectType> result (length, false);
		
		for (int i = 0; i < length; ++i)
			result.put (i, ObjectType (collection1.wrapAt (i), 
                                       collection2.wrapAt (i)));
		
		return result;
	}
	
	template<class CollectionType1, 
			 class CollectionType2, 
			 class CollectionType3>
	static ObjectArray<ObjectType> collect (ObjectArray<CollectionType1> const& collection1,
										    ObjectArray<CollectionType2> const& collection2,
										    ObjectArray<CollectionType3> const& collection3) throw()
	{
		const int length = plonk::max (collection1.length(), 
                                       collection2.length(), 
                                       collection3.length());
		
		ObjectArray<ObjectType> result (length, false);
		
		for (int i = 0; i < length; ++i)
			result.put (i, ObjectType (collection1.wrapAt (i), 
									   collection2.wrapAt (i),
									   collection3.wrapAt (i)));
		
		return result;
	}
	
	template<class CollectionType1, 
			 class CollectionType2, 
			 class CollectionType3, 
			 class CollectionType4>
	static ObjectArray<ObjectType> collect (ObjectArray<CollectionType1> const& collection1,
										    ObjectArray<CollectionType2> const& collection2,
										    ObjectArray<CollectionType3> const& collection3,
										    ObjectArray<CollectionType4> const& collection4) throw()
	{
		const int length = plonk::max (collection1.length(), 
                                       collection2.length(), 
                                       collection3.length(),
                                       collection4.length());
		
		ObjectArray<ObjectType> result (length, false);
		
		for (int i = 0; i < length; ++i)
			result.put (i, ObjectType (collection1.wrapAt (i), 
								 	   collection2.wrapAt (i),
									   collection3.wrapAt (i),
									   collection4.wrapAt (i)));
		
		return result;
	}
	
	/** Creates a deep copy of the array.
	 Note that items themselves are not deep-copied so reference counted
	 objects will still refer to the same objects. */
	ObjectArray<ObjectType> copy() const throw()
	{
		const int size = this->size();
		ObjectArray<ObjectType> newArray = ObjectArray<ObjectType> (size, this->isNullTerminated());
		ObjectType *newArrayPtr = newArray.getArray();
		const ObjectType *thisArrayPtr = this->getArray();
        
		for (int i = 0; i < size; ++i)
			newArrayPtr[i] = thisArrayPtr[i];
		
		return newArray;
	}
	
	/** Create an array with a single item. */
	ObjectArray (ObjectType const& single) throw()
	:	Base (new Internal (1, false))
	{
		this->put (0, single);
	}		
	
protected:
	static int countValidInitialisers (InitialObject const& i03) throw()
	{
		int size = 3;
		if (i03.valid) size++; else return size;
		return size;
	}	
	
public:
	ObjectArray (InitialObject const &i00,
				 InitialObject const &i01,
				 InitialObject const &i02,
				 InitialObject const &i03 = InitialObject()) throw()
	:	Base (new Internal (countValidInitialisers (i03), false))
	{
		ObjectType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.object; else return;
		if (i01.valid) thisArray[ 1] = i01.object; else return;
		if (i02.valid) thisArray[ 2] = i02.object; else return;
		if (i03.valid) thisArray[ 3] = i03.object; else return;
	}
	
protected:
	static int countValidInitialisers (InitialObject const& i05,
									   InitialObject const& i06,
									   InitialObject const& i07) throw()
	{
		int size = 5;
		
		if (i05.valid) size++; else return size;
		if (i06.valid) size++; else return size;
		if (i07.valid) size++; else return size;
		
		return size;
	}	
	
public:
	ObjectArray (InitialObject const &i00,
				 InitialObject const &i01,
				 InitialObject const &i02,
				 InitialObject const &i03,
				 InitialObject const &i04,
				 InitialObject const &i05 = InitialObject(),
				 InitialObject const &i06 = InitialObject(),
				 InitialObject const &i07 = InitialObject()) throw()
	:	Base (new Internal (countValidInitialisers (i05, i06, i07), false))
	{
		ObjectType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.object; else return;
		if (i01.valid) thisArray[ 1] = i01.object; else return;
		if (i02.valid) thisArray[ 2] = i02.object; else return;
		if (i03.valid) thisArray[ 3] = i03.object; else return;
		if (i04.valid) thisArray[ 4] = i04.object; else return;
		if (i05.valid) thisArray[ 5] = i05.object; else return;
		if (i06.valid) thisArray[ 6] = i06.object; else return;
		if (i07.valid) thisArray[ 7] = i07.object; else return;
	}
		
protected:
	static int countValidInitialisers (InitialObject const& i09,
									   InitialObject const& i10,
									   InitialObject const& i11,
									   InitialObject const& i12,
									   InitialObject const& i13,
									   InitialObject const& i14,
									   InitialObject const& i15) throw()
	{
		int size = 9;
		
		if (i09.valid) size++; else return size;
		if (i10.valid) size++; else return size;
		if (i11.valid) size++; else return size;
		if (i12.valid) size++; else return size;
		if (i13.valid) size++; else return size;
		if (i14.valid) size++; else return size;
		if (i15.valid) size++; else return size;
		
		return size;
	}	
	
public:
	ObjectArray (InitialObject const &i00,
			 	 InitialObject const &i01,
				 InitialObject const &i02,
				 InitialObject const &i03,
				 InitialObject const &i04,
				 InitialObject const &i05,
				 InitialObject const &i06,
				 InitialObject const &i07,
				 InitialObject const &i08,
				 InitialObject const &i09 = InitialObject(),
				 InitialObject const &i10 = InitialObject(),
				 InitialObject const &i11 = InitialObject(),
				 InitialObject const &i12 = InitialObject(),
				 InitialObject const &i13 = InitialObject(),
				 InitialObject const &i14 = InitialObject(),
				 InitialObject const &i15 = InitialObject()) throw()
	:	Base (new Internal (countValidInitialisers (i09, i10, i11, i12, i13, i14, i15), false))
	{
		ObjectType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.object; else return;
		if (i01.valid) thisArray[ 1] = i01.object; else return;
		if (i02.valid) thisArray[ 2] = i02.object; else return;
		if (i03.valid) thisArray[ 3] = i03.object; else return;
		if (i04.valid) thisArray[ 4] = i04.object; else return;
		if (i05.valid) thisArray[ 5] = i05.object; else return;
		if (i06.valid) thisArray[ 6] = i06.object; else return;
		if (i07.valid) thisArray[ 7] = i07.object; else return;
		if (i08.valid) thisArray[ 8] = i08.object; else return;
		if (i09.valid) thisArray[ 9] = i09.object; else return;
		if (i10.valid) thisArray[10] = i10.object; else return;
		if (i11.valid) thisArray[11] = i11.object; else return;
		if (i12.valid) thisArray[12] = i12.object; else return;
		if (i13.valid) thisArray[13] = i13.object; else return;
		if (i14.valid) thisArray[14] = i14.object; else return;
		if (i15.valid) thisArray[15] = i15.object; else return;
	}	
	
protected:
	static int countValidInitialisers (InitialObject const& i17,
									   InitialObject const& i18,
									   InitialObject const& i19,
									   InitialObject const& i20,
									   InitialObject const& i21,
									   InitialObject const& i22,
									   InitialObject const& i23,
									   InitialObject const& i24,
									   InitialObject const& i25,
									   InitialObject const& i26,
									   InitialObject const& i27,
									   InitialObject const& i28,
									   InitialObject const& i29,
									   InitialObject const& i30,
									   InitialObject const& i31) throw()
	{
		int size = 17;
		
		if (i17.valid) size++; else return size;
		if (i18.valid) size++; else return size;
		if (i19.valid) size++; else return size;
		if (i20.valid) size++; else return size;
		if (i21.valid) size++; else return size;
		if (i22.valid) size++; else return size;
		if (i23.valid) size++; else return size;
		if (i24.valid) size++; else return size;
		if (i25.valid) size++; else return size;
		if (i26.valid) size++; else return size;
		if (i27.valid) size++; else return size;
		if (i28.valid) size++; else return size;
		if (i29.valid) size++; else return size;
		if (i30.valid) size++; else return size;
		if (i31.valid) size++; else return size;
		
		return size;
	}	
	
public:
	ObjectArray (InitialObject const &i00,
				 InitialObject const &i01,
				 InitialObject const &i02,
				 InitialObject const &i03,
				 InitialObject const &i04,
				 InitialObject const &i05,
				 InitialObject const &i06,
				 InitialObject const &i07,
				 InitialObject const &i08,
				 InitialObject const &i09,
				 InitialObject const &i10,
				 InitialObject const &i11,
				 InitialObject const &i12,
				 InitialObject const &i13,
				 InitialObject const &i14,
				 InitialObject const &i15,
				 InitialObject const &i16,
				 InitialObject const &i17 = InitialObject(),
				 InitialObject const &i18 = InitialObject(),
				 InitialObject const &i19 = InitialObject(),
				 InitialObject const &i20 = InitialObject(),
				 InitialObject const &i21 = InitialObject(),
				 InitialObject const &i22 = InitialObject(),
				 InitialObject const &i23 = InitialObject(),
				 InitialObject const &i24 = InitialObject(),
				 InitialObject const &i25 = InitialObject(),
				 InitialObject const &i26 = InitialObject(),
				 InitialObject const &i27 = InitialObject(),
				 InitialObject const &i28 = InitialObject(),
				 InitialObject const &i29 = InitialObject(),
				 InitialObject const &i30 = InitialObject(),
				 InitialObject const &i31 = InitialObject()) throw()
	:	Base
            (new Internal(countValidInitialisers (     i17, i18, i19,
                                                  i20, i21, i22, i23,
                                                  i24, i25, i26, i27,
                                                  i28, i29, i30, i31), 
											 false))
	{
		ObjectType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.object; else return;
		if (i01.valid) thisArray[ 1] = i01.object; else return;
		if (i02.valid) thisArray[ 2] = i02.object; else return;
		if (i03.valid) thisArray[ 3] = i03.object; else return;
		if (i04.valid) thisArray[ 4] = i04.object; else return;
		if (i05.valid) thisArray[ 5] = i05.object; else return;
		if (i06.valid) thisArray[ 6] = i06.object; else return;
		if (i07.valid) thisArray[ 7] = i07.object; else return;
		if (i08.valid) thisArray[ 8] = i08.object; else return;
		if (i09.valid) thisArray[ 9] = i09.object; else return;
		if (i10.valid) thisArray[10] = i10.object; else return;
		if (i11.valid) thisArray[11] = i11.object; else return;
		if (i12.valid) thisArray[12] = i12.object; else return;
		if (i13.valid) thisArray[13] = i13.object; else return;
		if (i14.valid) thisArray[14] = i14.object; else return;
		if (i15.valid) thisArray[15] = i15.object; else return;
		if (i16.valid) thisArray[16] = i16.object; else return;
		if (i17.valid) thisArray[17] = i17.object; else return;
		if (i18.valid) thisArray[18] = i18.object; else return;
		if (i19.valid) thisArray[19] = i19.object; else return;
		if (i20.valid) thisArray[20] = i20.object; else return;
		if (i21.valid) thisArray[21] = i21.object; else return;
		if (i22.valid) thisArray[22] = i22.object; else return;
		if (i23.valid) thisArray[23] = i23.object; else return;
		if (i24.valid) thisArray[24] = i24.object; else return;
		if (i25.valid) thisArray[25] = i25.object; else return;
		if (i26.valid) thisArray[26] = i26.object; else return;
		if (i27.valid) thisArray[27] = i27.object; else return;
		if (i28.valid) thisArray[28] = i28.object; else return;
		if (i29.valid) thisArray[29] = i29.object; else return;
		if (i30.valid) thisArray[30] = i30.object; else return;
		if (i31.valid) thisArray[31] = i31.object; else return;
	}	
	
	
	/** Concatenate two arrays into one. */
	ObjectArray (ObjectArray<ObjectType> const& array0, 
			 	 ObjectArray<ObjectType> const& array1) throw()
	:	Base (new Internal (0, false))
	{
		const bool bothNullTerminated = array0.isNullTerminated() && array1.isNullTerminated();
		const bool eitherNullTerminated = array0.isNullTerminated() || array1.isNullTerminated();
		const int summedSizes = array0.size() + array1.size();
		const int newSize = bothNullTerminated ? summedSizes - 1 : summedSizes;
		
		if (newSize > 0)
		{
			this->setInternal (new Internal (newSize, eitherNullTerminated));
			
			int newIndex = 0;
			
			ObjectType *array = this->getArray();
			
			if (array != 0)
			{
				const ObjectType *arrayPtr0 = array0.getArray();
				const ObjectType *arrayPtr1 = array1.getArray();
				
                int i;
				for (i = 0; i < array0.length(); ++i) 
                {
					array[newIndex++] = arrayPtr0[i];
				}
								
				for (i = 0; i < array1.length(); ++i) 
                {
					array[newIndex++] = arrayPtr1[i];
				}
				
				if (eitherNullTerminated)
					array[newIndex] = getNullObject();
			}
		}
	}
    
    /** Sets a new size for the array. 
     If the contents are retained the overalapping size is copied and/or any 
     additionally allocated space is cleared. Otherwise the data is left
     unitialised.
     @return True the buffer was resized. */
    inline bool setSize (const int newSize, const bool keepContents) throw() 
    { 
        return this->getInternal()->setSize (newSize, keepContents);
    }

    /** Sets the length of the array to zero. */
    inline void clear() throw()
	{
		this->getInternal()->setSize (isNullTerminated() ? 1 : 0, false);
	}
            
	/** Size is the actual number of elements of storage needed for the array.
	 If this is a null terminated array the null value is inlcuded in the size. */
	inline int size() const throw() { return this->getInternal()->size(); }
    
    /** This is the memory allocated to the array.
     It includes memory reserved for future expansion. */
    inline int sizeAllocated() const throw() { return this->getInternal()->sizeAllocated(); }
	
	/** Length is the number of real elements of storage available.
	 For arrays that are NOT null terminated this is synonymous with size().
	 If this is a null terminated array the length is one less than the size. */
	inline int length() const throw() { return this->getInternal()->length(); }
	
	/** Actual memory requirements of the array elements and the null terminator (if needed).
	 Equivalent to size() * sizeof(ObjectType). */
	inline int memorySize() const throw() { return this->getInternal()->size() * sizeof(ObjectType); }
	
	/** Returns a pointer to the raw array. */
	inline ObjectType* getArray() throw() { return this->getInternal()->getArray(); }
	
	/** Returns a pointer to the raw array for read-only ops. */
	inline const ObjectType* getArray() const throw() { return this->getInternal()->getArray(); }
	
	/** Returns a pointer to the raw array for read-only ops. */
	inline operator const ObjectType*() const throw() { return this->getArray(); }
	/** Returns a pointer to the raw array. */
	inline operator ObjectType*() throw() { return this->getArray(); }
	
	/** Returns whether this is a null terminated array or not. */
	inline bool isNullTerminated() const throw() { return this->getInternal()->isNullTerminated(); }
	
	/** Changes this array's null terminator flag. */
	inline void setNullTerminated (const bool state) throw() { this->getInternal()->setNullTerminated(state); }
	
	ObjectArrayConcatOperatorsDefine(ObjectArray, ObjectType);
    
    /** Force the internal array to refer to other data.
     This will deallocate internally allocated memory and use the data provided.
     Take care to ensure that the data to which this array refers exists until
     this ObjectArray has been deleted and that the size argument is correct. */
    void referTo (const int size, ObjectType *dataToUse, const bool needsNullTermination = false)
    {
        this->getInternal()->referTo (size, dataToUse, needsNullTermination);
    }
	
	/** Adds an item in-place. */
	ObjectArray<ObjectType>& add (ObjectType const& item) throw()
	{ 
		this->getInternal()->add (item); 
		return *this;
	}
	
	/** Adds several items in-place. */
	ObjectArray<ObjectType>& add (ObjectArray<ObjectType> const& other) throw() // had changed to addAll but caused other errors. add is nice polymorphism!
	{
        this->getInternal()->add (other.length(), other.getArray()); 
		return *this;
	}
	
	/** Removes an item at the given index in-place. 
	 Indices out of range will be ignored.*/
	ObjectArray<ObjectType>& remove (const int index) throw()
	{ 
		this->getInternal()->remove (index); 
		return *this;
	}
	
	/** Places an item at the given index. 
	 Indices out of range will be ignored. */
	ObjectArray<ObjectType>& put (const int index, ObjectType const& item) throw()
	{
		if (index < 0 || index >= this->size()) 
			return *this;
		
		this->getArray()[index] = item;
        
		return *this;
	}
	
	/** Returns a reference to an item at the specified index. 
	 If you are not sure about the index being in range check for the object being "null" as a
	 separate internal null value may have been returned instead. */
    ObjectType& operator[] (const int index) throw()
	{
		if (index < 0 || index >= this->size()) 
			return getNullObject();
		else
			return this->getArray()[index];
	}
	
	/** Returns a reference to an item at the specified index. 
	 If you are not sure about the index being in range check for the object being "null" as a
	 separate internal null value may have been returned instead. */	
    const ObjectType& operator[] (const int index) const throw()
	{
		if (index < 0 || index >= this->size()) 
			return getNullObject();
		else
			return this->getArray()[index];
	}
	
	/** Returns a reference to an item at the specified index. 
	 If you are not sure about the index being in range check for the object being "null" as a
	 separate internal null value may have been returned instead. */	
	ObjectType& at (const int index) throw()
	{
		if (index < 0 || index >= this->size()) 
			return getNullObject();
		else
			return this->getArray()[index];
	}
	
	/** Returns a reference to an item at the specified index. 
	 If you are not sure about the index being in range check for the object being "null" as a
	 separate internal null value may have been returned instead. */	
	const ObjectType& at (const int index) const throw()
	{
		if (index < 0 || index >= this->size()) 
			return getNullObject();
		else
			return this->getArray()[index];
	}
	
	/** Returns a new array by iterating through an array of indices provided.  */
	ObjectArray<ObjectType> at (ObjectArray<int> const& indices) const throw()
	{
		if (this->getInternal()->size() == 0) { return *this; }
			
		const bool needsNull = this->isNullTerminated();
		const int length = indices.length();
		const int size = needsNull ? length + 1 : length;
		
		ObjectArray<ObjectType> result (size, needsNull);
		ObjectType* resultArray = result.getArray();
		const ObjectType* thisArray = this->getArray();
		
		const int thisSize = this->size();
		for (int i = 0; i < length; ++i)
		{
			const int index = indices[i];
			resultArray[i] = index < 0 || index >= thisSize ? getNullObject() : thisArray[index];
		}
		
		if (needsNull)
			resultArray[length] = getNullObject();
		
		return result;
	}

	/** Returns a new array by iterating through an array of indices provided.  */
	ObjectArray<ObjectType> atUnchecked (ObjectArray<int> const& indices) const throw()
	{				
		const bool needsNull = this->isNullTerminated();
		const int length = indices.length();
		const int size = needsNull ? length + 1 : length;
		
		ObjectArray<ObjectType> result(size, needsNull);
		ObjectType* resultArray = result.getArray();
		const ObjectType* thisArray = this->getArray();
		
		for (int i = 0; i < length; ++i)
		{
			const int index = indices[i];
			resultArray[i] = thisArray[index];
		}
		
		if (needsNull)
			resultArray[length] = getNullObject();
		
		return result;
	}
	
	
	/** Returns a reference to an itemn at the specified index. 
	 You must make sure that the array index is in range when using this version. */		
	inline ObjectType& atUnchecked (const int index) throw()
	{
		plonk_assert ((index >= 0) && (index < this->getInternal()->size()));
		return this->getInternal()->getArray()[index];
	}

	/** Returns a refernce to an itemn at the specified index. 
	 You must make sure that the array index is in range when using this version. */		
	inline const ObjectType& atUnchecked (const int index) const throw()
	{
		plonk_assert ((index >= 0) && (index < this->getInternal()->size()));
		return this->getInternal()->getArray()[index];
	}	
	
	/** Returns a reference to an item at the specified index. 
	 If the index is out of range it will be wrapped in-range. For example, if
	 the array size is 10 and the index requested is 12, wrapAt() will return
	 the item at index 2, if the index is -1 the item returned will be at index 9 and so on. */		
	ObjectType& wrapAt (const int index) throw()
	{
		if (this->getInternal()->size() == 0) { return getNullObject(); }
		
		int indexToUse = index;
		while (indexToUse < 0)
			indexToUse += this->size();
		
		return this->getArray()[(unsigned int)indexToUse % (unsigned int)this->size()];
	}
	
	/** Returns a reference to an item at the specified index. 
	 If the index is out of range it will be wrapped in-range. For example, if
	 the array size is 10 and the index requested is 12, wrapAt() will return
	 the item at index 2, if the index is -1 the item returned will be at index 9 and so on. */			
	const ObjectType& wrapAt (const int index) const throw()
	{
		if (this->getInternal()->size() == 0) { return getNullObject(); }
		
		int indexToUse = index;
		while (indexToUse < 0)
			indexToUse += this->size();
		
		return this->getArray()[(unsigned int)indexToUse % (unsigned int)this->size()];
	}
		
	/** Returns the first item in the array.
	 If the array is empty it returns a "null" ersion of the object. */
	ObjectType& first() throw()
	{
		if (this->getInternal()->size() == 0) 
			return getNullObject();
		else
			return this->getArray()[0];
	}
	
	/** Returns the first item in the array.
	 If the array is empty it returns a "null" ersion of the object. */
	const ObjectType& first() const throw()
	{
		if (this->getInternal()->size() == 0) 
			return getNullObject();
		else
			return this->getArray()[0];
	}
	
	/** Returns the last item in the array.
	 If the array is empty it returns a "null" ersion of the object. */
	ObjectType& last() throw()
	{
		if (this->getInternal()->size() == 0) 
			return getNullObject();
		else
			return this->getArray()[this->size() - 1];
	}
	
	/** Returns the last item in the array.
	 If the array is empty it returns a "null" ersion of the object. */
	const ObjectType& last() const throw()
	{
		if (this->getInternal()->size() == 0) 
			return getNullObject();
		else
			return this->getArray()[this->size() - 1];
	}
	
	/** Returns an item from the array chosen at random.
	 The weighting for the random choice should be roughtly equally distributed.
	 If the array is empty it returns a "null" ersion of the object. */
	ObjectType& choose() throw()
	{
		if (this->getInternal()->size() == 0) 
			return getNullObject();
		else
			return this->getArray()[plonk::rand (this->length())];
	}
	
	/** Returns an item from the array chosen at random.
	 The weighting for the random choice should be roughtly equally distributed.
	 If the array is empty it returns a "null" ersion of the object. */	
	const ObjectType& choose() const throw()
	{
		if (this->getInternal()->size() == 0) 
			return getNullObject();
		else
			return this->getArray()[plonk::rand (this->length())];
	}
	
	/** Returns an item from the array using a weighted random choice.
	 This chooses an item in the array at random using another array as weights.
	 For example, if an array has 4 items and the weight array is @c IntArray(40, 30, 20, 10)
	 The item at index 0 has a 40% chance, index 1 a 30% chance and so on. The scale of the weights is
	 arbitrary (i.e., they do not need to sum to any particular value, 100 was chosen in the example
	 above to make explanation easier).
	 @param weights The weights array which can be any numerical type of array.
	 If the array is empty it returns a "null" ersion of the object. */
	template<class WeightType>
	ObjectType& wchoose (ObjectArray<WeightType> const& weights) throw()
	{
		if (this->getInternal()->size() == 0) 
		{
			return getNullObject();
		}
		else 
		{
			const int length = this->length();
			ObjectArray<WeightType> thresholds = ObjectArray<WeightType> (length, false);
			
			WeightType sum = 0;
            int i;
			for (i = 0; i < length; ++i)
			{
				sum += weights.wrapAt (i);
				thresholds[i] = sum;
			}
			
			const WeightType random = plonk::rand (sum);
			
			for (i = 0; i < length; ++i)
			{
				if (random < thresholds[i])
					return this->getArray()[i];
			}
			
			return this->getArray()[length-1];
		}
	}
	
	/** Returns an item from the array using a weighted random choice.
	 This chooses an item in the array at random using another array as weights.
	 For example, if an array has 4 items and the weight array is @c IntArray(40, 30, 20, 10)
	 The item at index 0 has a 40% chance, index 1 a 30% chance and so on. The scale of the weights is
	 arbitrary (i.e., they do not need to sum to any particular value, 100 was chosen in the example
	 above to make explanation easier).
	 @param weights The weights array which can be any numerical type of array.
	 If the array is empty it returns a "null" ersion of the object. */	
	template<class WeightType>
	const ObjectType& wchoose (ObjectArray<WeightType> const& weights) const throw()
	{
		if (this->getInternal()->size() == 0) 
		{
			return getNullObject();
		}
		else 
		{
			const int length = this->length();
			ObjectArray<WeightType> thresholds = ObjectArray<WeightType> (length, false);
			
			WeightType sum = 0;
            int i;
			for (i = 0; i < length; i++)
			{
				sum += weights.wrapAt (i);
				thresholds[i] = sum;
			}
			
			const WeightType random = plonk::rand (sum);
			
			for (i = 0; i < length; i++)
			{
				if (random < thresholds[i])
					return this->getArray()[i];
			}
			
			return this->getArray()[length-1];
		}
	}
    
    /** Determine if each item in the array is equal to the others. 
     If the array is empty it will return true. */
    bool areAllEqual() const throw()
    {
        if (this->length() > 0)
        {
            const ObjectType *thisArray = this->getArray();
            const ObjectType first = thisArray[0];
            
            for (int i = 1; i < this->length(); ++i)
                if (thisArray[i] != first)
                    return false;
        }
        
        return true;
    }
	    
    static const ObjectArray& getNull() throw()
	{
		static ObjectArray null;
		return null;
	}	        
        
    static ObjectType& getNullObject() throw()
	{
		static ObjectType null;
        null = ObjectType();
		return null;
	}	
    
//    static const ObjectType& getNullObject() throw()
//	{
//		return TypeUtility<ObjectType>::getNull();
//	}	
    
	/** Compares whether the contents of two arrays are the same. */
	bool operator== (ObjectArray<ObjectType> const& other) const throw()
	{		
		const ObjectType *thisArray = this->getArray();
		const ObjectType *otherArray = other.getArray();

		if (thisArray == otherArray) 
            return true;
		
		const int size = this->size();
        
		if (size != other.size()) 
            return false;
        
		if (size == 0) 
            return true;
		
		for (int i = 0; i < size; ++i)
			if (thisArray[i] != otherArray[i])
				return false;
		
		return true;
	}
	
	/** Compares whether the contents of two arrays are the same. */
	bool operator== (const ObjectType* otherArray) const throw()
	{		
		plonk_assert (otherArray != 0);
		
		const ObjectType *thisArray = this->getArray();
		
		if (thisArray == otherArray) 
            return true;
		
		const int size = this->size();
        
		for (int i = 0; i < size; ++i)
			if (thisArray[i] != otherArray[i])
				return false;
		
		return true;
	}
	
	/** Compares whether the contents of two arrays are the different. */
	bool operator!= (ObjectArray<ObjectType> const& other) const throw()
	{
		return !operator== (other);
	}
	
	/** Compares whether the contents of two arrays are the different. */
	bool operator!= (const ObjectType* otherArray) const throw()
	{
		return !operator== (otherArray);
	}
	
	/** Returns whether the array contains a particular item. */
	bool contains (ObjectType const& itemToSearchFor) const throw()
	{
		const ObjectType *array = this->getArray();
		
		if (array != 0)
		{
			const int size = this->size();
            
			for (int i = 0; i < size; ++i)
				if (array[i] == itemToSearchFor)
					return true;
		}		
		
		return false;
	}
		
	/** Returns whether the array contains one or more other items. 
	 @param itemsToSearchFor The array of other items. */
	bool contains (ObjectArray<ObjectType> const& itemsToSearchFor) const throw()
	{
		const ObjectType *array = this->getArray();
		const ObjectType *items = itemsToSearchFor.getArray();
		
		if (array != 0 && items != 0)
		{
			const int length = this->length();
			int itemsToFind = itemsToSearchFor.length();
			
			if (length == 0 || itemsToFind == 0)
				return false;
			
			int nextIndexToFind = 0;
			
			for (int i = 0; i < length; ++i)
			{
				if (array[i] == items[nextIndexToFind])
				{
					itemsToFind--;
					
					while (itemsToFind > 0)
					{
						i++;
						nextIndexToFind++;
						
						if ((i > length) || (array[i] != items[nextIndexToFind]))
							return false;
						
						itemsToFind--;
					}
					
					return true;
				}
			}
		}		
		
		return false;
	}
	
	/** Gets the index of a particular item.
	 @param itemToSearchFor The item to search for.
	 @param startIndex The start index for the search (useful if iterating over the array to find several items).
	 @return The index of the item or -1 if it is not found. */
	int indexOf (ObjectType const& itemToSearchFor, const int startIndex = 0) const throw()
	{
		const ObjectType *array = this->getArray();
		
		if (array != 0)
		{
			const int size = this->size();
			for (int i = startIndex < 0 ? 0 : startIndex; i < size; ++i)
				if (array[i] == itemToSearchFor)
					return i;
		}		
		
		return -1;
	}
	
	/** Search for a particular sub sequence withing the array. */
	int indexOf (ObjectArray<ObjectType> const& itemsToSearchFor, const int startIndex = 0) const throw()
	{
		const ObjectType *array = this->getArray();
		const ObjectType *items = itemsToSearchFor.getArray();

		if (array != 0 && items != 0)
		{
			const int length = this->length();
			int itemsToFind = itemsToSearchFor.length();
			
			if (length == 0 || itemsToFind == 0)
				return -1;
			
			int nextIndexToFind = 0;
			
			for (int i = startIndex < 0 ? 0 : startIndex; i < length; ++i)
			{
				if (array[i] == items[nextIndexToFind])
				{
					int foundAtIndex = i;
					
					itemsToFind--;
					
					while (itemsToFind > 0)
					{
						i++;
						nextIndexToFind++;
						
						if ((i > length) || (array[i] != items[nextIndexToFind]))
							return -1;
						
						itemsToFind--;
					}
					
					return foundAtIndex;
				}
			}
		}		
		
		return -1;
	}
	
	/** Search for any items in a given array. */
	int indexOfAny (ObjectArray<ObjectType> const& itemsToSearchFor, const int startIndex = 0) const throw()
	{
		const ObjectType *array = this->getArray();
		
		if (array != 0)
		{
			const int length = this->length();
			const int itemsToFind = itemsToSearchFor.length();
			
			if (length == 0 || itemsToFind == 0)
				return -1;
			
			for (int i = startIndex < 0 ? 0 : startIndex; i < length; ++i)
				if (itemsToSearchFor.contains (array[i]))
					return i;
		}
		
		return -1;
	}
	
	/** Return a new array which is a subarray of this one. 
	 @return An array from startIndex to the end of the array. */
	ObjectArray<ObjectType> from (const int startIndex) const throw()
	{
		return range (startIndex);
	}
	
	/** Return a new array which is a subarray of this one. 
	 @return An array from startIndex (inclusive) to the end of the array. */
	ObjectArray<ObjectType> range (const int startIndex) const throw()
	{
		return this->range (startIndex, 0x7fffffff);
	}
	
	/** Return a new array which is a subarray of this one. 
	 @return An array from startIndex (inclusive) to the end of the array. */
	ObjectArray<ObjectType> to (const int endIndex) const throw()
	{
		return range (0, endIndex);
	}
	
	/** Return a new array which is a subarray of this one. 
	 @return An array from startIndex (inclusive) to endIndex (exclusive). */
	ObjectArray<ObjectType> range (const int startIndex, const int endIndex) const throw()
	{
		const int size = this->size();
        
		if (size == 0) 
            return ObjectArray<ObjectType>();
		
		const int startIndexChecked = startIndex < 0 ? 0 : startIndex;
		const int endIndexChecked = endIndex > size ? size : endIndex;

		if (startIndexChecked >= endIndexChecked) return ObjectArray<ObjectType>();
		
		const ObjectType *sourceArray = this->getArray();
        
		if (sourceArray == 0) 
            return ObjectArray<ObjectType>();
		
		const bool needsNull = this->isNullTerminated();
		
		const int diff = endIndexChecked - startIndexChecked;
		const int newSize = needsNull && (endIndexChecked < size) ? diff + 1 : diff;
				
		ObjectArray<ObjectType> result (newSize, needsNull);
		ObjectType *resultArray = result.getArray();
		
		int resultIndex = 0;
		for (int sourceIndex = startIndexChecked; sourceIndex < endIndexChecked; ++sourceIndex)
			resultArray[resultIndex++] = sourceArray[sourceIndex];
		
		if (needsNull)
			resultArray[resultIndex] = this->getNullObject();
		
		return result;
	}
	
	/** Perform a find and replace on the array and return the result.
	 All occurrences of @c find are replaced with @c substitute. */
	ObjectArray<ObjectType> replace (ObjectArray<ObjectType> const& find, ObjectArray<ObjectType> const& substitute) const throw()
	{
		const int size = this->size();
		const int findLength = find.length();
		int startIndex = 0;
		int findIndex;
		
		ObjectArray<ObjectType> result;
		
		while ((findIndex = this->indexOf (find, startIndex)) >= 0)
		{
			result.add (this->range (startIndex, findIndex));
			result.add (substitute);
			startIndex = findIndex + findLength;
		}
		
		result.add (this->range (startIndex, size));
		
		return result;
	}
	
	/** Remove an item in-place. */
	ObjectArray<ObjectType>& removeItem (const ObjectType item) throw()
	{
		const int index = this->indexOf (item);
		
		if (index >= 0)
			this->getInternal()->remove (index);
		
		return *this;
	}
	
	/** Remove several items in-place. */
	ObjectArray<ObjectType>& removeItems (ObjectArray<ObjectType> const& items) throw()
	{
		for (int i = 0; i < items.length(); i++)
			this->removeItem (items[i]);
		
		return *this;
	}
	
	/** Group this array into a 2D array with a particular group size. */
	ObjectArray2DType group (const int groupSize) const throw()
	{
		const int length = this->length();
        
		if (length == 0) 
            return ObjectArray<ObjectType>();
		
		const int groupSizeChecked = groupSize < 1 ? 1 : groupSize;
		const int groupSizeMinus1 = groupSizeChecked - 1;
		const bool needsNull = this->isNullTerminated();
		
		ObjectArray< ObjectArray<ObjectType> > result;
		ObjectArray<ObjectType> element;
		
		const ObjectType *thisArray = this->getArray();
		
		for (int i = 0; i < length; ++i)
		{
			element.add (thisArray[i]);
            
			if ((i % groupSizeChecked) == groupSizeMinus1)
			{
				if (needsNull) 
				{
					element.add (this->getNullObject());
					element.setNullTerminated (true);
				}
				
				result.add (element);
				element = ObjectArray<ObjectType>(); // reset
			}
		}
		
		if (element.length() != 0)
		{
			if (needsNull) 
			{
				element.add (this->getNullObject());
				element.setNullTerminated (true);
			}
			
			result.add (element);
		}
		
		return result;
	}
	
	/** Split the array into a 2D array using delimiters. 
	 The array will the split when ANY of the delimiters is found. */
	ObjectArray2DType split (ObjectArray<ObjectType> delimiters) const throw()
	{
		const int length = this->length();
        
		if (length == 0) 
            return ObjectArray<ObjectType>();
		
		const bool needsNull = this->isNullTerminated();
		
		ObjectArray< ObjectArray<ObjectType> > result;
		ObjectArray<ObjectType> element;
		
		const ObjectType *thisArray = this->getArray();
        
		if (thisArray != 0)
		{
			for (int i = 0; i < length; ++i)
			{
				const ObjectType item = thisArray[i];
				
				if (delimiters.contains(item))
				{
					if (element.length() > 0)
					{
						if (needsNull) 
						{
							element.add (this->getNullObject());
							element.setNullTerminated (true);
						}
						
						result.add (element);
						element = ObjectArray<ObjectType>();	// reset
					}
				}
				else
				{
					element.add (item);
				}
			}
			
			if (element.length() != 0)
			{
				if (needsNull) 
				{
					element.add (this->getNullObject());
					element.setNullTerminated (true);
				}
				
				result.add (element);
			}
		}
		
		return result;		
	}
	
    /** Split the array into a 2D array using a delimiting sequence. 
	 The array will the split when ALL of the delimiters are found in the same sequence. */
	ObjectArray2DType splitSequence (ObjectArray<ObjectType> delimitingSequence) const throw()
	{
		const int size = this->size();
		ObjectArray< ObjectArray<ObjectType> > result;
		
		if (size != 0)
		{
			const int findLength = delimitingSequence.length();
			int startIndex = 0;
			int findIndex;
			
			while ((findIndex = this->indexOf (delimitingSequence, startIndex)) >= 0)
			{
				result.add (this->range (startIndex, findIndex));
				startIndex = findIndex + findLength;
			}
			
			result.add (this->range (startIndex, size));
		}
		
		return result;		
	}     
    
    /** De-interleave an array into a 2D array.
     The 2D array will contain @c numGroups sub-arrays. Items in the source
     array are added to the sub-arrays in turn until all items are added.
     
     Example:
     @code
     IntArray source1d (1, 2, 3, 4, 5, 6, 7);
     IntArray2D dest2d = source1d.deinterleave (2);
      // result will be {{1, 3, 5, 7}, {2, 4, 6}}
     @endcode */
    ObjectArray2DType deinterleave (const int numGroups) const throw()
	{
        int i, j;
        
        plonk_assert (!this->isNullTerminated()); // could be done but need this for audio de-interleaving for now..
        
		const int length = this->length();
        
        if (length == 0) 
            return ObjectArray<ObjectType>();
        
        plonk_assert (numGroups > 0);
                
        const int subLength = ((unsigned int)length % (unsigned int)numGroups) == 0 
                              ? (unsigned int)length / (unsigned int)numGroups 
                              : (unsigned int)length / (unsigned int)numGroups + 1;
        
        ObjectArray2DType result = ObjectArray2DType::withSize (numGroups);
        ObjectArray* const resultArray = result.getArray();
        
        int lengthRemaining = length;
        
        for (i = 0; i < numGroups; ++i)
        {
            const int subLengthUsed = plonk::min (subLength, lengthRemaining);
            ObjectArray sub = ObjectArray::withSize (subLengthUsed);
            ObjectType* const subArray = sub.getArray();

            const ObjectType *thisArray = this->getArray() + i;
            
            for (j = 0; j < subLengthUsed; ++j, thisArray += numGroups)
                subArray[j] = *thisArray;
            
            resultArray[i] = sub;
            lengthRemaining -= subLengthUsed;
        }		
		
		return result;
	}

    PLONK_OBJECTARROWOPERATOR(ObjectArray);

//    static void deinterleave (ObjectType* dst, const ObjectType* src, const int srcSize, const int numGroups) throw()
//    {
//        if (numGroups <= 1)
//        {
//            Memory::copy (dst, src, srcSize * sizeof (ObjectType));
//        }
//        else
//        {
//            plonk_assert (srcSize % numGroups);
//
//            const int dstSize = srcSize / numGroups;
//            int i, j;
//            
//            if (numGroups == 2)
//            {
//                for (i = 0; i < srcSize; ++i, ++dst, src += 2)
//                {
//                    dst[0] = src[0];
//                    dst[dstSize] = src[1];
//                }
//            }
//            else 
//            {
//                for (i = 0; i < srcSize; ++i, ++dst, src += numGroups)
//                    for (j = 0; j < numGroups; ++j)
//                        dst[j * dstSize] = src[j];
//            }
//        }
//    }

    static void deinterleave (ObjectType* dst, const ObjectType* src, const int srcSize, const int numGroups) throw()
    {
        if (numGroups <= 1)
        {
            Memory::copy (dst, src, srcSize * sizeof (ObjectType));
        }
        else
        {
            plonk_assert (srcSize % numGroups);
            
            const int dstSize = srcSize / numGroups;
            int i, j;
            
            for (i = 0; i < numGroups; ++i)
            {
                ObjectType* const dstTemp = dst + srcSize * i;
                const ObjectType* srcTemp = src + i;
                
                for (j = 0; j < dstSize; ++j, srcTemp += numGroups)
                    dstTemp[j] = *srcTemp;
            }        
        }
    }

    static void deinterleave (ObjectType** dst, const ObjectType* src, const int srcSize, const int numGroups) throw()
    {
        plonk_assert (srcSize % numGroups);
        
        const int dstSize = srcSize / numGroups;
        int i, j;
        
        for (i = 0; i < numGroups; ++i)
        {
            const ObjectType* srcTemp = src + i;
            ObjectType* const dstTemp = dst[i];
            
            for (j = 0; j < dstSize; ++j, srcTemp += numGroups)
                dstTemp[j] = *srcTemp;
        }        
    }
    
    static void interleave (ObjectType* dst, const ObjectType* src, const int srcSize, const int numGroups) throw()
    {        
        if (numGroups <= 1)
        {
            Memory::copy (dst, src, srcSize * sizeof (ObjectType));
        }
        else
        {            
            const int dstSize = srcSize * numGroups;
            int i, j;

            for (i = 0; i < numGroups; ++i)
            {
                ObjectType* dstTemp = dst + i;
                const ObjectType* const srcTemp = src + srcSize * i;
                
                for (j = 0; j < dstSize; ++j, dstTemp += numGroups)
                    *dstTemp = srcTemp[j];
            }        
        }
    }
    
    static void interleave (ObjectType* dst, const ObjectType** src, const int srcSize, const int numGroups) throw()
    {
        plonk_assert (srcSize % numGroups);
 
        const int dstSize = srcSize * numGroups;
        int i, j;

        for (i = 0; i < numGroups; ++i)
        {
            ObjectType* dstTemp = dst + i;
            const ObjectType* const srcTemp = src[i];
            
            for (j = 0; j < dstSize; ++j, dstTemp += numGroups)
                *dstTemp = srcTemp[j];
        }        
    }
};




#endif // PLONK_OBJECTARRAY_H
