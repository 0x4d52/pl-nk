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

#ifndef PLANK_AUDIOFILEMETADATA_H
#define PLANK_AUDIOFILEMETADATA_H

#include "plank_AudioFileCommon.h"
#include "../../containers/plank_DynamicArray.h"
#include "../../containers/plank_SimpleLinkedList.h"

PLANK_BEGIN_C_LINKAGE

/** An audio file meta data container.
  
 @defgroup PlankAudioFileMetaDataClass Plank AudioFileMetaData class
 @ingroup PlankClasses
 @{
 */


/** Create a <i>Plank AudioFileMetaData</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileMetaData</i> object as an opaque reference or PLANK_NULL. */
PlankAudioFileMetaDataRef pl_AudioFileMetaData_Create();

PlankAudioFileMetaDataRef pl_AudioFileMetaData_CreateAndInit();


/** Destroy a <i>Plank AudioFileMetaData</i> object. 
 @param p The <i>Plank AudioFileMetaData</i> object. */
PlankResult pl_AudioFileMetaData_Destroy (PlankAudioFileMetaDataRef p);

PlankResult pl_AudioFileMetaData_Init (PlankAudioFileMetaDataRef p);

PlankResult pl_AudioFileMetaData_DeInit (PlankAudioFileMetaDataRef p);

/** Adds a cue point.
 Copies the data from another cue point. The source cue point is zeroed on return to help avoid memory allocation/deallocation issues. */
PlankResult pl_AudioFileMetaData_AddCuePoint (PlankAudioFileMetaDataRef p, PlankAudioFileCuePointRef cuePoint);
PlankResult pl_AudioFileMetaData_FindCuePointWithID (PlankAudioFileMetaDataRef p, const PlankUI cueID, PlankAudioFileCuePointRef* cuePoint, PlankL* index);
PlankResult pl_AudioFileMetaData_RemoveCuePointWithID (PlankAudioFileMetaDataRef p, const PlankUI cueID, PlankAudioFileCuePointRef cuePoint, PlankB* success);

/** Adds a region.
 Copies the data from another region. The source region is zeroed on return to help avoid memory allocation/deallocation issues. */
PlankResult pl_AudioFileMetaData_AddRegion (PlankAudioFileMetaDataRef p, PlankAudioFileRegionRef region);

PlankResult pl_AudioFileMetaData_AddCodingHistory (PlankAudioFileMetaDataRef p, const char* text);


/** Adds a format specific block of data.
 The AudioFileMetaData object takes ownership of the block. */
PlankResult pl_AudioFileMetaData_AddFormatSpecificBlock (PlankAudioFileMetaDataRef p, PlankDynamicArrayRef block);

/** @} */

PLANK_END_C_LINKAGE


#if !DOXYGEN
//typedef struct PlankAudioFileMetaDataParseState
//{
//    PlankUI cueLabelIndex;
//    PlankUI cueNoteIndex;
//} PlankAudioFileMetaDataParseState;


typedef struct PlankAudioFileMetaData
{
    // smpl / inst
    PlankI midiUnityNote;
    PlankUI midiPitchFraction; // detune
    PlankUI gain;
    PlankI lowNote;
    PlankI highNote;
    PlankI lowVelocity;
    PlankI highVelocity;
    
    PlankUI manufacturer;
    PlankUI product;
    PlankUI samplePeriod;
    PlankUI smpteFormat;
    PlankUI smpteOffset;
    
    PlankUI textEncoding;

    PlankDynamicArray descriptionComments; // an array of comments to allow for Ogg comments
    PlankDynamicArray originatorArtist;
    PlankDynamicArray originatorRef;
    PlankDynamicArray originationDate;
    PlankDynamicArray originationTime;
    
    PlankDynamicArray title;
    PlankDynamicArray album;
    PlankDynamicArray genre;
    PlankDynamicArray lyrics;
    
    PlankDynamicArray art;
    
    PlankI year;
    PlankI trackNum;
    PlankI trackTotal;
    
    PlankLL timeRef;
    PlankUS version;
    PlankDynamicArray umid; //?? bwav
    PlankDynamicArray codingHistory;

	PlankDynamicArray cuePoints;
    PlankDynamicArray loopPoints;
    PlankDynamicArray regions;
    
    // just a list of unparsed data - if we write the same format
    // wih no change this can be simply piped out
    PlankSimpleLinkedList formatSpecific;
    
//    PlankAudioFileMetaDataParseState parseState;
    
} PlankAudioFileMetaData;
#endif 

#endif // PLANK_AUDIOFILEMETADATA_H
