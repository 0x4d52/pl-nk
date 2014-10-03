/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#include "../../core/plank_StandardHeader.h"
#include "../plank_File.h"
#include "../plank_IffFileWriter.h"
#include "../../maths/plank_Maths.h"
#include "../../random/plank_RNG.h"
#include "plank_AudioFileWriter.h"
#include "plank_AudioFileMetaData.h"
#include "plank_AudioFileCuePoint.h"
#include "plank_AudioFileRegion.h"

#define PLANKAUDIOFILEWRITER_BUFFERLENGTH 256

// private

typedef PlankResult (*PlankAudioFileWriterWriteHeaderFunction)(PlankAudioFileWriterRef);
typedef PlankResult (*PlankAudioFileWriterWriteFramesFunction)(PlankAudioFileWriterRef, const PlankB, const int, const void*);

PlankResult pl_AudioFileWriter_WAV_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_WAV_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_WAV_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAVEXT_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAVRF64_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAV_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder,  const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_AIFF_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_AIFF_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_AIFF_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_AIFF_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_AIFC_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_AIFC_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_AIFC_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_AIFC_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_CAF_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_CAF_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_CAF_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_CAF_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_W64_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_W64_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_W64_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_W64_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_Iff_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const char* chunkID, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_OggVorbis_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_OggVorbis_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_OggVorbis_Close (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_OggVorbis_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_Opus_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_Opus_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);
PlankResult pl_AudioFileWriter_Opus_Close (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_Opus_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_WAV_WriteMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_AIFFAIFC_WriteMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_CAF_WritePreDataMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_CAF_WritePostDataMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_W64_WriteMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_OggVorbis_WriteMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_Opus_WriteMetaData (PlankAudioFileWriterRef p);

typedef struct PlankIffAudioFileWriter* PlankIffAudioFileWriterRef;
typedef struct PlankIffAudioFileWriter
{
    PlankIffFileWriter iff;
//    PlankDynamicArray temp;
} PlankIffAudioFileWriter;

static PlankResult pl_IffAudioFileWriter_DeInit (PlankIffAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_IffFileWriter_DeInit (&p->iff)) != PlankResult_OK) goto exit;
//    if ((result = pl_DynamicArray_DeInit (&p->temp)) != PlankResult_OK) goto exit;
    
    pl_MemoryZero (p, sizeof (PlankIffAudioFileWriter));
    
exit:
    return result;
}

static PlankResult pl_IffAudioFileWriter_Destroy (PlankIffAudioFileWriterRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_IffAudioFileWriter_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

static PlankIffAudioFileWriterRef pl_IffAudioFileWriter_Create()
{
    PlankMemoryRef m;
    PlankIffAudioFileWriterRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankIffAudioFileWriterRef)pl_Memory_AllocateBytes (m, sizeof (PlankIffAudioFileWriter));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankIffAudioFileWriter));
    
    return p;
}

static PlankIffAudioFileWriterRef pl_IffAudioFileWriter_CreateAndInit()
{
    PlankIffAudioFileWriterRef p;
    p = pl_IffAudioFileWriter_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_IffFileWriter_Init (&p->iff) != PlankResult_OK)
            pl_IffAudioFileWriter_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}


///---


PlankAudioFileWriterRef pl_AudioFileWriter_CreateAndInit()
{
    PlankAudioFileWriterRef p;
    p = pl_AudioFileWriter_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_AudioFileWriter_Init (p) != PlankResult_OK)
            pl_AudioFileWriter_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankAudioFileWriterRef pl_AudioFileWriter_Create()
{
    PlankMemoryRef m;
    PlankAudioFileWriterRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAudioFileWriterRef)pl_Memory_AllocateBytes (m, sizeof (PlankAudioFileWriter));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAudioFileWriter));
    
    return p;
}

PlankResult pl_AudioFileWriter_Init (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    pl_MemoryZero (p, sizeof (PlankAudioFileWriter));
    
    p->peer                        = PLANK_NULL;
    p->formatInfo.format           = PLANKAUDIOFILE_FORMAT_INVALID;
    p->formatInfo.encoding         = PLANKAUDIOFILE_ENCODING_INVALID;
    p->formatInfo.bitsPerSample    = 0;
    p->formatInfo.bytesPerFrame    = 0;
    
    p->formatInfo.sampleRate       = 0.0;
    
    p->numFrames                   = 0;
    p->dataPosition                = -1;
    p->metaDataChunkPosition       = -1;
    p->headerPad                   = 0;
    p->metaDataIOFlags             = PLANKAUDIOFILEMETADATA_IOFLAGS_ALL;
    p->metaData                    = PLANK_NULL;
    
    p->writeFramesFunction         = PLANK_NULL;
    p->writeHeaderFunction         = PLANK_NULL;
    
    return result;
}

PlankResult pl_AudioFileWriter_DeInit (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_AudioFileWriter_Close (p)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileFormatInfo_DeInit (&p->formatInfo)) != PlankResult_OK) goto exit;

    pl_AudioFileWriter_SetMetaData (p, PLANK_NULL); // don't check for error
    pl_MemoryZero (p, sizeof (PlankAudioFileWriter));
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_Destroy (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_AudioFileWriter_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);    
    
exit:
    return result;
}

PlankFileRef pl_AudioFileWriter_GetFile (PlankAudioFileWriterRef p)
{
    return (PlankFileRef)p->peer;
}

PlankAudioFileFormatInfo* pl_AudioFileWriter_GetFormatInfo (PlankAudioFileWriterRef p)
{
    return p->peer ? 0 : &p->formatInfo;
}

const PlankAudioFileFormatInfo* pl_AudioFileWriter_GetFormatInfoReadOnly (PlankAudioFileWriterRef p)
{
    return &p->formatInfo;
}

PlankAudioFileMetaDataIOFlags pl_AudioFileWriter_GetMetaDataIOFlags (PlankAudioFileWriterRef p)
{
    return p->metaDataIOFlags;
}

PlankResult pl_AudioFileWriter_SetMetaDataIOFlags (PlankAudioFileWriterRef p, PlankAudioFileMetaDataIOFlags metaDataIOFlags)
{
    p->metaDataIOFlags = metaDataIOFlags;
    return PlankResult_OK;
}

PlankAudioFileMetaDataRef pl_AudioFileWriter_GetMetaData (PlankAudioFileWriterRef p)
{    
    if (!p->metaData)
        pl_AudioFileMetaData_CreateSharedPtr (&p->metaData);
    
    return p->metaData;
}

PlankResult pl_AudioFileWriter_SetMetaData (PlankAudioFileWriterRef p, PlankAudioFileMetaDataRef metaData)
{    
    pl_SharedPtrSwap ((PlankSharedPtrRef*)&p->metaData, (PlankSharedPtrRef*)&metaData);
    return pl_AudioFileMetaData_DecrementRefCount (metaData);
}

PlankResult pl_AudioFileWriter_GetNumChannels (PlankAudioFileWriterRef p, int* numChannels)
{
    *numChannels = pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatWAV (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat)
{
    PlankUI numChannels;
    
    if (p->peer)
        return PlankResult_UnknownError;
    
    if (isFloat)
    {
        if ((bitsPerSample != 32) && (bitsPerSample != 64))
            return PlankResult_AudioFileInavlidType;
    }
    
    numChannels = channelLayout & 0x0000FFFF;
    
    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_WAV;
    p->formatInfo.encoding          = isFloat ? PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN : PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;
    
    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);
    
    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_SetSimpleLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);
    
    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = (int)(p->formatInfo.bytesPerFrame * p->formatInfo.sampleRate * numChannels * PLANKAUDIOFILE_CHARBITS);
    p->formatInfo.minimumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.maximumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.frameDuration     = 1.0 / p->formatInfo.sampleRate;
    p->formatInfo.quality           = p->formatInfo.bitsPerSample / 64.f;
    p->dataOffset                   = 0;

    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatAIFF (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate)
{
    PlankUI numChannels;

    if (p->peer)
        return PlankResult_UnknownError;
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_AIFF;
    p->formatInfo.encoding          = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);

    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_SetSimpleLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);
    
    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = (int)(p->formatInfo.bytesPerFrame * p->formatInfo.sampleRate * numChannels * PLANKAUDIOFILE_CHARBITS);
    p->formatInfo.minimumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.maximumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.frameDuration     = 1.0 / p->formatInfo.sampleRate;
    p->formatInfo.quality           = p->formatInfo.bitsPerSample / 64.f;
    p->dataOffset                   = 8;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatAIFC (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat, const PlankB isLittleEndian)
{
    PlankUI numChannels;

    if (p->peer)
        return PlankResult_UnknownError;
    
    if (isFloat)
    {
        if ((bitsPerSample != 32) && (bitsPerSample != 64))
            return PlankResult_AudioFileInavlidType;
        
        if (isLittleEndian)
            return PlankResult_AudioFileInavlidType;
    }
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_AIFC;
    p->formatInfo.encoding          = isFloat ? PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN : isLittleEndian ? PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN : PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);

    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_SetSimpleLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);

    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = (int)(p->formatInfo.bytesPerFrame * p->formatInfo.sampleRate * numChannels * PLANKAUDIOFILE_CHARBITS);
    p->formatInfo.minimumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.maximumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.frameDuration     = 1.0 / p->formatInfo.sampleRate;
    p->formatInfo.quality           = p->formatInfo.bitsPerSample / 64.f;
    p->dataOffset                   = 8;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatCAF (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat, const PlankB isLittleEndian)
{
    PlankUI numChannels;

    if (p->peer)
        return PlankResult_UnknownError;
    
    if (isFloat)
    {
        if ((bitsPerSample != 32) && (bitsPerSample != 64))
            return PlankResult_AudioFileInavlidType;        
    }
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_CAF;
    p->formatInfo.encoding          = 0;
    
    if (isFloat)
        p->formatInfo.encoding     |= PLANKAUDIOFILE_ENCODING_FLOAT_FLAG;
    
    if (!isLittleEndian)
        p->formatInfo.encoding     |= PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG;
    
    p->formatInfo.bitsPerSample     = bitsPerSample;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);

    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_SetSimpleLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);

    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = (int)(p->formatInfo.bytesPerFrame * p->formatInfo.sampleRate * numChannels * PLANKAUDIOFILE_CHARBITS);
    p->formatInfo.minimumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.maximumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.frameDuration     = 1.0 / p->formatInfo.sampleRate;
    p->formatInfo.quality           = p->formatInfo.bitsPerSample / 64.f;
    p->dataOffset                   = 4;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatW64 (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat)
{
    PlankUI numChannels;

    if (p->peer)
        return PlankResult_UnknownError;
    
    if (isFloat)
    {
        if ((bitsPerSample != 32) && (bitsPerSample != 64))
            return PlankResult_AudioFileInavlidType;
    }
    
    numChannels = channelLayout & 0x0000FFFF;
    
    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_W64;
    p->formatInfo.encoding          = isFloat ? PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN : PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);

    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_SetSimpleLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);

    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = (int)(p->formatInfo.bytesPerFrame * p->formatInfo.sampleRate * numChannels * PLANKAUDIOFILE_CHARBITS);
    p->formatInfo.minimumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.maximumBitRate    = p->formatInfo.nominalBitRate;
    p->formatInfo.frameDuration     = 1.0 / p->formatInfo.sampleRate;
    p->formatInfo.quality           = p->formatInfo.bitsPerSample / 64.f;
    p->dataOffset                   = 0;
    
    return PlankResult_OK;    
}

PlankResult pl_AudioFileWriter_SetFormatOggVorbis (PlankAudioFileWriterRef p, const float quality, const PlankChannelLayout channelLayout, const double sampleRate)
{
    PlankUI numChannels;
    int bitsPerSample = 32;
    
    if (p->peer)
        return PlankResult_UnknownError;
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_OGGVORBIS;
    p->formatInfo.encoding          = PLANK_BIGENDIAN ? PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN : PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);

    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);

    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = 0;
    p->formatInfo.minimumBitRate    = 0;
    p->formatInfo.maximumBitRate    = 0;
    p->formatInfo.frameDuration     = 0.f;
    p->formatInfo.quality           = pl_ClipF (quality, 0., 10.f);
    p->dataOffset                   = 0;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatOggVorbisManaged (PlankAudioFileWriterRef p, const int min, const int nominal, const int max, const PlankChannelLayout channelLayout, const double sampleRate)
{
    PlankUI numChannels;
    int bitsPerSample = 32;

    if (p->peer)
        return PlankResult_UnknownError;
    
    if (max < min)
        return PlankResult_AudioFileInavlidType;
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_OGGVORBIS;
    p->formatInfo.encoding          = PLANK_BIGENDIAN ? PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN : PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);
    
    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);


    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = ((nominal < min) || (nominal > max)) ? (min + max) / 2 : nominal;
    p->formatInfo.minimumBitRate    = min;
    p->formatInfo.maximumBitRate    = max;
    p->formatInfo.frameDuration     = 0.f;
    p->formatInfo.quality           = 0.f;
    p->dataOffset                   = 0;
    
    return PlankResult_OK;
}


PlankResult pl_AudioFileWriter_SetFormatOpus (PlankAudioFileWriterRef p, const float quality, const PlankChannelLayout channelLayout, const double sampleRate, const double frameDuration)
{
    PlankUI numChannels;
    int bitsPerSample = 32;

    if (p->peer)
        return PlankResult_UnknownError;
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_OPUS;
    p->formatInfo.encoding          = PLANK_BIGENDIAN ? PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN : PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;
    p->formatInfo.frameDuration     = frameDuration;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);
    
    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_Opus_SetDefaultLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);

    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = 0;
    p->formatInfo.minimumBitRate    = 0;
    p->formatInfo.maximumBitRate    = 0;
    p->formatInfo.frameDuration     = frameDuration;
    p->formatInfo.quality           = pl_ClipF (quality, 0., 10.f);
    p->dataOffset                   = 0;

    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatOpusManaged (PlankAudioFileWriterRef p, const int nominalBitRate, const PlankChannelLayout channelLayout, const double sampleRate, const double frameDuration)
{
    PlankUI numChannels;

    int bitsPerSample = 32;

    if (p->peer)
        return PlankResult_UnknownError;
    
    numChannels = channelLayout & 0x0000FFFF;

    p->formatInfo.format            = PLANKAUDIOFILE_FORMAT_OPUS;
    p->formatInfo.encoding          = PLANK_BIGENDIAN ? PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN : PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    p->formatInfo.bitsPerSample     = bitsPerSample;
    p->formatInfo.frameDuration     = frameDuration;

    pl_AudioFileFormatInfo_SetNumChannels (&p->formatInfo, numChannels, PLANK_FALSE);
    
    if (channelLayout < PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM)
        pl_AudioFileFormatInfo_Opus_SetDefaultLayout (&p->formatInfo);
    else
        pl_AudioFileWriter_SetChanneLayout (p, channelLayout);

    p->formatInfo.sampleRate        = sampleRate;
    p->formatInfo.bytesPerFrame     = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    p->formatInfo.nominalBitRate    = nominalBitRate;
    p->formatInfo.minimumBitRate    = nominalBitRate;
    p->formatInfo.maximumBitRate    = nominalBitRate;
    p->formatInfo.frameDuration     = frameDuration;
    p->formatInfo.quality           = 0.f;
    p->dataOffset                   = 0;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    
    if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_WAV)
    {
        result = pl_AudioFileWriter_WAV_Open (p, filepath);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFF)
    {
        result = pl_AudioFileWriter_AIFF_Open (p, filepath);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFC)
    {
        result = pl_AudioFileWriter_AIFC_Open (p, filepath);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_CAF)
    {
        result = pl_AudioFileWriter_CAF_Open (p, filepath);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_W64)
    {
        result = pl_AudioFileWriter_W64_Open (p, filepath);
    }
#if PLANK_OGGVORBIS
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_OGGVORBIS)
    {
        result = pl_AudioFileWriter_OggVorbis_Open (p, filepath);
    }
#endif
#if PLANK_OPUS
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_OPUS)
    {
        result = pl_AudioFileWriter_Opus_Open (p, filepath);
    }
#endif
    else
    {
        result = PlankResult_AudioFileInavlidType;
    }
    
//exit:
    return result;
}

PlankResult pl_AudioFileWriter_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    
    if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_WAV)
    {
        result = pl_AudioFileWriter_WAV_OpenWithFile (p, file);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFF)
    {
        result = pl_AudioFileWriter_AIFF_OpenWithFile (p, file);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFC)
    {
        result = pl_AudioFileWriter_AIFC_OpenWithFile (p, file);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_CAF)
    {
        result = pl_AudioFileWriter_CAF_OpenWithFile (p, file);
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_W64)
    {
        result = pl_AudioFileWriter_W64_OpenWithFile (p, file);
    }
#if PLANK_OGGVORBIS
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_OGGVORBIS)
    {
        result = pl_AudioFileWriter_OggVorbis_OpenWithFile (p, file);
    }
#endif
#if PLANK_OPUS
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_OPUS)
    {
        result = pl_AudioFileWriter_Opus_OpenWithFile (p, file);
    }
#endif
    else
    {
        result = PlankResult_AudioFileInavlidType;
    }
    
//exit:
    return result;
}

PlankResult pl_AudioFileWriter_Close (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if (p->peer == PLANK_NULL)
        return PlankResult_OK;
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
    switch (p->formatInfo.format)
    {
        case PLANKAUDIOFILE_FORMAT_WAV:
        case PLANKAUDIOFILE_FORMAT_AIFF:
        case PLANKAUDIOFILE_FORMAT_AIFC:
        case PLANKAUDIOFILE_FORMAT_UNKNOWNIFF:
            result = pl_IffAudioFileWriter_Destroy ((PlankIffAudioFileWriter*)p->peer);
            break;
        case PLANKAUDIOFILE_FORMAT_CAF:
            if ((result = pl_AudioFileWriter_CAF_WritePostDataMetaData (p)) != PlankResult_OK) goto exit;
            result = pl_IffAudioFileWriter_Destroy ((PlankIffAudioFileWriter*)p->peer);
            break;
#if PLANK_OGGVORBIS
        case PLANKAUDIOFILE_FORMAT_OGGVORBIS:
            result = pl_AudioFileWriter_OggVorbis_Close (p);
            break;
#endif
#if PLANK_OPUS
        case PLANKAUDIOFILE_FORMAT_OPUS:
            result = pl_AudioFileWriter_Opus_Close (p);
            break;
#endif
        default:
            if (p->peer != PLANK_NULL)
                result = PlankResult_UnknownError;
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = PLANK_NULL;
            
exit:
    return result;
}

PlankResult pl_AudioFileWriter_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    PlankResult result = PlankResult_OK;
    
    if (!p->writeFramesFunction)
    {
        result = PlankResult_FunctionsInvalid;
        goto exit;
    }
    
    result = ((PlankAudioFileWriterWriteFramesFunction)p->writeFramesFunction) (p, convertByteOrder, numFrames, data);
    
    if (result == PlankResult_OK)
        p->numFrames += numFrames;
    
exit:
    return result;
}

PlankB pl_AudioFileWriter_IsEncodingNativeEndian (PlankAudioFileWriterRef p)
{
#if PLANK_BIGENDIAN
    return !! (p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG);
#elif PLANK_LITTLEENDIAN
    return  ! (p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG);
#else
#error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}

PlankResult pl_AudioFileWriter_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p->writeHeaderFunction) // not all formats have a header so this is OK
        result = ((PlankAudioFileWriterWriteHeaderFunction)p->writeHeaderFunction)(p);
    
    p->headerPad = 0;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetHeaderPad (PlankAudioFileWriterRef p, const PlankUI headerPad)
{
    p->headerPad = headerPad;
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetChannelItentifier (PlankAudioFileWriterRef p, const int channel, const PlankChannelIdentifier channelIdentifier)
{
    return pl_AudioFileFormatInfo_SetChannelItentifier (&p->formatInfo, channel, channelIdentifier);
}

PlankResult pl_AudioFileWriter_GetChannelItentifier (PlankAudioFileWriterRef p, const int channel, PlankChannelIdentifier* identifier)
{
    *identifier = pl_AudioFileFormatInfo_GetChannelItentifier (&p->formatInfo, channel);
    return PlankResult_OK;
}

PlankB pl_AudioFileWriter_SetChanneLayout (PlankAudioFileWriterRef p, const PlankChannelLayout layout)
{
    return pl_AudioFileFormatInfo_LayoutToFormatChannelIdentifiers (&p->formatInfo, layout);
}

PlankResult pl_AudioFileWriter_GetChannelLayout (PlankAudioFileWriterRef p, PlankChannelLayout* layout)
{
    *layout = pl_AudioFileFormatInfo_GetChannelLayout (&p->formatInfo);
    return PlankResult_OK;
}

static PlankResult pl_AudioFileWriter_WAV_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;

    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }

    if (!((p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN) ||
          (p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN)))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo) < 1)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample > 32) &&
        (p->formatInfo.encoding != PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample < 8) || (p->formatInfo.bitsPerSample > 64))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.bytesPerFrame == 0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
        
    iff = pl_IffAudioFileWriter_CreateAndInit();
    
    if (!iff)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if (filepath)
    {
        result = pl_IffFileWriter_OpenReplacing ((PlankIffFileWriterRef)iff, filepath, PLANK_FALSE, "RIFF", "WAVE", PLANKIFFFILE_ID_FCC);
    }
    else
    {
        result = pl_IffFileWriter_OpenWithFile ((PlankIffFileWriterRef)iff, file, "RIFF", "WAVE", PLANKIFFFILE_ID_FCC);        
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
    
    p->writeFramesFunction = (PlankM)pl_AudioFileWriter_WAV_WriteFrames;    
    p->writeHeaderFunction = (PlankM)pl_AudioFileWriter_WAV_WriteHeader;

    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}


PlankResult pl_AudioFileWriter_WAV_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_WAV_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_WAV_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_WAV_OpenInternal (p, 0, file);
}

PlankResult pl_AudioFileWriter_WAV_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    const char* fmt;
    PlankUS encoding;
    PlankUS numChannels;
    int chunkHeaderLength;
    
    iff = (PlankIffAudioFileWriterRef)p->peer;
    chunkHeaderLength = iff->iff.common.headerInfo.headerLength;
    
    numChannels = (PlankUS)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    
    if ((numChannels > 2) || (iff->iff.common.headerInfo.mainLength > 0xffffffff))
        return pl_AudioFileWriter_WAVEXT_WriteHeader (p);
    
    fmt = "fmt ";
    
    switch (p->formatInfo.encoding)
    {
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:   encoding = PLANKAUDIOFILE_WAV_COMPRESSION_PCM;   break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN: encoding = PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT; break;
        default: result = PlankResult_AudioFileInavlidType; goto exit;
    }
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, fmt, 0, PLANKAUDIOFILE_WAV_HEADER_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        
        if (p->headerPad > 0)
        {
            if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, "JUNK", 0, p->headerPad, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        }
        
        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        p->metaDataChunkPosition = chunkInfo->chunkPos + PLANKAUDIOFILE_WAV_HEADER_LENGTH;
        p->dataPosition = p->metaDataChunkPosition + (p->headerPad > 0 ? p->headerPad + chunkHeaderLength : 0) + chunkHeaderLength + p->dataOffset;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
    
    if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, fmt, PLANKAUDIOFILE_WAV_FMT_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, encoding)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;

    // don't use up the junk reserved for the larger WAV header
    if ((result = pl_AudioFileWriter_WAV_WriteMetaData (p)) != PlankResult_OK) goto exit;

exit:
    return result;
}

static PlankUI pl_AudioFileWriter_WAVEXT_ChannelIdentifiersToMask (const PlankAudioFileFormatInfo* formatInfo)
{
    PlankUI channelMask, numChannels, i;
    PlankChannelIdentifier channelIdentifier, lastChannelIdentifier;
    PlankChannelIdentifier* channelIdentifiers;
    
    channelMask             = 0x00000000;
    lastChannelIdentifier   = 0;
    numChannels             = (PlankUI)pl_AudioFileFormatInfo_GetNumChannels ((PlankAudioFileFormatInfo*)formatInfo);
    channelIdentifiers      = pl_AudioFileFormatInfo_GetChannelIdentifiers ((PlankAudioFileFormatInfo*)formatInfo);
    
    for (i = 0; i < numChannels; ++i)
    {
        channelIdentifier = channelIdentifiers[i];
        
        if ((channelIdentifier <= lastChannelIdentifier) || // must be in order
            (channelIdentifier > 31))                       
            return PLANKAUDIOFILE_WAV_SPEAKER_NONE;          // can't encode this channel map to a valid 
        
        lastChannelIdentifier = channelIdentifier;
        
        channelMask |= ((PlankUI)1) << (channelIdentifier - 1);
    }
    
    return channelMask & PLANKAUDIOFILE_WAV_SPEAKER_VALIDBITS;
}

PlankResult pl_AudioFileWriter_WAVEXT_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankGUID ext;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    const char* fmt;
    int chunkHeaderLength;
    
    iff = (PlankIffAudioFileWriterRef)p->peer;
    chunkHeaderLength = iff->iff.common.headerInfo.headerLength;

    if (iff->iff.common.headerInfo.mainLength > 0xffffffff)
        return pl_AudioFileWriter_WAVRF64_WriteHeader (p);
    
    fmt = "fmt ";

    switch (p->formatInfo.encoding)
    {
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:   pl_GUID_InitHexString (&ext, "00000001-0000-0010-8000-00aa00389b71"); break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN: pl_GUID_InitHexString (&ext, "00000003-0000-0010-8000-00aa00389b71"); break;
        default: result = PlankResult_AudioFileInavlidType; goto exit;
    }
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, fmt, 0, PLANKAUDIOFILE_WAV_HEADER_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        
        if (p->headerPad > 0)
        {
            if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, "JUNK", 0, p->headerPad, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        }

        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        p->metaDataChunkPosition = chunkInfo->chunkPos + PLANKAUDIOFILE_WAV_HEADER_LENGTH;
        p->dataPosition = p->metaDataChunkPosition + (p->headerPad > 0 ? p->headerPad + chunkHeaderLength : 0) + chunkHeaderLength + p->dataOffset;

        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
    
    if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, fmt, PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    // regular WAV fmt
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    
    // extensible part
    if ((result = pl_File_WriteUS   ((PlankFileRef)iff, (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH - PLANKAUDIOFILE_WAV_FMT_LENGTH - sizeof (PlankUS)))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS   ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI   ((PlankFileRef)iff, pl_AudioFileWriter_WAVEXT_ChannelIdentifiersToMask (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteGUID ((PlankFileRef)iff, &ext)) != PlankResult_OK) goto exit;
    
    if ((result = pl_AudioFileWriter_WAV_WriteMetaData (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_WAVRF64_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankGUID ext;
    PlankIffFileWriterChunkInfoRef fmtChunkInfo, junkChunkInfo, ds64ChunkInfo, dataChunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    const char* fmt;
    const char* ds64;
    const char* RF64;
    const char* data;
    PlankLL dataLength;
    char JUNK[64];
    
    // need to add metadata
    
    iff = (PlankIffAudioFileWriterRef)p->peer;

    fmt  = "fmt ";
    ds64 = "ds64";
    RF64 = "RF64";
    data = "data";
    pl_IffFile_ChunkIDString ((PlankIffFileRef)iff, &iff->iff.common.headerInfo.junkID, JUNK);
    
    // update to RF64 header
    iff->iff.common.headerInfo.mainID.fcc = pl_FourCharCode (RF64);
    if ((result = pl_IffFileWriter_WriteHeader ((PlankIffFileWriterRef)iff)) != PlankResult_OK) goto exit;
    
    switch (p->formatInfo.encoding)
    {
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:   pl_GUID_InitHexString (&ext, "00000001-0000-0010-8000-00aa00389b71"); break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN: pl_GUID_InitHexString (&ext, "00000003-0000-0010-8000-00aa00389b71"); break;
        default: result = PlankResult_AudioFileInavlidType; goto exit;
    }
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, ds64, &ds64ChunkInfo, 0)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, fmt,  &fmtChunkInfo,  0)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, data, &dataChunkInfo, 0)) != PlankResult_OK) goto exit;

    dataLength = dataChunkInfo ? dataChunkInfo->chunkLength : 0;
        
    if (!fmtChunkInfo && !ds64ChunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, ds64, 0, PLANKAUDIOFILE_WAV_DS64_MINIMUMLENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, fmt, 0, PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
    }
    else if (!ds64ChunkInfo)
    {
        // we have format but not ds64
        
        // ensure the fmt size if correct
        if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, fmt, PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;

        // seek junk
        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, fmtChunkInfo->chunkPos, JUNK, &junkChunkInfo, 0)) != PlankResult_OK) goto exit;

        if (!junkChunkInfo)
        {
            // the junk isn't there
            result = PlankResult_UnknownError;
            goto exit;
        }
        
        if ((fmtChunkInfo->chunkPos + fmtChunkInfo->chunkLength + (fmtChunkInfo->chunkLength & 1)) != (junkChunkInfo->chunkPos - 8))
        {
            // the junk isn't next
            result = PlankResult_UnknownError;
            goto exit;
        }
        
        junkChunkInfo->chunkPos = fmtChunkInfo->chunkPos;
        fmtChunkInfo->chunkPos += junkChunkInfo->chunkLength + 4 + iff->iff.common.headerInfo.lengthSize;
        junkChunkInfo->chunkID.fcc = pl_FourCharCode (ds64);
        
        // update chunk headers writing no other bytes
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, ds64, 0, 0, PLANKIFFFILEWRITER_MODEREPLACEGROW)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, fmt,  0, 0, PLANKIFFFILEWRITER_MODEREPLACEGROW)) != PlankResult_OK) goto exit;
    }

    // "ds64"
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, ds64, &ds64ChunkInfo, 0)) != PlankResult_OK) goto exit;
    
    // write the ds64 stuff...
    if ((result = pl_File_WriteLL    ((PlankFileRef)iff, iff->iff.common.headerInfo.mainLength)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteLL    ((PlankFileRef)iff, dataLength)) != PlankResult_OK) goto exit;
    
    // could search all the chunks for those > 0xffffffff and inlcude a table
    // ... but the mininum size we use for ds64 isn't large enough to accomodate
    if ((result = pl_File_WriteZeros ((PlankFileRef)iff, 12)) != PlankResult_OK) goto exit;

    // "fmt "
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, fmt,  &fmtChunkInfo,  0)) != PlankResult_OK) goto exit;
    
    // regular WAV fmt
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;

    // extensible part
    if ((result = pl_File_WriteUS   ((PlankFileRef)iff, (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH - PLANKAUDIOFILE_WAV_FMT_LENGTH - sizeof (PlankUS)))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS   ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI   ((PlankFileRef)iff, pl_AudioFileWriter_WAVEXT_ChannelIdentifiersToMask (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteGUID ((PlankFileRef)iff, &ext)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_WAV_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    PlankResult result;
    PlankIffFileWriterRef iff;
    
    result = PlankResult_OK;
    iff  = (PlankIffFileWriterRef)p->peer;
    
    if ((result = pl_AudioFileWriter_Iff_WriteFrames (p, convertByteOrder, "data", numFrames, data)) != PlankResult_OK) goto exit;
    
    if (iff->common.headerInfo.mainLength > 0xffffffff)
    {
        if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_AIFF_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;

    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }

    if (p->formatInfo.encoding != PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo) < 1)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample < 8) || (p->formatInfo.bitsPerSample > 32))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.bytesPerFrame == 0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
     
    iff = pl_IffAudioFileWriter_CreateAndInit();
    
    if (filepath)
    {
        result = pl_IffFileWriter_OpenReplacing ((PlankIffFileWriterRef)iff, filepath, PLANK_TRUE, "FORM", "AIFF", PLANKIFFFILE_ID_FCC);
    }
    else
    {
        result = pl_IffFileWriter_OpenWithFile ((PlankIffFileWriterRef)iff, file, "FORM", "AIFF", PLANKIFFFILE_ID_FCC);
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
        
    p->writeFramesFunction = (PlankM)pl_AudioFileWriter_AIFF_WriteFrames;
    p->writeHeaderFunction = (PlankM)pl_AudioFileWriter_AIFF_WriteHeader;
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFF_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_AIFF_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_AIFF_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_AIFF_OpenInternal (p, 0, file);
}

PlankResult pl_AudioFileWriter_AIFF_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    const char* COMM;
    const char* SSND;
    const char* FREE;
    PlankF80 sampleRate;
    int chunkHeaderLength;
    
    iff = (PlankIffAudioFileWriterRef)p->peer;
    chunkHeaderLength = iff->iff.common.headerInfo.headerLength;

    COMM = "COMM";
    SSND = "SSND";
    FREE = "    ";
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, COMM, 0, PLANKAUDIOFILE_AIFF_COMM_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        
        if (p->headerPad > 0)
        {
            if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, FREE, 0, p->headerPad, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        }

        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        p->metaDataChunkPosition = chunkInfo->chunkPos + PLANKAUDIOFILE_AIFF_COMM_LENGTH;
        p->dataPosition = p->metaDataChunkPosition + (p->headerPad > 0 ? p->headerPad + chunkHeaderLength : 0) + chunkHeaderLength + p->dataOffset; // plus blocksize and offset values
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
        
    if (p->numFrames > 0xffffffff)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, COMM, PLANKAUDIOFILE_AIFF_COMM_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    sampleRate = pl_I2F80 ((PlankUI)p->formatInfo.sampleRate);
    
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write   ((PlankFileRef)iff, sampleRate.data, sizeof (sampleRate))) != PlankResult_OK) goto exit;
    
    if ((result = pl_AudioFileWriter_AIFFAIFC_WriteMetaData (p)) != PlankResult_OK) goto exit;
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, SSND, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        // write zero blocksize and offset 8 bytes into SSND
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, SSND, 0, 8, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, SSND, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }

exit:
    return result;    
}

PlankResult pl_AudioFileWriter_AIFF_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, convertByteOrder, "SSND", numFrames, data);
}

static PlankResult pl_AudioFileWriter_AIFC_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    int mode;
    
    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }

    if (pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo) < 1)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample > 32) &&
        (p->formatInfo.encoding != PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample < 8) || (p->formatInfo.bitsPerSample > 64))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.bytesPerFrame == 0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    iff = pl_IffAudioFileWriter_CreateAndInit();
    
    if (filepath)
    {
        result = pl_IffFileWriter_OpenReplacing ((PlankIffFileWriterRef)iff, filepath, PLANK_TRUE, "FORM", "AIFC", PLANKIFFFILE_ID_FCC);
    }
    else
    {
        if ((result = pl_File_GetMode (file, &mode)) != PlankResult_OK) goto exit;
        
        if (!(mode & PLANKFILE_BIGENDIAN))
        {
            result = PlankResult_FileInvalid;
            goto exit;
        }

        result = pl_IffFileWriter_OpenWithFile ((PlankIffFileWriterRef)iff, file, "FORM", "AIFC", PLANKIFFFILE_ID_FCC);
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
        
    p->writeFramesFunction = (PlankM)pl_AudioFileWriter_AIFC_WriteFrames;
    p->writeHeaderFunction = (PlankM)pl_AudioFileWriter_AIFC_WriteHeader;
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFC_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_AIFC_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_AIFC_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_AIFC_OpenInternal (p, 0, file);
}

PlankResult pl_AudioFileWriter_AIFC_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    const char* COMM;
    const char* FVER;
    const char* SSND;
    const char* FREE;
    PlankFourCharCode compressionID;
    PlankF80 sampleRate;
    PlankUI fver;
    int chunkHeaderLength;
    
    iff = (PlankIffAudioFileWriterRef)p->peer;
    chunkHeaderLength = iff->iff.common.headerInfo.headerLength;

    COMM = "COMM";
    FVER = "FVER";
    SSND = "SSND";
    FREE = "    ";
    fver = PLANKAUDIOFILE_AIFC_VERSION;
    
#if PLANK_LITTLEENDIAN
    pl_SwapEndianUI (&fver);
#endif
    
    if (p->numFrames > 0xffffffff)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
        
    if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, FVER, &fver, sizeof (fver), PLANKIFFFILEWRITER_MODEREPLACEGROW)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, COMM, 0, PLANKAUDIOFILE_AIFC_COMM_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;

        if (p->headerPad > 0)
        {
            if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, FREE, 0, p->headerPad, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        }

        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        p->metaDataChunkPosition = chunkInfo->chunkPos + PLANKAUDIOFILE_AIFF_COMM_LENGTH;
        p->dataPosition = p->metaDataChunkPosition + (p->headerPad > 0 ? p->headerPad + 8 : 0) + 8 + p->dataOffset; // plus blocksize and offset values

        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
        
    sampleRate = pl_I2F80 ((PlankUI)p->formatInfo.sampleRate);
    
    switch (p->formatInfo.encoding) {
        case PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN:
            compressionID = pl_FourCharCode ("NONE");
            break;
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:
            compressionID = pl_FourCharCode ("sowt");
            break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN:
            if (p->formatInfo.bitsPerSample == 32)
            {
                compressionID = pl_FourCharCode ("fl32");
            }
            else if (p->formatInfo.bitsPerSample == 64)
            {
                compressionID = pl_FourCharCode ("fl64");
            }
            else
            {
                result = PlankResult_AudioFileInavlidType;
                goto exit;
            }
                
            break;
		default:
			result = PlankResult_AudioFileInavlidType;
                goto exit;
    }
    
    if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, COMM, PLANKAUDIOFILE_AIFC_COMM_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write   ((PlankFileRef)iff, sampleRate.data, sizeof (sampleRate))) != PlankResult_OK) goto exit;
    
    if ((result = pl_File_WriteFourCharCode ((PlankFileRef)p->peer, compressionID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, 0)) != PlankResult_OK) goto exit; // compression description as a pascal string
    
    if ((result = pl_AudioFileWriter_AIFFAIFC_WriteMetaData (p)) != PlankResult_OK) goto exit;
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, SSND, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        // write zero blocksize and offset 8 bytes
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, SSND, 0, 8, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, SSND, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }

exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFC_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, convertByteOrder, "SSND", numFrames, data);
}

static PlankResult pl_AudioFileWriter_CAF_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    int mode;
    
    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if (pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo) < 1)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample > 32) &&
        !(p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_FLOAT_FLAG))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample < 8) || (p->formatInfo.bitsPerSample > 64))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.bytesPerFrame == 0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    iff = pl_IffAudioFileWriter_CreateAndInit();
    
    if (filepath)
    {
        result = pl_IffFileWriter_OpenReplacing ((PlankIffFileWriterRef)iff, filepath, PLANK_TRUE, "caff", "", PLANKIFFFILE_ID_FCC);
    }
    else
    {
        if ((result = pl_File_GetMode (file, &mode)) != PlankResult_OK) goto exit;
        
        if (!(mode & PLANKFILE_BIGENDIAN))
        {
            result = PlankResult_FileInvalid;
            goto exit;
        }

        result = pl_IffFileWriter_OpenWithFile ((PlankIffFileWriterRef)iff, file, "caff", "", PLANKIFFFILE_ID_FCC);
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
    
    p->writeFramesFunction = (PlankM)pl_AudioFileWriter_CAF_WriteFrames;
    p->writeHeaderFunction = (PlankM)pl_AudioFileWriter_CAF_WriteHeader;
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_CAF_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_CAF_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_CAF_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_CAF_OpenInternal (p, 0, file);
}

PlankResult pl_AudioFileWriter_CAF_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    const char* desc;
    const char* data;
    const char* free;
    PlankUI formatFlags;
    int chunkHeaderLength;
    
    iff = (PlankIffAudioFileWriterRef)p->peer;
    chunkHeaderLength = iff->iff.common.headerInfo.headerLength;

    desc   = "desc";
    data   = "data";
    free   = "free";
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, desc, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, desc, 0, PLANKAUDIOFILE_CAF_DESC_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        
        if (p->headerPad > 0)
        {
            if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, free, 0, p->headerPad, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        }

        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, desc, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        p->metaDataChunkPosition = chunkInfo->chunkPos + PLANKAUDIOFILE_CAF_DESC_LENGTH;
        p->dataPosition = p->metaDataChunkPosition + (p->headerPad > 0 ? p->headerPad + chunkHeaderLength : 0) + chunkHeaderLength + p->dataOffset; // plus edit count
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
    
    formatFlags = 0;
    
    if (p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_FLOAT_FLAG)
        formatFlags |= PLANKAUDIOFILE_CAF_FLOAT_FLAG;
    
    if (!(p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG))
        formatFlags |= PLANKAUDIOFILE_CAF_LITTLEENDIAN_FLAG;
    
    if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, desc, PLANKAUDIOFILE_CAF_DESC_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    if ((result = pl_File_WriteD ((PlankFileRef)iff, p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteFourCharCode ((PlankFileRef)iff, pl_FourCharCode ("lpcm"))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, formatFlags)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, 1)) != PlankResult_OK) goto exit; // frames per packet
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;

    if ((result = pl_AudioFileWriter_CAF_WritePreDataMetaData (p)) != PlankResult_OK) goto exit;
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, data, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        // write the 4 bytes "edit count"
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, data, 0, 4, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, data, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }    
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_CAF_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, convertByteOrder, "data", numFrames, data);
}

static PlankResult pl_AudioFileWriter_W64_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    
    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if (!((p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN) ||
          (p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN)))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo) < 1)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample > 32) &&
        (p->formatInfo.encoding != PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample < 8) || (p->formatInfo.bitsPerSample > 64))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.bytesPerFrame == 0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    iff = pl_IffAudioFileWriter_CreateAndInit();
    
    if (!iff)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if (filepath)
    {
        result = pl_IffFileWriter_OpenReplacing ((PlankIffFileWriterRef)iff, filepath, PLANK_FALSE, PLANKAUDIOFILE_W64_RIFF_ID, PLANKAUDIOFILE_W64_WAVE_ID, PLANKIFFFILE_ID_GUID);
    }
    else
    {
        result = pl_IffFileWriter_OpenWithFile ((PlankIffFileWriterRef)iff, file, PLANKAUDIOFILE_W64_RIFF_ID, PLANKAUDIOFILE_W64_WAVE_ID, PLANKIFFFILE_ID_FCC);
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
    
    p->writeFramesFunction = (PlankM)pl_AudioFileWriter_W64_WriteFrames;
    p->writeHeaderFunction = (PlankM)pl_AudioFileWriter_W64_WriteHeader;
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_W64_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_W64_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_W64_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_W64_OpenInternal (p, 0, file);
}

PlankResult pl_AudioFileWriter_W64_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankGUID ext;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffAudioFileWriterRef iff;
    int chunkHeaderLength;
    
    iff = (PlankIffAudioFileWriterRef)p->peer;
    chunkHeaderLength = iff->iff.common.headerInfo.headerLength;
    
    switch (p->formatInfo.encoding)
    {
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:   pl_GUID_InitHexString (&ext, "00000001-0000-0010-8000-00aa00389b71"); break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN: pl_GUID_InitHexString (&ext, "00000003-0000-0010-8000-00aa00389b71"); break;
        default: result = PlankResult_AudioFileInavlidType; goto exit;
    }
    
    if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, PLANKAUDIOFILE_W64_FMT_ID, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, PLANKAUDIOFILE_W64_FMT_ID, 0, PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        
        if (p->headerPad > 0)
        {
            if ((result = pl_IffFileWriter_WriteChunk ((PlankIffFileWriterRef)iff, 0, PLANKAUDIOFILE_W64_JUNK_ID, 0, p->headerPad, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        }
        
        if ((result = pl_IffFileWriter_SeekChunk ((PlankIffFileWriterRef)iff, 0, PLANKAUDIOFILE_W64_FMT_ID, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
    
    if ((result = pl_IffFileWriter_ResizeChunk ((PlankIffFileWriterRef)iff, 0, PLANKAUDIOFILE_W64_FMT_ID, PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    // regular WAV fmt
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    
    // extensible part
    if ((result = pl_File_WriteUS   ((PlankFileRef)iff, (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH - PLANKAUDIOFILE_WAV_FMT_LENGTH - sizeof (PlankUS)))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS   ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI   ((PlankFileRef)iff, pl_AudioFileWriter_WAVEXT_ChannelIdentifiersToMask (&p->formatInfo))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteGUID ((PlankFileRef)iff, &ext)) != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_W64_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, convertByteOrder, PLANKAUDIOFILE_W64_DATA_ID, numFrames, data);
}

PlankResult pl_AudioFileWriter_Iff_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const char* chunkID, const int numFrames, const void* data)
{
    PlankUC buffer[PLANKAUDIOFILEWRITER_BUFFERLENGTH];
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankUC* ptr;
    PlankUC* swapPtr;
    int numSamplesRemaining, numSamplesThisTime, numBufferSamples, bytesPerSample, numBytes, i, numChannels;
    
    iff = (PlankIffFileWriterRef)p->peer;
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    bytesPerSample = p->formatInfo.bytesPerFrame / numChannels;

    if ((bytesPerSample == 1) || pl_AudioFileWriter_IsEncodingNativeEndian (p))
    {
        result = pl_IffFileWriter_WriteChunk (iff, 0, chunkID, data, numFrames * p->formatInfo.bytesPerFrame, PLANKIFFFILEWRITER_MODEAPPEND);
        if (result != PlankResult_OK) goto exit;
    }
    else
    {
        numBufferSamples = (PLANKAUDIOFILEWRITER_BUFFERLENGTH / p->formatInfo.bytesPerFrame) * numChannels;
        numSamplesRemaining = numFrames * numChannels;
        ptr = (PlankUC*)data;
        
        // this is unrolled to help optimisation as this is slower than we want anyway: having to swap endianness...!
        switch (bytesPerSample)
        {
            case 2:
                while (numSamplesRemaining > 0)
                {
                    numSamplesThisTime = pl_MinI (numSamplesRemaining, numBufferSamples);
                    numBytes = numSamplesThisTime * bytesPerSample;
                    pl_MemoryCopy (buffer, ptr, numBytes);
                    
                    if (convertByteOrder)
                    {
                        swapPtr = buffer;
                        
                        for (i = 0; i < numSamplesThisTime; ++i)
                        {
                            pl_SwapEndianS ((PlankS*)swapPtr);
                            swapPtr += 2;
                        }
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, 0, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
                    if (result != PlankResult_OK) goto exit;

                    numSamplesRemaining -= numSamplesThisTime;
                    ptr += numBytes;
                }
                break;
            case 3:
                while (numSamplesRemaining > 0)
                {
                    numSamplesThisTime = pl_MinI (numSamplesRemaining, numBufferSamples);
                    numBytes = numSamplesThisTime * bytesPerSample;
                    pl_MemoryCopy (buffer, ptr, numBytes);
                    
                    // not sure why!
                    
                    if (convertByteOrder)
                    {
                        swapPtr = buffer;
                        
                        for (i = 0; i < numSamplesThisTime; ++i)
                        {
                            pl_SwapEndianI24 ((PlankI24*)swapPtr);
                            swapPtr += 3;
                        }
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, 0, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
                    if (result != PlankResult_OK) goto exit;
                    
                    numSamplesRemaining -= numSamplesThisTime;
                    ptr += numBytes;
                }
                break;
            case 4:
                while (numSamplesRemaining > 0)
                {
                    numSamplesThisTime = pl_MinI (numSamplesRemaining, numBufferSamples);
                    numBytes = numSamplesThisTime * bytesPerSample;
                    pl_MemoryCopy (buffer, ptr, numBytes);
                    
                    if (convertByteOrder)
                    {
                        swapPtr = buffer;
                        
                        for (i = 0; i < numSamplesThisTime; ++i)
                        {
                            pl_SwapEndianI ((PlankI*)swapPtr);
                            swapPtr += 4;
                        }
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, 0, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
                    if (result != PlankResult_OK) goto exit;
                    
                    numSamplesRemaining -= numSamplesThisTime;
                    ptr += numBytes;
                }
                break;
            case 8:
                while (numSamplesRemaining > 0)
                {
                    numSamplesThisTime = pl_MinI (numSamplesRemaining, numBufferSamples);
                    numBytes = numSamplesThisTime * bytesPerSample;
                    pl_MemoryCopy (buffer, ptr, numBytes);
                    
                    if (convertByteOrder)
                    {
                        swapPtr = buffer;
                        
                        for (i = 0; i < numSamplesThisTime; ++i)
                        {
                            pl_SwapEndianLL ((PlankLL*)swapPtr);
                            swapPtr += 8;
                        }
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, 0, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
                    if (result != PlankResult_OK) goto exit;
                    
                    numSamplesRemaining -= numSamplesThisTime;
                    ptr += numBytes;
                }
                break;
            default:
                result = PlankResult_AudioFileInavlidType;
                goto exit;
        }        
    }
    
    if ((result = pl_IffFileWriter_SeekChunk (iff, p->dataPosition, chunkID, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (chunkInfo)
        p->dataPosition = pl_MaxLL (p->dataPosition, chunkInfo->chunkPos);
    
exit:
    return result;
}

#if PLANK_OGGVORBIS || PLANK_OPUS

static PlankResult pl_AudioFileWriter_Ogg_WritePage (PlankFileRef file, const ogg_page* og)
{
    PlankResult result = PlankResult_OK;
    
    if ((result = pl_File_Write (file, og->header, (int)og->header_len)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write (file, og->body,   (int)og->body_len))   != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_Ogg_CommentAddTag (PlankAudioFileWriterRef p, const char* key, const char* string);

#endif

#if PLANK_OGGVORBIS

#include "../../containers/plank_DynamicArray.h"
typedef struct PlankOggVorbisFileWriter
{
    PlankFile file;
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op;
    vorbis_info vi;
    vorbis_comment vc;
    vorbis_dsp_state vd;
    vorbis_block vb;
} PlankOggVorbisFileWriter;

typedef PlankOggVorbisFileWriter* PlankOggVorbisFileWriterRef;

static PlankResult pl_AudioFileWriter_OggVorbis_CommentAddTag (PlankAudioFileWriterRef p, const char* key, const char* string)
{
    PlankOggVorbisFileWriterRef ogg;
 
    ogg = (PlankOggVorbisFileWriterRef)p->peer;
    
    if (strlen (string) > 0)
    {
        vorbis_comment_add_tag (&ogg->vc, key, string);
    }
    
    return PlankResult_OK;
}

static PlankResult pl_AudioFileWriter_OggVorbis_Clear (PlankOggVorbisFileWriterRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();

    ogg_stream_clear (&p->os);
    vorbis_block_clear (&p->vb);
    vorbis_dsp_clear (&p->vd);
    vorbis_comment_clear (&p->vc);
    vorbis_info_clear (&p->vi);
    
    result = pl_Memory_Free (m, p);

    return result;
}

PlankResult pl_AudioFileWriter_OggVorbis_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    PlankResult result;
    PlankOggVorbisFileWriterRef ogg;
    PlankRNGRef rng;
    PlankMemoryRef m;    
    int err, mode, numChannels;
    
    result = PlankResult_OK;
    
    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    
    if ((numChannels < 1) || (numChannels > 255))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (!(p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_FLOAT_FLAG))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
        
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_AudioFileNotReady;
        goto exit;
    }
    
    m = pl_MemoryGlobal();
    ogg = (PlankOggVorbisFileWriterRef)pl_Memory_AllocateBytes (m, sizeof (PlankOggVorbisFileWriter));
    
    if (ogg == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (ogg, sizeof (PlankOggVorbisFileWriter));
    
    if (filepath)
    {
        if ((result = pl_File_Init ((PlankFileRef)ogg)) != PlankResult_OK) goto exit;
        if ((result = pl_File_OpenBinaryWrite ((PlankFileRef)ogg, filepath, PLANK_FALSE, PLANK_TRUE, PLANK_FALSE)) != PlankResult_OK) goto exit;
    }
    else
    {
        if ((result = pl_File_GetMode (file, &mode)) != PlankResult_OK) goto exit;

        if (mode & PLANKFILE_BIGENDIAN)
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }
        
        if (!(mode & PLANKFILE_BINARY))
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }
        
        if (!(mode & PLANKFILE_WRITE))
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }

        pl_MemoryCopy (&ogg->file, file, sizeof (PlankFile));
        pl_MemoryZero (file, sizeof (PlankFile));
    }
    
    vorbis_info_init (&ogg->vi);
    
    if (p->formatInfo.nominalBitRate != 0)
    {
        err = vorbis_encode_init (&ogg->vi,
                                  numChannels,
                                  (int)p->formatInfo.sampleRate,
                                  p->formatInfo.maximumBitRate,
                                  p->formatInfo.nominalBitRate,
                                  p->formatInfo.minimumBitRate);
    }
    else
    {
        err = vorbis_encode_init_vbr (&ogg->vi,
                                      numChannels,
                                      (int)p->formatInfo.sampleRate,
                                      pl_ClipF (p->formatInfo.quality * 0.1f, -0.1f, 1.f));
    }
    
    if (!err)
    {
        p->peer = ogg;

        vorbis_comment_init (&ogg->vc);
        pl_AudioFileWriter_OggVorbis_CommentAddTag (p, "ENCODER", "Plink|Plonk|Plank");
        
        if (p->metaData)
        {
            if ((result = pl_AudioFileWriter_OggVorbis_WriteMetaData (p)) != PlankResult_OK) goto exit;
        }
        
        vorbis_analysis_init (&ogg->vd, &ogg->vi);
        vorbis_block_init (&ogg->vd, &ogg->vb);
        
        rng = pl_RNGGlobal();
        ogg_stream_init (&ogg->os, pl_RNG_Next (rng));
        
        vorbis_analysis_headerout (&ogg->vd, &ogg->vc, &header, &header_comm, &header_code);
        
        ogg_stream_packetin (&ogg->os, &header);
        ogg_stream_packetin (&ogg->os, &header_comm);
        ogg_stream_packetin (&ogg->os, &header_code);
        
        do
        {
            if ((err = ogg_stream_flush (&ogg->os, &ogg->og)) != 0)
            {
                if ((result = pl_AudioFileWriter_Ogg_WritePage ((PlankFileRef)ogg, &ogg->og)) != PlankResult_OK) goto exit;
            }
        } while (err != 0);
                
        p->writeFramesFunction = (PlankM)pl_AudioFileWriter_OggVorbis_WriteFrames;
        p->writeHeaderFunction = 0;
    }

exit:
    if ((p->peer == 0) && (ogg != 0))
    {
        pl_AudioFileWriter_OggVorbis_Clear (ogg);
    }
    
    return result;
}

PlankResult pl_AudioFileWriter_OggVorbis_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_OggVorbis_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_OggVorbis_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_OggVorbis_OpenInternal (p, 0, file);
}

static PlankResult pl_AudioFileWriter_OggVorbis_WriteData (PlankAudioFileWriterRef p, const int count)
{
    PlankResult result;
    PlankOggVorbisFileWriterRef ogg;
    int err;
    
    ogg = (PlankOggVorbisFileWriterRef)p->peer;

    vorbis_analysis_wrote (&ogg->vd, count);
    
    while (vorbis_analysis_blockout (&ogg->vd, &ogg->vb) == 1)
    {
        vorbis_analysis (&ogg->vb, 0);
        vorbis_bitrate_addblock (&ogg->vb);
        
        while (vorbis_bitrate_flushpacket (&ogg->vd, &ogg->op))
        {
            ogg_stream_packetin (&ogg->os, &ogg->op);
            
            do
            {
                if ((err = ogg_stream_pageout (&ogg->os, &ogg->og)) != 0)
                {
                    if ((result = pl_AudioFileWriter_Ogg_WritePage ((PlankFileRef)ogg, &ogg->og)) != PlankResult_OK) goto exit;
                }
            } while ((err != 0) && (ogg_page_eos (&ogg->og) == 0));
        }
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_OggVorbis_Close (PlankAudioFileWriterRef p)
{
    PlankResult result;
    PlankOggVorbisFileWriterRef ogg;

    result = PlankResult_OK;
    ogg = (PlankOggVorbisFileWriterRef)p->peer;
    
    if (ogg)
    {
        pl_AudioFileWriter_OggVorbis_WriteData (p, 0);
        if ((result = pl_File_DeInit ((PlankFileRef)ogg)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileWriter_OggVorbis_Clear (ogg)) != PlankResult_OK) goto exit;
    }
    
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_OggVorbis_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    PlankResult result;
    PlankOggVorbisFileWriterRef ogg;
    float** buffer;
    float* dst;
    const float* src;
    int channel, i, numChannels;
    
    (void)convertByteOrder;
    
    result = PlankResult_OK;
    ogg = (PlankOggVorbisFileWriterRef)p->peer;

    if (numFrames > 0)
    {
        buffer = vorbis_analysis_buffer (&ogg->vd, numFrames);
        numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
        
        for (channel = 0; channel < numChannels; ++channel)
        {
            src = (const float*)data + channel;
            dst = buffer[channel];
            
            for (i = 0; i < numFrames; ++i)
            {
                dst[i] = *src;
                src += numChannels;
            }
        }        
    }
    
    result = pl_AudioFileWriter_OggVorbis_WriteData (p, numFrames);
    
exit:
    return result;
}
#endif // PLANK_OGGVORBIS


#if PLANK_OPUS

#include "../../containers/plank_DynamicArray.h"
typedef struct PlankOpusFileWriter
{
    PlankFile file;
    OpusHeader header;
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op;
    OpusMSEncoder* oe;
    
    PlankDynamicArray comments;
    PlankFile commentWriter;
    
    int frameSize;
    int frameSize48kHz;
    PlankDynamicArray packet;
    PlankDynamicArray buffer;
    int bufferPos;
    
    int totalNumSegments;
    PlankLL totalPCMFrames;
    PlankLL currentGranulePos;
    PlankLL lastPageGranulePos;
    
} PlankOpusFileWriter;

typedef PlankOpusFileWriter* PlankOpusFileWriterRef;

static PlankResult pl_AudioFileWriter_Opus_Clear (PlankOpusFileWriterRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    ogg_stream_clear (&p->os);
    opus_multistream_encoder_destroy (p->oe);
    
    result = pl_Memory_Free (m, p);
    
    return result;
}

static PlankResult pl_AudioFileWriter_Opus_CommentAdd (PlankAudioFileWriterRef p, const char *tag, const char *value)
{
    PlankResult result;
    PlankOpusFileWriterRef opus;
    int vendorLength, numComments, tagLength, valueLength;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileWriterRef)p->peer;
    
    pl_File_SetPosition (&opus->commentWriter, 8);
    pl_File_ReadI (&opus->commentWriter, &vendorLength);
    
    pl_File_SetPosition (&opus->commentWriter, 8 + 4 + vendorLength);
    pl_File_ReadI (&opus->commentWriter, &numComments);

    tagLength = (int)(tag ? strlen (tag) + 1 : 0);
    valueLength = (int)strlen (value);
    
    pl_File_SetPositionEnd (&opus->commentWriter);
    pl_File_WriteI (&opus->commentWriter, tagLength + valueLength);
    
    if (tag)
    {
        pl_File_Write  (&opus->commentWriter, tag, tagLength - 1);
        pl_File_WriteC (&opus->commentWriter, '=');
    }
    
    pl_File_Write (&opus->commentWriter, value, valueLength);

    pl_File_SetPosition (&opus->commentWriter, 8 + 4 + vendorLength);
    pl_File_WriteI (&opus->commentWriter, numComments + 1);
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_Opus_OpenInternal (PlankAudioFileWriterRef p, const char* filepath, PlankFileRef file)
{
    unsigned char headerData[100];
    const char* vendor;
    PlankResult result;
    PlankOpusFileWriterRef opus;
    PlankRNGRef rng;
    PlankMemoryRef m;
    int err, delay, bitRate, streamCount, coupledStreamCount, quality, headerPacketSize, i, mode, numChannels;
    opus_int32 sampleRate;
    PlankChannelIdentifier* channelIdentifiers;
    OpusEncoder *oeStream;

    result = PlankResult_OK;
    opus = PLANK_NULL;
    
    if (((filepath) && (file)) || ((filepath == 0) && (file == 0)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    
    if ((numChannels < 1) || (numChannels > 255))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (!(p->formatInfo.encoding & PLANKAUDIOFILE_ENCODING_FLOAT_FLAG))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
        
    sampleRate = (opus_int32)p->formatInfo.sampleRate;
    
    switch (sampleRate)
    {
        case 8000: case 12000: case 16000: case 24000: case 48000: break;
        default:
            result = PlankResult_AudioFileUnsupportedType;
            goto exit;
    }
    
    m = pl_MemoryGlobal();
    opus = (PlankOpusFileWriterRef)pl_Memory_AllocateBytes (m, sizeof (PlankOpusFileWriter));
    
    if (opus == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (opus, sizeof (PlankOpusFileWriter));
    
    if (filepath)
    {
        if ((result = pl_File_Init ((PlankFileRef)opus)) != PlankResult_OK) goto exit;
        if ((result = pl_File_OpenBinaryWrite ((PlankFileRef)opus, filepath, PLANK_FALSE, PLANK_TRUE, PLANK_FALSE)) != PlankResult_OK) goto exit;
    }
    else
    {
        if ((result = pl_File_GetMode (file, &mode)) != PlankResult_OK) goto exit;
        
        if (mode & PLANKFILE_BIGENDIAN)
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }
        
        if (!(mode & PLANKFILE_BINARY))
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }
        
        if (!(mode & PLANKFILE_WRITE))
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }
        
        pl_MemoryCopy (&opus->file, file, sizeof (PlankFile));
        pl_MemoryZero (file, sizeof (PlankFile));
    }
    
    p->formatInfo.frameDuration = p->formatInfo.frameDuration == 0.f ? 0.02 : p->formatInfo.frameDuration;
    opus->frameSize48kHz        = (int)(p->formatInfo.frameDuration * 48000.f);
    
    switch (opus->frameSize48kHz) {
        case 120: case 240: case 480: case 960: case 1920: case 2880:
            break;
        default:
            opus->frameSize48kHz = 960;
            p->formatInfo.frameDuration = 0.02;
    }
    
    channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (&p->formatInfo);
    
    if ((numChannels == 1) &&
        (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_MONO))
    {
        streamCount                  = 1;
        coupledStreamCount           = 0;
        opus->header.stream_map[0]   = 0;
        opus->header.channel_mapping = 0; // RTP
    }
    else if ((numChannels == 2) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT))
    {
        streamCount                  = 1;
        coupledStreamCount           = 1;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 1;
        opus->header.channel_mapping = 0; // RTP
    }
    else if ((numChannels == 3) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_CENTER) &&
             (channelIdentifiers[2] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT))
    {
        streamCount                  = 1;
        coupledStreamCount           = 1;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 2;
        opus->header.stream_map[2]   = 1;
        opus->header.channel_mapping = 1; // Vorbis mapping
    }
    else if ((numChannels == 4) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT) &&
             (channelIdentifiers[2] == PLANKAUDIOFILE_CHANNEL_BACK_LEFT) &&
             (channelIdentifiers[3] == PLANKAUDIOFILE_CHANNEL_BACK_RIGHT))
    {
        streamCount                  = 2;
        coupledStreamCount           = 2;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 1;
        opus->header.stream_map[2]   = 2;
        opus->header.stream_map[3]   = 3;
        opus->header.channel_mapping = 1; // Vorbis mapping
    }
    else if ((numChannels == 5) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_CENTER) &&
             (channelIdentifiers[2] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT) &&
             (channelIdentifiers[3] == PLANKAUDIOFILE_CHANNEL_BACK_LEFT) &&
             (channelIdentifiers[4] == PLANKAUDIOFILE_CHANNEL_BACK_RIGHT))
    {
        streamCount                  = 3;
        coupledStreamCount           = 2;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 4;
        opus->header.stream_map[2]   = 1;
        opus->header.stream_map[3]   = 2;
        opus->header.stream_map[4]   = 3;
        opus->header.channel_mapping = 1; // Vorbis mapping
    }
    else if ((numChannels == 6) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_CENTER) &&
             (channelIdentifiers[2] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT) &&
             (channelIdentifiers[3] == PLANKAUDIOFILE_CHANNEL_BACK_LEFT) &&
             (channelIdentifiers[4] == PLANKAUDIOFILE_CHANNEL_BACK_RIGHT) &&
             (channelIdentifiers[5] == PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY))
    {
        streamCount                  = 4;
        coupledStreamCount           = 2;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 4;
        opus->header.stream_map[2]   = 1;
        opus->header.stream_map[3]   = 2;
        opus->header.stream_map[4]   = 3;
        opus->header.stream_map[5]   = 5;
        opus->header.channel_mapping = 1; // Vorbis mapping
    }
    else if ((numChannels == 7) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_CENTER) &&
             (channelIdentifiers[2] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT) &&
             (channelIdentifiers[3] == PLANKAUDIOFILE_CHANNEL_SIDE_LEFT) &&
             (channelIdentifiers[4] == PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT) &&
             (channelIdentifiers[5] == PLANKAUDIOFILE_CHANNEL_BACK_CENTER) &&
             (channelIdentifiers[6] == PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY))
    {
        streamCount                  = 5;
        coupledStreamCount           = 2;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 4;
        opus->header.stream_map[2]   = 1;
        opus->header.stream_map[3]   = 2;
        opus->header.stream_map[4]   = 3;
        opus->header.stream_map[5]   = 5;
        opus->header.stream_map[6]   = 6;
        opus->header.channel_mapping = 1; // Vorbis mapping
    }
    else if ((numChannels == 8) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_CENTER) &&
             (channelIdentifiers[2] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT) &&
             (channelIdentifiers[3] == PLANKAUDIOFILE_CHANNEL_SIDE_LEFT) &&
             (channelIdentifiers[4] == PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT) &&
             (channelIdentifiers[5] == PLANKAUDIOFILE_CHANNEL_BACK_CENTER) &&
             (channelIdentifiers[6] == PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY) &&
             (channelIdentifiers[7] == PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY))
    {
        streamCount                  = 5;
        coupledStreamCount           = 3;
        opus->header.stream_map[0]   = 0;
        opus->header.stream_map[1]   = 6;
        opus->header.stream_map[2]   = 1;
        opus->header.stream_map[3]   = 2;
        opus->header.stream_map[4]   = 3;
        opus->header.stream_map[5]   = 4;
        opus->header.stream_map[6]   = 5;
        opus->header.stream_map[7]   = 7;
        opus->header.channel_mapping = 1; // Vorbis mapping
    }
    else
    {
        streamCount                  = numChannels;
        coupledStreamCount           = 0;
        opus->header.channel_mapping = 255;
                
        for (i = 0; i < (numChannels / 2); ++i)
        {                
            if (!pl_AudioFileFormatInfoChannelIdentifiersArePair (channelIdentifiers[i * 2],
                                                                  channelIdentifiers[i * 2 + 1]))
                break; // pairs must be consecutive unless using vorbis mapping

            --streamCount;
            ++coupledStreamCount;
        }
        
        for (i = 0; i < numChannels; ++i)
            opus->header.stream_map[i] = i;
    }
    
    opus->frameSize         = opus->frameSize48kHz * sampleRate / 48000;
    quality                 = pl_ClipI ((int)p->formatInfo.quality, 0, 10);
        
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&opus->packet, 1, PLANKAUDIOFILE_OPUS_MAXPACKETSIZE * streamCount, PLANK_FALSE)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&opus->buffer, sizeof (float), numChannels * opus->frameSize, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    opus->bufferPos          = 0;
    opus->totalPCMFrames     = 0;
    opus->totalNumSegments   = 0;
    opus->currentGranulePos  = 0;
    opus->lastPageGranulePos = opus->currentGranulePos;
    
    opus->oe = opus_multistream_encoder_create (sampleRate,
                                                numChannels,
                                                streamCount,
                                                coupledStreamCount,
                                                opus->header.stream_map,
                                                opus->frameSize < 480 / (48000 / sampleRate) ? OPUS_APPLICATION_RESTRICTED_LOWDELAY : OPUS_APPLICATION_AUDIO,
                                                &err);    

    if (err)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    bitRate = (p->formatInfo.nominalBitRate == 0) ? 64000 * streamCount + 32000 * coupledStreamCount : p->formatInfo.nominalBitRate;

    if ((p->formatInfo.nominalBitRate != 0) &&
        (p->formatInfo.nominalBitRate == p->formatInfo.minimumBitRate) &&
        (p->formatInfo.nominalBitRate == p->formatInfo.maximumBitRate))
    {
        if ((err = opus_multistream_encoder_ctl (opus->oe, OPUS_SET_BITRATE (bitRate))) != 0) { result = PlankResult_AudioFileUnsupportedType; goto exit; }
        if ((err = opus_multistream_encoder_ctl (opus->oe, OPUS_SET_VBR (0))) != 0) { result = PlankResult_UnknownError; goto exit; }
    }
    else
    {
        if ((err = opus_multistream_encoder_ctl (opus->oe, OPUS_SET_BITRATE (bitRate))) != 0) { result = PlankResult_AudioFileUnsupportedType; goto exit; }
        if ((err = opus_multistream_encoder_ctl (opus->oe, OPUS_SET_COMPLEXITY (quality))) != 0) { result = PlankResult_UnknownError; goto exit; }
    }
    
    // use low bandwidth for the LFE and haptic channels
    for (i = 0; i < numChannels; ++i)
    {
        if ((channelIdentifiers[i] == PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY) ||
            (channelIdentifiers[i] == PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA) ||
            (channelIdentifiers[i] == PLANKAUDIOFILE_CHANNEL_HAPTIC))
        {
            oeStream = PLANK_NULL;
            
            if ((err = opus_multistream_encoder_ctl (opus->oe, OPUS_MULTISTREAM_GET_ENCODER_STATE (opus->header.stream_map[i] - coupledStreamCount, &oeStream))) != 0)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            if ((err = opus_encoder_ctl (oeStream, OPUS_SET_MAX_BANDWIDTH (OPUS_BANDWIDTH_NARROWBAND))) != 0)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
        }
    }
    
    if ((err = opus_multistream_encoder_ctl (opus->oe, OPUS_GET_LOOKAHEAD (&delay))) != 0) { result = PlankResult_UnknownError; goto exit; }

    opus->header.version           = 1;
    opus->header.channels          = numChannels;
    opus->header.preskip           = delay * 48000 / sampleRate;
    opus->header.input_sample_rate = sampleRate;
    opus->header.gain              = 0;
    opus->header.nb_streams        = streamCount;
    opus->header.nb_coupled        = coupledStreamCount;
    
    rng = pl_RNGGlobal();
    err = ogg_stream_init (&opus->os, pl_RNG_Next (rng));

    headerPacketSize = opus_header_to_packet (&opus->header, headerData, 100);
    
    opus->op.packet     = headerData;
    opus->op.bytes      = headerPacketSize;
    opus->op.b_o_s      = 1;
    opus->op.e_o_s      = 0;
    opus->op.granulepos = opus->currentGranulePos;
    opus->op.packetno   = 0;
    ogg_stream_packetin (&opus->os, &opus->op);
    
    do
    {
        if ((err = ogg_stream_flush (&opus->os, &opus->og)) != 0)
        {
            if ((result = pl_AudioFileWriter_Ogg_WritePage ((PlankFileRef)opus, &opus->og)) != PlankResult_OK) goto exit;
        }
    } while (err != 0);
    
    p->peer = opus;
    
    pl_DynamicArray_InitWithItemSizeAndCapacity (&opus->comments, 1, 128);
    pl_File_Init (&opus->commentWriter);
    pl_File_OpenDynamicArray (&opus->commentWriter, &opus->comments, PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_READ);
    
    pl_File_WriteString (&opus->commentWriter, PLANKAUDIOFILE_OPUS_TAGS);
    vendor = PLANKAUDIOFILE_OPUS_VENDOR; // opus_get_version_string();
    
    pl_File_WriteI (&opus->commentWriter, strlen (vendor));
    pl_File_WriteString (&opus->commentWriter, vendor);
    pl_File_WriteI (&opus->commentWriter, 0);

    pl_AudioFileWriter_Opus_CommentAdd (p, "ENCODER", "Plink|Plonk|Plank");
    
    if (p->metaData)
    {
        if ((result = pl_AudioFileWriter_Opus_WriteMetaData (p)) != PlankResult_OK) goto exit;
    }
    
    opus->op.packet     = (unsigned char *)pl_DynamicArray_GetArray (&opus->comments);
    opus->op.bytes      = pl_DynamicArray_GetSize (&opus->comments);
    opus->op.b_o_s      = 0;
    opus->op.e_o_s      = 0;
    opus->op.granulepos = opus->currentGranulePos;
    opus->op.packetno   = 1;
    ogg_stream_packetin (&opus->os, &opus->op);
    
    do
    {
        if ((err = ogg_stream_flush (&opus->os, &opus->og)) != 0)
        {
            if ((result = pl_AudioFileWriter_Ogg_WritePage ((PlankFileRef)opus, &opus->og)) != PlankResult_OK) goto exit;
        }
    } while (err != 0);
    
    pl_File_DeInit (&opus->commentWriter);
    pl_DynamicArray_DeInit (&opus->comments);
        
    p->writeFramesFunction = (PlankM)pl_AudioFileWriter_Opus_WriteFrames;
    p->writeHeaderFunction = 0; // no header that needs rewriting later
    
exit:
    if ((p->peer == 0) && (opus != 0))
    {
        pl_AudioFileWriter_Opus_Clear (opus);
    }
    
    return result;
}

PlankResult pl_AudioFileWriter_Opus_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    return pl_AudioFileWriter_Opus_OpenInternal (p, filepath, 0);
}

PlankResult pl_AudioFileWriter_Opus_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file)
{
    return pl_AudioFileWriter_Opus_OpenInternal (p, 0, file);
}

static PlankResult pl_AudioFileWriter_Opus_WritePage (PlankOpusFileWriterRef opus)
{
    PlankResult result = PlankResult_OK;
    
    if (ogg_page_packets (&opus->og) != 0)
        opus->lastPageGranulePos = ogg_page_granulepos (&opus->og);
    
    opus->totalNumSegments -= opus->og.header[PLANKAUDIOFILE_OPUS_HEADERNUMSEGSPOS];
    
    if ((result = pl_AudioFileWriter_Ogg_WritePage ((PlankFileRef)opus, &opus->og)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_Opus_WriteBuffer (PlankOpusFileWriterRef opus)
{
    PlankResult result;
    int ret, bufferLength, numSegments;
    float* buffer;
    unsigned char* packetData;
    opus_int32 packetLength;
    ogg_uint32_t sampleRate;
    int maxOggDelay;
    int numFramesAt48K;
    int framesThisTime;
    
    maxOggDelay = PLANKAUDIOFILE_OPUS_MAXFRAMESIZE;  // for now
    
    result         = PlankResult_OK;
    buffer         = (float*)pl_DynamicArray_GetArray (&opus->buffer);
    bufferLength   = (int)pl_DynamicArray_GetSize (&opus->buffer);
    packetData     = (unsigned char*)pl_DynamicArray_GetArray (&opus->packet);
    packetLength   = (opus_int32)pl_DynamicArray_GetSize (&opus->packet);
    sampleRate     = opus->header.input_sample_rate;
    framesThisTime = opus->bufferPos / opus->header.channels;

    opus->totalPCMFrames += framesThisTime;
    
    if (opus->bufferPos < bufferLength)
    {
        // is last buffer - pad with zeros
        pl_MemoryZero (buffer + opus->bufferPos, (bufferLength - opus->bufferPos) * sizeof (float));
    }

    ret = opus_multistream_encode_float (opus->oe, (float*)buffer, opus->frameSize, packetData, packetLength);
    
    if (ret < 0)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    numSegments = (ret + PLANKAUDIOFILE_OPUS_MAXSEGMENTS) / PLANKAUDIOFILE_OPUS_MAXSEGMENTS;
    numFramesAt48K = opus->frameSize * 48000 / sampleRate;
    opus->currentGranulePos += numFramesAt48K;
    
    while ((((numSegments <= PLANKAUDIOFILE_OPUS_MAXSEGMENTS) && (opus->totalNumSegments + numSegments > PLANKAUDIOFILE_OPUS_MAXSEGMENTS)) ||
            (opus->currentGranulePos - opus->lastPageGranulePos > maxOggDelay)) &&
          ogg_stream_flush_fill (&opus->os, &opus->og, PLANKAUDIOFILE_OPUS_FLUSHFILLSIZE))
    {        
        if ((result = pl_AudioFileWriter_Ogg_WritePage ((PlankFileRef)opus, &opus->og)) != PlankResult_OK) goto exit;
    }
    
    opus->op.packet     = packetData;
    opus->op.bytes      = ret;
    opus->op.b_o_s      = 0;
    opus->op.e_o_s      = opus->bufferPos < bufferLength ? 1 : 0;
    opus->op.granulepos = opus->op.e_o_s ? ((opus->totalPCMFrames * 48000 + sampleRate - 1) / sampleRate) + opus->header.preskip : opus->currentGranulePos;
    
    opus->op.packetno++;
    ogg_stream_packetin (&opus->os, &opus->op);
    opus->totalNumSegments += numSegments;
    
    while ((opus->op.e_o_s || (opus->currentGranulePos + numFramesAt48K - opus->lastPageGranulePos > maxOggDelay) || (opus->totalNumSegments >= PLANKAUDIOFILE_OPUS_MAXSEGMENTS)) ?
           ogg_stream_flush_fill (&opus->os, &opus->og, PLANKAUDIOFILE_OPUS_FLUSHFILLSIZE) : ogg_stream_pageout_fill (&opus->os, &opus->og, PLANKAUDIOFILE_OPUS_FLUSHFILLSIZE))
    {
        if ((result = pl_AudioFileWriter_Opus_WritePage (opus)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_Opus_Close (PlankAudioFileWriterRef p)
{
    PlankResult result;
    PlankOpusFileWriterRef opus;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileWriterRef)p->peer;
    
    if (opus)
    {
        if ((result = pl_AudioFileWriter_Opus_WriteBuffer (opus)) != PlankResult_OK) goto exit;

        if ((result = pl_File_DeInit ((PlankFileRef)opus)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_DeInit (&opus->packet)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_DeInit (&opus->buffer)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileWriter_Opus_Clear (opus)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_Opus_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data)
{
    PlankResult result;
    PlankOpusFileWriterRef opus;
    float* buffer;
    const float* src;
    int bufferLength, bufferRemaining, numSamplesRemaining, bufferThisTime;
    int numChannels, numSamples;
    
    (void)convertByteOrder;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileWriterRef)p->peer;
    
    numChannels         = (int)pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    buffer              = (float*)pl_DynamicArray_GetArray (&opus->buffer);
    bufferLength        = (int)pl_DynamicArray_GetSize (&opus->buffer);
    bufferRemaining     = bufferLength - opus->bufferPos;
    numSamples          = numFrames * numChannels;
    numSamplesRemaining = numSamples;
    src                 = (const float*)data;
    
    while (numSamplesRemaining > 0)
    {
        bufferThisTime = pl_MinI (bufferRemaining, numSamplesRemaining);
        pl_MemoryCopy (buffer + opus->bufferPos, src, bufferThisTime * sizeof (float));
        
        bufferRemaining -= bufferThisTime;
        opus->bufferPos += bufferThisTime;
        numSamplesRemaining -= bufferThisTime;
        src += bufferThisTime;
        
        if (opus->bufferPos == bufferLength)
        {
            result = pl_AudioFileWriter_Opus_WriteBuffer (opus);
            bufferRemaining = bufferLength;
            opus->bufferPos = 0;
        }
    }
        
exit:
    return result;
}

#endif // PLANK_OPUS

static PlankResult pl_AudioFileWriter_WAV_WriteChunk_bext (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    char description [257];
    char originator [33];
    char originatorRef [33];
    char originationDate [11];
    char originationTime [9];
    const char* string;
    const char* codingHistory;
    const char* bext;
    int stringLength, codingHistoryLength;
    PlankUI chunkSize;
    PlankLL timeRef;
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    
    pl_MemoryZero (description, 257);
    pl_MemoryZero (originator, 33);
    pl_MemoryZero (originatorRef, 33);
    pl_MemoryZero (originationDate, 11);
    pl_MemoryZero (originationTime, 9);
    
    if ((string = pl_AudioFileMetaData_GetDescriptionComment (p->metaData)) != PLANK_NULL)
    {
        stringLength = (int)strlen (string);
        
        if (stringLength)
            pl_MemoryCopy (description, string, pl_MinI (stringLength, 256));
    }

    if ((string = pl_AudioFileMetaData_GetOriginatorArtist (p->metaData)) != PLANK_NULL)
    {
        stringLength = (int)strlen (string);
        
        if (stringLength)
            pl_MemoryCopy (originator, string, pl_MinI (stringLength, 32));
    }

    if ((string = pl_AudioFileMetaData_GetOriginatorRef (p->metaData)) != PLANK_NULL)
    {
        stringLength = (int)strlen (string);
        
        if (stringLength)
            pl_MemoryCopy (originatorRef, string, pl_MinI (stringLength, 32));
    }
    
    if ((string = pl_AudioFileMetaData_GetOriginationDate (p->metaData)) != PLANK_NULL)
    {
        stringLength = (int)strlen (string);
        
        if (stringLength)
            pl_MemoryCopy (originationDate, string, pl_MinI (stringLength, 10));
    }

    if ((string = pl_AudioFileMetaData_GetOriginationTime (p->metaData)) != PLANK_NULL)
    {
        stringLength = (int)strlen (string);
        
        if (stringLength)
            pl_MemoryCopy (originationTime, string, pl_MinI (stringLength, 8));
    }

    if ((description[0] + originator[0] + originatorRef[0] + originationDate[0] + originationTime[0]) > 0)
    {
        // assume we have BEXT
        bext = "bext";
        
        chunkSize = 256 + 32 + 32 + 10 + 8 + 4 + 4 + 4 + 64 + 190;
        
        codingHistory       = pl_AudioFileMetaData_GetCodingHistory (p->metaData);
        codingHistoryLength = codingHistory ? (int)strlen (codingHistory) + 1 : 0;
        chunkSize          += codingHistoryLength;

        if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, bext, 0, chunkSize)) != PlankResult_OK) goto exit;

        if ((result = pl_File_Write ((PlankFileRef)iff, description, 256)) != PlankResult_OK) goto exit;
        if ((result = pl_File_Write ((PlankFileRef)iff, originator, 32)) != PlankResult_OK) goto exit;
        if ((result = pl_File_Write ((PlankFileRef)iff, originatorRef, 32)) != PlankResult_OK) goto exit;
        if ((result = pl_File_Write ((PlankFileRef)iff, originationDate, 10)) != PlankResult_OK) goto exit;
        if ((result = pl_File_Write ((PlankFileRef)iff, originationTime, 8)) != PlankResult_OK) goto exit;
        
        timeRef = pl_AudioFileMetaData_GetTimeRef (p->metaData);
        
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)((*(PlankULL*)&timeRef) & 0xFFFFFFFF))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)((*(PlankULL*)&timeRef) >> 32))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUS ((PlankFileRef)iff, 1)) != PlankResult_OK) goto exit; // Version 1?
        
        if ((result = pl_File_Write ((PlankFileRef)iff, pl_AudioFileMetaData_GetUMID (p->metaData), 64)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteZeros ((PlankFileRef)iff, 190)) != PlankResult_OK) goto exit;
        
        if (codingHistoryLength > 0)
        {
            if ((result = pl_File_Write ((PlankFileRef)iff, codingHistory, codingHistoryLength)) != PlankResult_OK) goto exit;

            if (codingHistoryLength & 1)
            {
                if ((result = pl_File_WriteUC ((PlankFileRef)iff, 0)) != PlankResult_OK) goto exit;
            }
        }
        
        p->dataPosition = pl_MaxLL (p->dataPosition,
                                    pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + chunkSize, iff->common.headerInfo.alignment)
                                    + iff->common.headerInfo.headerLength
                                    + p->dataOffset);
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_WAV_WriteChunk_smpl (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    const char* smpl;
    PlankUI smplChunkSize;
    PlankUI manufacturer, product, samplePeriod, smpteFormat, smpteOffset;
    PlankI baseNote, detune;
    PlankSharedPtrArrayRef loopPoints;
    PlankAudioFileRegionRef loop;
    PlankDynamicArrayRef extraSamplerData;
    PlankUI numLoops, i, extraSamplerDataSize;

    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;

    loopPoints = pl_AudioFileMetaData_GetLoopPoints (p->metaData);
    numLoops   = (PlankUI)pl_SharedPtrArray_GetLength (loopPoints);
    
    if ((result = pl_AudioFileMetaData_GetSamplerData (p->metaData, &manufacturer, &product, &samplePeriod, &smpteFormat, &smpteOffset)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_GetInstrumentData (p->metaData, &baseNote, &detune, 0, 0, 0, 0, 0)) != PlankResult_OK) goto exit;
    
    if ((manufacturer | product | samplePeriod | smpteFormat | smpteOffset | numLoops) != 0)
    {
        smpl = "smpl";
        
        extraSamplerData = pl_AudioFileMetaData_GetExtraSamplerData (p->metaData);
        extraSamplerDataSize = (PlankUI)pl_DynamicArray_GetSize (extraSamplerData);
        
        smplChunkSize = 36 + (numLoops * 24) + extraSamplerDataSize;

        if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, smpl, 0, smplChunkSize)) != PlankResult_OK) goto exit;
        
        if (!samplePeriod)
            samplePeriod = 1000000000.0 / p->formatInfo.sampleRate;
    
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, manufacturer)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, product)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, samplePeriod)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, baseNote)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, (pl_ClipI (detune, -50, 50) * 32768) / 50)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, smpteFormat)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, smpteOffset)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, numLoops)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, extraSamplerDataSize)) != PlankResult_OK) goto exit; // sampler data

        for (i = 0; i < numLoops; ++i)
        {
            loop = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (loopPoints, i);
            
            if (!loop)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }

            if ((result = pl_File_WriteUI ((PlankFileRef)iff, loop->anchor->cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)loop->regionOptions)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(loop->start->position * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(loop->end->position * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(loop->fraction * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)loop->playCount)) != PlankResult_OK) goto exit;
        }

        if (extraSamplerDataSize > 0)
        {
            if ((result = pl_File_Write ((PlankFileRef)iff, pl_DynamicArray_GetArray (extraSamplerData), extraSamplerDataSize)) != PlankResult_OK) goto exit;
            
            if (extraSamplerDataSize & 1)
            {
                if ((result = pl_File_WriteUC ((PlankFileRef)iff, 0)) != PlankResult_OK) goto exit;
            }
        }
        
        p->dataPosition = pl_MaxLL (p->dataPosition,
                                    pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + smplChunkSize, iff->common.headerInfo.alignment)
                                    + iff->common.headerInfo.headerLength
                                    + p->dataOffset);
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_WAV_WriteChunk_inst (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    const char* inst;
    PlankUI instChunkSize;
    PlankI baseNote, detune, gain, lowNote, highNote, lowVelocity, highVelocity;
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
        
    if ((result = pl_AudioFileMetaData_GetInstrumentData (p->metaData, &baseNote, &detune, &gain, &lowNote, &highNote, &lowVelocity, &highVelocity)) != PlankResult_OK) goto exit;
    
    if (baseNote > 0)
    {
        inst = "inst";
        
        instChunkSize = 7;

        if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, inst, 0, instChunkSize)) != PlankResult_OK) goto exit;
        
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (baseNote, 0, 127))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (detune, -50, 50))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (gain, -64, 64))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (lowNote, 0, 127))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (highNote, 0, 127))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (lowVelocity, 0, 127))) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC ((PlankFileRef)iff, (PlankC)pl_ClipI (highVelocity, 0, 127))) != PlankResult_OK) goto exit;
        
        if ((result = pl_File_WriteUC ((PlankFileRef)iff, 0)) != PlankResult_OK) goto exit; // align to 8 bytes
        
        p->dataPosition = pl_MaxLL (p->dataPosition,
                                    pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + instChunkSize, iff->common.headerInfo.alignment)
                                    + iff->common.headerInfo.headerLength
                                    + p->dataOffset);
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_WAV_WriteChunk_cue (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankSharedPtrArrayRef cuePoints;
    PlankAudioFileCuePointRef cue;
    PlankSharedPtrArrayRef regions;
    PlankAudioFileRegionRef region;
    PlankSharedPtrArrayRef loopPoints;
    PlankAudioFileRegionRef loop;
    PlankUI cueChunkSize;
    int numCues, numRegions, numLoops, totalCues, i;
    PlankFourCharCode dataID;
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    
    cuePoints  = pl_AudioFileMetaData_GetCuePoints (p->metaData);
    numCues    = (int)pl_SharedPtrArray_GetLength (cuePoints);
    regions    = pl_AudioFileMetaData_GetRegions (p->metaData);
    numRegions = (int)pl_SharedPtrArray_GetLength (regions);
    loopPoints = pl_AudioFileMetaData_GetLoopPoints (p->metaData);
    numLoops   = (int)pl_SharedPtrArray_GetLength (loopPoints);
    totalCues  = numCues + numRegions + numLoops;
        
    if (totalCues > 0)
    {
        cueChunkSize = totalCues * 24 + 4;

        if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, "cue ", 0, cueChunkSize)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)totalCues)) != PlankResult_OK) goto exit;
        
        dataID = pl_FourCharCode ("data");
        
        for (i = 0; i < numCues; ++i)
        {
            cue = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (cuePoints, i);
            
            if (!cue)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, cue->cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*order*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteFourCharCode ((PlankFileRef)iff, dataID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*chunkStart*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*blockStart*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(cue->position * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
        }
                
        for (i = 0; i < numRegions; ++i)
        {
            region = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (regions, i);
            
            if (!region)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, region->start->cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*order*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteFourCharCode ((PlankFileRef)iff, dataID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*chunkStart*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*blockStart*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(region->start->position * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
        }
                
        for (i = 0; i < numLoops; ++i)
        {
            loop = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (loopPoints, i);

            if (!loop)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }

            if ((result = pl_File_WriteUI ((PlankFileRef)iff, loop->anchor->cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*order*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteFourCharCode ((PlankFileRef)iff, dataID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*chunkStart*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, 0 /*blockStart*/)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(loop->anchor->position * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
        }
        
        p->dataPosition = pl_MaxLL (p->dataPosition,
                                    pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + cueChunkSize, iff->common.headerInfo.alignment)
                                    + iff->common.headerInfo.headerLength
                                    + p->dataOffset);
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_WAV_WriteChunk_list (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankSharedPtrArrayRef cuePoints;
    PlankAudioFileCuePointRef cue;
    PlankSharedPtrArrayRef regions;
    PlankAudioFileRegionRef region;
    PlankDynamicArray chunkData;
    PlankFile chunkStream;
    PlankUI listChunkSize;
    int numCues, numRegions, i;
    const char* list;
    const char* adtl;
    const char* labl;
    const char* ltxt;
    const char* string;
    PlankUI stringSize;
    
    if (!p->metaData)
        goto exit;
    
    list = "list";
    adtl = "adtl";
    labl = "labl";
    ltxt = "ltxt";
    
    iff  = (PlankIffFileWriterRef)p->peer;
    
    cuePoints  = pl_AudioFileMetaData_GetCuePoints (p->metaData);
    numCues    = (int)pl_SharedPtrArray_GetLength (cuePoints);
    regions    = pl_AudioFileMetaData_GetRegions (p->metaData);
    numRegions = (int)pl_SharedPtrArray_GetLength (regions);
    
    if ((numCues + numRegions) > 0)
    {
        if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (&chunkData, 1, 512)) != PlankResult_OK) goto exit;
        
        pl_File_Init (&chunkStream);
        pl_File_OpenDynamicArray (&chunkStream, &chunkData, PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE);
        
        if ((result = pl_File_WriteFourCharCode (&chunkStream, pl_FourCharCode (adtl))) != PlankResult_OK) goto exit;
            
        for (i = 0; i < numCues; ++i)
        {
            cue = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (cuePoints, i);

            if (!cue)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            string = pl_AudioFileCuePoint_GetLabel (cue);
            if (!string) continue;
            
            stringSize = (PlankUI)strlen (string) + 1;
            if (!stringSize) continue;
            
            if ((result = pl_File_WriteFourCharCode (&chunkStream, pl_FourCharCode (labl))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, 4 + stringSize + (stringSize & 1))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, pl_AudioFileCuePoint_GetID (cue))) != PlankResult_OK) goto exit;
            if ((result = pl_File_Write (&chunkStream, string, stringSize)) != PlankResult_OK) goto exit;
            
            if (stringSize & 1)
            {
                if ((result = pl_File_WriteC (&chunkStream, 0)) != PlankResult_OK) goto exit;
            }
        }
                
        for (i = 0; i < numRegions; ++i)
        {
            region = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (regions, i);
            
            if (!region)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            string = pl_AudioFileRegion_GetLabel (region);
            if (!string) continue;
            
            stringSize = (PlankUI)strlen (string) + 1;
            if (!stringSize) continue;
            
            if ((result = pl_File_WriteFourCharCode (&chunkStream, pl_FourCharCode (ltxt))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, 4 + 4 + 4 + 2 + 2 + 2 + 2 + stringSize + (stringSize & 1))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, region->start->cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, (PlankUI)((region->end->position - region->start->position) * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, pl_FourCharCode (labl))) != PlankResult_OK) goto exit; // purpose
            if ((result = pl_File_WriteUS (&chunkStream, 0)) != PlankResult_OK) goto exit;                      // country
            if ((result = pl_File_WriteUS (&chunkStream, 0)) != PlankResult_OK) goto exit;                      // language
            if ((result = pl_File_WriteUS (&chunkStream, 0)) != PlankResult_OK) goto exit;                      // dialect
            if ((result = pl_File_WriteUS (&chunkStream, 20127)) != PlankResult_OK) goto exit;                  // code page ASCII
            if ((result = pl_File_Write   (&chunkStream, string, stringSize)) != PlankResult_OK) goto exit;
            
            if (stringSize & 1)
            {
                if ((result = pl_File_WriteC (&chunkStream, 0)) != PlankResult_OK) goto exit;
            }
        }
        
        listChunkSize = (PlankUI)pl_DynamicArray_GetSize (&chunkData);
        
        if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, list, pl_DynamicArray_GetArray (&chunkData), listChunkSize)) != PlankResult_OK) goto exit;
                
        p->dataPosition = pl_MaxLL (p->dataPosition,
                                    pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + listChunkSize, iff->common.headerInfo.alignment)
                                    + iff->common.headerInfo.headerLength
                                    + p->dataOffset);
        
        pl_File_DeInit (&chunkStream);
        pl_DynamicArray_DeInit (&chunkData);
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_WAV_WriteMetaData (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    const char* JUNK;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankLL metaDataJunkSize;
    int chunkHeaderLength;

    iff  = (PlankIffFileWriterRef)p->peer;
    JUNK = "JUNK";
    chunkHeaderLength = iff->common.headerInfo.headerLength;
    
    // junk the meta data - dont check errors, some may not exist
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "bext", JUNK);
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "smpl", JUNK);
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "inst", JUNK);
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "cue ", JUNK);
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "list", JUNK);
        
    if ((result = pl_IffFileWriter_SeekChunk (iff, p->metaDataChunkPosition, JUNK, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (chunkInfo)
    {
        metaDataJunkSize = p->dataPosition - p->metaDataChunkPosition - chunkHeaderLength - chunkHeaderLength; // each for junk & data headers 
        if ((result = pl_IffFileWriter_ResizeChunk (iff, p->metaDataChunkPosition, JUNK, metaDataJunkSize, PLANK_TRUE)) != PlankResult_OK) goto exit;
    }
    
    pl_IffFileWriter_PurgeChunkInfos (iff);
    
    if (p->metaData)
    {
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_INFO)
            if ((result = pl_AudioFileWriter_WAV_WriteChunk_bext (p)) != PlankResult_OK) goto exit;
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_SAMPLER)
        {
            if ((result = pl_AudioFileWriter_WAV_WriteChunk_smpl (p)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileWriter_WAV_WriteChunk_inst (p)) != PlankResult_OK) goto exit;
        }
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_CUEPOINTS)
            if ((result = pl_AudioFileWriter_WAV_WriteChunk_cue  (p)) != PlankResult_OK) goto exit;
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT)
            if ((result = pl_AudioFileWriter_WAV_WriteChunk_list (p)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_AIFFAIFC_WriteChunk_MARK (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankSharedPtrArrayRef cuePoints;
    PlankAudioFileCuePointRef cue;
    PlankUI markChunkSize;
    PlankDynamicArray chunkData;
    PlankFile chunkStream;
    int numCues, i;
    const char* MARK;
    const char* label;
    PlankUC stringLength;
    int chunkHeaderLength;
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    chunkHeaderLength = iff->common.headerInfo.headerLength;

    cuePoints  = pl_AudioFileMetaData_GetCuePoints (p->metaData);
    numCues    = (int)pl_SharedPtrArray_GetLength (cuePoints);
    
    if (numCues > 0)
    {
        MARK  = "MARK";
        
        if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (&chunkData, 1, 512)) != PlankResult_OK) goto exit;
        
        pl_File_Init (&chunkStream);
        pl_File_OpenDynamicArray (&chunkStream, &chunkData, PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_BIGENDIAN);
            
        if ((result = pl_File_WriteUS (&chunkStream, (PlankUS)numCues)) != PlankResult_OK) goto exit;
        
        for (i = 0; i < numCues; ++i)
        {
            cue = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (cuePoints, i);
            
            if (!cue)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            if ((result = pl_File_WriteS (&chunkStream, cue->cueID + 1)) != PlankResult_OK) goto exit; // offset by 1 so ids are non-zero according to the spec
            if ((result = pl_File_WriteUI (&chunkStream, (PlankUI)(cue->position * p->formatInfo.sampleRate + 0.5))) != PlankResult_OK) goto exit;
            
            label = (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT) ? pl_AudioFileCuePoint_GetLabel (cue) : PLANK_NULL;
            stringLength = label ? (PlankUC)strlen (label) : 0;
            if ((result = pl_File_WriteUC (&chunkStream, stringLength)) != PlankResult_OK) goto exit;
            
            if (stringLength > 0)
            {
                if ((result = pl_File_Write (&chunkStream, label, stringLength)) != PlankResult_OK) goto exit;
                
                if (!(stringLength & 1)) // inlcudes length byte
                {
                    if ((result = pl_File_WriteUC (&chunkStream, 0)) != PlankResult_OK) goto exit;
                }
            }
        }
        
        markChunkSize = (PlankUI)pl_DynamicArray_GetSize (&chunkData);
        
        if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, MARK, pl_DynamicArray_GetArray (&chunkData), markChunkSize)) != PlankResult_OK) goto exit;
        
        p->dataPosition = pl_MaxLL (p->dataPosition,
                                    pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + markChunkSize, iff->common.headerInfo.alignment)
                                    + iff->common.headerInfo.headerLength
                                    + p->dataOffset);
        
        pl_File_DeInit (&chunkStream);
        pl_DynamicArray_DeInit (&chunkData);
    }
    
exit:
    return result;    
}

static PlankResult pl_AudioFileWriter_AIFFAIFC_WriteChunk_INST (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
	(void)p;  
//exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFFAIFC_WriteMetaData (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    const char* free;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankLL metaDataJunkSize;
    int chunkHeaderLength;
        
    iff  = (PlankIffFileWriterRef)p->peer;
    free = "    ";
    chunkHeaderLength = iff->common.headerInfo.headerLength;
    
    // junk the meta data - dont check errors, some may not exist
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "MARK", free);
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "INST", free);
    
    if ((result = pl_IffFileWriter_SeekChunk (iff, p->metaDataChunkPosition, free, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (chunkInfo)
    {
        metaDataJunkSize = p->dataPosition - p->metaDataChunkPosition - chunkHeaderLength - chunkHeaderLength - p->dataOffset; // one header each for junk & SSND headers and 8 for offset into SSND
        if ((result = pl_IffFileWriter_ResizeChunk (iff, p->metaDataChunkPosition, free, metaDataJunkSize, PLANK_TRUE)) != PlankResult_OK) goto exit;
    }
    
    pl_IffFileWriter_PurgeChunkInfos (iff);
    
    if (p->metaData)
    {
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_CUEPOINTS)
            if ((result = pl_AudioFileWriter_AIFFAIFC_WriteChunk_MARK (p)) != PlankResult_OK) goto exit;
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_SAMPLER)
            if ((result = pl_AudioFileWriter_AIFFAIFC_WriteChunk_INST (p)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_CAF_WriteChunk_chan (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankUI chanChunkSize, numChannels, i;
    PlankDynamicArray chunkData;
    PlankFile chunkStream;
    PlankChannelIdentifier* channelIdentifiers;
    PlankChannelIdentifier  channelIdentifier;
    PlankSpeakerPosition*   speakerPositions;
    PlankSpeakerPosition    speakerPosition;
    int chunkHeaderLength;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    chunkHeaderLength = iff->common.headerInfo.headerLength;

    pl_File_Init (&chunkStream);
    pl_DynamicArray_Init (&chunkData);
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (&p->formatInfo);
    channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (&p->formatInfo);
    speakerPositions = pl_AudioFileFormatInfo_GetChannelCoords (&p->formatInfo);
    
    if ((numChannels == 1) && (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_MONO))
    {
        goto exit; // not required
    }
    else if ((numChannels == 2) &&
             (channelIdentifiers[0] == PLANKAUDIOFILE_CHANNEL_FRONT_LEFT) &&
             (channelIdentifiers[1] == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT))
    {
        goto exit; // not required
    }
        
    if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (&chunkData, 1, 256)) != PlankResult_OK) goto exit;
    
    pl_File_OpenDynamicArray (&chunkStream, &chunkData, PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_BIGENDIAN);
    
    if (1) // could use other techniques to write the channel format
    {
        if ((result = pl_File_WriteUI (&chunkStream, PLANKAUDIOFILE_LAYOUT_USECHANNELDESCRIPTIONS)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUI (&chunkStream, 0)) != PlankResult_OK) goto exit; // bit field
        if ((result = pl_File_WriteUI (&chunkStream, numChannels)) != PlankResult_OK) goto exit;

        for (i = 0; i < numChannels; ++i)
        {
            channelIdentifier = channelIdentifiers[i];
            if ((result = pl_File_WriteUI (&chunkStream, channelIdentifier)) != PlankResult_OK) goto exit;
            
            if ((speakerPositions != PLANK_NULL) && (channelIdentifier == PLANKAUDIOFILE_CHANNEL_USE_COORDS))
                pl_MemoryCopy (&speakerPosition, speakerPositions + i, sizeof (PlankSpeakerPosition));
            else
                pl_MemoryZero (&speakerPosition, sizeof (PlankSpeakerPosition));
            
            if ((result = pl_File_WriteUI (&chunkStream, speakerPosition.flags)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteF  (&chunkStream, speakerPosition.coords.values[0])) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteF  (&chunkStream, speakerPosition.coords.values[1])) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteF  (&chunkStream, speakerPosition.coords.values[2])) != PlankResult_OK) goto exit;
        }
    }
    
    chanChunkSize = (PlankUI)pl_DynamicArray_GetSize (&chunkData);
    
    if ((result = pl_IffFileWriter_PrepareChunk (iff, p->metaDataChunkPosition, "chan", pl_DynamicArray_GetArray (&chunkData), chanChunkSize)) != PlankResult_OK) goto exit;
    
    p->dataPosition = pl_MaxLL (p->dataPosition,
                                pl_AlignLL (pl_IffFileWriter_GetCurrentChunk (iff)->chunkPos + chanChunkSize, iff->common.headerInfo.alignment)
                                + chunkHeaderLength
                                + p->dataOffset);
    
exit:
    pl_File_DeInit (&chunkStream);
    pl_DynamicArray_DeInit (&chunkData);
    
    return result;
}

PlankResult pl_AudioFileWriter_CAF_WritePreDataMetaData (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    const char* free;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankLL metaDataJunkSize;
    int chunkHeaderLength;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    free = "free";
    chunkHeaderLength = iff->common.headerInfo.headerLength;
    
    // junk the meta data - dont check errors, some may not exist
    pl_IffFileWriter_RenameChunk (iff, p->metaDataChunkPosition, "chan", free);
    
    if ((result = pl_IffFileWriter_SeekChunk (iff, p->metaDataChunkPosition, free, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (chunkInfo)
    {
        metaDataJunkSize = p->dataPosition - p->metaDataChunkPosition - chunkHeaderLength - chunkHeaderLength - p->dataOffset;
        if ((result = pl_IffFileWriter_ResizeChunk (iff, p->metaDataChunkPosition, free, metaDataJunkSize, PLANK_TRUE)) != PlankResult_OK) goto exit;
    }
    
    pl_IffFileWriter_PurgeChunkInfos (iff);
    
    // always try for this one - it checks if it is necessary based on the channel layout
    if ((result = pl_AudioFileWriter_CAF_WriteChunk_chan (p)) != PlankResult_OK) goto exit;
    
    if (p->metaData)
    {
        // .. others
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_CAF_WriteChunk_mark (PlankAudioFileWriterRef p, PlankDynamicArrayRef stringIndices, PlankDynamicArrayRef strings)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankSharedPtrArrayRef cuePoints;
    PlankAudioFileCuePointRef cue;
    PlankUI markChunkSize;
    PlankDynamicArray chunkData;
    PlankFile chunkStream;
    PlankUI numCues, i;
    const char* mark;

	(void)strings;
	(void)stringIndices;
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    
    cuePoints  = pl_AudioFileMetaData_GetCuePoints (p->metaData);
    numCues    = (PlankUI)pl_SharedPtrArray_GetLength (cuePoints);
    
    pl_File_Init (&chunkStream);
    pl_DynamicArray_Init (&chunkData);
    
    if (numCues > 0)
    {
        mark  = "mark";
        
        if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (&chunkData, 1, 512)) != PlankResult_OK) goto exit;
        
        pl_File_OpenDynamicArray (&chunkStream, &chunkData, PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_BIGENDIAN);
        
        if ((result = pl_File_WriteUI (&chunkStream, 0)) != PlankResult_OK) goto exit; // smpte time type
        if ((result = pl_File_WriteUI (&chunkStream, numCues)) != PlankResult_OK) goto exit;
                
        for (i = 0; i < numCues; ++i)
        {
            cue = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (cuePoints, i);
            
            if (!cue)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            if ((result = pl_File_WriteUI (&chunkStream, PLANKAUDIOFILE_CAF_MARKERTYPE_GENERIC)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteD  (&chunkStream, cue->position * p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, cue->cueID + 1)) != PlankResult_OK) goto exit;
            
            // smpte time
            if ((result = pl_File_WriteUC (&chunkStream, 0xff)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUC (&chunkStream, 0xff)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUC (&chunkStream, 0xff)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUC (&chunkStream, 0xff)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteUI (&chunkStream, 0xffffffff)) != PlankResult_OK) goto exit;
            
            if ((result = pl_File_WriteUI (&chunkStream, 0)) != PlankResult_OK) goto exit; // channel
        }
        
        markChunkSize = (PlankUI)pl_DynamicArray_GetSize (&chunkData);
        
        if ((result = pl_IffFileWriter_WriteChunk(iff, 0, mark, pl_DynamicArray_GetArray (&chunkData), markChunkSize, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
    }
    
exit:
    pl_File_DeInit (&chunkStream);
    pl_DynamicArray_DeInit (&chunkData);

    return result;
}

static PlankResult pl_AudioFileWriter_CAF_WriteChunk_strg (PlankAudioFileWriterRef p, PlankDynamicArrayRef stringIndices, PlankDynamicArrayRef strings)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankCAFStringID* stringIndicesArray;
    PlankUI stringsDataSize, numStrings, i;
    PlankUI strgChunkSize;
    PlankDynamicArray chunkData;
    PlankFile chunkStream;
    const char* stringsData;
    const char* strg;
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    
    pl_File_Init (&chunkStream);
    pl_DynamicArray_Init (&chunkData);

    stringIndicesArray = (PlankCAFStringID*)pl_DynamicArray_GetArray (stringIndices);
    numStrings = (PlankUI)pl_DynamicArray_GetSize (stringIndices);
    
    if (numStrings > 0)
    {
        strg = "strg";
        stringsData = (const char*)pl_DynamicArray_GetArray (strings);
        stringsDataSize = (PlankUI)pl_DynamicArray_GetSize (strings);

        // size here is only approximate and should slightly over-budget
        if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (&chunkData, 1, stringsDataSize + numStrings * sizeof (PlankCAFStringID))) != PlankResult_OK) goto exit;
        
        pl_File_OpenDynamicArray (&chunkStream, &chunkData, PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_BIGENDIAN);

        if ((result = pl_File_WriteUI (&chunkStream, numStrings)) != PlankResult_OK) goto exit;

        for (i = 0; i < numStrings; ++i)
        {
            if ((result = pl_File_WriteUI (&chunkStream, stringIndicesArray[i].stringID)) != PlankResult_OK) goto exit;
            if ((result = pl_File_WriteLL (&chunkStream, stringIndicesArray[i].offset)) != PlankResult_OK) goto exit;
        }
        
        if ((result = pl_File_Write (&chunkStream, stringsData, stringsDataSize)) != PlankResult_OK) goto exit;

        strgChunkSize = (PlankUI)pl_DynamicArray_GetSize (&chunkData);

        if ((result = pl_IffFileWriter_WriteChunk (iff, 0, strg, pl_DynamicArray_GetArray (&chunkData), strgChunkSize, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
    }

exit:
    pl_File_DeInit (&chunkStream);
    pl_DynamicArray_DeInit (&chunkData);

    return result;
}

PlankResult pl_AudioFileWriter_CAF_WritePostDataMetaData (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankSharedPtrArrayRef cuePoints;
    PlankAudioFileCuePointRef cue;
    PlankSharedPtrArrayRef regions;
    PlankAudioFileRegionRef region;
    PlankSharedPtrArrayRef loopPoints;
    PlankAudioFileRegionRef loop;
    PlankDynamicArray stringIndices, strings;
    PlankCAFStringID stringEntry;
    PlankFile stringsDataStream;
    const char* label;
    PlankUI labelLength, numCues, numRegions, numLoops, totalCues, i;
    
    pl_File_Init (&stringsDataStream);
    pl_DynamicArray_Init (&stringIndices);
    pl_DynamicArray_Init (&strings);
    
    if (!p->metaData)
        goto exit;
    
    iff  = (PlankIffFileWriterRef)p->peer;
    
    cuePoints  = pl_AudioFileMetaData_GetCuePoints (p->metaData);
    numCues    = (PlankUI)pl_SharedPtrArray_GetLength (cuePoints);
    regions    = pl_AudioFileMetaData_GetRegions (p->metaData);
    numRegions = (PlankUI)pl_SharedPtrArray_GetLength (regions);
    loopPoints = pl_AudioFileMetaData_GetLoopPoints (p->metaData);
    numLoops   = (PlankUI)pl_SharedPtrArray_GetLength (loopPoints);
    totalCues  = numCues + numRegions + numLoops;
    
    if (totalCues > 0)
    {
        pl_DynamicArray_InitWithItemSizeAndCapacity (&stringIndices, sizeof (PlankCAFStringID), totalCues);
        pl_DynamicArray_InitWithItemSizeAndCapacity (&strings, 1, 512);
        pl_File_OpenDynamicArray (&stringsDataStream, &strings, PLANKFILE_BINARY | PLANKFILE_WRITE);
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_CUEPOINTS)
        {
            for (i = 0; i < numCues; ++i)
            {
                cue = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (cuePoints, i);
                
                if (!cue)
                {
                    result = PlankResult_UnknownError;
                    goto exit;
                }

                if ((p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT) &&
                    ((label = pl_AudioFileCuePoint_GetLabel (cue)) != PLANK_NULL))
                {
                    if ((labelLength = (PlankUI)strlen (label)) > 0)
                    {
                        stringEntry.stringID = cue->cueID + 1;
                        pl_File_GetPosition (&stringsDataStream, &stringEntry.offset);
                        pl_DynamicArray_AddItem (&stringIndices, &stringEntry);
                        pl_File_Write (&stringsDataStream, label, labelLength + 1);
                    }
                }
            }
        }
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_REGIONS)
        {
            for (i = 0; i < numRegions; ++i)
            {
                region = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (regions, i);
                
                if (!region)
                {
                    result = PlankResult_UnknownError;
                    goto exit;
                }

                if ((p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT) &&
                    ((label = pl_AudioFileRegion_GetLabel (region)) != PLANK_NULL))
                {
                    if ((labelLength = (PlankUI)strlen (label)) > 0)
                    {
                        stringEntry.stringID = region->anchor->cueID + 1;
                        pl_File_GetPosition (&stringsDataStream, &stringEntry.offset);
                        pl_DynamicArray_AddItem (&stringIndices, &stringEntry);
                        pl_File_Write (&stringsDataStream, label, labelLength + 1);
                    }
                }
            }
        }
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_LOOPS)
        {
            for (i = 0; i < numLoops; ++i)
            {
                loop = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (loopPoints, i);
                
                if (!loop)
                {
                    result = PlankResult_UnknownError;
                    goto exit;
                }

                if ((p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT) &&
                    ((label = pl_AudioFileRegion_GetLabel (loop)) != PLANK_NULL))
                {
                    if ((labelLength = (PlankUI)strlen (label)) > 0)
                    {
                        stringEntry.stringID = loop->anchor->cueID + 1;
                        pl_File_GetPosition (&stringsDataStream, &stringEntry.offset);
                        pl_DynamicArray_AddItem (&stringIndices, &stringEntry);
                        pl_File_Write (&stringsDataStream, label, labelLength + 1);
                    }
                }
            }
        }
        
        if ((result = pl_AudioFileWriter_CAF_WriteChunk_mark (p, &stringIndices, &strings)) != PlankResult_OK) goto exit;
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT)
            if ((result = pl_AudioFileWriter_CAF_WriteChunk_strg (p, &stringIndices, &strings)) != PlankResult_OK) goto exit;
    }
    
exit:    
    pl_File_DeInit (&stringsDataStream);
    pl_DynamicArray_DeInit (&stringIndices);
    pl_DynamicArray_DeInit (&strings);

    return result;
}

PlankResult pl_AudioFileWriter_W64_WriteMetaData (PlankAudioFileWriterRef p)
{
	(void)p;
    return PlankResult_UnknownError;
}

#if PLANK_OGGVORBIS || PLANK_OPUS
static PlankResult pl_AudioFileWriter_Ogg_WriteMetaData (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankSharedPtrArrayRef cuePoints;
    PlankAudioFileCuePointRef cue;
    int numCues, i;
    PlankUI chapter;
    char tag[256];
    char timeText[64];
    const char* label;
    double time;
    int hours, minutes, seconds, millis;
        
    if (!p->metaData || !(p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_CUEPOINTS))
        goto exit;
    
    cuePoints  = pl_AudioFileMetaData_GetCuePoints (p->metaData);
    numCues    = (int)pl_SharedPtrArray_GetLength (cuePoints);
           
    for (i = 0; i < numCues; ++i)
    {
        cue = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (cuePoints, i);
        
        if (!cue)
        {
            result = PlankResult_UnknownError;
            goto exit;
        }

        chapter = pl_AudioFileCuePoint_GetID (cue) + 1;
        time = pl_AudioFileCuePoint_GetPosition (cue);
        
        hours = (int)(time / 3600.0);
        
        if (hours > 99)
            continue;
        
        time -= (double)hours;
        minutes = (int)(time / 60.0);
        time -= (double)minutes;
        seconds = (int)time;
        time -= (double)seconds;
        millis = (int)(time * 1000.0);
        
        snprintf (tag, 256, "CHAPTER%03u", chapter);
        snprintf (timeText, 64, "%02d:%02d:%02d.%03d", hours, minutes, seconds, millis);
        pl_AudioFileWriter_Ogg_CommentAddTag (p, tag, timeText);
        
        if (p->metaDataIOFlags & PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT)
        {
            snprintf (tag, 256, "CHAPTER%03uNAME", chapter);
            label =  pl_AudioFileCuePoint_GetLabel (cue);
            pl_AudioFileWriter_Ogg_CommentAddTag (p, tag, label ? label : "");
        }
    }
    
exit:
    return result;
}
#endif

#if PLANK_OGGVORBIS
PlankResult pl_AudioFileWriter_OggVorbis_WriteMetaData (PlankAudioFileWriterRef p)
{
    return pl_AudioFileWriter_Ogg_WriteMetaData (p);
}
#endif // PLANK_OGGVORBIS

#if PLANK_OPUS
PlankResult pl_AudioFileWriter_Opus_WriteMetaData (PlankAudioFileWriterRef p)
{
    return pl_AudioFileWriter_Ogg_WriteMetaData (p);
}
#endif // PLANK_OPUS

#if PLANK_OGGVORBIS || PLANK_OPUS

PlankResult pl_AudioFileWriter_Ogg_CommentAddTag (PlankAudioFileWriterRef p, const char* key, const char* string)
{
    PlankResult result = PlankResult_OK;

    switch (p->formatInfo.format)
    {
#if PLANK_OGGVORBIS
        case PLANKAUDIOFILE_FORMAT_OGGVORBIS:
            result = pl_AudioFileWriter_OggVorbis_CommentAddTag (p, key, string);
            break;
#endif
#if PLANK_OPUS
        case PLANKAUDIOFILE_FORMAT_OPUS:
            result = pl_AudioFileWriter_Opus_CommentAdd (p, key, string);
            break;
#endif
        default:
            result = PlankResult_UnknownError;
    }

    return result;
}

#endif

