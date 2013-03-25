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
#define PLANKAUDIOFILE_WAV_FMT_LENGTH                 16
#define PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH      40

// EBU-TECH 3306
#define PLANKAUDIOFILE_WAV_SPEAKER_FRONT_LEFT               0x00000001
#define PLANKAUDIOFILE_WAV_SPEAKER_FRONT_RIGHT              0x00000002
#define PLANKAUDIOFILE_WAV_SPEAKER_FRONT_CENTER             0x00000004
#define PLANKAUDIOFILE_WAV_SPEAKER_LOW_FREQUENCY            0x00000008
#define PLANKAUDIOFILE_WAV_SPEAKER_BACK_LEFT                0x00000010
#define PLANKAUDIOFILE_WAV_SPEAKER_BACK_RIGHT               0x00000020
#define PLANKAUDIOFILE_WAV_SPEAKER_FRONT_LEFT_OF_CENTER     0x00000040
#define PLANKAUDIOFILE_WAV_SPEAKER_FRONT_RIGHT_OF_CENTER    0x00000080
#define PLANKAUDIOFILE_WAV_SPEAKER_BACK_CENTER              0x00000100
#define PLANKAUDIOFILE_WAV_SPEAKER_SIDE_LEFT                0x00000200
#define PLANKAUDIOFILE_WAV_SPEAKER_SIDE_RIGHT               0x00000400
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_CENTER               0x00000800
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_FRONT_LEFT           0x00001000
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_FRONT_CENTER         0x00002000
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_FRONT_RIGHT          0x00004000
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_BACK_LEFT            0x00008000
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_BACK_CENTER          0x00010000
#define PLANKAUDIOFILE_WAV_SPEAKER_TOP_BACK_RIGHT           0x00020000
#define PLANKAUDIOFILE_WAV_SPEAKER_ALL                      0x80000000
#define PLANKAUDIOFILE_WAV_SPEAKER_STEREO_LEFT              0x20000000
#define PLANKAUDIOFILE_WAV_SPEAKER_STEREO_RIGHT             0x40000000

#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_1_LEFT         0x00800000
#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_1_RIGHT        0x01000000
#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_2_LEFT         0x02000000
#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_2_RIGHT        0x04000000

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

#define PLANKAUDIOFILE_AIFF_COMM_LENGTH             18
#define PLANKAUDIOFILE_AIFC_VERSION                 0xA2805140
#define PLANKAUDIOFILE_AIFC_COMM_LENGTH             24

/* AIFF spec channel maps
1: left
2: left, right
3: left, right, centre
4: left, right, rear left, rear right
5: left, centre, right, surround
6: left, left centre, centre, right, right centre, surround
*/

#define PLANKAUDIOFILEMETADATA_TEXTENCODING_ASCII   0
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_UTF8    1
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_UTF16   2
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_LATIN1  0

//#define PLANKAUDIOFILEMETADATA_UNSET  INT_MIN


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
    #include "../../../../ext/opus/opusfile/opusfile.h"
    #include "../../../../ext/opus/opus_header.h"
    #define PLANKAUDIOFILE_OPUS_VENDOR    "libopus 1.01 in Plank " PLANK_VERSION
    #define PLANKAUDIOFILE_OPUS_MAXFRAMESIZE                           (960 * 6)
    #define PLANKAUDIOFILE_OPUS_MAXPACKETSIZE                     (1275 * 3 + 7)
    #define PALNKAUDIOFILE_OPUS_MAXSEGMENTS                                  255
    #define PALNKAUDIOFILE_OPUS_FLUSHFILLSIZE                        (255 * 255)
    #define PLANKAUDIOFILE_OPUS_DELAY                                        312
    #define PLANKAUDIOFILE_OPUS_READBYTES                                    200
    #define PLANKAUDIOFILE_OPUS_MINSAMPLERATE                               8000
    #define PLANKAUDIOFILE_OPUS_MAXSAMPLERATE                             192000
    #define PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE                          48000
    #define PLANKAUDIOFILE_OPUS_HEAD                                  "OpusHead"
    #define PLANKAUDIOFILE_OPUS_HEAD_LEN                                       8
    #define PLANKAUDIOFILE_OPUS_PREROLL_MS                                    80
    #define PALNKAUDIOFILE_OPUS_HEADERNUMSEGSPOS                              26
#endif


typedef struct PlankAudioFileFormatInfo
{
    PlankC format;
    PlankC encoding;
    
    PlankI bitsPerSample;
    PlankI bytesPerFrame;
    PlankI minimumBitRate;
    PlankI maximumBitRate;
    PlankI nominalBitRate;
    PlankI numChannels;
    PlankD sampleRate;
    PlankD frameDuration;
    PlankF quality;
    
    PlankUI channelMask;

} PlankAudioFileFormatInfo;

/** An opaque reference to the <i>Plank AudioFileMetaData</i> object. */
typedef struct PlankAudioFileMetaData* PlankAudioFileMetaDataRef;

/** An opaque reference to the <i>Plank AudioFileCuePoint</i> object. */
typedef struct PlankAudioFileCuePoint* PlankAudioFileCuePointRef;

/** An opaque reference to the <i>Plank AudioFileRegion</i> object. */
typedef struct PlankAudioFileRegion* PlankAudioFileRegionRef;


typedef struct PlankAudioFileWAVExtensible
{
    PlankUI ext1;
    PlankUS ext2;
    PlankUS ext3;
    PlankUC ext4[8];
} PlankAudioFileWAVExtensible;

static inline PlankAudioFileWAVExtensible* pl_AudioFileWAVExtensible_GetPCM()
{
    static PlankAudioFileWAVExtensible data;
    static PlankB firstTime = PLANK_TRUE;
    
    if (firstTime)
    {
        firstTime = PLANK_FALSE;
        pl_MemoryZero (&data, sizeof (PlankAudioFileWAVExtensible));
        data.ext1 = 0x00000001;
        data.ext2 = 0x0000;
        data.ext3 = 0x0010;
        data.ext4[0] = 0x80; data.ext4[1] = 0x00; data.ext4[2] = 0x00; data.ext4[3] = 0xAA;
        data.ext4[4] = 0x00; data.ext4[5] = 0x38; data.ext4[6] = 0x9B; data.ext4[7] = 0x71;
    }
    
    return &data;
}

static inline PlankAudioFileWAVExtensible* pl_AudioFileWAVExtensible_GetFloat()
{
    static PlankAudioFileWAVExtensible data;
    static PlankB firstTime = PLANK_TRUE;
    
    if (firstTime)
    {
        firstTime = PLANK_FALSE;
        pl_MemoryZero (&data, sizeof (PlankAudioFileWAVExtensible));
        data.ext1 = 0x00000003;
        data.ext2 = 0x0000;
        data.ext3 = 0x0010;
        data.ext4[0] = 0x80; data.ext4[1] = 0x00; data.ext4[2] = 0x00; data.ext4[3] = 0xAA;
        data.ext4[4] = 0x00; data.ext4[5] = 0x38; data.ext4[6] = 0x9B; data.ext4[7] = 0x71;
    }
    
    return &data;
}

static inline PlankAudioFileWAVExtensible* pl_AudioFileWAVExtensible_GetAmbisonic()
{
    static PlankAudioFileWAVExtensible data;
    static PlankB firstTime = PLANK_TRUE;
    
    if (firstTime)
    {
        firstTime = PLANK_FALSE;
        pl_MemoryZero (&data, sizeof (PlankAudioFileWAVExtensible));
        data.ext1 = 0x00000001;
        data.ext2 = 0x0721;
        data.ext3 = 0x11D3;
        data.ext4[0] = 0x86; data.ext4[1] = 0x44; data.ext4[2] = 0xC8; data.ext4[3] = 0xC1;
        data.ext4[4] = 0xCA; data.ext4[5] = 0x00; data.ext4[6] = 0x00; data.ext4[7] = 0x00;
    }
    
    return &data;
}

#endif // PLANK_AUDIOFILECOMMON_H
