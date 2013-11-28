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

#ifndef PLONK_AUDIOFILE_H
#define PLONK_AUDIOFILE_H

/** Some audio file common constants. */
class AudioFile
{
public:
    enum Format
    {
        FormatInvalid               = PLANKAUDIOFILE_FORMAT_INVALID,
        FormatUnknown               = PLANKAUDIOFILE_FORMAT_UNKNOWN,
        FormatWAV                   = PLANKAUDIOFILE_FORMAT_WAV,
        FormatAIFF                  = PLANKAUDIOFILE_FORMAT_AIFF,
        FormatAIFC                  = PLANKAUDIOFILE_FORMAT_AIFC,
        FormatOggVorbis             = PLANKAUDIOFILE_FORMAT_OGGVORBIS,
        FormatOpus                  = PLANKAUDIOFILE_FORMAT_OPUS,
        FormatCAF                   = PLANKAUDIOFILE_FORMAT_CAF,
        FormatW64                   = PLANKAUDIOFILE_FORMAT_W64,
        FormatRegion                = PLANKAUDIOFILE_FORMAT_REGION,
        FormatMulti                 = PLANKAUDIOFILE_FORMAT_MULTI,
        FormatArray                 = PLANKAUDIOFILE_FORMAT_ARRAY,
        FormatQueue                 = PLANKAUDIOFILE_FORMAT_QUEUE,
        ForamtCustom                = PLANKAUDIOFILE_FORMAT_CUSTOM
    };
    
    enum EncodingFlag
    {
        EncodingFlagBigEndian       = PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG,
        EncodingFlagPCM             = PLANKAUDIOFILE_ENCODING_PCM_FLAG,
        EncodingFlagFloat           = PLANKAUDIOFILE_ENCODING_FLOAT_FLAG,
        EncodingFlagNonIntervleaved = PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG
    };
    
    enum Encoding
    {
        EncodingInvalid                         = PLANKAUDIOFILE_ENCODING_INVALID,
        EncodingUnknown                         = PLANKAUDIOFILE_ENCODING_UNKNOWN,
        EncodingUnused1                         = PLANKAUDIOFILE_ENCODING_UNUSED1,
                
        EncodingPCMLittleEndian                 = PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN,
        EncodingPCMBigEndian                    = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN,
        EncodingFloatLittleEndian               = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN,
        EncodingFloatBigEndian                  = PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN,
        
        EncodingPCMLittleEndianNonInterleaved   = PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN_NONINTERLEAVED,
        EncodingPCMBigEndianNonInterleaved      = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN_NONINTERLEAVED,
        EncodingFloatLittleEndianNonInterleaved = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN_NONINTERLEAVED,
        EncodingFloatBigEndianNonInterleaved    = PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN_NONINTERLEAVED,
                
        EncodingMin                             = PLANKAUDIOFILE_ENCODING_MIN,
        EncodingMax                             = PLANKAUDIOFILE_ENCODING_MAX
    };
    
    enum WAVOption
    {
        CompressionPCM              = PLANKAUDIOFILE_WAV_COMPRESSION_PCM,
        CompressionFloat            = PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT,
        CompressionExtensible       = PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE
    };
    
    enum CAFOption
    {
        CAFDefaultHeaderPad         = PLANKAUDIOFILE_CAF_DEFAULTHEADERPAD
    };
    
    enum AIFFOption
    {
        AIFCVersion                 = PLANKAUDIOFILE_AIFC_VERSION
    };
    
    enum SampleType
    {
        Invalid,
        Short,
        Int24,
        Int,
        Char,
        Float,
        Double,
        NumSampleTypes
    };
    
    enum Defaults
    {
        DefaultBufferSize = 32768
    };
    
    enum MultiFileTypes
    {
        MultiFileUnknown = PLANKMULITFILE_MODE_UNKNOWN,
        MultiFileArraySequenceOnce = PLANKMULITFILE_MODE_ARRAYSEQUENCEONCE,
        MultiFileArraySequenceLoop = PLANKMULITFILE_MODE_ARRAYSEQUENCELOOP,
        MultiFileArrayRandom = PLANKMULITFILE_MODE_ARRAYRANDOM,
        MultiFileArrayRandomNoRepeat = PLANKMULITFILE_MODE_ARRAYRANDOMNOREPEAT,
        MultiFileArrayIndexRef = PLANKMULITFILE_MODE_ARRAYINDEXREF,
        MultiFileQueue = PLANKMULITFILE_MODE_QUEUE,
        MultiFileCustom = PLANKMULITFILE_MODE_CUSTOM
    };
    
    enum MetaDataOption
    {
        DontConvertCuePointsToRegions = 0,
        ConvertCuePointsToRegions = 1,
        RemoveCuePoints = 2,
        ConvertCuePointsToRegionsRemovingCuePoints = 3
    };
    
    enum MetaDataIOFlags
    {
        MetaDatIOFlagsNone = PLANKAUDIOFILEMETADATA_IOFLAGS_NONE,                           //0x00000000
        MetaDatIOFlagsCuePoints = PLANKAUDIOFILEMETADATA_IOFLAGS_CUEPOINTS,                 //0x00000001
        MetaDatIOFlagsRegions = PLANKAUDIOFILEMETADATA_IOFLAGS_REGIONS,                     //0x00000002
        MetaDatIOFlagsLoops = PLANKAUDIOFILEMETADATA_IOFLAGS_LOOPS,                         //0x00000004
        MetaDatIOFlagsText = PLANKAUDIOFILEMETADATA_IOFLAGS_TEXT,                           //0x00000008
        MetaDatIOFlagssSampler = PLANKAUDIOFILEMETADATA_IOFLAGS_SAMPLER,                    //0x00000010
        MetaDatIOFlagsExtraSamplerData = PLANKAUDIOFILEMETADATA_IOFLAGS_EXTRASAMPLERDATA,   //0x00000020
        MetaDatIOFlagsInfo = PLANKAUDIOFILEMETADATA_IOFLAGS_INFO,                           //0x00000100
        MetaDatIOFlagsNormal = PLANKAUDIOFILEMETADATA_IOFLAGS_NORMAL,                       //0x0000013F
        MetaDatIOFlagsOverview = PLANKAUDIOFILEMETADATA_IOFLAGS_OVERVIEW,                   //0x00001000
        MetaDatIOFlagsCustom = PLANKAUDIOFILEMETADATA_IOFLAGS_CUSTOM,                       //0x80000000
        MetaDatIOFlagsAll = PLANKAUDIOFILEMETADATA_IOFLAGS_ALL                              //0xFFFFFFFF
    };
    
    enum CuePointTypes
    {
        CuePointUnknown = PLANKAUDIOFILE_CUEPOINTTYPE_UNKNOWN,
        CuePointCuePoint = PLANKAUDIOFILE_CUEPOINTTYPE_CUEPOINT,
        CuePointInsertPoint = PLANKAUDIOFILE_CUEPOINTTYPE_INSERTPOINT,
        CuePointPlayPosition = PLANKAUDIOFILE_CUEPOINTTYPE_PLAYPOSITION,
        CuePointRegionStart = PLANKAUDIOFILE_CUEPOINTTYPE_REGIONSTART,
        CuePointRegionEnd = PLANKAUDIOFILE_CUEPOINTTYPE_REGIONEND
    };
    
    enum RegionTypes
    {
        RegionUnknown = PLANKAUDIOFILE_REGIONTYPE_UNKNOWN,
        RegionRegion = PLANKAUDIOFILE_REGIONTYPE_REGION,
        RegionSelection = PLANKAUDIOFILE_REGIONTYPE_SELECTION,
        RegionLoop = PLANKAUDIOFILE_REGIONTYPE_LOOP,
        RegionLyrics = PLANKAUDIOFILE_REGIONTYPE_LYRICS
    };
    
    enum LoopTypes
    {
        LoopUnknown = PLANKAUDIOFILE_LOOPTYPE_UNKNOWN,
        LoopForward = PLANKAUDIOFILE_LOOPTYPE_FORWARD,
        LoopPingPong = PLANKAUDIOFILE_LOOPTYPE_PINGPONG,
        LoopReverse = PLANKAUDIOFILE_LOOPTYPE_REVERESE
    };
    
    enum ChannelIdentifiers
    {
        ChannelNone = PLANKAUDIOFILE_CHANNEL_NONE,
        ChannelFrontLeft = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT,
        ChannelFrontRight = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT,
        ChannelFrontCenter = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER,
        ChannelLowFrequency = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY,
        ChannelBackLeft = PLANKAUDIOFILE_CHANNEL_BACK_LEFT,
        ChannelBackRight = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT,
        ChannelFrontLeftOfCenter = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER,
        ChannelFrontRightOfCenter = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER,
        ChannelBackCenter = PLANKAUDIOFILE_CHANNEL_BACK_CENTER,
        ChannelSideLeft = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT,
        ChannelSideRight = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT,
        ChannelTopCenter = PLANKAUDIOFILE_CHANNEL_TOP_CENTER,
        ChannelTopFrontLeft = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT,
        ChannelTopFrontCenter = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_CENTER,
        ChannelTopFrontRight = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT,
        ChannelTopBackLeft = PLANKAUDIOFILE_CHANNEL_TOP_BACK_LEFT,
        ChannelTopBackCenter = PLANKAUDIOFILE_CHANNEL_TOP_BACK_CENTER,
        ChannelTopBackRight = PLANKAUDIOFILE_CHANNEL_TOP_BACK_RIGHT,
        ChannelUNUSED0 = PLANKAUDIOFILE_CHANNEL_UNUSED0,
        ChannelUNUSED1 = PLANKAUDIOFILE_CHANNEL_UNUSED1,
        ChannelUNUSED2 = PLANKAUDIOFILE_CHANNEL_UNUSED2,
        ChannelUNUSED3 = PLANKAUDIOFILE_CHANNEL_UNUSED3,
        ChannelUNUSED4 = PLANKAUDIOFILE_CHANNEL_UNUSED4,
        ChannelBitStream1Left = PLANKAUDIOFILE_CHANNEL_BITSTREAM_1_LEFT,
        ChannelBitStream1Right = PLANKAUDIOFILE_CHANNEL_BITSTREAM_1_RIGHT,
        ChannelBitStream2Left = PLANKAUDIOFILE_CHANNEL_BITSTREAM_2_LEFT,
        ChannelBitStream2Right = PLANKAUDIOFILE_CHANNEL_BITSTREAM_2_RIGHT,
        ChannelStereoLeft = PLANKAUDIOFILE_CHANNEL_STEREO_LEFT,
        ChannelStereoRight = PLANKAUDIOFILE_CHANNEL_STEREO_RIGHT,
        ChannelReserved = PLANKAUDIOFILE_CHANNEL_RESERVED,
        ChannelRearSurroundLeft = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT,
        ChannelRearSurroundRight = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT,
        ChannelLeftWide = PLANKAUDIOFILE_CHANNEL_LEFT_WIDE,
        ChannelRightWide = PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE,
        ChannelLowFrequencyExtra = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA,
        ChannelLeftTotal = PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL,
        ChannelRightTotal = PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL,
        ChannelHearingImpaired = PLANKAUDIOFILE_CHANNEL_HEARING_IMPAIRED,
        ChannelNarration = PLANKAUDIOFILE_CHANNEL_NARRATION,
        ChannelMono = PLANKAUDIOFILE_CHANNEL_MONO,
        ChannelDialogueCentricMix = PLANKAUDIOFILE_CHANNEL_DIALOGCENTRICMIX,
        ChannelCenterSurroundDirect = PLANKAUDIOFILE_CHANNEL_CENTERSURROUND_DIRECT,
        ChannelHaptic = PLANKAUDIOFILE_CHANNEL_HAPTIC,
        ChannelUseCooords = PLANKAUDIOFILE_CHANNEL_USE_COORDS,
        ChannelAmbisonicW = PLANKAUDIOFILE_CHANNEL_AMBISONIC_W,
        ChannelAmbisonicX = PLANKAUDIOFILE_CHANNEL_AMBISONIC_X,
        ChannelAmbisonicY = PLANKAUDIOFILE_CHANNEL_AMBISONIC_Y,
        ChannelAmbisonicZ = PLANKAUDIOFILE_CHANNEL_AMBISONIC_Z,
        ChannelMSMid = PLANKAUDIOFILE_CHANNEL_MS_MID,
        ChannelMSSide = PLANKAUDIOFILE_CHANNEL_MS_SIDE,
        ChannelXYX = PLANKAUDIOFILE_CHANNEL_XY_X,
        ChannelXYY = PLANKAUDIOFILE_CHANNEL_XY_Y,
        ChannelHeadphonesLeft = PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT,
        ChannelHeadphonesRight = PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT,
        ChannelClickTrack = PLANKAUDIOFILE_CHANNEL_CLICK_TRACK,
        ChannelForeignLanguage = PLANKAUDIOFILE_CHANNEL_FOREIGN_LANGUAGE,
        ChannelDiscrete = PLANKAUDIOFILE_CHANNEL_DISCRETE,
        ChannelDiscreteN = PLANKAUDIOFILE_CHANNEL_DISCRETE_N,
        ChannelUnknown = PLANKAUDIOFILE_CHANNEL_UNKNOWN
    };
    
    enum ChannelLayouts
    {
        LayoutUndeifine = PLANKAUDIOFILE_LAYOUT_UNDEFINED,
        LayoutUseChannelBitMap = PLANKAUDIOFILE_LAYOUT_USECHANNELBITMAP,
        LayoutStandardMinimum = PLANKAUDIOFILE_LAYOUT_STANARDMINIMUM,
        LayoutMono = PLANKAUDIOFILE_LAYOUT_MONO,
        LayoutStereo = PLANKAUDIOFILE_LAYOUT_STEREO,
        LayoutStereoHeadphones = PLANKAUDIOFILE_LAYOUT_STEREOHEADPHONES,
        LayoutMatrixStereo = PLANKAUDIOFILE_LAYOUT_MATRIXSTEREO,
        LayoutMidSide = PLANKAUDIOFILE_LAYOUT_MIDSIDE,
        LayoutXY = PLANKAUDIOFILE_LAYOUT_XY,
        LayoutBinaural = PLANKAUDIOFILE_LAYOUT_BINAURAL,
        LayoutAmbisonicBFormat = PLANKAUDIOFILE_LAYOUT_AMBISONIC_B_FORMAT,
        LayoutQuadraphonic = PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC,
        LayoutPentagonal = PLANKAUDIOFILE_LAYOUT_PENTAGONAL,
        LayoutHexagonal = PLANKAUDIOFILE_LAYOUT_HEXAGONAL,
        LayoutOctagonal = PLANKAUDIOFILE_LAYOUT_OCTAGONAL,
        LayoutCube = PLANKAUDIOFILE_LAYOUT_CUBE,
        LayoutMPEG_1_0 = PLANKAUDIOFILE_LAYOUT_MPEG_1_0,
        LayoutMPEG_2_0 = PLANKAUDIOFILE_LAYOUT_MPEG_2_0,
        LayoutMPEG_3_0_A = PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A,
        LayoutMPEG_3_0_B = PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B,
        LayoutMPEG_4_0_A = PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A,
        LayoutMPEG_4_0_B = PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B,
        LayoutMPEG_5_0_A = PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A,
        LayoutMPEG_5_0_B = PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B,
        LayoutMPEG_5_0_C = PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C,
        LayoutMPEG_5_0_D = PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D,
        LayoutMPEG_5_1_A = PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A,
        LayoutMPEG_5_1_B = PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B,
        LayoutMPEG_5_1_C = PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C,
        LayoutMPEG_5_1_D = PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D,
        LayoutMPEG_6_1_A = PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A,
        LayoutMPEG_7_1_A = PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A,
        LayoutMPEG_7_1_B = PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B,
        LayoutMPEG_7_1_C = PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C,
        LayoutEmagicDefault_7_1 = PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1,
        LayoutSMPTE_DTV = PLANKAUDIOFILE_LAYOUT_SMPTE_DTV,
        LayoutITU_1_0 = PLANKAUDIOFILE_LAYOUT_ITU_1_0,
        LayoutITU_2_0 = PLANKAUDIOFILE_LAYOUT_ITU_2_0,
        LayoutITU_2_1 = PLANKAUDIOFILE_LAYOUT_ITU_2_1,
        LayoutITU_2_2 = PLANKAUDIOFILE_LAYOUT_ITU_2_2,
        LayoutITU_3_0 = PLANKAUDIOFILE_LAYOUT_ITU_3_0,
        LayoutITU_3_1 = PLANKAUDIOFILE_LAYOUT_ITU_3_1,
        LayoutITU_3_2 = PLANKAUDIOFILE_LAYOUT_ITU_3_2,
        LayoutITU_3_2_1 = PLANKAUDIOFILE_LAYOUT_ITU_3_2_1,
        LayoutITU_3_4_1 = PLANKAUDIOFILE_LAYOUT_ITU_3_4_1,
        LayoutDVD_0 = PLANKAUDIOFILE_LAYOUT_DVD_0,
        LayoutDVD_1 = PLANKAUDIOFILE_LAYOUT_DVD_1,
        LayoutDVD_2 = PLANKAUDIOFILE_LAYOUT_DVD_2,
        LayoutDVD_3 = PLANKAUDIOFILE_LAYOUT_DVD_3,
        LayoutDVD_4 = PLANKAUDIOFILE_LAYOUT_DVD_4,
        LayoutDVD_5 = PLANKAUDIOFILE_LAYOUT_DVD_5,
        LayoutDVD_6 = PLANKAUDIOFILE_LAYOUT_DVD_6,
        LayoutDVD_7 = PLANKAUDIOFILE_LAYOUT_DVD_7,
        LayoutDVD_8 = PLANKAUDIOFILE_LAYOUT_DVD_8,
        LayoutDVD_9 = PLANKAUDIOFILE_LAYOUT_DVD_9,
        LayoutDVD_10 = PLANKAUDIOFILE_LAYOUT_DVD_10,
        LayoutDVD_11 = PLANKAUDIOFILE_LAYOUT_DVD_11,
        LayoutDVD_12 = PLANKAUDIOFILE_LAYOUT_DVD_12,
        LayoutDVD_13 = PLANKAUDIOFILE_LAYOUT_DVD_13,
        LayoutDVD_14 = PLANKAUDIOFILE_LAYOUT_DVD_14,
        LayoutDVD_15 = PLANKAUDIOFILE_LAYOUT_DVD_15,
        LayoutDVD_16 = PLANKAUDIOFILE_LAYOUT_DVD_16,
        LayoutDVD_17 = PLANKAUDIOFILE_LAYOUT_DVD_17,
        LayoutDVD_18 = PLANKAUDIOFILE_LAYOUT_DVD_18,
        LayoutDVD_19 = PLANKAUDIOFILE_LAYOUT_DVD_19,
        LayoutDVD_20 = PLANKAUDIOFILE_LAYOUT_DVD_20,
        LayoutAudioUnit_4 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_4,
        LayoutAudioUnit_5 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5,
        LayoutAudioUnit_6 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6,
        LayoutAudioUnit_8 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_8,
        LayoutAudioUnit_5_0 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5_0,
        LayoutAudioUnit_6_0 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0,
        LayoutAudioUnit_7_0 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0,
        LayoutAudioUnit_7_0_Front = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT,
        LayoutAudioUnit_5_1 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5_1,
        LayoutAudioUnit_6_1 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_1,
        LayoutAudioUnit_7_1 = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_1,
        LayoutAudioUnit_7_1_Front = PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_1_FRONT,
        LayoutAAC_3_0 = PLANKAUDIOFILE_LAYOUT_AAC_3_0,
        LayoutAAC_Quadraphonic = PLANKAUDIOFILE_LAYOUT_AAC_QUADRAPHONIC,
        LayoutAAC_4_0 = PLANKAUDIOFILE_LAYOUT_AAC_4_0,
        LayoutAAC_5_0 = PLANKAUDIOFILE_LAYOUT_AAC_5_0,
        LayoutAAC_5_1 = PLANKAUDIOFILE_LAYOUT_AAC_5_1,
        LayoutAAC_6_0 = PLANKAUDIOFILE_LAYOUT_AAC_6_0,
        LayoutAAC_6_1 = PLANKAUDIOFILE_LAYOUT_AAC_6_1,
        LayoutAAC_7_0 = PLANKAUDIOFILE_LAYOUT_AAC_7_0,
        LayoutAAC_7_1 = PLANKAUDIOFILE_LAYOUT_AAC_7_1,
        LayoutAAC_Octagonal = PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL,
        LayoutTMH_10_2_STD = PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD,
        LayoutTMH_10_2_Full = PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL,
        LayoutAC3_1_0_1 = PLANKAUDIOFILE_LAYOUT_AC3_1_0_1,
        LayoutAC3_3_0 = PLANKAUDIOFILE_LAYOUT_AC3_3_0,
        LayoutAC3_3_1 = PLANKAUDIOFILE_LAYOUT_AC3_3_1,
        LayoutAC3_3_0_1 = PLANKAUDIOFILE_LAYOUT_AC3_3_0_1,
        LayoutAC3_2_1_1 = PLANKAUDIOFILE_LAYOUT_AC3_2_1_1,
        LayoutAC3_3_1_1 = PLANKAUDIOFILE_LAYOUT_AC3_3_1_1,
        LayoutEAC_6_0_A = PLANKAUDIOFILE_LAYOUT_EAC_6_0_A,
        LayoutEAC_7_0_A = PLANKAUDIOFILE_LAYOUT_EAC_7_0_A,
        LayoutEAC3_6_1_A = PLANKAUDIOFILE_LAYOUT_EAC3_6_1_A,
        LayoutEAC3_6_1_B = PLANKAUDIOFILE_LAYOUT_EAC3_6_1_B,
        LayoutEAC3_6_1_C = PLANKAUDIOFILE_LAYOUT_EAC3_6_1_C,
        LayoutEAC3_7_1_A = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_A,
        LayoutEAC3_7_1_B = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_B,
        LayoutEAC3_7_1_C = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_C,
        LayoutEAC3_7_1_D = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_D,
        LayoutEAC3_7_1_E = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_E,
        LayoutEAC3_7_1_F = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_F,
        LayoutEAC3_7_1_G = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_G,
        LayoutEAC3_7_1_H = PLANKAUDIOFILE_LAYOUT_EAC3_7_1_H,
        LayoutDTS_3_1 = PLANKAUDIOFILE_LAYOUT_DTS_3_1,
        LayoutDTS_4_1 = PLANKAUDIOFILE_LAYOUT_DTS_4_1,
        LayoutDTS_6_0_A = PLANKAUDIOFILE_LAYOUT_DTS_6_0_A,
        LayoutDTS_6_0_B = PLANKAUDIOFILE_LAYOUT_DTS_6_0_B,
        LayoutDTS_6_0_C = PLANKAUDIOFILE_LAYOUT_DTS_6_0_C,
        LayoutDTS_6_1_A = PLANKAUDIOFILE_LAYOUT_DTS_6_1_A,
        LayoutDTS_6_1_B = PLANKAUDIOFILE_LAYOUT_DTS_6_1_B,
        LayoutDTS_6_1_C = PLANKAUDIOFILE_LAYOUT_DTS_6_1_C,
        LayoutDTS_7_0 = PLANKAUDIOFILE_LAYOUT_DTS_7_0,
        LayoutDTS_7_1 = PLANKAUDIOFILE_LAYOUT_DTS_7_1,
        LayoutDTS_8_0_A = PLANKAUDIOFILE_LAYOUT_DTS_8_0_A,
        LayoutDTS_8_0_B = PLANKAUDIOFILE_LAYOUT_DTS_8_0_B,
        LayoutDTS_8_1_A = PLANKAUDIOFILE_LAYOUT_DTS_8_1_A,
        LayoutDTS_8_1_B = PLANKAUDIOFILE_LAYOUT_DTS_8_1_B,
        LayoutDTS_6_1_D = PLANKAUDIOFILE_LAYOUT_DTS_6_1_D,
        LayoutDiscrete = PLANKAUDIOFILE_LAYOUT_DISCRETE,
        LayoutUnknown = PLANKAUDIOFILE_LAYOUT_UNKNOWN,
        LayoutOggVorbis_6_1 = PLANKAUDIOFILE_LAYOUT_OGGVORBIS_6_1,
        LayoutOggVorbis_7_1 = PLANKAUDIOFILE_LAYOUT_OGGVORBIS_7_1,
        LayoutAIFF_6_0 = PLANKAUDIOFILE_LAYOUT_AIFF_6_0,
        LayoutStereoPairs = PLANKAUDIOFILE_LAYOUT_STEREOPAIRS
    };

};

class ChannelLayout;
class ChannelIdentifier;

class ChannelLayout : public PlonkBase
{
public:
    ChannelLayout (PlankChannelLayout const& value) throw()
    :   peer (value)
    {
    }
    
    inline operator const UnsignedInt& () const throw()
    {
        return static_cast<const UnsignedInt&> (peer);
    }
    
    inline const UnsignedInt& getValue() const throw()
    {
        return static_cast<const UnsignedInt&> (peer);
    }
    
    Text getName() const throw()
    {
        return pl_AudioFileFormatInfoChannelLayoutToName (peer);
    }
    
    UnsignedInt getNumChannels() const throw()
    {
        return static_cast<UnsignedInt> (peer) & 0x0000FFFF;
    }
    
private:
    PlankChannelLayout peer;
    
    ChannelLayout (ChannelIdentifier const&);
};

class ChannelIdentifier : public PlonkBase
{
public:
    ChannelIdentifier (PlankChannelIdentifier const& value) throw()
    :   peer (value)
    {
    }
    
    inline operator const UnsignedInt& () const throw()
    {
        return static_cast<const UnsignedInt&> (peer);
    }
    
    inline const UnsignedInt& getValue() const throw()
    {
        return static_cast<const UnsignedInt&> (peer);
    }
    
    Text getName() const throw()
    {
        return pl_AudioFileFormatInfoChannelIdentifierToName (peer);
    }
    
    bool isPairWith (ChannelIdentifier const& other) const throw()
    {
        return pl_AudioFileFormatInfoChannelIdentifiersArePair (peer, other.peer);
    }
    
private:
    PlankChannelIdentifier peer;
    
    ChannelIdentifier (ChannelLayout const&);
};

class AudioFileMetaDataIOFlags : public PlonkBase
{
public:
    AudioFileMetaDataIOFlags (PlankAudioFileMetaDataIOFlags const& value) throw()
    :   peer (value)
    {
    }

    inline operator const UnsignedInt& () const throw()
    {
        return static_cast<const UnsignedInt&> (peer);
    }
    
    inline const UnsignedInt& getValue() const throw()
    {
        return static_cast<const UnsignedInt&> (peer);
    }
    
    AudioFileMetaDataIOFlags& add (AudioFileMetaDataIOFlags const& other) throw()
    {
        peer = static_cast<PlankAudioFileMetaDataIOFlags> (getValue() | other.getValue());
        return *this;
    }
    
    AudioFileMetaDataIOFlags& remove (AudioFileMetaDataIOFlags const& other) throw()
    {
        peer = static_cast<PlankAudioFileMetaDataIOFlags> (getValue() & ~other.getValue());
        return *this;
    }

    
private:
    PlankAudioFileMetaDataIOFlags peer;
    
    AudioFileMetaDataIOFlags (bool);
};



#endif // PLONK_AUDIOFILE_H
