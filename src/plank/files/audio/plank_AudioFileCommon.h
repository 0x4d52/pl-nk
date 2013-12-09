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

#include "../../maths/plank_Maths.h"
#include "../plank_IffFileCommon.h"

#define PLANKAUDIOFILE_NAMENULL                       "(null)"

#define PLANKAUDIOFILE_CHARBITS                       8

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
#define PLANKAUDIOFILE_WAV_HEADER_LENGTH              (PLANKAUDIOFILE_WAV_FMT_EXTENSIBLE_LENGTH + PLANKAUDIOFILE_WAV_DS64_MINIMUMLENGTH + 8)

typedef PlankUI PlankChannelIdentifier;

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

#define PLANKAUDIOFILE_CHANNEL_USE_COORDS                   100

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

#define PLANKAUDIOFILE_CHANNEL_UNKNOWN                      0xFFFFFFFF

// the table of strings for unknown and discrete layouts/channels
// can be overrided with a preprocessor macro
#ifndef PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS      
#define PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS 128
#endif


// EBU-TECH 3306
#define PLANKAUDIOFILE_WAV_SPEAKER_NONE                     0x00000000

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

#define PLANKAUDIOFILE_WAV_SPEAKER_VALIDBITS                0xF783FFFF
#define PLANKAUDIOFILE_CAF_SPEAKER_VALIDBITS                0x0003FFFF


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

typedef struct PlankCAFStringID {
    
    PlankUI  stringID;
    PlankLL  offset;
} PlankCAFStringID;

#define PLANKAUDIOFILE_CAF_DEFAULTHEADERPAD         4016 // 4028
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

typedef PlankUI PlankChannelLayout;

// essentially the same as CAF
#define PLANKAUDIOFILE_LAYOUT_UNDEFINED                 0
#define PLANKAUDIOFILE_LAYOUT_USECHANNELDESCRIPTIONS    ((0<<16) | 0)
#define PLANKAUDIOFILE_LAYOUT_USECHANNELBITMAP          ((1<<16) | 0)

#define PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM            PLANKAUDIOFILE_LAYOUT_USECHANNELBITMAP

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

#define PLANKAUDIOFILE_LAYOUT_OGGVORBIS_6_1           ((16384<<16) | 7) // front left, center, front right, side left, side right, rear center, LFE
#define PLANKAUDIOFILE_LAYOUT_OGGVORBIS_7_1           ((16385<<16) | 8) // front left, center, front right, side left, side right, rear left, rear right, LFE
#define PLANKAUDIOFILE_LAYOUT_AIFF_6_0                ((16386<<16) | 6) // left, left centre, centre, right, right centre, surround

#define PLANKAUDIOFILE_LAYOUT_STEREOPAIRS               0xFFFE0000                             // needs to be ORed with the actual number of channels


// custom ones.. non-CAF standard may need to update these in the future is CAF adds need formats

#define PLANKAUDIOFILE_LAYOUT_MAXSTANDARDCHANNELS       21

#define PLANKAUDIOFILEMETADATA_TEXTENCODING_ASCII   0
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_UTF8    1
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_UTF16   2
#define PLANKAUDIOFILEMETADATA_TEXTENCODING_LATIN1  0

//#define PLANKAUDIOFILEMETADATA_UNSET  INT_MIN

#define PLANKAUDIOFILEMETADATA_IOFLAGS_NONE                 0x00000000
#define PLANKAUDIOFILEMETADATA_IOFLAGS_CUEPOINTS            0x00000001
#define PLANKAUDIOFILEMETADATA_IOFLAGS_REGIONS              0x00000002
#define PLANKAUDIOFILEMETADATA_IOFLAGS_LOOPS                0x00000004
#define PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT                 0x00000008
#define PLANKAUDIOFILEMETADATA_IOFLAGS_SAMPLER              0x00000010
#define PLANKAUDIOFILEMETADATA_IOFLAGS_EXTRASAMPLERDATA     0x00000020
#define PLANKAUDIOFILEMETADATA_IOFLAGS_INFO                 0x00000100
#define PLANKAUDIOFILEMETADATA_IOFLAGS_NORMAL               0x0000013F

#define PLANKAUDIOFILEMETADATA_IOFLAGS_OVERVIEW             0x00001000
#define PLANKAUDIOFILEMETADATA_IOFLAGS_CUSTOM               0x80000000
#define PLANKAUDIOFILEMETADATA_IOFLAGS_ALL                  0xFFFFFFFF

typedef PlankUI PlankAudioFileMetaDataIOFlags;

#define PLANKAUDIOFILE_CUEPOINTTYPE_UNKNOWN        -1
#define PLANKAUDIOFILE_CUEPOINTTYPE_CUEPOINT        0
#define PLANKAUDIOFILE_CUEPOINTTYPE_INSERTPOINT     1
#define PLANKAUDIOFILE_CUEPOINTTYPE_PLAYPOSITION    2
#define PLANKAUDIOFILE_CUEPOINTTYPE_REGIONSTART     3
#define PLANKAUDIOFILE_CUEPOINTTYPE_REGIONEND       4

#define PLANKAUDIOFILE_REGIONTYPE_UNKNOWN          -1
#define PLANKAUDIOFILE_REGIONTYPE_REGION            0
#define PLANKAUDIOFILE_REGIONTYPE_SELECTION         1
#define PLANKAUDIOFILE_REGIONTYPE_LOOP              2
#define PLANKAUDIOFILE_REGIONTYPE_LYRICS            3

#define PLANKAUDIOFILE_WAV_LOOPTYPE_FORWARD         0
#define PLANKAUDIOFILE_WAV_LOOPTYPE_PINGPONG        1
#define PLANKAUDIOFILE_WAV_LOOPTYPE_REVERESE        2

#define PLANKAUDIOFILE_LOOPTYPE_UNKNOWN            -1
#define PLANKAUDIOFILE_LOOPTYPE_FORWARD             PLANKAUDIOFILE_WAV_LOOPTYPE_FORWARD
#define PLANKAUDIOFILE_LOOPTYPE_PINGPONG            PLANKAUDIOFILE_WAV_LOOPTYPE_PINGPONG
#define PLANKAUDIOFILE_LOOPTYPE_REVERESE            PLANKAUDIOFILE_WAV_LOOPTYPE_REVERESE

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
    #define PLANKAUDIOFILE_OPUS_HEADERNUMSEGSPOS                              26
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
    
    PlankChannelLayout channelLayout;
    PlankDynamicArray channelIdentifiers;
    PlankDynamicArray channelCoords;
    
} PlankAudioFileFormatInfo;

typedef struct PlankAudioFileFormatInfo* PlankAudioFileFormatInfoRef;

//PlankB pl_AudioFileFormatInfo_IsChannelMapClear (const PlankAudioFileFormatInfo* info);

PLANK_BEGIN_C_LINKAGE

const char* pl_PlankAudioFileGetFormatName (int format);

PlankResult pl_AudioFileFormatInfo_Init (PlankAudioFileFormatInfoRef formatInfo);
PlankResult pl_AudioFileFormatInfo_DeInit (PlankAudioFileFormatInfoRef formatInfo);
PlankChannelIdentifier* pl_AudioFileFormatInfo_GetChannelIdentifiers (PlankAudioFileFormatInfoRef formatInfo);
PlankSpeakerPosition* pl_AudioFileFormatInfo_GetChannelCoords (PlankAudioFileFormatInfoRef formatInfo);
PlankChannelIdentifier pl_AudioFileFormatInfo_GetChannelItentifier (PlankAudioFileFormatInfoRef formatInfo, const int channel);
PlankResult pl_AudioFileFormatInfo_SetChannelItentifier (PlankAudioFileFormatInfoRef formatInfo, const int channel, const PlankChannelIdentifier channelIdentifier);
PlankChannelLayout pl_AudioFileFormatInfo_GetChannelLayout (PlankAudioFileFormatInfoRef formatInfo);
int pl_AudioFileFormatInfo_GetNumChannels (PlankAudioFileFormatInfoRef formatInfo);
PlankResult pl_AudioFileFormatInfo_SetNumChannels (PlankAudioFileFormatInfoRef formatInfo, const int numChannels, const PlankB useCoords);
void pl_AudioFileFormatInfo_CAF_ChannelMaskToFormat (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask);
void pl_AudioFileFormatInfo_WAV_ChannelMaskToFormat (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask);
const char* pl_AudioFileFormatInfoDiscreteIndexToName (const PlankUS discreteIndex);
const char* pl_AudioFileFormatInfoChannelIdentifierToName (const PlankChannelIdentifier identifier);
char* pl_AudioFileFormatInfoAbbreviateIdentifierName (const char* name, char* abbrev, const int abbrevLength);
const char* pl_AudioFileFormatInfoChannelLayoutToName (const PlankChannelLayout channelLayoutTag);
PlankB pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (PlankChannelIdentifier* channelIdentifiers, const PlankChannelLayout channelLayoutTag);
PlankB pl_AudioFileFormatInfo_LayoutToFormatChannelIdentifiers (PlankAudioFileFormatInfoRef formatInfo, const PlankChannelLayout channelLayoutTag);
PlankB pl_AudioFileFormatInfo_ChannelIdentifiersMatchesLayout (PlankAudioFileFormatInfoRef formatInfo, const PlankChannelLayout channelLayoutTag);
PlankChannelLayout pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (PlankAudioFileFormatInfoRef formatInfo);
PlankB pl_AudioFileFormatInfoChannelIdentifiersArePair (const PlankChannelIdentifier A, const PlankChannelIdentifier B);
void pl_AudioFileFormatInfo_SetDiscreteLayout (PlankAudioFileFormatInfoRef formatInfo);
void pl_AudioFileFormatInfo_SetSimpleLayout (PlankAudioFileFormatInfoRef formatInfo);
void pl_AudioFileFormatInfo_WAV_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo);
void pl_AudioFileFormatInfo_AIFF_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo);
void pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo);
void pl_AudioFileFormatInfo_Opus_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo);

PLANK_END_C_LINKAGE

/** An opaque reference to the <i>Plank AudioFileMetaData</i> object. */
typedef struct PlankAudioFileMetaData* PlankAudioFileMetaDataRef;

/** An opaque reference to the <i>Plank AudioFileCuePointExtra</i> object. */
typedef struct PlankAudioFileCuePointExtra* PlankAudioFileCuePointExtraRef;

/** An opaque reference to the <i>Plank AudioFileCuePoint</i> object. */
typedef struct PlankAudioFileCuePoint* PlankAudioFileCuePointRef;

/** An opaque reference to the <i>Plank AudioFileRegion</i> object. */
typedef struct PlankAudioFileRegion* PlankAudioFileRegionRef;



#endif // PLANK_AUDIOFILECOMMON_H
