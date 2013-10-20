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

#ifndef PLONK_AUDIOFILEREADER_H
#define PLONK_AUDIOFILEREADER_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../core/plonk_SmartPointer.h"
#include "../../core/plonk_WeakPointer.h"
#include "../../core/plonk_SenderContainer.h"
#include "../../core/plonk_SmartPointerContainer.h"
#include "../../containers/plonk_Text.h"
#include "../../core/plonk_Sender.h"
#include "plonk_AudioFile.h"



class AudioFileReaderInternal : public SmartPointer
{
public:
    typedef AudioFileReader Container;

    AudioFileReaderInternal() throw();
    AudioFileReaderInternal (const char* path, const int bufferSize, const bool readMetaData) throw();
    AudioFileReaderInternal (ByteArray const& bytes, const int bufferSize, const bool readMetaData) throw();
    AudioFileReaderInternal (FilePathArray const& paths, const AudioFile::MultiFileTypes multiMode, const int bufferSize) throw();
    AudioFileReaderInternal (FilePathArray const& paths, IntVariable const& indexRef, const int bufferSize) throw();
    AudioFileReaderInternal (AudioFileReaderArray const& audioFiles, const AudioFile::MultiFileTypes multiMode, const int bufferSize, IntVariable* indexRef = 0) throw();
    AudioFileReaderInternal (FilePathQueue const& paths, const int bufferSize) throw();

    ~AudioFileReaderInternal();
    
    ResultCode open (const char* path, const int bufferSize, const bool readMetaData) throw();
    
    AudioFile::Format getFormat() const throw();
    AudioFile::Encoding getEncoding() const throw();
    AudioFile::SampleType getSampleType() const throw();
    
    int getBitsPerSample() const throw();
    int getBytesPerFrame() const throw();
    int getBytesPerSample() const throw();
    int getNumChannels() const throw();
    int getDefaultNumChannels() throw();
    void setDefaultNumChannels (const int numChannels) throw();
    double getSampleRate() const throw();
    LongLong getNumFrames() const throw();
    LongLong getFramePosition() const throw();
    
    bool isPositionable() const throw();
    void setFramePosition (const LongLong position) throw();
    void resetFramePosition() throw();
    void setFramePositionOnNextRead (const LongLong position) throw();
    
    template<class SampleType>
    void readFrames (NumericalArray<SampleType>& data, const bool applyScaling, const bool deinterleave, IntVariable& numLoops) throw();
    
    template<class SampleType>
    inline void initSignal (SignalBase<SampleType>& signal, const int numFrames) const throw()
    {
        const int numChannels = getNumChannels();
        const int length = (numFrames > 0) ? numFrames * numChannels : (int)getNumFrames() * numChannels;
        NumericalArray<SampleType> buffer = NumericalArray<SampleType>::withSize (length);
        signal = SignalBase<SampleType> (buffer, getSampleRate(), numChannels);
    }
    
    template<class SampleType>
    inline void readSignal (SignalBase<SampleType>& signal, const bool applyScaling) throw()
    {
        // would need to decide in here to deinterleave..
        
        signal.getSampleRate().setValue (getSampleRate());
        IntVariable oneLoop (1);
        readFrames (signal.getBuffers().first(), applyScaling, false, oneLoop);
    }
    
    void setOwner (void* owner) throw();
    void* getOwner() const throw();
    bool isOwned() const throw();
    bool isReady() const throw();
    
    void disownPeer (PlankAudioFileReaderRef otherReader) throw();
    
    bool didHitEOF() const throw() { return hitEndOfFile; }
    bool didNumChannelsChange() const throw() { return numChannelsChanged; }
    
private:
    inline PlankAudioFileReaderRef getPeerRef() { return static_cast<PlankAudioFileReaderRef> (&peer); }
    inline const PlankAudioFileReaderRef getPeerRef() const { return const_cast<const PlankAudioFileReaderRef> (&peer); }
    ResultCode init (const char* path, const bool readMetaData) throw();
    ResultCode init (ByteArray const& bytes, const bool readMetaData) throw();

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
    AtomicLongLong newPositionOnNextRead;
    AtomicValue<void*> owner;
    bool hitEndOfFile;
    bool numChannelsChanged;
    IntVariable nextMultiIndexRef;
    int defaultNumChannels;
};

//------------------------------------------------------------------------------

template<class SampleType>
void AudioFileReaderInternal::readFrames (NumericalArray<SampleType>& data,
                                          const bool applyScaling, 
                                          const bool deinterleave,
                                          IntVariable& numLoops) throw()
{        
    typedef NumericalArray<SampleType> Buffer;
    
    this->hitEndOfFile = false;
    this->numChannelsChanged = false;
    
    ResultCode result;
    
    const int dataLength = data.length();
    int dataRemaining = dataLength;
    
    SampleType* dataArray = data.getArray();
    void* const readBufferArray = readBuffer.getArray();
    
    int encoding = getEncoding();
    int channels = getNumChannels();
    int bytesPerSample = getBytesPerSample();
    
    plonk_assert ((encoding >= AudioFile::EncodingMin) && (encoding <= AudioFile::EncodingMax));
    plonk_assert (getBitsPerSample() > 0);
    plonk_assert (channels > 0);
    plonk_assert (getBytesPerFrame() > 0);
    
    bool isPCM = encoding & AudioFile::EncodingFlagPCM;
    bool isFloat = encoding & AudioFile::EncodingFlagFloat;
    bool isBigEndian = encoding & AudioFile::EncodingFlagBigEndian;
    bool isInterleaved = !(encoding & AudioFile::EncodingFlagNonIntervleaved);
    
    int dataIndex = 0;
    
    const int numFailsAllowed = 3;
    int numFails = 0;
    
    while ((dataRemaining > 0) && 
           (result != PlankResult_FileEOF) &&
           (result != PlankResult_AudioFileFrameFormatChanged) &&
           (numFails < numFailsAllowed))
    {
        AtomicLongLong newPosition (-1);
        newPositionOnNextRead.swapWith (newPosition);
        
        if (newPosition.getValueUnchecked() >= 0)
        {
            result = pl_AudioFileReader_SetFramePosition (getPeerRef(), newPosition.getValueUnchecked());
            plonk_assert (result == PlankResult_OK); // just continue though in release
        }
        
        int framesRead;
        const int framesToRead = plonk::min (dataRemaining / channels, numFramesPerBuffer);
        result = pl_AudioFileReader_ReadFrames (getPeerRef(), framesToRead, readBufferArray, &framesRead);
        plonk_assert ((result == PlankResult_OK) || (result == PlankResult_FileEOF) || (result == PlankResult_AudioFileFrameFormatChanged));
        
        if (framesRead > 0)
        {
            const int samplesRead = framesRead * channels;
            plonk_assert (samplesRead <= dataRemaining);
            
            if (isPCM)
            {            
                if (bytesPerSample == 2)
                {
                    Short* const convertBuffer = static_cast<Short*> (readBufferArray); 
                    swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                    Buffer::convert (dataArray, convertBuffer, samplesRead, applyScaling);
                }
                else if (bytesPerSample == 3)
                {
                    Int24* const convertBuffer = static_cast<Int24*> (readBufferArray); 
                    swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                    Buffer::convert (dataArray, convertBuffer, samplesRead, applyScaling);
                }
                else if (bytesPerSample == 4)
                {
                    Int* const convertBuffer = static_cast<Int*> (readBufferArray); 
                    swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                    Buffer::convert (dataArray, convertBuffer, samplesRead, applyScaling);
                }
                else if (bytesPerSample == 1)
                {
                    Char* const convertBuffer = static_cast<Char*> (readBufferArray); 
                    Buffer::convert (dataArray, convertBuffer, samplesRead, applyScaling);
                }
                else
                {
                    plonk_assertfalse;
                    return;
                }
            }
            else if (isFloat)
            {
                if (bytesPerSample == 4)
                {
                    Float* const convertBuffer = static_cast<Float*> (readBufferArray); 
                    swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                    Buffer::convert (dataArray, convertBuffer, samplesRead, applyScaling);
                }
                else if (bytesPerSample == 8)
                {
                    Double* const convertBuffer = static_cast<Double*> (readBufferArray); 
                    swapEndianIfNotNative (convertBuffer, samplesRead, isBigEndian);
                    Buffer::convert (dataArray, convertBuffer, samplesRead, applyScaling);
                }
                else
                {
                    plonk_assertfalse;
                    return;
                }
            }
            
            if (deinterleave && isInterleaved)
            {
                plonk_assertfalse; // haven't tested this yet...
                SampleType* const deinterleaveBuffer = static_cast<SampleType*> (readBufferArray); 
                Buffer::deinterleave (deinterleaveBuffer, dataArray, samplesRead, channels);
                Buffer::copyData (dataArray, deinterleaveBuffer, samplesRead);
            }
            else if (!deinterleave && !isInterleaved)
            {
                plonk_assertfalse; // haven't tested this yet...
                SampleType* const interleaveBuffer = static_cast<SampleType*> (readBufferArray); 
                Buffer::interleave (interleaveBuffer, dataArray, samplesRead / channels, channels);
                Buffer::copyData (dataArray, interleaveBuffer, samplesRead);
            }
            
            dataArray += samplesRead;
            dataIndex += samplesRead;
            dataRemaining -= samplesRead;
        }
        else
        {
            numFails++;
        }
            
        if ((result == PlankResult_FileEOF) && ((numLoops.getValue() == 0) || (numLoops.getValue() > 1)))
        {
            result = pl_AudioFileReader_ResetFramePosition (getPeerRef());              
            plonk_assert (result == PlankResult_OK);
            
            if (numLoops.getValue() > 1)
                numLoops.setValue (numLoops.getValue() - 1);
        }
        else if (result == PlankResult_AudioFileFrameFormatChanged)
        {
            encoding = getEncoding();
            
            const int newNumChannels = getNumChannels();
            
            if (newNumChannels == channels)
            {
                result = PlankResult_OK; // no need to exit our loop unless the number of channels changed
            }
            else
            {
                channels = getNumChannels();
            }
            
            bytesPerSample = getBytesPerSample();
            
            plonk_assert ((encoding >= AudioFile::EncodingMin) && (encoding <= AudioFile::EncodingMax));
            plonk_assert (getBitsPerSample() > 0);
            plonk_assert (channels > 0);
            plonk_assert (getBytesPerFrame() > 0);
            
            isPCM = encoding & AudioFile::EncodingFlagPCM;
            isFloat = encoding & AudioFile::EncodingFlagFloat;
            isBigEndian = encoding & AudioFile::EncodingFlagBigEndian;
            isInterleaved = !(encoding & AudioFile::EncodingFlagNonIntervleaved);
        }
    }
    
    if (dataIndex < dataLength)
    {
        data.setSize (dataIndex, true);
    
        this->hitEndOfFile = (result == PlankResult_FileEOF);
        this->numChannelsChanged = (result == PlankResult_AudioFileFrameFormatChanged);
    }    
}


//------------------------------------------------------------------------------

/** Audio file reader. 
 This can read audio frames from WAV, AIFF, AIFC or (with the appropriately
 enabled compile time options), Ogg Vorbis or Opus files.
 
 @see BinaryFile 
 @ingroup PlonkOtherUserClasses */
class AudioFileReader : public SmartPointerContainer<AudioFileReaderInternal>
{
public:
    typedef AudioFileReaderInternal                 Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<AudioFileReader>   Weak;

    
    static const AudioFileReader& getNull() throw()
    {
        static AudioFileReader null;        
        return null;
    }
    
    /** Creates a null object. 
     Use open to open a file. */
    AudioFileReader() throw()
    :   Base (new Internal())
    {
    }
    
    /** Creates an audio file reader from the given path. 
     The default buffer size is used given by AudioFile::DefaultBufferSize (32768).
     @param path        The path of the file to read.  */
	AudioFileReader (Text const& path) throw()
	:	Base (new Internal (path.getArray(), 0, false))
	{
	}
        
    /** Creates an audio file reader from the given path.
     The default buffer size is used given by AudioFile::DefaultBufferSize (32768).
     @param path        The path of the file to read.  */
	AudioFileReader (const char* path) throw()
	:	Base (new Internal (path, 0, false))
	{
	}
    
    /** Creates an audio file reader from the given path.
     The default buffer size is used given by AudioFile::DefaultBufferSize (32768).
     @param path        The path of the file to read.  */
	AudioFileReader (FilePath const& path) throw()
	:	Base (new Internal (path.fullpath().getArray(), 0, false))
	{
	}
    
    /** Creates an audio file reader from the given path.
     @param path        The path of the file to read.
     @param bufferSize  The buffer size to use when reading. */
	AudioFileReader (const char* path, const int bufferSize, const bool readMetaData) throw()
	:	Base (new Internal (path, bufferSize, readMetaData))
	{
	}

    /** Creates a multiple audio file reader from the given paths.
     @param paths       The paths of the files to read.
     @param bufferSize  The buffer size to use when reading. */
	AudioFileReader (FilePathArray const& paths, const AudioFile::MultiFileTypes multiMode = AudioFile::MultiFileArraySequenceLoop, const int bufferSize = 0) throw()
	:	Base (new Internal (paths, multiMode, bufferSize))
	{
	}
    
    /** Creates a multiple audio file reader from the given paths.
     @param paths       The paths of the files to read.
     @param bufferSize  The buffer size to use when reading. */
	AudioFileReader (FilePathArray const& paths, IntVariable const& nextIndex, const int bufferSize = 0) throw()
	:	Base (new Internal (paths, nextIndex, bufferSize))
	{
	}
    
    AudioFileReader (AudioFileReaderArray const& audioFiles, const AudioFile::MultiFileTypes multiMode, const int bufferSize = 0, IntVariable* indexRef = 0) throw()
	:	Base (new Internal (audioFiles, multiMode, bufferSize, indexRef))
    {
    }

    AudioFileReader (FilePathQueue const& paths, const int bufferSize = 0) throw()
    :	Base (new Internal (paths, bufferSize))
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
    
    AudioFileReader (ByteArray const& bytes, const int bufferSize = 0, const bool readMetaData = false) throw()
	:	Base (new Internal (bytes, bufferSize, readMetaData))
	{
	}
    
    /** Assignment operator. */
    AudioFileReader& operator= (AudioFileReader const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static AudioFileReader fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Get the format of the audio file. 
     i.e., WAV, AIFF etc 
     See AudioFile::Format the available types. */
    inline AudioFile::Format getFormat() const throw()
    {
        return getInternal()->getFormat();
    }
    
    /** Get the encoding of the audio file. 
     i.e., PCM (integer), floating point, whether big or little endian 
     See AudioFile::Encoding the available types. */
    inline AudioFile::Encoding getEncoding() const throw()
    {
        return getInternal()->getEncoding();
    }
    
    /** Get the sample type of the audio file. 
     i.e., Char (8-bit), Short (16-bit) etc 
     See AudioFile::SampleType the available types. */
    inline AudioFile::SampleType getSampleType() const throw()
    {
        return getInternal()->getSampleType();   
    }
    
    /** Get the number of bits used per sample in the file. */
    inline int getBitsPerSample() const throw()
    {
        return getInternal()->getBitsPerSample();
    }
    
    /** Get the number of bytes in each frame of the file. */
    inline int getBytesPerFrame() const throw()
    {
        return getInternal()->getBytesPerFrame();
    }
    
    /** Get the number of bytes in each sample of the file. */
    inline int getBytesPerSample() const throw()
    {
        return getInternal()->getBytesPerSample();
    }
    
    /** Get the number of channels in the file. */
    inline int getNumChannels() const throw()
    {
        return getInternal()->getNumChannels();
    }
    
    /** Set the default number of channels in the file. 
     This can be used to suggest a maximum number of channels especially where this file
     is a multi-audiofile. */
    inline void setDefaultNumChannels (const int numChannels) throw()
    {
        getInternal()->setDefaultNumChannels (numChannels);
    }
    
    /** Get the default number of channels in the file. */
    inline int getDefaultNumChannels() const throw()
    {
        return getInternal()->getDefaultNumChannels();
    }
    
    /** Get the sample rate of the file. */
    inline double getSampleRate() const throw()
    {
        return getInternal()->getSampleRate();
    }
    
    /** Get the total number of frames in the file. */
    inline LongLong getNumFrames() const throw()
    {
        return getInternal()->getNumFrames();
    }
    
    inline bool isPositionable() const throw()
    {
        return getInternal()->isPositionable();
    }
    
    /** Get the current frame position. */
    inline LongLong getFramePosition() const throw()
    {
        return getInternal()->getFramePosition();
    }
    
    /** Set the frame position to a particular frame. 
     This is not thread safe, use setFramePositionOnNextRead() to call this from another
     thread (e.g., a GUI) while playing a file (e.g., in real time). */
    inline void setFramePosition (const LongLong position) throw()
    {
        getInternal()->setFramePosition (position);
    }
    
    /** Set the frame position to a particular frame just before the next read operation.
     This is thread safe. */
    inline void setFramePositionOnNextRead (const LongLong position) throw()
    {
        getInternal()->setFramePositionOnNextRead (position);
    }
    
    /** Resets the frame position back to the start of the frames. */
    inline void resetFramePosition() throw()
    {
        getInternal()->resetFramePosition();
    }
    
    inline double getDuration() const throw()
    {
        return getInternal()->getNumFrames() / getInternal()->getSampleRate();
    }
        
    inline double getTime() const throw()
    {
        return getInternal()->getFramePosition() / getInternal()->getSampleRate();
    }
    
    inline void setTime (const double time) throw()
    {
        getInternal()->setFramePosition (LongLong (time * getInternal()->getSampleRate()));
    }
    
    inline void setTimeOnNextRead (const double time) throw()
    {
        getInternal()->setFramePositionOnNextRead (LongLong (time * getInternal()->getSampleRate()));
    }
    
    /** Read frames into a pre-allocated NumericalArray and apply scaling. 
     Here the samples are automatically scaled depending on the destination 
     data type of the NumericalArray. 
     @param data    The NumericalArray object to read interleaved frames into. 
     @param numLoops    How many loops to read, 0 means infinite loops. */
    template<class SampleType>
    inline void readFrames (NumericalArray<SampleType>& data, IntVariable& numLoops) throw()
    {
        getInternal()->readFrames (data, true, false, numLoops);
    }
    
    template<class SampleType>
    inline void readFrames (NumericalArray<SampleType>& data) throw()
    {
        IntVariable numLoops (1);
        getInternal()->readFrames (data, true, false, numLoops);
    }
    
    /** Read frames into a pre-allocated NumericalArray without scaling. 
     @param data    The NumericalArray object to read interleaved frames into. 
     @param numLoops    How many loops to read, 0 means infinite loops.
     @return @c true if the array was resized as fewer samples were available, otherwise @c false. */
    template<class SampleType>
    inline void readFramesDirect (NumericalArray<SampleType>& data, IntVariable& numLoops) throw()
    {
        getInternal()->readFrames (data, false, false, numLoops);
    }
    
    template<class SampleType>
    inline void readFramesDirect (NumericalArray<SampleType>& data) throw()
    {
        IntVariable numLoops (1);
        getInternal()->readFrames (data, false, false, numLoops);
    }
    
    /** Initialises a Signal object in the appropriate format for the audio in the file.
     @param signal    The Signal object to initialise.
     @param numFrames The number of frames the Signal should store. */
    template<class SampleType>
    void initSignal (SignalBase<SampleType>& signal, const int numFrames = 0) const throw()
    {
        getInternal()->initSignal (signal, numFrames);
    }
    
    /** Read frames into a pre-allocated Signal object and apply scaling. 
     The Signal object MUST be an interleaved type.
     Here the samples are automatically scaled depending on the destination 
     data type of the Signal. 
     @param signal    The Signal object to read frames into. */
    template<class SampleType>
    inline void readSignal (SignalBase<SampleType>& signal) throw()
    {
        plonk_assert (signal.isInterleaved());
        getInternal()->readSignal (signal, true);
    }
    
    /** Read frames into a pre-allocated Signal object without scaling. 
     The Signal object MUST be an interleaved type. 
     @param signal    The Signal object to read frames into. */
    template<class SampleType>
    inline void readSignalDirect (SignalBase<SampleType>& signal) throw()
    {
        plonk_assert (signal.isInterleaved());
        getInternal()->readSignal (signal, false);
    }
    
    /** Reads all the frames in the file and returns them in an interleaved array.
     Here the samples can be automatically scaled depending on the destination 
     data type. 
     @param applyScaling 
     @return A NumericalArray containing the interleaved sample frames. */
    template<class SampleType>
    inline NumericalArray<SampleType> readAllFrames (const bool applyScaling) throw()
    {
        typedef NumericalArray<SampleType> SampleArray;
        SampleArray data = SampleArray::withSize ((int)getNumFrames() * getNumChannels());
        resetFramePosition();
        IntVariable oneLoop (1);
        getInternal()->readFrames (data, applyScaling, false, oneLoop);
        return data;
    }

    /** Reads all the frames in the file and returns them in an interleaved array. 
     @return A NumericalArray containing the interleaved sample frames.*/
    template<class SampleType>
    inline operator NumericalArray<SampleType> () throw()
    {
        return readAllFrames<SampleType> (true);
    }
    
    /** Reads all the frames in the file and returns them in Signal object 
     @return A Signal containing sample frames. */
    template<class SampleType>
    inline operator SignalBase<SampleType> () throw()
    {
        NumericalArray<SampleType> data = readAllFrames<SampleType> (true);
        return SignalBase<SampleType> (data, getSampleRate(), getNumChannels());
    }
    
    /** Reads all the frames in the file and returns them in Signal object 
     @return A Signal containing sample frames. */
    template<class SampleType>
    inline SignalBase<SampleType> getOtherSignal() throw()
    {
        NumericalArray<SampleType> data = readAllFrames<SampleType> (true);
        return SignalBase<SampleType> (data, getSampleRate(), getNumChannels());
    }
    
    /** Reads all the frames in the file and returns them in Signal object 
     @return A Signal containing sample frames. */
    inline SignalBase<PLONK_TYPE_DEFAULT> getSignal() throw()
    {
        return getOtherSignal<PLONK_TYPE_DEFAULT>();
    }    
    
    void setOwner (void* owner) throw()
    {
        getInternal()->setOwner (owner);
    }
    
    void* getOwner() const throw()
    {
        return getInternal()->getOwner();
    }
    
    bool isOwned() const throw()
    {
        return getInternal()->isOwned();
    }
    
    /** Determines whether the audio data is ready to be read.
     This should return true if the file open and header read operations succeeded. */
    bool isReady() const throw()
    {
        return getInternal()->isReady();
    }
    
    void disownPeer (PlankAudioFileReaderRef otherReader) throw()
    {
        getInternal()->disownPeer (otherReader);
    }
    
    bool didHitEOF() const throw()
    {
        return getInternal()->didHitEOF();
    }
    
    bool didNumChannelsChange() const throw()
    {
        return getInternal()->didNumChannelsChange();
    }
    
private:
    // these could be currently dangerous across threads, need to look at a safer way to open
    // a new file once one is already running...
    
    ResultCode open (const char* path, const int bufferSize, const bool readMetaData) throw()
    {
        return getInternal()->open (path, bufferSize, readMetaData);
    }
    
    ResultCode open (Text const& path) throw()
    {
        return getInternal()->open (path.getArray(), 0, false);
    }
};



#endif // PLONK_AUDIOFILEREADER_H
