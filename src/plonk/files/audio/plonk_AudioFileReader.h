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
    
    int getBitsPerSample() const throw();
    int getBytesPerFrame() const throw();
    int getNumChannels() const throw();
    double getSampleRate() const throw();
    int getNumFrames() const throw();
    int getFramePosition() const throw();
    void setFramePosition (const int position) throw();
    void resetFramePosition() throw();
    
    template<class SampleType>
    void readFrames (NumericalArray<SampleType>& data, const bool applyScaling) throw();
    
private:
    inline PlankAudioFileReaderRef getPeerRef() { return static_cast<PlankAudioFileReaderRef> (&peer); }
    inline const PlankAudioFileReaderRef getPeerRef() const { return const_cast<const PlankAudioFileReaderRef> (&peer); }
    
    template<class Type>
    static inline void swapEndianIfNotNative (Type* data, const int numItems, const bool dataIsBigEndian) throw()
    {
#if PLONK_BIGENDIAN
        if (! dataIsBigEndian) Endian::swap (data, numItems);
#endif
        
#if PLONK_LITTLEENDIAN
        if (dataIsBigEndian) Endian::swap (data, numItems);
#endif
    }

    PlankAudioFileReader peer;
    Chars readBuffer;
    int numFramesPerBuffer;
};

template<class SampleType>
void AudioFileReaderInternal::readFrames (NumericalArray<SampleType>& data, const bool applyScaling) throw()
{
    // this needs to do many more checks...
    
    ResultCode result;
    
    typedef NumericalArray<SampleType> SampleArray;
    
    const int dataLength = data.length();
    
    SampleType* dataArray = data.getArray();
    void* const readBufferArray = readBuffer.getArray();
    
    int encoding, bits, channels, bytesPerFrame, bytesPerSample;
    result = pl_AudioFileReader_GetEncoding (getPeerRef(), &encoding);              plonk_assert (result == PlankResult_OK);
    result = pl_AudioFileReader_GetBitsPerSample (getPeerRef(), &bits);             plonk_assert (result == PlankResult_OK);
    result = pl_AudioFileReader_GetNumChannels (getPeerRef(), &channels);           plonk_assert (result == PlankResult_OK);
    result = pl_AudioFileReader_GetBytesPerFrame (getPeerRef(), &bytesPerFrame);    plonk_assert (result == PlankResult_OK);
    
    plonk_assert ((encoding >= PLANKAUDIOFILE_ENCODING_MIN) && (encoding <= PLANKAUDIOFILE_ENCODING_MAX));
    plonk_assert (bits > 0);
    plonk_assert (channels > 0);
    plonk_assert (bytesPerFrame > 0);
    
    bytesPerSample = bytesPerFrame / channels;
    
    const bool isPCM = encoding & PLANKAUDIOFILE_ENCODING_PCM_FLAG;
    const bool isFloat = encoding & PLANKAUDIOFILE_ENCODING_FLOAT_FLAG;
    const bool isBigEndian = encoding & PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG;
    
    int dataIndex = 0;
    
    while (dataIndex < dataLength && result != PlankResult_FileEOF)
    {
        int framesRead, samplesRead;
        result = pl_AudioFileReader_ReadFrames (getPeerRef(), numFramesPerBuffer, readBufferArray, &framesRead);
        plonk_assert (result == PlankResult_OK || result == PlankResult_FileEOF);

        samplesRead = framesRead * channels;
        
        if (isPCM)
        {            
            if (bytesPerSample == 2)
            {
                Short* const convertBuffer = static_cast<Short*> (readBufferArray); 
                swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                SampleArray::convert (dataArray, convertBuffer, samplesRead, applyScaling);
            }
            else if (bytesPerSample == 3)
            {
                Int24* const convertBuffer = static_cast<Int24*> (readBufferArray); 
                swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                SampleArray::convert (dataArray, convertBuffer, samplesRead, applyScaling);
            }
            else if (bytesPerSample == 4)
            {
                Int* const convertBuffer = static_cast<Int*> (readBufferArray); 
                swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                SampleArray::convert (dataArray, convertBuffer, samplesRead, applyScaling);
            }
            else if (bytesPerSample == 1)
            {
                Char* const convertBuffer = static_cast<Char*> (readBufferArray); 
                SampleArray::convert (dataArray, convertBuffer, samplesRead, applyScaling);
            }
            else
            {
                plonk_assertfalse;
            }
        }
        else if (isFloat)
        {
            if (bytesPerSample == 4)
            {
                Float* const convertBuffer = static_cast<Float*> (readBufferArray); 
                swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                SampleArray::convert (dataArray, convertBuffer, samplesRead, applyScaling);
            }
            else if (bytesPerSample == 8)
            {
                Double* const convertBuffer = static_cast<Double*> (readBufferArray); 
                swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                SampleArray::convert (dataArray, convertBuffer, samplesRead, applyScaling);
            }
            else
            {
                plonk_assertfalse;
            }
        }
        
        dataArray += samplesRead;
        dataIndex += samplesRead;
    }
    
    if (dataIndex < dataLength)
        data.setSize (dataIndex, true);
   
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
    
    inline int getBitsPerSample() const throw()
    {
        return getInternal()->getBitsPerSample();
    }
    
    inline int getBytesPerFrame() const throw()
    {
        return getInternal()->getBytesPerFrame();
    }
    
    inline int getNumChannels() const throw()
    {
        return getInternal()->getNumChannels();
    }
    
    inline double getSampleRate() const throw()
    {
        return getInternal()->getSampleRate();
    }
    
    inline int getNumFrames() const throw()
    {
        return getInternal()->getNumFrames();
    }
    
    inline int getFramePosition() const throw()
    {
        return getInternal()->getFramePosition();
    }
    
    inline void setFramePosition (const int position) throw()
    {
        return getInternal()->setFramePosition (position);
    }
    
    inline void resetFramePosition() throw()
    {
        return getInternal()->resetFramePosition();
    }
    
    template<class SampleType>
    inline void readFrames (NumericalArray<SampleType>& data) throw()
    {
        getInternal()->readFrames (data, true);
    }
    
    template<class SampleType>
    inline void readFramesDirect (NumericalArray<SampleType>& data) throw()
    {
        getInternal()->readFrames (data, false);
    }

    
    template<class SampleType>
    inline NumericalArray<SampleType> readAllFrames (const bool applyScaling) throw()
    {
        typedef NumericalArray<SampleType> SampleArray;
        SampleArray data = SampleArray::withSize (getNumFrames() * getNumChannels());
        resetFramePosition();
        getInternal()->readFrames (data, applyScaling);
        return data;
    }

    template<class SampleType>
    inline operator NumericalArray<SampleType> () throw()
    {
        return readAllFrames<SampleType> (true);
    }
    
    template<class SampleType>
    inline operator SignalBase<SampleType> () throw()
    {
        NumericalArray<SampleType> data = readAllFrames<SampleType> (true);
        return SignalBase<SampleType> (data, getSampleRate(), getNumChannels());
    }

};



#endif // PLONK_AUDIOFILEREADER_H
