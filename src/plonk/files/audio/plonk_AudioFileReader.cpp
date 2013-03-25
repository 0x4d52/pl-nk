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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

AudioFileReaderInternal::AudioFileReaderInternal() throw()
:   numFramesPerBuffer (0),
    newPositionOnNextRead (-1)
{
    pl_AudioFileReader_Init (getPeerRef());
}

AudioFileReaderInternal::AudioFileReaderInternal (const char* path, const int bufferSize, const bool readMetaData) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1)
{
    init (path, readMetaData);
}

ResultCode AudioFileReaderInternal::init (const char* path, const bool readMetaData) throw()
{
    plonk_assert (path != 0);
    
    pl_AudioFileReader_Init (getPeerRef());
    ResultCode result = pl_AudioFileReader_OpenInternal (getPeerRef(), path, readMetaData);
    
    if (result == PlankResult_OK)
        numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
    
    return result;
}

ResultCode AudioFileReaderInternal::init (ByteArray const& bytes, const bool readMetaData) throw()
{
    pl_AudioFileReader_Init (getPeerRef());
    
    PlankFile file;
    
    if (BinaryFileInternal::setupBytes (&file, bytes, false))
    {        
        if (pl_AudioFileReader_OpenWithFile (getPeerRef(), &file, readMetaData) == PlankResult_OK)
        {
            numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();
            return PlankResult_OK;
        }
    }

    return PlankResult_UnknownError;
}

AudioFileReaderInternal::AudioFileReaderInternal (ByteArray const& bytes, const int bufferSize, const bool readMetaData) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize)),
    numFramesPerBuffer (0),
    newPositionOnNextRead (-1)
{
    init (bytes, readMetaData);
}

AudioFileReaderInternal::~AudioFileReaderInternal()
{
    pl_AudioFileReader_DeInit (getPeerRef());
}

ResultCode AudioFileReaderInternal::open (const char* path, const int bufferSize, const bool readMetaData) throw()
{
    if ((readBuffer.length() == 0) || (bufferSize > 0))
        readBuffer.setSize ((bufferSize > 0) ? bufferSize : AudioFile::DefaultBufferSize, false);

    numFramesPerBuffer = 0;
    newPositionOnNextRead = -1;
    
    return init (path, readMetaData);
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

END_PLONK_NAMESPACE
