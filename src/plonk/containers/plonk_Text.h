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

#ifndef PLONK_TEXT_H
#define PLONK_TEXT_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_Receiver.h"

#include "plonk_ObjectArray.h"
#include "plonk_ObjectArray2D.h"
#include "plonk_NumericalArray.h"
#include "plonk_NumericalArray2D.h"



/** A class for storing and manipulating text strings.
 Similar to std:string this is plonk's default text format. 
 Internally Text is just a null terminated C string (array of chars) and is just
 a specialised versio of CharArray or NumericalArray<char>. 
 @ingroup PlonkContainerClasses */
class Text : public CharArray
{
public:
    typedef ObjectArrayInternal<char>   Internal;
    typedef CharArray                   Base;    
    typedef WeakPointerContainer<Text>  Weak;
    
	static const char space;
	
	/** Creates an emply text string. */
	Text() throw();
	    
    explicit Text (Internal* internalToUse) throw();
    
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */
    static Text fromWeak (Weak const& weak) throw();
    static const Text& getNull() throw();
    static const Text& getEmpty() throw();
    
    
	/** Creates a text string containing a single character. */
	Text (const char c) throw();
    
    Text (const int value) throw();
    Text (const LongLong value) throw();
    Text (const float value) throw();
    Text (const double value) throw();
    
    Text (Text const& copy) throw();
    Text (Dynamic const& other) throw();
    Text& operator= (Text const& other) throw();

        
	/** Copy numerical values from another numerical array type to a new Text string. */
	template<class CopyType>
	Text (NumericalArray<CopyType> const& copy) throw()
	:	Base (copy)
	{
	}
	
	/** Copy values from another array type to a new Text string. */
	template<class CopyType>
	Text (ObjectArray<CopyType> const& copy) throw()
	:	Base (copy)
	{
	}
	
	/** Create a Text string from a wide char string. */
	Text (const wchar_t* string) throw();
	
	ObjectArrayAssignmentDefinition(Text, char);
	
	/** Concatentate two Text strings into one. */
	Text (Text const& array0, Text const& array1) throw();
		
	Text (NumericalArraySpec const& spec) throw();
	Text (NumericalArraySpec const& spec, const bool needsNullTermination) throw();
	static Text withSize (const int size, const bool zeroData = false) throw();	
	static Text newClear (const int size) throw();
	static Text fromValue (const int num) throw();
	static Text fromValue (const LongLong num) throw();
	static Text fromValue (const float num) throw();
	static Text fromValue (const double num) throw();

    Text& operator= (Base const& other) throw();
	Text& operator= (const char* other) throw();
	Text& operator= (const wchar_t* other) throw();
	
	Text operator+ (Text const& rightOperand) const throw();
	Text operator+= (Text const& rightOperand) throw();

	Text offset (Text const& rightOperand) const throw();
	Text& add (Text const& other) throw();
	
	Text (const char* nullTerminatedSourceArray);
	Text (const int size, const char* sourceArray, const bool needsNullTermination = false);
		
	Text toUpper() const throw();
	Text toLower() const throw();
	
	bool equalsIgnoreCase (Text const& other) const throw();
	
	bool containsIgnoreCase (Text const& toFind) const throw();	
	int indexOfIgnoreCase (Text const& toFind, const int startIndex = 0) const throw();
	int indexOfAnyIgnoreCase (Text const& toFind, const int startIndex = 0) const throw();
	Text replaceIgnoreCase (Text const& find, Text const& substitute) const throw();
	
	void print (const char *prefix = 0) const throw();
	
	static const Text& getPrintable() throw();
	static const Text& getAlphabetLower() throw();
	static const Text& getAlphabetUpper() throw();
	static const Text& getAlphabetAll() throw();
	static const Text& getDigits() throw();
	static const Text& getAlphabetAndDigits() throw();
	static const Text& getPunctuation() throw();
	static const Text& getStandardDelimiters() throw();
    
    static const Text& getMessageDone() throw();
    static const Text& getMessageLooped() throw();
    static const Text& getMessagePatchStart() throw();
    static const Text& getMessagePatchEnd() throw();

    
    PLONK_OBJECTARROWOPERATOR(Text);
};

Text operator+ (const char* text1, Text const& text2) throw();
Text operator+ (const wchar_t* text1, Text const& text2) throw();


#endif // PLONK_TEXT_H
