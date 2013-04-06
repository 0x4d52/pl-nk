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

//------------------------------------------------------------------------------

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
    p->baseNote         = -1;
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
    
    if ((result = pl_AudioFileMetaData_ClearDescriptionComments (p)) != PlankResult_OK) goto exit;
    
    if ((result = pl_DynamicArray_DeInit (&p->originatorArtist)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originatorRef)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originationDate)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originationTime)) != PlankResult_OK) goto exit;

    if ((result = pl_DynamicArray_DeInit (&p->title)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->album)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->genre)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->lyrics)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->vendor)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->isrc)) != PlankResult_OK) goto exit;

    if ((result = pl_DynamicArray_DeInit (&p->art)) != PlankResult_OK) goto exit;
    
    if ((result = pl_DynamicArray_DeInit (&p->codingHistory)) != PlankResult_OK) goto exit;

    if ((result = pl_AudioFileMetaDataDeInitCuePoints (&p->cuePoints)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaDataDeInitRegions (&p->loopPoints)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaDataDeInitRegions (&p->regions)) != PlankResult_OK) goto exit;

    // this should free any extra data stored as dynamic arrays in the linked list
    if ((result = pl_SimpleLinkedList_Clear (&p->formatSpecific)) != PlankResult_OK) goto exit;

    pl_MemoryZero (p, sizeof (PlankAudioFileMetaData));
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_SetEditCount (PlankAudioFileMetaDataRef p, const PlankUI count)
{
    if (count < p->editCount)
        return PlankResult_UnknownError;
    
    p->editCount = count;
    
    return PlankResult_OK;
}

PlankUI pl_AudioFileMetaData_GetEditCount (PlankAudioFileMetaDataRef p)
{
    return p->editCount;
}

PlankResult pl_AudioFileMetaData_SetInstrumentData (PlankAudioFileMetaDataRef p,
                                                    PlankI baseNote,
                                                    PlankI detune,
                                                    PlankI gain,
                                                    PlankI lowNote,
                                                    PlankI highNote,
                                                    PlankI lowVelocity,
                                                    PlankI highVelocity)
{
    PlankResult result = PlankResult_OK;
    
    p->baseNote     = baseNote;
    p->detune       = detune;
    p->gain         = gain;
    p->lowNote      = lowNote;
    p->highNote     = highNote;
    p->lowVelocity  = lowVelocity;
    p->highVelocity = highVelocity;
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_GetInstrumentData (PlankAudioFileMetaDataRef p,
                                                    PlankI* baseNote,
                                                    PlankI* detune,
                                                    PlankI* gain,
                                                    PlankI* lowNote,
                                                    PlankI* highNote,
                                                    PlankI* lowVelocity,
                                                    PlankI* highVelocity)
{
    PlankResult result = PlankResult_OK;
    
    if (baseNote     != PLANK_NULL)     *baseNote = p->baseNote;
    if (detune       != PLANK_NULL)       *detune = p->detune;
    if (gain         != PLANK_NULL)         *gain = p->gain;
    if (lowNote      != PLANK_NULL)      *lowNote = p->lowNote;
    if (highNote     != PLANK_NULL)     *highNote = p->highNote;
    if (lowVelocity  != PLANK_NULL)  *lowVelocity = p->lowVelocity;
    if (highVelocity != PLANK_NULL) *highVelocity = p->highVelocity;
    
exit:
    return result;    
}

PlankResult pl_AudioFileMetaData_SetSamplerData (PlankAudioFileMetaDataRef p,
                                                 PlankUI manufacturer,
                                                 PlankUI product,
                                                 PlankUI samplePeriod,
                                                 PlankUI smpteFormat,
                                                 PlankUI smpteOffset)
{
    PlankResult result = PlankResult_OK;
    
    p->manufacturer = manufacturer;
    p->product      = product;
    p->samplePeriod = samplePeriod;
    p->smpteFormat  = smpteFormat;
    p->smpteOffset  = smpteOffset;
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_GetSamplerData (PlankAudioFileMetaDataRef p,
                                                 PlankUI* manufacturer,
                                                 PlankUI* product,
                                                 PlankUI* samplePeriod,
                                                 PlankUI* smpteFormat,
                                                 PlankUI* smpteOffset)
{
    PlankResult result = PlankResult_OK;
    
    if (manufacturer != PLANK_NULL) *manufacturer = p->manufacturer;
    if (product      != PLANK_NULL)      *product = p->product;
    if (samplePeriod != PLANK_NULL) *samplePeriod = p->samplePeriod;
    if (smpteFormat  != PLANK_NULL)  *smpteFormat = p->smpteFormat;
    if (smpteOffset  != PLANK_NULL)  *smpteOffset = p->smpteOffset;

exit:
    return result;
}

PlankResult pl_AudioFileMetaData_ClearDescriptionComments (PlankAudioFileMetaDataRef p)
{
    PlankResult result = PlankResult_OK;
    int i, numComments;
    PlankDynamicArray* comments;
        
    numComments = pl_AudioFileMetaData_GetDescriptionCommentsCount (p);
    comments = (PlankDynamicArray*)pl_DynamicArray_GetArray (&p->descriptionComments);
    
    for (i = 0; i < numComments; ++i)
    {
        if ((result = pl_DynamicArray_DeInit (&comments[i])) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_DynamicArray_DeInit (&p->descriptionComments)) != PlankResult_OK) goto exit;
        
exit:
    return result;
    
}

PlankResult pl_AudioFileMetaData_AddDescriptionComment (PlankAudioFileMetaDataRef p, const char* text)
{
    PlankResult result = PlankResult_OK;
    PlankDynamicArray comment;
    
    if (pl_DynamicArray_GetItemSize (&p->descriptionComments) == 0)
        pl_DynamicArray_InitWithItemSize (&p->descriptionComments, sizeof (PlankDynamicArray));
    
    if ((result = pl_DynamicArray_Init (&comment)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_SetAsText (&comment, text)) != PlankResult_OK) goto exit;

    if ((result = pl_DynamicArray_AddItem (&p->descriptionComments, &comment)) != PlankResult_OK) goto exit;

exit:
    return result;
}

int pl_AudioFileMetaData_GetDescriptionCommentsCount (PlankAudioFileMetaDataRef p)
{
    return (int)pl_DynamicArray_GetSize (&p->descriptionComments);
}

const char* pl_AudioFileMetaData_GetCommentDescription (PlankAudioFileMetaDataRef p, const int index)
{
    int size;
    PlankDynamicArray* comments;
    
    size = (int)pl_DynamicArray_GetSize (&p->descriptionComments);
    comments = (PlankDynamicArray*)pl_DynamicArray_GetArray (&p->descriptionComments);

    return pl_DynamicArray_GetArray (&comments[index]);
}

PlankResult pl_AudioFileMetaData_SetOriginatorArtist (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->originatorArtist, text);
}

PlankResult pl_AudioFileMetaData_SetOriginatorRef (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->originatorRef, text);
}

PlankResult pl_AudioFileMetaData_SetOriginationDate (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->originationDate, text);
}

PlankResult pl_AudioFileMetaData_SetOriginationTime (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->originationTime, text);
}

const char* pl_AudioFileMetaData_GetOriginatorArtist (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->originatorArtist);
}

const char* pl_AudioFileMetaData_GetOriginatorRef (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->originatorRef);
}

const char* pl_AudioFileMetaData_GetOriginationDate (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->originationDate);
}

const char* pl_AudioFileMetaData_GetOriginationTime (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->originationTime);
}

PlankResult pl_AudioFileMetaData_SetTitle (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->title, text);
}

PlankResult pl_AudioFileMetaData_SetAlbum (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->album, text);
}

PlankResult pl_AudioFileMetaData_SetGenre (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->genre, text);
}

PlankResult pl_AudioFileMetaData_SetLyrics (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->lyrics, text);
}

PlankResult pl_AudioFileMetaData_SetVendor (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->vendor, text);
}

PlankResult pl_AudioFileMetaData_SetISRC (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->isrc, text);
}

const char* pl_AudioFileMetaData_GetTitle (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->title);
}

const char* pl_AudioFileMetaData_GetAlbum (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->album);
}

const char* pl_AudioFileMetaData_GetGenre (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->genre);
}

const char* pl_AudioFileMetaData_GetLyrics (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->lyrics);
}

const char* pl_AudioFileMetaData_GetVendor (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->vendor);
}

const char* pl_AudioFileMetaData_GetISRC (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->isrc);
}

PlankResult pl_AudioFileMetaData_SetArt (PlankAudioFileMetaDataRef p, PlankConstantP data, const PlankL size)
{
    PlankResult result = PlankResult_OK;
    
    if (pl_DynamicArray_GetItemSize (&p->art) == 0)
        pl_DynamicArray_InitWithItemSize (&p->art, 1);
    
    if (pl_DynamicArray_GetItemSize (&p->art) != 1)
    {
        result = PlankResult_UnknownError;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_SetSize (&p->art, size)) != PlankResult_OK) goto exit;
    
    pl_MemoryCopy (pl_DynamicArray_GetArray (&p->art), data, size);
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_GetArt (PlankAudioFileMetaDataRef p, PlankConstantP* data, PlankL* size)
{
    *data = pl_DynamicArray_GetArray (&p->art);
    *size = pl_DynamicArray_GetSize (&p->art);
    return PlankResult_OK;
}

PlankResult pl_AudioFileMetaData_SetYear (PlankAudioFileMetaDataRef p, const PlankI year)
{
    p->year = year;
    return PlankResult_OK;
}

PlankI pl_AudioFileMetaData_GetYear (PlankAudioFileMetaDataRef p)
{
    return p->year;
}

PlankResult pl_AudioFileMetaData_SetTrackInfo (PlankAudioFileMetaDataRef p, const PlankI trackNum, const PlankI trackTotal)
{
    p->trackNum = trackNum;
    p->trackTotal = trackTotal;
    return PlankResult_OK;
}

PlankResult pl_AudioFileMetaData_GetTrackInfo (PlankAudioFileMetaDataRef p, PlankI* trackNum, PlankI* trackTotal)
{
    if (trackNum != PLANK_NULL)
        *trackNum = p->trackNum;
    
    if (trackTotal != PLANK_NULL)
        *trackTotal = p->trackTotal;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileMetaData_SetTimeRef (PlankAudioFileMetaDataRef p, const PlankLL timeRef)
{
    p->timeRef = timeRef;
    return PlankResult_OK;
}

PlankLL pl_AudioFileMetaData_GetTimeRef (PlankAudioFileMetaDataRef p)
{
    return p->timeRef;;
}

PlankResult pl_AudioFileMetaData_SetSource (PlankAudioFileMetaDataRef p, const PlankUI source)
{
    p->sourceType = source;
    return PlankResult_OK;
}

PlankUI pl_AudioFileMetaData_GetSource (PlankAudioFileMetaDataRef p)
{
    return p->sourceType;
}

PlankResult pl_AudioFileMetaData_SetUMID (PlankAudioFileMetaDataRef p, const PlankUC* umid)
{
    return pl_MemoryCopy (p->umid, umid, 64);
}

const PlankUC* pl_AudioFileMetaData_GetUMID (PlankAudioFileMetaDataRef p)
{
    return p->umid;
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
    
    // should also search regions and loops points...

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

PlankResult pl_AudioFileMetaData_AddLoopPoint (PlankAudioFileMetaDataRef p, PlankAudioFileRegionRef region)
{
    PlankResult result = PlankResult_OK;
    
    if (pl_DynamicArray_GetItemSize (&p->regions) == 0)
    {
        if ((result = pl_DynamicArray_InitWithItemSize (&p->loopPoints, sizeof (PlankAudioFileRegion))) != PlankResult_OK) goto exit;
    }
    
    if ((result = pl_DynamicArray_AddItem (&p->loopPoints, region)) != PlankResult_OK) goto exit;
    
    pl_MemoryZero (region, sizeof (PlankAudioFileRegion));
    
exit:
    return result;   
}

PlankResult pl_AudioFileMetaData_AddCodingHistory (PlankAudioFileMetaDataRef p, const char* text)
{
    pl_DynamicArray_AppendTextLineCRLF (&p->codingHistory, text);
    return 0;
}

PlankResult pl_AudioFileMetaData_AddFormatSpecificBlock (PlankAudioFileMetaDataRef p, PlankDynamicArrayRef block)
{
    PlankResult result;
    result = PlankResult_OK;
    
    if ((result = pl_SimpleLinkedList_Add (&p->formatSpecific, pl_SimpleLinkedListElement_CreateAndInitWthData (block))) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_CuePoints_InsertFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    PlankAudioFileCuePoint* cuePoints;
    PlankLL position;
    
    numCuePoints = pl_DynamicArray_GetSize (&p->cuePoints);
    cuePoints = (PlankAudioFileCuePoint*)pl_DynamicArray_GetArray (&p->cuePoints);
    
    for (i = numCuePoints; --i >= 0;)
    {
        position = pl_AudioFileCuePoint_GetPosition (&cuePoints[i]);
        
        if (start <= position)
        {
            if ((result = pl_AudioFileCuePoint_SetPosition (&cuePoints[i], position + length) != PlankResult_OK)) goto exit;
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_RegionsArray_InsertFrames (PlankDynamicArrayRef regionArray, const PlankLL start, const PlankLL length)
{
    PlankResult result = PlankResult_OK;
    PlankL numRegions, i;
    PlankAudioFileRegion* regions;
    PlankLL regionStart, regionEnd, end;
    
    numRegions = pl_DynamicArray_GetSize (regionArray);
    regions = (PlankAudioFileRegion*)pl_DynamicArray_GetArray (regionArray);
    end = start + length;
    
    for (i = numRegions; --i >= 0;)
    {
        regionStart = pl_AudioFileRegion_GetStartPosition (&regions[i]);
        regionEnd = pl_AudioFileRegion_GetEndPosition (&regions[i]);
        
        if (start < regionEnd)
        {
            if ((result = pl_AudioFileRegion_SetEndPosition (&regions[i], regionEnd + length) != PlankResult_OK)) goto exit;
            
            if (start < regionStart)
            {
                if ((result = pl_AudioFileRegion_SetStartPosition (&regions[i], regionStart + length) != PlankResult_OK)) goto exit;
            }
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_Regions_InsertFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_InsertFrames (&p->regions, start, length);
}

static PlankResult pl_AudioFileMetaData_LoopPoints_InsertFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_InsertFrames (&p->loopPoints, start, length);
}

PlankResult pl_AudioFileMetaData_InsertFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    PlankResult result;
    result = PlankResult_OK;
    
    if ((result = pl_AudioFileMetaData_CuePoints_InsertFrames (p, start, length)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_Regions_InsertFrames (p, start, length)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_LoopPoints_InsertFrames (p, start, length)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_CuePoints_RemoveFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    PlankAudioFileCuePoint* cuePoints;
    PlankLL end, position;
    
    numCuePoints = pl_DynamicArray_GetSize (&p->cuePoints);
    cuePoints = (PlankAudioFileCuePoint*)pl_DynamicArray_GetArray (&p->cuePoints);
    end = start + length;
    
    // must go in reverse since we might remove items
    for (i = numCuePoints; --i >= 0;)
    {
        position = pl_AudioFileCuePoint_GetPosition (&cuePoints[i]);
        
        if (position >= start)
        {
            if (position < end)
            {
                if ((result = pl_DynamicArray_RemoveItem (&p->cuePoints, i)) != PlankResult_OK) goto exit;
            }
            else
            {
                if ((result = pl_AudioFileCuePoint_SetPosition (&cuePoints[i], position - length) != PlankResult_OK)) goto exit;
            }
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_RegionsArray_RemoveFrames (PlankDynamicArrayRef regionArray, const PlankLL start, const PlankLL length)
{
    PlankResult result = PlankResult_OK;
    PlankL numRegions, i;
    PlankAudioFileRegion* regions;
    PlankLL regionStart, regionEnd, end;
    
    numRegions = pl_DynamicArray_GetSize (regionArray);
    regions = (PlankAudioFileRegion*)pl_DynamicArray_GetArray (regionArray);
    end = start + length;
    
    // must go in reverse since we might remove items
    for (i = numRegions; --i >= 0;)
    {
        regionStart = pl_AudioFileRegion_GetStartPosition (&regions[i]);
        regionEnd = pl_AudioFileRegion_GetEndPosition (&regions[i]);
        
        if (start < regionEnd)
        {
            if (end <= regionStart)
            {
                if ((result = pl_AudioFileRegion_SetStartPosition (&regions[i], regionStart - length) != PlankResult_OK)) goto exit;
                if ((result = pl_AudioFileRegion_SetEndPosition (&regions[i], regionEnd - length) != PlankResult_OK)) goto exit;
            }
            else
            {
                if ((result = pl_DynamicArray_RemoveItem (regionArray, i)) != PlankResult_OK) goto exit;
            }
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_Regions_RemoveFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_RemoveFrames (&p->regions, start, length);
}

static PlankResult pl_AudioFileMetaData_LoopPoints_RemoveFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_RemoveFrames (&p->loopPoints, start, length);
}

PlankResult pl_AudioFileMetaData_RemoveFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    PlankResult result;
    result = PlankResult_OK;
    
    if ((result = pl_AudioFileMetaData_CuePoints_RemoveFrames (p, start, length)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_Regions_RemoveFrames (p, start, length)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileMetaData_LoopPoints_RemoveFrames (p, start, length)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}


