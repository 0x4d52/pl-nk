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
#include "plank_IffFileReader.h"

static inline PlankResult pl_IffFileReader_ReadChunkLength (PlankIffFileReaderRef p, PlankLL* length)
{
    PlankResult result = PlankResult_OK;
    PlankUI length32;
    
    if (p->headerInfo.lengthSize == 4)
    {
        if ((result = pl_File_ReadUI (&p->file, &length32)) != PlankResult_OK) goto exit;
        *length = length32;
    }
    else if (p->headerInfo.lengthSize == 8)
    {
        if ((result = pl_File_ReadLL (&p->file, length)) != PlankResult_OK) goto exit;
    }
    else
    {
        result = PlankResult_UnknownError;
    }
    
exit:
    return result;
}

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
    
    p->headerInfo.lengthSize         = 4;  // default to 32 bit, CAF is 64
    p->headerInfo.mainEndOffset      = 8;  // default 8 bytes
    p->headerInfo.initMainLength     = 4;
    
    result = pl_File_Init (&p->file);
    
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

    pl_MemoryZero (p, sizeof (PlankIffFileReader));

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

static PlankResult pl_IffFileReader_ParseMain (PlankIffFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    PlankUI chunkLength32;
    PlankUS vers, resv;
    
    if ((result = pl_File_ReadFourCharCode (&p->file, &p->headerInfo.mainID.fcc)) != PlankResult_OK) goto exit;
    
    if (p->headerInfo.mainID.fcc == pl_FourCharCode ("caff"))
    {
        p->headerInfo.formatID.fcc       = 0;
        p->headerInfo.lengthSize         = 8;
        p->headerInfo.mainEndOffset      = 8;
        p->headerInfo.mainLength         = -1; // can't know with CAF
        p->headerInfo.initMainLength     = 0;
        p->headerInfo.junkID.fcc         = pl_FourCharCode ("free");

        if ((result = pl_File_ReadUS (&p->file, &vers)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUS (&p->file, &resv)) != PlankResult_OK) goto exit;

        pl_SwapEndianUS (&vers); // while reading the header we're still LE
        
        if ((vers != 1) || (resv != 0))
        {
            result = PlankResult_UnknownError;
            goto exit;
        }        
    }
    else
    {
        if (p->headerInfo.mainID.fcc == pl_FourCharCode ("FORM"))
            p->headerInfo.junkID.fcc = pl_FourCharCode ("    "); // Iff junk ID is four spaces
        else if (p->headerInfo.mainID.fcc == pl_FourCharCode ("RIFF"))
            p->headerInfo.junkID.fcc = pl_FourCharCode ("JUNK");
        
        if ((result = pl_File_ReadUI (&p->file, &chunkLength32)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadFourCharCode (&p->file, &p->headerInfo.formatID.fcc)) != PlankResult_OK) goto exit;
        
        // could check for 0xffffffff length here for RF64
        
        p->headerInfo.mainLength = (PlankLL)chunkLength32;
    }
    
    
exit:
    return result;
}

PlankResult pl_IffFileReader_Open (PlankIffFileReaderRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
        
    result = pl_File_OpenBinaryRead (&p->file, 
                                     filepath, 
                                     PLANK_FALSE,   // not writable
                                     PLANK_FALSE);  // little endian at least at first

    if (result != PlankResult_OK)
        goto exit;

    result = pl_IffFileReader_ParseMain (p);
    
exit:
    return result;
}

PlankResult pl_IffFileReader_OpenWithFile (PlankIffFileReaderRef p, PlankFileRef file)
{
    PlankResult result = PlankResult_OK;
    int mode;
    
    if ((result = pl_File_GetMode (file, &mode)) != PlankResult_OK) goto exit;
    
    if (!(mode & PLANKFILE_READ))
    {
        result = PlankResult_FileReadError;
        goto exit;
    }
    
    if ((mode & PLANKFILE_BIGENDIAN))
    {
        result = PlankResult_FileReadError;
        goto exit;
    }
    
    pl_MemoryCopy (&p->file, file, sizeof (PlankFile));
    pl_MemoryZero (file, sizeof (PlankFile));

    result = pl_IffFileReader_ParseMain (p);
    
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
    *result = p->headerInfo.mainID.fcc;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetFormatID (PlankIffFileReaderRef p, PlankFourCharCode* result)
{
    *result = p->headerInfo.formatID.fcc;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetMainLength (PlankIffFileReaderRef p, PlankLL* result)
{
    *result = p->headerInfo.mainLength;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetMainEnd (PlankIffFileReaderRef p, PlankLL* result)
{
    *result = (p->headerInfo.mainLength < 0) ? -1 : p->headerInfo.mainLength + p->headerInfo.mainEndOffset;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_IsBigEndian (PlankIffFileReaderRef p, PlankB* result)
{
    *result = pl_File_IsBigEndian (&p->file);
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_SetEndian (PlankIffFileReaderRef p, const PlankB isBigEndian)
{
    PlankUI chunkLength32;
    
    if (pl_File_IsBigEndian (&p->file) != (PlankB)(!!isBigEndian))
    {
        pl_File_SetEndian (&p->file, isBigEndian);
        
        if (p->headerInfo.lengthSize == 4)
        {
            chunkLength32 = (PlankUI)p->headerInfo.mainLength;
            pl_SwapEndianUI (&chunkLength32);
            p->headerInfo.mainLength = (PlankLL)chunkLength32;
        }
        else if (p->headerInfo.lengthSize == 8)
        {
            // nothing
        }
        else return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_SeekChunk (PlankIffFileReaderRef p, const PlankLL startPosition, const PlankFourCharCode chunkID, PlankLL* chunkLength, PlankLL* chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankLL readChunkEnd, mainEnd, pos;
    PlankFourCharCode readChunkID;
    PlankLL readChunkLength;

    switch (startPosition)
    {
        case 0:
            pos = p->headerInfo.mainEndOffset + p->headerInfo.initMainLength;
            break;
        case -1:
            if ((result = pl_File_GetPosition (&p->file, &pos)) != PlankResult_OK) goto exit;
            break;
        default:
            pos = startPosition;
    }
    
    if ((result = pl_IffFileReader_GetMainEnd (p, &mainEnd)) != PlankResult_OK) goto exit;
    if ((result = pl_File_SetPosition (&p->file, pos)) != PlankResult_OK) goto exit;
    
    while (((mainEnd < 0) || (pos < mainEnd)) &&
           (pl_File_IsEOF (&p->file) == PLANK_FALSE))
    {
        if ((result = pl_IffFileReader_ParseChunkHeader (p, &readChunkID, &readChunkLength, &readChunkEnd, &pos)) != PlankResult_OK) goto exit;

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


PlankResult pl_IffFileReader_ParseChunkHeader (PlankIffFileReaderRef p, PlankFourCharCode* chunkID, PlankLL* chunkLength, PlankLL* chunkEnd, PlankLL* posOut)
{
    PlankResult result;
    PlankFourCharCode cid;
    PlankLL pos, len;
    
    result = PlankResult_OK;
    
    if ((result = pl_File_ReadFourCharCode (&p->file, &cid)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFileReader_ReadChunkLength (p, &len)) != PlankResult_OK) goto exit;
    if ((result = pl_File_GetPosition (&p->file, &pos)) != PlankResult_OK) goto exit;
    
    if (chunkID != PLANK_NULL)
        *chunkID = cid;
    
    if (chunkLength != PLANK_NULL)
        *chunkLength = len;

    if (chunkEnd)
        *chunkEnd = pos + len + (len & 1);
        
    if (posOut)
        *posOut = pos;
    
exit:    
    return result;
}



