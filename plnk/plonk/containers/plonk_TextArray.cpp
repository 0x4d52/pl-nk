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

#include "../core/plonk_StandardHeader.h"

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
        this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
    
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

static int countStrings (Text const &i00,
                         Text const &i01,
                         Text const &i02,
                         Text const &i03) throw()
{
    int size = 0;
    
    if (i00.length() > 0) size++; else return size;
    if (i01.length() > 0) size++; else return size;
    if (i02.length() > 0) size++; else return size;
    if (i03.length() > 0) size++; else return size;
    
    return size;
}

TextArray::TextArray (Text const &i00,
                      Text const &i01,
                      Text const &i02,
                      Text const &i03) throw()
:	Base (countStrings (i00, i01, i02, i03), false)
{
    Text *thisArray = this->getArray();
    
    if (i00.length() > 0) thisArray[ 0] = i00; else return;
    if (i01.length() > 0) thisArray[ 1] = i01; else return;
    if (i02.length() > 0) thisArray[ 2] = i02; else return;
    if (i03.length() > 0) thisArray[ 3] = i03; else return;
}

static int countStrings (Text const &i00,
                         Text const &i01,
                         Text const &i02,
                         Text const &i03,
                         Text const &i04,
                         Text const &i05,
                         Text const &i06,
                         Text const &i07) throw()
{
    int size = 0;
    
    if (i00.length() > 0) size++; else return size;
    if (i01.length() > 0) size++; else return size;
    if (i02.length() > 0) size++; else return size;
    if (i03.length() > 0) size++; else return size;
    if (i04.length() > 0) size++; else return size;
    if (i05.length() > 0) size++; else return size;
    if (i06.length() > 0) size++; else return size;
    if (i07.length() > 0) size++; else return size;
    
    return size;
}

TextArray::TextArray (Text const &i00,
                      Text const &i01,
                      Text const &i02,
                      Text const &i03,
                      Text const &i04,
                      Text const &i05,
                      Text const &i06,
                      Text const &i07) throw()
:   Base (countStrings (i00, i01, i02, i03, i04, i05, i06, i07), false)
{
    Text *thisArray = this->getArray();
    
    if (i00.length() > 0) thisArray[ 0] = i00; else return;
    if (i01.length() > 0) thisArray[ 1] = i01; else return;
    if (i02.length() > 0) thisArray[ 2] = i02; else return;
    if (i03.length() > 0) thisArray[ 3] = i03; else return;
    if (i04.length() > 0) thisArray[ 4] = i04; else return;
    if (i05.length() > 0) thisArray[ 5] = i05; else return;
    if (i06.length() > 0) thisArray[ 6] = i06; else return;
    if (i07.length() > 0) thisArray[ 7] = i07; else return;
}

static int countStrings (Text const &i00,
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
{
    int size = 0;
    
    if (i00.length() > 0) size++; else return size;
    if (i01.length() > 0) size++; else return size;
    if (i02.length() > 0) size++; else return size;
    if (i03.length() > 0) size++; else return size;
    if (i04.length() > 0) size++; else return size;
    if (i05.length() > 0) size++; else return size;
    if (i06.length() > 0) size++; else return size;
    if (i07.length() > 0) size++; else return size;
    if (i08.length() > 0) size++; else return size;
    if (i09.length() > 0) size++; else return size;
    if (i10.length() > 0) size++; else return size;
    if (i11.length() > 0) size++; else return size;
    if (i12.length() > 0) size++; else return size;
    if (i13.length() > 0) size++; else return size;
    if (i14.length() > 0) size++; else return size;
    if (i15.length() > 0) size++; else return size;
    
    return size;
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
:	Base (countStrings (i00, i01, i02, i03, i04, i05, i06, i07,
                        i08, i09, i10, i11, i12, i13, i14, i15), false)
{
    Text *thisArray = this->getArray();
    
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

static int countStrings (Text const &i00,
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
{
    int size = 0;
    
    if (i00.length() > 0) size++; else return size;
    if (i01.length() > 0) size++; else return size;
    if (i02.length() > 0) size++; else return size;
    if (i03.length() > 0) size++; else return size;
    if (i04.length() > 0) size++; else return size;
    if (i05.length() > 0) size++; else return size;
    if (i06.length() > 0) size++; else return size;
    if (i07.length() > 0) size++; else return size;
    if (i08.length() > 0) size++; else return size;
    if (i09.length() > 0) size++; else return size;
    if (i10.length() > 0) size++; else return size;
    if (i11.length() > 0) size++; else return size;
    if (i12.length() > 0) size++; else return size;
    if (i13.length() > 0) size++; else return size;
    if (i14.length() > 0) size++; else return size;
    if (i15.length() > 0) size++; else return size;
    if (i16.length() > 0) size++; else return size;
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
:	Base (countStrings (i00, i01, i02, i03, i04, i05, i06, i07,
                        i08, i09, i10, i11, i12, i13, i14, i15,
                        i16, i17, i18, i19, i20, i21, i22, i23,
                        i24, i25, i26, i27, i28, i29, i30, i31), false)
{
    Text *thisArray = this->getArray();
    
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

Text TextArray::asText() const throw()
{
    Text result = "[\"";
    
    const int length = this->length();
    
    for (int i = 0; i < length - 1; ++i)
    {
        result.add (this->at (i));
        result.add ("\", ");
    }
    
    result.add (this->at (length - 1));
    result.add ("\"]");
    
    return result;
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
