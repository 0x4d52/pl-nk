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

#ifndef PLANK_AUDIOFILEREGION_H
#define PLANK_AUDIOFILEREGION_H

#include "plank_AudioFileCommon.h"
#include "plank_AudioFileCuePoint.h"

#define PLANKSHAREDPTR_TYPE_AUDIOFILEREGION pl_FourCharCode("regn")


PLANK_BEGIN_C_LINKAGE

/** An audio file region reference.
 This is a generic region, this may also be specified as a loop, selection etc. 
 
 @defgroup PlankAudioFileRegionClass Plank AudioFileRegion class
 @ingroup PlankClasses
 @{
 */

/** Create a <i>Plank AudioFileRegion</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileRegion</i> object as an opaque reference or PLANK_NULL. */
PlankResult pl_AudioFileRegion_CreateSharedPtr (PlankAudioFileRegionRef* pp);
PlankAudioFileRegionRef pl_AudioFileRegion_IncrementRefCountAndGet (PlankAudioFileRegionRef p);
PlankResult pl_AudioFileRegion_DecrementRefCount (PlankAudioFileRegionRef p);

PlankAudioFileCuePointRef pl_AudioFileRegion_GetAnchorCuePoint (PlankAudioFileRegionRef p);
PlankAudioFileCuePointRef pl_AudioFileRegion_GetStartCuePoint (PlankAudioFileRegionRef p);
PlankAudioFileCuePointRef pl_AudioFileRegion_GetEndCuePoint (PlankAudioFileRegionRef p);

double pl_AudioFileRegion_GetAnchorPosition (PlankAudioFileRegionRef p);
double pl_AudioFileRegion_GetStartPosition (PlankAudioFileRegionRef p);
double pl_AudioFileRegion_GetEndPosition (PlankAudioFileRegionRef p);
double pl_AudioFileRegion_GetLength (PlankAudioFileRegionRef p);
int pl_AudioFileRegion_GetType (PlankAudioFileRegionRef p);
PlankUI pl_AudioFileRegion_GetOptions (PlankAudioFileRegionRef p);
double pl_AudioFileRegion_GetFraction (PlankAudioFileRegionRef p);
PlankUI pl_AudioFileRegion_GetPlayCount (PlankAudioFileRegionRef p);

const char* pl_AudioFileRegion_GetLabel (PlankAudioFileRegionRef p);
char* pl_AudioFileRegion_GetLabelWritable (PlankAudioFileRegionRef p);
const char* pl_AudioFileRegion_GetComment (PlankAudioFileRegionRef p);
char* pl_AudioFileRegion_GetCommentWritable (PlankAudioFileRegionRef p);

PlankResult pl_AudioFileRegion_SetAnchorPosition (PlankAudioFileRegionRef p, const double position);
PlankResult pl_AudioFileRegion_SetStartPosition (PlankAudioFileRegionRef p, const double position);
PlankResult pl_AudioFileRegion_SetEndPosition (PlankAudioFileRegionRef p, const double position);
PlankResult pl_AudioFileRegion_SetLength (PlankAudioFileRegionRef p, const double length);
PlankResult pl_AudioFileRegion_SetType (PlankAudioFileRegionRef p, const int type);
PlankResult pl_AudioFileRegion_SetOptions (PlankAudioFileRegionRef p, const PlankUI options);
PlankResult pl_AudioFileRegion_SetFraction (PlankAudioFileRegionRef p, const double fraction);
PlankResult pl_AudioFileRegion_SetPlayCount (PlankAudioFileRegionRef p, const PlankUI playCount);
PlankResult pl_AudioFileRegion_SetLabel (PlankAudioFileRegionRef p, const char* label);
PlankResult pl_AudioFileRegion_SetComment (PlankAudioFileRegionRef p, const char* comment);

PlankResult pl_AudioFileRegion_GetRegion (PlankAudioFileRegionRef p, double* start, double* end, double* anchor);
PlankResult pl_AudioFileRegion_SetRegion (PlankAudioFileRegionRef p, const double start, const double end);
PlankResult pl_AudioFileRegion_SetRegionWithAnchor (PlankAudioFileRegionRef p, const double start, const double end, const double anchor);

PlankResult pl_AudioFileRegion_OffsetPosition (PlankAudioFileRegionRef p, const double offset);


/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankAudioFileRegion
{
    PlankSharedPtr sharedHeader;
	PlankAudioFileCuePointRef anchor;
	PlankAudioFileCuePointRef start;
    PlankAudioFileCuePointRef end;
    int regionType;
    PlankUI regionOptions;
    double fraction;
    PlankUI playCount;
} PlankAudioFileRegion;
#endif

#endif // PLANK_AUDIOFILEREGION_H
