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

#ifndef PLANK_AUDIOFILECOMMON_H
#define PLANK_AUDIOFILECOMMON_H

#define PLANKAUDIOFILE_CHARBITS                       8

#define PLANKAUDIOFILE_FORMAT_INVALID                -1
#define PLANKAUDIOFILE_FORMAT_UNKNOWN                 0
#define PLANKAUDIOFILE_FORMAT_UNKNOWNIFF              1
#define PLANKAUDIOFILE_FORMAT_WAV                     2
#define PLANKAUDIOFILE_FORMAT_AIFF                    3
#define PLANKAUDIOFILE_FORMAT_AIFC                    4
#define PLANKAUDIOFILE_FORMAT_OGGVORBIS               5
#define PLANKAUDIOFILE_FORMAT_OPUS                    6

static inline const char* pl_PlankAudioFileGetFormatName (int format)
{
    switch (format)
    {
        case PLANKAUDIOFILE_FORMAT_INVALID:     return "Invalid";
        case PLANKAUDIOFILE_FORMAT_UNKNOWN:     return "Unknown";
        case PLANKAUDIOFILE_FORMAT_UNKNOWNIFF:  return "Unknown IFF";
        case PLANKAUDIOFILE_FORMAT_WAV:         return "WAV";
        case PLANKAUDIOFILE_FORMAT_AIFF:        return "AIFF";
        case PLANKAUDIOFILE_FORMAT_AIFC:        return "AIFC";
        case PLANKAUDIOFILE_FORMAT_OGGVORBIS:   return "Ogg Vorbis";
        case PLANKAUDIOFILE_FORMAT_OPUS:        return "Opus";

        default: return "invalid";
    }
}

#define PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG        1
#define PLANKAUDIOFILE_ENCODING_PCM_FLAG              2
#define PLANKAUDIOFILE_ENCODING_FLOAT_FLAG            4
#define PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG   8

#define PLANKAUDIOFILE_ENCODING_INVALID              -1
#define PLANKAUDIOFILE_ENCODING_UNKNOWN               0
#define PLANKAUDIOFILE_ENCODING_UNUSED1               1

#define PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN      (PLANKAUDIOFILE_ENCODING_PCM_FLAG)
#define PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN         (PLANKAUDIOFILE_ENCODING_PCM_FLAG | PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG)
#define PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN    (PLANKAUDIOFILE_ENCODING_FLOAT_FLAG)
#define PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN       (PLANKAUDIOFILE_ENCODING_FLOAT_FLAG | PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG)

#define PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN_NONINTERLEAVED      (PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN | PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG)
#define PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN_NONINTERLEAVED         (PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN | PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG)
#define PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN_NONINTERLEAVED    (PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN | PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG)
#define PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN_NONINTERLEAVED       (PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN | PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG)

#define PLANKAUDIOFILE_ENCODING_MIN                   PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN
#define PLANKAUDIOFILE_ENCODING_MAX                   PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN_NONINTERLEAVED

#define PLANKAUDIOFILE_WAV_COMPRESSION_PCM            0x0001
#define PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT          0x0003
#define PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE     0xFFFE

//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_RIFF = PLANKFOURCHARCODE("RIFF");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_WAVE = PLANKFOURCHARCODE("WAVE");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_FMT  = PLANKFOURCHARCODE("fmt ");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_DATA = PLANKFOURCHARCODE("data");
//
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_FORM = PLANKFOURCHARCODE("FORM");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_AIFF = PLANKFOURCHARCODE("AIFF");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_AIFC = PLANKFOURCHARCODE("AIFC");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_COMM = PLANKFOURCHARCODE("COMM");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_SSND = PLANKFOURCHARCODE("SSND");
//static const PlankFourCharCode PLANKAUDIOFILE_CHUNKID_FVER = PLANKFOURCHARCODE("FVER");

#define PLANKAUDIOFILE_AIFC_VERSION                   0xA2805140

#define PLANKAUDIOFILEMETADATA_TEXTENCODING_ASCII   0
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_UTF8    1
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_UTF16   2
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_LATIN1  0

#define PLANKAUDIOFILE_CUEPOINTTYPE_CUEPOINT        0
#define PLANKAUDIOFILE_CUEPOINTTYPE_INSERTPOINT     1
#define PLANKAUDIOFILE_CUEPOINTTYPE_PLAYPOSITION    2
#define PLANKAUDIOFILE_CUEPOINTTYPE_REGIONSTART     3
#define PLANKAUDIOFILE_CUEPOINTTYPE_REGIONEND       4

#define PLANKAUDIOFILE_REGIONTYPE_REGION            0
#define PLANKAUDIOFILE_REGIONTYPE_SELECTION         1
#define PLANKAUDIOFILE_REGIONTYPE_LOOP              2
#define PLANKAUDIOFILE_REGIONTYPE_LYRICS            3

#if PLANK_OGGVORBIS
    #ifndef OV_EXCLUDE_STATIC_CALLBACKS
        #define OV_EXCLUDE_STATIC_CALLBACKS
    #endif

    #include "../../../../ext/vorbis/vorbis/vorbisenc.h"
    #include "../../../../ext/vorbis/vorbis/codec.h"
    #include "../../../../ext/vorbis/vorbis/vorbisfile.h"
#endif

#if PLANK_OPUS
    #include "../../../../ext/ogg/ogg.h"
    #include "../../../../ext/opus/opus.h"
    #include "../../../../ext/opus/opus_multistream.h"
    #include "../../../../ext/opus/opus_header.h"
    #define PLANKAUDIOFILE_OPUS_MAXFRAMESIZE             5760  // (960 * 6)
    #define PLANKAUDIOFILE_OPUS_READBYTES                 200
    #define PLANKAUDIOFILE_OPUS_MINSAMPLERATE            8000
    #define PLANKAUDIOFILE_OPUS_MAXSAMPLERATE          192000
    #define PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE       48000
    #define PLANKAUDIOFILE_OPUS_HEAD                "OpusHead"
    #define PLANKAUDIOFILE_OPUS_PREROLL_MS                 80
#endif


typedef struct PlankAudioFileFormatInfo
{
    PlankC format;
    PlankC encoding;
    
    PlankI bitsPerSample;
    PlankI bytesPerFrame;
    PlankI numChannels;
    PlankD sampleRate;

} PlankAudioFileFormatInfo;

/** An opaque reference to the <i>Plank AudioFileMetaData</i> object. */
typedef struct PlankAudioFileMetaData* PlankAudioFileMetaDataRef;

/** An opaque reference to the <i>Plank AudioFileCuePoint</i> object. */
typedef struct PlankAudioFileCuePoint* PlankAudioFileCuePointRef;

/** An opaque reference to the <i>Plank AudioFileRegion</i> object. */
typedef struct PlankAudioFileRegion* PlankAudioFileRegionRef;


#endif // PLANK_AUDIOFILECOMMON_H
