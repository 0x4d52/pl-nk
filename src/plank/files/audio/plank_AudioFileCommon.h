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

#include "../plank_IffFileCommon.h"

#define PLANKAUDIOFILE_CHARBITS                       8
//#define PLANKAUDIOFILE_CHANNELMAPLENGTH             256

#define PLANKAUDIOFILE_FORMAT_INVALID                -1
#define PLANKAUDIOFILE_FORMAT_UNKNOWN                 0
#define PLANKAUDIOFILE_FORMAT_UNKNOWNIFF              1
#define PLANKAUDIOFILE_FORMAT_WAV                     2
#define PLANKAUDIOFILE_FORMAT_AIFF                    3
#define PLANKAUDIOFILE_FORMAT_AIFC                    4
#define PLANKAUDIOFILE_FORMAT_OGGVORBIS               5
#define PLANKAUDIOFILE_FORMAT_OPUS                    6
#define PLANKAUDIOFILE_FORMAT_CAF                     7
#define PLANKAUDIOFILE_FORMAT_W64                     8
#define PLANKAUDIOFILE_FORMAT_REGION                 99
#define PLANKAUDIOFILE_FORMAT_MULTI                 100
#define PLANKAUDIOFILE_FORMAT_ARRAY                 101
#define PLANKAUDIOFILE_FORMAT_QUEUE                 102
#define PLANKAUDIOFILE_FORMAT_CUSTOM                103

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
        case PLANKAUDIOFILE_FORMAT_CAF:         return "CAF";
        case PLANKAUDIOFILE_FORMAT_W64:         return "W64";
        case PLANKAUDIOFILE_FORMAT_REGION:      return "Region";
        case PLANKAUDIOFILE_FORMAT_MULTI:       return "Multi";
        case PLANKAUDIOFILE_FORMAT_ARRAY:       return "Array";
        case PLANKAUDIOFILE_FORMAT_QUEUE:       return "Queue";
        case PLANKAUDIOFILE_FORMAT_CUSTOM:      return "Custom";

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
#define PLANKAUDIOFILE_WAV_DS64_MINIMUMLENGTH         28
//#define PLANKAUDIOFILE_WAV_JUNK_LENGTH                (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH - PLANKAUDIOFILE_WAV_FMT_LENGTH + PLANKAUDIOFILE_WAV_DS64_LENGTH)
//#define PLANKAUDIOFILE_WAVEXT_JUNK_LENGTH             PLANKAUDIOFILE_WAV_DS64_LENGTH
#define PLANKAUDIOFILE_WAV_HEADER_LENGTH              (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH + PLANKAUDIOFILE_WAV_DS64_MINIMUMLENGTH + 8)


// from WAV and CAF

#define PLANKAUDIOFILE_CHANNEL_NONE                         0
#define PLANKAUDIOFILE_CHANNEL_FRONT_LEFT                   1 //   1.	Front Left - FL
#define PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT                  2 //   2.	Front Right - FR
#define PLANKAUDIOFILE_CHANNEL_FRONT_CENTER                 3 //   3.	Front Center - FC
#define PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY                4 //   4.	Low Frequency - LF
#define PLANKAUDIOFILE_CHANNEL_BACK_LEFT                    5 //   5.	Back Left - BL
#define PLANKAUDIOFILE_CHANNEL_BACK_RIGHT                   6 //   6.	Back Right - BR
#define PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER         7 //   7.	Front Left of Center - FLC
#define PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER        8 //   8.	Front Right of Center - FRC
#define PLANKAUDIOFILE_CHANNEL_BACK_CENTER                  9 //   9.	Back Center - BC
#define PLANKAUDIOFILE_CHANNEL_SIDE_LEFT                    10 // 10.	Side Left - SL
#define PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT                   11 // 11.	Side Right - SR
#define PLANKAUDIOFILE_CHANNEL_TOP_CENTER                   12 // 12.	Top Center - TC
#define PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT               13 // 13.	Top Front Left - TFL
#define PLANKAUDIOFILE_CHANNEL_TOP_FRONT_CENTER             14 // 14.	Top Front Center - TFC
#define PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT              15 // 15.	Top Front Right - TFR
#define PLANKAUDIOFILE_CHANNEL_TOP_BACK_LEFT                16 // 16.	Top Back Left - TBL
#define PLANKAUDIOFILE_CHANNEL_TOP_BACK_CENTER              17 // 17.	Top Back Center - TBC
#define PLANKAUDIOFILE_CHANNEL_TOP_BACK_RIGHT               18 // 18.	Top Back Right - TBR

#define PLANKAUDIOFILE_CHANNEL_UNUSED0                      19
#define PLANKAUDIOFILE_CHANNEL_UNUSED1                      20
#define PLANKAUDIOFILE_CHANNEL_UNUSED2                      21
#define PLANKAUDIOFILE_CHANNEL_UNUSED3                      22
#define PLANKAUDIOFILE_CHANNEL_UNUSED4                      23

#define PLANKAUDIOFILE_CHANNEL_BITSTREAM_1_LEFT             24
#define PLANKAUDIOFILE_CHANNEL_BITSTREAM_1_RIGHT            25
#define PLANKAUDIOFILE_CHANNEL_BITSTREAM_2_LEFT             26
#define PLANKAUDIOFILE_CHANNEL_BITSTREAM_2_RIGHT            27

#define PLANKAUDIOFILE_CHANNEL_STEREO_LEFT                  30
#define PLANKAUDIOFILE_CHANNEL_STEREO_RIGHT                 31

#define PLANKAUDIOFILE_CHANNEL_RESERVED                     32

// CAF
#define PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT           33
#define PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT          34
#define PLANKAUDIOFILE_CHANNEL_LEFT_WIDE                    35
#define PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE                   36
#define PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA          37
#define PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL                   38           // matrix encoded 4 channels
#define PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL                  39           // matrix encoded 4 channels
#define PLANKAUDIOFILE_CHANNEL_HEARING_IMPAIRED             40
#define PLANKAUDIOFILE_CHANNEL_NARRATION                    41
#define PLANKAUDIOFILE_CHANNEL_MONO                         42
#define PLANKAUDIOFILE_CHANNEL_DIALOGCENTRICMIX             43
#define PLANKAUDIOFILE_CHANNEL_CENTERSURROUND_DIRECT        44           // back center non diffuse
#define PLANKAUDIOFILE_CHANNEL_HAPTIC                       45

#define PLANKAUDIOFILE_CHANNEL_AMBISONIC_W                  200
#define PLANKAUDIOFILE_CHANNEL_AMBISONIC_X                  201
#define PLANKAUDIOFILE_CHANNEL_AMBISONIC_Y                  202
#define PLANKAUDIOFILE_CHANNEL_AMBISONIC_Z                  203
#define PLANKAUDIOFILE_CHANNEL_MS_MID                       204
#define PLANKAUDIOFILE_CHANNEL_MS_SIDE                      205
#define PLANKAUDIOFILE_CHANNEL_XY_X                         206
#define PLANKAUDIOFILE_CHANNEL_XY_Y                         207
#define PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT              301
#define PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT             302
#define PLANKAUDIOFILE_CHANNEL_CLICK_TRACK                  304
#define PLANKAUDIOFILE_CHANNEL_FOREIGN_LANGUAGE             305
#define PLANKAUDIOFILE_CHANNEL_DISCRETE                     400

#define PLANKAUDIOFILE_CHANNEL_DISCRETE_N                (1<<16) // OR with 0-based channel index 0-65535


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
#define PLANKAUDIOFILE_WAV_SPEAKER_RESERVED                 0x80000000
#define PLANKAUDIOFILE_WAV_SPEAKER_STEREO_LEFT              0x20000000
#define PLANKAUDIOFILE_WAV_SPEAKER_STEREO_RIGHT             0x40000000

#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_1_LEFT         0x00800000
#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_1_RIGHT        0x01000000
#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_2_LEFT         0x02000000
#define PLANKAUDIOFILE_WAV_SPEAKER_BITSTREAM_2_RIGHT        0x04000000
#define PLANKAUDIOFILE_WAV_SPEAKER_ANY                      0xffffffff

#define PLANKAUDIOFILE_W64_RIFF_ID          PLANKIFFFILE_W64_RIFF_ID
#define PLANKAUDIOFILE_W64_LIST_ID          PLANKIFFFILE_W64_LIST_ID
#define PLANKAUDIOFILE_W64_WAVE_ID          "wave-ACF3-11D3-8CD1-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_FMT_ID           "fmt -ACF3-11D3-8CD1-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_FACT_ID          "fact-ACF3-11D3-8CD1-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_DATA_ID          "data-ACF3-11D3-8CD1-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_LEVL_ID          "levl-ACF3-11D3-8CD1-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_JUNK_ID          PLANKIFFFILE_W64_JUNK_ID
#define PLANKAUDIOFILE_W64_BEXT_ID          "bext-ACF3-11D3-8CD1-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_MARKER_ID        "ABF76256-392D-11D2-86C7-00C04F8EDB8A"
#define PLANKAUDIOFILE_W64_SUMMARYLIST_ID   "925F94BC-525A-11D2-86DC-00C04F8EDB8A"

/*
RIFF = 1179011410
WAVE = 1163280727
fmt  = 544501094
data = 1635017060
JUNK = 1263424842
FORM = 1297239878
AIFF = 1179011393
SSND = 1145983827
COMM = 1296912195
*/

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

typedef struct PlankCAFStringID {
    
    PlankUI  stringID;
    PlankLL  offset;
} PlankCAFStringID;

#define PLANKAUDIOFILE_CAF_DEFAULTHEADERPAD         4028
#define PLANKAUDIOFILE_CAF_DESC_LENGTH              32
#define PLANKAUDIOFILE_CAF_FLOAT_FLAG               0x00000001 // (1L << 0)
#define PLANKAUDIOFILE_CAF_LITTLEENDIAN_FLAG        0x00000002 // (1L << 1)

#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPENONE      0
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE24        1
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE25        2
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE30DROP    3
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE30        4
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE2997      5
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE2997DROP  6
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE60        7
#define PLANKAUDIOFILE_CAF_SMPTE_TIMETYPE5994      8



#define PLANKAUDIOFILE_CAF_MARKERTYPE_GENERIC              0
#define PLANKAUDIOFILE_CAF_MARKERTYPE_PROGRAMSTART         pl_FourCharCode ("pbeg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_PROMGRAMEND          pl_FourCharCode ("pend")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_TRACKSTART           pl_FourCharCode ("tbeg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_TRACKEND             pl_FourCharCode ("tend")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_INDEX                pl_FourCharCode ("indx")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_REGIONSTART          pl_FourCharCode ("rbeg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_REGIONEND            pl_FourCharCode ("rend")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_REGIONSYNCPOINT      pl_FourCharCode ("rsyc")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_SELECTIONSTART       pl_FourCharCode ("sbeg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_SELECTIONEND         pl_FourCharCode ("send")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_EDITSOURCEBEGIN      pl_FourCharCode ("cbeg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_EDITSOURCEEND        pl_FourCharCode ("cend")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_EDITDESTINATIONBEGIN pl_FourCharCode ("dbeg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_EDITDESTINATIONEND   pl_FourCharCode ("dend")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_SUSTAINLOOPSTART     pl_FourCharCode ("slbg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_SUSTAINLOOPEND       pl_FourCharCode ("slen")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_RELEASELOOPSTART     pl_FourCharCode ("rlbg")
#define PLANKAUDIOFILE_CAF_MARKERTYPE_RELEASELOOPEND       pl_FourCharCode ("rlen")

// essentially the same as CAF
#define PLANKAUDIOFILE_LAYOUT_USECHANNELDESCRIPTIONS    ((0<<16) | 0)
#define PLANKAUDIOFILE_LAYOUT_USECHANNELBITMAP          ((1<<16) | 0)
#define PLANKAUDIOFILE_LAYOUT_MONO                      ((100<<16) | 1)
#define PLANKAUDIOFILE_LAYOUT_STEREO                    ((101<<16) | 2)
#define PLANKAUDIOFILE_LAYOUT_STEREOHEADPHONES          ((102<<16) | 2)
#define PLANKAUDIOFILE_LAYOUT_MATRIXSTEREO              ((103<<16) | 2)
#define PLANKAUDIOFILE_LAYOUT_MIDSIDE                   ((104<<16) | 2)
#define PLANKAUDIOFILE_LAYOUT_XY                        ((105<<16) | 2)
#define PLANKAUDIOFILE_LAYOUT_BINAURAL                  ((106<<16) | 2)
#define PLANKAUDIOFILE_LAYOUT_AMBISONIC_B_FORMAT        ((107<<16) | 4)
#define PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC              ((108<<16) | 4)
#define PLANKAUDIOFILE_LAYOUT_PENTAGONAL                ((109<<16) | 5)
#define PLANKAUDIOFILE_LAYOUT_HEXAGONAL                 ((110<<16) | 6)
#define PLANKAUDIOFILE_LAYOUT_OCTAGONAL                 ((111<<16) | 8)
#define PLANKAUDIOFILE_LAYOUT_CUBE                      ((112<<16) | 8)
#define PLANKAUDIOFILE_LAYOUT_MPEG_1_0                  PLANKAUDIOFILE_LAYOUT_MONO         //  C
#define PLANKAUDIOFILE_LAYOUT_MPEG_2_0                  PLANKAUDIOFILE_LAYOUT_STEREO       //  L R
#define PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A                ((113<<16) | 3)                      //  L R C
#define PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B                ((114<<16) | 3)                      //  C L R
#define PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A                ((115<<16) | 4)                       //  L R C Cs
#define PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B                ((116<<16) | 4)                       //  C L R Cs
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A                ((117<<16) | 5)                       //  L R C Ls Rs
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B                ((118<<16) | 5)                       //  L R Ls Rs C
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C                ((119<<16) | 5)                       //  L C R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D                ((120<<16) | 5)                       //  C L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A                ((121<<16) | 6)                       //  L R C LFE Ls Rs
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B                ((122<<16) | 6)                       //  L R Ls Rs C LFE
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C                ((123<<16) | 6)                       //  L C R Ls Rs LFE
#define PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D                ((124<<16) | 6)                       //  C L R Ls Rs LFE
#define PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A                ((125<<16) | 7)                       //  L R C LFE Ls Rs Cs
#define PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A                ((126<<16) | 8)                       //  L R C LFE Ls Rs Lc Rc
#define PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B                ((127<<16) | 8)                       //  C Lc Rc L R Ls Rs LFE 
#define PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C                ((128<<16) | 8)                       //  L R C LFE Ls Rs Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1        ((129<<16) | 8)                       //  L R Ls Rs C LFE Lc Rc
#define PLANKAUDIOFILE_LAYOUT_SMPTE_DTV                 ((130<<16) | 8)                       //  L R C LFE Ls Rs Lt Rt
#define PLANKAUDIOFILE_LAYOUT_ITU_1_0                   PLANKAUDIOFILE_LAYOUT_MONO         //  C
#define PLANKAUDIOFILE_LAYOUT_ITU_2_0                   PLANKAUDIOFILE_LAYOUT_STEREO       //  L R
#define PLANKAUDIOFILE_LAYOUT_ITU_2_1                   ((131<<16) | 3)                       //  L R Cs
#define PLANKAUDIOFILE_LAYOUT_ITU_2_2                   ((132<<16) | 4)                       //  L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_ITU_3_0                   PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A   //  L R C
#define PLANKAUDIOFILE_LAYOUT_ITU_3_1                   PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A   //  L R C Cs
#define PLANKAUDIOFILE_LAYOUT_ITU_3_2                   PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A   //  L R C Ls Rs
#define PLANKAUDIOFILE_LAYOUT_ITU_3_2_1                 PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A   //  L R C LFE Ls Rs
#define PLANKAUDIOFILE_LAYOUT_ITU_3_4_1                 PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C   //  L R C LFE Ls Rs Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_DVD_0                     PLANKAUDIOFILE_LAYOUT_MONO         // C (mono)
#define PLANKAUDIOFILE_LAYOUT_DVD_1                     PLANKAUDIOFILE_LAYOUT_STEREO       // L R
#define PLANKAUDIOFILE_LAYOUT_DVD_2                     PLANKAUDIOFILE_LAYOUT_ITU_2_1      // L R Cs
#define PLANKAUDIOFILE_LAYOUT_DVD_3                     PLANKAUDIOFILE_LAYOUT_ITU_2_2      // L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DVD_4                     ((133<<16) | 3)                       // L R LFE
#define PLANKAUDIOFILE_LAYOUT_DVD_5                     ((134<<16) | 4)                       // L R LFE Cs
#define PLANKAUDIOFILE_LAYOUT_DVD_6                     ((135<<16) | 5)                       // L R LFE Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DVD_7                     PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A   // L R C
#define PLANKAUDIOFILE_LAYOUT_DVD_8                     PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A   // L R C Cs
#define PLANKAUDIOFILE_LAYOUT_DVD_9                     PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A   // L R C Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DVD_10                    ((136<<16) | 4)                       // L R C LFE
#define PLANKAUDIOFILE_LAYOUT_DVD_11                    ((137<<16) | 5)                       // L R C LFE Cs
#define PLANKAUDIOFILE_LAYOUT_DVD_12                    PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A   // L R C LFE Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DVD_13                    PLANKAUDIOFILE_LAYOUT_DVD_8        // L R C Cs
#define PLANKAUDIOFILE_LAYOUT_DVD_14                    PLANKAUDIOFILE_LAYOUT_DVD_9        // L R C Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DVD_15                    PLANKAUDIOFILE_LAYOUT_DVD_10       // L R C LFE
#define PLANKAUDIOFILE_LAYOUT_DVD_16                    PLANKAUDIOFILE_LAYOUT_DVD_11       // L R C LFE Cs
#define PLANKAUDIOFILE_LAYOUT_DVD_17                    PLANKAUDIOFILE_LAYOUT_DVD_12       // L R C LFE Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DVD_18                    ((138<<16) | 5)                       // L R Ls Rs LFE
#define PLANKAUDIOFILE_LAYOUT_DVD_19                    PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B   // L R Ls Rs C
#define PLANKAUDIOFILE_LAYOUT_DVD_20                    PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B   // L R Ls Rs C LFE
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_4               PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5               PLANKAUDIOFILE_LAYOUT_PENTAGONAL
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6               PLANKAUDIOFILE_LAYOUT_HEXAGONAL
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_8               PLANKAUDIOFILE_LAYOUT_OCTAGONAL
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5_0             PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B   // L R Ls Rs C
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0             ((139<<16) | 6)                       // L R Ls Rs C Cs
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0             ((140<<16) | 7)                       // L R Ls Rs C Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT       ((148<<16) | 7)                       // L R Ls Rs C Lc Rc
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5_1             PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A   // L R C LFE Ls Rs
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_1             PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A   // L R C LFE Ls Rs Cs
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_1             PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C   // L R C LFE Ls Rs Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_1_FRONT       PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A   // L R C LFE Ls Rs Lc Rc
#define PLANKAUDIOFILE_LAYOUT_AAC_3_0                   PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B   // C L R
#define PLANKAUDIOFILE_LAYOUT_AAC_QUADRAPHONIC          PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC // L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_AAC_4_0                   PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B   // C L R Cs
#define PLANKAUDIOFILE_LAYOUT_AAC_5_0                   PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D   // C L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_AAC_5_1                   PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D   // C L R Ls Rs Lfe
#define PLANKAUDIOFILE_LAYOUT_AAC_6_0                   ((141<<16) | 6)                       // C L R Ls Rs Cs
#define PLANKAUDIOFILE_LAYOUT_AAC_6_1                   ((142<<16) | 7)                       // C L R Ls Rs Cs Lfe
#define PLANKAUDIOFILE_LAYOUT_AAC_7_0                   ((143<<16) | 7)                       // C L R Ls Rs Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_AAC_7_1                   PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B   // C Lc Rc L R Ls Rs Lfe
#define PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL             ((144<<16) | 8)                       // C L R Ls Rs Rls Rrs Cs
#define PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD              ((145<<16) | 16)                      // L R C Vhc Lsd Rsd Ls Rs Vhl Vhr Lw Rw Csd Cs LFE1 LFE2
#define PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL             ((146<<16) | 21)                      // TMH_10_2_std plus: Lc Rc HI VI Haptic
#define PLANKAUDIOFILE_LAYOUT_AC3_1_0_1                 ((149<<16) | 2)                       // C LFE
#define PLANKAUDIOFILE_LAYOUT_AC3_3_0                   ((150<<16) | 3)                       // L C R
#define PLANKAUDIOFILE_LAYOUT_AC3_3_1                   ((151<<16) | 4)                       // L C R Cs
#define PLANKAUDIOFILE_LAYOUT_AC3_3_0_1                 ((152<<16) | 4)                       // L C R LFE
#define PLANKAUDIOFILE_LAYOUT_AC3_2_1_1                 ((153<<16) | 4)                       // L R Cs LFE
#define PLANKAUDIOFILE_LAYOUT_AC3_3_1_1                 ((154<<16) | 5)                       // L C R Cs LFE
#define PLANKAUDIOFILE_LAYOUT_EAC_6_0_A                 ((155<<16) | 6)                       // L C R Ls Rs Cs
#define PLANKAUDIOFILE_LAYOUT_EAC_7_0_A                 ((156<<16) | 7)                       // L C R Ls Rs Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_EAC3_6_1_A                ((157<<16) | 7)                       // L C R Ls Rs LFE Cs
#define PLANKAUDIOFILE_LAYOUT_EAC3_6_1_B                ((158<<16) | 7)                       // L C R Ls Rs LFE Ts
#define PLANKAUDIOFILE_LAYOUT_EAC3_6_1_C                ((159<<16) | 7)                       // L C R Ls Rs LFE Vhc
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_A                ((160<<16) | 8)                       // L C R Ls Rs LFE Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_B                ((161<<16) | 8)                       // L C R Ls Rs LFE Lc Rc
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_C                ((162<<16) | 8)                       // L C R Ls Rs LFE Lsd Rsd
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_D                ((163<<16) | 8)                       // L C R Ls Rs LFE Lw Rw
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_E                ((164<<16) | 8)                       // L C R Ls Rs LFE Vhl Vhr
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_F                ((165<<16) | 8)                        // L C R Ls Rs LFE Cs Ts
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_G                ((166<<16) | 8)                        // L C R Ls Rs LFE Cs Vhc
#define PLANKAUDIOFILE_LAYOUT_EAC3_7_1_H                ((167<<16) | 8)                        // L C R Ls Rs LFE Ts Vhc
#define PLANKAUDIOFILE_LAYOUT_DTS_3_1                   ((168<<16) | 4)                        // C L R LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_4_1                   ((169<<16) | 5)                        // C L R Cs LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_6_0_A                 ((170<<16) | 6)                        // Lc Rc L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DTS_6_0_B                 ((171<<16) | 6)                        // C L R Rls Rrs Ts
#define PLANKAUDIOFILE_LAYOUT_DTS_6_0_C                 ((172<<16) | 6)                        // C Cs L R Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_DTS_6_1_A                 ((173<<16) | 7)                        // Lc Rc L R Ls Rs LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_6_1_B                 ((174<<16) | 7)                        // C L R Rls Rrs Ts LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_6_1_C                 ((175<<16) | 7)                        // C Cs L R Rls Rrs LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_7_0                   ((176<<16) | 7)                        // Lc C Rc L R Ls Rs
#define PLANKAUDIOFILE_LAYOUT_DTS_7_1                   ((177<<16) | 8)                        // Lc C Rc L R Ls Rs LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_8_0_A                 ((178<<16) | 8)                        // Lc Rc L R Ls Rs Rls Rrs
#define PLANKAUDIOFILE_LAYOUT_DTS_8_0_B                 ((179<<16) | 8)                        // Lc C Rc L R Ls Cs Rs
#define PLANKAUDIOFILE_LAYOUT_DTS_8_1_A                 ((180<<16) | 9)                        // Lc Rc L R Ls Rs Rls Rrs LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_8_1_B                 ((181<<16) | 9)                        // Lc C Rc L R Ls Cs Rs LFE
#define PLANKAUDIOFILE_LAYOUT_DTS_6_1_D                 ((182<<16) | 7)                        // C L R Ls Rs LFE Cs
#define PLANKAUDIOFILE_LAYOUT_DISCRETE                  ((147<<16) | 0)                        // needs to be ORed with the actual number of channels
#define PLANKAUDIOFILE_LAYOUT_UNKNOWN                   0xFFFF0000                             // needs to be ORed with the actual number of channels

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

#define PLANKAUDIOFILE_WAV_LOOPTYPE_FORWARD         0
#define PLANKAUDIOFILE_WAV_LOOPTYPE_PINGPONG        1
#define PLANKAUDIOFILE_WAV_LOOPTYPE_REVERESE        2

#define PLANKAUDIOFILE_LOOPTYPE_FORWARD             PLANKAUDIOFILE_WAV_LOOPTYPE_FORWARD
#define PLANKAUDIOFILE_LOOPTYPE_PINGPONG            PLANKAUDIOFILE_WAV_LOOPTYPE_PINGPONG
#define PLANKAUDIOFILE_LOOPTYPE_REVERESE            PLANKAUDIOFILE_LOOPTYPE_REVERESE

#define PLANKAUDIOFILE_AIFF_LOOPTYPE_NOLOOP         0 
#define PLANKAUDIOFILE_AIFF_LOOPTYPE_FORWARD        1 
#define PLANKAUDIOFILE_AIFF_LOOPTYPE_PINGPONG       2

#define PLANKAUDIOFILE_REGIONNAME_SEPARATOR         "#"


#if PLANK_OGGVORBIS
    #ifndef OV_EXCLUDE_STATIC_CALLBACKS
        #define OV_EXCLUDE_STATIC_CALLBACKS
    #endif

    #include "../../../../ext/vorbis/vorbis/vorbisenc.h"
    #include "../../../../ext/vorbis/vorbis/codec.h"
    #include "../../../../ext/vorbis/vorbis/vorbisfile.h"

/*
 
                      Num-coupled   Force narrow     The channel map
one channel : 1     { 0,            0,               0               },
the stream is monophonic
 
two channels : 2    { 1,            0,               0,1             },
the stream is stereo. channel order: left, right

three channels : 3  { 1,            0,               0,2,1           },
the stream is a 1d-surround encoding. channel order: left, center, right

four channels : 4   { 2,            0,               0,1,2,3         },
the stream is quadraphonic surround. channel order: front left, front right, rear left, rear right

five channels : 5   { 2,            0,               0,4,1,2,3       },
the stream is five-channel surround. channel order: front left, center, front right, rear left, rear right

six channels : 6    { 2,            1<<3,            0,4,1,2,3,5     },
the stream is 5.1 surround. channel order: front left, center, front right, rear left, rear right, LFE

seven channels : 7  { 2,            1<<4,            0,4,1,2,3,5,6   },
the stream is 6.1 surround. channel order: front left, center, front right, side left, side right, rear center, LFE

eight channels : 8  { 3,            1<<4,            0,6,1,2,3,4,5,7 }
the stream is 7.1 surround. channel order: front left, center, front right, side left, side right, rear left, rear right, LFE
 
 
 Stream count 'N'
 
 This field indicates the total number of streams so the decoder can correctly parse the packed Opus packets inside the Ogg packet.
 
 For channel mapping family 0, this value defaults to 1, and is not coded.
 
 A multi-channel Opus file is composed of one or more individual Opus streams, each of which produce one or two channels of decoded data. Each Ogg packet contains one Opus packet from each stream. The first N-1 Opus packets are packed using the self-delimiting framing from Appendix B of the Opus Specification. The remaining Opus packet is packed using the regular, undelimited framing from Section 3 of the Opus Specification. All the Opus packets in a single Ogg packet MUST be constrained to produce the same number of decoded samples. A decoder SHOULD treat any Opus packet whose duration is different from that of the first Opus packet in an Ogg packet as if it were an Opus packet with an illegal TOC sequence.
 
 Two-channel stream count 'M'
 
 Describes the number of streams whose decoders should be configured to produce two channels. This must be no larger than the number of total streams.
 
 For channel mapping family 0, this value defaults to c-1 (i.e., 0 for mono and 1 for stereo), and is not coded.
 
 Each packet in an Opus stream has an internal channel count of 1 or 2, which can change from packet to packet. This is selected by the encoder depending on the bitrate and the contents being encoded. The original channel count of the encoder input is not preserved by the lossy compression.
 
 Regardless of the internal channel count, any Opus stream may be decoded as mono (a single channel) or stereo (two channels) by appropriate initialization of the decoder. The "two-channel stream count" field indicates that the first M Opus decoders should be initialized in stereo mode, and the remaining N-M decoders should be initialized in mono mode. The total number of decoded channels (M+N) MUST be no larger than 255, as there is no way to index more channels than that in the channel mapping.
*/

#endif

#if PLANK_OPUS
    #include "../../../../ext/ogg/ogg.h"
    #include "../../../../ext/opus/opusfile/opusfile.h"
    #include "../../../../ext/opus/opus_header.h"
    #define PLANKAUDIOFILE_OPUS_VENDOR    "libopus 1.01 in Plank " PLANK_VERSION
    #define PLANKAUDIOFILE_OPUS_MAXFRAMESIZE                           (960 * 6)
    #define PLANKAUDIOFILE_OPUS_MAXPACKETSIZE                     (1275 * 3 + 7)
    #define PLANKAUDIOFILE_OPUS_MAXSEGMENTS                                  255
    #define PLANKAUDIOFILE_OPUS_FLUSHFILLSIZE                        (255 * 255)
    #define PLANKAUDIOFILE_OPUS_DELAY                                        312
    #define PLANKAUDIOFILE_OPUS_READBYTES                                    200
    #define PLANKAUDIOFILE_OPUS_MINSAMPLERATE                               8000
    #define PLANKAUDIOFILE_OPUS_MAXSAMPLERATE                             192000
    #define PLANKAUDIOFILE_OPUS_DEFAULTSAMPLERATE                          48000
    #define PLANKAUDIOFILE_OPUS_HEAD                                  "OpusHead"
    #define PLANKAUDIOFILE_OPUS_TAGS                                  "OpusTags"
    #define PLANKAUDIOFILE_OPUS_HEAD_LEN                                       8
    #define PLANKAUDIOFILE_OPUS_PREROLL_MS                                    80
    #define PALNKAUDIOFILE_OPUS_HEADERNUMSEGSPOS                              26
#endif


#define PLANKAUDIOFILE_CHANNELFLAGS_ALLOFF                  0
#define PLANKAUDIOFILE_CHANNELFLAGS_RECTANGULARCOORDINATES  (1<<0)
#define PLANKAUDIOFILE_CHANNELFLAGS_SPHERICALCOORDINATES    (1<<1)
#define PLANKAUDIOFILE_CHANNELFLAGS_METERS                  (1<<2)

typedef struct PlankSpeakerPosition
{
    PlankUI flags;
    PlankVec3F coords;
} PlankSpeakerPosition;

typedef struct PlankAudioFileFormatInfo
{
    PlankC format;
    PlankC encoding;
    
    PlankI bitsPerSample;
    PlankI bytesPerFrame;
    PlankI minimumBitRate;
    PlankI maximumBitRate;
    PlankI nominalBitRate;
    PlankD sampleRate;
    PlankD frameDuration;
    PlankF quality;
        
    PlankDynamicArray channelIdentifiers;
//    PlankDynamicArray channelMap;
    PlankDynamicArray channelCoords;

} PlankAudioFileFormatInfo;

typedef struct PlankAudioFileFormatInfo* PlankAudioFileFormatInfoRef;


//static inline PlankB pl_AudioFileFormatInfo_IsChannelMapClear (const PlankAudioFileFormatInfo* info)
//{
//    int i;
//    
//    for (i = 0; i < info->numChannels; ++i)
//    {
//        if (info->channelMap[i] != PLANKAUDIOFILE_CHANNEL_NONE)
//            return PLANK_FALSE;
//    }
//    
//    return PLANK_TRUE;
//}


static PlankResult pl_AudioFileFormatInfo_Init (PlankAudioFileFormatInfoRef formatInfo)
{
    pl_MemoryZero (formatInfo, sizeof (PlankAudioFileFormatInfo));
    return PlankResult_OK;
}

static PlankResult pl_AudioFileFormatInfo_DeInit (PlankAudioFileFormatInfoRef formatInfo)
{
    pl_DynamicArray_DeInit (&formatInfo->channelIdentifiers);
//    pl_DynamicArray_DeInit (&formatInfo->channelMap);
    pl_DynamicArray_DeInit (&formatInfo->channelCoords);
    pl_MemoryZero (formatInfo, sizeof (PlankAudioFileFormatInfo));
    return PlankResult_OK;
}

static PlankUI* pl_AudioFileFormatInfo_GetChannelIdentifiers (PlankAudioFileFormatInfoRef formatInfo)
{
    return (PlankUI*)pl_DynamicArray_GetArray (&formatInfo->channelIdentifiers);
}

//static PlankUS* pl_AudioFileFormatInfo_GetChannelMap (PlankAudioFileFormatInfoRef formatInfo)
//{
//    return (PlankUS*)pl_DynamicArray_GetArray (&formatInfo->channelMap);
//}

static PlankSpeakerPosition* pl_AudioFileFormatInfo_GetChannelCoords (PlankAudioFileFormatInfoRef formatInfo)
{
    return (PlankSpeakerPosition*)pl_DynamicArray_GetArray (&formatInfo->channelCoords);
}

static PlankUI pl_AudioFileFormatInfo_GetChannelItentifier (PlankAudioFileFormatInfoRef formatInfo, const int channel)
{
    PlankUI channelIdentifier;
    channelIdentifier = 0;
    pl_DynamicArray_GetItem (&formatInfo->channelIdentifiers, channel, &channelIdentifier);
    return channelIdentifier;
}

static PlankResult pl_AudioFileFormatInfo_SetChannelItentifier (PlankAudioFileFormatInfoRef formatInfo, const int channel, const PlankUI channelIdentifier)
{
    PlankResult result;
    
    result = pl_DynamicArray_SetItem (&formatInfo->channelIdentifiers, channel, (void*)&channelIdentifier);
    
exit:
    return result;
}

//static PlankUS pl_AudioFileFormatInfo_GetChannelMapForChannel (PlankAudioFileFormatInfoRef formatInfo, const int channel)
//{
//    PlankUS map;
//    map = 0xFFFF;
//    pl_DynamicArray_GetItem (&formatInfo->channelMap, channel, &map);
//    return map;
//}

static int pl_AudioFileFormatInfo_GetNumChannels (PlankAudioFileFormatInfoRef formatInfo)
{
    return (int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
}

static PlankResult pl_AudioFileFormatInfo_SetNumChannels (PlankAudioFileFormatInfoRef formatInfo, const int numChannels, const PlankB useCoords)
{
//    int original, i;
//    PlankUS* map;
    
    // resize identifiers
    if (formatInfo->channelIdentifiers.itemSize)
        pl_DynamicArray_SetSize (&formatInfo->channelIdentifiers, numChannels);
    else
        pl_DynamicArray_InitWithItemSizeAndSize (&formatInfo->channelIdentifiers, sizeof(PlankUI), numChannels, PLANK_TRUE);
    
//    // resize map
//    if (formatInfo->channelMap.itemSize)
//    {
//        original = (int)pl_DynamicArray_GetSize (&formatInfo->channelMap);
//        pl_DynamicArray_SetSize (&formatInfo->channelMap, numChannels);
//        
//        if (original < numChannels)
//        {
//            map = pl_AudioFileFormatInfo_GetChannelMap (formatInfo);
//            for (i = original; i < numChannels; ++i)
//                map[i] = (PlankUS)i;
//        }
//    }
//    else
//    {
//        pl_DynamicArray_InitWithItemSizeAndSize (&formatInfo->channelMap, sizeof (PlankUS), numChannels, PLANK_FALSE);
//        map = (PlankUS*)pl_DynamicArray_GetArray (&formatInfo->channelMap);
//        
//        for (i = 0; i < numChannels; ++i)
//            map[i] = (PlankUS)i;
//    }
    
    if (useCoords)
    {
        if (formatInfo->channelCoords.itemSize)
            pl_DynamicArray_SetSize (&formatInfo->channelCoords, numChannels);
        else
            pl_DynamicArray_InitWithItemSizeAndSize (&formatInfo->channelCoords, sizeof (PlankUI), numChannels, PLANK_TRUE);
    }
    
    return PlankResult_OK;
}

static void pl_AudioFileFormatInfo_SetDiscreteLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankUI* channelIdentfiers;
    PlankUI numChannels, i;
    
    numChannels = (int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
    
    if (numChannels > 0)
    {
        channelIdentfiers = (PlankUI*)pl_DynamicArray_GetArray (&formatInfo->channelIdentifiers);
        
        for (i = 0; i < numChannels; ++i)
            channelIdentfiers[i] = PLANKAUDIOFILE_CHANNEL_DISCRETE_N | i;
    }
}

static void pl_AudioFileFormatInfo_SetSimpleLayout (PlankAudioFileFormatInfo* formatInfo)
{
    PlankUI* channelIdentfiers;
    PlankUI numChannels;
    
    numChannels = (int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
    channelIdentfiers = (PlankUI*)pl_DynamicArray_GetArray (&formatInfo->channelIdentifiers);
   
    switch (numChannels)
    {
        case 1:
            channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_MONO;
            break;
        case 2:
            channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            break;
    }
}

static void pl_AudioFileFormatInfo_WAV_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankUI* channelIdentfiers;
    PlankUI numChannels, i;
    
    numChannels = (int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
    
    if (numChannels > 0)
    {
        channelIdentfiers = (PlankUI*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        if (numChannels > 18)
        {
            pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
        else
        {
            for (i = 0; i < numChannels; ++i)
                channelIdentfiers[i] = i + 1;
        }
    }
}

static void pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankUI* channelIdentfiers;
    PlankUI numChannels;

    numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {
        channelIdentfiers = (PlankUI*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);

        switch (numChannels)
        {
            case 1:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_MONO;
                break;
            case 2:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case 3:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentfiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case 4:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentfiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentfiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case 5:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentfiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentfiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentfiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case 6:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentfiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentfiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentfiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                channelIdentfiers[5] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case 7:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentfiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentfiers[3] = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                channelIdentfiers[4] = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                channelIdentfiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                channelIdentfiers[6] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case 8:
                channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentfiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentfiers[3] = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                channelIdentfiers[4] = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                channelIdentfiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentfiers[6] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                channelIdentfiers[7] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            default:
                pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
    }
}

static void pl_AudioFileFormatInfo_Opus_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    return pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (formatInfo);
}

static void pl_AudioFileFormatInfo_WAVCAF_ChannelMaskToIdentifiers (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask)
{
    PlankUI bit, mask;
    PlankUI* channelIdentfiers;
    int channel, numChannels;
    
    numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {    
        channelIdentfiers = (PlankUI*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        pl_MemoryZero (channelIdentfiers, numChannels * sizeof (PlankUI));

        bit     = 0;
        channel = 0;
        
        while ((bit < 31) && (channel < numChannels))
        {
            mask = ((PlankUI)1) << bit;
            
            if (mask & channelMask)
            {
                channelIdentfiers[channel] = bit + 1;
                ++channel;
            }
            
            ++bit;
        }
    }
}

static char* pl_AudioFileFormatInfoChannelIdentifierToName (const PlankUI identifier, char* name, const int nameLength)
{
    // just get the uppercase chars only for the abbreviations
    static const char* names[] = {
        "None",
        "Front Left",
        "Front Right",
        "Front Center",
        "Low Frequency",
        "Back Left",
        "Back Right",
        "Front Left of Center",
        "Front Right of Center",
        "Back Center",
        "Side Left",
        "Side Right",
        "Top Center",
        "Top Front Left",
        "Top Front Center",
        "Top Front Right",
        "Top Back Left",
        "Top Back Center",
        "Top Back Right", // 18
        "Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown",
        "Rear Surround Left", // 33
        "Rear Surround Right", 
        "Left Wide", 
        "Right Wide", 
        "Low Frequency 2", 
        "Left Total", 
        "Right Total", 
        "Hearing Impaired", 
        "Voice Over", // PLANKAUDIOFILE_CHANNEL_NARRATION
        "Mono", 
        "Dialogue Centric Mix", 
        "Center Surround Direct", 
        "Vibration" // PLANKAUDIOFILE_CHANNEL_HAPTIC
    };
        
    if (identifier <= PLANKAUDIOFILE_CHANNEL_HAPTIC)
    {
        return strncpy (name, names[identifier], nameLength);
    }
    else if ((identifier & 0xFFFF0000) == PLANKAUDIOFILE_CHANNEL_DISCRETE_N)
    {
        snprintf (name, nameLength, "Discrete %u", identifier & 0x0000FFFF);
        return name;
    }
    else
    {
        switch (identifier)
        {
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_W:        return strncpy (name, "Ambisonic W", nameLength);
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_X:        return strncpy (name, "Ambisonic X", nameLength);
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_Y:        return strncpy (name, "Ambisonic Y", nameLength);
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_Z:        return strncpy (name, "Ambisonic Z", nameLength);
            case PLANKAUDIOFILE_CHANNEL_MS_MID:             return strncpy (name, "MS M", nameLength);
            case PLANKAUDIOFILE_CHANNEL_MS_SIDE:            return strncpy (name, "MS S", nameLength);
            case PLANKAUDIOFILE_CHANNEL_XY_X:               return strncpy (name, "XY X", nameLength);
            case PLANKAUDIOFILE_CHANNEL_XY_Y:               return strncpy (name, "XY Y", nameLength);
            case PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT:    return strncpy (name, "Head Phones L", nameLength);
            case PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT:   return strncpy (name, "Head Phones R", nameLength);
            case PLANKAUDIOFILE_CHANNEL_CLICK_TRACK:        return strncpy (name, "Click Track", nameLength);
            case PLANKAUDIOFILE_CHANNEL_FOREIGN_LANGUAGE:   return strncpy (name, "Foreign Language", nameLength);
            case PLANKAUDIOFILE_CHANNEL_DISCRETE:           return strncpy (name, "Discrete", nameLength);
        }
    }
    
    return strncpy (name, "Unknown", nameLength);
}

static char* pl_AudioFileFormatInfoAbbreviateIdentifierName (const char* name, char* abbrev, const int abbrevLength)
{
    int i, length;
    
    i = 0;
    length = strlen (name);
    
    while ((i < length) && (i < (abbrevLength - 1)))
    {
        if (isupper (name[i]) || isdigit (name[i]))
            abbrev[i++] = name[i];
    }
    
    abbrev[i] = '\0';
    
    return abbrev;
}

static PlankResult pl_AudioFileFormatInfo_CAF_LayoutTagToChannelMap (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelLayoutTag)
{
    PlankResult result = PlankResult_OK;
    PlankUI* channelIdentifiers;
    
    channelIdentifiers = (PlankUI*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
    
    switch (channelLayoutTag)
    {
        case PLANKAUDIOFILE_LAYOUT_MONO:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_MONO;
            break;
        case PLANKAUDIOFILE_LAYOUT_STEREO:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_STEREOHEADPHONES:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_MATRIXSTEREO:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL;
            break;
        case PLANKAUDIOFILE_LAYOUT_MIDSIDE:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_MS_MID;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_MS_SIDE;
            break;
        case PLANKAUDIOFILE_LAYOUT_XY:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_XY_X;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_XY_Y;
            break;
        case PLANKAUDIOFILE_LAYOUT_BINAURAL:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_AMBISONIC_B_FORMAT:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_AMBISONIC_W;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_AMBISONIC_X;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_AMBISONIC_Y;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_AMBISONIC_Z;
            break;
        case PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_PENTAGONAL:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_HEXAGONAL:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_OCTAGONAL:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_CUBE:
            //Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.TopBackLeft, Label.TopBackRight, Label.TopBackCenter, Label.TopCenterSurround
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A:
            //Label.Left, Label.Right, Label.Center),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B:
            //Label.Center, Label.Left, Label.Right),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A:
            //Label.Left, Label.Right, Label.Center, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B:
            //Label.Center, Label.Left, Label.Right, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A:
            //Label.Left, Label.Right, Label.Center, Label.LeftSurround, Label.RightSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B:
            //Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.Center),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C:
            //Label.Left, Label.Center, Label.Right, Label.LeftSurround, Label.RightSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D:
            //Label.Center, Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B:
            //Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.Center, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C:
            //Label.Left, Label.Center, Label.Right, Label.LeftSurround, Label.RightSurround, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D:
            //Label.Center, Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A:
            //Label.Left, Label.Right, Label.Center, Label.LFEScreen, Label.LeftSurround, Label.RightSurround, Label.Right),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A:
            //Label.Left, Label.Right, Label.Center, Label.LFEScreen, Label.LeftSurround, Label.RightSurround, Label.LeftCenter, Label.RightCenter),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B:
            //Label.Center, Label.LeftCenter, Label.RightCenter, Label.Left,Label.Right, Label.LeftSurround, Label.RightSurround, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C:
            //Label.Left, Label.Right, Label.Center, Label.LFEScreen, Label.LeftSurround, Label.RightSurround, Label.RearSurroundLeft, Label.RearSurroundRight),
            break;
        case PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1:
            //Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.Center, Label.LFEScreen, Label.LeftCenter, Label.RightCenter),
            break;
        case PLANKAUDIOFILE_LAYOUT_SMPTE_DTV:
            //Label.Left, Label.Right, Label.Center, Label.LFEScreen, Label.LeftSurround, Label.RightSurround, Label.LeftTotal, Label.RightTotal),
            break;
        case PLANKAUDIOFILE_LAYOUT_ITU_2_1:
            //Label.Left, Label.Right, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_ITU_2_2:
            //Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_4:
            //Label.Left, Label.Right, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_5:
            //Label.Left, Label.Right, Label.LFEScreen, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_6:
            //Label.Left, Label.Right, Label.LFEScreen, Label.LeftSurround, Label.RightSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_10:
            //Label.Left, Label.Right, Label.Center, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_11:
            //Label.Left, Label.Right, Label.Center, Label.LFEScreen,Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_18:
            //Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround,Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0:
            // Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.Center, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0:
            //Label.Left, Label.Right, Label.LeftSurround,Label.RightSurround, Label.Center, Label.RearSurroundLeft, Label.RearSurroundRight),
            break;
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT:
            //// L R Ls Rs C Lc Rc
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_6_0:
            //Label.Center, Label.Left, Label.Right, Label.LeftSurround,Label.RightSurround, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_6_1:
            //Label.Center, Label.Left, Label.Right, Label.LeftSurround, Label.RightSurround, Label.CenterSurround, Label.LFEScreen),
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_7_0:
            //Label.Center, Label.Left, Label.Right, Label.LeftSurround,Label.RightSurround, Label.RearSurroundLeft, Label.RearSurroundRight),
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL:
            //Label.Center, Label.Left, Label.Right, Label.LeftSurround,Label.RightSurround, Label.RearSurroundLeft, Label.RearSurroundRight, Label.CenterSurround),
            break;
        case PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD:
            //abel.Left, Label.Right, Label.Center, Label.Mono, Label.Mono,
            //Label.Mono, Label.LeftSurround, Label.RightSurround, Label.Mono, Label.Mono, Label.Mono, Label.Mono,
            //Label.Mono, Label.CenterSurround, Label.LFEScreen, Label.LFE2),
            break;
        case PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL:
            //Label.LeftCenter, Label.RightCenter, Label.Mono, Label.Mono, Label.Mono), ??
            break;
        case PLANKAUDIOFILE_LAYOUT_AC3_1_0_1:
            // C LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_AC3_3_0:
            // L C R
            break;
        case PLANKAUDIOFILE_LAYOUT_AC3_3_1:
            // L C R Cs
            break;
        case PLANKAUDIOFILE_LAYOUT_AC3_3_0_1:
            // L C R LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_AC3_2_1_1:
            // L R Cs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_AC3_3_1_1:
            // L C R Cs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC_6_0_A:
            // L C R Ls Rs Cs
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC_7_0_A:
            // L C R Ls Rs Rls Rrs
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_6_1_A:
            // L C R Ls Rs LFE Cs
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_6_1_B:
            // L C R Ls Rs LFE Ts
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_6_1_C:
            // L C R Ls Rs LFE Vhc
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_A:
            // L C R Ls Rs LFE Rls Rrs
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_B:
            // L C R Ls Rs LFE Lc Rc
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_C:
            // L C R Ls Rs LFE Lsd Rsd
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_D:
            // L C R Ls Rs LFE Lw Rw
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_E:
            // L C R Ls Rs LFE Vhl Vhr
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_F:
            // L C R Ls Rs LFE Cs Ts
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_G:
            // L C R Ls Rs LFE Cs Vhc
            break;
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_H:
            // L C R Ls Rs LFE Ts Vhc
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_3_1:
            // C L R LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_4_1:
            // C L R Cs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_0_A:
            // Lc Rc L R Ls Rs
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_0_B:
            // C L R Rls Rrs Ts
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_0_C:
            // C Cs L R Rls Rrs
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_A:
            // Lc Rc L R Ls Rs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_B:
            // C L R Rls Rrs Ts LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_C:
            // C Cs L R Rls Rrs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_7_0:
            // Lc C Rc L R Ls Rs
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_7_1:
            // Lc C Rc L R Ls Rs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_8_0_A:
            // Lc Rc L R Ls Rs Rls Rrs
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_8_0_B:
            // Lc C Rc L R Ls Cs Rs
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_8_1_A:
            // Lc Rc L R Ls Rs Rls Rrs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_8_1_B:
            // Lc C Rc L R Ls Cs Rs LFE
            break;
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_D:
            // C L R Ls Rs LFE Cs
            break;
    }
    
exit:
    return result;
}


/** An opaque reference to the <i>Plank AudioFileMetaData</i> object. */
typedef struct PlankAudioFileMetaData* PlankAudioFileMetaDataRef;

/** An opaque reference to the <i>Plank AudioFileCuePoint</i> object. */
typedef struct PlankAudioFileCuePoint* PlankAudioFileCuePointRef;

/** An opaque reference to the <i>Plank AudioFileRegion</i> object. */
typedef struct PlankAudioFileRegion* PlankAudioFileRegionRef;


#endif // PLANK_AUDIOFILECOMMON_H
