/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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
#include "plank_AudioFileRegion.h"
#include "plank_AudioFileCuePoint.h"



//PlankAudioFileRegionRef pl_AudioFileRegion_Create()
//{
//    return (PlankAudioFileRegionRef)PLANK_NULL;
//}
//
//PlankResult pl_AudioFileRegion_Destroy (PlankAudioFileRegionRef p)
//{
//	(void)p;
//    return PlankResult_OK;
//}
//
//PlankResult pl_AudioFileRegion_Init (PlankAudioFileRegionRef p)
//{
//    PlankResult result = PlankResult_OK;
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    pl_MemoryZero (p, sizeof (PlankAudioFileRegion));
//    
//exit:
//    return result;
//}
//
//PlankResult pl_AudioFileRegion_InitCopy (PlankAudioFileRegionRef p, PlankAudioFileRegionRef original)
//{
//    PlankResult result = PlankResult_OK;
//    
//    if ((result = pl_AudioFileRegion_Init (p)) != PlankResult_OK)
//        goto exit;
//    
//    if ((result = pl_AudioFileCuePoint_InitCopy (&p->anchor, &original->anchor)) != PlankResult_OK) goto exit;
//    if ((result = pl_AudioFileCuePoint_InitCopy (&p->start, &original->start)) != PlankResult_OK) goto exit;
//    if ((result = pl_AudioFileCuePoint_InitCopy (&p->end, &original->end)) != PlankResult_OK) goto exit;
//
//    p->regionType = original->regionType;
//    p->fraction   = original->fraction;
//    p->playCount  = original->playCount;
//    
//exit:
//    return result;
//}

static PlankResult pl_AudioFileRegion_Init (PlankAudioFileRegionRef p);
static PlankResult pl_AudioFileRegion_DeInit (PlankAudioFileRegionRef p);

PLANKSHAREDPTR_CREATEANDINIT_DEFINE(AudioFileRegion, PLANKSHAREDPTR_TYPE_AUDIOFILEREGION)
PLANKSHAREDPTR_INCREMENTREFCOUNTANDGET_DEFINE(AudioFileRegion)
PLANKSHAREDPTR_DECREMENTREFCOUNT_DEFINE(AudioFileRegion)
PLANKSHAREDPTR_GETWEAKPTR_DEFINE(AudioFileRegion)
PLANKSHAREDPTR_GETFROMWEAKPTR_DEFINE(AudioFileRegion)

static PlankResult pl_AudioFileRegion_Init (PlankAudioFileRegionRef p)
{
#if PLANKSHAREDPTR_DEBUG
    printf("pl_AudioFileRegion_Init (%p)\n",p);
#endif 
    
    pl_AudioFileCuePoint_CreateSharedPtr (&p->anchor);
    pl_AudioFileCuePoint_CreateSharedPtr (&p->start);
    pl_AudioFileCuePoint_CreateSharedPtr (&p->end);
    
    return PlankResult_OK;
}

static PlankResult pl_AudioFileRegion_DeInit (PlankAudioFileRegionRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }

#if PLANKSHAREDPTR_DEBUG
    printf("pl_AudioFileRegion_DeInit (%p)\n",p);
#endif 
    
    if ((result = pl_AudioFileCuePoint_DecrementRefCount (p->anchor)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileCuePoint_DecrementRefCount (p->start)) != PlankResult_OK) goto exit;
    if ((result = pl_AudioFileCuePoint_DecrementRefCount (p->end)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankAudioFileCuePointRef pl_AudioFileRegion_GetAnchorCuePoint (PlankAudioFileRegionRef p)
{
    return p->anchor;
}

PlankAudioFileCuePointRef pl_AudioFileRegion_GetStartCuePoint (PlankAudioFileRegionRef p)
{
    return p->start;
}

PlankAudioFileCuePointRef pl_AudioFileRegion_GetEndCuePoint (PlankAudioFileRegionRef p)
{
    return p->end;
}

double pl_AudioFileRegion_GetAnchorPosition (PlankAudioFileRegionRef p)
{
    return p->anchor->position;
}

double pl_AudioFileRegion_GetStartPosition (PlankAudioFileRegionRef p)
{
    return p->start->position;
}

double pl_AudioFileRegion_GetEndPosition (PlankAudioFileRegionRef p)
{
    return p->end->position;
}

double pl_AudioFileRegion_GetLength (PlankAudioFileRegionRef p)
{
    return p->end->position - p->start->position;
}

int pl_AudioFileRegion_GetType (PlankAudioFileRegionRef p)
{
    return p->regionType;
}

PlankUI pl_AudioFileRegion_GetOptions (PlankAudioFileRegionRef p)
{
    return p->regionOptions;
}

double pl_AudioFileRegion_GetFraction (PlankAudioFileRegionRef p)
{
    return p->fraction;
}

PlankUI pl_AudioFileRegion_GetPlayCount (PlankAudioFileRegionRef p)
{
    return p->playCount;
}

const char* pl_AudioFileRegion_GetLabel (PlankAudioFileRegionRef p)
{
    return pl_AudioFileCuePoint_GetLabel (p->anchor);
}

char* pl_AudioFileRegion_GetLabelWritable (PlankAudioFileRegionRef p)
{
    return pl_AudioFileCuePoint_GetLabelWritable (p->anchor);
}

const char* pl_AudioFileRegion_GetComment (PlankAudioFileRegionRef p)
{
    return pl_AudioFileCuePoint_GetComment (p->anchor);
}

char* pl_AudioFileRegion_GetCommentWritable (PlankAudioFileRegionRef p)
{
    return pl_AudioFileCuePoint_GetCommentWritable (p->anchor);
}

PlankResult pl_AudioFileRegion_SetAnchorPosition (PlankAudioFileRegionRef p, const double position)
{
    return pl_AudioFileCuePoint_SetPosition (p->anchor, position);
}

PlankResult pl_AudioFileRegion_SetStartPosition (PlankAudioFileRegionRef p, const double position)
{
    return pl_AudioFileCuePoint_SetPosition (p->start, position);
}

PlankResult pl_AudioFileRegion_SetEndPosition (PlankAudioFileRegionRef p, const double position)
{
    return pl_AudioFileCuePoint_SetPosition (p->end, position);
}

PlankResult pl_AudioFileRegion_SetLength (PlankAudioFileRegionRef p, const double length)
{
    return pl_AudioFileCuePoint_SetPosition (p->end, p->start->position + length);
}

PlankResult pl_AudioFileRegion_SetType (PlankAudioFileRegionRef p, const int type)
{
    p->start->type = PLANKAUDIOFILE_CUEPOINTTYPE_REGIONSTART;
    p->end->type   = PLANKAUDIOFILE_CUEPOINTTYPE_REGIONEND;
    p->regionType  = type;
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_SetOptions (PlankAudioFileRegionRef p, const PlankUI options)
{
    p->regionOptions = options;
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_SetFraction (PlankAudioFileRegionRef p, const double fraction)
{
    p->fraction = fraction;
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_SetPlayCount (PlankAudioFileRegionRef p, const PlankUI playCount)
{
    p->playCount = playCount;
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_SetLabel (PlankAudioFileRegionRef p, const char* label)
{
    return pl_AudioFileCuePoint_SetLabel (p->anchor, label);
}

PlankResult pl_AudioFileRegion_SetComment (PlankAudioFileRegionRef p, const char* comment)
{
    return pl_AudioFileCuePoint_SetComment (p->anchor, comment);
}

PlankResult pl_AudioFileRegion_GetRegion (PlankAudioFileRegionRef p, double* start, double* end, double* anchor)
{
    if (start != PLANK_NULL)
        *start = p->start->position;
    
    if (end != PLANK_NULL)
        *end = p->end->position;
    
    if (anchor != PLANK_NULL)
        *anchor = p->anchor->position;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_SetRegion (PlankAudioFileRegionRef p, const double start, const double end)
{
    if (start >= 0)
    {
        p->start->position = start;
        p->anchor->position = start;
    }
    
    if (end >= 0)
        p->end->position = end;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_SetRegionWithAnchor (PlankAudioFileRegionRef p, const double start, const double end, const double anchor)
{
    if (start >= 0)
        p->start->position = start;
    
    if (end >= 0)
        p->end->position = end;

    if (anchor >= 0)
        p->anchor->position = anchor;
    
    return PlankResult_OK;
}

PlankResult pl_AudioFileRegion_OffsetPosition (PlankAudioFileRegionRef p, const double offset)
{
    p->start->position  += offset;
    p->end->position    += offset;
    p->anchor->position += offset;
    return PlankResult_OK;
}




