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
#include "../../maths/plank_Maths.h"

//#define PLANKAUDIOFILEMETADATA_ID3GENRE_BLUES 0 //.Blues
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_CLASSICROCk 1 //.Classic Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_COUNTRY 2 //.Country
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_DANCE 3 //.Dance
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_DISCO 4 //.Disco
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_FUNK 5 //.Funk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_GRUNGE 6 //.Grunge
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_HIPHOP 7 //.Hip-Hop
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_JAZZ 8 //.Jazz
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_METAL 9//.Metal
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_NEWAGE 10//.New Age
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_OLDIES 11//.Oldies
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_OTHER 12//.Other
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_POP 13//.Pop
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_RNB 14//.R&B
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_RAP 15//.Rap
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_REGGAE 16//.Reggae
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ROCK 17//.Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_TECHNO 18//.Techno
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_INDUSTRIAL 19//.Industrial
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ALTERNATIVE 20//.Alternative
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_SKA 21//.Ska
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_DEATHMETAL 22//.Death Metal
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_PRANKS 23//.Pranks
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_SOUNDTRACK 24//.Soundtrack
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_EUROTECHNO 25//.Euro-Techno
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_AMBIENT 26//.Ambient
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_TRIPHOP 27//.Trip-Hop
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_VOCAL 28//.Vocal
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_JAZZFUNK 29//.Jazz+Funk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_FUSION 30//.Fusion
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_TRANCE 31//.Trance
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_CLASSICAL 32//.Classical
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_INSTRUMENTAL 33//.Instrumental
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ACID 34//.Acid
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_HOUSE 35//.House
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_GAME 36//.Game
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_SOUNDCLIP 37//.Sound Clip
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_GOSPEL 38//.Gospel
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_NOISE 39//.Noise
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ALTERNROCK 40//.AlternRock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_BASS 41//.Bass
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_SOUL 42//.Soul
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_PUNK 43//.Punk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_SPACE 44//.Space
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_MEDITATIVE 45//.Meditative
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_INSTRUMENTALPOP 46//.Instrumental Pop
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_INSTRUMENTALROCK 47//.Instrumental Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ETHNIC 48//.Ethnic
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_GOTHIC 49//.Gothic
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_DARKWAVE 50//.Darkwave
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_TECHNOINDUSTRIAL 51//.Techno-Industrial
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ELECTRONIC 52//.Electronic
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_POPFOLK 53//.Pop-Folk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_EURODANCE 54//.Eurodance
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_DREAM 55//.Dream
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_SOUTHERNROCK 56//.Southern Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_COMEDY 57//.Comedy
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_CULT 58//.Cult
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_GANGSTA 59//.Gangsta
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_TOP40 60//.Top 40
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_CHRISTIANPOP 61//.Christian Rap
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_POPFUNK 62//.Pop/Funk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 63//.Jungle
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 64//.Native American
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 65//.Cabaret
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 66//.New Wave
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 67//.Psychadelic
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 68//.Rave
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 69//.Showtunes
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 70//.Trailer
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 71//.Lo-Fi
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 72//.Tribal
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 73//.Acid Punk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 74//.Acid Jazz
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 75//.Polka
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 76//.Retro
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 77//.Musical
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 78//.Rock & Roll
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 79//.Hard Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 80//.Folk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 81//.Folk-Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 82//.National Folk
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 83//.Swing
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 84//.Fast Fusion
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 85//.Bebob
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 86//.Latin
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 87//.Revival
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 88//.Celtic
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 89//.Bluegrass
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 90//.Avantgarde
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 91//.Gothic Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 92//.Progressive Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 93//.Psychedelic Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 94//.Symphonic Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 95//.Slow Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 96//.Big Band
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 97//.Chorus
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 98//.Easy Listening
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 99//.Acoustic
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 100//.Humour
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 101//.Speech
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 102//.Chanson
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 103//.Opera
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 104//.Chamber Music
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 105//.Sonata
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 106//.Symphony
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 107//.Booty Bass
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 108//.Primus
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 109//.Porn Groove
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 110//.Satire
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 111//.Slow Jam
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 112//.Club
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 113//.Tango
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 114//.Samba
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 115//.Folklore
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 116//.Ballad
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 117//.Power Ballad
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 118//.Rhythmic Soul
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 119//.Freestyle
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 120//.Duet
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 121///.Punk Rock
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 122//.Drum Solo
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 123//.Acapella
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 124//.Euro-House
//#define PLANKAUDIOFILEMETADATA_ID3GENRE_ 125//.Dance Hall
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

PlankResult pl_AudioFileMetaData_Init (PlankAudioFileMetaDataRef p);
PlankResult pl_AudioFileMetaData_DeInit (PlankAudioFileMetaDataRef p);

static PlankResult pl_AudioFileMetaDataFormatSpecificFree (PlankP ptr)
{
    return pl_DynamicArray_Destroy ((PlankDynamicArrayRef)ptr);
}

PLANKSHAREDPTR_CREATEANDINIT_DEFINE(AudioFileMetaData)
PLANKSHAREDPTR_INCREMENTREFCOUNTANDGET_DEFINE(AudioFileMetaData)
PLANKSHAREDPTR_DECREMENTREFCOUNT_DEFINE(AudioFileMetaData)
PLANKSHAREDPTR_GETWEAKPTR_DEFINE(AudioFileMetaData)
PLANKSHAREDPTR_GETFROMWEAKPTR_DEFINE(AudioFileMetaData)

PlankResult pl_AudioFileMetaData_Init (PlankAudioFileMetaDataRef p)
{
    PlankResult result = PlankResult_OK;
        
    if (p == PLANK_NULL)
    {
        result = PlankResult_NullPointerError;
        goto exit;
    }
    
#if PLANKSHAREDPTR_DEBUG
    printf("pl_AudioFileMetaData_Init (%p)\n", p);
#endif 
    
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
    
    pl_SharedPtrArray_CreateSharedPtr (&p->cuePoints);
    pl_SharedPtrArray_CreateSharedPtr (&p->loopPoints);
    pl_SharedPtrArray_CreateSharedPtr (&p->regions);
    
    // the formatSpecific linked list contains elements that are DynamicArrays
    pl_SimpleLinkedList_SetFreeElementDataFunction (&p->formatSpecific, pl_AudioFileMetaDataFormatSpecificFree);
    

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
    
#if PLANKSHAREDPTR_DEBUG
    printf("pl_AudioFileMetaData_DeInit (%p)\n", p);
#endif
    
    if ((result = pl_DynamicArray_DeInit (&p->descriptionComment)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originatorArtist)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originatorRef)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originationDate)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->originationTime)) != PlankResult_OK) goto exit;

    if ((result = pl_DynamicArray_DeInit (&p->performer)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->title)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->album)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->genre)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->lyrics)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->vendor)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->isrc)) != PlankResult_OK) goto exit;

    if ((result = pl_DynamicArray_DeInit (&p->art)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_DeInit (&p->samplerData)) != PlankResult_OK) goto exit;
    
    if ((result = pl_DynamicArray_DeInit (&p->codingHistory)) != PlankResult_OK) goto exit;

    if ((result = pl_SharedPtrArray_DecrementRefCount (p->cuePoints)) != PlankResult_OK) goto exit;
    if ((result = pl_SharedPtrArray_DecrementRefCount (p->loopPoints)) != PlankResult_OK) goto exit;
    if ((result = pl_SharedPtrArray_DecrementRefCount (p->regions)) != PlankResult_OK) goto exit;

    // this should free any extra data stored as dynamic arrays in the linked list
    if ((result = pl_SimpleLinkedList_Clear (&p->formatSpecific)) != PlankResult_OK) goto exit;
        
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

PlankDynamicArrayRef pl_AudioFileMetaData_GetExtraSamplerData (PlankAudioFileMetaDataRef p)
{
    return &p->samplerData;
}

PlankResult pl_AudioFileMetaData_SetDescriptionComment (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->descriptionComment, text);
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

const char* pl_AudioFileMetaData_GetDescriptionComment (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->descriptionComment);
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

PlankResult pl_AudioFileMetaData_SetPerformer (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_SetAsText (&p->performer, text);
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

const char* pl_AudioFileMetaData_GetPerformer (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->performer);
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

PlankResult pl_AudioFileMetaData_AddPerformer (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLineCRLF (&p->performer, text);
}

PlankResult pl_AudioFileMetaData_AddDescriptionComment (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLineCRLF (&p->descriptionComment, text);
}

PlankResult pl_AudioFileMetaData_AddOriginatorArtist (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLineCRLF (&p->originatorArtist, text);
}

PlankResult pl_AudioFileMetaData_AddOriginatorRef (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLineCRLF (&p->originatorRef, text);
}

PlankResult pl_AudioFileMetaData_AddGenre (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLineCRLF (&p->genre, text);
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
    PlankResult result;
    
    if ((result = pl_SharedPtrArray_AddSharedPtr (p->cuePoints, (PlankSharedPtrRef)cuePoint)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_AudioFileMetaData_FindCuePointWithID (PlankAudioFileMetaDataRef p, const PlankUI cueID, PlankAudioFileCuePointRef* cuePoint, PlankL* index)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    PlankAudioFileCuePointRef temp;
    
    numCuePoints = pl_SharedPtrArray_GetLength (p->cuePoints);
    
    for (i = 0; i < numCuePoints; ++i)
    {
        temp = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (p->cuePoints, i);
        
        if (pl_AudioFileCuePoint_GetID (temp) == cueID)
        {
            *cuePoint = temp;
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

PlankResult pl_AudioFileMetaData_RemoveCuePointWithID (PlankAudioFileMetaDataRef p, const PlankUI cueID, PlankAudioFileCuePointRef* cuePoint, PlankB* success)
{
    PlankResult result = PlankResult_OK;
    PlankL numCuePoints, i;
    
    numCuePoints = pl_SharedPtrArray_GetLength (p->cuePoints);
    *success = PLANK_FALSE;
    
    for (i = 0; i < numCuePoints; ++i)
    {
        if (pl_AudioFileCuePoint_GetID ((PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (p->cuePoints, i)) == cueID)
        {
            if (cuePoint)
            {
                *cuePoint = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (p->cuePoints, i);
                pl_AudioFileCuePoint_IncrementRefCountAndGet (*cuePoint);
            }
            
            result = pl_SharedPtrArray_RemoveSharedPtr (p->cuePoints, i);
            *success = PLANK_TRUE;
            goto exit;
        }
    }
        
exit:
    return result;    
}

PlankResult pl_AudioFileMetaData_GetNextCueID (PlankAudioFileMetaDataRef p, PlankUI* cueID)
{
    PlankResult result = PlankResult_OK;
    PlankUI nextCueID;
    PlankAtomicP* cueArray;
    PlankL numCues, i;

    nextCueID = 1;
    numCues = pl_SharedPtrArray_GetLength (p->cuePoints);
    cueArray = (PlankAtomicP*)pl_SharedPtrArray_GetArray (p->cuePoints);
    
    for (i = 0; i < numCues; ++i)
        nextCueID = pl_MaxUI (nextCueID, pl_AudioFileCuePoint_GetID (cueArray[i].ptr)) + 1;
    
    *cueID = nextCueID;
    
exit:
    return result;
}

PlankSharedPtrArrayRef pl_AudioFileMetaData_GetCuePoints (PlankAudioFileMetaDataRef p)
{
    return p->cuePoints;
}

PlankSharedPtrArrayRef pl_AudioFileMetaData_GetRegions (PlankAudioFileMetaDataRef p)
{
    return p->regions;
}

PlankSharedPtrArrayRef pl_AudioFileMetaData_GetLoopPoints (PlankAudioFileMetaDataRef p)
{
    return p->loopPoints;
}

PlankResult pl_AudioFileMetaData_AddRegion (PlankAudioFileMetaDataRef p, PlankAudioFileRegionRef region)
{    
    PlankResult result;
    
    if ((result = pl_SharedPtrArray_AddSharedPtr (p->regions, (PlankSharedPtrRef)region)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_ConvertCuePointsToRegions (PlankAudioFileMetaDataRef p, const PlankLL numFrames, const PlankB removeCuePoints)
{
    PlankAudioFileRegionRef region;
    PlankResult result = PlankResult_OK;
    PlankAudioFileCuePointRef* cueArray;
    PlankAudioFileCuePointRef regionAnchorCue;
    PlankAudioFileCuePointRef regionStartCue;
    PlankAudioFileCuePointRef regionEndCue;
    PlankL numCues, i;
    PlankUI cueID;
    PlankLL start, end;
    
    numCues = pl_SharedPtrArray_GetLength (p->cuePoints);
    
    if (numCues <= 0)
        goto exit;
    
    cueArray = (PlankAudioFileCuePointRef*)pl_SharedPtrArray_GetArray (p->cuePoints);
    pl_AudioFileMetaData_GetNextCueID (p, &cueID);
    
    start = 0;
    end = pl_AudioFileCuePoint_GetPosition (cueArray[0]);
    region = PLANK_NULL;
    
    // if the first cue is not at frame zero, make a region from zero to the first cue
    if (end != 0)
    {
        pl_AudioFileRegion_CreateSharedPtr (&region);
        regionAnchorCue = pl_AudioFileRegion_GetAnchorCuePoint (region);
        regionStartCue  = pl_AudioFileRegion_GetStartCuePoint (region);
        regionEndCue    = pl_AudioFileRegion_GetEndCuePoint (region);

        pl_AudioFileCuePoint_SetPosition (regionAnchorCue, 0);
        pl_AudioFileCuePoint_SetPosition (regionStartCue, 0);
        pl_AudioFileCuePoint_SetPosition (regionEndCue, end);
        pl_AudioFileCuePoint_SetID (regionAnchorCue, cueID++);
        pl_AudioFileCuePoint_SetID (regionStartCue, cueID++);
        pl_AudioFileCuePoint_SetID (regionEndCue, cueID++);
        
        pl_AudioFileMetaData_AddRegion (p, region);
        pl_AudioFileRegion_DecrementRefCount (region);
        region = PLANK_NULL;
    }
    
    // then regions between all the other cues, keeping labels for the region starts
    for (i = 1; i < numCues; ++i)
    {
        pl_AudioFileRegion_CreateSharedPtr (&region);
        regionAnchorCue = pl_AudioFileRegion_GetAnchorCuePoint (region);
        regionStartCue  = pl_AudioFileRegion_GetStartCuePoint (region);
        regionEndCue    = pl_AudioFileRegion_GetEndCuePoint (region);

        pl_AudioFileCuePoint_SetCopy (regionAnchorCue, cueArray[i - 1]);
        start = pl_AudioFileCuePoint_GetPosition (regionAnchorCue);
        end   = pl_AudioFileCuePoint_GetPosition (cueArray[i]);
        pl_AudioFileCuePoint_SetPosition (regionStartCue, start);
        pl_AudioFileCuePoint_SetPosition (regionEndCue, end);
        
        pl_AudioFileCuePoint_SetID (regionAnchorCue, cueID++);
        pl_AudioFileCuePoint_SetID (regionStartCue, cueID++);
        pl_AudioFileCuePoint_SetID (regionEndCue, cueID++);

        pl_AudioFileMetaData_AddRegion (p, region);
        pl_AudioFileRegion_DecrementRefCount (region);
        region = PLANK_NULL;
    }
    
    // if the last cue is not at the end of the file, add a region from the last cue to the end
    if (end < numFrames)
    {
        pl_AudioFileRegion_CreateSharedPtr (&region);
        regionAnchorCue = pl_AudioFileRegion_GetAnchorCuePoint (region);
        regionStartCue  = pl_AudioFileRegion_GetStartCuePoint (region);
        regionEndCue    = pl_AudioFileRegion_GetEndCuePoint (region);

        pl_AudioFileCuePoint_SetCopy (regionAnchorCue, cueArray[numCues - 1]);
        start = pl_AudioFileCuePoint_GetPosition (regionAnchorCue);
        end   = numFrames;
        pl_AudioFileCuePoint_SetPosition (regionStartCue, start);
        pl_AudioFileCuePoint_SetPosition (regionEndCue, numFrames);
        
        pl_AudioFileCuePoint_SetID (regionAnchorCue, cueID++);
        pl_AudioFileCuePoint_SetID (regionStartCue, cueID++);
        pl_AudioFileCuePoint_SetID (regionEndCue, cueID++);
        
        pl_AudioFileMetaData_AddRegion (p, region);
        pl_AudioFileRegion_DecrementRefCount (region);
        region = PLANK_NULL;
    }
    
    if (removeCuePoints)
    {
        pl_SharedPtrArray_Clear (p->cuePoints);
    }
    
exit:
    return result;
}

PlankResult pl_AudioFileMetaData_SortCuePoints (PlankAudioFileMetaDataRef p)
{
    // should do atomic swaps..
    return pl_SharedPtrArray_Sort (p->cuePoints, (PlankSharedPtrArrayCompareFunction)pl_AudioFileCuePoint_ComparePosition);
}

//PlankResult pl_AudioFileMetaData_SortCuePoints (PlankAudioFileMetaDataRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankAudioFileCuePointRef* cueArray;
//    PlankL numCues, i, j;
//    PlankB swapped;
//    
//    numCues = pl_SharedPtrArray_GetLength (p->cuePoints);
//    cueArray = (PlankAudioFileCuePointRef*)pl_SharedPtrArray_GetArray (p->cuePoints);
//    
//    for (i = numCues; --i >= 0;)
//    {
//        swapped = PLANK_FALSE;
//        
//        for (j = 0; j < i; j++)
//        {
//            if (cueArray[j]->position > cueArray[j + 1]->position)
//            {
//                pl_SharedPtrArray_Swap (p->cuePoints, j, j + 1);
//                swapped = PLANK_TRUE;
//            }
//        }
//        
//        if (!swapped)
//            goto exit;
//    }
//    
//exit:
//    return result;
//}

PlankResult pl_AudioFileMetaData_AddLoopPoint (PlankAudioFileMetaDataRef p, PlankAudioFileRegionRef region)
{    
    PlankResult result;
    
    if ((result = pl_SharedPtrArray_AddSharedPtr (p->loopPoints, (PlankSharedPtrRef)region)) != PlankResult_OK) goto exit;
    
exit:
    return result;

}

PlankResult pl_AudioFileMetaData_AddCodingHistory (PlankAudioFileMetaDataRef p, const char* text)
{
    return pl_DynamicArray_AppendTextLineCRLF (&p->codingHistory, text);
}

const char* pl_AudioFileMetaData_GetCodingHistory (PlankAudioFileMetaDataRef p)
{
    return pl_DynamicArray_GetArray (&p->codingHistory);
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
    PlankAudioFileCuePointRef cuePoint;
    PlankLL position;
    
    numCuePoints = pl_SharedPtrArray_GetLength (p->cuePoints);
    
    for (i = numCuePoints; --i >= 0;)
    {
        cuePoint = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (p->cuePoints, i);
        position = pl_AudioFileCuePoint_GetPosition (cuePoint);
        
        if (start <= position)
        {
            if ((result = pl_AudioFileCuePoint_SetPosition (cuePoint, position + length) != PlankResult_OK)) goto exit;
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_RegionsArray_InsertFrames (PlankSharedPtrArrayRef regionArray, const PlankLL start, const PlankLL length)
{
    PlankResult result = PlankResult_OK;
    PlankL numRegions, i;
    PlankAudioFileRegionRef region;
    PlankLL regionStart, regionEnd, end;
    
    numRegions = pl_SharedPtrArray_GetLength (regionArray);
    end = start + length;
    
    for (i = numRegions; --i >= 0;)
    {
        region      = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (regionArray, i);
        regionStart = pl_AudioFileRegion_GetStartPosition (region);
        regionEnd   = pl_AudioFileRegion_GetEndPosition (region);
        
        if (start < regionEnd)
        {
            if ((result = pl_AudioFileRegion_SetEndPosition (region, regionEnd + length) != PlankResult_OK)) goto exit;
            
            if (start < regionStart)
            {
                if ((result = pl_AudioFileRegion_SetStartPosition (region, regionStart + length) != PlankResult_OK)) goto exit;
            }
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_Regions_InsertFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_InsertFrames (p->regions, start, length);
}

static PlankResult pl_AudioFileMetaData_LoopPoints_InsertFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_InsertFrames (p->loopPoints, start, length);
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
    PlankAudioFileCuePointRef cuePoint;
    PlankLL end, position;
    
    numCuePoints = pl_SharedPtrArray_GetLength (p->cuePoints);
    end = start + length;
    
    // must go in reverse since we might remove items
    for (i = numCuePoints; --i >= 0;)
    {
        cuePoint = (PlankAudioFileCuePointRef)pl_SharedPtrArray_GetSharedPtr (p->cuePoints, i);
        position = pl_AudioFileCuePoint_GetPosition (cuePoint);
        
        if (position >= start)
        {
            if (position < end)
            {
                if ((result = pl_SharedPtrArray_RemoveSharedPtr (p->cuePoints, i)) != PlankResult_OK) goto exit;
            }
            else
            {
                if ((result = pl_AudioFileCuePoint_SetPosition (cuePoint, position - length) != PlankResult_OK)) goto exit;
            }
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_RegionsArray_RemoveFrames (PlankSharedPtrArrayRef regionArray, const PlankLL start, const PlankLL length)
{
    PlankResult result = PlankResult_OK;
    PlankL numRegions, i;
    PlankAudioFileRegionRef region;
    PlankLL regionStart, regionEnd, end;
    
    numRegions = pl_SharedPtrArray_GetLength (regionArray);
    end = start + length;
    
    // must go in reverse since we might remove items
    for (i = numRegions; --i >= 0;)
    {
        region      = (PlankAudioFileRegionRef)pl_SharedPtrArray_GetSharedPtr (regionArray, i);
        regionStart = pl_AudioFileRegion_GetStartPosition (region);
        regionEnd   = pl_AudioFileRegion_GetEndPosition (region);
        
        if (start < regionEnd)
        {
            if (end <= regionStart)
            {
                if ((result = pl_AudioFileRegion_SetStartPosition (region, regionStart - length) != PlankResult_OK)) goto exit;
                if ((result = pl_AudioFileRegion_SetEndPosition (region, regionEnd - length) != PlankResult_OK)) goto exit;
            }
            else
            {
                if ((result = pl_SharedPtrArray_RemoveSharedPtr (regionArray, i)) != PlankResult_OK) goto exit;
            }
        }
    }
    
exit:
    return result;
}

static PlankResult pl_AudioFileMetaData_Regions_RemoveFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_RemoveFrames (p->regions, start, length);
}

static PlankResult pl_AudioFileMetaData_LoopPoints_RemoveFrames (PlankAudioFileMetaDataRef p, const PlankLL start, const PlankLL length)
{
    return pl_AudioFileMetaData_RegionsArray_RemoveFrames (p->loopPoints, start, length);
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

