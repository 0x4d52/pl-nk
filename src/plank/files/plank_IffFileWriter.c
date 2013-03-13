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


// private functions
PlankResult pl_IffFileWriter_WriteHeader (PlankIffFileWriterRef p);
PlankResult pl_IffFileWriter_FindLastChunk (PlankIffFileWriterRef p, PlankIffFileWriterChunkInfo** lastChunkInfo);


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
    p->headerInfo.mainEnd       = PLANKIFFFILE_FIRSTCHUNKPOSITION;
    
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

PlankResult pl_IffFileWriter_SeekChunk (PlankIffFileWriterRef p, const PlankFourCharCode chunkID, PlankUI* chunkLength, PlankLL* chunkDataPos, PlankB* isLastChunk)
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
    
    if (chunkLength)
        *chunkLength = len;
    
    if (chunkDataPos)
        *chunkDataPos = pos;
    
    if (isLastChunk)
        *isLastChunk = (i == (numChunks - 1)) ? PLANK_TRUE : PLANK_FALSE;
    
exit:
    return result;
}

/*
 to write:
 - seek chunk and determine if if is the last chunk or not
 - if it doesn't exist, find the last chunk and seek to the end and add it
 - ensure seeked to same place as if it already existed and remember whether it's the last chunk
 - write data to the chunk enabling the ability to add more data than is currently in the chunk if this is the last chunk (otherwise fail for now but add a fix for this later)
 - must remember not to allow more data if this is not the last chunk and not increase the main length !!
 */

PlankResult pl_IffFileWriter_WriteChunk (PlankIffFileWriterRef p, const PlankFourCharCode chunkID, const void* data, const PlankUI dataLength)
{
    PlankIffFileWriterChunkInfo* lastChunkInfo;
    PlankIffFileWriterChunkInfo chunkInfo;
    PlankResult result;
    PlankLL chunkPos;
    PlankUI originalChunkLength, dataLengthPadded;
    PlankB isLastChunk;
    PlankB updateMain;
    PlankB isNewChunk;
    
    result = PlankResult_OK;
    updateMain = PLANK_FALSE;
    isNewChunk = PLANK_FALSE;
    
    if ((result = pl_IffFileWriter_SeekChunk (p, chunkID, &originalChunkLength, &chunkPos, &isLastChunk)) != PlankResult_OK) goto exit;

    if (chunkPos == 0)
    {
        // chunk doesn't exist...
        
        originalChunkLength = 0;
        
        pl_IffFileWriter_FindLastChunk (p, &lastChunkInfo);
        
        if (lastChunkInfo)
        {
            // we have at least one subchunk so seek to its end...
            chunkPos = lastChunkInfo->chunkPos + lastChunkInfo->chunkLength + (lastChunkInfo->chunkLength & 1);
        }
        else
        {
            // this is the first chunk
            chunkPos = PLANKIFFFILE_FIRSTCHUNKPOSITION;
        }
        
        isLastChunk = PLANK_TRUE;
        updateMain = PLANK_TRUE;
        isNewChunk = PLANK_TRUE;
    }
    
    if (dataLength > originalChunkLength)
    {
        if (!isLastChunk)
        {
            result = PlankResult_FileWriteError;
            goto exit;
        }
        else
        {
            updateMain = PLANK_TRUE;
        }
    }
    
    if ((result = pl_File_SetPosition (&p->file, chunkPos)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteFourCharCode (&p->file, chunkID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_WriteUI (&p->file, dataLength)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Write (&p->file, data, (int)dataLength)) != PlankResult_OK) goto exit;
    
    if (dataLength & 1)
    {
        if ((result = pl_File_WriteUC (&p->file, 0)) != PlankResult_OK) goto exit;
        
        dataLengthPadded = dataLength + 1;        
    }
    else
    {
        dataLengthPadded = dataLength;
    }
    
    if (updateMain)
    {
        p->headerInfo.mainLength += dataLengthPadded - originalChunkLength; // take off original length which will be zero
        
        if (isNewChunk)
            p->headerInfo.mainLength += 8;
    }
    
    // add to the array.
    chunkInfo.chunkID       = chunkID;
    chunkInfo.chunkLength   = dataLength;   // not rounded up!
    chunkInfo.chunkPos      = chunkPos + 8; // store the data pos

    if ((result = pl_DynamicArray_AddItem (&p->chunkInfos, &chunkInfo)) != PlankResult_OK) goto exit;

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

