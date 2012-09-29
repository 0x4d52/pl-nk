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


static PlankResult pl_AudioFileMetaDataFormatSpecificFree (PlankP ptr)
{
    return pl_DynamicArray_Destroy ((PlankDynamicArrayRef)ptr);
}

PlankAudioFileMetaDataRef pl_AudioFileMetaData_CreateAndInit()
{
    PlankAudioFileMetaDataRef p;
    p = pl_AudioFileMetaData_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_AudioFileMetaData_Init (p) != PlankResult_OK)
            pl_AudioFileMetaData_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

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
    
    // the formatSpecific linked list contains elements that are DynamicArrays
    pl_SimpleLinkedList_SetFreeElementDataFunction (&p->formatSpecific, pl_AudioFileMetaDataFormatSpecificFree);
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaDataDeInitCuePoints (PlankDynamicArrayRef p)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    PlankAudioFileCuePoint* cuePoints;
    
    numCuePoints = pl_DynamicArray_GetSize (p);
    cuePoints = (PlankAudioFileCuePoint*)pl_DynamicArray_GetArray (p);
    
    for (i = 0; i < numCuePoints; ++i)
    {
        if ((result = pl_AudioFileCuePoint_DeInit (&cuePoints[i])) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_DynamicArray_DeInit (p)) != PlankResult_OK) goto exit;

exit:
    return result;
}

static PlankResult pl_AudioFileMetaDataDeInitRegions (PlankDynamicArrayRef p)
{
    PlankResult result = PlankResult_OK;
    PlankL numRegions, i;
    PlankAudioFileRegion* regions;
    
    numRegions = pl_DynamicArray_GetSize (p);
    regions = (PlankAudioFileRegion*)pl_DynamicArray_GetArray (p);
    
    for (i = 0; i < numRegions; ++i)
    {
        if ((result = pl_AudioFileRegion_DeInit (&regions[i])) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_DynamicArray_DeInit (p)) != PlankResult_OK) goto exit;

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
        
    if ((result = pl_AudioFileMetaDataDeInitCuePoints (&p->cuePoints)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaDataDeInitRegions (&p->loopPoints)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaDataDeInitRegions (&p->regions)) != PlankResult_OK) goto exit;

    // this should free any extra data stored as dynamic arrays in the linked list
    if ((result = pl_SimpleLinkedList_Clear (&p->formatSpecific)) != PlankResult_OK) goto exit;

    pl_MemoryZero (p, sizeof (PlankAudioFileMetaData));
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_AddCuePoint (PlankAudioFileMetaDataRef p, PlankAudioFileCuePointRef cuePoint)
{
    PlankResult result = PlankResult_OK;    
    
    if (pl_DynamicArray_GetItemSize (&p->cuePoints) == 0)
    {
        if ((result = pl_DynamicArray_InitWithItemSize (&p->cuePoints, sizeof (PlankAudioFileCuePoint))) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_DynamicArray_AddItem (&p->cuePoints, cuePoint)) != PlankResult_OK) goto exit;
    
    pl_MemoryZero (cuePoint, sizeof (PlankAudioFileCuePoint));
    
exit:
    return result;
}

//PlankResult pl_AudioFileMetaData_AddCuePoint (PlankAudioFileMetaDataRef p, const PlankUI cueID, const PlankUI offset, const char* label, const int cuePointType)
//{
//    PlankResult result = PlankResult_OK;
//    PlankAudioFileCuePoint cuePoint;
//    
//    if ((result = pl_AudioFileCuePoint_Init (&cuePoint)) != PlankResult_OK) goto exit;
//    if ((result = pl_AudioFileCuePoint_SetID (&cuePoint, cueID)) != PlankResult_OK) goto exit;
//    if ((result = pl_AudioFileCuePoint_SetPosition (&cuePoint, offset)) != PlankResult_OK) goto exit;
//    if ((result = pl_AudioFileCuePoint_SetLabel (&cuePoint, label)) != PlankResult_OK) goto exit;
//    if ((result = pl_AudioFileCuePoint_SetType (&cuePoint, cuePointType)) != PlankResult_OK) goto exit;
//
//    if (pl_DynamicArray_GetItemSize (&p->cuePoints) == 0)
//    {
//        if ((result = pl_DynamicArray_InitWithItemSize (&p->cuePoints, sizeof (PlankAudioFileCuePoint))) != PlankResult_OK) goto exit;
//    }
//    
//    if ((result = pl_DynamicArray_AddItem (&p->cuePoints, &cuePoint)) != PlankResult_OK) goto exit;
//        
//exit:
//    return result;
//}

PlankResult pl_AudioFileMetaData_FindCuePointWithID (PlankAudioFileMetaDataRef p, const PlankUI cueID, PlankAudioFileCuePointRef* cuePoint, PlankL* index)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    PlankAudioFileCuePoint* cuePoints;
    
    numCuePoints = pl_DynamicArray_GetSize (&p->cuePoints);
    cuePoints = (PlankAudioFileCuePoint*)pl_DynamicArray_GetArray (&p->cuePoints);
    
    for (i = 0; i < numCuePoints; ++i)
    {
        if (pl_AudioFileCuePoint_GetID (&cuePoints[i]) == cueID)
        {
            *cuePoint = &cuePoints[i];
            goto exit;
        }
    }

    *cuePoint = PLANK_NULL;
    i = -1;
    
exit:
    
    if (index != PLANK_NULL)
        *index = i;
    
    return result;
}

PlankResult pl_AudioFileMetaData_RemoveCuePointWithID (PlankAudioFileMetaDataRef p, const PlankUI cueID, PlankAudioFileCuePointRef cuePoint, PlankB* success)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    PlankAudioFileCuePoint* cuePoints;
    
    numCuePoints = pl_DynamicArray_GetSize (&p->cuePoints);
    cuePoints = (PlankAudioFileCuePoint*)pl_DynamicArray_GetArray (&p->cuePoints);
    *success = PLANK_FALSE;
    
    for (i = 0; i < numCuePoints; ++i)
    {
        if (pl_AudioFileCuePoint_GetID (&cuePoints[i]) == cueID)
        {
            if ((result = pl_DynamicArray_GetItem (&p->cuePoints, i, cuePoint)) != PlankResult_OK) goto exit;
            if ((result = pl_DynamicArray_RemoveItem (&p->cuePoints, i)) != PlankResult_OK) goto exit;
            *success = PLANK_TRUE;
            goto exit;
        }
    }
        
exit:
    return result;    
}

PlankResult pl_AudioFileMetaData_AddFormatSpecificBlock (PlankAudioFileMetaDataRef p, PlankDynamicArrayRef block)
{
    PlankResult result;
    result = PlankResult_OK;
    
    if ((result = pl_SimpleLinkedList_Add (&p->formatSpecific, pl_SimpleLinkedListElement_CreateAndInitWthData (block))) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_AddRegion (PlankAudioFileMetaDataRef p, PlankAudioFileRegionRef region)
{
    PlankResult result = PlankResult_OK;
    
    if (pl_DynamicArray_GetItemSize (&p->regions) == 0)
    {
        if ((result = pl_DynamicArray_InitWithItemSize (&p->regions, sizeof (PlankAudioFileRegion))) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_DynamicArray_AddItem (&p->regions, region)) != PlankResult_OK) goto exit;
    
    pl_MemoryZero (region, sizeof (PlankAudioFileRegion));
    
exit:
    return result;
}



