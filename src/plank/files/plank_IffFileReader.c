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

#include "../core/plank_StandardHeader.h"
#include "plank_IffFileReader.h"

PlankIffFileReaderRef pl_IffFileReader_CreateAndInit()
{
    PlankIffFileReaderRef p;
    p = pl_IffFileReader_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_IffFileReader_Init (p) != PlankResult_OK)
            pl_IffFileReader_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankIffFileReaderRef pl_IffFileReader_Create()
{
    PlankMemoryRef m;
    PlankIffFileReaderRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankIffFileReaderRef)pl_Memory_AllocateBytes (m, sizeof (PlankIffFileReader));
            
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankIffFileReader));
    
    return p;
}

PlankResult pl_IffFileReader_Init (PlankIffFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankIffFileReader));
        
    result = pl_File_Init (&p->file);
    
exit:
    return result;
}

PlankResult pl_IffFileReader_InitWithFile (PlankIffFileReaderRef p, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankIffFileReader));
    pl_MemoryCopy (&p->file, file, sizeof(PlankFile));
    
exit:
    return result;    
}

PlankResult pl_IffFileReader_DeInit (PlankIffFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_File_DeInit (&p->file)) != PlankResult_OK)
        goto exit;

exit:
    return result;    
}

PlankResult pl_IffFileReader_Destroy (PlankIffFileReaderRef p)
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
        
    if ((result = pl_IffFileReader_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;    
}

PlankFileRef pl_IffFileReader_GetFile (PlankIffFileReaderRef p)
{
    return &p->file;
}

PlankResult pl_IffFileReader_Open (PlankIffFileReaderRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
    PlankUI chunkLength;
    PlankB bigEndian = PLANK_FALSE; // little endian at least at first
        
    result = pl_File_OpenBinaryRead (&p->file, 
                                     filepath, 
                                     PLANK_FALSE,   // not writable
                                     bigEndian); 

    if (result != PlankResult_OK)
        goto exit;

    if ((result = pl_File_ReadFourCharCode (&p->file, &p->headerInfo.mainID)) != PlankResult_OK) goto exit;
    if ((result = pl_File_ReadUI (&p->file, &chunkLength)) != PlankResult_OK) goto exit;    
    if ((result = pl_File_ReadFourCharCode (&p->file, &p->headerInfo.formatID)) != PlankResult_OK) goto exit;
    
    p->headerInfo.mainLength = (PlankLL)chunkLength;
    p->headerInfo.mainEnd = p->headerInfo.mainLength + 8;
        
exit:
    return result;
}

PlankResult pl_IffFileReader_Close (PlankIffFileReaderRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_FileCloseFailed;
    
    return pl_File_Close (&p->file);
}

PlankResult pl_IffFileReader_GetMainID (PlankIffFileReaderRef p, PlankFourCharCode* result)
{
    *result = p->headerInfo.mainID;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetFormatID (PlankIffFileReaderRef p, PlankFourCharCode* result)
{
    *result = p->headerInfo.formatID;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetMainLength (PlankIffFileReaderRef p, PlankLL* result)
{
    *result = p->headerInfo.mainLength;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetMainEnd (PlankIffFileReaderRef p, PlankLL* result)
{
    *result = p->headerInfo.mainEnd;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_IsBigEndian (PlankIffFileReaderRef p, PlankB* result)
{
    *result = pl_File_IsBigEndian (&p->file);
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_SetEndian (PlankIffFileReaderRef p, const PlankB isBigEndian)
{
    PlankUI chunkLength;
    
    if (pl_File_IsBigEndian (&p->file) != (PlankB)(!!isBigEndian))
    {
        pl_File_SetEndian (&p->file, isBigEndian);
        
        chunkLength = (PlankUI)p->headerInfo.mainLength;
        pl_SwapEndianUI (&chunkLength);
        p->headerInfo.mainLength = (PlankLL)chunkLength;
        p->headerInfo.mainEnd = p->headerInfo.mainLength + 8;
    }
    
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_SeekChunk (PlankIffFileReaderRef p, const PlankFourCharCode chunkID, PlankUI* chunkLength, PlankLL* chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankLL readChunkEnd, pos = PLANKIFFFILE_FIRSTCHUNKPOSITION;
    PlankFourCharCode readChunkID;
    PlankUI readChunkLength;
    
    if ((result = pl_File_SetPosition (&p->file, pos)) != PlankResult_OK) goto exit;

    while ((pos < p->headerInfo.mainEnd) && (pl_File_IsEOF (&p->file) == PLANK_FALSE))
    {
        if ((result = pl_File_ReadFourCharCode (&p->file, &readChunkID)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUI (&p->file, &readChunkLength)) != PlankResult_OK) goto exit;
        if ((result = pl_File_GetPosition (&p->file, &pos)) != PlankResult_OK) goto exit;
        
        readChunkEnd = pos + readChunkLength + (readChunkLength & 1);
        
        if (chunkID == readChunkID)
        {
            if (chunkLength) 
                *chunkLength = readChunkLength;
            
            if (chunkDataPos)
                *chunkDataPos = pos;
            
            result = PlankResult_OK;
            goto exit;
        }
        
        pos = readChunkEnd;
        if ((result = pl_File_SetPosition (&p->file, pos)) != PlankResult_OK) goto exit;        
    }
    
    result = PlankResult_IffFileReaderChunkNotFound;
    
exit:
    return result;
}
