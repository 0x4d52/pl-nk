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

#include "../../core/plank_StandardHeader.h"
#include "../plank_File.h"
#include "../plank_IffFileReader.h"
#include "plank_AudioFileReader.h"

// private functions
PlankResult pl_AudioFileReader_WAV_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_WAV_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_AIFF_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFF_ParseData(PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_AIFC_ParseVersion (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFC_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFC_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_ParseMain  (PlankAudioFileReaderRef p, const PlankFourCharCode mainID, const PlankFourCharCode formatID);


PlankAudioFileReaderRef pl_AudioFileReader_CreateAndInit()
{
    PlankAudioFileReaderRef p;
    p = pl_AudioFileReader_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_AudioFileReader_Init (p) != PlankResult_OK)
            pl_AudioFileReader_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankAudioFileReaderRef pl_AudioFileReader_Create()
{
    PlankMemoryRef m;
    PlankAudioFileReaderRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAudioFileReaderRef)pl_Memory_AllocateBytes (m, sizeof (PlankAudioFileReader));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAudioFileReader));        
        
    return p;
}

PlankResult pl_AudioFileReader_Init (PlankAudioFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
        
    pl_IffFileReader_Init (&p->iff);
    
    p->formatInfo.format           = PLANKAUDIOFILE_FORMAT_INVALID;
    p->formatInfo.encoding         = PLANKAUDIOFILE_ENCODING_INVALID;
    p->formatInfo.bitsPerSample    = 0;
    p->formatInfo.bytesPerFrame    = 0;
    p->formatInfo.numChannels      = 0;
    p->formatInfo.sampleRate       = 0.0;
    p->numFrames                   = 0;
    p->dataPosition                = -1;
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_DeInit (PlankAudioFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    result = pl_IffFileReader_DeInit (&p->iff);
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Destroy (PlankAudioFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_AudioFileReader_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);    
    
exit:
    return result;
}

PlankFileRef pl_AudioFileReader_GetFile (PlankAudioFileReaderRef p)
{
    return (PlankFileRef)p; // pl_IffFileReader_GetFile (&p->iff); 
}

PlankResult pl_AudioFileReader_Open (PlankAudioFileReaderRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    PlankUI chunkLength;
    PlankLL chunkDataPos;
    PlankFourCharCode mainID, formatID;
    
    // open the file as an IFF
    if ((result = pl_IffFileReader_Open (&p->iff, filepath)) != PlankResult_OK) goto exit;
    
    // deteriming the file format, could be IFF or RIFF
    if ((result = pl_IffFileReader_GetMainID (&p->iff, &mainID)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileReader_GetFormatID (&p->iff, &formatID)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileReader_ParseMain (p, mainID, formatID)) != PlankResult_OK) goto exit;

    // parse based on the format
    if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_WAV)
    {
        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("fmt "), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_WAV_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;

        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("data"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_WAV_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFF)
    {
        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("COMM"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFF_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
        
        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("SSND"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFF_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;        
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFC)
    {
        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("FVER"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFC_ParseVersion (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;

        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("COMM"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFC_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        
        if ((result = pl_IffFileReader_SeekChunk (&p->iff, pl_FourCharCode ("SSND"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFC_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;        
    }
    else
    {
        result = PlankResult_AudioFileReaderInavlidType;
        goto exit;
    }    
    
    if ((result = pl_AudioFileReader_ResetFramePosition (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Close (PlankAudioFileReaderRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_FileCloseFailed;
    
    return pl_File_Close ((PlankFileRef)p); //pl_File_Close (pl_AudioFileReader_GetFile (p));
}

PlankResult pl_AudioFileReader_GetFormat (PlankAudioFileReaderRef p, int *format)
{
    *format = (int)p->formatInfo.format;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_GetEncoding (PlankAudioFileReaderRef p, int *encoding)
{
    *encoding = (int)p->formatInfo.encoding;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_GetBitsPerSample (PlankAudioFileReaderRef p, int *bitsPerSample)
{
    *bitsPerSample = p->formatInfo.bitsPerSample;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_GetBytesPerFrame (PlankAudioFileReaderRef p, int *bytesPerFrame)
{
    *bytesPerFrame = p->formatInfo.bytesPerFrame;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_GetNumChannels (PlankAudioFileReaderRef p, int *numChannels)
{
    *numChannels = p->formatInfo.numChannels;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_GetSampleRate (PlankAudioFileReaderRef p, double *sampleRate)
{
    *sampleRate = p->formatInfo.sampleRate;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_GetNumFrames (PlankAudioFileReaderRef p, int *numFrames)
{
    *numFrames = p->numFrames;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_SetFramePosition (PlankAudioFileReaderRef p, const int frameIndex)
{
    PlankResult result = PlankResult_OK;
    PlankLL pos;
    
    if ((p->dataPosition < 0) || (p->formatInfo.bytesPerFrame <= 0))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    pos = p->dataPosition + frameIndex * p->formatInfo.bytesPerFrame;
    
    if ((result = pl_File_SetPosition ((PlankFileRef)p, pos)) != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

PlankResult pl_AudioFileReader_ResetFramePosition (PlankAudioFileReaderRef p)
{
    return pl_AudioFileReader_SetFramePosition (p, 0);
}

PlankResult pl_AudioFileReader_GetFramePosition (PlankAudioFileReaderRef p, int *frameIndex)
{
    PlankResult result = PlankResult_OK;
    PlankLL pos;
    
    if ((p->dataPosition < 0) || (p->formatInfo.bytesPerFrame <= 0))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    if ((result = pl_File_GetPosition ((PlankFileRef)p, &pos)) != PlankResult_OK) goto exit;

    *frameIndex = (pos - p->dataPosition) / p->formatInfo.bytesPerFrame;
    
exit:
    return result;    
}


PlankResult pl_AudioFileReader_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead)
{
    PlankResult result = PlankResult_OK;
    int startFrame, endFrame, framesToRead, bytesToRead, bytesRead;
    
    if ((p->dataPosition < 0) || (p->formatInfo.bytesPerFrame <= 0))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    if ((result = pl_AudioFileReader_GetFramePosition (p, &startFrame)) != PlankResult_OK) goto exit;
    
    if (startFrame < 0)
    {
        result = PlankResult_AudioFileReaderInvalidFilePosition;
        goto exit;
    }
    
    endFrame = startFrame + numFrames;

    framesToRead = (endFrame > p->numFrames) ? (p->numFrames - startFrame) : (numFrames);
    bytesToRead = framesToRead * p->formatInfo.bytesPerFrame;
    
    result = pl_File_Read ((PlankFileRef)p, data, bytesToRead, &bytesRead);
    
    if (framesRead != PLANK_NULL)
        *framesRead = bytesRead / p->formatInfo.bytesPerFrame;

    // should zero if framesToRead < numFrames
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_WAV_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankUS compressionCode, numChannels;
    PlankUI sampleRate, byteRate;
    PlankUS blockAlign, bitsPerSample;

    if ((result = pl_File_ReadUS ((PlankFileRef)p, &compressionCode)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS ((PlankFileRef)p, &numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p, &sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p, &byteRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS ((PlankFileRef)p, &blockAlign)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS ((PlankFileRef)p, &bitsPerSample)) != PlankResult_OK) goto exit;
    
    if (compressionCode == PLANKAUDIOFILE_WAV_COMPRESSION_PCM)
    {
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN;
    }
    else if (compressionCode == PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT)
    {
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    }
    else if (compressionCode == PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE)
    {
        // must implememnt
        result = PlankResult_AudioFileReaderInavlidType;
        goto exit;
    }
    else 
    {
        result = PlankResult_AudioFileReaderInavlidType;
        goto exit;
    }
    
    // set these last so that if the format is not recognised everything remains uninitialised
    p->formatInfo.bitsPerSample = (PlankI) bitsPerSample;
    p->formatInfo.bytesPerFrame = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8); // round up to whole bytes
    p->formatInfo.numChannels   = (PlankI) numChannels;
    p->formatInfo.sampleRate    = (PlankD) sampleRate;

exit:
    return result;
}

PlankResult pl_AudioFileReader_WAV_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
        
    p->dataPosition = chunkDataPos;
            
    if (p->formatInfo.bytesPerFrame > 0)
        p->numFrames = chunkLength / p->formatInfo.bytesPerFrame;

    if ((chunkLength % p->formatInfo.bytesPerFrame) != 0)
        result = PlankResult_AudioFileReaderDataChunkInvalid;
  
exit:
    return result;
}

PlankResult pl_AudioFileReader_AIFF_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankS numChannels;
    PlankUI numFrames;
    PlankS bitsPerSample;
    PlankF80 sampleRate;

    if ((result = pl_File_ReadS ((PlankFileRef)p, &numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p, &numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadS ((PlankFileRef)p, &bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read ((PlankFileRef)p, sampleRate.data, sizeof (sampleRate), PLANK_NULL)) != PlankResult_OK) goto exit;    
    
    p->formatInfo.bitsPerSample = (PlankI) bitsPerSample;
    p->formatInfo.bytesPerFrame = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8); // round up to whole bytes
    p->formatInfo.numChannels   = (PlankI) numChannels;
    p->formatInfo.sampleRate    = (PlankD) pl_F802I (sampleRate);
    p->numFrames                = (PlankLL) numFrames;

exit:
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_AIFF_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankUI offset, blockSize;
    PlankLL pos;
    
    if ((result = pl_File_ReadUI ((PlankFileRef)p, &offset)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p, &blockSize)) != PlankResult_OK) goto exit;
    if ((result = pl_File_GetPosition ((PlankFileRef)p, &pos)) != PlankResult_OK) goto exit;    
    
    p->dataPosition = pos;
        
exit:
    return result;
}

PlankResult pl_AudioFileReader_AIFC_ParseVersion (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankUI version;
    
    if ((result = pl_File_ReadUI ((PlankFileRef)p, &version)) != PlankResult_OK) goto exit;
    if (version == 0) goto exit;    
    if (version == PLANKAUDIOFILE_AIFC_VERSION) goto exit;
    
    result = PlankResult_AudioFileReaderInavlidType;
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_AIFC_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankFourCharCode compressionID;
    PlankPascalString255 compressionName;
    
    if ((result = pl_AudioFileReader_AIFF_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;

    if ((result = pl_File_ReadFourCharCode ((PlankFileRef)p, &compressionID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadPascalString255 ((PlankFileRef)p, &compressionName)) != PlankResult_OK) goto exit;
    
    if (compressionID == pl_FourCharCode ("NONE"))
    {
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
    }
    else if (compressionID == pl_FourCharCode ("twos"))
    {
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
    }
    else if (compressionID == pl_FourCharCode ("sowt"))
    {
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN;
    }
    else if (compressionID == pl_FourCharCode ("fl32"))
    {
        if (p->formatInfo.bitsPerSample != 32)
        {
            result = PlankResult_AudioFileReaderInavlidType;
            goto exit;
        }
        
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN;
    }
    else if (compressionID == pl_FourCharCode ("fl64"))
    {
        if (p->formatInfo.bitsPerSample != 64)
        {
            result = PlankResult_AudioFileReaderInavlidType;
            goto exit;
        }
        
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN;
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_AIFC_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    return pl_AudioFileReader_AIFF_ParseData (p, chunkLength, chunkDataPos);
}

PlankResult pl_AudioFileReader_ParseMain  (PlankAudioFileReaderRef p, 
                                           const PlankFourCharCode mainID, 
                                           const PlankFourCharCode formatID)
{        
    PlankB isBigEndian = PLANK_FALSE;
    
    if (mainID == pl_FourCharCode ("RIFF"))
    {
        isBigEndian = PLANK_FALSE;

        if (formatID == pl_FourCharCode ("WAVE"))
        {
            p->formatInfo.format = PLANKAUDIOFILE_FORMAT_WAV;
        }
        else goto exit;
    }
    else if (mainID == pl_FourCharCode ("FORM"))
    {
        isBigEndian = PLANK_TRUE;
        
        if (formatID == pl_FourCharCode ("AIFF"))
        {
            p->formatInfo.format = PLANKAUDIOFILE_FORMAT_AIFF;
        }
        else if (formatID == pl_FourCharCode ("AIFC"))
        {
            p->formatInfo.format = PLANKAUDIOFILE_FORMAT_AIFC;
        }
        else goto exit;
    }
    else goto exit;
    
    pl_IffFileReader_SetEndian (&p->iff, isBigEndian);
    
    return PlankResult_OK;
    
exit:
    return PlankResult_AudioFileReaderInavlidType;
}
