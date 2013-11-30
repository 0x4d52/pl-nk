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

#include "../core/plank_StandardHeader.h"
#include "plank_IffFileWriter.h"
#include "plank_IffFileReader.h"
#include "../maths/plank_Maths.h"

#define PLANKIFFFILEWRITER_COPYLENGTH 256

// private functions
PlankResult pl_IffFileWriter_FindLastChunk (PlankIffFileWriterRef p, PlankIffFileWriterChunkInfo** lastChunkInfo);
PlankResult pl_IffFileWriter_RewriteFileUpdatingChunkInfo (PlankIffFileWriterRef p, PlankIffFileWriterChunkInfo* updatedChunkInfo);

PlankIffFileWriterRef pl_IffFileWriter_CreateAndInit()
{
    PlankIffFileWriterRef p;
    p = pl_IffFileWriter_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_IffFileWriter_Init (p) != PlankResult_OK)
            pl_IffFileWriter_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankIffFileWriterRef pl_IffFileWriter_Create()
{
    PlankMemoryRef m;
    PlankIffFileWriterRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankIffFileWriterRef)pl_Memory_AllocateBytes (m, sizeof (PlankIffFileWriter));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankIffFileWriter));
    
    return p;
}

PlankResult pl_IffFileWriter_Init (PlankIffFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    
    pl_MemoryZero (p, sizeof (PlankIffFileWriter));
    pl_File_Init ((PlankFileRef)p);
    
    result = pl_DynamicArray_InitWithItemSizeAndCapacity (&p->chunkInfos, sizeof(PlankIffFileWriterChunkInfo), 16);
    
//exit:
    return result;
}

PlankResult pl_IffFileWriter_DeInit (PlankIffFileWriterRef p)
{
    PlankResult result;

    result = PlankResult_OK;

    if ((result = pl_IffFileWriter_Close (p)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->chunkInfos)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_IffFileWriter_Destroy (PlankIffFileWriterRef p)
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
        
    if ((result = pl_IffFileWriter_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    
    result = pl_Memory_Free (m, p);       
    
exit:
    return result;
}

PlankFileRef pl_IffFileWriter_GetFile (PlankIffFileWriterRef p)
{
    return (PlankFileRef)p;
}

static PlankResult pl_IffFileWriter_ParseMainInfo (PlankIffFileWriterRef p, const char* mainID, const char* formatID, const int idType)
{
    PlankResult result = PlankResult_OK;
    
    if (idType == PLANKIFFFILE_ID_FCC)
    {
        p->common.headerInfo.idType            = idType;
        p->common.headerInfo.mainID.fcc        = pl_FourCharCode (mainID);
        p->common.headerInfo.formatID.fcc      = pl_FourCharCode (formatID);
        
        if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("RIFF"))
        {
            p->common.headerInfo.initMainLength     = 4;
            p->common.headerInfo.junkID.fcc         = pl_FourCharCode ("JUNK");
            p->common.headerInfo.lengthSize         = 4;
            p->common.headerInfo.mainHeaderEnd      = 4 + p->common.headerInfo.lengthSize + 4;
            p->common.headerInfo.headerLength       = 4 + p->common.headerInfo.lengthSize;
            p->common.headerInfo.alignment          = 2;
        }
        else if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("FORM"))
        {
            p->common.headerInfo.initMainLength     = 4;
            p->common.headerInfo.junkID.fcc         = pl_FourCharCode ("    "); // Iff junk ID is four spaces
            p->common.headerInfo.lengthSize         = 4;
            p->common.headerInfo.mainHeaderEnd      = 4 + p->common.headerInfo.lengthSize + 4;
            p->common.headerInfo.headerLength       = 4 + p->common.headerInfo.lengthSize;
            p->common.headerInfo.alignment          = 2;
        }
        else if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("caff"))
        {
            p->common.headerInfo.initMainLength     = 0;
            p->common.headerInfo.junkID.fcc         = pl_FourCharCode ("free");
            p->common.headerInfo.lengthSize         = 8;
            p->common.headerInfo.mainHeaderEnd      = 4 + 2 + 2;
            p->common.headerInfo.headerLength       = 4 + p->common.headerInfo.lengthSize;
            p->common.headerInfo.alignment          = 2;
        }
        else
        {
            p->common.headerInfo.initMainLength     = 4;
            p->common.headerInfo.junkID.fcc         = pl_FourCharCode ("JUNK");
            p->common.headerInfo.lengthSize         = 4;
            p->common.headerInfo.mainHeaderEnd      = 4 + p->common.headerInfo.lengthSize + 4;
            p->common.headerInfo.headerLength       = 4 + p->common.headerInfo.lengthSize;
            p->common.headerInfo.alignment          = 2;
        }
    }
    else if (idType == PLANKIFFFILE_ID_GUID)
    {
        p->common.headerInfo.idType = idType;
        
        if ((result = pl_GUID_InitString (&p->common.headerInfo.mainID.guid, mainID)) != PlankResult_OK) goto exit;
        if ((result = pl_GUID_InitString (&p->common.headerInfo.formatID.guid, formatID)) != PlankResult_OK) goto exit;
        
        if (pl_GUID_EqualWithString (&p->common.headerInfo.mainID.guid, PLANKIFFFILE_W64_RIFF_ID))
        {
            pl_GUID_InitChunkString (&p->common.headerInfo.junkID.guid, PLANKIFFFILE_W64_JUNK_ID);
            p->common.headerInfo.initMainLength     = 16;
            p->common.headerInfo.lengthSize         = 8;
            p->common.headerInfo.mainHeaderEnd      = 16 + 8 + 16;
            p->common.headerInfo.headerLength       = 16 + p->common.headerInfo.lengthSize;
            p->common.headerInfo.alignment          = 8;
        }
        else
        {
            pl_GUID_InitChunkString (&p->common.headerInfo.junkID.guid, PLANKIFFFILE_W64_JUNK_ID);
            p->common.headerInfo.initMainLength     = 16;
            p->common.headerInfo.lengthSize         = 8;
            p->common.headerInfo.mainHeaderEnd      = 16 + 8 + 16;
            p->common.headerInfo.headerLength       = 16 + p->common.headerInfo.lengthSize;
            p->common.headerInfo.alignment          = 8;
        }
    }
    else
    {
        result = PlankResult_UnknownError;
        goto exit;
    };
    
    p->common.headerInfo.mainLength = p->common.headerInfo.initMainLength;
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_OpenReplacing (PlankIffFileWriterRef p, 
                                            const char* filepath, 
                                            const PlankB bigEndian, 
                                            const char* mainID, 
                                            const char* formatID,
                                            const int idType)
{
    PlankResult result = PlankResult_OK;
    
    result = pl_File_OpenBinaryWrite ((PlankFileRef)p, 
                                      filepath, 
                                      PLANK_TRUE, // and read
                                      PLANK_TRUE, // replace existing 
                                      bigEndian);
    
    if (result != PlankResult_OK)
        goto exit;
    
    if ((result = pl_IffFileWriter_ParseMainInfo (p, mainID, formatID, idType)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_OpenWithFile (PlankIffFileWriterRef p,
                                           PlankFileRef file,
                                           const char* mainID,
                                           const char* formatID,
                                           const int idType)
{
    PlankResult result = PlankResult_OK;
    int mode;
    
    if ((result = pl_File_GetMode (file, &mode)) != PlankResult_OK) goto exit;
    
    if (!(mode & PLANKFILE_BINARY))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (!(mode & PLANKFILE_READ))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
    
    if (!(mode & PLANKFILE_WRITE))
    {
        result = PlankResult_AudioFileInavlidType;
        goto exit;
    }
        
    pl_MemoryCopy ((PlankFileRef)p, file, sizeof (PlankFile));
    pl_MemoryZero (file, sizeof (PlankFile));

    if ((result = pl_IffFileWriter_ParseMainInfo (p, mainID, formatID, idType)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_Close (PlankIffFileWriterRef p)
{
    PlankResult result = PlankResult_OK;

    if ((result = pl_IffFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    if ((result = pl_File_DeInit ((PlankFileRef)p)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_IffFileWriter_SeekChunk (PlankIffFileWriterRef p, const PlankLL startPositionInit, const char* chunkIDstr, PlankIffFileWriterChunkInfoRef* chunkInfo, PlankB* isLastChunk)
{
    PlankLL pos;
    PlankIffFileWriterChunkInfoRef chunkInfo_i;
    PlankIffFileWriterChunkInfo* chunkInfos;
    PlankIffFileWriterChunkInfo* currentChunk;
    PlankResult result;
    int numChunks, i, chunkHeaderLength, winnerIndex;
    PlankLL len, lastPosition, startPosition, offsetFromStartPosition, minOffsetFromStartPosition;
    PlankIffID chunkID;
    
    result                      = PlankResult_OK;
    numChunks                   = (int)pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos                  = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    pos                         = 0;
    len                         = 0;
    currentChunk                = 0;
    lastPosition                = 0;
    winnerIndex                 = -1;
    chunkHeaderLength           = p->common.headerInfo.lengthSize + pl_IffFile_ChunkIDLength ((PlankIffFileRef)p);
    minOffsetFromStartPosition  = PLANK_LL_MAX;

    if ((result = pl_IffFile_InitID ((PlankIffFileRef)p, chunkIDstr, &chunkID)) != PlankResult_OK) goto exit;

    if (startPositionInit < 0)
    {
        if (p->currentChunk)
        {
            startPosition = p->currentChunk->chunkPos;
        }
        else
        {
            if ((result = pl_File_GetPosition ((PlankFileRef)p, &startPosition)) != PlankResult_OK) goto exit;
        }
    }
    else
    {
        startPosition = startPositionInit;
    }
    
    // find chunk closest to the start position
    for (i = 0; i < numChunks; ++i)
    {
        chunkInfo_i = &chunkInfos[i];
        
        if (pl_IffFile_IsNullID ((PlankIffFileRef)p, &chunkInfo_i->chunkID))
            continue; // this was a deleted chunk
            
        lastPosition = pl_MaxLL (lastPosition, chunkInfo_i->chunkPos);
        offsetFromStartPosition = chunkInfo_i->chunkPos - startPosition;
        
        if (((pl_IffFile_EqualIDs ((PlankIffFileRef)p, &chunkInfo_i->chunkID, &chunkID)) || ((pl_IffFile_EqualIDs ((PlankIffFileRef)p, &chunkID, pl_IffFileAnyID())))) &&
            (offsetFromStartPosition >= 0) &&
            (offsetFromStartPosition < minOffsetFromStartPosition))
        {
            minOffsetFromStartPosition = offsetFromStartPosition;
            winnerIndex = i;
        }
    }
    
    if (winnerIndex >= 0)
    {
        currentChunk = &chunkInfos[winnerIndex];
        
        len = currentChunk->chunkLength;
        pos = currentChunk->chunkPos;
        
        if ((result = pl_File_SetPosition ((PlankFileRef)p, pos)) != PlankResult_OK) goto exit;
        
        p->currentChunk = currentChunk;
    }
        
    if (chunkInfo)
        *chunkInfo = currentChunk;
            
    if (isLastChunk)
        *isLastChunk = p->currentChunk ? p->currentChunk->chunkPos == lastPosition : PLANK_FALSE;
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_WriteChunk (PlankIffFileWriterRef p, const PlankLL startPosition, const char* chunkIDstr, const void* data, const PlankUI dataLength, const PlankIffFileWriterMode mode)
{
    PlankIffFileWriterChunkInfo newChunkInfo;
    PlankIffFileWriterChunkInfo* origChunkInfo;
    PlankResult result;
    PlankLL chunkDataPos, chunkStartPos, mainLength, originalChunkLength, alignedLength;
    PlankB isLastChunk, isNewChunk;
    PlankIffID chunkID;
    int idLength;
    
    result = PlankResult_OK;
    isNewChunk = PLANK_FALSE;
    idLength = pl_IffFile_ChunkIDLength ((PlankIffFileRef)p);
    
    if ((result = pl_IffFile_InitID ((PlankIffFileRef)p, chunkIDstr, &chunkID)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_SeekChunk (p, startPosition, chunkIDstr, &origChunkInfo, &isLastChunk)) != PlankResult_OK) goto exit;

    if (origChunkInfo == 0)
    {
        // chunk doesn't exist...
        originalChunkLength = 0;
                
        // for 64-bit actually we don't need to seek the last chunk, just seek to the end from the main length!
//        chunkStartPos = p->common.headerInfo.mainLength + p->common.headerInfo.headerLength; // - p->common.headerInfo.mainLengthOffset;
        chunkStartPos = p->common.headerInfo.mainLength + p->common.headerInfo.mainHeaderEnd - p->common.headerInfo.initMainLength;

        chunkDataPos  = chunkStartPos + idLength + p->common.headerInfo.lengthSize;
        isLastChunk   = PLANK_TRUE;
        isNewChunk    = PLANK_TRUE;
    }
    else
    {
        originalChunkLength = origChunkInfo->chunkLength;
        chunkDataPos = origChunkInfo->chunkPos;
        chunkStartPos = chunkDataPos - idLength - p->common.headerInfo.lengthSize;
    }
    
    newChunkInfo.chunkID       = chunkID;
    newChunkInfo.chunkPos      = chunkDataPos; // store the data pos
    
    switch (mode)
    {
        case PLANKIFFFILEWRITER_MODEAPPEND:         newChunkInfo.chunkLength = originalChunkLength + dataLength;                    break;
        case PLANKIFFFILEWRITER_MODEREPLACEGROW:    newChunkInfo.chunkLength = pl_MaxLL ((PlankLL)dataLength, originalChunkLength); break;
        case PLANKIFFFILEWRITER_MODEREPLACESHRINK:  newChunkInfo.chunkLength = pl_MinLL ((PlankLL)dataLength, originalChunkLength); break;
        default:
            result = PlankResult_FileModeInvalid;
            goto exit;
    }
        
    if (newChunkInfo.chunkLength > originalChunkLength)
    {
        if (!isLastChunk)
        {
            if (isNewChunk)
            {
                // shouldn't happen but the next few lines assume this is not a new chunk
                result = PlankResult_FileWriteError;
                goto exit;
            }
            
            if (mode != PLANKIFFFILEWRITER_MODEREPLACEGROW)
            {
                result = PlankResult_FileModeInvalid;
                goto exit;
            }
            
            if ((result = pl_IffFileWriter_RewriteFileUpdatingChunkInfo (p, &newChunkInfo)) != PlankResult_OK) goto exit;
        }
    }
    else if (newChunkInfo.chunkLength < originalChunkLength)
    {
        if (!isNewChunk && (mode == PLANKIFFFILEWRITER_MODEREPLACESHRINK))
        {
            if ((result = pl_IffFileWriter_RewriteFileUpdatingChunkInfo (p, &newChunkInfo)) != PlankResult_OK) goto exit;
        }
    }
    
    // chunkInfo should still be valid as only chunks after it may have moved position
    
    if ((result = pl_File_SetPosition ((PlankFileRef)p, chunkStartPos)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &chunkID)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
    
    if (mode == PLANKIFFFILEWRITER_MODEAPPEND)
    {
        // add offset
        if ((result = pl_File_SetPosition ((PlankFileRef)p, newChunkInfo.chunkPos + originalChunkLength)) != PlankResult_OK) goto exit;
    }
    
    if (dataLength > 0)
    {
        if (data)
        {
            if ((result = pl_File_Write ((PlankFileRef)p, data, (int)dataLength)) != PlankResult_OK) goto exit;
        }
        else
        {
            if ((result = pl_File_WriteZeros ((PlankFileRef)p, (int)dataLength)) != PlankResult_OK) goto exit;
        }
        
        alignedLength = pl_AlignULL (newChunkInfo.chunkLength, p->common.headerInfo.alignment);
        
        if (newChunkInfo.chunkLength < alignedLength)
        {
            if ((result = pl_File_WriteZeros ((PlankFileRef)p, (int)(alignedLength - newChunkInfo.chunkLength))) != PlankResult_OK) goto exit;
        }
    }
    
    mainLength = p->common.headerInfo.mainLength - pl_AlignULL (originalChunkLength, p->common.headerInfo.alignment) + pl_AlignULL (newChunkInfo.chunkLength, p->common.headerInfo.alignment);

    if (isNewChunk)
        mainLength += idLength + p->common.headerInfo.lengthSize;

    if (p->common.headerInfo.mainLength != mainLength)
    {
        p->common.headerInfo.mainLength = mainLength;
        if ((result = pl_IffFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    }    
    
    if (isNewChunk)
    {        
        if ((result = pl_DynamicArray_AddItem (&p->chunkInfos, &newChunkInfo)) != PlankResult_OK) goto exit;
    }
    else
    {
        origChunkInfo->chunkLength = newChunkInfo.chunkLength;
    }
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_WriteHeader (PlankIffFileWriterRef p)
{
    PlankResult result = PlankResult_OK;
    PlankLL length = p->common.headerInfo.mainLength;
    
    if ((result = pl_File_SetPosition ((PlankFileRef)p, 0)) != PlankResult_OK) goto exit;
    
    if ((p->common.headerInfo.idType == PLANKIFFFILE_ID_FCC) &&
        (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("caff")))
    {
        if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &p->common.headerInfo.mainID)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUS ((PlankFileRef)p, 1)) != PlankResult_OK) goto exit; // version
        if ((result = pl_File_WriteUS ((PlankFileRef)p, 0)) != PlankResult_OK) goto exit; // reserved
    }
    else
    {
        if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &p->common.headerInfo.mainID)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, length)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &p->common.headerInfo.formatID)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_RenameChunk (PlankIffFileWriterRef p, const PlankLL startPosition, const char* oldChunkID, const char* newChunkID)
{
    PlankIffFileWriterChunkInfo* origChunkInfo;
    PlankResult result;
    PlankIffID chunkID;
    int idLength;
    
    result = PlankResult_OK;
    
    if ((result = pl_IffFile_InitID ((PlankIffFileRef)p, newChunkID, &chunkID)) != PlankResult_OK) goto exit;

    idLength = pl_IffFile_ChunkIDLength ((PlankIffFileRef)p);
    
    if ((result = pl_IffFileWriter_SeekChunk (p, startPosition, oldChunkID, &origChunkInfo, 0)) != PlankResult_OK) goto exit;

    if (!origChunkInfo)
    {
        result = PlankResult_IffFileReaderChunkNotFound;
        goto exit;
    }
        
    if ((result = pl_File_SetPosition ((PlankFileRef)p, origChunkInfo->chunkPos - idLength - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &chunkID)) != PlankResult_OK) goto exit;

    origChunkInfo->chunkID = chunkID;

    // seek back to the position of the chunk's data
    if ((result = pl_File_SetPosition ((PlankFileRef)p, origChunkInfo->chunkPos)) != PlankResult_OK) goto exit;

    p->currentChunk = origChunkInfo;

exit:
    return result;
}

PlankResult pl_IffFileWriter_ResizeChunk (PlankIffFileWriterRef p, const PlankLL startPosition, const char* chunkID, const PlankLL newLength, const PlankB concatenateJunk)
{
    PlankIffFileWriterChunkInfo updatedChunkInfo;
    PlankIffFileWriterChunkInfo newJunkChunkInfo;
    PlankIffFileWriterChunkInfo* thisChunkInfo;
    PlankIffFileWriterChunkInfo* nextChunkInfo;
    
    PlankLL chunkChange, alignedNewLength, position;
    PlankIffID nextChunkID;
    PlankB thisChunkIsLast, nextChunkIsLast;
    PlankResult result;
    int chunkHeaderLength;
    char chunkIDStr[64];
    
    result = PlankResult_OK;
    
    if ((result = pl_IffFileWriter_SeekChunk (p, startPosition, chunkID, &thisChunkInfo, &thisChunkIsLast)) != PlankResult_OK) goto exit;
    
    if (!thisChunkInfo)
    {
        result = PlankResult_IffFileReaderChunkNotFound;
        goto exit;
    }
    
    if (newLength == thisChunkInfo->chunkLength)
        goto exit; // nothing to do!
    
    alignedNewLength = pl_AlignLL (newLength, p->common.headerInfo.alignment);
    chunkChange = newLength - thisChunkInfo->chunkLength;
    
    if ((p->common.headerInfo.lengthSize == 4) && (chunkChange > 0x7fffffff))
    {
        // could support this but it is a highly suspicious request to reserve such a large space..
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    chunkHeaderLength = p->common.headerInfo.lengthSize + pl_IffFile_ChunkIDLength ((PlankIffFileRef)p);
    
    if (thisChunkIsLast)
    {
        if (thisChunkIsLast && (chunkChange > 0))
        {
            // if it is getting larger then expand the chunk adding zeros to the end
            if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos + thisChunkInfo->chunkLength)) != PlankResult_OK) goto exit; // not including the original padding byte if present
            if ((result = pl_File_WriteZeros  ((PlankFileRef)p, (int)chunkChange + (int)(alignedNewLength - newLength))) != PlankResult_OK) goto exit; // add padding bytes based on the new length if needed
            if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
            
            thisChunkInfo->chunkLength = newLength;
        }
        else // getting smaller
        {
            // flip the sign
            chunkChange = -chunkChange;
            
            // could put back more complex criteria based on alignment
            if (chunkChange >= chunkHeaderLength)
            {
                // if it is getting smaller, shrink it and add junk at the end
                if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
                
                if (chunkChange > 1)  // keep in case we put back the more specific criteria..
                {
                    // need to add junk
                    newJunkChunkInfo.chunkID     = p->common.headerInfo.junkID;
                    newJunkChunkInfo.chunkPos    = thisChunkInfo->chunkPos + alignedNewLength + chunkHeaderLength;
                    newJunkChunkInfo.chunkLength = pl_AlignULL (chunkChange, p->common.headerInfo.alignment) - chunkHeaderLength;
                    
                    if ((p->common.headerInfo.lengthSize == 4) && (newJunkChunkInfo.chunkLength > 0xffffffff))
                    {
                        // need multiple junk chunks rather than one, unlikely to need to reserve this amount of free space
                        result = PlankResult_UnknownError;
                        goto exit;
                    }
                    
                    if ((result = pl_File_SetPosition ((PlankFileRef)p, newJunkChunkInfo.chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &newJunkChunkInfo.chunkID)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newJunkChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_DynamicArray_AddItem (&p->chunkInfos, &newJunkChunkInfo)) != PlankResult_OK) goto exit;
                }
                
                thisChunkInfo->chunkLength = newLength;
            }
            else goto slowCopy;
        }
    }
    else
    {
        // we know this isn't the last chunk so see if the next chunk is junk
        position = thisChunkInfo->chunkPos + pl_AlignULL (thisChunkInfo->chunkLength, p->common.headerInfo.alignment);
        if ((result = pl_File_SetPosition ((PlankFileRef)p, position)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFile_ReadChunkID ((PlankIffFileRef)p, &nextChunkID)) != PlankResult_OK) goto exit;
        
        pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &nextChunkID, chunkIDStr);
        if ((result = pl_IffFileWriter_SeekChunk (p, position, chunkIDStr, &nextChunkInfo, &nextChunkIsLast)) != PlankResult_OK) goto exit;
        
        if (!nextChunkInfo)
        {
            // something really unexpected happened, we already checked there was another chunk following...
            result = PlankResult_UnknownError;
            goto exit;
        }
        
        if (concatenateJunk && pl_IffFile_EqualIDs ((PlankIffFileRef)p, &nextChunkInfo->chunkID, &p->common.headerInfo.junkID))
        {
            if (chunkChange > 0) // getting bigger
            {
                if ((chunkChange < p->common.headerInfo.alignment) && ((thisChunkInfo->chunkLength % p->common.headerInfo.alignment) != 0))
                {
                    // just change the length and leave the junk alone
                    if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
                    
                    thisChunkInfo->chunkLength =  newLength;
                }
                else if (chunkChange <= (nextChunkInfo->chunkLength + chunkHeaderLength))
                {                    
                    if (chunkChange > nextChunkInfo->chunkLength)
                    {
                        if (((chunkChange - nextChunkInfo->chunkLength) >= p->common.headerInfo.alignment) &&
                            ((chunkChange - nextChunkInfo->chunkLength) < chunkHeaderLength))
                        {
                            goto slowCopy; // incompatible length change
                        }
                        
                        // zero the junk header to avoid the file looking confusing during debugging
                        if ((result = pl_File_SetPosition ((PlankFileRef)p, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                        if ((result = pl_File_WriteZeros  ((PlankFileRef)p, chunkHeaderLength)) != PlankResult_OK) goto exit;
                        
                        // expand this chunk
                        if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                        if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
                        
                        thisChunkInfo->chunkLength =  newLength;
                        
                        // null the junk chunk reference
                        pl_MemoryZero (&nextChunkInfo->chunkID, sizeof (PlankIffID));
                    }
                    else
                    {
                        // zero the junk header to avoid the file looking confusing during debugging
                        if ((result = pl_File_SetPosition ((PlankFileRef)p, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                        if ((result = pl_File_WriteZeros  ((PlankFileRef)p, chunkHeaderLength)) != PlankResult_OK) goto exit;
                        
                        // expand this chunk
                        if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                        if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
                        
                        thisChunkInfo->chunkLength =  newLength;

                        //..and shrink the junk
                        nextChunkInfo->chunkPos    =  thisChunkInfo->chunkPos + pl_AlignULL (thisChunkInfo->chunkLength, p->common.headerInfo.alignment) + chunkHeaderLength;
                        nextChunkInfo->chunkLength -= chunkChange;
                        
                        if ((result = pl_File_SetPosition  ((PlankFileRef)p, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                        if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &nextChunkInfo->chunkID)) != PlankResult_OK) goto exit;
                        if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, nextChunkInfo->chunkLength)) != PlankResult_OK) goto exit;
                    }
                }
                else
                {
                    // recursively concatenate junk chunks as far as we can...
                    alignedNewLength = thisChunkInfo->chunkLength + pl_AlignLL (nextChunkInfo->chunkLength, p->common.headerInfo.alignment) + chunkHeaderLength;
                    if ((result = pl_IffFileWriter_ResizeChunk (p, startPosition, chunkID, alignedNewLength, PLANK_TRUE)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_ResizeChunk (p, startPosition, chunkID, newLength, PLANK_TRUE)) != PlankResult_OK) goto exit;
                }
            }
            else // getting smaller
            {
                // flip the sign
                chunkChange = -chunkChange;
                
                // zero the junk header to avoid the file looking confusing during debugging
                if ((result = pl_File_SetPosition ((PlankFileRef)p, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                if ((result = pl_File_WriteZeros  ((PlankFileRef)p, chunkHeaderLength)) != PlankResult_OK) goto exit;
                
                // resize this chunk
                if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
                
                thisChunkInfo->chunkLength =  newLength;
                
                // move and resize junk
                nextChunkInfo->chunkPos    =  thisChunkInfo->chunkPos + pl_AlignULL (thisChunkInfo->chunkLength, p->common.headerInfo.alignment) + chunkHeaderLength;
                nextChunkInfo->chunkLength += chunkChange;
                
                if ((p->common.headerInfo.lengthSize == 4) && (nextChunkInfo->chunkLength > 0xffffffff))
                {
                    // need multiple junk chunks rather than one, unlikely to need to reserve this amount of free space
                    result = PlankResult_UnknownError;
                    goto exit;
                }
                
                if ((result = pl_File_SetPosition ((PlankFileRef)p, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &nextChunkInfo->chunkID)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, nextChunkInfo->chunkLength)) != PlankResult_OK) goto exit;
            }
        }
        else if ((chunkChange > 0) && (chunkChange < p->common.headerInfo.alignment) && ((thisChunkInfo->chunkLength % p->common.headerInfo.alignment) != 0))
        {
            // just change the chunk length
            if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
            
            thisChunkInfo->chunkLength = newLength;
        }
        else if (chunkChange < 0)
        {
            chunkChange = -chunkChange;
            
            if (chunkChange >= chunkHeaderLength)
            {
                // if it is getting smaller, shrink it and add junk at the end
                if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos - p->common.headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newLength)) != PlankResult_OK) goto exit;
                
                if (chunkChange > 1) // keep in case we put back the more specific criteria..
                {
                    // need to add junk
                    newJunkChunkInfo.chunkID     = p->common.headerInfo.junkID;
                    newJunkChunkInfo.chunkPos    = thisChunkInfo->chunkPos + alignedNewLength + chunkHeaderLength;
                    newJunkChunkInfo.chunkLength = pl_AlignULL (chunkChange, p->common.headerInfo.alignment) - chunkHeaderLength;
                    
                    if ((p->common.headerInfo.lengthSize == 4) && (newJunkChunkInfo.chunkLength > 0xffffffff))
                    {
                        // need multiple junk chunks rather than one, unlikely to need to reserve this amount of free space
                        result = PlankResult_UnknownError;
                        goto exit;
                    }
                    
                    if ((result = pl_File_SetPosition ((PlankFileRef)p, newJunkChunkInfo.chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFile_WriteChunkID ((PlankIffFileRef)p, &newJunkChunkInfo.chunkID)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFile_WriteChunkLength ((PlankIffFileRef)p, newJunkChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_DynamicArray_AddItem (&p->chunkInfos, &newJunkChunkInfo)) != PlankResult_OK) goto exit;
                }
                
                thisChunkInfo->chunkLength = newLength;
            }
            else goto slowCopy;
        }
        else
        {
        slowCopy:
            // we're going to have to do a slow rewrite..
            updatedChunkInfo.chunkID     = thisChunkInfo->chunkID;
            updatedChunkInfo.chunkLength = newLength;
            updatedChunkInfo.chunkPos    = thisChunkInfo->chunkPos;
            
            if ((result = pl_IffFileWriter_RewriteFileUpdatingChunkInfo (p, &updatedChunkInfo)) != PlankResult_OK) goto exit;
        }
    }
    
    // seek back to the position of the chunk's data
    if ((result = pl_File_SetPosition ((PlankFileRef)p, thisChunkInfo->chunkPos)) != PlankResult_OK) goto exit;
    
    p->currentChunk = thisChunkInfo;
    
exit:
    return result;
}


PlankResult pl_IffFileWriter_PrepareChunk (PlankIffFileWriterRef p, const PlankLL startPosition, const char* chunkID, const void* data, const PlankUI dataLength)
{
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterChunkInfoRef chunkInfo;
    char JUNK[64];
    
    pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &p->common.headerInfo.junkID, JUNK);
    if ((result = pl_IffFileWriter_SeekChunk (p, startPosition, JUNK, &chunkInfo, 0)) != PlankResult_OK) goto exit;

    if (!data)
    {        
        if (chunkInfo)
        {
            if ((result = pl_IffFileWriter_ResizeChunk (p, startPosition, JUNK, dataLength, PLANK_FALSE)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFileWriter_RenameChunk (p, startPosition, JUNK, chunkID)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFileWriter_SeekChunk (p,  startPosition, chunkID, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        }
        else
        {
            if ((result = pl_IffFileWriter_WriteChunk (p, startPosition, chunkID, data, dataLength, PLANKIFFFILEWRITER_MODEREPLACEGROW)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFileWriter_SeekChunk (p,  startPosition, chunkID, &chunkInfo, 0)) != PlankResult_OK) goto exit;
        }
    }
    else
    {        
        if (chunkInfo)
        {
            if ((result = pl_IffFileWriter_ResizeChunk (p, startPosition, JUNK, dataLength, PLANK_FALSE)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFileWriter_RenameChunk (p, startPosition, JUNK, chunkID)) != PlankResult_OK) goto exit;
        }
        
        if ((result = pl_IffFileWriter_WriteChunk (p, startPosition, chunkID, data, dataLength, PLANKIFFFILEWRITER_MODEREPLACEGROW)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_SeekChunk (p,  startPosition, chunkID, &chunkInfo, 0)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_FindLastChunk (PlankIffFileWriterRef p, PlankIffFileWriterChunkInfo** lastChunkInfo)
{
    PlankLL lastChunkPos;
    PlankIffFileWriterChunkInfo* chunkInfos;
    PlankIffFileWriterChunkInfo* currentLastChunk;
    PlankResult result;
    int numChunks, i;
    
    result           = PlankResult_OK;
    numChunks        = (int)pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos       = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    lastChunkPos     = 0;
    currentLastChunk = 0;
    
    for (i = 0; i < numChunks; ++i)
    {
        if (pl_IffFile_IsNullID ((PlankIffFileRef)p, &chunkInfos[i].chunkID))
            continue; // this was a deleted chunk
        
        if (chunkInfos[i].chunkPos > lastChunkPos)
        {
            lastChunkPos = chunkInfos[i].chunkPos;
            currentLastChunk = &chunkInfos[i];
        }
    }
    
    *lastChunkInfo = currentLastChunk;
    
//exit:
    return result;
}



PlankResult pl_IffFileWriter_RewriteFileUpdatingChunkInfo (PlankIffFileWriterRef p, PlankIffFileWriterChunkInfo* updatedChunkInfo)
{
    PlankUC copyBuffer[PLANKIFFFILEWRITER_COPYLENGTH];
    PlankResult result = PlankResult_OK;
    PlankIffFileWriterChunkInfo* chunkInfos;
    PlankIffFileWriter tempWriter;
    PlankPath tempPath;
    int mode, numChunks, i, bytesRead, bytesThisTime;
    PlankLL copyChunkLengthRemain;
    PlankB eraseOriginalFile;
    char chunkIDStr[64];
    char otherIDStr[64];
    
    eraseOriginalFile = PLANK_FALSE;
    
    if ((result = pl_File_GetMode ((PlankFileRef)p, &mode)) != PlankResult_OK)              return result;
    if (!(mode & PLANKFILE_READ))                                                           return PlankResult_FileReadError;
    if ((result = pl_Path_InitTemp (&tempPath, "IffWriter-", "tmp")) != PlankResult_OK)     return result;
    if ((result = pl_IffFileWriter_Init (&tempWriter)) != PlankResult_OK)                   goto earlyExit;
    
    pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &p->common.headerInfo.mainID, chunkIDStr);
    pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &p->common.headerInfo.formatID, otherIDStr);

    result = pl_IffFileWriter_OpenReplacing (&tempWriter,
                                             pl_Path_GetFullPath (&tempPath),
                                             pl_File_IsBigEndian ((PlankFileRef)p),
                                             chunkIDStr,
                                             otherIDStr,
                                             p->common.headerInfo.idType);
    
    if (result != PlankResult_OK) goto earlyExit;

    numChunks = (int)pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    
    for (i = 0; i < numChunks; ++i)
    {
        if (pl_IffFile_IsNullID ((PlankIffFileRef)p, &chunkInfos[i].chunkID))
            continue; // this was a deleted chunk
            
        if (!pl_IffFile_EqualIDs ((PlankIffFileRef)p, &chunkInfos[i].chunkID, &updatedChunkInfo->chunkID))
        {            
            if ((result = pl_File_SetPosition ((PlankFileRef)p, chunkInfos[i].chunkPos)) != PlankResult_OK) goto exit;
            
            copyChunkLengthRemain = chunkInfos[i].chunkLength;
            
            while (copyChunkLengthRemain > 0)
            {
                bytesThisTime = (int)pl_MinLL (copyChunkLengthRemain, PLANKIFFFILEWRITER_COPYLENGTH);
                
                if ((result = pl_File_Read ((PlankFileRef)p, copyBuffer, bytesThisTime, &bytesRead)) != PlankResult_OK) goto exit;
                
                if (bytesThisTime != bytesRead)
                {
                    // something is wrong if these don't match here
                    result = PlankResult_FileReadError;
                    goto exit;
                }
                
                pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &chunkInfos[i].chunkID, chunkIDStr);
                                    
                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter,
                                                           PLANKIFFFILE_CURRENTCHUNKPOSITION, chunkIDStr,
                                                           copyBuffer, bytesThisTime,
                                                           PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
                
                copyChunkLengthRemain -= bytesThisTime;
            }
        }
        else
        {
            if (chunkInfos[i].chunkPos != updatedChunkInfo->chunkPos)
            {
                // something is wrong if these don't match here
                result = PlankResult_FileReadError;
                goto exit;
            }
            
            if (updatedChunkInfo->chunkLength < chunkInfos[i].chunkLength)
                eraseOriginalFile = PLANK_TRUE;

            if ((result = pl_File_SetPosition ((PlankFileRef)p, chunkInfos[i].chunkPos)) != PlankResult_OK) goto exit;
            
            copyChunkLengthRemain = pl_MinLL (chunkInfos[i].chunkLength, updatedChunkInfo->chunkLength);

            while (copyChunkLengthRemain > 0)
            {
                bytesThisTime = (int)pl_MinLL (copyChunkLengthRemain, PLANKIFFFILEWRITER_COPYLENGTH);
                
                if ((result = pl_File_Read ((PlankFileRef)p, copyBuffer, bytesThisTime, &bytesRead)) != PlankResult_OK) goto exit;

                if ((int)bytesThisTime != bytesRead)
                {
                    // something is wrong if these don't match here
                    result = PlankResult_FileReadError;
                    goto exit;
                }
                
                pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &chunkInfos[i].chunkID, chunkIDStr);
                
                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter,
                                                           PLANKIFFFILE_CURRENTCHUNKPOSITION, chunkIDStr,
                                                           copyBuffer, bytesThisTime,
                                                           PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
                
                copyChunkLengthRemain -= bytesThisTime;
            }
            
            // pad with zero for the larger size, no need to round to even bytes as pl_IffFileWriter_WriteChunk does this for us
            if (updatedChunkInfo->chunkLength > chunkInfos[i].chunkLength)
            {
                bytesThisTime = (int)(updatedChunkInfo->chunkLength - chunkInfos[i].chunkLength);
                
                pl_MemoryZero (copyBuffer, bytesThisTime);
                pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &chunkInfos[i].chunkID, chunkIDStr);

                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter,
                                                           PLANKIFFFILE_CURRENTCHUNKPOSITION, chunkIDStr,
                                                           copyBuffer, bytesThisTime,
                                                           PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
            }
        }
        
        // after copying a chunk need to copy chunk infos from the temp back to this file's infos        
        if ((result = pl_DynamicArray_GetItem (&tempWriter.chunkInfos, i, &chunkInfos[i])) != PlankResult_OK) goto exit;        
    }
    

    if (eraseOriginalFile)
    {
        if ((result = pl_File_Clear ((PlankFileRef)p)) != PlankResult_OK) goto exit;
    }
    else
    {
        if ((result = pl_File_ResetPosition ((PlankFileRef)p)) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_File_ResetPosition (pl_IffFileWriter_GetFile (&tempWriter))) != PlankResult_OK) goto exit;
    if ((result = pl_File_Copy ((PlankFileRef)p, pl_IffFileWriter_GetFile (&tempWriter), 0)) != PlankResult_OK) goto exit;
    
exit:
    pl_IffFileWriter_DeInit (&tempWriter);
    pl_FileErase (pl_Path_GetFullPath (&tempPath));
    
earlyExit:
    pl_Path_DeInit (&tempPath);

    return result;
}

PlankResult pl_IffFileWriter_PurgeChunkInfos (PlankIffFileWriterRef p)
{
    PlankIffFileWriterChunkInfo* chunkInfos;
    PlankResult result;
    int numChunks, i;
    
    result           = PlankResult_OK;
    numChunks        = (int)pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos       = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    
    for (i = numChunks; --i >= 0;)
    {
        if (pl_IffFile_IsNullID ((PlankIffFileRef)p, &chunkInfos[i].chunkID))
            pl_DynamicArray_RemoveItem (&p->chunkInfos, i);
    }
    
    
//exit:
    return result;
}

PlankIffFileWriterChunkInfoRef pl_IffFileWriter_GetCurrentChunk (PlankIffFileWriterRef p)
{
    return p->currentChunk;
}

