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

#ifndef PLONK_BINARYFILE_H
#define PLONK_BINARYFILE_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "../core/plonk_SmartPointerContainer.h"
#include "../containers/plonk_Text.h"


class BinaryFileInternal : public SmartPointer
{
public:
    BinaryFileInternal() throw();
    BinaryFileInternal (Text const& path, 
                        const bool writable = false, 
                        const bool clearContents = false,
                        const bool bigEndian = false) throw();
    ~BinaryFileInternal();
    
    LongLong getPosition() const throw();
    void setPosition (const LongLong position) throw();
    
    void setEof() throw();
    bool isEof() const throw();
    
    bool isBigEndian() const throw();
    bool isLittleEndian() const throw();
    bool isNativeEndian() const throw();

    int read (void* data, const int maximumBytes) throw();
    void read (char& value) throw();
	void read (short& value) throw();
    void read (int& value) throw();
    void read (Int24& value) throw();
    void read (LongLong& value) throw();
    void read (unsigned char& value) throw();
    void read (unsigned short& value) throw();
    void read (unsigned int& value) throw();
    void read (UnsignedLongLong& value) throw();
	void read (float& value) throw();
	void read (double& value) throw();
    
    template<class NumericalType>
    int read (NumericalArray<NumericalType>& array) throw();
    
    void write (void* data, const int maximumBytes) throw();
	void write (const char value) throw();
	void write (const short value) throw();
    void write (const int value) throw();
    void write (const Int24 value) throw();
    void write (const LongLong value) throw();
    void write (const unsigned char value) throw();
    void write (const unsigned short value) throw();
    void write (const unsigned int value) throw();
    void write (const UnsignedLongLong value) throw();
	void write (const float value) throw();
	void write (const double value) throw();
        
    template<class NumericalType>
    void write (NumericalArray<NumericalType> const& array) throw();

private:
    inline PlankFileRef getPeerRef() { return static_cast<PlankFileRef> (&peer); }
    inline const PlankFileRef getPeerRef() const { return const_cast<const PlankFileRef> (&peer); }

    PlankFile peer;
};

template<class NumericalType>
int BinaryFileInternal::read (NumericalArray<NumericalType>& array) throw()
{
    int bytesRead;
    plonk_assert (array.length() > 0);
    
    ResultCode result = pl_File_Read (getPeerRef(), 
                                      reinterpret_cast<void*> (array.getArray()), 
                                      sizeof (NumericalType) * array.length()
                                      &bytesRead);
    
    plonk_assert (result == PlankResult_OK || result == PlankResult_FileEOF); 
    
    if (! isNativeEndian())
        Endian::swap (array);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif    
    
    return bytesRead;
}

template<class NumericalType>
void BinaryFileInternal::write (NumericalArray<NumericalType> const& array) throw()
{
    const int length = array.length();
    
    if (length > 0)
    {        
        if (isNativeEndian())
        {
            ResultCode result = pl_File_Write (getPeerRef(), 
                                               reinterpret_cast<const void*> (array.getArray()), 
                                               sizeof (NumericalType) * length);
            
            plonk_assert (result == PlankResult_OK);
            
#ifndef PLONK_DEBUG
            (void)result;
#endif
        }
        else
        {
            const NumericalType* rawArray = array.getArray();
            
            for (int i = 0; i < length; ++i)
                write (rawArray[i]);
        }        
    }
}


/** A utility class for reading and writing binary files. 
 @see TextFile 
 @ingroup PlonkOtherUserClasses */
class BinaryFile : public SmartPointerContainer<BinaryFileInternal>
{
public:
    typedef BinaryFileInternal                      Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<BinaryFile>        Weak;
    
    /** Creates a null object. 
     This can't be used for reading or writing. */
    BinaryFile() throw()
    :   Base (new Internal())
    {
    }
    
    /** Creates a binary file reader from the given path. 
     @param path        The path of the file to read or write (this must already exist). 
     @param bigEndian   If @c true this will create a file where the multi-byte numerical
                        values are written in big endian format otherwise it will use 
                        little endian (i.e., the default on most platforms). This argument
                        defaults to the default endianness of runtime environment. */
	BinaryFile (Text const& path, const bool bigEndian = PLANK_BIGENDIAN) throw()
	:	Base (new Internal (path, false, false, bigEndian))
	{
	}
    
    /** Creates a binary file reader from the given path. 
     @param path            The path of the file to read or write (this must already exist). 
     @param writable        If @c true this creates a readable and writable file, otherwise it is a read-only file. 
     @param clearContents   If @c false then an exisiting file's contents will be retained AND the write operations
                            will always be appended to the end of the file. If @c true the file's contents will be erased and
                            write operations can be repositioned. It is safer to update an existing file by using a temporary
                            file and copy the contents when all the changes are complete. 
                            This must be @c false if writable is also @c false.
     @param bigEndian       If @c true this will create a file where the multi-byte numerical
                            values are written in big endian format otherwise it will use 
                            little endian (i.e., the default on most platforms). This argument
                            defaults to the default endianness of runtime environment. */    
    BinaryFile (Text const& path, const bool writable, const bool clearContents, const bool bigEndian = PLANK_BIGENDIAN) throw()
	:	Base (new Internal (path, writable, clearContents, bigEndian))
	{
	}        
    
    /** Creates a binary file reader from the given path. 
     @param path        The path of the file to read or write (this must already exist). 
     @param bigEndian   If @c true this will create a file where the multi-byte numerical
                        values are written in big endian format otherwise it will use 
                        little endian (i.e., the default on most platforms). This argument
                        defaults to the default endianness of runtime environment. */    
    BinaryFile (const char* path, const bool bigEndian = PLANK_BIGENDIAN) throw()
	:	Base (new Internal (path, false, false, bigEndian))
	{
	}
	
    /** Creates a binary file reader from the given path. 
     @param path            The path of the file to read or write (this must already exist). 
     @param writable        If @c true this creates a readable and writable file, otherwise it is a read-only file. 
     @param clearContents   If @c false then an exisiting file's contents will be retained AND the write operations
                            will always be appended to the end of the file. If @c true the file's contents will be erased and
                            write operations can be repositioned. It is safer to update an existing file by using a temporary
                            file and copy the contents when all the changes are complete. 
                            This must be @c false if writable is also @c false.
     @param bigEndian       If @c true this will create a file where the multi-byte numerical
                            values are written in big endian format otherwise it will use 
                            little endian (i.e., the default on most platforms). This argument
                            defaults to the default endianness of runtime environment. */
    BinaryFile (const char* path, const bool writable, const bool clearContents, const bool bigEndian = PLANK_BIGENDIAN) throw()
	:	Base (new Internal (path, writable, clearContents, bigEndian))
	{
	}            
    
    /** @internal */
    explicit BinaryFile (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}    
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    BinaryFile (BinaryFile const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    /** Assignment operator. */
    BinaryFile& operator= (BinaryFile const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static BinaryFile fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    	
    /** Gets the position of the file stream. */
    LongLong getPosition() throw()
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
    
    /** Reads a numerical value from the file.
     This must be one of: char, short, int, long or LongLong 
     (and their unsigned versions). This is read in the endian format
     specified in the constructor. */    
    template<class ValueType>
    ValueType read() throw()
    {
        ValueType value;
        getInternal()->read (value);
        return value;
    }    
    
    /** Reads a value or array from the file.
     This must be one of: char, short, int, long or LongLong 
     (and their unsigned versions) or a NumericalArray of a built-in type. 
     This is read in the endian format specified in the constructor. */        
    template<class ValueType>
    void read (ValueType& value) throw()
    {
        getInternal()->read (value);
    }    
    
    /** Write a numerical value to the file.
     This must be one of: char, short, int, long or LongLong 
     (and their unsigned versions). This is written in the endian format
     specified in the constructor. */    
    template<class ValueType>
    void write (const ValueType value) throw()
    {
        getInternal()->write (value);
    }
	        
    /** Creates a chunk name identifier.
     A convenience function for creating an interger to write to a file
     for chunk IDs in many IFF-type files. */
    static inline int chunkID (const char* const fourCharCode) throw()
    {
        plonk_assert (Text (fourCharCode).length() == 4);
        return pl_FourCharCode (fourCharCode);
    }
    
    PLONK_OBJECTARROWOPERATOR(BinaryFile);

};



#endif // PLONK_BINARYFILE_H
