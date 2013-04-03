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
    pl_File_Init (&p->file);
    
    result = pl_DynamicArray_InitWithItemSizeAndCapacity (&p->chunkInfos, sizeof(PlankIffFileWriterChunkInfo), 16);
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_DeInit (PlankIffFileWriterRef p)
{
    PlankResult result;

    result = PlankResult_OK;

    if ((result = pl_IffFileWriter_Close (p)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->chunkInfos)) != PlankResult_OK) goto exit;

exit:
    return PlankResult_UnknownError;
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
    return &p->file;
}

static PlankResult pl_IffFileWriter_ParseMainInfo (PlankIffFileWriterRef p, const char* mainID, const char* formatID, const int idType)
{
    PlankResult result = PlankResult_OK;
    
    if (idType == PLANKIFFFILE_ID_FCC)
    {
        p->headerInfo.idType            = idType;
        p->headerInfo.mainID.fcc        = pl_FourCharCode (mainID);
        p->headerInfo.formatID.fcc      = pl_FourCharCode (formatID);
        
        if (p->headerInfo.mainID.fcc == pl_FourCharCode ("RIFF"))
        {
            p->headerInfo.initMainLength     = 4;
            p->headerInfo.junkID.fcc         = pl_FourCharCode ("JUNK");
            p->headerInfo.lengthSize         = 4;
            p->headerInfo.mainEndOffset      = 8;
            p->headerInfo.headerLength       = 8;
            p->headerInfo.mainLengthOffset   = 0;
            p->headerInfo.alignment          = 2;
        }
        else if (p->headerInfo.mainID.fcc == pl_FourCharCode ("FORM"))
        {
            p->headerInfo.initMainLength     = 4;
            p->headerInfo.junkID.fcc         = pl_FourCharCode ("    "); // Iff junk ID is four spaces
            p->headerInfo.lengthSize         = 4;
            p->headerInfo.mainEndOffset      = 8;
            p->headerInfo.headerLength       = 8;
            p->headerInfo.mainLengthOffset   = 0;
            p->headerInfo.alignment          = 2;
        }
        else if (p->headerInfo.mainID.fcc == pl_FourCharCode ("caff"))
        {
            p->headerInfo.initMainLength     = 0;
            p->headerInfo.junkID.fcc         = pl_FourCharCode ("free");
            p->headerInfo.lengthSize         = 8;
            p->headerInfo.mainEndOffset      = 8;
            p->headerInfo.headerLength       = 8;
            p->headerInfo.mainLengthOffset   = 0;
            p->headerInfo.alignment          = 2;
        }
        else
        {
            p->headerInfo.initMainLength     = 4;
            p->headerInfo.junkID.fcc         = pl_FourCharCode ("JUNK");
            p->headerInfo.lengthSize         = 4;
            p->headerInfo.mainEndOffset      = 8;
            p->headerInfo.headerLength       = 8;
            p->headerInfo.mainLengthOffset   = 0;
            p->headerInfo.alignment          = 2;
        }
    }
    else if (idType == PLANKIFFFILE_ID_GUID)
    {
        p->headerInfo.idType = idType;
        
        if ((result = pl_GUID_InitString (&p->headerInfo.mainID.guid, mainID)) != PlankResult_OK) goto exit;
        if ((result = pl_GUID_InitString (&p->headerInfo.formatID.guid, formatID)) != PlankResult_OK) goto exit;
        
        if (pl_GUID_EqualWithString (&p->headerInfo.mainID.guid, PLANKIFFFILE_W64_RIFF_ID))
        {
            pl_GUID_InitChunkString (&p->headerInfo.junkID.guid, PLANKIFFFILE_W64_JUNK_ID);
            p->headerInfo.initMainLength     = 16 + 8 + 16;
            p->headerInfo.lengthSize         = 8;
            p->headerInfo.mainEndOffset      = 0;
            p->headerInfo.headerLength       = 8 + 16;
            p->headerInfo.mainLengthOffset   = 16 + 8;
            p->headerInfo.alignment          = 8;
        }
        else
        {
            pl_GUID_InitChunkString (&p->headerInfo.junkID.guid, PLANKIFFFILE_W64_JUNK_ID);
            p->headerInfo.initMainLength     = 16 + 8 + 16;
            p->headerInfo.lengthSize         = 8;
            p->headerInfo.mainEndOffset      = 0;
            p->headerInfo.headerLength       = 8 + 16;
            p->headerInfo.mainLengthOffset   = 16 + 8;
            p->headerInfo.alignment          = 8;
        }
    }
    else
    {
        result = PlankResult_UnknownError;
        goto exit;
    };
    
    p->headerInfo.mainLength = p->headerInfo.initMainLength;
    
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
    
    result = pl_File_OpenBinaryWrite (&p->file, 
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
        
    pl_MemoryCopy (&p->file, file, sizeof (PlankFile));
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
    if ((result = pl_File_DeInit (&p->file)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_IffFileWriter_SeekChunk (PlankIffFileWriterRef p, const PlankLL startPositionInit, const char* chunkIDstr, PlankIffFileWriterChunkInfoRef* chunkInfo, PlankB* isLastChunk)
{
    PlankLL pos;
    PlankIffFileWriterChunkInfo* chunkInfos;
    PlankIffFileWriterChunkInfo* currentChunk;
    PlankResult result;
    int numChunks, i;
    PlankLL len, lastPosition, startPosition;
    PlankIffID chunkID;
    
    result       = PlankResult_OK;
    numChunks    = pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos   = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    pos          = 0;
    len          = 0;
    currentChunk = 0;
    lastPosition = 0;
    
    if ((result = pl_IffFileWriter_InitID (p, chunkIDstr, &chunkID)) != PlankResult_OK) goto exit;

    if (startPositionInit < 0)
    {
        if (p->currentChunk)
        {
            startPosition = p->currentChunk->chunkPos;
        }
        else
        {
            if ((result = pl_File_GetPosition (&p->file, &startPosition)) != PlankResult_OK) goto exit;
        }
    }
    else
    {
        startPosition = startPositionInit;
    }
        
    for (i = 0; i < numChunks; ++i)
    {
        if (pl_IffFileWriter_IsNullID (p, &chunkInfos[i].chunkID))
            continue; // this was a deleted chunk
            
        lastPosition = pl_MaxLL (lastPosition, chunkInfos[i].chunkPos);
        
        if ((currentChunk == 0) &&
            ((pl_IffFileWriter_EqualIDs (p, &chunkInfos[i].chunkID, &chunkID)) || ((pl_IffFileWriter_EqualIDs (p, &chunkID, pl_IffFileWriterAnyID())))) &&
            (chunkInfos[i].chunkPos >= startPosition))
        {
            currentChunk = &chunkInfos[i];
            
            len = currentChunk->chunkLength;
            pos = currentChunk->chunkPos;
            
            if ((result = pl_File_SetPosition (&p->file, pos)) != PlankResult_OK) goto exit;
            
            p->currentChunk = currentChunk;
            
            if (!isLastChunk)
                break;
        }
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
    idLength = pl_IffFileWriter_ChunkIDLength (p);
    
    if ((result = pl_IffFileWriter_InitID (p, chunkIDstr, &chunkID)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_SeekChunk (p, startPosition, chunkIDstr, &origChunkInfo, &isLastChunk)) != PlankResult_OK) goto exit;

    if (origChunkInfo == 0)
    {
        // chunk doesn't exist...
        originalChunkLength = 0;
                
        // for 64-bit actually we don't need to seek the last chunk, just seek to the end from the main length!
        chunkStartPos = p->headerInfo.mainLength + p->headerInfo.headerLength - p->headerInfo.mainLengthOffset;
        chunkDataPos  = chunkStartPos + idLength + p->headerInfo.lengthSize;
        isLastChunk   = PLANK_TRUE;
        isNewChunk    = PLANK_TRUE;
    }
    else
    {
        originalChunkLength = origChunkInfo->chunkLength;
        chunkDataPos = origChunkInfo->chunkPos;
        chunkStartPos = chunkDataPos - idLength - p->headerInfo.lengthSize;
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
    
    if ((result = pl_File_SetPosition (&p->file, chunkStartPos)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_WriteChunkID (p, &chunkID)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_WriteChunkLength (p, newChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
    
    if (mode == PLANKIFFFILEWRITER_MODEAPPEND)
    {
        // add offset
        if ((result = pl_File_SetPosition (&p->file, newChunkInfo.chunkPos + originalChunkLength)) != PlankResult_OK) goto exit;
    }
    
    if (dataLength > 0)
    {
        if (data)
        {
            if ((result = pl_File_Write (&p->file, data, (int)dataLength)) != PlankResult_OK) goto exit;
        }
        else
        {
            if ((result = pl_File_WriteZeros (&p->file, (int)dataLength)) != PlankResult_OK) goto exit;
        }
        
        alignedLength = pl_AlignULL (newChunkInfo.chunkLength, p->headerInfo.alignment);
        
        if (newChunkInfo.chunkLength < alignedLength)
        {
            if ((result = pl_File_WriteZeros (&p->file, (int)(alignedLength - newChunkInfo.chunkLength))) != PlankResult_OK) goto exit;
        }
    }
    
    mainLength = p->headerInfo.mainLength - pl_AlignULL (originalChunkLength, p->headerInfo.alignment) + pl_AlignULL (newChunkInfo.chunkLength, p->headerInfo.alignment);

    if (isNewChunk)
        mainLength += idLength + p->headerInfo.lengthSize;

    if (p->headerInfo.mainLength != mainLength)
    {
        p->headerInfo.mainLength = mainLength;
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
    PlankLL length = p->headerInfo.mainLength;
    
    if ((result = pl_File_SetPosition (&p->file, 0)) != PlankResult_OK) goto exit;
    
    if ((p->headerInfo.idType == PLANKIFFFILE_ID_FCC) &&
        (p->headerInfo.mainID.fcc == pl_FourCharCode ("caff")))
    {
        if ((result = pl_IffFileWriter_WriteChunkID (p, &p->headerInfo.mainID)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteUS (&p->file, 1)) != PlankResult_OK) goto exit; // version
        if ((result = pl_File_WriteUS (&p->file, 0)) != PlankResult_OK) goto exit; // reserved
    }
    else
    {
        if ((result = pl_IffFileWriter_WriteChunkID (p, &p->headerInfo.mainID)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_WriteChunkLength (p, length)) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_WriteChunkID (p, &p->headerInfo.formatID)) != PlankResult_OK) goto exit;
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
    
    if ((result = pl_IffFileWriter_InitID (p, newChunkID, &chunkID)) != PlankResult_OK) goto exit;

    idLength = pl_IffFileWriter_ChunkIDLength (p);
    
    if ((result = pl_IffFileWriter_SeekChunk (p, startPosition, oldChunkID, &origChunkInfo, 0)) != PlankResult_OK) goto exit;

    if (!origChunkInfo)
    {
        result = PlankResult_IffFileReaderChunkNotFound;
        goto exit;
    }
        
    if ((result = pl_File_SetPosition (&p->file, origChunkInfo->chunkPos - idLength - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileWriter_WriteChunkID (p, &chunkID)) != PlankResult_OK) goto exit;

    origChunkInfo->chunkID = chunkID;

    // seek back to the position of the chunk's data
    if ((result = pl_File_SetPosition (&p->file, origChunkInfo->chunkPos)) != PlankResult_OK) goto exit;

    p->currentChunk = origChunkInfo;

exit:
    return result;
}

PlankResult pl_IffFileWriter_ResizeChunk (PlankIffFileWriterRef p, const PlankLL startPosition, const char* chunkID, const PlankLL newLength)
{
    PlankIffFileWriterChunkInfo updatedChunkInfo;
    PlankIffFileWriterChunkInfo newJunkChunkInfo;
    PlankIffFileWriterChunkInfo* thisChunkInfo;
    PlankIffFileWriterChunkInfo* nextChunkInfo;
    
    PlankLL chunkChange, alignedNewLength;
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
    
    alignedNewLength = pl_AlignUI (newLength, p->headerInfo.alignment);
    chunkChange = newLength - thisChunkInfo->chunkLength;
    
    if ((p->headerInfo.lengthSize == 4) && (chunkChange > 0x7fffffff))
    {
        // could support this but it is a highly suspicious request to reserve such a large space..
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    chunkHeaderLength = p->headerInfo.lengthSize + pl_IffFileWriter_ChunkIDLength (p);
    
    if (thisChunkIsLast)
    {
        if (thisChunkIsLast && (chunkChange > 0))
        {
            // if it is getting larger then expand the chunk adding zeros to the end
            if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos + thisChunkInfo->chunkLength)) != PlankResult_OK) goto exit; // not including the original padding byte if present
            if ((result = pl_File_WriteZeros  (&p->file, (int)chunkChange + (int)(alignedNewLength - newLength))) != PlankResult_OK) goto exit; // add padding bytes based on the new length if needed
            if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;
            
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
                if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;

                if (chunkChange > 1)
                {
                    // need to add junk
                    newJunkChunkInfo.chunkID     = p->headerInfo.junkID;
                    newJunkChunkInfo.chunkPos    = thisChunkInfo->chunkPos + alignedNewLength + chunkHeaderLength;
                    newJunkChunkInfo.chunkLength = chunkChange + (thisChunkInfo->chunkLength % p->headerInfo.alignment) - chunkHeaderLength;
                    
                    if ((p->headerInfo.lengthSize == 4) && (newJunkChunkInfo.chunkLength > 0xffffffff))
                    {
                        // need multiple junk chunks rather than one, unlikely to need to reserve this amount of free space
                        result = PlankResult_UnknownError;
                        goto exit;
                    }
                    
                    if ((result = pl_File_SetPosition (&p->file, newJunkChunkInfo.chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkID (p, &newJunkChunkInfo.chunkID)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkLength (p, newJunkChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
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
        if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos + pl_AlignULL (thisChunkInfo->chunkLength, p->headerInfo.alignment))) != PlankResult_OK) goto exit;
        if ((result = pl_IffFileWriter_ReadChunkID (p, &nextChunkID)) != PlankResult_OK) goto exit;
        
        pl_IffFileWriter_ChunkIDString (p, &nextChunkID, chunkIDStr);
        if ((result = pl_IffFileWriter_SeekChunk (p, thisChunkInfo->chunkPos, chunkIDStr, &nextChunkInfo, &nextChunkIsLast)) != PlankResult_OK) goto exit;
        
        if (!nextChunkInfo)
        {
            // something really unexpected happened, we already checked there was another chunk following...
            result = PlankResult_UnknownError;
            goto exit;
        }
        
        if (pl_IffFileWriter_EqualIDs (p, &nextChunkInfo->chunkID, &p->headerInfo.junkID))
        {
            if (chunkChange > 0)
            {
                if ((chunkChange < p->headerInfo.alignment) && ((thisChunkInfo->chunkLength % p->headerInfo.alignment) != 0))
                {
                    // just change the length and leave the junk alone
                    if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;

                }
                else if (chunkChange <= nextChunkInfo->chunkLength)
                {
                    // zero the junk header to avoid the file looking confusing during debugging
                    if ((result = pl_File_SetPosition (&p->file, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_File_WriteZeros  (&p->file, chunkHeaderLength)) != PlankResult_OK) goto exit;
                    
                    // expand this chunk
                    if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;

                    thisChunkInfo->chunkLength =  newLength;
                    
                    //..and shrink the junk
                    nextChunkInfo->chunkPos    =  thisChunkInfo->chunkPos + pl_AlignULL (thisChunkInfo->chunkLength, p->headerInfo.alignment) + chunkHeaderLength;
                    nextChunkInfo->chunkLength -= chunkChange;
                    
                    if ((result = pl_File_SetPosition  (&p->file, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkID (p, &nextChunkInfo->chunkID)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkLength (p, nextChunkInfo->chunkLength)) != PlankResult_OK) goto exit;

                }
                else goto slowCopy;
            }
            else // getting smaller
            {
                // flip the sign
                chunkChange = -chunkChange;

                // zero the junk header to avoid the file looking confusing during debugging
                if ((result = pl_File_SetPosition (&p->file, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                if ((result = pl_File_WriteZeros  (&p->file, chunkHeaderLength)) != PlankResult_OK) goto exit;

                // resize this chunk
                if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;

                thisChunkInfo->chunkLength =  newLength;

                // move and resize junk
                nextChunkInfo->chunkPos    =  thisChunkInfo->chunkPos + pl_AlignULL (thisChunkInfo->chunkLength, p->headerInfo.alignment) + chunkHeaderLength;
                nextChunkInfo->chunkLength += chunkChange;
                
                if ((p->headerInfo.lengthSize == 4) && (nextChunkInfo->chunkLength > 0xffffffff))
                {
                    // need multiple junk chunks rather than one, unlikely to need to reserve this amount of free space
                    result = PlankResult_UnknownError;
                    goto exit;
                }
                
                if ((result = pl_File_SetPosition (&p->file, nextChunkInfo->chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFileWriter_WriteChunkID (p, &nextChunkInfo->chunkID)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFileWriter_WriteChunkLength (p, nextChunkInfo->chunkLength)) != PlankResult_OK) goto exit;
            }
        }
        else if ((chunkChange < p->headerInfo.alignment) && ((thisChunkInfo->chunkLength % p->headerInfo.alignment) != 0))
        {
            // just change the chunk length
            if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
            if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;

            thisChunkInfo->chunkLength = newLength;
        }
        else if (chunkChange < 0)
        {
            chunkChange = -chunkChange;
            
            if (chunkChange >= chunkHeaderLength)
            {
                // if it is getting smaller, shrink it and add junk at the end
                if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos - p->headerInfo.lengthSize)) != PlankResult_OK) goto exit;
                if ((result = pl_IffFileWriter_WriteChunkLength (p, newLength)) != PlankResult_OK) goto exit;
                
                if (chunkChange > 1)
                {
                    // need to add junk
                    newJunkChunkInfo.chunkID     = p->headerInfo.junkID;
                    newJunkChunkInfo.chunkPos    = thisChunkInfo->chunkPos + alignedNewLength + chunkHeaderLength;
                    newJunkChunkInfo.chunkLength = chunkChange + (thisChunkInfo->chunkLength % p->headerInfo.alignment) - chunkHeaderLength;
                    
                    if ((p->headerInfo.lengthSize == 4) && (newJunkChunkInfo.chunkLength > 0xffffffff))
                    {
                        // need multiple junk chunks rather than one, unlikely to need to reserve this amount of free space
                        result = PlankResult_UnknownError;
                        goto exit;
                    }
                    
                    if ((result = pl_File_SetPosition (&p->file, newJunkChunkInfo.chunkPos - chunkHeaderLength)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkID (p, &newJunkChunkInfo.chunkID)) != PlankResult_OK) goto exit;
                    if ((result = pl_IffFileWriter_WriteChunkLength (p, newJunkChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
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
    if ((result = pl_File_SetPosition (&p->file, thisChunkInfo->chunkPos)) != PlankResult_OK) goto exit;
    
    p->currentChunk = thisChunkInfo;
    
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
    numChunks        = pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos       = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    lastChunkPos     = 0;
    currentLastChunk = 0;
    
    for (i = 0; i < numChunks; ++i)
    {
        if (pl_IffFileWriter_IsNullID (p, &chunkInfos[i].chunkID))
            continue; // this was a deleted chunk
        
        if (chunkInfos[i].chunkPos > lastChunkPos)
        {
            lastChunkPos = chunkInfos[i].chunkPos;
            currentLastChunk = &chunkInfos[i];
        }
    }
    
    *lastChunkInfo = currentLastChunk;
    
exit:
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
    
    if ((result = pl_File_GetMode (&p->file, &mode)) != PlankResult_OK)     return result;
    if (!(mode & PLANKFILE_READ))                                           return PlankResult_FileReadError;
    if ((result = pl_Path_InitTemp (&tempPath)) != PlankResult_OK)          return result;
    if ((result = pl_IffFileWriter_Init (&tempWriter)) != PlankResult_OK)   goto earlyExit;
    
    pl_IffFileWriter_ChunkIDString (p, &p->headerInfo.mainID, chunkIDStr);
    pl_IffFileWriter_ChunkIDString (p, &p->headerInfo.formatID, otherIDStr);

    result = pl_IffFileWriter_OpenReplacing (&tempWriter,
                                             pl_Path_GetFullPath (&tempPath),
                                             pl_File_IsBigEndian (&p->file),
                                             chunkIDStr,
                                             otherIDStr,
                                             p->headerInfo.idType);
    
    if (result != PlankResult_OK) goto earlyExit;

    numChunks = pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    
    for (i = 0; i < numChunks; ++i)
    {
        if (pl_IffFileWriter_IsNullID (p, &chunkInfos[i].chunkID))
            continue; // this was a deleted chunk
            
        if (!pl_IffFileWriter_EqualIDs (p, &chunkInfos[i].chunkID, &updatedChunkInfo->chunkID))
        {            
            if ((result = pl_File_SetPosition (&p->file, chunkInfos[i].chunkPos)) != PlankResult_OK) goto exit;
            
            copyChunkLengthRemain = chunkInfos[i].chunkLength;
            
            while (copyChunkLengthRemain > 0)
            {
                bytesThisTime = (int)pl_MinLL (copyChunkLengthRemain, PLANKIFFFILEWRITER_COPYLENGTH);
                
                if ((result = pl_File_Read (&p->file, copyBuffer, bytesThisTime, &bytesRead)) != PlankResult_OK) goto exit;
                
                if (bytesThisTime != bytesRead)
                {
                    // something is wrong if these don't match here
                    result = PlankResult_FileReadError;
                    goto exit;
                }
                
                pl_IffFileWriter_ChunkIDString (p, &chunkInfos[i].chunkID, chunkIDStr);
                                    
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

            if ((result = pl_File_SetPosition (&p->file, chunkInfos[i].chunkPos)) != PlankResult_OK) goto exit;
            
            copyChunkLengthRemain = pl_MinLL (chunkInfos[i].chunkLength, updatedChunkInfo->chunkLength);

            while (copyChunkLengthRemain > 0)
            {
                bytesThisTime = pl_MinUI (copyChunkLengthRemain, PLANKIFFFILEWRITER_COPYLENGTH);
                
                if ((result = pl_File_Read (&p->file, copyBuffer, bytesThisTime, &bytesRead)) != PlankResult_OK) goto exit;

                if ((int)bytesThisTime != bytesRead)
                {
                    // something is wrong if these don't match here
                    result = PlankResult_FileReadError;
                    goto exit;
                }
                
                pl_IffFileWriter_ChunkIDString (p, &chunkInfos[i].chunkID, chunkIDStr);
                
                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter,
                                                           PLANKIFFFILE_CURRENTCHUNKPOSITION, chunkIDStr,
                                                           copyBuffer, bytesThisTime,
                                                           PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
                
                copyChunkLengthRemain -= bytesThisTime;
            }
            
            // pad with zero for the larger size, no need to round to even bytes as pl_IffFileWriter_WriteChunk does this for us
            if (updatedChunkInfo->chunkLength > chunkInfos[i].chunkLength)
            {
                bytesThisTime = updatedChunkInfo->chunkLength - chunkInfos[i].chunkLength;
                
                pl_MemoryZero (copyBuffer, bytesThisTime);
                
                pl_IffFileWriter_ChunkIDString (p, &chunkInfos[i].chunkID, chunkIDStr);

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
        if ((result = pl_File_Clear (&p->file)) != PlankResult_OK) goto exit;
    }
    else
    {
        if ((result = pl_File_ResetPosition (&p->file)) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_File_ResetPosition (pl_IffFileWriter_GetFile (&tempWriter))) != PlankResult_OK) goto exit;
    if ((result = pl_File_Copy (&p->file, pl_IffFileWriter_GetFile (&tempWriter), 0)) != PlankResult_OK) goto exit;
    
exit:
    pl_IffFileWriter_DeInit (&tempWriter);
    pl_FileErase (pl_Path_GetFullPath (&tempPath));
    
earlyExit:
    pl_Path_DeInit (&tempPath);

    return result;
}

