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
#include "plank_IffFileWriter.h"
#include "plank_IffFileReader.h"


// private functions
PlankResult pl_IffFileWriter_WriteHeader (PlankIffFileWriterRef p);


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
    PlankMemoryRef m;
    PlankL numBytes;

    m = pl_MemoryGlobal();
    
    pl_MemoryZero (p, sizeof (PlankIffFileWriter));
    pl_File_Init (&p->file);
    
    p->allocatedChunks = 16;
    p->numChunks = 0;
    numBytes = sizeof (PlankIffFileWriterChunkInfo) * p->allocatedChunks;
    p->chunkInfo = (PlankIffFileWriterChunkInfo*) pl_Memory_AllocateBytes (m, numBytes);
    
    if (p->chunkInfo == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p->chunkInfo, numBytes);
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_DeInit (PlankIffFileWriterRef p)
{
	(void)p;
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
        
    if ((result = pl_IffFileWriter_Close (p)) != PlankResult_OK)
        goto exit;
    
    if ((result = pl_Memory_Free (m, p->chunkInfo)) != PlankResult_OK)
        goto exit;
    
    p->chunkInfo = PLANK_NULL;
    p->allocatedChunks = 0;
    p->numChunks = 0;
    
    if ((result = pl_File_DeInit (&p->file)) != PlankResult_OK)
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
    
    p->headerInfo.mainID = mainID;
    p->headerInfo.mainLength = 0;
    p->headerInfo.formatID = formatID;
    p->headerInfo.mainEnd = PLANKIFFFILE_FIRSTCHUNKPOSITION;
    
    if ((result = pl_IffFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_IffFileWriter_Close (PlankIffFileWriterRef p)
{
    PlankResult result = PlankResult_OK;

    if ((result = pl_IffFileWriter_WriteHeader (p)) != PlankResult_OK) goto exit;

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


