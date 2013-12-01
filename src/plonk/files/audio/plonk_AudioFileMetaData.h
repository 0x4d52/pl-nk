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

#ifndef PLONK_AUDIOFILEMETADATA_H
#define PLONK_AUDIOFILEMETADATA_H

#include "plonk_AudioFile.h"
#include "../../containers/plonk_Text.h"

class AudioFileCuePoint : public PlankSharedPtrContainer<PlankAudioFileCuePointRef>
{
public:
    typedef PlankAudioFileCuePointExtra Extra;
    typedef PlankSharedPtrContainer<PlankAudioFileCuePointRef> Base;
    typedef Base::Weak Weak;
    static AudioFileCuePoint getNull() throw();
    AudioFileCuePoint() throw();
    AudioFileCuePoint (PlankAudioFileCuePointRef p) throw();
    AudioFileCuePoint (AudioFileCuePoint const& copy) throw();
    AudioFileCuePoint (Base const& base) throw();
    AudioFileCuePoint (Weak const& weak) throw();
    AudioFileCuePoint& operator= (AudioFileCuePoint const &other) throw();
    
    AudioFileCuePoint (const UnsignedInt cueID, const double position,
                       Text const& label = Text::getEmpty(),
                       AudioFile::CuePointTypes type = AudioFile::CuePointCuePoint,
                       Text const& comment = Text::getEmpty()) throw();
    
    AudioFileCuePoint& setCopy (AudioFileCuePoint source) throw();
    AudioFileCuePoint& setPosition (const double position) throw();
    AudioFileCuePoint& setID (const UnsignedInt cueID) throw();
    AudioFileCuePoint& setLabel (const char* label) throw();
    AudioFileCuePoint& setComment (const char* comment) throw();
    AudioFileCuePoint& setType (const AudioFile::CuePointTypes type) throw();
    AudioFileCuePoint& setExtra (UnsignedInt purpose, UnsignedShort country, UnsignedShort language, UnsignedShort dialect, UnsignedShort codePage) throw();
    
    double getPosition() const throw();
    UnsignedInt getID() const throw();
    const char* getLabel() const throw();
    const char* getComment() const throw();
    AudioFile::CuePointTypes getType() const throw();
    AudioFileCuePoint::Extra* getExtra() throw();
    const AudioFileCuePoint::Extra* getExtra() const throw();
    
    AudioFileCuePoint& offsetPosition (const double offset) throw();

private:
    PlankAudioFileCuePointRef incrementRefCountAndGetPeer() throw();
};

class AudioFileRegion : public PlankSharedPtrContainer<PlankAudioFileRegionRef>
{
public:
    typedef PlankSharedPtrContainer<PlankAudioFileRegionRef> Base;
    typedef Base::Weak Weak;
    static AudioFileRegion getNull() throw();
    AudioFileRegion() throw();
    AudioFileRegion (PlankAudioFileRegionRef p) throw();
    AudioFileRegion (AudioFileRegion const& copy) throw();
    AudioFileRegion (Base const& base) throw();
    AudioFileRegion (Weak const& weak) throw();
    AudioFileRegion& operator= (AudioFileRegion const &other) throw();

    AudioFileRegion (const UnsignedInt cueID, const double start, const double end, const double anchor,
                     Text const& label = Text::getEmpty(),
                     AudioFile::RegionTypes type = AudioFile::RegionRegion,
                     Text const& comment = Text::getEmpty()) throw();
    
    AudioFileCuePoint getAnchorCuePoint() const throw();
    AudioFileCuePoint getStartCuePoint() const throw();
    AudioFileCuePoint getEndCuePoint() const throw();
    double getAnchorPosition() const throw();
    double getStartPosition() const throw();
    double getEndPosition() const throw();
    double getLength() const throw();
    AudioFile::RegionTypes getType() const throw();
    UnsignedInt getOptions() const throw();
    UnsignedInt getFraction() const throw();
    UnsignedInt getPlayCount() const throw();
    const char* getLabel() const throw();
    const char* getComment() const throw();
    AudioFileRegion& setAnchorPosition (const double position) throw();
    AudioFileRegion& setStartPosition (const double position) throw();
    AudioFileRegion& setEndPosition (const double position) throw();
    AudioFileRegion& setLength (const double length) throw();
    AudioFileRegion& setType (const AudioFile::RegionTypes type) throw();
    AudioFileRegion& setOptions (const UnsignedInt options) throw();
    AudioFileRegion& setFraction (const double fraction) throw();
    AudioFileRegion& setPlayCount (const UnsignedInt playCount) throw();
    AudioFileRegion& setLabel (const char* label) throw();
    AudioFileRegion& setComment (const char* comment) throw();
    void getRegion (double& start, double& end, double& anchor) throw();
    AudioFileRegion& setRegion (const double start, const double end) throw();
    AudioFileRegion& setRegionWithAnchor (const double start, const double end, const double anchor) throw();
    AudioFileRegion& offsetPosition (const double offset) throw();

private:
    PlankAudioFileRegionRef incrementRefCountAndGetPeer() throw();
};

typedef PlankSharedPtrArrayContainer<AudioFileCuePoint> AudioFileCuePointArray;
typedef PlankSharedPtrArrayContainer<AudioFileRegion>   AudioFileRegionArray;

class AudioFileMetaData : public PlankSharedPtrContainer<PlankAudioFileMetaDataRef>
{
public:
    typedef PlankSharedPtrContainer<PlankAudioFileMetaDataRef> Base;
    typedef Base::Weak Weak;
    static AudioFileMetaData getNull() throw();
    AudioFileMetaData() throw();
    AudioFileMetaData (PlankAudioFileMetaDataRef p) throw();
    AudioFileMetaData (AudioFileMetaData const& copy) throw();
    AudioFileMetaData (Base const& base) throw();
    AudioFileMetaData (Weak const& weak) throw();
    AudioFileMetaData& operator= (AudioFileMetaData const &other) throw();

    AudioFileMetaData& setEditCount (const UnsignedInt count) throw();
    UnsignedInt getEditCount() throw();
    
    AudioFileMetaData& setInstrumentData (const int baseNote, const int detune, const int gain, const int lowNote, const int highNote, const int lowVelocity, const int highVelocity) throw();
    void getInstrumentData (int& baseNote, int& detune, int& gain, int& lowNote, int& highNote, int& lowVelocity, int& highVelocity) throw();
    AudioFileMetaData& setSamplerData (const UnsignedInt manufacturer, const UnsignedInt product, const UnsignedInt samplePeriod, const UnsignedInt smpteFormat, const UnsignedInt smpteOffset) throw();
    void getSamplerData (UnsignedInt& manufacturer, UnsignedInt& product, UnsignedInt& samplePeriod, UnsignedInt& smpteFormat, UnsignedInt& smpteOffset) throw();
    
//    PlankDynamicArrayRef getExtraSamplerData () throw();
    
    AudioFileMetaData& setDescriptionComment (const char* text) throw();
    AudioFileMetaData& setOriginatorArtist (const char* text) throw();
    AudioFileMetaData& setOriginatorRef (const char* text) throw();
    AudioFileMetaData& setOriginationDate (const char* text) throw();
    AudioFileMetaData& setOriginationTime (const char* text) throw();
    
    const char* getDescriptionComment() throw();
    const char* getOriginatorArtist() throw();
    const char* getOriginatorRef() throw();
    const char* getOriginationDate() throw();
    const char* getOriginationTime() throw();
    
    AudioFileMetaData& setPerformer (const char* text) throw();
    AudioFileMetaData& setTitle (const char* text) throw();
    AudioFileMetaData& setAlbum (const char* text) throw();
    AudioFileMetaData& setGenre (const char* text) throw();
    AudioFileMetaData& setLyrics (const char* text) throw();
    AudioFileMetaData& setVendor (const char* text) throw();
    AudioFileMetaData& setISRC (const char* text) throw();
    
    const char* getPerformer() throw();
    const char* getTitle() throw();
    const char* getAlbum() throw();
    const char* getGenre() throw();
    const char* getLyrics() throw();
    const char* getVendor() throw();
    const char* getISRC() throw();
    
    AudioFileMetaData& addPerformer (const char* text) throw();
    AudioFileMetaData& addDescriptionComment (const char* text) throw();
    AudioFileMetaData& addOriginatorArtist (const char* text) throw();
    AudioFileMetaData& addOriginatorRef (const char* text) throw();
    AudioFileMetaData& addGenre (const char* text) throw();
    
//    AudioFileMetaData& setArt (PlankConstantP data, const PlankL size) throw();
//    PlankResult getArt (PlankConstantP* data, PlankL* size) throw();
    
    AudioFileMetaData& setYear (const int year) throw();
    int getYear() throw();
    
    AudioFileMetaData& setTrackInfo (const int trackNum, const int trackTotal) throw();
    void getTrackInfo (int& trackNum, int& trackTotal) throw();
    
    AudioFileMetaData& setTimeRef (const LongLong timeRef) throw();
    LongLong getTimeRef() throw();
    
    AudioFileMetaData& setSource (const UnsignedInt source) throw();
    UnsignedInt getSource() throw();
    
    AudioFileMetaData& setUMID (const UnsignedChar* umid) throw();
    const UnsignedChar* getUMID() throw();
    
    AudioFileCuePointArray getCuePoints() const throw();
    AudioFileRegionArray getRegions() const throw();
    AudioFileRegionArray getLoopPoints() const throw();
    
    AudioFileMetaData& convertCuePointsToRegions (const double totalDuration, const bool removeCuePoints) throw();
    AudioFileMetaData& sortCuePointsByPosition() throw();
    AudioFileMetaData& sortCuePointsByID() throw();
    
    friend class AudioFileReaderInternal;
    template<class SampleType> friend class AudioFileWriterInternal;
    
private:
    PlankAudioFileMetaDataRef incrementRefCountAndGetPeer() throw();
};


#endif // PLONK_AUDIOFILEMETADATA_H
