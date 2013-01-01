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

#ifndef PLONK_TEXTARRAY_H
#define PLONK_TEXTARRAY_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_Receiver.h"

#include "plonk_ObjectArray.h"
#include "plonk_ObjectArray2D.h"
#include "plonk_NumericalArray.h"
#include "plonk_NumericalArray2D.h"

/** @ingroup PlonkContainerClasses */
class TextArray : public ObjectArray2DBase<char, Text>
{
public:
    typedef ObjectArray<char>                   RowType;
    typedef ObjectArray2DBase<char,Text>        Base;

    typedef ObjectArrayInternal<Text>           Internal;
    
	TextArray (const int rows = 0) throw();
    explicit TextArray (Internal* internalToUse) throw();
    static const TextArray& getNull() throw();
    
    TextArray (TextArray const& copy) throw(); 
    TextArray (Dynamic const& other) throw();
    TextArray& operator= (TextArray const& other) throw();

    TextArray (ObjectArray<Text> const& other) throw();
	TextArray (ObjectArray<RowType> const& other) throw();
	TextArray (Text const& single) throw();
	
	TextArray (Text const &i00,
			   Text const &i01,
			   Text const &i02,
			   Text const &i03 = Text()) throw();
    
	TextArray (Text const &i00,
			   Text const &i01,
			   Text const &i02,
			   Text const &i03,
			   Text const &i04,
			   Text const &i05 = Text(),
			   Text const &i06 = Text(),
			   Text const &i07 = Text()) throw();
    
	TextArray (Text const &i00,
			   Text const &i01,
			   Text const &i02,
			   Text const &i03,
			   Text const &i04,
			   Text const &i05,
			   Text const &i06,
			   Text const &i07,
			   Text const &i08,
			   Text const &i09 = Text(),
			   Text const &i10 = Text(),
			   Text const &i11 = Text(),
			   Text const &i12 = Text(),
			   Text const &i13 = Text(),
			   Text const &i14 = Text(),
			   Text const &i15 = Text()) throw();
	
	TextArray (Text const &i00,
			   Text const &i01,
			   Text const &i02,
			   Text const &i03,
			   Text const &i04,
			   Text const &i05,
			   Text const &i06,
			   Text const &i07,
			   Text const &i08,
			   Text const &i09,
			   Text const &i10,
			   Text const &i11,
			   Text const &i12,
			   Text const &i13,
			   Text const &i14,
			   Text const &i15,
			   Text const &i16,
			   Text const &i17 = Text(),
			   Text const &i18 = Text(),
			   Text const &i19 = Text(),
			   Text const &i20 = Text(),
			   Text const &i21 = Text(),
			   Text const &i22 = Text(),
			   Text const &i23 = Text(),
			   Text const &i24 = Text(),
			   Text const &i25 = Text(),
			   Text const &i26 = Text(),
			   Text const &i27 = Text(),
			   Text const &i28 = Text(),
			   Text const &i29 = Text(),
			   Text const &i30 = Text(),
			   Text const &i31 = Text()) throw();
	
	ObjectArrayAssignmentDefinition(TextArray, Text);
	
	TextArray (Text const& array0, 
			   Text const& array1) throw();
	
	TextArray (TextArray const& array0, 
			   TextArray const& array1) throw();
	
	int indexOf (Text const& item, const int startIndex = 0) const throw();
	int indexOfIgnoreCase (Text const& item, const int startIndex = 0) const throw();
	
    Text asText() const throw();
    
	void print (const char *prefix = 0) const throw();
    
    PLONK_OBJECTARROWOPERATOR(TextArray);

};


#endif // PLONK_TEXTARRAY_H
