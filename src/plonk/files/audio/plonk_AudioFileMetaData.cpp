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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

PLANKSHAREDPTRCONTAINER_DEFINE(AudioFileCuePoint)

AudioFileCuePoint::AudioFileCuePoint (const UnsignedInt cueID, const LongLong position,
                                      Text const& label, AudioFile::CuePointTypes type, Text const& comment) throw()
:   Base (Base::getNullSharedPtr())
{
    initInternalWithFunction (pl_AudioFileCuePoint_CreateSharedPtr);
    
    Internal const internal = getInternal();
    
    pl_AudioFileCuePoint_SetID (internal, cueID);
    pl_AudioFileCuePoint_SetPosition (internal, position);
    
    if (label.length() == 0)
        pl_AudioFileCuePoint_SetLabel (internal, label.getArray());
    
    pl_AudioFileCuePoint_SetType (internal, type);
    
    if (comment.length() == 0)
        pl_AudioFileCuePoint_SetComment (internal, comment.getArray());
}

AudioFileCuePoint& AudioFileCuePoint::setCopy (AudioFileCuePoint source) throw()
{
    Internal const internal = getInternal();
    Internal const sourceInternal = source.getInternal();
    
    if ((internal != Base::getNullSharedPtr()) &&
        (sourceInternal != Base::getNullSharedPtr()))
    {
        pl_AudioFileCuePoint_SetCopy(internal, sourceInternal);
    }
    else plonk_assertfalse;
    
    return *this;
}

AudioFileCuePoint& AudioFileCuePoint::setPosition (const LongLong position) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_SetPosition (internal, position);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileCuePoint& AudioFileCuePoint::setID (const UnsignedInt cueID) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_SetID (internal, cueID);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileCuePoint& AudioFileCuePoint::setLabel (const char* label) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_SetLabel (internal, label);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileCuePoint& AudioFileCuePoint::setComment (const char* comment) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_SetComment (internal, comment);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileCuePoint& AudioFileCuePoint::setType (const AudioFile::CuePointTypes type) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_SetType (internal, type);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileCuePoint& AudioFileCuePoint::setExtra (UnsignedInt purpose, UnsignedShort country, UnsignedShort language, UnsignedShort dialect, UnsignedShort codePage) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_SetExtra (internal, purpose, country, language, dialect, codePage);
    else plonk_assertfalse;
    
    return *this;
}

LongLong AudioFileCuePoint::getPosition() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileCuePoint_GetPosition (internal);
    else plonk_assertfalse;
    
    return -1;
}

UnsignedInt AudioFileCuePoint::getID() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileCuePoint_GetID (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileCuePoint::getLabel() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileCuePoint_GetLabel (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileCuePoint::getComment() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileCuePoint_GetComment (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFile::CuePointTypes AudioFileCuePoint::getType() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
    {
        switch (pl_AudioFileCuePoint_GetType (internal))
        {
            case AudioFile::CuePointCuePoint:       return AudioFile::CuePointCuePoint;
            case AudioFile::CuePointInsertPoint:    return AudioFile::CuePointInsertPoint;
            case AudioFile::CuePointPlayPosition:   return AudioFile::CuePointPlayPosition;
            case AudioFile::CuePointRegionStart:    return AudioFile::CuePointRegionStart;
            case AudioFile::CuePointRegionEnd:      return AudioFile::CuePointRegionEnd;
            default:                                return AudioFile::CuePointUnknown;
        }
    }
    else plonk_assertfalse;
    
    return AudioFile::CuePointUnknown;
}

AudioFileCuePoint::Extra* AudioFileCuePoint::getExtra() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return reinterpret_cast<AudioFileCuePoint::Extra*> (pl_AudioFileCuePoint_GetExtra (internal));
    else plonk_assertfalse;
    
    return static_cast<AudioFileCuePoint::Extra*> (0);
}

const AudioFileCuePoint::Extra* AudioFileCuePoint::getExtra() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return reinterpret_cast<const AudioFileCuePoint::Extra*> (pl_AudioFileCuePoint_GetExtra (internal));
    else plonk_assertfalse;
    
    return static_cast<const AudioFileCuePoint::Extra*> (0);
}

AudioFileCuePoint& AudioFileCuePoint::offsetPosition (const LongLong offset) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileCuePoint_OffsetPosition (internal, offset);
    else plonk_assertfalse;
    
    return *this;
}

PLANKSHAREDPTRCONTAINER_DEFINE(AudioFileRegion)

AudioFileRegion::AudioFileRegion (const UnsignedInt cueID, const LongLong start, const LongLong end, const LongLong anchor,
                                  Text const& label, AudioFile::RegionTypes type, Text const& comment) throw()
:   Base (Base::getNullSharedPtr())
{
    initInternalWithFunction (pl_AudioFileRegion_CreateSharedPtr);
    
    Internal const internal = getInternal();
    
    pl_AudioFileRegion_SetRegionWithAnchor (internal, start, end, anchor);
    
    if (label.length() > 0)
        pl_AudioFileRegion_SetLabel (internal, label.getArray());
    
    pl_AudioFileRegion_SetType (internal, type);
    
    if (comment.length() > 0)
        pl_AudioFileRegion_SetComment (internal, comment.getArray());
}

AudioFileCuePoint AudioFileRegion::getAnchorCuePoint() const throw()
{
    Internal const internal = getInternal();
    plonk_assert (internal != Base::getNullSharedPtr());
    return internal ? AudioFileCuePoint (pl_AudioFileRegion_GetAnchorCuePoint (internal)) : AudioFileCuePoint::getNull();
}

AudioFileCuePoint AudioFileRegion::getStartCuePoint() const throw()
{
    Internal const internal = getInternal();
    plonk_assert (internal != Base::getNullSharedPtr());
    return internal ? AudioFileCuePoint (pl_AudioFileRegion_GetStartCuePoint (internal)) : AudioFileCuePoint::getNull();
}

AudioFileCuePoint AudioFileRegion::getEndCuePoint() const throw()
{
    Internal const internal = getInternal();
    plonk_assert (internal != Base::getNullSharedPtr());
    return internal ? AudioFileCuePoint (pl_AudioFileRegion_GetEndCuePoint (internal)) : AudioFileCuePoint::getNull();
}

LongLong AudioFileRegion::getAnchorPosition() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetAnchorPosition (internal);
    else plonk_assertfalse;
    
    return 0;
}

LongLong AudioFileRegion::getStartPosition() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetStartPosition (internal);
    else plonk_assertfalse;
    
    return 0;
}

LongLong AudioFileRegion::getEndPosition() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetEndPosition (internal);
    else plonk_assertfalse;
    
    return 0;
}

LongLong AudioFileRegion::getLength() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetLength (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFile::RegionTypes AudioFileRegion::getType() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
    {
        switch (pl_AudioFileRegion_GetType (internal))
        {
            case AudioFile::RegionRegion:       return AudioFile::RegionRegion;
            case AudioFile::RegionSelection:    return AudioFile::RegionSelection;
            case AudioFile::RegionLoop:         return AudioFile::RegionLoop;
            case AudioFile::RegionLyrics:       return AudioFile::RegionLyrics;
            default:                            return AudioFile::RegionUnknown;
        }
    }
    else plonk_assertfalse;
    
    return AudioFile::RegionUnknown;
}

UnsignedInt AudioFileRegion::getOptions() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetOptions (internal);
    else plonk_assertfalse;
    
    return 0;
}

UnsignedInt AudioFileRegion::getFraction() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetFraction (internal);
    else plonk_assertfalse;
    
    return 0;
}

UnsignedInt AudioFileRegion::getPlayCount() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetPlayCount (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileRegion::getLabel() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetLabel (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileRegion::getComment() const throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileRegion_GetComment (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFileRegion& AudioFileRegion::setAnchorPosition (const LongLong position) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetAnchorPosition (internal, position);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setStartPosition (const LongLong position) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetAnchorPosition (internal, position);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setEndPosition (const LongLong position) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetAnchorPosition (internal, position);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setLength (const LongLong length) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetLength (internal, length);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setType (const AudioFile::RegionTypes type) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetType (internal, type);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setOptions (const UnsignedInt options) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetOptions (internal, options);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setFraction (const UnsignedInt fraction) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetFraction (internal, fraction);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setPlayCount (const UnsignedInt playCount) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetPlayCount (internal, playCount);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setLabel (const char* label) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetLabel (internal, label);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setComment (const char* comment) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetComment (internal, comment);
    else plonk_assertfalse;
    
    return *this;
}

void AudioFileRegion::getRegion (LongLong& start, LongLong& end, LongLong& anchor) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_GetRegion (internal, &start, &end, &anchor);
    else plonk_assertfalse;    
}

AudioFileRegion& AudioFileRegion::setRegion (const LongLong start, const LongLong end) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetRegion (internal, start, end);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::setRegionWithAnchor (const LongLong start, const LongLong end, const LongLong anchor) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_SetRegionWithAnchor (internal, start, end, anchor);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileRegion& AudioFileRegion::offsetPosition (const LongLong offset) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileRegion_OffsetPosition (internal, offset);
    else plonk_assertfalse;
    
    return *this;
}


PLANKSHAREDPTRCONTAINER_DEFINE(AudioFileMetaData)

AudioFileCuePointArray AudioFileMetaData::getCuePoints() const throw()
{
    Internal const internal = getInternal();
    plonk_assert (internal != Base::getNullSharedPtr());
    return internal ? AudioFileCuePointArray (pl_AudioFileMetaData_GetCuePoints (internal)) : AudioFileCuePointArray::getNull();
}

AudioFileRegionArray AudioFileMetaData::getRegions() const throw()
{
    Internal const internal = getInternal();
    plonk_assert (internal != Base::getNullSharedPtr());
    return internal ? AudioFileRegionArray (pl_AudioFileMetaData_GetRegions (internal)) : AudioFileRegionArray::getNull();
}

AudioFileRegionArray AudioFileMetaData::getLoopPoints() const throw()
{
    Internal const internal = getInternal();
    plonk_assert (internal != Base::getNullSharedPtr());
    return internal ? AudioFileRegionArray (pl_AudioFileMetaData_GetLoopPoints (internal)) : AudioFileRegionArray::getNull();
}

AudioFileMetaData& AudioFileMetaData::sortCuePointsByPosition() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SortCuePointsByPosition (internal);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::sortCuePointsByID() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SortCuePointsByID (internal);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setEditCount (const UnsignedInt count) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetEditCount (internal, count);
    else plonk_assertfalse;
    
    return *this;
}

UnsignedInt AudioFileMetaData::getEditCount() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetEditCount (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFileMetaData& AudioFileMetaData::setInstrumentData (const int baseNote, const int detune, const int gain,
                                                         const int lowNote, const int highNote,
                                                         const int lowVelocity, const int highVelocity) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetInstrumentData (internal, baseNote, detune, gain, lowNote, highNote, lowVelocity, highVelocity);
    else plonk_assertfalse;
    
    return *this;
}

void AudioFileMetaData::getInstrumentData (int& baseNote, int& detune, int& gain,
                                           int& lowNote, int& highNote,
                                           int& lowVelocity, int& highVelocity) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_GetInstrumentData (internal, &baseNote, &detune, &gain, &lowNote, &highNote, &lowVelocity, &highVelocity);
    else plonk_assertfalse;
}

AudioFileMetaData& AudioFileMetaData::setSamplerData (const UnsignedInt manufacturer, const UnsignedInt product, const UnsignedInt samplePeriod,
                                                      const UnsignedInt smpteFormat, const UnsignedInt smpteOffset) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetSamplerData (internal, manufacturer, product, samplePeriod, smpteFormat, smpteOffset);
    else plonk_assertfalse;
    
    return *this;
}

void AudioFileMetaData::getSamplerData (UnsignedInt& manufacturer, UnsignedInt& product, UnsignedInt& samplePeriod,
                                        UnsignedInt& smpteFormat, UnsignedInt& smpteOffset) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_GetSamplerData (internal, &manufacturer, &product, &samplePeriod, &smpteFormat, &smpteOffset);
    else plonk_assertfalse;
}

//    PlankDynamicArrayRef getExtraSamplerData () throw();

AudioFileMetaData& AudioFileMetaData::setDescriptionComment (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetDescriptionComment (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setOriginatorArtist (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetOriginatorArtist (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setOriginatorRef (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetOriginatorRef (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setOriginationDate (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetOriginationDate (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setOriginationTime (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetOriginationTime (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

const char* AudioFileMetaData::getDescriptionComment() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetDescriptionComment (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getOriginatorArtist() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetOriginatorArtist (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getOriginatorRef() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetOriginatorRef (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getOriginationDate() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetOriginationDate (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getOriginationTime() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetOriginationTime (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFileMetaData& AudioFileMetaData::setPerformer (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetPerformer (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setTitle (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetTitle (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setAlbum (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetAlbum (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setGenre (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetGenre (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setLyrics (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetLyrics (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setVendor (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetVendor (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::setISRC (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetISRC (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

const char* AudioFileMetaData::getPerformer() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetPerformer (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getTitle() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetTitle (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getAlbum() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetAlbum (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getGenre() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetGenre (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getLyrics() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetLyrics (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getVendor() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetVendor (internal);
    else plonk_assertfalse;
    
    return 0;
}

const char* AudioFileMetaData::getISRC() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetISRC (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFileMetaData& AudioFileMetaData::addPerformer (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_AddPerformer (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::addDescriptionComment (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_AddDescriptionComment (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::addOriginatorArtist (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_AddOriginatorArtist (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::addOriginatorRef (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_AddOriginatorRef (internal, text);
    else plonk_assertfalse;
    
    return *this;
}

AudioFileMetaData& AudioFileMetaData::addGenre (const char* text) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_AddGenre (internal, text);
    else plonk_assertfalse;
    
    return *this;
}


//    AudioFileMetaData& setArt (PlankConstantP data, const PlankL size) throw();
//    PlankResult getArt (PlankConstantP* data, PlankL* size) throw();

AudioFileMetaData& AudioFileMetaData::setYear (const int year) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetYear (internal, year);
    else plonk_assertfalse;
    
    return *this;
}

int AudioFileMetaData::getYear() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetYear (internal);
    else plonk_assertfalse;
    
    return 0;
}


AudioFileMetaData& AudioFileMetaData::setTrackInfo (const int trackNum, const int trackTotal) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetTrackInfo (internal, trackNum, trackTotal);
    else plonk_assertfalse;
    
    return *this;
}

void AudioFileMetaData::getTrackInfo (int& trackNum, int& trackTotal) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
         pl_AudioFileMetaData_GetTrackInfo (internal, &trackNum, &trackTotal);
    else plonk_assertfalse;
}

AudioFileMetaData& AudioFileMetaData::setTimeRef (const LongLong timeRef) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetTimeRef (internal, timeRef);
    else plonk_assertfalse;
    
    return *this;
}

LongLong AudioFileMetaData::getTimeRef() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetTimeRef (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFileMetaData& AudioFileMetaData::setSource (const UnsignedInt source) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetSource (internal, source);
    else plonk_assertfalse;
    
    return *this;
}

UnsignedInt AudioFileMetaData::getSource() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetSource (internal);
    else plonk_assertfalse;
    
    return 0;
}

AudioFileMetaData& AudioFileMetaData::setUMID (const UnsignedChar* umid) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetUMID (internal, umid);
    else plonk_assertfalse;
    
    return *this;
}

const UnsignedChar* AudioFileMetaData::getUMID() throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        return pl_AudioFileMetaData_GetUMID (internal);
    else plonk_assertfalse;
    
    return 0;
}



END_PLONK_NAMESPACE
