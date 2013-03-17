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

#include "../../core/plank_StandardHeader.h"
#include "../plank_File.h"
#include "../plank_IffFileWriter.h"
#include "../../maths/plank_Maths.h"
#include "plank_AudioFileWriter.h"
#include "plank_AudioFileMetaData.h"
#include "plank_AudioFileCuePoint.h"
#include "plank_AudioFileRegion.h"

#define PLANKAUDIOFILEWRITER_BUFFERLENGTH 256

// private

typedef PlankResult (*PlankAudioFileWriterWriteHeaderFunction)(PlankAudioFileWriterRef);
typedef PlankResult (*PlankAudioFileWriterWriteFramesFunction)(PlankAudioFileWriterRef, const int, const void*);
typedef PlankResult (*PlankAudioFileWriterSetFramePositionFunction)(PlankAudioFileWriterRef, const PlankLL);
typedef PlankResult (*PlankAudioFileWriterGetFramePositionFunction)(PlankAudioFileWriterRef, PlankLL *);

PlankResult pl_AudioFileWriter_WriteHeader (PlankAudioFileWriterRef p);

PlankResult pl_AudioFileWriter_WAV_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_WAV_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAVEXT_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAV_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_AIFF_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_AIFF_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_AIFF_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_AIFC_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_AIFC_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_AIFC_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data);

PlankResult pl_AudioFileWriter_Iff_WriteFrames (PlankAudioFileWriterRef p, const PlankFourCharCode chunkID, const int numFrames, const void* data);


//PlankAudioFileWriterRef pl_AudioFileWriter_CreateAndInit()
//{
//    PlankAudioFileWriterRef p;
//    p = pl_AudioFileWriter_Create();
//    
//    if (p != PLANK_NULL)
//    {
//        if (pl_AudioFileWriter_Init (p) != PlankResult_OK)
//            pl_AudioFileWriter_Destroy (p);
//        else
//            return p;
//    }
//    
//    return (PlankAudioFileWriterRef)PLANK_NULL;
//}
//
//PlankAudioFileWriterRef pl_AudioFileWriter_Create()
//{
//    return (PlankAudioFileWriterRef)PLANK_NULL;
//}

PlankResult pl_AudioFileWriter_Init (PlankAudioFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    p->peer                        = PLANK_NULL;
    p->formatInfo.format           = PLANKAUDIOFILE_FORMAT_INVALID;
    p->formatInfo.encoding         = PLANKAUDIOFILE_ENCODING_INVALID;
    p->formatInfo.bitsPerSample    = 0;
    p->formatInfo.bytesPerFrame    = 0;
    p->formatInfo.numChannels      = 0;
    p->formatInfo.sampleRate       = 0.0;
    p->formatInfo.channelMask      = 0;
    p->dataLength                  = 0;
    p->numFrames                   = 0;
    p->dataPosition                = -1;
    
    p->metaData                    = PLANK_NULL;
    
    p->writeFramesFunction         = PLANK_NULL;
    p->setFramePositionFunction    = PLANK_NULL;
    p->getFramePositionFunction    = PLANK_NULL;
    
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
    
    pl_MemoryZero (p, sizeof (PlankAudioFileWriter));
    
exit:
    return result;
}

//PlankResult pl_AudioFileWriter_Destroy (PlankAudioFileWriterRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m = pl_MemoryGlobal();
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    if ((result = pl_AudioFileWriter_DeInit (p)) != PlankResult_OK)
//        goto exit;
//    
//    result = pl_Memory_Free (m, p);    
//    
//exit:
//    return result;
//}

PlankFileRef pl_AudioFileWriter_GetFile (PlankAudioFileWriterRef p)
{
    return (PlankFileRef)p->peer;
}

PlankAudioFileFormatInfo* pl_AudioFileWriter_GetFormat (PlankAudioFileWriterRef p)
{
    return p->peer ? 0 : &p->formatInfo;
}

PlankResult pl_AudioFileWriter_SetFormatWAV (PlankAudioFileWriterRef p, const int bitsPerSample, const int numChannels, const double sampleRate, const PlankB isFloat)
{
    if (p->peer)
        return PlankResult_UnknownError;
    
    if (isFloat)
    {
        if ((bitsPerSample != 32) && (bitsPerSample != 64))
            return PlankResult_AudioFileInavlidType;
    }
    
    p->formatInfo.format        = PLANKAUDIOFILE_FORMAT_WAV;
    p->formatInfo.encoding      = isFloat ? PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN : PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN;
    p->formatInfo.bitsPerSample = bitsPerSample;
    p->formatInfo.numChannels   = numChannels;
    p->formatInfo.sampleRate    = sampleRate;
    p->formatInfo.bytesPerFrame = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatAIFF (PlankAudioFileWriterRef p, const int bitsPerSample, const int numChannels, const double sampleRate)
{
    if (p->peer)
        return PlankResult_UnknownError;
        
    p->formatInfo.format        = PLANKAUDIOFILE_FORMAT_AIFF;
    p->formatInfo.encoding      = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
    p->formatInfo.bitsPerSample = bitsPerSample;
    p->formatInfo.numChannels   = numChannels;
    p->formatInfo.sampleRate    = sampleRate;
    p->formatInfo.bytesPerFrame = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileWriter_SetFormatAIFC (PlankAudioFileWriterRef p, const int bitsPerSample, const int numChannels, const double sampleRate, const PlankB isFloat, const PlankB isLittleEndian)
{
    if (p->peer)
        return PlankResult_UnknownError;
    
    if (isFloat)
    {
        if ((bitsPerSample != 32) && (bitsPerSample != 64))
            return PlankResult_AudioFileInavlidType;
        
        if (isLittleEndian)
            return PlankResult_AudioFileInavlidType;
    }
    
    p->formatInfo.format        = PLANKAUDIOFILE_FORMAT_AIFC;
    p->formatInfo.encoding      = isFloat ? PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN : isLittleEndian ? PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN : PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
    p->formatInfo.bitsPerSample = bitsPerSample;
    p->formatInfo.numChannels   = numChannels;
    p->formatInfo.sampleRate    = sampleRate;
    p->formatInfo.bytesPerFrame = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8);
    
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
        
exit:
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
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
    switch (p->formatInfo.format)
    {
        case PLANKAUDIOFILE_FORMAT_WAV:
        case PLANKAUDIOFILE_FORMAT_AIFF:
        case PLANKAUDIOFILE_FORMAT_AIFC:
        case PLANKAUDIOFILE_FORMAT_UNKNOWNIFF:
            result = pl_IffFileWriter_Destroy ((PlankIffFileWriter*)p->peer);
            break;
        default:
            if (p->peer != PLANK_NULL)
                result = PlankResult_UnknownError;
    }
    
    if (result != PlankResult_OK)
        goto exit;
    
    if (p->metaData != PLANK_NULL)
    {
        if ((result = pl_AudioFileMetaData_Destroy (p->metaData)) != PlankResult_OK) goto exit;
    }
    
    result = pl_File_Close ((PlankFileRef)p->peer);
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data)
{
    if (!p->writeFramesFunction)
        return PlankResult_FunctionsInvalid;
    
    return ((PlankAudioFileWriterWriteFramesFunction)p->writeFramesFunction)(p, numFrames, data);
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
    if (!p->writeHeaderFunction)
        return PlankResult_FunctionsInvalid;
    
    return ((PlankAudioFileWriterWriteHeaderFunction)p->writeHeaderFunction)(p);
}

PlankResult pl_AudioFileWriter_WAV_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
        
    if (!((p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN) ||
          (p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN)))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.numChannels < 1)
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
    
    iff = pl_IffFileWriter_CreateAndInit();
    
    result = pl_IffFileWriter_OpenReplacing (iff, filepath, PLANK_FALSE, pl_FourCharCode ("RIFF"), pl_FourCharCode ("WAVE"));
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
        
    p->writeFramesFunction = pl_AudioFileWriter_WAV_WriteFrames;
    p->writeHeaderFunction = pl_AudioFileWriter_WAV_WriteHeader;
    p->setFramePositionFunction = 0; // need ?
    p->getFramePositionFunction = 0; // need ?
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_WAV_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankFourCharCode fmt;
    PlankUS encoding;
    
    iff = (PlankIffFileWriterRef)p->peer;
    fmt = pl_FourCharCode ("fmt ");
    
    switch (p->formatInfo.encoding)
    {
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:   encoding = PLANKAUDIOFILE_WAV_COMPRESSION_PCM;   break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN: encoding = PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT; break;
        default: result = PlankResult_AudioFileInavlidType; goto exit;
    }
    
    if ((result = pl_IffFileWriter_SeekChunk (iff, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;

    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, 0, PLANKAUDIOFILE_WAV_FMT_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk (iff, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
        
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, encoding)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_AudioFileWriter_WAVEXT_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankAudioFileWAVExtensible* ext;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankFourCharCode fmt;
    
    iff = (PlankIffFileWriterRef)p->peer;
    fmt = pl_FourCharCode ("fmt ");
    
    switch (p->formatInfo.encoding)
    {
        case PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN:   ext = pl_AudioFileWAVExtensible_GetPCM();   break;
        case PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN: ext = pl_AudioFileWAVExtensible_GetFloat(); break;
        default: result = PlankResult_AudioFileInavlidType; goto exit;
    }
    
    if ((result = pl_IffFileWriter_SeekChunk (iff, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, 0, PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk (iff, fmt, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
    
    // regular WAV fmt
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bytesPerFrame)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;

    // extensible part
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH - PLANKAUDIOFILE_WAV_FMT_LENGTH - sizeof (PlankUS)))) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)p->formatInfo.channelMask)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, (PlankUI)ext->ext1)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)ext->ext2)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, (PlankUS)ext->ext3)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write   ((PlankFileRef)iff, ext->ext4, 8)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_AudioFileWriter_WAV_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, pl_FourCharCode ("data"), numFrames, data);
}

PlankResult pl_AudioFileWriter_AIFF_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    
    if (p->formatInfo.encoding != PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN)
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (p->formatInfo.numChannels < 1)
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
    
    iff = pl_IffFileWriter_CreateAndInit();
    
    result = pl_IffFileWriter_OpenReplacing (iff, filepath, PLANK_TRUE, pl_FourCharCode ("FORM"), pl_FourCharCode ("AIFF"));
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
        
    p->writeFramesFunction = pl_AudioFileWriter_AIFF_WriteFrames;
    p->writeHeaderFunction = pl_AudioFileWriter_AIFF_WriteHeader;
    p->setFramePositionFunction = 0; // need ?
    p->getFramePositionFunction = 0; // need ?
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_AIFF_GetNumFrames (PlankAudioFileWriterRef p, PlankUI* numFrames)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;

    iff = (PlankIffFileWriterRef)p->peer;

    if ((result = pl_IffFileWriter_SeekChunk (iff, pl_FourCharCode ("SSND"), &chunkInfo, 0)) != PlankResult_OK) goto exit;

    if (chunkInfo)
    {
        *numFrames = chunkInfo->chunkLength / p->formatInfo.bytesPerFrame;
    }
    else
    {
        *numFrames = 0;
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFF_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankFourCharCode COMM;
    PlankF80 sampleRate;
    PlankUI numFrames;
    
    iff = (PlankIffFileWriterRef)p->peer;
    COMM = pl_FourCharCode ("COMM");
        
    if ((result = pl_IffFileWriter_SeekChunk (iff, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk (iff, COMM, 0, PLANKAUDIOFILE_AIFF_COMM_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk (iff, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
        if (!chunkInfo)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
    }
    
    if ((result = pl_AudioFileWriter_AIFF_GetNumFrames (p, &numFrames)) != PlankResult_OK) goto exit;
    
    sampleRate = pl_I2F80 ((PlankUI)p->formatInfo.sampleRate);
    
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)p->formatInfo.numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write   ((PlankFileRef)iff, sampleRate.data, sizeof (sampleRate))) != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

PlankResult pl_AudioFileWriter_AIFF_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, pl_FourCharCode ("SSND"), numFrames, data);
}

PlankResult pl_AudioFileWriter_AIFC_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    
    if (p->formatInfo.numChannels < 1)
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
    
    iff = pl_IffFileWriter_CreateAndInit();
    
    result = pl_IffFileWriter_OpenReplacing (iff, filepath, PLANK_TRUE, pl_FourCharCode ("FORM"), pl_FourCharCode ("AIFC"));
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
    
    p->writeFramesFunction = pl_AudioFileWriter_AIFC_WriteFrames;
    p->writeHeaderFunction = pl_AudioFileWriter_AIFC_WriteHeader;
    p->setFramePositionFunction = 0; // need ?
    p->getFramePositionFunction = 0; // need ?
    
    if ((result = pl_AudioFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

static PlankResult pl_AudioFileWriter_AIFC_GetNumFrames (PlankAudioFileWriterRef p, PlankUI* numFrames)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    
    iff = (PlankIffFileWriterRef)p->peer;
    
    if ((result = pl_IffFileWriter_SeekChunk (iff, pl_FourCharCode ("SSND"), &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (chunkInfo)
    {
        *numFrames = chunkInfo->chunkLength / p->formatInfo.bytesPerFrame;
    }
    else
    {
        *numFrames = 0;
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFC_WriteHeader (PlankAudioFileWriterRef p)
{
    PlankIffFileWriterChunkInfoRef chunkInfo;
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankFourCharCode COMM, FVER, compressionID;
    PlankF80 sampleRate;
    PlankUI numFrames, fver;
    
    iff = (PlankIffFileWriterRef)p->peer;
    COMM = pl_FourCharCode ("COMM");
    FVER = pl_FourCharCode ("FVER");
    fver = PLANKAUDIOFILE_AIFC_VERSION;
    
#if PLANK_LITTLEENDIAN
    pl_SwapEndianUI (&fver);
#endif
    
    if ((result = pl_AudioFileWriter_AIFC_GetNumFrames (p, &numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_WriteChunk (iff, FVER, &fver, sizeof (fver), PLANKIFFFILEWRITER_MODEREPLACEGROW)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_SeekChunk (iff, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    
    if (!chunkInfo)
    {
        if ((result = pl_IffFileWriter_WriteChunk (iff, COMM, 0, PLANKAUDIOFILE_AIFC_COMM_LENGTH, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk (iff, COMM, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        
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
    }
    
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)p->formatInfo.numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI ((PlankFileRef)iff, numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteS  ((PlankFileRef)iff, (PlankS)p->formatInfo.bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write   ((PlankFileRef)iff, sampleRate.data, sizeof (sampleRate))) != PlankResult_OK) goto exit;
    
    if ((result = pl_File_WriteFourCharCode ((PlankFileRef)p->peer, compressionID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUS ((PlankFileRef)iff, 0)) != PlankResult_OK) goto exit; // compression description as a pascal string
    
exit:
    return result;
}

PlankResult pl_AudioFileWriter_AIFC_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data)
{
    return pl_AudioFileWriter_Iff_WriteFrames (p, pl_FourCharCode ("SSND"), numFrames, data);
}

PlankResult pl_AudioFileWriter_Iff_WriteFrames (PlankAudioFileWriterRef p, const PlankFourCharCode chunkID, const int numFrames, const void* data)
{
    PlankUC buffer[PLANKAUDIOFILEWRITER_BUFFERLENGTH];
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    PlankUC* ptr;
    PlankUC* swapPtr;
    int numSamplesRemaining, numSamplesThisTime, numBufferSamples, bytesPerSample, numBytes, i, numChannels;
    
    iff = (PlankIffFileWriterRef)p->peer;
    numChannels = p->formatInfo.numChannels;
    bytesPerSample = p->formatInfo.bytesPerFrame / numChannels;

    if ((bytesPerSample == 1) || pl_AudioFileWriter_IsEncodingNativeEndian (p))
    {
        result = pl_IffFileWriter_WriteChunk (iff, chunkID, data, numFrames * p->formatInfo.bytesPerFrame, PLANKIFFFILEWRITER_MODEAPPEND);
        if (result != PlankResult_OK) goto exit;
    }
    else
    {
        numBufferSamples = (PLANKAUDIOFILEWRITER_BUFFERLENGTH / p->formatInfo.bytesPerFrame) * p->formatInfo.numChannels;
        numSamplesRemaining = numFrames * p->formatInfo.numChannels;
        ptr = (PlankUC*)data;
        
        // this is unrolled to help optimisation as this is slower that we want anyway: having to swap endianness...!
        switch (bytesPerSample)
        {
            case 2:
                while (numSamplesRemaining > 0)
                {
                    numSamplesThisTime = pl_MinI (numSamplesRemaining, numBufferSamples);
                    numBytes = numSamplesThisTime * bytesPerSample;
                    pl_MemoryCopy (buffer, ptr, numBytes);
                    
                    swapPtr = buffer;
                    
                    for (i = 0; i < numSamplesThisTime; ++i)
                    {
                        pl_SwapEndianS ((PlankS*)swapPtr);
                        swapPtr += 2;
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
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
                    
//                    swapPtr = buffer;
//                    
//                    for (i = 0; i < numSamplesThisTime; ++i)
//                    {
//                        pl_SwapEndianI24 ((PlankI24*)swapPtr);
//                        swapPtr += 3;
//                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
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
                    
                    swapPtr = buffer;
                    
                    for (i = 0; i < numSamplesThisTime; ++i)
                    {
                        pl_SwapEndianI ((PlankI*)swapPtr);
                        swapPtr += 4;
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
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
                    
                    swapPtr = buffer;
                    
                    for (i = 0; i < numSamplesThisTime; ++i)
                    {
                        pl_SwapEndianLL ((PlankLL*)swapPtr);
                        swapPtr += 8;
                    }
                    
                    result = pl_IffFileWriter_WriteChunk (iff, chunkID, buffer, numBytes, PLANKIFFFILEWRITER_MODEAPPEND);
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
    
exit:
    return result;
}




