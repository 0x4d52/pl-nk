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

#ifndef PLONK_OBJECTARRAY2D_H
#define PLONK_OBJECTARRAY2D_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

/** @ingroup PlonkContainerClasses */
template<class ArrayType, class RowType>
class ObjectArray2DBase : public ObjectArray<RowType>
{
public:
    typedef ObjectArrayInternal<RowType>                Internal;
    typedef ObjectArray<RowType>                        Base;    
    typedef WeakPointerContainer<ObjectArray2DBase>     Weak;
    
    typedef ObjectArray<ArrayType>                      ObjectArrayType;
    typedef ObjectArray<ObjectArrayType>                ObjectArray2DType;

    
	ObjectArray2DBase (const int rows = 0) throw()
	:	ObjectArray<RowType> (rows < 0 ? 0 : rows, false) 
	{
	}
    
	ObjectArray2DBase (RowType const& single) throw()
	:	ObjectArray<RowType> (single)
	{
	}
    
    explicit ObjectArray2DBase (Internal* internalToUse) throw() 
	:	ObjectArray<RowType> (internalToUse)
	{
	} 
    
	/** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    ObjectArray2DBase (ObjectArray2DBase const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    ObjectArray2DBase (Dynamic const& other) throw()
    :   Base (other.as<ObjectArray2DBase>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    ObjectArray2DBase& operator= (ObjectArray2DBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
	
private:
	static int countValidInitialisers (RowType const& i03) throw()
	{
		int size = 3;
		if (i03.length() > 0) size++; else return size;
		return size;
	}	
    
public:
	ObjectArray2DBase (RowType const &i00,
					   RowType const &i01,
					   RowType const &i02,
					   RowType const &i03 = RowType()) throw()
	:	ObjectArray<RowType> (countValidInitialisers (i03), false) 
	{
		RowType *thisArray = this->getArray();
		
		if (i00.length() > 0) thisArray[ 0] = i00; else return;
		if (i01.length() > 0) thisArray[ 1] = i01; else return;
		if (i02.length() > 0) thisArray[ 2] = i02; else return;
		if (i03.length() > 0) thisArray[ 3] = i03; else return;
	}
    
private:
	static int countValidInitialisers (RowType const &i05,
									   RowType const &i06,
									   RowType const &i07) throw()
	{
		int size = 5;
		
		if (i05.length() > 0) size++; else return size;
		if (i06.length() > 0) size++; else return size;
		if (i07.length() > 0) size++; else return size;
		
		return size;
	}
    
public:
	ObjectArray2DBase (RowType const &i00,
					   RowType const &i01,
					   RowType const &i02,
					   RowType const &i03,
					   RowType const &i04,
					   RowType const &i05 = RowType(),
					   RowType const &i06 = RowType(),
					   RowType const &i07 = RowType()) throw()
	:	ObjectArray<RowType> (countValidInitialisers (i05, i06, i07), false)
	{
		RowType *thisArray = this->getArray();
		
		if (i00.length() > 0) thisArray[ 0] = i00; else return;
		if (i01.length() > 0) thisArray[ 1] = i01; else return;
		if (i02.length() > 0) thisArray[ 2] = i02; else return;
		if (i03.length() > 0) thisArray[ 3] = i03; else return;
		if (i04.length() > 0) thisArray[ 4] = i04; else return;
		if (i05.length() > 0) thisArray[ 5] = i05; else return;
		if (i06.length() > 0) thisArray[ 6] = i06; else return;
		if (i07.length() > 0) thisArray[ 7] = i07; else return;
	}
    
private:
	static int countValidInitialisers(RowType const &i09,
									  RowType const &i10,
									  RowType const &i11,
									  RowType const &i12,
									  RowType const &i13,
									  RowType const &i14,
									  RowType const &i15) throw()
	{
		int size = 9;
		
		if (i09.length() > 0) size++; else return size;
		if (i10.length() > 0) size++; else return size;
		if (i11.length() > 0) size++; else return size;
		if (i12.length() > 0) size++; else return size;
		if (i13.length() > 0) size++; else return size;
		if (i14.length() > 0) size++; else return size;
		if (i15.length() > 0) size++; else return size;
		
		return size;
	}
    
public:
	ObjectArray2DBase (RowType const &i00,
					   RowType const &i01,
					   RowType const &i02,
					   RowType const &i03,
					   RowType const &i04,
					   RowType const &i05,
					   RowType const &i06,
					   RowType const &i07,
					   RowType const &i08,
					   RowType const &i09 = RowType(),
					   RowType const &i10 = RowType(),
					   RowType const &i11 = RowType(),
					   RowType const &i12 = RowType(),
					   RowType const &i13 = RowType(),
					   RowType const &i14 = RowType(),
					   RowType const &i15 = RowType()) throw()
	:	ObjectArray<RowType> (countValidInitialisers (     i09, i10, i11, 
													  i12, i13, i14, i15), 
							  false)
	{
		RowType *thisArray = this->getArray();
		
		if (i00.length() > 0) thisArray[ 0] = i00; else return;
		if (i01.length() > 0) thisArray[ 1] = i01; else return;
		if (i02.length() > 0) thisArray[ 2] = i02; else return;
		if (i03.length() > 0) thisArray[ 3] = i03; else return;
		if (i04.length() > 0) thisArray[ 4] = i04; else return;
		if (i05.length() > 0) thisArray[ 5] = i05; else return;
		if (i06.length() > 0) thisArray[ 6] = i06; else return;
		if (i07.length() > 0) thisArray[ 7] = i07; else return;
		if (i08.length() > 0) thisArray[ 8] = i08; else return;
		if (i09.length() > 0) thisArray[ 9] = i09; else return;
		if (i10.length() > 0) thisArray[10] = i10; else return;
		if (i11.length() > 0) thisArray[11] = i11; else return;
		if (i12.length() > 0) thisArray[12] = i12; else return;
		if (i13.length() > 0) thisArray[13] = i13; else return;
		if (i14.length() > 0) thisArray[14] = i14; else return;
		if (i15.length() > 0) thisArray[15] = i15; else return;
	}
    
private:
	static int countValidInitialisers (RowType const &i17,
									   RowType const &i18,
									   RowType const &i19,
									   RowType const &i20,
									   RowType const &i21,
									   RowType const &i22,
									   RowType const &i23,
									   RowType const &i24,
									   RowType const &i25,
									   RowType const &i26,
									   RowType const &i27,
									   RowType const &i28,
									   RowType const &i29,
									   RowType const &i30,
									   RowType const &i31) throw()
	{
		int size = 17;
		
		if (i17.length() > 0) size++; else return size;
		if (i18.length() > 0) size++; else return size;
		if (i19.length() > 0) size++; else return size;
		if (i20.length() > 0) size++; else return size;
		if (i21.length() > 0) size++; else return size;
		if (i22.length() > 0) size++; else return size;
		if (i23.length() > 0) size++; else return size;
		if (i24.length() > 0) size++; else return size;
		if (i25.length() > 0) size++; else return size;
		if (i26.length() > 0) size++; else return size;
		if (i27.length() > 0) size++; else return size;
		if (i28.length() > 0) size++; else return size;
		if (i29.length() > 0) size++; else return size;
		if (i30.length() > 0) size++; else return size;
		if (i31.length() > 0) size++; else return size;
		
		return size;
	}
    
public:
	ObjectArray2DBase (RowType const &i00,
					   RowType const &i01,
					   RowType const &i02,
					   RowType const &i03,
					   RowType const &i04,
					   RowType const &i05,
					   RowType const &i06,
					   RowType const &i07,
					   RowType const &i08,
					   RowType const &i09,
					   RowType const &i10,
					   RowType const &i11,
					   RowType const &i12,
					   RowType const &i13,
					   RowType const &i14,
					   RowType const &i15,
					   RowType const &i16,
					   RowType const &i17 = RowType(),
					   RowType const &i18 = RowType(),
					   RowType const &i19 = RowType(),
					   RowType const &i20 = RowType(),
					   RowType const &i21 = RowType(),
					   RowType const &i22 = RowType(),
					   RowType const &i23 = RowType(),
					   RowType const &i24 = RowType(),
					   RowType const &i25 = RowType(),
					   RowType const &i26 = RowType(),
					   RowType const &i27 = RowType(),
					   RowType const &i28 = RowType(),
					   RowType const &i29 = RowType(),
					   RowType const &i30 = RowType(),
					   RowType const &i31 = RowType()) throw()
	:	ObjectArray<RowType> (countValidInitialisers (     i17, i18, i19,
													  i20, i21, i22, i23,
													  i24, i25, i26, i27,
													  i28, i29, i30, i31), 
							  false)
	{
		RowType *thisArray = this->getArray();
		
		if (i00.length() > 0) thisArray[ 0] = i00; else return;
		if (i01.length() > 0) thisArray[ 1] = i01; else return;
		if (i02.length() > 0) thisArray[ 2] = i02; else return;
		if (i03.length() > 0) thisArray[ 3] = i03; else return;
		if (i04.length() > 0) thisArray[ 4] = i04; else return;
		if (i05.length() > 0) thisArray[ 5] = i05; else return;
		if (i06.length() > 0) thisArray[ 6] = i06; else return;
		if (i07.length() > 0) thisArray[ 7] = i07; else return;
		if (i08.length() > 0) thisArray[ 8] = i08; else return;
		if (i09.length() > 0) thisArray[ 9] = i09; else return;
		if (i10.length() > 0) thisArray[10] = i10; else return;
		if (i11.length() > 0) thisArray[11] = i11; else return;
		if (i12.length() > 0) thisArray[12] = i12; else return;
		if (i13.length() > 0) thisArray[13] = i13; else return;
		if (i14.length() > 0) thisArray[14] = i14; else return;
		if (i15.length() > 0) thisArray[15] = i15; else return;
		if (i16.length() > 0) thisArray[16] = i16; else return;
		if (i17.length() > 0) thisArray[17] = i17; else return;
		if (i18.length() > 0) thisArray[18] = i18; else return;
		if (i19.length() > 0) thisArray[19] = i19; else return;
		if (i20.length() > 0) thisArray[20] = i20; else return;
		if (i21.length() > 0) thisArray[21] = i21; else return;
		if (i22.length() > 0) thisArray[22] = i22; else return;
		if (i23.length() > 0) thisArray[23] = i23; else return;
		if (i24.length() > 0) thisArray[24] = i24; else return;
		if (i25.length() > 0) thisArray[25] = i25; else return;
		if (i26.length() > 0) thisArray[26] = i26; else return;
		if (i27.length() > 0) thisArray[27] = i27; else return;
		if (i28.length() > 0) thisArray[28] = i28; else return;
		if (i29.length() > 0) thisArray[29] = i29; else return;
		if (i30.length() > 0) thisArray[30] = i30; else return;
		if (i31.length() > 0) thisArray[31] = i31; else return;
	}
    
	ObjectArrayAssignmentDefinition(ObjectArray2DBase, RowType);
	
	ObjectArray2DBase (ObjectArray2DBase<ArrayType,RowType> const& array0, 
					   ObjectArray2DBase<ArrayType,RowType> const& array1) throw()
	:	ObjectArray< RowType > (array0, array1)
	{
	}
    
    /** Does the item contain a particular item. */
	bool contains (ArrayType const& item) const throw()
	{
		const int numRows = this->length();
		for (int i = 0; i < numRows; ++i)
		{
			if (this->atUnchecked (i).ObjectArray<ArrayType>::contains (item))
				return true;
		}
		
		return false;
	}
	
	/** Search for the row and column index of an item.
	 On input row and column are the start indices to search so should be
	 set to zero before calling this function if you want to search the whole
	 2D array. On exit they will contain -1 if the item is not found. */
	void indexOf (ArrayType const& item, int& row, int& column) const throw()
	{
		const int numRows = this->length();
		for (int i = row < 0 ? 0 : row; i < numRows; ++i)
		{
			int c = this->atUnchecked (i).ObjectArray<ArrayType>::indexOf (item, column);
			if (c >= 0)
			{
				row = i;
				column = c;
				return;
			}
		}
		
		row = -1;
		column = -1;
	}
	
    /** Does each of the rows of the 2D array contain the same number of items. */
	bool isMatrix() const throw()
	{
		const int numRows = this->length();
		if (numRows < 2) return false;
		
		int numColumns = this->atUnchecked (0).length();
		if (numColumns < 2) return false;
		
		int prevNumColumns = numColumns;
		for (int row = 1; row < numRows; ++row)
		{
			numColumns = this->atUnchecked (row).length();
			if (numColumns != prevNumColumns)
				return false;
			
			prevNumColumns = numColumns;
		}
		
		return true;
	}
	
    /** Return the number rows in the 2D array. */
	int numRows() const throw()
	{
		return this->size();
	}
	
    /** Return the largest length row in each of the rows in the 2D array. 
     This can be used in conjunction with isMatrix() to determin if all the row
     lengths are the same (i.e., the 2D array is a matrix). */
	int numColumns() const throw()
	{
		const int numRows = this->length();
		if (numRows < 1) return 0;
		
		int numColumns = this->atUnchecked(0).length();
		
		int maxNumColumns = numColumns;
		for (int row = 1; row < numRows; ++row)
		{
			numColumns = this->atUnchecked (row).length();
			if (numColumns > maxNumColumns)
				maxNumColumns = numColumns;
		}
		
		return maxNumColumns;
	}
    
    /** Flatten the array to a one dimensional array. */
	RowType flatten() const throw()
	{
		RowType result;
		const int numRows = this->length();
		for (int i = 0; i < numRows; ++i)
		{
			const RowType& item = this->atUnchecked (i);
			result <<= item;
		}			
		
		return result;
	}
    
    RowType columnWrapAt (const int index) throw()
    {
        const int numRows = this->length();
        RowType result (RowType::withSize (numRows));
        
        for (int i = 0; i < numRows; ++i)
		{
			const RowType& row = this->atUnchecked (i);
			result.put (i, row.wrapAt (index));
		}			
        
        return result;
    }
    
    /** Interleave the 2D array into one dimension.
     The result will contain the same number of array items but flattened
     to a one dimensional array. 
     
     Example:
     @code
     IntArray source1d (1, 2, 3, 4, 5, 6, 7);
     
     IntArray2D dest2d = source1d.deinterleave (2);
     // result will be {{1, 3, 5, 7}, {2, 4, 6}}
     
     IntArray dest1d = dest2d.interleave();
     // result will be {1, 2, 3, 4, 5, 6, 7}
     @endcode */
    ObjectArrayType interleave() throw()
    {
        int i, j;
        int actualSize = 0;
        const int numRows = this->numRows();
        
        ObjectArrayType result = ObjectArrayType::withSize (numRows * this->numColumns());
        const RowType* const rows = this->getArray();
        
        for (i = 0; i < numRows; ++i)
        {
            const ArrayType* const sourceArray = rows[i].getArray();
            const int numColumns = rows[i].length();
            ArrayType* resultArray = result.getArray() + i;
            
            for (j = 0; j < numColumns; ++j, resultArray += numRows, ++actualSize)
                *resultArray = sourceArray[j];
        }
        
        result.setSize (actualSize, true); // shrink the size if needed
        
        return result;
    }
	
	
	//ObjectArrayConcatOperatorsDefine(ObjectArray2DBase, ArrayType);
};


//------------------------------------------------------------------------------



template<class ArrayType, class RowType>
class ObjectArray2D : public ObjectArray2DBase<ArrayType,RowType>
{
public:
    typedef ObjectArrayInternal<RowType>            Internal; 
    typedef ObjectArray2DBase<ArrayType,RowType>    Base;    
    typedef WeakPointerContainer<ObjectArray2D>     Weak;
    
    typedef ObjectArray<ArrayType>                  ObjectArrayType;
    typedef ObjectArray<ObjectArrayType>            ObjectArray2DType;
    
	
    explicit ObjectArray2D (Internal* internalToUse) throw() 
	:	ObjectArray<RowType> (internalToUse)
	{
	} 
    
	/** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    ObjectArray2D (ObjectArray2D const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    ObjectArray2D (Dynamic const& other) throw()
    :   Base (other.as<ObjectArray2D>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    ObjectArray2D& operator= (ObjectArray2D const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static ObjectArray2D fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
        
    static const ObjectArray2D& getNull() throw()
	{
		static ObjectArray2D null;
		return null;
	}	                
    
    ObjectArray2D (const int rows = 0) throw()
	:	ObjectArray2DBase< ArrayType, RowType > (rows < 0 ? 0 : rows)//, false) 
	{
	}
    
	ObjectArray2D (const int rows, const int columns) throw()
	:	ObjectArray2DBase< ArrayType, RowType > (rows < 0 ? 0 : rows)//, false) 
	{
		if (columns > 0 && rows > 0)
		{
			for (int i = 0; i < rows; i++)
			{
				this->put (i, RowType (columns, false));
			}
		}
	}
    
	ObjectArray2D (RowType const& single) throw()
	:	ObjectArray2DBase< ArrayType, RowType > (single)
	{
	}
    
	ObjectArray2D (RowType const &i00,
				   RowType const &i01,
				   RowType const &i02 = RowType(),
				   RowType const &i03 = RowType()) throw()
	:	ObjectArray2DBase<ArrayType, RowType> (i00, i01, i02, i03) 
	{
	}
    
	ObjectArray2D (RowType const &i00,
				   RowType const &i01,
				   RowType const &i02,
				   RowType const &i03,
				   RowType const &i04,
				   RowType const &i05 = RowType(),
				   RowType const &i06 = RowType(),
				   RowType const &i07 = RowType()) throw()
	:	ObjectArray2DBase<ArrayType, RowType> (i00, i01, i02, i03,
											   i04, i05, i06, i07) 
	{
	}
    
	ObjectArray2D (RowType const &i00,
				   RowType const &i01,
				   RowType const &i02,
				   RowType const &i03,
				   RowType const &i04,
				   RowType const &i05,
				   RowType const &i06,
				   RowType const &i07,
				   RowType const &i08,
				   RowType const &i09 = RowType(),
				   RowType const &i10 = RowType(),
				   RowType const &i11 = RowType(),
				   RowType const &i12 = RowType(),
				   RowType const &i13 = RowType(),
				   RowType const &i14 = RowType(),
				   RowType const &i15 = RowType()) throw()
	:	ObjectArray2DBase<ArrayType, RowType> (i00, i01, i02, i03,
											   i04, i05, i06, i07,
											   i08, i09, i10, i11, 
											   i12, i13, i14, i15)
	{
	}
    
	ObjectArray2D (RowType const &i00,
				   RowType const &i01,
				   RowType const &i02,
				   RowType const &i03,
				   RowType const &i04,
				   RowType const &i05,
				   RowType const &i06,
				   RowType const &i07,
				   RowType const &i08,
				   RowType const &i09,
				   RowType const &i10,
				   RowType const &i11,
				   RowType const &i12,
				   RowType const &i13,
				   RowType const &i14,
				   RowType const &i15,
				   RowType const &i16,
				   RowType const &i17 = RowType(),
				   RowType const &i18 = RowType(),
				   RowType const &i19 = RowType(),
				   RowType const &i20 = RowType(),
				   RowType const &i21 = RowType(),
				   RowType const &i22 = RowType(),
				   RowType const &i23 = RowType(),
				   RowType const &i24 = RowType(),
				   RowType const &i25 = RowType(),
				   RowType const &i26 = RowType(),
				   RowType const &i27 = RowType(),
				   RowType const &i28 = RowType(),
				   RowType const &i29 = RowType(),
				   RowType const &i30 = RowType(),
				   RowType const &i31 = RowType()) throw()
	:	ObjectArray2DBase<ArrayType, RowType> (i00, i01, i02, i03,
											   i04, i05, i06, i07,
											   i08, i09, i10, i11, 
											   i12, i13, i14, i15, 
											   i16, i17, i18, i19,
											   i20, i21, i22, i23,
											   i24, i25, i26, i27,
											   i28, i29, i30, i31)
	{
	}
    
	ObjectArrayAssignmentDefinition(ObjectArray2D, RowType);
    
	ObjectArray2D (ObjectArray2D<ArrayType,RowType> const& array0, 
				   ObjectArray2D<ArrayType,RowType> const& array1) throw()
	:	ObjectArray2DBase< ArrayType, RowType > (array0, array1)
	{
	}
    
    PLONK_OBJECTARROWOPERATOR(ObjectArray2D)

};



#endif // PLONK_OBJECTARRAY2D_H
