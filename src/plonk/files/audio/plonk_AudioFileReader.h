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

#ifndef PLONK_AUDIOFILEREADER_H
#define PLONK_AUDIOFILEREADER_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../core/plonk_SmartPointer.h"
#include "../../core/plonk_WeakPointer.h"
#include "../../core/plonk_SmartPointerContainer.h"
#include "../../containers/plonk_Text.h"


class AudioFileReaderInternal : public SmartPointer
{
public:
    AudioFileReaderInternal() throw();
    AudioFileReaderInternal (Text const& path, const int bufferSize = 32768) throw();
    ~AudioFileReaderInternal();
    
    int getBitsPerSample() throw();
    int getBytesPerFrame() throw();
    int getNumChannels() throw();
    double getSampleRate() throw();
    int getNumFrames() throw();
    int getFramePosition() throw();
    void setFramePosition (const int position) throw();
    void resetFramePosition() throw();
    
    template<class SampleType>
    void readFrames (NumericalArray<SampleType>& data) throw();
    
private:
    inline PlankAudioFileReaderRef getPeerRef() { return static_cast<PlankAudioFileReaderRef> (&peer); }
    inline const PlankAudioFileReaderRef getPeerRef() const { return const_cast<const PlankAudioFileReaderRef> (&peer); }

    PlankAudioFileReader peer;
    Chars readBuffer;
    int numFramesPerBuffer;
};

template<class SampleType>
void AudioFileReaderInternal::readFrames (NumericalArray<SampleType>& data) throw()
{
    // this needs to do many more checks...
    
    ResultCode result;
    
    typedef NumericalArray<SampleType> SampleArray;
    
    const int dataLength = data.length();
    
    SampleType* dataArray = data.getArray();
    void* const readBufferArray = readBuffer.getArray();
    
    int encoding, bits, channels, bytesPerFrame, bytesPerSample;;
    result = pl_AudioFileReader_GetEncoding (getPeerRef(), &encoding);
    plonk_assert (result == PlankResult_OK);
    
    result = pl_AudioFileReader_GetBitsPerSample (getPeerRef(), &bits);
    plonk_assert (result == PlankResult_OK);
    
    result = pl_AudioFileReader_GetNumChannels (getPeerRef(), &channels);
    plonk_assert (result == PlankResult_OK);

    result = pl_AudioFileReader_GetBytesPerFrame (getPeerRef(), &bytesPerFrame);
    plonk_assert (result == PlankResult_OK);
    
    plonk_assert (bits > 0);
    plonk_assert (channels > 0);
    plonk_assert (bytesPerFrame > 0);
    
    bytesPerSample = bytesPerFrame / channels;
    
    int dataIndex = 0;
    
    while (dataIndex < dataLength)
    {
        result = pl_AudioFileReader_ReadFrames (getPeerRef(), numFramesPerBuffer, readBufferArray);
        plonk_assert (result == PlankResult_OK);

        if (encoding & PLANKAUDIOFILE_ENCODING_PCM_FLAG)
        {            
            if (bytesPerSample == 16)
            {
                Short* const convertBuffer = static_cast<Short*> (readBufferArray); 
            }
            else if (bytesPerSample == 24)
            {
                Int24* const convertBuffer = static_cast<Int24*> (readBufferArray); 
            }
            else if (bytesPerSample == 32)
            {
                Int* const convertBuffer = static_cast<Int*> (readBufferArray); 
            }
            else if (bytesPerSample == 8)
            {
                Char* const convertBuffer = static_cast<Char*> (readBufferArray); 
            }
            else
            {
                plonk_assertfalse;
            }
        }
        else if (encoding & PLANKAUDIOFILE_ENCODING_FLOAT_FLAG)
        {
            if (bytesPerSample == 32)
            {
                Float* const convertBuffer = static_cast<Float*> (readBufferArray); 
            }
            else if (bytesPerSample == 64)
            {
                Double* const convertBuffer = static_cast<Double*> (readBufferArray); 
            }
            else
            {
                plonk_assertfalse;
            }
        }
        
#if PLONK_LITTLEENDIAN
        const bool shouldSwapEndian = (encoding & PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG);
#elif PLONK_BIGENDIAN
        const bool shouldSwapEndian = ! (encoding & PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG);
#endif
        
        if (shouldSwapEndian)
        {
            
        }
    }
   
#ifndef PLONK_DEBUG
    (void)result;
#endif
}



/** Audio file reader. 
 @see BinaryFile 
 @ingroup PlonkOtherUserClasses */
class AudioFileReader : public SmartPointerContainer<AudioFileReaderInternal>
{
public:
    typedef AudioFileReaderInternal                 Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<AudioFileReader>   Weak;
    
    /** Creates a null object. 
     This can't be used for reading or writing. */
    AudioFileReader() throw()
    :   Base (new Internal())
    {
    }
    
    /** Creates a binary file reader from the given path. 
     @param path        The path of the file to read.  */
	AudioFileReader (Text const& path) throw()
	:	Base (new Internal (path))
	{
	}
           
    /** @internal */
    explicit AudioFileReader (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}    
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    AudioFileReader (AudioFileReader const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    /** Assignment operator. */
    AudioFileReader& operator= (AudioFileReader const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static AudioFileReader fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    	
//    /** Gets the position of the file stream. */
//    LongLong getPosition() throw()
//    {
//        return getInternal()->getPosition();
//    }
//    
//    /** Sets the position of the file stream. 
//     0 is the start of the stream. */
//    void setPosition (const LongLong position) throw()
//    {
//        getInternal()->setPosition (position);
//    }
//
//    /** Sets the position of the file stream to the end of the file. */
//    void setEof() throw()
//    {
//        getInternal()->setEof();
//    }    
//    
//    /** Determines if the file stream is positioned at its end. */
//	bool isEof() const throw()
//	{
//		return getInternal()->isEof();
//	}
//    
//    /** Reads a numerical value from the file.
//     This must be one of: char, short, int, long or LongLong 
//     (and their unsigned versions). This is read in the endian format
//     specified in the constructor. */    
//    template<class ValueType>
//    ValueType read() throw()
//    {
//        ValueType value;
//        getInternal()->read (value);
//        return value;
//    }    
//    
//    /** Reads a value or array from the file.
//     This must be one of: char, short, int, long or LongLong 
//     (and their unsigned versions) or a NumericalArray of a built-in type. 
//     This is read in the endian format specified in the constructor. */        
//    template<class ValueType>
//    void read (ValueType& value) throw()
//    {
//        getInternal()->read (value);
//    }    
//    
//    /** Write a numerical value to the file.
//     This must be one of: char, short, int, long or LongLong 
//     (and their unsigned versions). This is written in the endian format
//     specified in the constructor. */    
//    template<class ValueType>
//    void write (const ValueType value) throw()
//    {
//        getInternal()->write (value);
//    }
//	        
//    /** Creates a chunk name identifier.
//     A convenience function for creating an interger to write to a file
//     for chunk IDs in many IFF-type files. */
//    static inline int chunkID (const char* const fourCharCode) throw()
//    {
//        plonk_assert (Text (fourCharCode).length() == 4);
//        return pl_FourCharCode (fourCharCode);
//    }
};



#endif // PLONK_AUDIOFILEREADER_H
