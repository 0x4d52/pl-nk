/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

//static PLANK_INLINE_LOW PlankResult pl_IffFileReader_ReadChunkLength (PlankIffFileReaderRef p, PlankLL* length)
//{
//    PlankResult result = PlankResult_OK;
//    PlankUI length32;
//    
//    if (p->common.headerInfo.lengthSize == 4)
//    {
//        if ((result = pl_File_ReadUI ((PlankFileRef)p, &length32)) != PlankResult_OK) goto exit;
//        *length = length32;
//    }
//    else if (p->common.headerInfo.lengthSize == 8)
//    {
//        if ((result = pl_File_ReadLL ((PlankFileRef)p, length)) != PlankResult_OK) goto exit;
//    }
//    else
//    {
//        result = PlankResult_UnknownError;
//    }
//    
//exit:
//    return result;
//}

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
    
    p->common.headerInfo.idType             = PLANKIFFFILE_ID_FCC;
    p->common.headerInfo.lengthSize         = 4;   // default to 32 bit, CAF is 64
    p->common.headerInfo.mainHeaderEnd      = 12;  // default 12 bytes
    p->common.headerInfo.initMainLength     = 4;
    p->common.headerInfo.alignment          = 2;
    p->common.headerInfo.headerLength       = 4 + 4;
    p->common.headerInfo.lengthSize         = 4;
    
    result = pl_File_Init ((PlankFileRef)p);
    
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
    
    if ((result = pl_File_DeInit ((PlankFileRef)p)) != PlankResult_OK)
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
    return (PlankFileRef)p;
}

static PlankResult pl_IffFileReader_ParseMain (PlankIffFileReaderRef p)
{
    PlankResult result = PlankResult_OK;
    PlankUI chunkLength32;
    PlankUS vers, resv;
    
    if ((result = pl_File_ReadFourCharCode ((PlankFileRef)p, &p->common.headerInfo.mainID.fcc)) != PlankResult_OK) goto exit;
    
    if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("OggS"))
    {
        goto exit;
    }
    else if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("caff"))
    {
        p->common.headerInfo.formatID.fcc       = 0;
        p->common.headerInfo.lengthSize         = 8;
        p->common.headerInfo.mainHeaderEnd      = 8;
        p->common.headerInfo.mainLength         = -1; // can't know with CAF
        p->common.headerInfo.initMainLength     = 0; // -1 ?
        p->common.headerInfo.junkID.fcc         = pl_FourCharCode ("free");

        if ((result = pl_File_ReadUS ((PlankFileRef)p, &vers)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadUS ((PlankFileRef)p, &resv)) != PlankResult_OK) goto exit;

        pl_SwapEndianUS (&vers); // while reading the header we're still LE
        
        if ((vers != 1) || (resv != 0))
        {
            result = PlankResult_UnknownError;
            goto exit;
        }        
    }
    else if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("riff"))
    {
        // this should be the start of the Wave64 RIFF GUID read - the remaining 96 bytes
        result = pl_File_Read ((PlankFileRef)p,
                               ((PlankUC*)(&p->common.headerInfo.mainID.guid)) + sizeof (PlankFourCharCode),
                               sizeof (PlankGUID) - sizeof (PlankFourCharCode),
                               0);
        
        if (result != PlankResult_OK)
            goto exit;
        
        if (!pl_GUID_EqualWithString (&p->common.headerInfo.mainID.guid, PLANKIFFFILE_W64_RIFF_ID))
        {
            result = PlankResult_AudioFileInavlidType;
            goto exit;
        }
        
        if ((result = pl_File_ReadLL ((PlankFileRef)p, &p->common.headerInfo.mainLength)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadGUID ((PlankFileRef)p, &p->common.headerInfo.formatID.guid)) != PlankResult_OK) goto exit;

        pl_GUID_InitChunkString (&p->common.headerInfo.junkID.guid, PLANKIFFFILE_W64_JUNK_ID);
        p->common.headerInfo.initMainLength     = 16;
        p->common.headerInfo.lengthSize         = 8;
        p->common.headerInfo.mainHeaderEnd      = 16 + 8 + 16;
        p->common.headerInfo.headerLength       = 8 + 16;
        p->common.headerInfo.alignment          = 8;
        p->common.headerInfo.idType             = PLANKIFFFILE_ID_GUID;
    }
    else
    {
        if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("FORM"))
            p->common.headerInfo.junkID.fcc = pl_FourCharCode ("    "); // Iff junk ID is four spaces http://www.martinreddy.net/gfx/2d/IFF.txt
        else if (p->common.headerInfo.mainID.fcc == pl_FourCharCode ("RIFF"))
            p->common.headerInfo.junkID.fcc = pl_FourCharCode ("JUNK");
        
        if ((result = pl_File_ReadUI ((PlankFileRef)p, &chunkLength32)) != PlankResult_OK) goto exit;
        if ((result = pl_File_ReadFourCharCode ((PlankFileRef)p, &p->common.headerInfo.formatID.fcc)) != PlankResult_OK) goto exit;
        
        // could check for 0xffffffff length here for RF64
        
        p->common.headerInfo.mainLength = (PlankLL)chunkLength32;
    }
    
exit:
    return result;
}

PlankResult pl_IffFileReader_Open (PlankIffFileReaderRef p, const char* filepath)
{
    PlankResult result = PlankResult_OK;
        
    result = pl_File_OpenBinaryRead ((PlankFileRef)p, 
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
    
    pl_MemoryCopy ((PlankFileRef)p, file, sizeof (PlankFile));
    pl_MemoryZero (file, sizeof (PlankFile));

    result = pl_IffFileReader_ParseMain (p);
    
exit:
    return result;
}

PlankResult pl_IffFileReader_Close (PlankIffFileReaderRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_FileCloseFailed;
    
    return pl_File_Close ((PlankFileRef)p);
}

PlankResult pl_IffFileReader_GetMainID (PlankIffFileReaderRef p, PlankIffID* result)
{
    *result = p->common.headerInfo.mainID;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetFormatID (PlankIffFileReaderRef p, PlankIffID* result)
{
    *result = p->common.headerInfo.formatID;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetMainLength (PlankIffFileReaderRef p, PlankLL* result)
{
    *result = p->common.headerInfo.mainLength;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_GetMainEnd (PlankIffFileReaderRef p, PlankLL* result)
{
    *result = (p->common.headerInfo.mainLength < 0) ? -1 : p->common.headerInfo.mainLength + p->common.headerInfo.headerLength;
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_IsBigEndian (PlankIffFileReaderRef p, PlankB* result)
{
    *result = pl_File_IsBigEndian ((PlankFileRef)p);
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_SetEndian (PlankIffFileReaderRef p, const PlankB isBigEndian)
{
    PlankUI chunkLength32;
    
    if (pl_File_IsBigEndian ((PlankFileRef)p) != (PlankB)(!!isBigEndian))
    {
        pl_File_SetEndian ((PlankFileRef)p, isBigEndian);
        
        if (p->common.headerInfo.lengthSize == 4)
        {
            chunkLength32 = (PlankUI)p->common.headerInfo.mainLength;
            pl_SwapEndianUI (&chunkLength32);
            p->common.headerInfo.mainLength = (PlankLL)chunkLength32;
        }
        else if (p->common.headerInfo.lengthSize == 8)
        {
            // nothing
        }
        else return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

PlankResult pl_IffFileReader_SeekChunk (PlankIffFileReaderRef p, const PlankLL startPosition, const char* chunkIDstr, PlankLL* chunkLength, PlankLL* chunkDataPos)
{
    PlankResult result = PlankResult_OK;
    PlankLL readChunkEnd, mainEnd, pos;
    PlankLL readChunkLength;
    PlankIffID chunkID, readChunkID;

    if ((result = pl_IffFile_InitID ((PlankIffFileRef)p, chunkIDstr, &chunkID)) != PlankResult_OK) goto exit;
    
    switch (startPosition)
    {
        case 0:
            pos = p->common.headerInfo.mainHeaderEnd;
            break;
        case -1:
            if ((result = pl_File_GetPosition ((PlankFileRef)p, &pos)) != PlankResult_OK) goto exit;
            break;
        default:
            pos = startPosition;
    }
    
    if ((result = pl_IffFileReader_GetMainEnd (p, &mainEnd)) != PlankResult_OK) goto exit;
    if ((result = pl_File_SetPosition ((PlankFileRef)p, pos)) != PlankResult_OK) goto exit;
    
    while (((mainEnd < 0) || (pos < mainEnd)) &&
           (pl_File_IsEOF ((PlankFileRef)p) == PLANK_FALSE))
    {
        if ((result = pl_IffFileReader_ParseChunkHeader (p, 0, &readChunkID, &readChunkLength, &readChunkEnd, &pos)) != PlankResult_OK) goto exit;
        
        if (pl_IffFile_EqualIDs ((PlankIffFileRef)p, &chunkID, &readChunkID))
        {
            if (chunkLength)
                *chunkLength = readChunkLength;
            
            if (chunkDataPos)
                *chunkDataPos = pos;
            
            result = PlankResult_OK;
            goto exit;
        }
        
        pos = readChunkEnd;
        if ((result = pl_File_SetPosition ((PlankFileRef)p, pos)) != PlankResult_OK) goto exit;
    }
    
    result = PlankResult_IffFileReaderChunkNotFound;
    
exit:
    return result == PlankResult_FileEOF ? PlankResult_IffFileReaderChunkNotFound : result;
}


PlankResult pl_IffFileReader_ParseChunkHeader (PlankIffFileReaderRef p, char* chunkIDstr, PlankIffID* chunkID, PlankLL* chunkLength, PlankLL* chunkEnd, PlankLL* posOut)
{
    PlankResult result;
    PlankIffID cid;
    PlankLL pos, len;
    
    result = PlankResult_OK;
    
    if ((result = pl_File_GetPosition        ((PlankFileRef)p, &pos)) != PlankResult_OK) goto exit;
    if ((result = pl_IffFile_ReadChunkID     ((PlankIffFileRef)p, &cid)) != PlankResult_OK) goto exit;
    
    // TODO should check here for a valid ChunkID and step slowly until one is found
    // the cid.fcc field should ALWAYS be four alphanumeric chars or a space 
    
    if ((result = pl_IffFile_ReadChunkLength ((PlankIffFileRef)p, &len)) != PlankResult_OK) goto exit;
    if ((result = pl_File_GetPosition        ((PlankFileRef)p, &pos)) != PlankResult_OK) goto exit;
    
    if (chunkIDstr != PLANK_NULL)
        pl_IffFile_ChunkIDString ((PlankIffFileRef)p, &cid, chunkIDstr);
    
    if (chunkID != PLANK_NULL)
        *chunkID = cid;
    
    if (chunkLength != PLANK_NULL)
        *chunkLength = len;

    if (chunkEnd != PLANK_NULL)
        *chunkEnd = pos + pl_AlignULL (len, p->common.headerInfo.alignment);
        
    if (posOut != PLANK_NULL)
        *posOut = pos;
    
exit:    
    return result;
}



