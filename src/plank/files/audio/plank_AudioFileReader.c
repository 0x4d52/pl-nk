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
#include "../../maths/plank_Maths.h"
#include "../plank_File.h"
#include "../plank_IffFileReader.h"
#include "plank_AudioFileReader.h"
#include "plank_AudioFileMetaData.h"
#include "plank_AudioFileCuePoint.h"
#include "plank_AudioFileRegion.h"


// private structures

// private functions and data
typedef PlankResult (*PlankAudioFileReaderReadFramesFunction)(PlankAudioFileReaderRef, const int, void*, int *);
typedef PlankResult (*PlankAudioFileReaderSetFramePositionFunction)(PlankAudioFileReaderRef, const PlankLL);
typedef PlankResult (*PlankAudioFileReaderGetFramePositionFunction)(PlankAudioFileReaderRef, PlankLL *);

#if PLANK_APPLE
#pragma mark Private Function Declarations
#endif

PlankResult pl_AudioFileReader_WAV_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_WAV_ParseMetaData (PlankAudioFileReaderRef p);
PlankResult pl_AudioFileReader_WAV_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_AIFF_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFF_ParseMetaData (PlankAudioFileReaderRef p);
PlankResult pl_AudioFileReader_AIFF_ParseData(PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_AIFC_ParseVersion (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFC_ParseFormat (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);
PlankResult pl_AudioFileReader_AIFF_ParseMetaData (PlankAudioFileReaderRef p);
PlankResult pl_AudioFileReader_AIFC_ParseData (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkDataPos);

PlankResult pl_AudioFileReader_Iff_Open (PlankAudioFileReaderRef p, const char* filepath);
PlankResult pl_AudioFileReader_Iff_ParseMain  (PlankAudioFileReaderRef p, const PlankFourCharCode mainID, const PlankFourCharCode formatID);
PlankResult pl_AudioFileReader_Iff_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead);
PlankResult pl_AudioFileReader_Iff_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex);
PlankResult pl_AudioFileReader_Iff_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex);

PlankResult pl_AudioFileReader_OggVorbis_Open  (PlankAudioFileReaderRef p, const char* filepath);
PlankResult pl_AudioFileReader_OggVorbis_Close (PlankAudioFileReaderRef p);
PlankResult pl_AudioFileReader_OggVorbis_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead);
PlankResult pl_AudioFileReader_OggVorbis_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex);
PlankResult pl_AudioFileReader_OggVorbis_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex);

PlankResult pl_AudioFileReader_Opus_Open  (PlankAudioFileReaderRef p, const char* filepath);
PlankResult pl_AudioFileReader_Opus_Close (PlankAudioFileReaderRef p);
PlankResult pl_AudioFileReader_Opus_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead);
PlankResult pl_AudioFileReader_Opus_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex);
PlankResult pl_AudioFileReader_Opus_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex);


#if PLANK_APPLE
#pragma mark Generic Functions
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
    
    p->metaData                    = PLANK_NULL;
    
    p->readFramesFunction          = PLANK_NULL;
    p->setFramePositionFunction    = PLANK_NULL;
    p->getFramePositionFunction    = PLANK_NULL;
    
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
#if PLANK_OGGVORBIS
        case PLANKAUDIOFILE_FORMAT_OGGVORBIS:
            result = pl_AudioFileReader_OggVorbis_Close (p);
            break;
#endif
        default:
            if (p->peer != PLANK_NULL)
                result = PlankResult_UnknownError;
    }
    
    if (p->metaData != PLANK_NULL)
    {
        if ((result = pl_AudioFileMetaData_Destroy (p->metaData)) != PlankResult_OK) goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankAudioFileReader));

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
    return (PlankFileRef)p->peer;
}

PlankResult pl_AudioFileReader_Open (PlankAudioFileReaderRef p, const char* filepath)
{
    return pl_AudioFileReader_OpenInternal (p, filepath, PLANK_FALSE);
}

PlankResult pl_AudioFileReader_OpenWithMetaData (PlankAudioFileReaderRef p, const char* filepath)
{
    return pl_AudioFileReader_OpenInternal (p, filepath, PLANK_TRUE);
}

PlankResult pl_AudioFileReader_OpenInternal (PlankAudioFileReaderRef p, const char* filepath, const PlankB readMetaData)
{
    PlankResult result;
    PlankFourCharCode mainID;
    PlankIffFileReaderRef iff;
        
    result = PlankResult_OK;
    iff = PLANK_NULL;
    
    if (readMetaData)
        p->metaData = pl_AudioFileMetaData_CreateAndInit();
    
    if ((iff = pl_IffFileReader_CreateAndInit()) == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    // so the iff reader gets destroyed it we hit an error further down but before we're finished
    p->peer = iff;
    p->formatInfo.format = PLANKAUDIOFILE_FORMAT_UNKNOWNIFF;
    
    // open the file as an IFF
    if ((result = pl_IffFileReader_Open (iff, filepath)) != PlankResult_OK) goto exit;

    // deterimine the file format, could be IFF or Ogg
    if ((result = pl_IffFileReader_GetMainID (iff, &mainID)) != PlankResult_OK) goto exit;

    if ((mainID == pl_FourCharCode ("RIFF")) || // Riff
        (mainID == pl_FourCharCode ("FORM")))   // Iff
    {
        if ((result = pl_AudioFileReader_Iff_Open (p, filepath)) != PlankResult_OK) goto exit;
    }
#if PLANK_OGGVORBIS || PLANK_OPUS
    else if (mainID == pl_FourCharCode ("OggS")) //Ogg this needs to handle any Ogg e.g., Vorbis or Opus
    {
        // close the Iff file and start again
        if ((result = pl_IffFileReader_Destroy (iff)) != PlankResult_OK) goto exit;
        
        p->peer = PLANK_NULL;
        p->formatInfo.format = PLANKAUDIOFILE_FORMAT_INVALID;
                
#if PLANK_OGGVORBIS    
        if (p->peer == PLANK_NULL)
        {
            result = pl_AudioFileReader_OggVorbis_Open (p, filepath);
            
            if (result != PlankResult_OK)
            {
                pl_AudioFileReader_OggVorbis_Close (p);
            
                p->peer = PLANK_NULL;
                p->formatInfo.format = PLANKAUDIOFILE_FORMAT_INVALID;
            }
        }
#endif
#if PLANK_OPUS
        if (p->peer == PLANK_NULL)
        {
            result = pl_AudioFileReader_Opus_Open (p, filepath);
            
            if (result != PlankResult_OK)
            {
                pl_AudioFileReader_Opus_Close (p);
                
                p->peer = PLANK_NULL;
                p->formatInfo.format = PLANKAUDIOFILE_FORMAT_INVALID;
            }
        }
#endif
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

PlankResult pl_AudioFileReader_GetNumFrames (PlankAudioFileReaderRef p, PlankLL *numFrames)
{
    *numFrames = p->numFrames;
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex)
{
    PlankResult result = PlankResult_OK;
    
    if (p->peer == PLANK_NULL)
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
        
    if (!p->setFramePositionFunction)
    {
        result = PlankResult_FunctionsInvalid;
        goto exit;
    }
    
    result = ((PlankAudioFileReaderSetFramePositionFunction)p->setFramePositionFunction)(p, frameIndex);
    
exit:
    return result;    
}

PlankResult pl_AudioFileReader_ResetFramePosition (PlankAudioFileReaderRef p)
{
    return pl_AudioFileReader_SetFramePosition (p, 0);
}

PlankResult pl_AudioFileReader_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex)
{
    PlankResult result = PlankResult_OK;
    
    if (p->peer == PLANK_NULL)
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    if (!p->getFramePositionFunction)
    {
        result = PlankResult_FunctionsInvalid;
        goto exit;
    }
    
    result = ((PlankAudioFileReaderGetFramePositionFunction)p->getFramePositionFunction)(p, frameIndex);
    
exit:
    return result;    
}

PlankResult pl_AudioFileReader_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead)
{
    if (!p->readFramesFunction)
        return PlankResult_FunctionsInvalid;
    
    return ((PlankAudioFileReaderReadFramesFunction)p->readFramesFunction)(p, numFrames, data, framesRead);
}

// -- WAV Functions -- /////////////////////////////////////////////////////////
#if PLANK_APPLE
#pragma mark WAV Functions
#endif

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

static PlankResult pl_AudioFileReader_WAV_ParseChunk_bext (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkEnd)
{
    PlankResult result = PlankResult_OK;
    char description [257];
    char originator [33];
    char originatorRef [33];
    char originationDate [11];
    char originationTime [9];
    PlankUI timeRefLow;
    PlankUI timeRefHigh;
    PlankUS version;
    PlankUC umid[64];
    PlankUC reserved[190];
    PlankUI fixedSize, lengthRemain;
    PlankIffFileReaderRef iff;

    iff = (PlankIffFileReaderRef)p->peer;

    fixedSize = 256 + 32 + 32 + 10 + 8 + 4 + 4 + 4 + 64 + 190;
    description[256] = '\0';
    originator[32] = '\0';
    originatorRef[32] = '\0';
    originationDate[10] = '\0';
    originationTime[8] = '\0';

    if ((result = pl_File_Read (&iff->file, description, 256, PLANK_NULL)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read (&iff->file, originator, 32, PLANK_NULL)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read (&iff->file, originatorRef, 32, PLANK_NULL)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read (&iff->file, originationDate, 10, PLANK_NULL)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read (&iff->file, originationTime, 8, PLANK_NULL)) != PlankResult_OK) goto exit;

    if ((result = pl_File_ReadUI (&iff->file, &timeRefLow)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI (&iff->file, &timeRefHigh)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUS (&iff->file, &version)) != PlankResult_OK) goto exit;

    if ((result = pl_File_Read (&iff->file, umid, 64, PLANK_NULL)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Read (&iff->file, reserved, 190, PLANK_NULL)) != PlankResult_OK) goto exit;
    
    // copy these into the metadata

    
    if ((result = pl_AudioFileMetaData_ClearDescriptionComments (p->metaData)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_AddDescriptionComment (p->metaData, description)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_SetOriginatorArtist (p->metaData, originator)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_SetOriginatorRef (p->metaData, originatorRef)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_SetOriginationDate (p->metaData, originationDate)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_SetOriginationTime (p->metaData, originationTime)) != PlankResult_OK) goto exit;
    
    lengthRemain = chunkLength - fixedSize;
    
    // then add the coding history
    
    
exit:
    return result;
}

static PlankResult pl_AudioFileReader_WAV_ParseChunk_smpl (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkEnd)
{
    (void)p;
    (void)chunkEnd;
    printf("smpl  - %d bytes\n", chunkLength);
    return PlankResult_OK;
}


static PlankResult pl_AudioFileReader_WAV_ParseChunk_inst (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkEnd)
{
    PlankResult result = PlankResult_OK;
    char baseNote, detune, gain, lowNote, highNote, lowVelocity, highVelocity;
    PlankIffFileReaderRef iff;
    
    iff = (PlankIffFileReaderRef)p->peer;

    if ((result = pl_File_ReadC (&iff->file, &baseNote)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadC (&iff->file, &detune)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadC (&iff->file, &gain)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadC (&iff->file, &lowNote)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadC (&iff->file, &highNote)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadC (&iff->file, &lowVelocity)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadC (&iff->file, &highVelocity)) != PlankResult_OK) goto exit;

    // copy to metadata
    
exit:
    return result;
}

static PlankResult pl_AudioFileReader_WAV_ParseChunk_cue (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkEnd)
{
    PlankResult result = PlankResult_OK;
    PlankUI numCuePoints, i;
    PlankIffFileReaderRef iff;
    PlankUI cueID, order, chunkID, chunkStart, blockStart, offset;
    PlankAudioFileCuePoint cuePoint;
    
    iff = (PlankIffFileReaderRef)p->peer;
    
    if ((result = pl_File_ReadUI (&iff->file, &numCuePoints)) != PlankResult_OK) goto exit;
    
    for (i = 0; i < numCuePoints; ++i)
    {
        if ((result = pl_File_ReadUI (&iff->file, &cueID)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUI (&iff->file, &order)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUI (&iff->file, &chunkID)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUI (&iff->file, &chunkStart)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUI (&iff->file, &blockStart)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUI (&iff->file, &offset)) != PlankResult_OK) goto exit;
        
        // assume there is no playlist, wavl or slnt chunks
        if ((chunkID == pl_FourCharCode ("data")) &&
            (chunkStart == 0) &&
            (blockStart == 0))
        {
            if ((result = pl_AudioFileCuePoint_Init (&cuePoint)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileCuePoint_SetID (&cuePoint, cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileCuePoint_SetPosition (&cuePoint, offset)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileCuePoint_SetType (&cuePoint, PLANKAUDIOFILE_CUEPOINTTYPE_CUEPOINT)) != PlankResult_OK) goto exit;

            if ((result = pl_AudioFileMetaData_AddCuePoint (p->metaData, &cuePoint)) != PlankResult_OK) goto exit;
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileReader_WAV_ParseChunk_LIST (PlankAudioFileReaderRef p, const PlankUI chunkLength, const PlankLL chunkEnd)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileReaderRef iff;
    PlankFourCharCode typeID, adtlChunkID;
    PlankUI adtlChunkLength, cueID, textLength, sampleLength, purpose;
    PlankLL pos, adtlChunkEnd;
    PlankUS country, language, dialect, codePage;
    PlankAudioFileCuePointRef cuePointRef;
    PlankAudioFileRegion region;
    PlankB success;

    
    iff = (PlankIffFileReaderRef)p->peer;
    
    if ((result = pl_File_ReadFourCharCode (&iff->file, &typeID)) != PlankResult_OK) goto exit;

    if (typeID != pl_FourCharCode ("adtl"))
    {
        result = PlankResult_AudioFileReaderInavlidType;
        goto exit;
    }
    
    if ((result = pl_File_GetPosition (&iff->file, &pos)) != PlankResult_OK) goto exit;
    
    while ((pos < chunkEnd) && (pl_File_IsEOF (&iff->file) == PLANK_FALSE))
    {
        if ((result = pl_IffFileReader_ParseChunkHeader (iff, &adtlChunkID, &adtlChunkLength, &adtlChunkEnd, &pos)) != PlankResult_OK) goto exit;
        
        if (adtlChunkID == pl_FourCharCode ("labl"))
        {
            // label or title
            
            if ((result = pl_File_ReadUI (&iff->file, &cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileMetaData_FindCuePointWithID (p->metaData, cueID, &cuePointRef, PLANK_NULL)) != PlankResult_OK) goto exit;

            if (cuePointRef != PLANK_NULL)
            {
                textLength = adtlChunkLength - 4;
                
                if ((result = pl_AudioFileCuePoint_SetLabelLengthClear (cuePointRef, textLength)) != PlankResult_OK) goto exit;
                if ((result = pl_File_Read (&iff->file, pl_AudioFileCuePoint_GetLabelRaw (cuePointRef), textLength, PLANK_NULL)) != PlankResult_OK) goto exit;
            }
        }
        if (adtlChunkID == pl_FourCharCode ("note"))
        {
            // comment
            
            if ((result = pl_File_ReadUI (&iff->file, &cueID)) != PlankResult_OK) goto exit;
            if ((result = pl_AudioFileMetaData_FindCuePointWithID (p->metaData, cueID, &cuePointRef, PLANK_NULL)) != PlankResult_OK) goto exit;
            
            if (cuePointRef != PLANK_NULL)
            {
                textLength = adtlChunkLength - 4;
                
                if ((result = pl_AudioFileCuePoint_SetCommentLengthClear (cuePointRef, textLength)) != PlankResult_OK) goto exit;
                if ((result = pl_File_Read (&iff->file, pl_AudioFileCuePoint_GetCommentRaw (cuePointRef), textLength, PLANK_NULL)) != PlankResult_OK) goto exit;
            }
        }
        else if (adtlChunkID == pl_FourCharCode ("ltxt"))
        {
            // labelled text chunk, add this as a region, removing the associated cue point from the cue array
            
            if ((result = pl_File_ReadUI (&iff->file, &cueID)) != PlankResult_OK) goto exit;
            
            if ((result = pl_AudioFileRegion_Init (&region)) != PlankResult_OK) goto exit;
            cuePointRef = pl_AudioFileRegion_GetStartCuePoint (&region);
            
            if ((result = pl_AudioFileMetaData_RemoveCuePointWithID (p->metaData, cueID, cuePointRef, &success)) != PlankResult_OK) goto exit;

            if (success)
            {
                if ((result = pl_File_ReadUI (&iff->file, &sampleLength)) != PlankResult_OK) goto exit;
                if ((result = pl_File_ReadUI (&iff->file, &purpose)) != PlankResult_OK) goto exit;
                if ((result = pl_File_ReadUS (&iff->file, &country)) != PlankResult_OK) goto exit;
                if ((result = pl_File_ReadUS (&iff->file, &language)) != PlankResult_OK) goto exit;
                if ((result = pl_File_ReadUS (&iff->file, &dialect)) != PlankResult_OK) goto exit;
                if ((result = pl_File_ReadUS (&iff->file, &codePage)) != PlankResult_OK) goto exit;
                
                textLength = adtlChunkLength - 20;

                if ((result = pl_AudioFileCuePoint_SetExtra (cuePointRef, purpose, country, language, dialect, codePage)) != PlankResult_OK) goto exit;
                if ((result = pl_AudioFileCuePoint_SetLabelLengthClear (cuePointRef, textLength)) != PlankResult_OK) goto exit;
                if ((result = pl_File_Read (&iff->file, pl_AudioFileCuePoint_GetLabelRaw (cuePointRef), textLength, PLANK_NULL)) != PlankResult_OK) goto exit;

                if ((result = pl_AudioFileRegion_SetLength (&region, sampleLength)) != PlankResult_OK) goto exit;
                if ((result = pl_AudioFileRegion_SetType (&region, PLANKAUDIOFILE_REGIONTYPE_REGION)) != PlankResult_OK) goto exit;

                if ((result = pl_AudioFileMetaData_AddRegion (p->metaData, &region)) != PlankResult_OK) goto exit;
            }
         }
        else
        {
            // notes:
            // - amadeus uses 'mcol' for marker colours
            // - 'file' chunk? for media file e.g. album art?
            printf("%s - chunk in LIST\n", pl_FourCharCode2String (adtlChunkID).string);
        }
        
    next:
        if ((result = pl_File_SetPosition (&iff->file, adtlChunkEnd)) != PlankResult_OK) goto exit;
        pos = adtlChunkEnd;
    }

    
    printf("LIST - %d bytes\n", chunkLength);
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_WAV_ParseMetaData (PlankAudioFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    PlankLL readChunkEnd, pos = PLANKIFFFILE_FIRSTCHUNKPOSITION;
    PlankFourCharCode readChunkID;
    PlankUI readChunkLength;
    PlankIffFileReaderRef iff;
    PlankDynamicArrayRef block;
    PlankC* data;
    int bytesRead;

    iff = (PlankIffFileReaderRef)p->peer;
    
    if ((result = pl_File_SetPosition (&iff->file, pos)) != PlankResult_OK) goto exit;
    
    while ((pos < iff->headerInfo.mainEnd) && (pl_File_IsEOF (&iff->file) == PLANK_FALSE))
    {
        if ((result = pl_IffFileReader_ParseChunkHeader (iff, &readChunkID, &readChunkLength, &readChunkEnd, &pos)) != PlankResult_OK) goto exit;
        
        if ((readChunkID == pl_FourCharCode ("fmt ")) ||
            (readChunkID == pl_FourCharCode ("data")))
        {
            goto next;
        }
        else if (readChunkID == pl_FourCharCode ("bext"))
        {
            if ((result = pl_AudioFileReader_WAV_ParseChunk_bext (p, readChunkLength, readChunkEnd)) != PlankResult_OK) goto exit;
        }
        else if (readChunkID == pl_FourCharCode ("smpl"))
        {
            if ((result = pl_AudioFileReader_WAV_ParseChunk_smpl (p, readChunkLength, readChunkEnd)) != PlankResult_OK) goto exit;
        }
        else if ((readChunkID == pl_FourCharCode ("inst")) ||
                 (readChunkID == pl_FourCharCode ("INST")))
        {
            if ((result = pl_AudioFileReader_WAV_ParseChunk_inst (p, readChunkLength, readChunkEnd)) != PlankResult_OK) goto exit;
        }
        else if (readChunkID == pl_FourCharCode ("cue "))
        {
            if ((result = pl_AudioFileReader_WAV_ParseChunk_cue (p, readChunkLength, readChunkEnd)) != PlankResult_OK) goto exit;
        }
        else if ((readChunkID == pl_FourCharCode ("list")) ||
                 (readChunkID == pl_FourCharCode ("LIST")) )
        {
            if ((result = pl_AudioFileReader_WAV_ParseChunk_LIST (p, readChunkLength, readChunkEnd)) != PlankResult_OK) goto exit;
        }
        else
        {
            printf("%s - %d bytes\n", pl_FourCharCode2String (readChunkID).string, readChunkLength);

            block = pl_DynamicArray_Create();
            
            if (block != PLANK_NULL)
            {
                if ((result = pl_DynamicArray_InitWithItemSizeAndSize (block, 1, readChunkLength + 8, PLANK_FALSE)) != PlankResult_OK) goto exit;
                
                data = (PlankC*)pl_DynamicArray_GetArray (block);
                
                *(PlankFourCharCode*)data = readChunkID;
                data += 4;
                *(PlankUI*)data = readChunkLength;
                data += 4;
                
                if ((result = pl_File_Read (&iff->file, data, readChunkLength, &bytesRead)) != PlankResult_OK) goto exit;
                if ((result = pl_AudioFileMetaData_AddFormatSpecificBlock (p->metaData, block)) != PlankResult_OK) goto exit;
            }
        }
        
    next:
        if ((result = pl_File_SetPosition (&iff->file, readChunkEnd)) != PlankResult_OK) goto exit;
        pos = readChunkEnd;
    }

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

// -- AIFF Functions -- ////////////////////////////////////////////////////////
#if PLANK_APPLE
#pragma mark AIFF Functions
#endif

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

PlankResult pl_AudioFileReader_AIFF_ParseMetaData (PlankAudioFileReaderRef p)
{
    (void)p;
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

// -- AIFC Functions -- ////////////////////////////////////////////////////////
#if PLANK_APPLE
#pragma mark AIFC Functions
#endif

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

PlankResult pl_AudioFileReader_AIFC_ParseMetaData (PlankAudioFileReaderRef p)
{
    (void)p;
    return PlankResult_OK;
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

// -- Generic Iff Functions -- /////////////////////////////////////////////////
#if PLANK_APPLE
#pragma mark Generic Iff Functions
#endif

PlankResult pl_AudioFileReader_Iff_Open (PlankAudioFileReaderRef p, const char* filepath)
{
    PlankResult result;
    PlankUI chunkLength;
    PlankLL chunkDataPos;
    PlankIffFileReaderRef iff;
    PlankFourCharCode mainID, formatID;
    (void)filepath;
    
    iff = (PlankIffFileReaderRef)p->peer;
    
    // deterimine the IFF file format, could be IFF or RIFF
    if ((result = pl_IffFileReader_GetMainID (iff, &mainID)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileReader_GetFormatID (iff, &formatID)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileReader_Iff_ParseMain (p, mainID, formatID)) != PlankResult_OK) goto exit;
    
    // parse based on the format
    if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_WAV)
    {
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("fmt "), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_WAV_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        
        if (p->metaData)
        {
            result = pl_AudioFileReader_WAV_ParseMetaData (p);
            
            if ((result != PlankResult_OK) && result != PlankResult_FileEOF)
                goto exit;
        }
        
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("data"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_WAV_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFF)
    {
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("COMM"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFF_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        
        p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN;
        
        if (p->metaData)
        {
            if ((result = pl_AudioFileReader_AIFF_ParseMetaData (p)) != PlankResult_OK) goto exit;
        }
        
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("SSND"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFF_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;        
    }
    else if (p->formatInfo.format == PLANKAUDIOFILE_FORMAT_AIFC)
    {
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("FVER"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFC_ParseVersion (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("COMM"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFC_ParseFormat (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;
        
        if (p->metaData)
        {
            if ((result = pl_AudioFileReader_AIFC_ParseMetaData (p)) != PlankResult_OK) goto exit;
        }
        
        if ((result = pl_IffFileReader_SeekChunk (iff, pl_FourCharCode ("SSND"), &chunkLength, &chunkDataPos)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_AIFC_ParseData (p, chunkLength, chunkDataPos)) != PlankResult_OK) goto exit;        
    }
    else
    {
        result = PlankResult_AudioFileReaderInavlidType;
        goto exit;
    }    
    
    p->readFramesFunction       = pl_AudioFileReader_Iff_ReadFrames;
    p->setFramePositionFunction = pl_AudioFileReader_Iff_SetFramePosition;
    p->getFramePositionFunction = pl_AudioFileReader_Iff_GetFramePosition;

    if ((result = pl_AudioFileReader_ResetFramePosition (p)) != PlankResult_OK) goto exit;     
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Iff_ParseMain  (PlankAudioFileReaderRef p, 
                                               const PlankFourCharCode mainID, 
                                               const PlankFourCharCode formatID)
{        
    PlankIffFileReader* iff = PLANK_NULL;
    PlankB isBigEndian = PLANK_FALSE;
    
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
    
    return PlankResult_OK;
    
exit:
    return PlankResult_AudioFileReaderInavlidType;
}

PlankResult pl_AudioFileReader_Iff_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesRead)
{
    PlankResult result = PlankResult_OK;
    PlankLL startFrame, endFrame;
    int framesToRead, bytesToRead, bytesRead;
    
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
    
    framesToRead = (endFrame > p->numFrames) ? (int)(p->numFrames - startFrame) : (numFrames);
    bytesToRead = framesToRead * p->formatInfo.bytesPerFrame;
    
    if (bytesToRead > 0)
    {
        result = pl_File_Read ((PlankFileRef)p->peer, data, bytesToRead, &bytesRead);
    }
    else
    {
        bytesRead = 0;
        result = PlankResult_FileEOF;
    }
    
    if (framesRead != PLANK_NULL)
        *framesRead = bytesRead / p->formatInfo.bytesPerFrame;
    
    // should zero if framesToRead < numFrames
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Iff_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex)
{
    PlankResult result;
    PlankLL pos;
    
    if ((p->dataPosition < 0) || (p->formatInfo.bytesPerFrame <= 0))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    pos = p->dataPosition + frameIndex * p->formatInfo.bytesPerFrame;
    result = pl_File_SetPosition ((PlankFileRef)p->peer, pos);
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Iff_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex)
{
    PlankResult result = PlankResult_OK;
    PlankLL pos;
    
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

// -- Useful Ogg Functions -- //////////////////////////////////////////////////

#if PLANK_OGGVORBIS || PLANK_OPUS
#if PLANK_APPLE
#pragma mark Useful Ogg Functions
#endif

static PlankResult pl_OggFile_FindNextPageOffset (PlankFileRef p, const PlankLL total, PlankLL left, PlankLL right, PlankLL* offset)
{
    PlankResult result;
    PlankLL original;
    char sync[4]; // could initialise here if not for C89!
    int syncIndex;
    char byte;
        
    sync[0] = 'O';
    sync[1] = 'g';
    sync[2] = 'g';
    sync[3] = 'S';
    syncIndex = 0;
    
    result = PlankResult_OK;
    
    if ((result = pl_File_GetPosition (p, &original)) != PlankResult_OK) goto exit;
    
    left = pl_ClipLL (left, 0, total);
    right = pl_ClipLL (right, 0, total);
    
    if ((result = pl_File_SetPosition (p, left)) != PlankResult_OK) goto exit;
    
    // now find 'OggS'
    while ((syncIndex < 4) && (left < right))
    {
        if ((result = pl_File_ReadC (p, &byte)) != PlankResult_OK)
            goto exit; // will hit EOF
        
        syncIndex = (byte == sync[syncIndex]) ? syncIndex + 1 : 0;
        left++;
    }
    
    *offset = (syncIndex == 4) ? left - 4 : -1; 

    if ((result = pl_File_SetPosition (p, original)) != PlankResult_OK) goto exit;

exit:
    return result;
}

static PlankResult pl_OggFile_FindPrevPageOffset (PlankFileRef p, const PlankLL total, PlankLL left, PlankLL right, PlankLL* offset)
{
    PlankResult result;
    PlankLL original;
    char sync[4]; // could initialise here if not for C89!
    int syncIndex;
    char byte;
    
    sync[0] = 'S';
    sync[1] = 'g';
    sync[2] = 'g';
    sync[3] = 'O';
    syncIndex = 0;
    
    result = PlankResult_OK;
    
    if ((result = pl_File_GetPosition (p, &original)) != PlankResult_OK) goto exit;
    
    left = pl_ClipLL (left, 0, total);
    right = pl_ClipLL (right, 0, total);
    
    
    // now find 'OggS'
    while ((syncIndex < 4) && (left < right))
    {
        if ((result = pl_File_SetPosition (p, right)) != PlankResult_OK) goto exit;

        result = pl_File_ReadC (p, &byte);
        
        if ((result != PlankResult_OK) && (result != PlankResult_FileEOF))
            goto exit;
        
        syncIndex = (byte == sync[syncIndex]) ? syncIndex + 1 : 0;
        right--;
        
    }
    
    *offset = (syncIndex == 4) ? right + 1 : -1; 
    
    if ((result = pl_File_SetPosition (p, original)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}


#endif

// -- Ogg Vorbis Functions -- //////////////////////////////////////////////////

#if PLANK_OGGVORBIS
#if PLANK_APPLE
#pragma mark Ogg Vorbis Functions
#endif

#include "../../containers/plank_DynamicArray.h"
typedef struct PlankOggVorbisFileReader
{
    PlankFile file;
    OggVorbis_File oggVorbisFile;
    ov_callbacks callbacks;
    PlankDynamicArray buffer;
    int bufferPosition;
    int bufferFrames;
    PlankLL totalFramesRead;
    int bitStream;
} PlankOggVorbisFileReader;

typedef PlankOggVorbisFileReader* PlankOggVorbisFileReaderRef;

size_t pl_OggVorbisFileReader_ReadCallback (PlankP ptr, size_t size, size_t size2, PlankP ref);
int pl_OggVorbisFileReader_SeekCallback (PlankP ref, PlankLL offset, int code);
int pl_OggVorbisFileReader_CloseCallback (PlankP ref);
long pl_OggVorbisFileReader_TellCallback (PlankP ref);


PlankResult pl_AudioFileReader_OggVorbis_Open  (PlankAudioFileReaderRef p, const char* filepath)
{
    PlankResult result;
    PlankOggVorbisFileReaderRef ogg;
    PlankMemoryRef m;
    PlankLL numFrames;
    PlankL bufferSize;
    PlankI bytesPerSample;
    
    int err;
    vorbis_info* info;
    
    m = pl_MemoryGlobal();
    
    // open as ogg
    ogg = (PlankOggVorbisFileReaderRef)pl_Memory_AllocateBytes (m, sizeof (PlankOggVorbisFileReader));
    
    if (ogg == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->peer = ogg;
    bytesPerSample = sizeof (float);
    p->formatInfo.format = PLANKAUDIOFILE_FORMAT_OGGVORBIS;
    p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    p->formatInfo.bitsPerSample = PLANKAUDIOFILE_CHARBITS * bytesPerSample;
    
    pl_MemoryZero (ogg, sizeof (PlankOggVorbisFileReader));
    
    ogg->bufferPosition  = 0;
    ogg->bufferFrames    = 0;
    ogg->totalFramesRead = 0;
    ogg->bitStream       = -1;

    if ((result = pl_File_Init (&ogg->file)) != PlankResult_OK) goto exit;
    
    // open as binary, not writable, litte endian
    if ((result = pl_File_OpenBinaryRead (&ogg->file, filepath, PLANK_FALSE, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    ogg->callbacks.read_func  = &pl_OggVorbisFileReader_ReadCallback;
    ogg->callbacks.seek_func  = &pl_OggVorbisFileReader_SeekCallback;
    ogg->callbacks.close_func = &pl_OggVorbisFileReader_CloseCallback;
    ogg->callbacks.tell_func  = &pl_OggVorbisFileReader_TellCallback;
    
    err = ov_open_callbacks (p, &ogg->oggVorbisFile, 0, 0, ogg->callbacks); // docs suggest this should be on the other thread if threaded...
    
    if (err != 0)
    {
        result = PlankResult_AudioFileReaderInavlidType;
        goto exit;
    }
    
    info = ov_info (&ogg->oggVorbisFile, -1);
    
    p->formatInfo.numChannels   = info->channels;
    p->formatInfo.sampleRate    = info->rate;
    p->formatInfo.bytesPerFrame = info->channels * bytesPerSample;
    
    numFrames = ov_pcm_total (&ogg->oggVorbisFile, -1);
    
    bufferSize = pl_MinL (numFrames * p->formatInfo.bytesPerFrame, (PlankL)4096);
    
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&ogg->buffer, 1, bufferSize, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    if (numFrames < 0) // could allow this for continuous streams?
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
        
    p->numFrames = numFrames;
    p->readFramesFunction       = pl_AudioFileReader_OggVorbis_ReadFrames;
    p->setFramePositionFunction = pl_AudioFileReader_OggVorbis_SetFramePosition;
    p->getFramePositionFunction = pl_AudioFileReader_OggVorbis_GetFramePosition;
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_OggVorbis_Close (PlankAudioFileReaderRef p)
{
    PlankOggVorbisFileReaderRef ogg;
    PlankResult result = PlankResult_OK;
    int err;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    
    err = ov_clear (&ogg->oggVorbisFile); // closes our PlankFile in the close callback
    
    if (err != 0)
    {
        result = PlankResult_FileCloseFailed;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_DeInit (&ogg->buffer)) != PlankResult_OK) goto exit;

    pl_Memory_Free (m, ogg);

exit:
    return result;
}

PlankResult pl_AudioFileReader_OggVorbis_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesReadOut)
{    
    PlankResult result;
    PlankOggVorbisFileReaderRef ogg;
    int numFramesRemaining, bufferFramesRemaining, bufferFramePosition;
    int bufferSizeInBytes, bytesPerFrame, bufferFrameEnd, bitStream;
    int framesThisTime, numChannels, framesRead, streamFrameEnd, i, j;
    float* buffer;
    float* dst;
    float** pcm;
    float* bufferTemp;
    float* pcmTemp;
    OggVorbis_File* file;
    vorbis_info* info;
    
    result = PlankResult_OK;
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    file = &ogg->oggVorbisFile;

    numFramesRemaining      = numFrames;
    bufferFramesRemaining   = ogg->bufferFrames;         // starts at 0
    bufferFramePosition     = ogg->bufferPosition;       // starts at 0
    bufferSizeInBytes       = pl_DynamicArray_GetSize (&ogg->buffer);
    bytesPerFrame           = p->formatInfo.bytesPerFrame;
    numChannels             = p->formatInfo.numChannels;
    bufferFrameEnd          = bufferSizeInBytes / bytesPerFrame;
    buffer                  = (float*)pl_DynamicArray_GetArray (&ogg->buffer);
    dst                     = (float*)data;
    streamFrameEnd          = p->numFrames;
    bitStream               = ogg->bitStream;
    
    framesRead = 0;
    
    while (numFramesRemaining > 0)
    {
        if (bufferFramesRemaining > 0)
        {
            framesThisTime = pl_MinI (bufferFramesRemaining, numFramesRemaining);
                        
            pl_MemoryCopy (dst, buffer + bufferFramePosition * numChannels, framesThisTime * bytesPerFrame);
            
            bufferFramePosition += framesThisTime;
            bufferFramesRemaining -= framesThisTime;
            numFramesRemaining -= framesThisTime;
            framesRead += framesThisTime;
            
            dst += framesThisTime * numChannels;
        }
        
        if (bufferFramesRemaining == 0)
        {
            bufferTemp = 0;
            pcmTemp = 0;
            pcm = 0;
            
            framesThisTime = (int)ov_read_float (file, &pcm, bufferFrameEnd, &bitStream);
            
            if (bitStream != ogg->bitStream)
            {
                ogg->bitStream = bitStream;
                info = ov_info (file, -1);
                
                if (info->channels != numChannels)
                {
                    result = PlankResult_UnknownError;
                    goto exit;
                }
                
                if (info->rate != (int)p->formatInfo.sampleRate)
                {
                    result = PlankResult_UnknownError;
                    goto exit;
                }
            }
            
            if (framesThisTime == 0)
            {
                result = PlankResult_FileEOF;                
                goto exit;
            }
            else if (framesThisTime < 0)
            {
                // OV_HOLE or OV_EINVAL
                result = PlankResult_FileReadError;
                goto exit;
            }
                        
            bufferFramePosition = 0;
            bufferFramesRemaining = framesThisTime;
            ogg->totalFramesRead += framesThisTime;
                        
            // interleave to buffer...
            
            for (i = 0; i < numChannels; ++i)
            {
                bufferTemp = buffer + i;
                pcmTemp = pcm[i];
                
                for (j = 0; j < framesThisTime; ++j, bufferTemp += numChannels)
                    *bufferTemp = pl_ClipF (pcmTemp[j], -1.f, 1.f);
            }
        }
    }
    
exit:
    if (numFramesRemaining > 0)
        pl_MemoryZero (dst, numFramesRemaining * bytesPerFrame);
    
    ogg->bufferFrames   = bufferFramesRemaining;
    ogg->bufferPosition = bufferFramePosition;

    *framesReadOut = framesRead;
    
    return result;
}


PlankResult pl_AudioFileReader_OggVorbis_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex)
{
    PlankOggVorbisFileReaderRef ogg;
    int err;
    
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    err = ov_pcm_seek_lap (&ogg->oggVorbisFile, frameIndex); // should probably eventually do my own lapping in readframes?
    
    if (err != 0)
        return PlankResult_FileSeekFailed;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileReader_OggVorbis_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex)
{
    PlankOggVorbisFileReaderRef ogg;
    PlankLL pos;
    
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    pos = ov_pcm_tell (&ogg->oggVorbisFile);
    
    if (pos < 0)
        return PlankResult_FileSeekFailed;
    
    *frameIndex = pos;
    
    return PlankResult_OK;
}

#if PLANK_APPLE
#pragma mark Ogg Vorbis Callbacks
#endif

size_t pl_OggVorbisFileReader_ReadCallback (PlankP ptr, size_t size, size_t nmemb, PlankP datasource)
{
    size_t ret;
    PlankResult result;
    PlankAudioFileReaderRef p;
    PlankOggVorbisFileReaderRef ogg;
    int bytesRead;
    
    p = (PlankAudioFileReaderRef)datasource;
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    
    result = pl_File_Read ((PlankFileRef)ogg, ptr, (int)(size * nmemb) / size, &bytesRead);
    ret = bytesRead > 0 ? bytesRead : 0;
    
    if ((result != PlankResult_OK) && (result != PlankResult_FileEOF))
        errno = -1;
    
    return ret;
}

int pl_OggVorbisFileReader_SeekCallback (PlankP datasource, PlankLL offset, int code)
{    
    PlankResult result;
    PlankAudioFileReaderRef p;
    PlankOggVorbisFileReaderRef ogg;
    PlankFileRef file;
    
    p = (PlankAudioFileReaderRef)datasource;
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    file = (PlankFileRef)ogg;
    
    // API says return -1 (OV_FALSE) if the file is not seekable    
    // call inner callback directly
    result = (file->setPositionFunction) (file, offset, code);
        
    return result == PlankResult_OK ? 0 : OV_FALSE;
}

int pl_OggVorbisFileReader_CloseCallback (PlankP datasource)
{
    PlankResult result;
    PlankAudioFileReaderRef p;
    PlankOggVorbisFileReaderRef ogg;
    PlankFileRef file;
    
    p = (PlankAudioFileReaderRef)datasource;
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    file = (PlankFileRef)ogg;
    
    result = pl_File_DeInit (file);
    
    return result == PlankResult_OK ? 0 : OV_FALSE;
}

long pl_OggVorbisFileReader_TellCallback (PlankP datasource)
{
    PlankResult result;
    PlankAudioFileReaderRef p;
    PlankOggVorbisFileReaderRef ogg;
    PlankFileRef file;
    PlankLL position;
    
    p = (PlankAudioFileReaderRef)datasource;
    ogg = (PlankOggVorbisFileReaderRef)p->peer;
    file = (PlankFileRef)ogg;
    
    result = pl_File_GetPosition (file, &position);

    return result == PlankResult_OK ? (long)position : (long)OV_FALSE;
}
#endif // PLANK_OGGVORBIS

// -- Opus Functions -- ////////////////////////////////////////////////////////

#if PLANK_OPUS
#if PLANK_APPLE
#pragma mark Opus Functions
#endif

/*
 Notes..
 
 - Granule position is time in PCM frames of the last PCM frame in a page at 48kHz
 
 
 */

typedef struct PlankOpusFileReader
{
    PlankFile file;
    
    ogg_sync_state      oy;
    ogg_page            og;
    ogg_packet          op;
    ogg_stream_state    os;
    
    PlankLL page_granule;
    PlankLL link_out;
    PlankLL packet_count;
    PlankLL totalStreamBytes;
    
    PlankLL audioPagesStart;

    OpusMSDecoder *st;
    
    int stream_init;
    int eos;
    int has_opus_stream;
    int opus_serialno;
    int total_links;
    int mapping_family;
    int preskip;
    int streams;
    int gran_offset;
    int stream_primed;
    int need_discard;
    
    int rate;
    int channels;
    
    float gain;
    float manual_gain;
    

    PlankDynamicArray buffer;
    int bufferFramePosition;
    int bufferFramesRemaining;
    
} PlankOpusFileReader;

typedef PlankOpusFileReader* PlankOpusFileReaderRef;

static PlankResult pl_AudioFileReader_Opus_ProcessHeader (PlankAudioFileReaderRef p, 
                                                          ogg_packet *op, 
                                                          float manual_gain, 
                                                          OpusMSDecoder** decoder)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    int err;
    OpusMSDecoder *st;
    OpusHeader header;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileReaderRef)p->peer;
    st = PLANK_NULL;
    
    if (opus_header_parse (op->packet, (int)op->bytes, &header) == 0)
        goto exit;
    
    opus->mapping_family = header.channel_mapping;
    opus->channels = header.channels;
    
    if (!opus->rate)
        opus->rate = header.input_sample_rate;
    
    /*If the rate is unspecified we decode to 48000*/
    if (opus->rate == 0)
        opus->rate = PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE;
    
    if (opus->rate < PLANKAUDIOFILE_OPUS_MINSAMPLERATE || opus->rate > PLANKAUDIOFILE_OPUS_MAXSAMPLERATE)
        opus->rate = PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE;
    
    opus->preskip = header.preskip;
    
    st = opus_multistream_decoder_create (PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE, header.channels, header.nb_streams, header.nb_coupled, header.stream_map, &err);
    
    if (err != OPUS_OK)
    {
        if (st)
            opus_multistream_decoder_destroy (st);
        
        st = PLANK_NULL;
    }
    
    if (!st)
        goto exit;
    
    opus->streams = header.nb_streams;
    
    if (header.gain != 0 || manual_gain != 0.f)
    {
        /*Gain API added in a newer libopus version, if we don't have it
         we apply the gain ourselves. We also add in a user provided
         manual gain at the same time.*/
        int gainadj = (int)(manual_gain * 256.f) + header.gain;
        
        err = opus_multistream_decoder_ctl (st, OPUS_SET_GAIN (gainadj));
        
        if (err == OPUS_UNIMPLEMENTED)
            opus->gain = pl_PowD (10.0, gainadj / 5120.0);
        else if (err != OPUS_OK)
            goto exit;
    }
    
exit:
    *decoder = st;
    return result;    
}

static PlankResult pl_AudioFileReader_Opus_ReadNextPage (PlankAudioFileReaderRef p)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    int err, numBytes;
    char *data;
    
    result = PlankResult_OK;
    err = 0;
    opus = (PlankOpusFileReaderRef)p->peer;
    
    if (pl_File_IsEOF (&opus->file))
        return PlankResult_FileEOF;
    
    while (!(err = ogg_sync_pageout (&opus->oy, &opus->og)) && (result == PlankResult_OK)) 
    {
        data = ogg_sync_buffer (&opus->oy, PLANKAUDIOFILE_OPUS_READBYTES);
        result = pl_File_Read (&opus->file, data, PLANKAUDIOFILE_OPUS_READBYTES, &numBytes);
        
        if ((err = ogg_sync_wrote (&opus->oy, numBytes)))
            goto exit;
    }
    
    if (opus->stream_init == 0) 
    {
        err = ogg_stream_init (&opus->os, ogg_page_serialno (&opus->og));
        opus->stream_init = 1;
    }

    if (ogg_page_serialno (&opus->og) != opus->os.serialno) 
        if ((err = ogg_stream_reset_serialno (&opus->os, ogg_page_serialno (&opus->og))))
            goto exit;
    
    if ((err = ogg_stream_pagein (&opus->os, &opus->og)))
        goto exit;
    
    opus->page_granule = ogg_page_granulepos (&opus->og);
    err = 0;
    
exit:
    return err == 0 ? PlankResult_OK : PlankResult_UnknownError;
}

PlankResult pl_AudioFileReader_Opus_Open  (PlankAudioFileReaderRef p, const char* filepath)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    PlankMemoryRef m;
    //PlankLL numFrames;  ?? how ??
    PlankI bytesPerSample;
    PlankB headerDone;
    PlankLL firstPageOffset;
    PlankLL lastPageOffset;
    
    int err;
    int bufferSize;

    m = pl_MemoryGlobal();
    
    // open as opus
    opus = (PlankOpusFileReaderRef)pl_Memory_AllocateBytes (m, sizeof (PlankOpusFileReader));
    
    if (opus == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->peer = opus;
    bytesPerSample = sizeof (float);
    p->formatInfo.format = PLANKAUDIOFILE_FORMAT_OPUS;
    p->formatInfo.encoding = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN;
    p->formatInfo.bitsPerSample = PLANKAUDIOFILE_CHARBITS * bytesPerSample;
    
    pl_MemoryZero (opus, sizeof (PlankOpusFileReader));
    opus->channels = -1;
    opus->gain = 1.f;
    
    err = ogg_sync_init (&opus->oy);

    if ((result = pl_File_Init (&opus->file)) != PlankResult_OK) goto exit;
    if ((result = pl_File_OpenBinaryRead (&opus->file, filepath, PLANK_FALSE, PLANK_FALSE)) != PlankResult_OK) goto exit;
    
    opus->totalStreamBytes = -1; // unseekable
    p->numFrames = -1;
    
    headerDone = PLANK_FALSE;

    while (!headerDone)
    {
        if ((err = ogg_stream_packetout (&opus->os, &opus->op)) == 1)
        {
            /*OggOpus streams are identified by a magic string in the initial stream header.*/
            if (opus->op.b_o_s && opus->op.bytes >= 8 && !memcmp (opus->op.packet, PLANKAUDIOFILE_OPUS_HEAD, 8)) 
            {
                if (!opus->has_opus_stream)
                {
                    opus->opus_serialno     = opus->os.serialno;
                    opus->has_opus_stream   = 1;
                    opus->link_out          = 0;
                    opus->packet_count      = 0;
                    opus->eos               = 0;
                    opus->total_links++;
                }
            }
            
            if (!opus->has_opus_stream || opus->os.serialno != opus->opus_serialno)
            {
                result = PlankResult_AudioFileReaderInavlidType;
                goto exit;
            }
            
            /*If first packet in a logical stream, process the Opus header*/
            if (opus->packet_count == 0)
            {
                result = pl_AudioFileReader_Opus_ProcessHeader (p, &opus->op, opus->manual_gain, &opus->st);
                
                if (!opus->st)
                {
                    result = PlankResult_AudioFileReaderInavlidType;
                    goto exit;
                }
                
                /*Remember how many samples at the front we were told to skip
                 so that we can adjust the timestamp counting.*/
                opus->gran_offset = opus->preskip;
                opus->packet_count++;
            }
            else if (opus->packet_count == 1)
            {
                // comments
                // ..etc
                
                headerDone = PLANK_TRUE;
                opus->packet_count++;
                
                if ((result = pl_File_GetPosition (&opus->file, &opus->audioPagesStart)) != PlankResult_OK)
                    goto exit;
            }
        }
        else 
        {
            result = pl_AudioFileReader_Opus_ReadNextPage (p);
            
            if (result != PlankResult_OK)
                goto exit;
        }
    }
    
    if ((result = pl_File_SetPositionEnd (&opus->file)) == PlankResult_OK) // only if seekable
    {
        if ((result = pl_File_GetPosition (&opus->file, &opus->totalStreamBytes)) != PlankResult_OK) goto exit;
    
        if ((result = pl_OggFile_FindNextPageOffset (&opus->file, opus->totalStreamBytes, 0, opus->totalStreamBytes, &firstPageOffset)) != PlankResult_OK) goto exit;
        if ((result = pl_OggFile_FindPrevPageOffset (&opus->file, opus->totalStreamBytes, 0, opus->totalStreamBytes, &lastPageOffset)) != PlankResult_OK) goto exit;
        
        if ((result = pl_File_SetPosition (&opus->file, lastPageOffset)) != PlankResult_OK) goto exit;
        
        if ((err = ogg_sync_reset (&opus->oy)) != 0)
        {
            result = PlankResult_UnknownError;
            goto exit;
        }
        
        if ((err = ogg_stream_reset (&opus->os)) != OPUS_OK)
        {
            result = PlankResult_UnknownError;
            goto exit;
        }
        
        result = pl_AudioFileReader_Opus_ReadNextPage (p);
        
        if (result == PlankResult_OK)
            p->numFrames = opus->page_granule;
        
        // resync
        if ((result = pl_AudioFileReader_Opus_SetFramePosition (p, 0)) != PlankResult_OK) goto exit;
    }
    
    p->formatInfo.numChannels   = opus->channels;
    p->formatInfo.sampleRate    = opus->rate;
    p->formatInfo.bytesPerFrame = opus->channels * bytesPerSample;
    
    bufferSize = PLANKAUDIOFILE_OPUS_MAXFRAMESIZE * p->formatInfo.bytesPerFrame;
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&opus->buffer, 1, bufferSize, PLANK_FALSE)) != PlankResult_OK) 
        goto exit;

    // this goes here as we only goto exit if something bad happened
    p->readFramesFunction       = pl_AudioFileReader_Opus_ReadFrames;
    p->setFramePositionFunction = pl_AudioFileReader_Opus_SetFramePosition;
    p->getFramePositionFunction = pl_AudioFileReader_Opus_GetFramePosition;
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Opus_Close (PlankAudioFileReaderRef p)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    int err;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    opus = (PlankOpusFileReaderRef)p->peer;

    if (opus->st)
        opus_multistream_decoder_destroy (opus->st);
    
    opus->st = NULL;
    
    if (opus->stream_init)
        err = ogg_stream_clear (&opus->os);
        
    err = ogg_sync_clear (&opus->oy);
        
    result = pl_File_DeInit (&opus->file);

    if ((result = pl_DynamicArray_DeInit (&opus->buffer)) != PlankResult_OK) goto exit;

    pl_Memory_Free (m, opus);
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Opus_FillBuffer (PlankAudioFileReaderRef p)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    int err;
    int i;
    int ret;
    int frame_size;
    PlankLL maxout;
    PlankLL outsamp;
    PlankB frameDone;
    float* buffer;
    int bufferSize;
    int numFrames;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileReaderRef)p->peer;
    
    if (!opus->stream_init || 
        !opus->has_opus_stream ||
        (opus->packet_count < 2))
    {
        result = PlankResult_AudioFileReaderNotReady;
        goto exit;
    }
    
    if (opus->eos)
    {
        result = PlankResult_FileEOF;
        goto exit;
    }
    
    buffer = pl_DynamicArray_GetArray (&opus->buffer);
    bufferSize = pl_DynamicArray_GetSize (&opus->buffer);
    numFrames = bufferSize / p->formatInfo.bytesPerFrame;
    
    frameDone = PLANK_FALSE;
    
    while (!frameDone)
    {
        if ((err = ogg_stream_packetout (&opus->os, &opus->op)) == 1)
        {
            if (!opus->has_opus_stream || opus->os.serialno != opus->opus_serialno)
            {
                result = PlankResult_UnknownError;
                goto exit;
            }
            
            /*End of stream condition*/
            if (opus->op.e_o_s && opus->os.serialno == opus->opus_serialno)
                opus->eos = 1; /* don't care for anything except opus eos */
            
            ret = opus_multistream_decode_float (opus->st, 
                                                 opus->op.packet, 
                                                 opus->op.bytes, 
                                                 buffer, 
                                                 numFrames, 0);
            
            frame_size = ret < 0 ? 0 : ret;
            
            if (frame_size > 0)
            {
                if (opus->gain != 0)
                {
                    for (i = 0; i < frame_size * opus->channels; i++)
                        buffer[i] *= opus->gain;
                }
                
                maxout = ((opus->page_granule - opus->gran_offset) * opus->rate / PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE);
                maxout -= opus->link_out;
                outsamp = maxout; // should be actual samples output..?
                opus->link_out += outsamp;
                
                opus->bufferFramePosition = 0;
                opus->bufferFramesRemaining = frame_size;
                
                frameDone = PLANK_TRUE;            
                opus->packet_count++;
                opus->stream_primed = 1;
                
                if (opus->need_discard > 0)
                {
                    opus->need_discard -= frame_size;
                    
                    if (opus->need_discard > 0)
                    {
                        frameDone = PLANK_FALSE; // cancel, discard the whole frame after seeking
                    }
                    else if (opus->need_discard < 0)
                    {
                        // some of the frame was usable.. work out how much and copy into place.
                        
                        frame_size += opus->need_discard;
                                                
                        pl_MemoryCopy (buffer, 
                                       buffer - opus->need_discard * p->formatInfo.bytesPerFrame, 
                                       frame_size * p->formatInfo.bytesPerFrame);
                        
                        opus->need_discard = 0;
                    }
                }
            }
        }
        else 
        {
            result = pl_AudioFileReader_Opus_ReadNextPage (p);
            
            if (result != PlankResult_OK)
                goto exit;
        }
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Opus_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data, int *framesReadOut)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    float* buffer;
    float* dst;
    int bufferSizeInBytes;
    int numFramesRemaining;
    int bufferFrameEnd;
    int bytesPerFrame;
    int numChannels;
    int framesRead;
    int framesThisTime;
    //PlankLL temp;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileReaderRef)p->peer;
            
    //pl_File_GetPosition(&opus->file, &temp);
    
    numFramesRemaining      = numFrames;
    bufferSizeInBytes       = pl_DynamicArray_GetSize (&opus->buffer);
    bytesPerFrame           = p->formatInfo.bytesPerFrame;
    numChannels             = p->formatInfo.numChannels;
    bufferFrameEnd          = bufferSizeInBytes / bytesPerFrame;
    buffer                  = (float*)pl_DynamicArray_GetArray (&opus->buffer);
    dst                     = (float*)data;

    framesRead = 0;
    
    while (numFramesRemaining > 0)
    {
        if (opus->bufferFramesRemaining > 0)
        {
            framesThisTime = pl_MinI (opus->bufferFramesRemaining, numFramesRemaining);
            
            pl_MemoryCopy (dst, buffer + opus->bufferFramePosition * numChannels, framesThisTime * bytesPerFrame);
            
            opus->bufferFramePosition += framesThisTime;
            opus->bufferFramesRemaining -= framesThisTime;
            numFramesRemaining -= framesThisTime;
            framesRead += framesThisTime;
            
            dst += framesThisTime * numChannels;
        }
        
        if (opus->bufferFramesRemaining == 0)
        {
            result = pl_AudioFileReader_Opus_FillBuffer (p);
            
            if (result != PlankResult_OK)
                goto exit;
        }
    }
    
exit:
    if (numFramesRemaining > 0)
        pl_MemoryZero (dst, numFramesRemaining * bytesPerFrame);
        
    *framesReadOut = framesRead;
    
    return result;
    
}

PlankResult pl_AudioFileReader_Opus_SetFramePosition (PlankAudioFileReaderRef p, const PlankLL frameIndex)
{
    PlankResult result;
    PlankOpusFileReaderRef opus;
    PlankLL prevGranule;
    PlankLL frameIndexPreRoll;
    
    int err;
    int minDiscard;
    int pageLength;
    int pageRemainPreRoll;
    
    result = PlankResult_OK;
    opus = (PlankOpusFileReaderRef)p->peer;
    
    opus->eos = 0;
    minDiscard = opus->rate * PLANKAUDIOFILE_OPUS_PREROLL_MS / 1000;
    
    if ((err = ogg_sync_reset (&opus->oy)) != 0)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((err = ogg_stream_reset (&opus->os)) != OPUS_OK)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((err = opus_multistream_decoder_ctl (opus->st, OPUS_RESET_STATE)) != OPUS_OK)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }

    if (frameIndex == 0)
    {
        // reset to start of stream...
        if ((result = pl_File_SetPosition (&opus->file, 0)) != PlankResult_OK) goto exit;
        
        // then read 2 pages.
        if ((result = pl_AudioFileReader_Opus_ReadNextPage (p)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_Opus_ReadNextPage (p)) != PlankResult_OK) goto exit;
        
        if (opus->stream_primed)
            opus->need_discard = 0;
    }
    else 
    {
        // slow but works..
        frameIndexPreRoll = frameIndex - minDiscard;
        
        // reset to start of stream...
        if ((result = pl_File_SetPosition (&opus->file, 0)) != PlankResult_OK) goto exit;
        
        // then read 2 pages.
        if ((result = pl_AudioFileReader_Opus_ReadNextPage (p)) != PlankResult_OK) goto exit;
        if ((result = pl_AudioFileReader_Opus_ReadNextPage (p)) != PlankResult_OK) goto exit;
                        
        while (opus->page_granule < frameIndex)
        {
            prevGranule = opus->page_granule;
            if ((result = pl_AudioFileReader_Opus_ReadNextPage (p)) != PlankResult_OK) goto exit;
        }
        
        pageLength = (int)(opus->page_granule - prevGranule);
        pageRemainPreRoll = (int)(opus->page_granule - frameIndexPreRoll);
        
        if (pageRemainPreRoll <= minDiscard)
            opus->need_discard = pageLength + (minDiscard - pageRemainPreRoll);
        else 
            opus->need_discard = (int)(frameIndex - prevGranule);
        
    }
    
    opus->need_discard = pl_MaxI (opus->need_discard, minDiscard);
    
    opus->stream_primed = 0;
    opus->bufferFramePosition = 0;
    opus->bufferFramesRemaining = 0;
    
exit:
    return result;
}

PlankResult pl_AudioFileReader_Opus_GetFramePosition (PlankAudioFileReaderRef p, PlankLL *frameIndex)
{
    (void)p;
    (void)frameIndex;
    return PlankResult_UnknownError;
}

#endif