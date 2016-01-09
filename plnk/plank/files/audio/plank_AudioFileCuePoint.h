/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLANK_AUDIOFILECUEPOINT_H
#define PLANK_AUDIOFILECUEPOINT_H

#include "plank_AudioFileCommon.h"
#include "../../containers/plank_SharedPtr.h"
#include "../../containers/plank_DynamicArray.h"

#define PLANKSHAREDPTR_TYPE_AUDIOFILECUEPOINT pl_FourCharCode("cuep")

PLANK_BEGIN_C_LINKAGE

/** An audio file cue point reference.
  
 @defgroup PlankAudioFileCuePointClass Plank AudioFileCuePoint class
 @ingroup PlankClasses
 @{
 */

/** Create a <i>Plank AudioFileCuePoint</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileCuePoint</i> object as an opaque reference or PLANK_NULL. */
PlankResult pl_AudioFileCuePoint_CreateSharedPtr (PlankAudioFileCuePointRef* pp);
PlankAudioFileCuePointRef pl_AudioFileCuePoint_IncrementRefCountAndGet (PlankAudioFileCuePointRef p);
PlankResult pl_AudioFileCuePoint_DecrementRefCount (PlankAudioFileCuePointRef p);

PlankResult pl_AudioFileCuePoint_SetCopy                (PlankAudioFileCuePointRef p, PlankAudioFileCuePointRef source);
PlankResult pl_AudioFileCuePoint_SetPosition            (PlankAudioFileCuePointRef p, const double position);
PlankResult pl_AudioFileCuePoint_SetID                  (PlankAudioFileCuePointRef p, const PlankUI cueID);
PlankResult pl_AudioFileCuePoint_SetLabel               (PlankAudioFileCuePointRef p, const char* label);
PlankResult pl_AudioFileCuePoint_SetLabelLengthClear    (PlankAudioFileCuePointRef p, const PlankL length);
PlankResult pl_AudioFileCuePoint_SetComment             (PlankAudioFileCuePointRef p, const char* comment);
PlankResult pl_AudioFileCuePoint_SetCommentLengthClear  (PlankAudioFileCuePointRef p, const PlankL length);
PlankResult pl_AudioFileCuePoint_SetType                (PlankAudioFileCuePointRef p, const int type);
PlankResult pl_AudioFileCuePoint_SetExtra               (PlankAudioFileCuePointRef p, PlankUI purpose, PlankUS country, PlankUS language, PlankUS dialect, PlankUS codePage);

double pl_AudioFileCuePoint_GetPosition (PlankAudioFileCuePointRef p);
PlankUI pl_AudioFileCuePoint_GetID (PlankAudioFileCuePointRef p);
const char* pl_AudioFileCuePoint_GetLabel (PlankAudioFileCuePointRef p);
char* pl_AudioFileCuePoint_GetLabelWritable (PlankAudioFileCuePointRef p);
const char* pl_AudioFileCuePoint_GetComment (PlankAudioFileCuePointRef p);
char* pl_AudioFileCuePoint_GetCommentWritable (PlankAudioFileCuePointRef p);
int pl_AudioFileCuePoint_GetType (PlankAudioFileCuePointRef p);
PlankUI pl_AudioFileCuePoint_GetLabelSize (PlankAudioFileCuePointRef p);
PlankUI pl_AudioFileCuePoint_GetCommentSize (PlankAudioFileCuePointRef p);
PlankAudioFileCuePointExtraRef pl_AudioFileCuePoint_GetExtra (PlankAudioFileCuePointRef p);
PlankResult pl_AudioFileCuePoint_OffsetPosition (PlankAudioFileCuePointRef p, const double offset);

PlankB pl_AudioFileCuePoint_ComparePosition (PlankAudioFileCuePointRef low, PlankAudioFileCuePointRef hi);
PlankB pl_AudioFileCuePoint_CompareID (PlankAudioFileCuePointRef low, PlankAudioFileCuePointRef hi);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankAudioFileCuePointExtra
{
    PlankUI purpose;
    PlankUS country;
    PlankUS language;
    PlankUS dialect;
    PlankUS codePage;
} PlankAudioFileCuePointExtra;

typedef struct PlankAudioFileCuePoint
{
    PlankSharedPtr sharedHeader;
	double position;
    PlankDynamicArray label;
    PlankDynamicArray comment;
    PlankAudioFileCuePointExtra* extra;
    PlankUI cueID;
    int type;
} PlankAudioFileCuePoint;
#endif

#endif // PLANK_AUDIOFILECUEPOINT_H
