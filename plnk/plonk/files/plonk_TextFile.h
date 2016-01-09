/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_TEXTFILE_H
#define PLONK_TEXTFILE_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_FilesForwardDeclarations.h"
#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "../core/plonk_SmartPointerContainer.h"
#include "../containers/plonk_Text.h"


class TextFileInternal : public SmartPointer
{
public:
    TextFileInternal() throw();
    TextFileInternal (Text const& path, 
                      const bool writable = false, 
                      const bool clearContents = false) throw();
    TextFileInternal (FilePathArray const& fileArray, const int multiMode) throw();
    TextFileInternal (TextFileQueue const& fileQueue) throw();

    ~TextFileInternal();
    
    LongLong getPosition() const throw();
    void setPosition (const LongLong position) throw();
    
    void setEof() throw();
    bool isEof() const throw();
    bool canRead() const throw();
    bool canWrite() const throw();

    Text readLine (const int maximumLength = 1024) throw();
    Text readAll() throw();
    Text read (const int numBytes) throw();
    
	void writeValue (const char value) throw();
	void writeValue (const short value) throw();
    void writeValue (const int value) throw();
	void writeValue (const long value) throw();
    void writeValue (const LongLong value) throw();
    void writeValue (const unsigned char value) throw();
    void writeValue (const unsigned short value) throw();
    void writeValue (const unsigned int value) throw();
	void writeValue (const unsigned long value) throw();
    void writeValue (const UnsignedLongLong value) throw();
	void writeValue (const float value) throw();
	void writeValue (const double value) throw();
	void write (const char* text) throw();
	void writeLine (const char* text) throw();
    
    void disownPeer (PlankFileRef otherFile) throw();

private:
    PLONK_INLINE_LOW PlankFileRef getPeerRef() { return static_cast<PlankFileRef> (&peer); }
    PLONK_INLINE_LOW const PlankFileRef getPeerRef() const { return const_cast<const PlankFileRef> (&peer); }
    
    PlankFile peer;
};

/** A utility class for reading and writing text files. 
 @see BinaryFile 
 @ingroup PlonkOtherUserClasses */
class TextFile : public SmartPointerContainer<TextFileInternal>
{
public:
    typedef TextFileInternal                        Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<TextFile>          Weak;
    
    enum MultiFileTypes
    {
        MultiFileUnknown = PLANKMULITFILE_MODE_UNKNOWN,
        MultiFileArraySequenceOnce = PLANKMULITFILE_MODE_ARRAYSEQUENCEONCE,
        MultiFileArraySequenceLoop = PLANKMULITFILE_MODE_ARRAYSEQUENCELOOP,
        MultiFileArrayRandom = PLANKMULITFILE_MODE_ARRAYRANDOM,
        MultiFileArrayRandomNoRepeat = PLANKMULITFILE_MODE_ARRAYRANDOMNOREPEAT,
        MultiFileArrayIndexRef = PLANKMULITFILE_MODE_ARRAYINDEXREF,
        MultiFileQueue = PLANKMULITFILE_MODE_QUEUE,
        MultiFileCustom = PLANKMULITFILE_MODE_CUSTOM
    };
    
    /** Creates a null object. 
     This can't be used for reading or writing. */
    TextFile() throw()
    :   Base (new Internal())
    {
    }
    
    /** Creates a text file reader from the given path. 
     @param path The path of the file to read or write (this must already exist). */
	TextFile (Text const& path) throw()
	:	Base (new Internal (path))
	{
	}
    
    /** Creates a text file reader or writer from the given path. 
     @param path            The path of the file to read or write. If writable is @c false then this file must exist. 
     @param writable        If @c true this creates a readable and writable file, otherwise it is a read-only file. 
     @param clearContents   If @c false then an exisiting file's contents will be retained AND the write operations
                            will always be appended to the end of the file. If @c true the file's contents will be erased and
                            write operations can be repositioned. It is safer to update an existing file by using a temporary
                            file and copy the contents when all the changes are complete. 
                            This must be @c false if writable is also @c false. */
    TextFile (Text const& path, const bool writable, const bool clearContents = false) throw()
	:	Base (new Internal (path, writable, clearContents))
	{
	}    
    
    /** Creates a text file reader from the given path. */
    TextFile (const char* path) throw()
	:	Base (new Internal (path))
	{
	}
    
    /** Creates a text file reader or writer from the given path. 
     @param path            The path of the file to read or write. If writable is @c false then this file must exist. 
     @param writable        If @c true this creates a readable and writable file, otherwise it is a read-only file. 
     @param clearContents   If @c false then an exisiting file's contents will be retained AND the write operations
                            will always be appended to the end of the file. If @c true the file's contents will be erased and
                            write operations can be repositioned. It is safer to update an existing file by using a temporary
                            file and copy the contents when all the changes are complete. 
                            This must be @c false if writable is also @c false. */    
    TextFile (const char* path, const bool writable, const bool clearContents = false) throw()
	:	Base (new Internal (path, writable, clearContents))
	{
	}
    
    /** Creates a multiple text file reader from the array of files.
     */
    TextFile (FilePathArray const& fileArray, const int multiMode) throw()
    :   Base (new Internal (fileArray, multiMode))
    {
    }
    
    /** Creates a multiple text file reader with a queue of files.
     */
    TextFile (TextFileQueue const& fileQueue) throw()
    :   Base (new Internal (fileQueue))
    {
    }
	
    /** @internal */
    explicit TextFile (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}    
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    TextFile (TextFile const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    /** Assignment operator. */
    TextFile& operator= (TextFile const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static TextFile fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Attempts to read a line of text from the file.
     This is returned as Text and the file stream is repositioned. */
	Text readLine (const int maximumLength = 1024) throw()
	{
        return getInternal()->readLine (maximumLength);
	}
	
	Text readAll() throw()
	{
		return getInternal()->readAll();
	}
    
    Text read (const int numBytes) throw()
	{
		return getInternal()->read (numBytes);
	}
	
    /** Gets the position of the file stream. */
    LongLong getPosition() const throw()
    {
        return getInternal()->getPosition();
    }
    
    /** Sets the position of the file stream. 
     0 is the start of the stream. */
    void setPosition (const LongLong position) throw()
    {
        getInternal()->setPosition (position);
    }

    /** Sets the position of the file stream to the end of the file. */
    void setEof() throw()
    {
        getInternal()->setEof();
    }    
    
    /** Determines if the file stream is positioned at its end. */
	bool isEof() const throw()
	{
		return getInternal()->isEof();
	}
    
    /** Write a numerical value as text to the file.
     This must be one of: char, short, int, long or LongLong 
     (and their unsigned versions). */
    template<class ValueType>
    void writeValue (const ValueType value) throw()
    {
        getInternal()->writeValue (value);
    }
	
    /** Write a string of text to the file. */
	void write (const char* text) throw()
    {
        getInternal()->write (text);
    }
    
    /** Write a string of text to the file. */
    void write (Text const& text) throw()
    {
        getInternal()->write (text.getArray());
    }    
    
    /** Write a string of text to the file and move to a new line. */
	void writeLine (const char* text) throw()
    {
        getInternal()->writeLine (text);
    }
    
    /** Write a string of text to the file and move to a new line. */
    void writeLine (Text const& text) throw()
    {
        getInternal()->writeLine (text.getArray());
    }    
    
    void disownPeer (PlankFileRef otherFile) throw()
    {
        getInternal()->disownPeer (otherFile);
    }
    
    PLONK_OBJECTARROWOPERATOR(TextFile);

};



#endif // PLONK_TEXTFILE_H
