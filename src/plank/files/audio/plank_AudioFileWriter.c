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
#include "plank_AudioFileWriter.h"
#include "plank_AudioFileMetaData.h"
#include "plank_AudioFileCuePoint.h"
#include "plank_AudioFileRegion.h"

// private

typedef PlankResult (*PlankAudioFileWriterWriteFramesFunction)(PlankAudioFileWriterRef, const int, const void*);
typedef PlankResult (*PlankAudioFileWriterSetFramePositionFunction)(PlankAudioFileWriterRef, const PlankLL);
typedef PlankResult (*PlankAudioFileWriterGetFramePositionFunction)(PlankAudioFileWriterRef, PlankLL *);

PlankResult pl_AudioFileWriter_WAV_Open (PlankAudioFileWriterRef p, const char* filepath);
PlankResult pl_AudioFileWriter_WAV_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAVEXT_WriteHeader (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_WAV_WriteFrames (PlankAudioFileWriterRef p, const int numFrames, const void* data);

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
            return PlankResult_UnknownError;
    }
    
    p->formatInfo.format        = PLANKAUDIOFILE_FORMAT_WAV;
    p->formatInfo.encoding      = isFloat ? PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN : PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN;
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

PlankResult pl_AudioFileWriter_WAV_Open (PlankAudioFileWriterRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
        
    if (!((p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN) ||
          (p->formatInfo.encoding == PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN)))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((p->formatInfo.numChannels < 1) || (p->formatInfo.numChannels > 2))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample > 32) &&
        (p->formatInfo.encoding != PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((p->formatInfo.bitsPerSample < 8) || (p->formatInfo.bitsPerSample > 64))
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if (p->formatInfo.bytesPerFrame == 0)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if (p->formatInfo.sampleRate <= 0.0)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    iff = pl_IffFileWriter_CreateAndInit();
    
    result = pl_IffFileWriter_OpenReplacing (iff, filepath, PLANK_FALSE, pl_FourCharCode ("RIFF"), pl_FourCharCode ("WAVE"));
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->peer = iff;
    
    if (p->formatInfo.numChannels > 2)
    {
        if ((result = pl_AudioFileWriter_WAVEXT_WriteHeader (p)) != PlankResult_OK) goto exit;
    }
    else
    {
        if ((result = pl_AudioFileWriter_WAV_WriteHeader (p)) != PlankResult_OK) goto exit;
    }
    
    p->writeFramesFunction = pl_AudioFileWriter_WAV_WriteFrames;
    p->setFramePositionFunction = 0; // need ?
    p->getFramePositionFunction = 0; // need ?
    
exit:
    return result;
}

//PlankResult pl_AudioFileWriter_WAV_WriteHeader (PlankAudioFileWriterRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankIffFileWriterRef iff;
//    PlankFourCharCode fmt;
//    
//    PlankUS compression;
//    PlankUS numChannels;
//    PlankUI sampleRate;
//    PlankUI byteRate;
//    PlankUS bytesPerFrame;
//    PlankUS bitsPerSample;
//    
//    iff = (PlankIffFileWriterRef)p->peer;
//    fmt = pl_FourCharCode ("fmt ");
//        
//    compression     = PLANKAUDIOFILE_WAV_COMPRESSION_PCM;
//    numChannels     = p->formatInfo.numChannels;
//    sampleRate      = (PlankUI)p->formatInfo.sampleRate;
//    byteRate        = (PlankUI)(p->formatInfo.bytesPerFrame * (int)p->formatInfo.sampleRate);
//    bytesPerFrame   = (PlankUS)p->formatInfo.bytesPerFrame;
//    bitsPerSample   = (PlankUS)p->formatInfo.bitsPerSample;
//    
//    if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, &compression, sizeof (compression), PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
//    if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, &numChannels, sizeof (numChannels), PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
//    if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, &sampleRate, sizeof (sampleRate), PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
//    if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, &byteRate, sizeof (byteRate), PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
//    if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, &bytesPerFrame, sizeof (bytesPerFrame), PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
//    if ((result = pl_IffFileWriter_WriteChunk (iff, fmt, &bitsPerSample, sizeof (bitsPerSample), PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
//    
//exit:
//    return result;
//}

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
        default: result = PlankResult_UnknownError; goto exit;
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
        default: result = PlankResult_UnknownError; goto exit;
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
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterRef iff;
    
    iff = (PlankIffFileWriterRef)p->peer;
    
    result = pl_IffFileWriter_WriteChunk (iff, pl_FourCharCode ("data"), data, numFrames * p->formatInfo.bytesPerFrame, PLANKIFFFILEWRITER_MODEAPPEND);

    if (result != PlankResult_OK)
        goto exit;
    
exit:
    return result;
}






