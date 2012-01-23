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

#include "../core/plonk_StandardHeader.h"
//#include <cstdarg>

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"



TextArray::TextArray (const int rows) throw()
:	Base (rows < 0 ? 0 : rows)
{
}

TextArray::TextArray (Internal* internalToUse) throw() 
:	Base (internalToUse)
{
}

const TextArray& TextArray::getNull() throw()
{
    static TextArray null;
    return null;
}	

TextArray::TextArray (TextArray const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}

TextArray::TextArray (Dynamic const& other) throw()
:   Base (other.as<TextArray>().getInternal())
{
} 

TextArray& TextArray::operator= (TextArray const& other) throw()
{
    if (this != &other)
        this->setInternal (other.containerCopy().getInternal());
    
    return *this;
}

TextArray::TextArray (ObjectArray<Text> const& other) throw()
:	Base (other.size())
{
    const int rows = this->size();
    
    for (int row = 0; row < rows; ++row)
        this->put (row, other[row]);
}    

TextArray::TextArray (ObjectArray<RowType> const& other) throw()
:	Base (other.size())
{
    const int rows = this->size();
    
    for (int row = 0; row < rows; ++row)
        this->put (row, Text (other[row]));
}

TextArray::TextArray (Text const& single) throw()
:	Base (single)
{
}

TextArray::TextArray (Text const &i00,
                      Text const &i01,
                      Text const &i02,
                      Text const &i03) throw()
:	Base (i00, i01, i02, i03) 
{
}

TextArray::TextArray (Text const &i00,
                      Text const &i01,
                      Text const &i02,
                      Text const &i03,
                      Text const &i04,
                      Text const &i05,
                      Text const &i06,
                      Text const &i07) throw()
:   Base (i00, i01, i02, i03, i04, i05, i06, i07)
{
}

TextArray::TextArray (Text const &i00,
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
                      Text const &i15) throw()
:	Base (i00, i01, i02, i03, i04, i05, i06, i07,
          i08, i09, i10, i11, i12, i13, i14, i15)
{
}

TextArray::TextArray (Text const &i00,
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
                      Text const &i17,
                      Text const &i18,
                      Text const &i19,
                      Text const &i20,
                      Text const &i21,
                      Text const &i22,
                      Text const &i23,
                      Text const &i24,
                      Text const &i25,
                      Text const &i26,
                      Text const &i27,
                      Text const &i28,
                      Text const &i29,
                      Text const &i30,
                      Text const &i31) throw()
:	Base(i00, i01, i02, i03, i04, i05, i06, i07,
         i08, i09, i10, i11, i12, i13, i14, i15, 
         i16, i17, i18, i19, i20, i21, i22, i23,
         i24, i25, i26, i27, i28, i29, i30, i31)
{
}

TextArray::TextArray (Text const& array0, 
                      Text const& array1) throw()
:	Base (array0, array1)
{
}

TextArray::TextArray (TextArray const& array0, 
                      TextArray const& array1) throw()
:	Base (array0, array1)
{
}

int TextArray::indexOf(Text const& item, const int startIndex) const throw()
{
	plonk_assert (startIndex >= 0);
	
	const Text *array = this->getArray();
	
	if (array != 0)
	{
		const int length = this->length();
		for (int i = startIndex < 0 ? 0 : startIndex; i < length; ++i)
		{
			if (array[i] == item)
				return i;
		}
	}
	
	return -1;
}

int TextArray::indexOfIgnoreCase (Text const& item, const int startIndex) const throw()
{
	plonk_assert (startIndex >= 0);
	
	const Text *array = this->getArray();
	
	if (array != 0)
	{
		const int length = this->length();
		for (int i = startIndex < 0 ? 0 : startIndex; i < length; ++i)
		{
			if (array[i].equalsIgnoreCase(item))
				return i;
		}
	}
	
	return -1;
}

void TextArray::print (const char *prefix) const throw()
{
	const Text *textArray = this->getArray();
	const int size = this->size();
	for (int i = 0; i < size; ++i)
	{
		if (prefix)
			printf("%s[%d]:", prefix, i);
		else
			printf("[%d]:", i);
		
		printf("%s\n", (const char*)textArray[i]);
	}
}

END_PLONK_NAMESPACE
