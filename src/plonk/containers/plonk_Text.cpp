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

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"


const char Text::space = ' ';

Text::Text() throw()
:	Base (NumericalArraySpec (1, true), true)
{
}

Text::Text (Internal* internalToUse) throw() 
:	Base (internalToUse)
{
}         

Text Text::fromWeak (Weak const& weak) throw()
{
    return weak.fromWeak();
}            

const Text& Text::getNull() throw()
{
    static Text null;
    return null;
}	

const Text& Text::getEmpty() throw()
{
    static Text empty ("");
    return empty;
}

Text::Text (const char c) throw()
:	Base (NumericalArraySpec((c != 0) ? 2 : 1, true), true)
{
    if (c != 0)
        this->put (0, c);
}

Text::Text (const int value) throw()
{
    operator= (Text::fromValue (value));
}

Text::Text (const LongLong value) throw()
{
    operator= (Text::fromValue (value));
}

Text::Text (const float value) throw()
{
    operator= (Text::fromValue (value));
}

Text::Text (const double value) throw()
{
    operator= (Text::fromValue (value));
}

Text::Text (Text const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}

Text::Text (Dynamic const& other) throw()
:   Base (other.as<Text>().getInternal())
{
}    

Text::Text (const wchar_t* string) throw()
:	Base (NumericalArray<wchar_t> (string))
{
}

Text::Text (Text const& array0, Text const& array1) throw()
:	Base (array0, array1)
{
}

Text::Text (NumericalArraySpec const& spec) throw()
:	Base (spec, true)
{
	if (spec.zeroData_)
		zero();
}

Text::Text (NumericalArraySpec const& spec, const bool needsNullTermination) throw()
:	Base (spec, needsNullTermination)
{
	if (spec.zeroData_)
		zero();
}

Text Text::withSize (const int size, const bool zeroData) throw()
{
	return Text (NumericalArraySpec (size, zeroData));
}

Text Text::newClear (const int size) throw()
{
	return Text (NumericalArraySpec (size, true));
}
 
Text Text::fromValue (const int num) throw()
{
	const int size = 64;
	char buf[size];
	
	snprintf (buf, size, "%d", num);
	
	return buf;
}

Text Text::fromValue (const LongLong num) throw()
{
	const int size = 64;
	char buf[size];
	
	snprintf (buf, size, "%lld", num);
	
	return buf;
}

Text Text::fromValue (const float num) throw()
{
	const int size = 64;
	char buf[size];
	
	snprintf (buf, size, "%f", num);
	
	return buf;
}

Text Text::fromValue (const double num) throw()
{
	const int size = 64;
	char buf[size];
	
	snprintf (buf, size, "%f", num);
	
	return buf;
}

Text& Text::operator= (Text const& other) throw()
{
    if (this != &other)
        this->setInternal (other.getInternal());
    
    return *this;
}

Text& Text::operator= (Base const& other) throw()
{
	return operator= (static_cast<Text const&> (other) );
}	

Text& Text::operator= (const char* other) throw()
{
	return operator= (Text (other));
}

Text& Text::operator= (const wchar_t* other) throw()
{
	return operator= (Text (other));
}

Text Text::operator+ (Text const& rightOperand) const throw()
{
	return Text (*this, rightOperand);
}

Text Text::operator+= (Text const& rightOperand) throw()
{
	return operator= (Text (*this, rightOperand));
}

Text operator+ (const char* text1, Text const& text2) throw()
{
	return Text (text1) + text2;
}

Text operator+ (const wchar_t* text1, Text const& text2) throw()
{
	return Text (text1) + text2;
}

Text Text::offset (Text const& rightOperand) const throw()
{
	CharArray left = *this;
	CharArray right = rightOperand;
	return left + right;
}

Text& Text::add (Text const& other) throw()
{
    this->getInternal()->add (other.length(), other.getArray());
	return *this;
}

Text::Text (const char* nullTerminatedSourceArray)
:	Base (nullTerminatedSourceArray)
{
}

Text::Text (const int size, 
            const char* sourceArray, 
            const bool needsNullTermination)
:	Base (Base::withArray (size, sourceArray, needsNullTermination))
{
}																								

Text Text::toUpper() const throw()
{
	const char *thisArray = this->getArray();
	const int size = this->size();
	
	if (thisArray == 0 || size <= 0) 
        return *this;
	
	Text newText = Text::withSize (size, false);
	char *newArray = newText.getArray();		
	
	for (int i = 0; i < size; ++i)
	{
		char c = thisArray[i];
		
		if (c >= 'a' && c <= 'z')
			c -= 32;
		
		newArray[i] = c;
	}			
	
	return newText;
}

Text Text::toLower() const throw()
{
	const char *thisArray = this->getArray();
	const int size = this->size();
	
	if (thisArray == 0 || size <= 0) 
        return *this;
	
	Text newText = Text::withSize (size, false);
	char *newArray = newText.getArray();		
	
	for (int i = 0; i < size; ++i)
	{
		char c = thisArray[i];
		
		if (c >= 'A' && c <= 'Z')
			c += 32;
		
		newArray[i] = c;
	}			
	
	return newText;	
}

bool Text::equalsIgnoreCase (Text const& other) const throw()
{
	return this->toLower() == other.toLower();
}

bool Text::containsIgnoreCase (Text const& toFind) const throw()
{
	return this->toLower().contains (toFind.toLower());
}

int Text::indexOfIgnoreCase (Text const& toFind, const int startIndex) const throw()
{
	return this->toLower().indexOf (toFind.toLower(), startIndex);
}

int Text::indexOfAnyIgnoreCase (Text const& toFind, const int startIndex) const throw()
{
	return this->toLower().indexOfAny (toFind.toLower(), startIndex);
}

Text Text::replaceIgnoreCase (Text const& find, Text const& substitute) const throw()
{
	Text thisLower = this->toLower();
	Text findLower = find.toLower();
	const int size = this->size();
	const int findLength = find.length();
	int startIndex = 0;
	int findIndex;
	
	Text result;
	
	while ((findIndex = thisLower.indexOf (findLower, startIndex)) >= 0)
	{
		result <<= this->range (startIndex, findIndex);
		result <<= substitute;
		startIndex = findIndex + findLength;
	}
	
	result <<= this->range (startIndex, size);
	
	return result;
}

void Text::print (const char *prefix) const throw()
{
	if (prefix)
		printf("%s: ", prefix);
	
	printf ("%s\n", this->getArray());
}

const Text& Text::getPrintable() throw()
{
	static const Text t = Text::series (128 - 32 - 1, 32, 1);
	return t;
}

const Text& Text::getAlphabetLower() throw()
{
	static const Text t = "abcdefghijklmnopqrstuvwxyz";
	return t;
}

const Text& Text::getAlphabetUpper() throw()
{
	static const Text t = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	return t;
}

const Text& Text::getAlphabetAll() throw()
{
	static const Text t = Text::getAlphabetUpper() << Text::getAlphabetLower();
	return t;
}

const Text& Text::getDigits() throw()
{
	static const Text t = "0123456789";
	return t;
}

const Text& Text::getAlphabetAndDigits() throw()
{
	static const Text t = Text::getAlphabetAll() << Text::getDigits();
	return t;
}

const Text& Text::getPunctuation() throw()
{
	static const Text t 
        = Text::getPrintable().copy().removeItems (Text::getAlphabetAndDigits()).removeItem (Text::space);
	return t;
}

const Text& Text::getStandardDelimiters() throw()
{
	static const Text t = " :;,.\t";
	return t;
}

END_PLONK_NAMESPACE
