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
#include "plank_AudioFileMetaData.h"
#include "plank_AudioFileRegion.h"
#include "plank_AudioFileCuePoint.h"

/*
 
 Should support the LEVL WAV chunk somehow as BBC are moving towards supporting it
 
 */

typedef struct PlankAudioFileIffSpecificMetaData
{
	PlankFourCharCode chunkID;
    PlankUI length;
    char data[1];
} PlankAudioFileIffSpecificMetaData;

typedef struct PlankAudioFileFormatSpecificMetaData
{
	int format;
    char data[1]; // for Iff would be a PlankAudioFileIffSpecificMetaData
} PlankAudioFileFormatSpecificMetaData;


PlankAudioFileMetaDataRef pl_AudioFileMetaData_Create()
{
    PlankMemoryRef m;
    PlankAudioFileMetaDataRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankAudioFileMetaDataRef)pl_Memory_AllocateBytes (m, sizeof (PlankAudioFileMetaData));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankAudioFileMetaData));
    
    return p;
}

PlankResult pl_AudioFileMetaData_Destroy (PlankAudioFileMetaDataRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_AudioFileMetaData_DeInit (p)) != PlankResult_OK)
        goto exit;
    
    result = pl_Memory_Free (m, p);
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_Init (PlankAudioFileMetaDataRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankAudioFileMetaData));
    
    // for some settings use -1 to indicate not set, others 0 is OK
    // but 0 should be checked for in some case to avoid errors (e.g., sample duration)
    p->midiUnityNote    = -1;
    p->lowNote          = -1;
    p->highNote         = -1;
    p->lowVelocity      = -1;
    p->highVelocity     = -1;
    p->year             = -1;
    p->trackNum         = -1;
    p->trackTotal       = -1;
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_DeInit (PlankAudioFileMetaDataRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
        
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_AddFormatSpecific (PlankAudioFileMetaDataRef p, PlankDynamicArrayRef block)
{
    (void)p;
    (void)block;
    return PlankResult_UnknownError;
}

