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

// private structures

// private functions and data
PlankResult pl_AudioFileReader_WAV_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_WAV_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_AIFF_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFF_ParseData(PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_AIFC_ParseVersion (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFC_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFC_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_Iff_ParseMain  (PlankAudioFileReaderRef p, const PlankFourCharCode mainID, const PlankFourCharCode formatID);


#if PLANK_OGGVORBIS
typedef struct PlankOggFileReader
{
    PlankFile file;
    OggVorbis_File oggVorbisFile;
    ov_callbacks callbacks;
} PlankOggFileReader;

typedef PlankOggFileReader* PlankOggFileReaderRef;

size_t pl_OggFileReader_ReadCallback (PlankP ptr, size_t size, size_t size2, PlankP ref);
int pl_OggFileReader_SeekCallback (PlankP ref, PlankLL offset, int code);
int pl_OggFileReader_CloseCallback (PlankP ref);
long pl_OggFileReader_TellCallback (PlankP ref);
#endif



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
                
    p->peer                        = PLANK_NULL;
    p->formatInfo.format           = PLANKAUDIOFILE_FORMAT_INVALID;
    p->formatInfo.encoding         = PLANKAUDIOFILE_ENCODING_INVALID;
    p->formatInfo.bitsPerSample    = 0;
    p->formatInfo.bytesPerFrame    = 0;
    p->formatInfo.numChannels      = 0;
    p->formatInfo.sampleRate       = 0.0;
    p->dataLength                  = 0;
    p->numFrames                   = 0;
    p->dataPosition                = -1;
    
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
    
    switch (p->formatInfo.format)
    {
        case PLANKAUDIOFILE_FORMAT_WAV:
        case PLANKAUDIOFILE_FORMAT_AIFF:
        case PLANKAUDIOFILE_FORMAT_AIFC:
        case PLANKAUDIOFILE_FORMAT_UNKNOWNIFF:
            result = pl_IffFileReader_Destroy ((PlankIffFileReader*)p->peer);
            break;
        case PLANKAUDIOFILE_FORMAT_OGGVORBIS:
            result = PlankResult_UnknownError;
            break;
        default:
            if (p->peer != PLANK_NULL)
                result = PlankResult_UnknownError;
    }

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
    return (PlankFileRef)p->peer; // pl_IffFileReader_GetFile (&p->iff); 
}

PlankResult pl_AudioFileReader_Open (PlankAudioFileReaderRef p, const char* filepath)
{
    PlankMemoryRef m;
    PlankResult result;
    PlankUI chunkLength;
    PlankLL chunkDataPos;
    PlankFourCharCode mainID, formatID;
    PlankIffFileReaderRef iff;
#if PLANK_OGGVORBIS
    PlankOggFileReaderRef ogg;
    int err;
#endif
    
    result = PlankResult_OK;
    iff = PLANK_NULL;
    m = pl_MemoryGlobal();
    
    if ((iff = pl_IffFileReader_CreateAndInit()) == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    // so the iff reader gets destroyed it we hit an error further down
    p->peer = iff;
    p->formatInfo.format = PLANKAUDIOFILE_FORMAT_UNKNOWNIFF;
    
    // open the file as an IFF
    if ((result = pl_IffFileReader_Open (iff, filepath)) != PlankResult_OK) goto exit;

    // deterimine the file format, could be IFF or Ogg
    if ((result = pl_IffFileReader_GetMainID (iff, &mainID)) != PlankResult_OK) goto exit;

    if ((mainID == pl_FourCharCode ("RIFF")) || // Riff
        (mainID == pl_FourCharCode ("FORM")))   // Iff
    {
        // deterimine the IFF file format, could be IFF or RIFF
        if ((result = pl_IffFileReader_GetFormatID (iff, &formatID)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_Iff_ParseMain (p, mainID, formatID)) != PlankResult_OK) goto exit;

        // parse based on the format
        if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_WAV)
        {
            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("fmt "), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_WAV_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;

            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("data"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_WAV_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        }
        else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFF)
        {
            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("COMM"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_AIFF_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
            
            p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
            
            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("SSND"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_AIFF_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;        
        }
        else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFC)
        {
            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("FVER"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_AIFC_ParseVersion (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;

            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("COMM"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_AIFC_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
            
            if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("SSND"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileReader_AIFC_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;        
        }
        else
        {
            result = PlankResult_AudioFileReaderInavlidType;
            goto exit;
        }    
        
        if ((result = pl_AudioFileReader_ResetFramePosition (p)) != PlankResult_OK) goto exit;    
    }
#if PLANK_OGGVORBIS
    else if (mainID == pl_FourCharCode ("OggS")) //Ogg
    {
        if ((result = pl_IffFileReader_Destroy (iff)) != PlankResult_OK) goto exit;
        
        p->peer = PLANK_NULL;
        p->formatInfo.format = PLANKAUDIOFILE_FORMAT_INVALID;
        
        // open as ogg
        
        ogg = (PlankOggFileReaderRef)pl_Memory_AllocateBytes (m, sizeof (PlankOggFileReader));
        p->peer = ogg;
        p->formatInfo.format = PLANKAUDIOFILE_FORMAT_OGGVORBIS;
        pl_MemoryZero (ogg, sizeof (PlankOggFileReader));
        
        if ((result = pl_File_Init (&ogg->file)) != PlankResult_OK) goto exit;
        
        // open as binary, not writable, litte endian
        if ((result = pl_File_OpenBinaryRead (&ogg->file, filepath, PLANK_FALSE, PLANK_FALSE)) != PlankResult_OK) goto exit;
        
        ogg->callbacks.read_func  = &pl_OggFileReader_ReadCallback;
        ogg->callbacks.seek_func  = &pl_OggFileReader_SeekCallback;
        ogg->callbacks.close_func = &pl_OggFileReader_CloseCallback;
        ogg->callbacks.tell_func  = &pl_OggFileReader_TellCallback;
        
        err = ov_open_callbacks (p, &ogg->oggVorbisFile, 0, 0, ogg->callbacks);
        
        if (err != 0)
        {
            result = PlankResult_AudioFileReaderInavlidType;
            goto exit;
        }
        
        // okk..
    }
#endif
    else 
    {
        if ((result = pl_IffFileReader_Destroy (iff)) != PlankResult_OK) goto exit;

        p->peer = PLANK_NULL;
        p->formatInfo.format = PLANKAUDIOFILE_FORMAT_INVALID;
        
        result = PlankResult_AudioFileReaderInavlidType;
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Close (PlankAudioFileReaderRef p)
{
    if (p == PLANK_NULL || p->peer == PLANK_NULL)
        return PlankResult_FileCloseFailed;
    
    return pl_File_Close ((PlankFileRef)p->peer); 
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
    
    if (p->peer == PLANK_NULL)
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }

    if ((p->dataPosition < 0) || (p->formatInfo.bytesPerFrame <= 0))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    pos = p->dataPosition + frameIndex * p->formatInfo.bytesPerFrame;
    
    if ((result = pl_File_SetPosition ((PlankFileRef)p->peer, pos)) != PlankResult_OK) goto exit;
    
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
    
    if (p->peer == PLANK_NULL)
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }

    if ((p->dataPosition < 0) || (p->formatInfo.bytesPerFrame <= 0))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    if ((result = pl_File_GetPosition ((PlankFileRef)p->peer, &pos)) != PlankResult_OK) goto exit;

    *frameIndex = (pos - p->dataPosition) / p->formatInfo.bytesPerFrame;
    
exit:
    return result;    
}


PlankResult pl_AudioFileReader_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead)
{
    PlankResult result = PlankResult_OK;
    int startFrame, endFrame, framesToRead, bytesToRead, bytesRead;
    
    if (p->peer == PLANK_NULL)
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }

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
    
    result = pl_File_Read ((PlankFileRef)p->peer, data, bytesToRead, &bytesRead);
    
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

    if ((result = pl_File_ReadUS ((PlankFileRef)p->peer, &compressionCode)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS ((PlankFileRef)p->peer, &numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p->peer, &sampleRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p->peer, &byteRate)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS ((PlankFileRef)p->peer, &blockAlign)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS ((PlankFileRef)p->peer, &bitsPerSample)) != PlankResult_OK) goto exit;
    
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

	(void)chunkDataPos;
	(void)chunkLength;

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
  
    return result;
}

PlankResult pl_AudioFileReader_AIFF_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankS numChannels;
    PlankUI numFrames;
    PlankS bitsPerSample;
    PlankF80 sampleRate;

    if ((result = pl_File_ReadS ((PlankFileRef)p->peer, &numChannels)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p->peer, &numFrames)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadS ((PlankFileRef)p->peer, &bitsPerSample)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read ((PlankFileRef)p->peer, sampleRate.data, sizeof (sampleRate), PLANK_NULL)) != PlankResult_OK) goto exit;    
    
    p->formatInfo.bitsPerSample = (PlankI) bitsPerSample;
    p->formatInfo.bytesPerFrame = (PlankI) (((bitsPerSample + (0x00000008 - 1)) & ~(0x00000008 - 1)) * numChannels / 8); // round up to whole bytes
    p->formatInfo.numChannels   = (PlankI) numChannels;
    p->formatInfo.sampleRate    = (PlankD) pl_F802I (sampleRate);
    p->numFrames                = (PlankLL) numFrames;

	(void)chunkDataPos;
	(void)chunkLength;

exit:
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_AIFF_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankUI offset, blockSize;
    PlankLL pos;
    
    if ((result = pl_File_ReadUI ((PlankFileRef)p->peer, &offset)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI ((PlankFileRef)p->peer, &blockSize)) != PlankResult_OK) goto exit;
    if ((result = pl_File_GetPosition ((PlankFileRef)p->peer, &pos)) != PlankResult_OK) goto exit;    
    
    p->dataPosition = pos;
        
	(void)chunkDataPos;
	(void)chunkLength;

exit:
    return result;
}

PlankResult pl_AudioFileReader_AIFC_ParseVersion (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankUI version;
    
    if ((result = pl_File_ReadUI ((PlankFileRef)p->peer, &version)) != PlankResult_OK) goto exit;
    if (version == 0) goto exit;    
    if (version == PLANKAUDIOFILE_AIFC_VERSION) goto exit;
    
    result = PlankResult_AudioFileReaderInavlidType;
    
	(void)chunkDataPos;
	(void)chunkLength;

exit:
    return result;
}

PlankResult pl_AudioFileReader_AIFC_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankFourCharCode compressionID;
    PlankPascalString255 compressionName;
    
    if ((result = pl_AudioFileReader_AIFF_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;

    if ((result = pl_File_ReadFourCharCode ((PlankFileRef)p->peer, &compressionID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadPascalString255 ((PlankFileRef)p->peer, &compressionName)) != PlankResult_OK) goto exit;
    
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

PlankResult pl_AudioFileReader_Iff_ParseMain  (PlankAudioFileReaderRef p, 
                                               const PlankFourCharCode mainID, 
                                               const PlankFourCharCode formatID)
{        
    PlankIffFileReader* iff = PLANK_NULL;
    PlankB isBigEndian = PLANK_FALSE;
    
    if (PLANK_TRUE) // for branching later when ogg is done
    {
        iff = (PlankIffFileReader*)p->peer;

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
        
        pl_IffFileReader_SetEndian (iff, isBigEndian);
    }
    
    return PlankResult_OK;
    
exit:
    return PlankResult_AudioFileReaderInavlidType;
}

size_t pl_OggFileReader_ReadCallback (PlankP ptr, size_t size, size_t size2, PlankP ref)
{
    (void)ptr; // ptr 
    (void)size; // size
    (void)size2; // nmemb
    (void)ref; // datasource // is the audio file reader

    
    
    //return (size_t) (static_cast <InputStream*> (datasource)->read (ptr, (int) (size * nmemb)) / size);

    return 0;
}

int pl_OggFileReader_SeekCallback (PlankP ref, PlankLL offset, int code)
{
    (void)ref; // datasource // is the audio file reader
    (void)offset; // offset
    (void)code; // whence
    return 0;
}

int pl_OggFileReader_CloseCallback (PlankP ref)
{
    (void)ref; // datasource // is the audio file reader
    return 0;
}

long pl_OggFileReader_TellCallback (PlankP ref)
{
    (void)ref; // datasource // is the audio file reader
    return 0;
}
