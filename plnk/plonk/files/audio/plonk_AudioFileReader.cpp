/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

AudioFileReaderInternal::AudioFileReaderInternal() throw()
:   numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
}

AudioFileReaderInternal::AudioFileReaderInternal (const char* path, const int bufferSize, AudioFileMetaDataIOFlags const& metaDataIOFlags) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    init (path, metaDataIOFlags);
}

ResultCode AudioFileReaderInternal::init (const char* path, AudioFileMetaDataIOFlags const& metaDataIOFlags) throw()
{
    plonk_assert (path != 0);
    
    pl_AudioFileReader_Init (getPeerRef());
    ResultCode result = pl_AudioFileReader_OpenInternal (getPeerRef(), path, metaDataIOFlags.getValue());
    
    const int bytesPerFrame = getBytesPerFrame();
    
    if (bytesPerFrame == 0)
        result = PlankResult_AudioFileInavlidType;
    else if (result == PlankResult_OK)
        numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
        
    if (this->hasMetaData())
        this->getMetaData().sortCuePointsByPosition();
    
    return result;
}

ResultCode AudioFileReaderInternal::init (ByteArray const& bytes, AudioFileMetaDataIOFlags const& metaDataIOFlags) throw()
{
    pl_AudioFileReader_Init (getPeerRef());
    
    PlankFile file;
    
    if (BinaryFileInternal::setupBytes (&file, bytes, false))
    {        
        if (pl_AudioFileReader_OpenWithFile (getPeerRef(), &file, metaDataIOFlags.getValue()) == PlankResult_OK)
        {
            numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
            
            if (this->hasMetaData())
                this->getMetaData().sortCuePointsByPosition();

            return PlankResult_OK;
        }
    }

    return PlankResult_UnknownError;
}

AudioFileReaderInternal::AudioFileReaderInternal (ByteArray const& bytes, const int bufferSize, AudioFileMetaDataIOFlags const& metaDataIOFlags) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    init (bytes, metaDataIOFlags);
}


AudioFileReaderInternal::AudioFileReaderInternal (FilePathArray const& fileArray, const AudioFile::MultiFileTypes multiMode, const int bufferSize) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
    
    PlankFile file;
    
    if (BinaryFileInternal::setupMulti (&file, fileArray, multiMode, PLANK_BIGENDIAN))
    {
        if (pl_AudioFileReader_OpenWithFile (getPeerRef(), &file, AudioFile::MetaDataIOFlagsNone) == PlankResult_OK)
        {
            if (getBytesPerFrame() > 0)
                numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
        }
    }
}

AudioFileReaderInternal::AudioFileReaderInternal (FilePathArray const& fileArray, IntVariable const& indexRef, const int bufferSize) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    nextMultiIndexRef (indexRef),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
    
    PlankFile file;
    
    if (BinaryFileInternal::setupMulti (&file, fileArray, AudioFile::MultiFileArrayIndexRef, PLANK_BIGENDIAN, &nextMultiIndexRef))
    {
        if (pl_AudioFileReader_OpenWithFile (getPeerRef(), &file, AudioFile::MetaDataIOFlagsNone) == PlankResult_OK)
        {
            if (getBytesPerFrame() > 0)
                numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
        }
    }
}

AudioFileReaderInternal::AudioFileReaderInternal (AudioFileReaderArray const& audioFiles, const AudioFile::MultiFileTypes multiMode, const int bufferSize, IntVariable* indexRef) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
        
    PlankDynamicArrayRef array = pl_DynamicArray_Create();
    pl_DynamicArray_InitWithItemSizeAndSize (array, sizeof (PlankAudioFileReader), audioFiles.length(), true);
    PlankAudioFileReader* rawArray = static_cast<PlankAudioFileReader*> (pl_DynamicArray_GetArray (array));
    
    for (int i = 0; i < audioFiles.length(); ++i)
    {
        AudioFileReader reader = audioFiles.atUnchecked (i);
        reader.disownPeer (&rawArray[i]);
    }
    
    if (indexRef)
        nextMultiIndexRef = *indexRef;
    
    ResultCode result = pl_AudioFileReader_OpenWithAudioFileArray (getPeerRef(), array, true, multiMode, nextMultiIndexRef.getValuePtr());
    
    if (result == PlankResult_OK)
    {
        if (getBytesPerFrame() > 0)
            numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
    }
}

AudioFileReaderInternal::AudioFileReaderInternal (FilePathQueue const& fileQueue, const int bufferSize) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
    
    PlankFile file;
    
    if (BinaryFileInternal::setupMulti (&file, fileQueue, PLANK_BIGENDIAN))
    {
        if (pl_AudioFileReader_OpenWithFile (getPeerRef(), &file, AudioFile::MetaDataIOFlagsNone) == PlankResult_OK)
        {
            if (getBytesPerFrame() > 0)
                numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
        }
    }
}

static PlankResult AudioFileReaderInternal_Queue_NextFunction (PlankP ref, PlankAudioFileReaderRef currentFile, PlankAudioFileReaderRef* audioFile) throw()
{
    AudioFileReaderQueue& queue = *static_cast<AudioFileReaderQueue*> (ref);
    AudioFileReader reader (queue.pop());
    PlankAudioFileReaderRef file = pl_AudioFileReader_CreateAndInit();
    reader.disownPeer (file);
    *audioFile = file;    
    
    PlankResult result = PlankResult_OK;
    
    if (currentFile)
        result = pl_AudioFileReader_Destroy (currentFile);
    
    return result;
}

static PlankResult AudioFileReaderInternal_Queue_FreeFunction (PlankP ref) throw()
{
    AudioFileReaderQueue* queue = static_cast<AudioFileReaderQueue*> (ref);
    delete queue;
    return PlankResult_OK;
}

static PlankResult AudioFileReaderInternal_Queue_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex)
{
    PlankAudioFileReaderCustomRef custom = static_cast<PlankAudioFileReaderCustomRef> (p->peer);
    return custom->currentAudioFile ? pl_AudioFileReader_SetFramePosition (custom->currentAudioFile, frameIndex) : PlankResult_OK;
}

static PlankResult AudioFileReaderInternal_Queue_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex)
{
    PlankAudioFileReaderCustomRef custom = static_cast<PlankAudioFileReaderCustomRef> (p->peer);
    
    if (custom->currentAudioFile)
    {
        return pl_AudioFileReader_GetFramePosition (custom->currentAudioFile, frameIndex);
    }
    else
    {
        *frameIndex = 0;
        return PlankResult_OK;
    }
}

AudioFileReaderInternal::AudioFileReaderInternal (AudioFileReaderQueue const& audioFiles, const int bufferSize) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
    
    if (pl_AudioFileReader_OpenWithCustomNextFunction (getPeerRef(),
                                                       AudioFileReaderInternal_Queue_NextFunction,
                                                       AudioFileReaderInternal_Queue_FreeFunction,
                                                       AudioFileReaderInternal_Queue_SetFramePosition, //0,
                                                       AudioFileReaderInternal_Queue_GetFramePosition, //0,
                                                       new AudioFileReaderQueue (audioFiles)) == PlankResult_OK)
    {
        if (getBytesPerFrame() > 0)
            numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
    }
}

class AudioFileReaderRegion : public PlonkBase
{
public:
    AudioFileReaderRegion (AudioFileReader const& o, AudioFileRegion const& r) throw()
    :   original (o), region (r)
    {
    }
    
    PLONK_INLINE_LOW AudioFileRegion& getRegion() throw() { return region; }
    PLONK_INLINE_LOW AudioFileReader& getOriginal() throw() { return original; }
    PLONK_INLINE_LOW const AudioFileRegion& getRegion() const throw() { return region; }
    PLONK_INLINE_LOW const AudioFileReader& getOriginal() const throw() { return original; }

private:
    AudioFileReader original;
    AudioFileRegion region;
    
    AudioFileReaderRegion();
    AudioFileReaderRegion (AudioFileReaderRegion const&);
};

static PlankResult AudioFileReaderInternal_Region_NextFunction (PlankP ref, PlankAudioFileReaderRef currentFile, PlankAudioFileReaderRef* audioFile) throw()
{
    PlankResult result = PlankResult_OK;
    AudioFileReaderRegion& readerRegion = *static_cast<AudioFileReaderRegion*> (ref);
    
    if (!currentFile)
    {
        PlankAudioFileReaderRef file = pl_AudioFileReader_CreateAndInit();
        result = pl_AudioFileReader_OpenWithRegion (file, readerRegion.getOriginal().getInternal()->getPeerRef(), readerRegion.getRegion().getInternal());
        *audioFile = (result == PlankResult_OK) ? file : (PlankAudioFileReaderRef)PLANK_NULL;
    }
    else
    {
        *audioFile = currentFile;
    }
    
    return result;
}

static PlankResult AudioFileReaderInternal_Region_FreeFunction (PlankP ref) throw()
{
    AudioFileReaderRegion* readerRegion = static_cast<AudioFileReaderRegion*> (ref);
    delete readerRegion;
    return PlankResult_OK;
}

static PlankResult AudioFileReaderInternal_Region_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex)
{
    PlankAudioFileReaderCustomRef custom = static_cast<PlankAudioFileReaderCustomRef> (p->peer);
    return pl_AudioFileReader_SetFramePosition (custom->currentAudioFile, frameIndex);
}

static PlankResult AudioFileReaderInternal_Region_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex)
{
    PlankAudioFileReaderCustomRef custom = static_cast<PlankAudioFileReaderCustomRef> (p->peer);
    return pl_AudioFileReader_GetFramePosition (custom->currentAudioFile, frameIndex);
}

//AudioFileReaderInternal::AudioFileReaderInternal (AudioFileReader const& original, const LongLong start, const LongLong end, const int bufferSize) throw()
//:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
//    numFramesPerBuffer (0),
//    newPositionOnNextRead (-1),
//    hitEndOfFile (false),
//    numChannelsChanged (false),
//    defaultNumChannels (0)
//{
//    pl_AudioFileReader_Init (getPeerRef());
//    
//    AudioFileReaderRegion* readerRegion = new AudioFileReaderRegion (original);
//    pl_AudioFileRegion_SetRegion (readerRegion->getRegion().getInternal(), start, end);
//
//    if (pl_AudioFileReader_OpenWithCustomNextFunction (getPeerRef(),
//                                                       AudioFileReaderInternal_Region_NextFunction,
//                                                       AudioFileReaderInternal_Region_FreeFunction,
//                                                       AudioFileReaderInternal_Region_SetFramePosition,
//                                                       AudioFileReaderInternal_Region_GetFramePosition,
//                                                       readerRegion) == PlankResult_OK)
//    {
//        if (getBytesPerFrame() > 0)
//            numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
//    }
//}

AudioFileReaderInternal::AudioFileReaderInternal (AudioFileReader const& original, AudioFileRegion const& region, const int bufferSize) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1),
    hitEndOfFile (false),
    numChannelsChanged (false),
    audioFileChanged (false),
    defaultNumChannels (0)
{
    pl_AudioFileReader_Init (getPeerRef());
    
    AudioFileReaderRegion* readerRegion = new AudioFileReaderRegion (original, region);
//    setName (original.getName() + "#" + region.getLabel());
    
    if (pl_AudioFileReader_OpenWithCustomNextFunction (getPeerRef(),
                                                       AudioFileReaderInternal_Region_NextFunction,
                                                       AudioFileReaderInternal_Region_FreeFunction,
                                                       AudioFileReaderInternal_Region_SetFramePosition,
                                                       AudioFileReaderInternal_Region_GetFramePosition,
                                                       readerRegion) == PlankResult_OK)
    {
        if (getBytesPerFrame() > 0)
            numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
    }
}


AudioFileReaderInternal::~AudioFileReaderInternal()
{
    pl_AudioFileReader_DeInit (getPeerRef());
}

ResultCode AudioFileReaderInternal::open (const char* path, const int bufferSize, AudioFileMetaDataIOFlags const& metaDataIOFlags) throw()
{
    if ((readBuffer.length() == 0) || (bufferSize > 0))
        readBuffer.setSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize, false);

    numFramesPerBuffer = 0;
    newPositionOnNextRead = -1;
    
    return init (path, metaDataIOFlags);
}

AudioFile::Format AudioFileReaderInternal::getFormat() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetFormat (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
        
    const int format = value;
    
    switch (format)
    {
        case AudioFile::FormatInvalid:      return AudioFile::FormatInvalid;
        case AudioFile::FormatUnknown:      return AudioFile::FormatUnknown;
        case AudioFile::FormatWAV:          return AudioFile::FormatWAV;
        case AudioFile::FormatAIFF:         return AudioFile::FormatAIFF;
        case AudioFile::FormatAIFC:         return AudioFile::FormatAIFC;
        case AudioFile::FormatOggVorbis:    return AudioFile::FormatOggVorbis;
        case AudioFile::FormatOpus:         return AudioFile::FormatOpus;
        case AudioFile::FormatRegion:       return AudioFile::FormatRegion;
        case AudioFile::FormatMulti:        return AudioFile::FormatMulti;
        case AudioFile::FormatArray:        return AudioFile::FormatArray;
        case AudioFile::FormatQueue:        return AudioFile::FormatQueue;
        case AudioFile::ForamtCustom:       return AudioFile::ForamtCustom;
        
        default: return AudioFile::FormatInvalid;
    }
}

AudioFile::Encoding AudioFileReaderInternal::getEncoding() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetEncoding (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    
    const int format = value;
    
    switch (format)
    {
        case AudioFile::EncodingInvalid:                            return AudioFile::EncodingInvalid;
        case AudioFile::EncodingUnknown:                            return AudioFile::EncodingUnknown;
        case AudioFile::EncodingPCMLittleEndian:                    return AudioFile::EncodingPCMLittleEndian;
        case AudioFile::EncodingPCMBigEndian:                       return AudioFile::EncodingPCMBigEndian;
        case AudioFile::EncodingFloatLittleEndian:                  return AudioFile::EncodingFloatLittleEndian;
        case AudioFile::EncodingFloatBigEndian:                     return AudioFile::EncodingFloatBigEndian;
        case AudioFile::EncodingPCMLittleEndianNonInterleaved:      return AudioFile::EncodingPCMLittleEndianNonInterleaved;
        case AudioFile::EncodingPCMBigEndianNonInterleaved:         return AudioFile::EncodingPCMBigEndianNonInterleaved;
        case AudioFile::EncodingFloatLittleEndianNonInterleaved:    return AudioFile::EncodingFloatLittleEndianNonInterleaved;
        case AudioFile::EncodingFloatBigEndianNonInterleaved:       return AudioFile::EncodingFloatBigEndianNonInterleaved;
        default: return AudioFile::EncodingInvalid;
    }
}

AudioFile::SampleType AudioFileReaderInternal::getSampleType() const throw()
{    
    const AudioFile::Encoding encoding = getEncoding();
    const int bytesPerSample = getBytesPerSample();
    const bool isPCM = encoding & AudioFile::EncodingFlagPCM;
    const bool isFloat = encoding & AudioFile::EncodingFlagFloat;

    if (isPCM)
    {        
        switch (bytesPerSample)
        {
            case 2: return AudioFile::Short;
            case 3: return AudioFile::Int24;
            case 4: return AudioFile::Int;
            case 1: return AudioFile::Char;
            default: goto exit;
        }        
    }
    else if (isFloat)
    {
        switch (bytesPerSample)
        {
            case 4: return AudioFile::Float;
            case 8: return AudioFile::Double;
            default: goto exit;
        }                
    }
    
exit:
    return AudioFile::Invalid;
}

int AudioFileReaderInternal::getBitsPerSample() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetBitsPerSample (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

int AudioFileReaderInternal::getBytesPerFrame() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetBytesPerFrame (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

int AudioFileReaderInternal::getBytesPerSample() const throw()
{ 
    return getBytesPerFrame() / getNumChannels();
}
    
int AudioFileReaderInternal::getNumChannels() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetNumChannels (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

void AudioFileReaderInternal::setDefaultNumChannels (const int numChannels) throw()
{
    defaultNumChannels = plonk::max (0, numChannels);
}

int AudioFileReaderInternal::getDefaultNumChannels() throw()
{
    return defaultNumChannels;
}

void AudioFileReaderInternal::setDefaultSampleRate (const double sampleRate) throw()
{
    defaultSampleRate = sampleRate;
}

double AudioFileReaderInternal::getDefaultSampleRate() const throw()
{
    return defaultSampleRate;
}

double AudioFileReaderInternal::getSampleRate() const throw()
{
    double value;
    ResultCode result = pl_AudioFileReader_GetSampleRate (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

ChannelLayout AudioFileReaderInternal::getChannelLayout() const throw()
{
    PlankChannelLayout layout;
    pl_AudioFileReader_GetChannelLayout (getPeerRef(), &layout);
    return layout;
}

ChannelIdentifier AudioFileReaderInternal::getChannelIdentifier (const int channel) const throw()
{
    PlankChannelIdentifier identifier;
    pl_AudioFileReader_GetChannelItentifier (getPeerRef(), channel, &identifier);
    return identifier;
}

LongLong AudioFileReaderInternal::getNumFrames() const throw()
{
    LongLong value;
    ResultCode result = pl_AudioFileReader_GetNumFrames (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

bool AudioFileReaderInternal::isPositionable() const throw()
{
    return pl_AudioFileReader_IsPositionable (getPeerRef());
}

LongLong AudioFileReaderInternal::getFramePosition() const throw()
{
    LongLong value;
    ResultCode result = pl_AudioFileReader_GetFramePosition (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

void AudioFileReaderInternal::setFramePosition (const LongLong position) throw()
{
    ResultCode result = pl_AudioFileReader_SetFramePosition (getPeerRef(), position);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void AudioFileReaderInternal::resetFramePosition() throw()
{
    ResultCode result = pl_AudioFileReader_ResetFramePosition (getPeerRef());
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void AudioFileReaderInternal::setFramePositionOnNextRead (const LongLong position) throw()
{
    newPositionOnNextRead = position; // atomic!
}

void AudioFileReaderInternal::setOwner (void* o) throw()
{
    plonk_assert ((o == 0) || (owner == 0));
    owner = o;
}

void* AudioFileReaderInternal::getOwner() const throw()
{
    return owner;
}

bool AudioFileReaderInternal::isOwned() const throw()
{
    return owner != 0;
}

bool AudioFileReaderInternal::isReady() const throw()
{
    return numFramesPerBuffer != 0;
}

void AudioFileReaderInternal::disownPeer (PlankAudioFileReaderRef otherReader) throw()
{
    pl_MemoryCopy (otherReader, getPeerRef(), sizeof (PlankAudioFileReader));
    pl_MemoryZero (getPeerRef(), sizeof (PlankAudioFileReader));
    pl_AudioFileReader_Init (getPeerRef());
}

void AudioFileReaderInternal::setName (Text const& name) throw()
{
    pl_AudioFileReader_SetName(getPeerRef(), name.getArray());
}

Text AudioFileReaderInternal::getName() const throw()
{
    return pl_AudioFileReader_GetName (getPeerRef());
}

bool AudioFileReaderInternal::hasMetaData() const throw()
{
    return pl_AudioFileReader_GetMetaData (getPeerRef()) != 0;
}

AudioFileMetaData AudioFileReaderInternal::getMetaData() const throw()
{    
    return AudioFileMetaData (pl_AudioFileReader_GetMetaData (getPeerRef()));
}

//AudioFileReaderArray AudioFileReader::regionsFromMetaData (const int metaDataOption, const int bufferSize) throw()
//{
//    AudioFileReaderArray regionReaderArray;
//    
//    PlankAudioFileMetaDataRef metaData = pl_AudioFileReader_GetMetaData (getInternal()->getPeerRef());
//    
//    if (metaData)
//    {
//        if (metaDataOption & AudioFile::ConvertCuePointsToRegions)
//            pl_AudioFileMetaData_ConvertCuePointsToRegions (metaData, getNumFrames(), metaDataOption & AudioFile::RemoveCuePoints);
//     
//        PlankSharedPtrArrayRef regions = pl_AudioFileMetaData_GetRegions (metaData);
//        const int numRegions = (int)pl_SharedPtrArray_GetLength (regions);
//        
//        if (numRegions > 0)
//        {
//            regionReaderArray.setSize (numRegions, false);
//            regionReaderArray.clear();
//            
//            for (PlankL i = 0; i < numRegions; ++i)
//            {
//                PlankAudioFileRegionRef region = reinterpret_cast<PlankAudioFileRegionRef> (pl_SharedPtrArray_GetSharedPtr (regions, i));
//                AudioFileReader reader = AudioFileReader (*this,
//                                                          pl_AudioFileRegion_GetStartPosition (region),
//                                                          pl_AudioFileRegion_GetEndPosition (region),
//                                                          bufferSize);
//                Text name = getName() + "#" + pl_AudioFileRegion_GetLabel (region);
//                reader.setName (name);
//                regionReaderArray.add (reader);
//            }
//        }
//    }
//    
//    return regionReaderArray;
//}

//AudioFileReaderArray AudioFileReader::regionsFromMetaData (const int metaDataOption, const int bufferSize) throw()
//{
//    AudioFileReaderArray regionReaderArray;
//    AudioFileMetaData metaData = getMetaData();
//    
//    if (metaData.isNotNull())
//    {
//        if (metaDataOption & AudioFile::ConvertCuePointsToRegions)
//            metaData.convertCuePointsToRegions (getNumFrames(), metaDataOption & AudioFile::RemoveCuePoints);
//        
//        AudioFileRegionArray regions = metaData.getRegions();
//        const Long numRegions = regions.length();
//        
//        if (numRegions > 0)
//        {
//            regionReaderArray.setSize (numRegions, false);
//            regionReaderArray.clear();
//            
//            for (Long i = 0; i < numRegions; ++i)
//            {
//                AudioFileRegion region = regions[i];
//                AudioFileReader reader = AudioFileReader (*this,
//                                                          region.getStartPosition(),
//                                                          region.getEndPosition(),
//                                                          bufferSize);
//                Text name = getName() + "#" + region.getLabel();
//                reader.setName (name);
//                regionReaderArray.add (reader);
//            }
//        }
//    }
//    
//    return regionReaderArray;
//}

AudioFileReaderArray AudioFileReader::regionsFromMetaData (const int metaDataOption, const int bufferSize) throw()
{
    AudioFileReaderArray regionReaderArray;
    AudioFileMetaData metaData = getMetaData();
    
    if (metaData.isNotNull())
    {
        if (metaDataOption & AudioFile::ConvertCuePointsToRegions)
            metaData.convertCuePointsToRegions (getNumFrames(), metaDataOption & AudioFile::RemoveCuePoints);
        
        AudioFileRegionArray regions = metaData.getRegions();
        const Long numRegions = regions.length();
        
        if (numRegions > 0)
        {
            regionReaderArray.setSize (int (numRegions), false);
            regionReaderArray.clear();
            
            for (Long i = 0; i < numRegions; ++i)
            {
                const AudioFileReader reader = AudioFileReader (*this, regions[i], bufferSize);
                regionReaderArray.add (reader);
            }
        }
    }
    
    return regionReaderArray;
}

END_PLONK_NAMESPACE
