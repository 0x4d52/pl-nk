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
#include "../plonk_FilesForwardDeclarations.h"
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
    
    AudioFileWriterInternal (const int bufferSize) throw()
    :   buffer (Buffer::withSize (bufferSize > 0 ? bufferSize : AudioFile::DefaultBufferSize, false)),
        ready (false)
    {
        pl_AudioFileWriter_Init (&peer);
    }
    
    bool openPath (FilePath const& path)
    {
        return pl_AudioFileWriter_Open (&peer, path.fullpath().getArray()) == PlankResult_OK;
    }
    
    bool openFile (PlankFileRef file)
    {
        return pl_AudioFileWriter_OpenWithFile (&peer, file) == PlankResult_OK;
    }
    
    bool initBytes (PlankFileRef file, ByteArray const& bytes) throw()
    {
        return BinaryFileInternal::setupBytes (file, bytes, true);
    }
    
    static AudioFileWriterInternal* createPCM (FilePath const& path, const int numChannels, const double sampleRate, const int bufferSize) throw()
    {
        AudioFileWriterInternal* internal = new AudioFileWriterInternal (numChannels * bufferSize);
        const Text ext = path.extension();
        
        AudioFile::Format format = AudioFile::FormatInvalid;
        
        if (ext.equalsIgnoreCase ("wav"))
        {
            format = AudioFile::FormatWAV;
        }
        else if (ext.equalsIgnoreCase ("aif"))
        {
            format = AudioFile::FormatAIFF;
        }
        else if (ext.equalsIgnoreCase ("aiff") || ext.equalsIgnoreCase ("aifc"))
        {
            format = AudioFile::FormatAIFC;
        }
        else if (ext.equalsIgnoreCase ("caf"))
        {
            format = AudioFile::FormatCAF;
        }
        else if (ext.equalsIgnoreCase ("w64"))
        {
            format = AudioFile::FormatW64;
        }
            
        if (!internal->initPCM (format, numChannels, sampleRate, bufferSize))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->openPath (path))
        {
            pl_AudioFileWriter_Close (&internal->peer);
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        internal->ready = true;
        
    exit:
        return internal;
    }

    static AudioFileWriterInternal* createPCM (ByteArray const& bytes, AudioFile::Format format, const int numChannels, const double sampleRate, const int bufferSize) throw()
    {
        AudioFileWriterInternal* internal = new AudioFileWriterInternal (numChannels * bufferSize);
        PlankFile file;
        
        if (!internal->initBytes (&file, bytes))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->initPCM (format, numChannels, sampleRate, bufferSize))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->openFile (&file))
        {
            pl_AudioFileWriter_Close (&internal->peer);
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        internal->ready = true;
        
    exit:
        return internal;
    }
    
    static AudioFileWriterInternal* createCompressedVBR (FilePath const& path, const int numChannels, const double sampleRate,
                                                         const float quality, const double frameDuration, const int bufferSize) throw()
    {
        AudioFileWriterInternal* internal = new AudioFileWriterInternal (numChannels * bufferSize);
        const Text ext = path.extension();

        AudioFile::Format format = AudioFile::FormatInvalid;
        
        if (false)
        {
        }
#if PLANK_OGGVORBIS
        else if (ext.equalsIgnoreCase ("ogg") &&
                 (sizeof (SampleType) == 4) &&
                 internal->isFloat)
        {
            format = AudioFile::FormatOggVorbis;
        }
#endif
#if PLANK_OPUS
        else if (ext.equalsIgnoreCase ("opus") &&
                 (sizeof (SampleType) == 4) &&
                 internal->isFloat)
        {
            format = AudioFile::FormatOpus;
        }
#endif
        
        if (!internal->initCompressedVBR (format, numChannels, sampleRate, quality, frameDuration, bufferSize))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->openPath (path))
        {
            pl_AudioFileWriter_Close (&internal->peer);
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        internal->ready = true;
        
    exit:
        return internal;
    }
    
    static AudioFileWriterInternal* createCompressedVBR (ByteArray const& bytes, AudioFile::Format format, const int numChannels, const double sampleRate,
                                                         const float quality, const double frameDuration, const int bufferSize) throw()
    {
        AudioFileWriterInternal* internal = new AudioFileWriterInternal (numChannels * bufferSize);
        PlankFile file;
        
        if (!internal->initBytes (&file, bytes))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->initCompressedVBR (format, numChannels, sampleRate, quality, frameDuration, bufferSize))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->openFile (&file))
        {
            pl_AudioFileWriter_Close (&internal->peer);
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        internal->ready = true;
        
    exit:
        return internal;
    }
    
    static AudioFileWriterInternal* createCompressedManaged (FilePath const& path, const int numChannels, const double sampleRate,
                                                             const int minBitRate, const int nominalBitRate, const int maxBitRate,
                                                             const double frameDuration, const int bufferSize) throw()
    {
        AudioFileWriterInternal* internal = new AudioFileWriterInternal (numChannels * bufferSize);
        const Text ext = path.extension();

        AudioFile::Format format = AudioFile::FormatInvalid;
        
        if (false)
        {
        }
#if PLANK_OGGVORBIS
        else if (ext.equalsIgnoreCase ("ogg") &&
                 (sizeof (SampleType) == 4) &&
                 internal->isFloat)
        {
            format = AudioFile::FormatOggVorbis;
        }
#endif
#if PLANK_OPUS
        else if (ext.equalsIgnoreCase ("opus") &&
                 (sizeof (SampleType) == 4) &&
                 internal->isFloat)
        {
            format = AudioFile::FormatOpus;
        }
#endif

        if (!internal->initCompressedManaged (format, numChannels, sampleRate, minBitRate, nominalBitRate, maxBitRate, frameDuration, bufferSize))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->openPath (path))
        {
            pl_AudioFileWriter_Close (&internal->peer);
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        internal->ready = true;
        
    exit:
        return internal;
    }
    
    static AudioFileWriterInternal* createCompressedManaged (ByteArray const& bytes, AudioFile::Format format, const int numChannels, const double sampleRate,
                                                             const int minBitRate, const int nominalBitRate, const int maxBitRate,
                                                             const double frameDuration, const int bufferSize) throw()
    {
        AudioFileWriterInternal* internal = new AudioFileWriterInternal (numChannels * bufferSize);
        PlankFile file;
        
        if (!internal->initBytes (&file, bytes))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->initCompressedManaged (format, numChannels, sampleRate, minBitRate, nominalBitRate, maxBitRate, frameDuration, bufferSize))
        {
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        if (!internal->openFile (&file))
        {
            pl_AudioFileWriter_Close (&internal->peer);
            pl_AudioFileWriter_Init (&internal->peer);
            goto exit;
        }
        
        internal->ready = true;
        
    exit:
        return internal;
    }

    
    bool initPCM (const AudioFile::Format format, const int numChannels, const double sampleRate, const int bufferSize) throw()
    {
        ResultCode result = PlankResult_UnknownError;
        
        if (format == AudioFile::FormatWAV)
        {
            if ((result = pl_AudioFileWriter_SetFormatWAV (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, this->isFloat)) != PlankResult_OK) goto exit;
        }
        else if (format == AudioFile::FormatAIFF)
        {
            // ideally we'd allow aiff but this is used to identify aifc (below)

            if (this->isFloat)
            {
                if ((result = pl_AudioFileWriter_SetFormatAIFC (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, true, false)) != PlankResult_OK) goto exit;;
            }
            else
            {
                if ((result = pl_AudioFileWriter_SetFormatAIFF (&peer, sizeof (SampleType) * 8, numChannels, sampleRate)) != PlankResult_OK) goto exit;;
            }
        }
        else if (format == AudioFile::FormatAIFC)
        {
            // ideally we'd use aifc only but some audio apps don't recognise this
#if PLANK_LITTLEENDIAN
            if ((result = pl_AudioFileWriter_SetFormatAIFC (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, this->isFloat, !this->isFloat && sizeof (SampleType) == 2)) != PlankResult_OK) goto exit;;
#endif
#if PLANK_BIGENDIAN
            if ((result = pl_AudioFileWriter_SetFormatAIFC (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, this->isFloat, false)) != PlankResult_OK) goto exit;;
#endif
        }
        else if (format == AudioFile::FormatCAF)
        {
            if ((result = pl_AudioFileWriter_SetFormatCAF (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, this->isFloat, PLANK_LITTLEENDIAN)) != PlankResult_OK) goto exit;;
        }
        else if (format == AudioFile::FormatW64)
        {
            if ((result = pl_AudioFileWriter_SetFormatW64 (&peer, sizeof (SampleType) * 8, numChannels, sampleRate, this->isFloat)) != PlankResult_OK) goto exit;;
        }
      
    exit:
        return result == PlankResult_OK;
    }
    
    bool initCompressedVBR (const AudioFile::Format format, const int numChannels, const double sampleRate, const float quality,
                            const double frameDuration, const int bufferSize) throw()
    {
        ResultCode result = PlankResult_UnknownError;
                
        if (false)
        {
        }
#if PLANK_OGGVORBIS
        else if (format == AudioFile::FormatOggVorbis)
        {
            if ((result = pl_AudioFileWriter_SetFormatOggVorbis (&peer, quality, numChannels, sampleRate)) != PlankResult_OK) goto exit;;
        }
#endif
#if PLANK_OPUS
        else if (format == AudioFile::FormatOpus)
        {
            if ((result = pl_AudioFileWriter_SetFormatOpus (&peer, quality, numChannels, sampleRate, frameDuration <= 0.0 ? 0.02 : frameDuration)) != PlankResult_OK) goto exit;;
        }
#endif
        
    exit:
        return result == PlankResult_OK;
    }
    
    bool initCompressedManaged (const AudioFile::Format format, const int numChannels, const double sampleRate,
                                const int minBitRate, const int nominalBitRate, const int maxBitRate,
                                const double frameDuration, const int bufferSize) throw()
    {
        ResultCode result = PlankResult_UnknownError;
            
        if (false)
        {
        }
#if PLANK_OGGVORBIS
        else if (format == AudioFile::FormatOggVorbis)
        {
            if ((result = pl_AudioFileWriter_SetFormatOggVorbisManaged (&peer,
                                                                        minBitRate, nominalBitRate, maxBitRate,
                                                                        numChannels, sampleRate)) != PlankResult_OK) goto exit;;
        }
#endif
#if PLANK_OPUS
        else if (format == AudioFile::FormatOpus)
        {
            if ((result = pl_AudioFileWriter_SetFormatOpusManaged (&peer,
                                                                   nominalBitRate == 0 ? (maxBitRate + minBitRate) / 2 : nominalBitRate,
                                                                   numChannels, sampleRate, frameDuration <= 0.0 ? 0.02 : frameDuration)) != PlankResult_OK) goto exit;;
        }
#endif
        
    exit:
        return result == PlankResult_OK;
    }
    
    ~AudioFileWriterInternal()
    {
        pl_AudioFileWriter_DeInit (&peer);
    }
    
    void writeHeader() throw()
    {
        pl_AudioFileWriter_WriteHeader (&peer);
    }
    
    void close() throw()
    {
        pl_AudioFileWriter_Close (&peer);
    }
    
    bool writeFrames (const int numFrames, const SampleType* frameData) throw()
    {
        return pl_AudioFileWriter_WriteFrames (&peer, numFrames, frameData) == PlankResult_OK;
    }
    
    bool writeFrames (Buffer const& frames) throw()
    {
        const int numChannels = peer.formatInfo.numChannels;
        plonk_assert ((frames.length() % numChannels) == 0);
        return pl_AudioFileWriter_WriteFrames (&peer, frames.length() / numChannels, frames.getArray()) == PlankResult_OK;
    }
    
    template<class OtherType>
    bool writeFrames (NumericalArray<OtherType> const& frames) throw()
    {
        bool success = true;
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
            success = pl_AudioFileWriter_WriteFrames (&peer, numSamplesThisTime / numChannels, nativeSamples) == PlankResult_OK;
            
            if (!success) break;
            
            numSamplesRemainaing -= numSamplesThisTime;
            sourceSamples += numSamplesThisTime;
        }
        
        return success;
    }
    
    bool writeFrames (AudioFileReader& reader, const int numFrames) throw()
    {
        const int numChannels = peer.formatInfo.numChannels;
        plonk_assert (peer.formatInfo.sampleRate == reader.getSampleRate());
        plonk_assert (peer.formatInfo.numChannels == reader.getNumChannels());
        
        const int bufferLength = buffer.length();
        const int bufferFrames = bufferLength / numChannels;
        int framesRead;
        
        if (numFrames <= 0)
        {
            do
            {
                reader.readFrames (buffer, false);
                framesRead = buffer.length() / numChannels;
            
                if (framesRead > 0)
                {
                    this->writeFrames (buffer);
                }
            }
            while (framesRead == bufferFrames);
        }
        else
        {
            int numFramesRemaining = numFrames;
            
            do
            {
                buffer.setSize (plonk::min (numFramesRemaining * numChannels, bufferLength), false);
                reader.readFrames (buffer, false);
                framesRead = buffer.length() / numChannels;
                
                if (framesRead == 0) break;
                
                numFramesRemaining -= framesRead;
                this->writeFrames (buffer);
            }
            while (numFramesRemaining > 0);
        }
        
        // and reset for next time
        buffer.setSize (bufferLength, false);
        
        return true;
    }
        
    friend class AudioFileWriter<SampleType>;
    
private:
    PlankAudioFileWriter peer;
    Buffer buffer;
    bool ready;
};


/** Audio file writing class.
 This can write PCM files in WAV or AIFF (or AIFC) format. And can also (wth the appropriately
 enabled compile time options) write Ogg Vorbis or Opus files either using VBR (variable bit rate)
 or managed bit rate (constant).
 @see BinaryFile
 @ingroup PlonkOtherUserClasses*/
template<class SampleType>
class AudioFileWriter: public SmartPointerContainer< AudioFileWriterInternal<SampleType> >
{
public:
    typedef AudioFileWriterInternal<SampleType> Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef NumericalArray<SampleType>          Buffer;

    AudioFileWriter (FilePath const& path, const int numChannels, const double sampleRate,
                     const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (Internal::createPCM (path, numChannels, sampleRate, bufferSize))
	{
	}
    
    AudioFileWriter (FilePath const& path, const int numChannels, const double sampleRate,
                     const float quality,
                     const double frameDuration = 0.0, const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (Internal::createCompressedVBR (path, numChannels, sampleRate, quality, frameDuration, bufferSize))
	{
	}
    
    AudioFileWriter (FilePath const& path, const int numChannels, const double sampleRate,
                     const int minBitRate, const int nominalBitRate, const int maxBitRate,
                     const double frameDuration = 0.0, const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (Internal::createCompressedManaged (path, numChannels, sampleRate, minBitRate, nominalBitRate, maxBitRate, frameDuration, bufferSize))
	{
	}
    
    AudioFileWriter (ByteArray const& bytes, const AudioFile::Format format, const int numChannels, const double sampleRate,
                     const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (Internal::createPCM (bytes, format, numChannels, sampleRate, bufferSize))
	{
	}
    
    AudioFileWriter (ByteArray const& bytes, const AudioFile::Format format, const int numChannels, const double sampleRate,
                     const float quality,
                     const double frameDuration = 0.0, const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (Internal::createCompressedVBR (bytes, format, numChannels, sampleRate, quality, frameDuration, bufferSize))
	{
	}

    AudioFileWriter (ByteArray const& bytes, const AudioFile::Format format, const int numChannels, const double sampleRate,
                     const int minBitRate, const int nominalBitRate, const int maxBitRate,
                     const double frameDuration = 0.0, const int bufferSize = AudioFile::DefaultBufferSize) throw()
	:	Base (Internal::createCompressedManaged (bytes, format, numChannels, sampleRate, minBitRate, nominalBitRate, maxBitRate, frameDuration, bufferSize))
	{
	}

    void writeHeader() throw()
    {
        this->getInternal()->writeHeader();
    }
    
    void close() throw()
    {
        this->getInternal()->close();
    }

    bool isReady() const throw()
    {
        return this->getInternal()->ready;
    }

    /** Get the number of channels in the file. */
    inline int getNumChannels() const throw()
    {
        return this->getInternal()->peer.formatInfo.numChannels;
    }
    
    /** Get the sample rate of the file. */
    inline double getSampleRate() const throw()
    {
        return this->getInternal()->peer.formatInfo.sampleRate;
    }
    
    bool writeFrames (const int numFrames, const SampleType* frameData) throw()
    {
        return this->getInternal()->writeFrames (numFrames, frameData);
    }

    bool writeFrames (Buffer const& frames) throw()
    {
        return this->getInternal()->writeFrames (frames);
    }
    
    bool writeFrames (AudioFileReader& reader, const int numFrames = 0) throw()
    {
        return this->getInternal()->writeFrames (reader, numFrames);
    }
    
    template<class OtherType>
    bool writeFrames (NumericalArray<OtherType> const& frames) throw()
    {
        return this->getInternal()->writeFrames (frames);
    }
};

#endif // PLONK_AUDIOFILEWRITER_H
