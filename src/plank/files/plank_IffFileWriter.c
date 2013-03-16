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
PlankResult pl_IffFileWriter_WriteHeader (PlankIffFileWriterRef p);
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

PlankResult pl_IffFileWriter_OpenReplacing (PlankIffFileWriterRef p, 
                                            const char* filepath, 
                                            const PlankB bigEndian, 
                                            const PlankFourCharCode mainID, 
                                            const PlankFourCharCode formatID)
{
    PlankResult result = PlankResult_OK;
    
    result = pl_File_OpenBinaryWrite (&p->file, 
                                      filepath, 
                                      PLANK_TRUE, // and read
                                      PLANK_TRUE, // replace existing 
                                      bigEndian);
    
    if (result != PlankResult_OK)
        goto exit;
    
    p->headerInfo.mainID        = mainID;
    p->headerInfo.mainLength    = PLANKIFFFILE_MINIMUMMAINLENGTH;
    p->headerInfo.formatID      = formatID;
//    p->headerInfo.mainEnd       = PLANKIFFFILE_FIRSTCHUNKPOSITION;
    
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

PlankResult pl_IffFileWriter_SeekChunk (PlankIffFileWriterRef p, const PlankFourCharCode chunkID, PlankIffFileWriterChunkInfoRef* chunkInfo, PlankB* isLastChunk)
{
    PlankLL pos;
    PlankIffFileWriterChunkInfo* chunkInfos;
    PlankIffFileWriterChunkInfo* currentChunk;
    PlankResult result;
    int numChunks, i;
    PlankUI len;
    
    result = PlankResult_OK;
    numChunks = pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    pos = 0;
    len = 0;
    currentChunk = 0;
    
    for (i = 0; i < numChunks; ++i)
    {
        if (chunkInfos[i].chunkID == chunkID)
        {
            currentChunk = &chunkInfos[i];
            
            len = currentChunk->chunkLength;
            pos = currentChunk->chunkPos;
            
            if ((result = pl_File_SetPosition (&p->file, pos)) != PlankResult_OK) goto exit;
            
            p->currentChunk = currentChunk;
            
            break;
        }
    }
    
    if (chunkInfo)
        *chunkInfo = currentChunk;
            
    if (isLastChunk)
        *isLastChunk = (i == (numChunks - 1)) ? PLANK_TRUE : PLANK_FALSE;
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_WriteChunk (PlankIffFileWriterRef p, const PlankFourCharCode chunkID, const void* data, const PlankUI dataLength, const PlankIffFileWriterMode mode)
{
    PlankIffFileWriterChunkInfo* lastChunkInfo;
    PlankIffFileWriterChunkInfo newChunkInfo;
    PlankIffFileWriterChunkInfo* origChunkInfo;
    PlankResult result;
    PlankLL chunkDataPos, chunkStartPos, chunkLengthChange, mainLength;
    PlankUI originalChunkLength;
    PlankB isLastChunk, isNewChunk;
    
    result = PlankResult_OK;
    isNewChunk = PLANK_FALSE;
    
    if ((result = pl_IffFileWriter_SeekChunk (p, chunkID, &origChunkInfo, &isLastChunk)) != PlankResult_OK) goto exit;

    if (origChunkInfo == 0)
    {
        // chunk doesn't exist...
        
        originalChunkLength = 0;
        
        pl_IffFileWriter_FindLastChunk (p, &lastChunkInfo);
        
        if (lastChunkInfo)
        {
            // we have at least one subchunk so seek to its end...
            chunkStartPos = lastChunkInfo->chunkPos + lastChunkInfo->chunkLength + (lastChunkInfo->chunkLength & 1);
        }
        else
        {
            // this is the first chunk
            chunkStartPos = PLANKIFFFILE_FIRSTCHUNKPOSITION;
        }
        
        chunkDataPos = chunkStartPos + 8;
        isLastChunk = PLANK_TRUE;
        isNewChunk = PLANK_TRUE;
    }
    else
    {
        originalChunkLength = origChunkInfo->chunkLength;
        chunkDataPos = origChunkInfo->chunkPos;
        chunkStartPos = chunkDataPos - 8;
    }
    
    newChunkInfo.chunkID       = chunkID;
    newChunkInfo.chunkPos      = chunkDataPos; // store the data pos

    switch (mode)
    {
        case PLANKIFFFILEWRITER_MODEAPPEND:         newChunkInfo.chunkLength = originalChunkLength + dataLength;           break;
        case PLANKIFFFILEWRITER_MODEREPLACEGROW:    newChunkInfo.chunkLength = pl_MaxUI (dataLength, originalChunkLength); break;
        case PLANKIFFFILEWRITER_MODEREPLACESHRINK:  newChunkInfo.chunkLength = pl_MinUI (dataLength, originalChunkLength); break;
        default:
            result = PlankResult_FileModeInvalid;
            goto exit;
    }
    
    chunkLengthChange = newChunkInfo.chunkLength - originalChunkLength;
    
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
    if ((result = pl_File_WriteFourCharCode (&p->file, chunkID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI (&p->file, newChunkInfo.chunkLength)) != PlankResult_OK) goto exit;
    
    if (mode == PLANKIFFFILEWRITER_MODEAPPEND)
    {
        if ((result = pl_File_SetPosition (&p->file, newChunkInfo.chunkPos + originalChunkLength)) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_File_Write (&p->file, data, (int)dataLength)) != PlankResult_OK) goto exit;
    
    if (newChunkInfo.chunkLength & 1)
    {
        if ((result = pl_File_WriteUC (&p->file, 0)) != PlankResult_OK) goto exit;
    }
    
    mainLength = p->headerInfo.mainLength + chunkLengthChange + (newChunkInfo.chunkLength & 1);
    
    if (isNewChunk)
        mainLength += 8;

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
    PlankUI length = (PlankUI)p->headerInfo.mainLength; // should rewrite to cope with RF64
    
    if ((result = pl_File_SetPosition (&p->file, 0)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteFourCharCode (&p->file, p->headerInfo.mainID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI (&p->file, length)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteFourCharCode (&p->file, p->headerInfo.formatID)) != PlankResult_OK) goto exit;

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
    
    result = PlankResult_OK;
    numChunks = pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    lastChunkPos = 0;
    currentLastChunk = 0;
    
    for (i = 0; i < numChunks; ++i)
    {
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
    int mode, numChunks, i, bytesRead;
    PlankUI copyChunkLengthRemain, bytesThisTime;
    PlankB eraseOriginalFile;
    
    eraseOriginalFile = PLANK_FALSE;
    
    if ((result = pl_File_GetMode (&p->file, &mode)) != PlankResult_OK)     return result;
    if (!(mode & PLANKFILE_READ))                                           return PlankResult_FileReadError;
    if ((result = pl_Path_InitTemp (&tempPath)) != PlankResult_OK)          return result;
    if ((result = pl_IffFileWriter_Init (&tempWriter)) != PlankResult_OK)   goto earlyExit;
    
    result = pl_IffFileWriter_OpenReplacing (&tempWriter,
                                             pl_Path_GetFullPath (&tempPath),
                                             pl_File_IsBigEndian (&p->file),
                                             p->headerInfo.mainID,
                                             p->headerInfo.formatID);
    
    if (result != PlankResult_OK) goto earlyExit;

    numChunks = pl_DynamicArray_GetSize (&p->chunkInfos);
    chunkInfos = (PlankIffFileWriterChunkInfo*)pl_DynamicArray_GetArray (&p->chunkInfos);
    
    for (i = 0; i < numChunks; ++i)
    {
        if (chunkInfos[i].chunkID != updatedChunkInfo->chunkID)
        {            
            if ((result = pl_File_SetPosition (&p->file, chunkInfos[i].chunkPos)) != PlankResult_OK) goto exit;
            
            copyChunkLengthRemain = chunkInfos[i].chunkLength;
            
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
                
                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter, chunkInfos[i].chunkID, copyBuffer, bytesThisTime, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
                
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
            
            copyChunkLengthRemain = pl_MinUI (chunkInfos[i].chunkLength, updatedChunkInfo->chunkLength);

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
                
                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter, chunkInfos[i].chunkID, copyBuffer, bytesThisTime, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
                
                copyChunkLengthRemain -= bytesThisTime;
            }
            
            // pad with zero for the larger size, no need to round to even bytes as pl_IffFileWriter_WriteChunk does this for us
            if (updatedChunkInfo->chunkLength > chunkInfos[i].chunkLength)
            {
                bytesThisTime = updatedChunkInfo->chunkLength - chunkInfos[i].chunkLength;
                
                pl_MemoryZero (copyBuffer, bytesThisTime);
                
                if ((result = pl_IffFileWriter_WriteChunk (&tempWriter, chunkInfos[i].chunkID, copyBuffer, bytesThisTime, PLANKIFFFILEWRITER_MODEAPPEND)) != PlankResult_OK) goto exit;
            }
        }
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

