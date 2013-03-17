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

#ifndef PLONK_AUDIOFILEWRITER_H
#define PLONK_AUDIOFILEWRITER_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../core/plonk_SmartPointer.h"
#include "../../core/plonk_WeakPointer.h"
#include "../../core/plonk_SmartPointerContainer.h"
#include "../../containers/plonk_Text.h"
#include "plonk_AudioFile.h"

template<class SampleType>
class AudioFileWriterInternalBase : public SmartPointer
{
private:
    AudioFileWriterInternalBase(); // incompatible SampleType
};

template<> class AudioFileWriterInternalBase<Char>    : public SmartPointer { public: AudioFileWriterInternalBase<Char>()   : isFloat (false) { } protected: const bool isFloat; };
template<> class AudioFileWriterInternalBase<Short>   : public SmartPointer { public: AudioFileWriterInternalBase<Short>()  : isFloat (false) { } protected: const bool isFloat; };
template<> class AudioFileWriterInternalBase<Int24>   : public SmartPointer { public: AudioFileWriterInternalBase<Int24>()  : isFloat (false) { } protected: const bool isFloat; };
template<> class AudioFileWriterInternalBase<Int>     : public SmartPointer { public: AudioFileWriterInternalBase<Int>()    : isFloat (false) { } protected: const bool isFloat; };
template<> class AudioFileWriterInternalBase<Float>   : public SmartPointer { public: AudioFileWriterInternalBase<Float>()  : isFloat (true)  { } protected: const bool isFloat; };
template<> class AudioFileWriterInternalBase<Double>  : public SmartPointer { public: AudioFileWriterInternalBase<Double>() : isFloat (true)  { } protected: const bool isFloat; };

template<class SampleType>
class AudioFileWriterInternal : public AudioFileWriterInternalBase<SampleType>
{
public:
    typedef NumericalArray<SampleType>  Buffer;
    
    AudioFileWriterInternal (FilePath const& path, const int numChannels, const double sampleRate, const int bufferSize) throw()
    :   buffer (Buffer::withSize (numChannels * bufferSize, false))
    {
        pl_AudioFileWriter_Init (&peer);

        const Text ext = path.extension();
        
        if (ext.equalsIgnoreCase ("wav"))
        {
            pl_AudioFileWriter_SetFormatWAV (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, this->isFloat);
            pl_AudioFileWriter_Open (&peer, path.fullpath().getArray());
        }
    }
    
    ~AudioFileWriterInternal()
    {
        pl_AudioFileWriter_DeInit (&peer);
    }
    
    void writeFrames (const int numFrames, const SampleType* frameData) throw()
    {
        pl_AudioFileWriter_WriteFrames (&peer, numFrames, frameData);
    }
        
    void writeFrames (Buffer const& frames) throw()
    {
        const int numChannels = peer.formatInfo.numChannels;
        plonk_assert ((frames.length() % numChannels) == 0);
        pl_AudioFileWriter_WriteFrames (&peer, frames.length() / numChannels, frames.getArray());
    }
        
    template<class OtherType>
    void writeFrames (NumericalArray<OtherType> const& frames) throw()
    {
        const int numChannels = peer.formatInfo.numChannels;

        plonk_assert ((frames.length() % numChannels) == 0);

        SampleType* const nativeSamples = buffer.getArray();
        const OtherType* sourceSamples = frames.getArray();
        const int nativeSamplesLength = buffer.length();
        
        int numSamplesRemainaing = frames.length();
        
        while (numSamplesRemainaing > 0)
        {
            const int numSamplesThisTime = plonk::min (nativeSamplesLength, numSamplesRemainaing);
            NumericalArrayConverter<SampleType, OtherType>::convertScaled (nativeSamples, sourceSamples, numSamplesThisTime);
            pl_AudioFileWriter_WriteFrames (&peer, numSamplesThisTime / numChannels, nativeSamples);
            numSamplesRemainaing -= numSamplesThisTime;
            sourceSamples += numSamplesThisTime;
        }
    }

//    template<class OtherType>
//    void writeFrames (NumericalArray2D<OtherType> const& frames) throw()
//    {
//        plonk_assert (frames.isMatrix());
//        
//        const int numChannels = peer.formatInfo.numChannels;
//        plonk_assert (frames.length() == numChannels);  // could be cleverer with channel mapping
//        
//        const int nativeSamplesLength = buffer.length() >> 1; // need half for deinterleaving
//        const int numNativeFrames = (nativeSamplesLength / numChannels);
//        
//        int numFramesRemainaing = frames.atUnchecked (0).length();
//        
//        while (numFramesRemainaing > 0)
//        {
//            const int numFramesThisTime = plonk::min (numNativeFrames, numFramesRemainaing);
//
//            for (int channel = 0; channel < numChannels; ++channel)
//            {
//                SampleType* const nativeSamples = buffer.getArray() + channel;
//                const OtherType* sourceSamples = frames.atUnchecked (channel).getArray();
//
//                for (int i = 0; i < numFramesThisTime; ++i)
//                    
//
//            }
//        }
//    }

    
private:
    PlankAudioFileWriter peer;
    Buffer buffer;
};


template<class SampleType>
class AudioFileWriter: public SmartPointerContainer< AudioFileWriterInternal<SampleType> >
{
public:
    typedef AudioFileWriterInternal<SampleType> Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef NumericalArray<SampleType>          Buffer;

    AudioFileWriter (FilePath const& path, const int numChannels, const double sampleRate, const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (new Internal (path, numChannels, sampleRate, bufferSize))
	{
	}

    void writeFrames (const int numFrames, const SampleType* frameData) throw()
    {
        this->getInternal()->writeFrames (numFrames, frameData);
    }

    void writeFrames (Buffer const& frames) throw()
    {
        this->getInternal()->writeFrames (frames);
    }
    
    template<class OtherType>
    void writeFrames (NumericalArray<OtherType> const& frames) throw()
    {
        this->getInternal()->writeFrames (frames);
    }

private:
    PlankAudioFileWriter peer;
};

#endif // PLONK_AUDIOFILEWRITER_H
