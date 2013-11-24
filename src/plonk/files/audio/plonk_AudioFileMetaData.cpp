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

AudioFileMetaData AudioFileMetaData::getNull() throw()
{
    static AudioFileMetaData null (Base::getNullSharedPtr());
    return null;
}

AudioFileMetaData::AudioFileMetaData() throw()
:   Base (Base::getNullSharedPtr())
{
    PlankAudioFileMetaDataRef newMeta = Base::getNullSharedPtr();
    pl_AudioFileMetaData_CreateAndInit (&newMeta);
    setInternal (newMeta);
    pl_AudioFileMetaData_DecrementRefCount (newMeta);
}

AudioFileMetaData::AudioFileMetaData (PlankAudioFileMetaDataRef p) throw()
:   Base (p)
{
}

AudioFileMetaData::AudioFileMetaData (AudioFileMetaData const& copy) throw()
:   Base (static_cast<Base const&> (copy))
{
}

AudioFileMetaData::AudioFileMetaData (Base const& base) throw()
:   Base (base)
{
}

AudioFileMetaData::AudioFileMetaData (Weak const& weak) throw()
:   Base (weak.fromWeak())
{
}

AudioFileMetaData& AudioFileMetaData::operator= (AudioFileMetaData const &other) throw()
{
    if (this != &other)
        this->setInternal (other.getInternal());
    
    return *this;
}

AudioFileCuePointArray AudioFileMetaData::getCuePoints() const throw()
{
    return AudioFileCuePointArray (pl_AudioFileMetaData_GetLoopPoints (getInternal()));
}

AudioFileRegionArray AudioFileMetaData::getRegions() const throw()
{
    return AudioFileRegionArray (pl_AudioFileMetaData_GetRegions (getInternal()));
}

AudioFileRegionArray AudioFileMetaData::getLoopPoints() const throw()
{
    return AudioFileRegionArray (pl_AudioFileMetaData_GetLoopPoints (getInternal()));
}

AudioFileMetaData& AudioFileMetaData::setEditCount (const UnsignedInt count) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetEditCount (getInternal(), count);
    else plonk_assertfalse;
    
    return *this;
}

UnsignedInt AudioFileMetaData::getEditCount() throw()
{
    
}


AudioFileMetaData& AudioFileMetaData::setInstrumentData (const int baseNote, const int detune, const int gain,
                                                         const int lowNote, const int highNote,
                                                         const int lowVelocity, const int highVelocity) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetInstrumentData (getInternal(), baseNote, detune, gain, lowNote, highNote, lowVelocity, highVelocity);
    else plonk_assertfalse;
    
    return *this;
}

void AudioFileMetaData::getInstrumentData (int& baseNote, int& detune, int& gain,
                                           int& lowNote, int& highNote,
                                           int& lowVelocity, int& highVelocity) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setSamplerData (const UnsignedInt manufacturer, const UnsignedInt product, const UnsignedInt samplePeriod,
                                                      const UnsignedInt smpteFormat, const UnsignedInt smpteOffset) throw()
{
    Internal const internal = getInternal();
    
    if (internal != Base::getNullSharedPtr())
        pl_AudioFileMetaData_SetSamplerData (getInternal(), manufacturer, product, samplePeriod, smpteFormat, smpteOffset);
    else plonk_assertfalse;
    
    return *this;
}

void AudioFileMetaData::getSamplerData (UnsignedInt& manufacturer, UnsignedInt& product, UnsignedInt& samplePeriod,
                                        UnsignedInt& smpteFormat, UnsignedInt& smpteOffset) throw()
{
    
}

//    PlankDynamicArrayRef getExtraSamplerData () throw();

AudioFileMetaData& AudioFileMetaData::setDescriptionComment (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setOriginatorArtist (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setOriginatorRef (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setOriginationDate (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setOriginationTime (const char* text) throw()
{
    
}

const char* AudioFileMetaData::getDescriptionComment() throw()
{
    
}

const char* AudioFileMetaData::getOriginatorArtist() throw()
{
    
}

const char* AudioFileMetaData::getOriginatorRef() throw()
{
    
}

const char* AudioFileMetaData::getOriginationDate() throw()
{
    
}

const char* AudioFileMetaData::getOriginationTime() throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setPerformer (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setTitle (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setAlbum (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setGenre (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setLyrics (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setVendor (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setISRC (const char* text) throw()
{
    
}

const char* AudioFileMetaData::getPerformer() throw()
{
    
}

const char* AudioFileMetaData::getTitle() throw()
{
    
}

const char* AudioFileMetaData::getAlbum() throw()
{
    
}

const char* AudioFileMetaData::getGenre() throw()
{
    
}

const char* AudioFileMetaData::getLyrics() throw()
{
    
}

const char* AudioFileMetaData::getVendor() throw()
{
    
}

const char* AudioFileMetaData::getISRC() throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::addPerformer (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::addDescriptionComment (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::addOriginatorArtist (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::addOriginatorRef (const char* text) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::addGenre (const char* text) throw()
{
    
}


//    AudioFileMetaData& setArt (PlankConstantP data, const PlankL size) throw();
//    PlankResult getArt (PlankConstantP* data, PlankL* size) throw();

AudioFileMetaData& AudioFileMetaData::setYear (const int year) throw()
{
    
}

int AudioFileMetaData::getYear() throw()
{
    
}


AudioFileMetaData& AudioFileMetaData::setTrackInfo (const int trackNum, const int trackTotal) throw()
{
    
}

void AudioFileMetaData::getTrackInfo (int& trackNum, int& trackTotal) throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setTimeRef (const LongLong timeRef) throw()
{
    
}

LongLong AudioFileMetaData::getTimeRef() throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setSource (const UnsignedInt source) throw()
{
    
}

UnsignedInt AudioFileMetaData::getSource() throw()
{
    
}

AudioFileMetaData& AudioFileMetaData::setUMID (const UnsignedChar* umid) throw()
{
    
}

const UnsignedChar* AudioFileMetaData::getUMID() throw()
{
    
}

PlankAudioFileMetaDataRef AudioFileMetaData::incrementRefCountAndGetPeer() throw()
{
    return pl_AudioFileMetaData_IncrementRefCountAndGet (this->getInternal());
}


END_PLONK_NAMESPACE
