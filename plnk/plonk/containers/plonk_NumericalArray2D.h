/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLONK_NUMERICALARRAY2D_H
#define PLONK_NUMERICALARRAY2D_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

/** @ingroup PlonkContainerClasses */
template<class NumericalType, class RowType>
class NumericalArray2D : public ObjectArray2DBase<NumericalType, RowType>
{
public:    
    typedef ObjectArrayInternal<RowType>                Internal; 
    typedef ObjectArray2DBase<NumericalType, RowType>   Base;    
    typedef WeakPointerContainer<NumericalArray2D>      Weak;
    
    typedef ObjectArray<NumericalType>                  ObjectArrayType;
    typedef ObjectArray<ObjectArrayType>                ObjectArray2DType;
    typedef typename Base::InitialObject                InitialObject;
    
	NumericalArray2D (const int rows = 0) throw()
	:	Base (rows < 0 ? 0 : rows, false) 
	{
	}
    
    explicit NumericalArray2D (Internal* internal) throw() 
	:	Base (internal)
	{
	}
        
    /** Copy constructor. */
    NumericalArray2D (NumericalArray2D const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    NumericalArray2D (Dynamic const& other) throw()
    :   Base (other.as<NumericalArray2D>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    NumericalArray2D& operator= (NumericalArray2D const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    	
	NumericalArray2D (const int rows, const int columns, const bool zeroData = false) throw()
	:	Base (rows < 0 ? 0 : rows, false) 
	{
		if (columns > 0 && rows > 0)
			for (int i = 0; i < rows; ++i)
				this->put (i, RowType (NumericalArraySpec (columns, zeroData)));
	}
    	
	NumericalArray2D (ObjectArray2DType const& other) throw()
	:	Base (other.size())
	{
		const int rows = this->size();
        
		for (int row = 0; row < rows; ++row)
			this->put (row, static_cast<RowType const&> (other[row]));
	}

	NumericalArray2D (RowType const& single) throw()
	:	Base (single)
	{
	}
	
	NumericalArray2D (InitialObject const &i00,
					  InitialObject const &i01,
					  InitialObject const &i02,// = InitialObject(),
					  InitialObject const &i03 = InitialObject()) throw()
	:	Base (i00, i01, i02, i03) 
	{
	}

	NumericalArray2D (InitialObject const &i00,
					  InitialObject const &i01,
					  InitialObject const &i02,
					  InitialObject const &i03,
					  InitialObject const &i04,
					  InitialObject const &i05 = InitialObject(),
					  InitialObject const &i06 = InitialObject(),
					  InitialObject const &i07 = InitialObject()) throw()
	:	Base (i00, i01, i02, i03, i04, i05, i06, i07) 
	{
	}

	NumericalArray2D (InitialObject const &i00,
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
	:	Base (i00, i01, i02, i03, i04, i05, i06, i07,
			  i08, i09, i10, i11, i12, i13, i14, i15)
	{
	}

	NumericalArray2D (InitialObject const &i00,
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
	:	Base (i00, i01, i02, i03, i04, i05, i06, i07,
			  i08, i09, i10, i11, i12, i13, i14, i15, 
              i16, i17, i18, i19, i20, i21, i22, i23,
			  i24, i25, i26, i27, i28, i29, i30, i31)
	{
	}

	ObjectArrayAssignmentDefinition(NumericalArray2D, RowType);

	
	NumericalArray2D (NumericalArray2D const& array0, 
					  NumericalArray2D const& array1) throw()
	:	Base (array0, array1)
	{
	}

	void print (const char *prefix = 0, const bool rowsOnOneLine = false) const throw()
	{
#if !PLONK_ANDROID
		if (rowsOnOneLine)
		{
			for (int row = 0; row < this->size(); ++row)
			{
				if (prefix) std::cout << prefix;
				
				std::cout << "[" << row << "]: ";
								
				this->at(row).print(0, true);
			}
		}
		else
		{
			for (int row = 0; row < this->size(); ++row)
			{
				const NumericalArray<NumericalType>& rowArray = this->at (row);
				
				for (int column = 0; column < rowArray.size(); column++)
				{
					if (prefix) std::cout << prefix;
					
					std::cout << "[" << row << "][" << column << "] = " << rowArray[column] << std::endl;
				}		
			}
		}
#endif
	}
	
    PLONK_OBJECTARROWOPERATOR(NumericalArray2D);

};


#endif // PLONK_NUMERICALARRAY2D_H
