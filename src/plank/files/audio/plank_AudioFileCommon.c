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

#include "../../core/plank_StandardHeader.h"
#include "plank_AudioFileCommon.h"


const char* pl_PlankAudioFileGetFormatName (int format)
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

PlankResult pl_AudioFileFormatInfo_Init (PlankAudioFileFormatInfoRef formatInfo)
{
    pl_MemoryZero (formatInfo, sizeof (PlankAudioFileFormatInfo));
    return PlankResult_OK;
}

PlankResult pl_AudioFileFormatInfo_DeInit (PlankAudioFileFormatInfoRef formatInfo)
{
    pl_DynamicArray_DeInit (&formatInfo->channelIdentifiers);
    pl_DynamicArray_DeInit (&formatInfo->channelCoords);
    pl_MemoryZero (formatInfo, sizeof (PlankAudioFileFormatInfo));
    return PlankResult_OK;
}

PlankChannelIdentifier* pl_AudioFileFormatInfo_GetChannelIdentifiers (PlankAudioFileFormatInfoRef formatInfo)
{
    return (PlankUI*)pl_DynamicArray_GetArray (&formatInfo->channelIdentifiers);
}

PlankSpeakerPosition* pl_AudioFileFormatInfo_GetChannelCoords (PlankAudioFileFormatInfoRef formatInfo)
{
    return (PlankSpeakerPosition*)pl_DynamicArray_GetArray (&formatInfo->channelCoords);
}

PlankChannelIdentifier pl_AudioFileFormatInfo_GetChannelItentifier (PlankAudioFileFormatInfoRef formatInfo, const int channel)
{
    PlankChannelIdentifier channelIdentifier;
    channelIdentifier = 0;
    pl_DynamicArray_GetItem (&formatInfo->channelIdentifiers, channel, &channelIdentifier);
    return channelIdentifier;
}

PlankResult pl_AudioFileFormatInfo_SetChannelItentifier (PlankAudioFileFormatInfoRef formatInfo, const int channel, const PlankChannelIdentifier channelIdentifier)
{
    PlankResult result;
    
    result = pl_DynamicArray_SetItem (&formatInfo->channelIdentifiers, channel, (void*)&channelIdentifier);
    
exit:
    return result;
}

int pl_AudioFileFormatInfo_GetNumChannels (PlankAudioFileFormatInfoRef formatInfo)
{
    return (int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
}

PlankResult pl_AudioFileFormatInfo_SetNumChannels (PlankAudioFileFormatInfoRef formatInfo, const int numChannels, const PlankB useCoords)
{    
    // resize identifiers
    if (formatInfo->channelIdentifiers.itemSize)
        pl_DynamicArray_SetSize (&formatInfo->channelIdentifiers, numChannels);
    else
        pl_DynamicArray_InitWithItemSizeAndSize (&formatInfo->channelIdentifiers, sizeof(PlankUI), numChannels, PLANK_TRUE);
        
    if (useCoords)
    {
        if (formatInfo->channelCoords.itemSize)
            pl_DynamicArray_SetSize (&formatInfo->channelCoords, numChannels);
        else
            pl_DynamicArray_InitWithItemSizeAndSize (&formatInfo->channelCoords, sizeof (PlankUI), numChannels, PLANK_TRUE);
    }
    
    return PlankResult_OK;
}

void pl_AudioFileFormatInfo_CAF_ChannelMaskToFormat (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask)
{
    PlankUI bit, mask;
    PlankChannelIdentifier* channelIdentfiers;
    PlankChannelIdentifier  channelIdentfier;
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
                channelIdentfier = bit + 1;
                channelIdentfiers[channel] = channelIdentfier;
                ++channel;
            }
            
            ++bit;
        }
    }
}

void pl_AudioFileFormatInfo_WAV_ChannelMaskToFormat (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask)
{
    PlankUI bit, mask;
    PlankChannelIdentifier* channelIdentfiers;
    PlankChannelIdentifier  channelIdentfier;
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
                channelIdentfier = bit + 1;
                channelIdentfiers[channel] = channelIdentfier;
                ++channel;
            }
            
            ++bit;
        }
    }
}

const char* pl_AudioFileFormatInfoDiscreteIndexToName (const PlankUS discreteIndex)
{
    static char names[512][16];
    
    if (discreteIndex >= 512)
    {
        return "Discrete 512+";
    }
    else
    {
        snprintf (names[discreteIndex], 16, "Discrete %d", discreteIndex);
        return names[discreteIndex];
    }
}

const char* pl_AudioFileFormatInfoChannelIdentifierToName (const PlankChannelIdentifier identifier)
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
        "Low Frequency Extra", 
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
        return names[identifier];
    }
    else if ((identifier & 0xFFFF0000) == PLANKAUDIOFILE_CHANNEL_DISCRETE_N)
    {
        return pl_AudioFileFormatInfoDiscreteIndexToName (identifier & 0x0000FFFF);
    }
    else
    {
        switch (identifier)
        {
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_W:        return "Ambisonic W";
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_X:        return "Ambisonic X";
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_Y:        return "Ambisonic Y";
            case PLANKAUDIOFILE_CHANNEL_AMBISONIC_Z:        return "Ambisonic Z";
            case PLANKAUDIOFILE_CHANNEL_MS_MID:             return "MS M";
            case PLANKAUDIOFILE_CHANNEL_MS_SIDE:            return "MS S";
            case PLANKAUDIOFILE_CHANNEL_XY_X:               return "XY X";
            case PLANKAUDIOFILE_CHANNEL_XY_Y:               return "XY Y";
            case PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT:    return "Head Phones L";
            case PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT:   return "Head Phones R";
            case PLANKAUDIOFILE_CHANNEL_CLICK_TRACK:        return "Click Track";
            case PLANKAUDIOFILE_CHANNEL_FOREIGN_LANGUAGE:   return "Foreign Language";
            case PLANKAUDIOFILE_CHANNEL_DISCRETE:           return "Discrete";
        }
    }
    
    return "Unknown";
}

char* pl_AudioFileFormatInfoAbbreviateIdentifierName (const char* name, char* abbrev, const int abbrevLength)
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

const char* pl_AudioFileFormatInfoChannelLayoutToName (const PlankChannelLayout channelLayoutTag)
{    
    switch (channelLayoutTag)
    {
        case PLANKAUDIOFILE_LAYOUT_MONO: return "Mono";
        case PLANKAUDIOFILE_LAYOUT_STEREO: return "Stereo";
        case PLANKAUDIOFILE_LAYOUT_STEREOHEADPHONES: return "Stereo Headphones";
        case PLANKAUDIOFILE_LAYOUT_MATRIXSTEREO: return "Matrix Stereo";
        case PLANKAUDIOFILE_LAYOUT_MIDSIDE: return "Mid Side";
        case PLANKAUDIOFILE_LAYOUT_XY: return "XY";
        case PLANKAUDIOFILE_LAYOUT_BINAURAL: return "Binaural";
        case PLANKAUDIOFILE_LAYOUT_AMBISONIC_B_FORMAT: return "B-Format";
        case PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC: return "Quadraphonic";
        case PLANKAUDIOFILE_LAYOUT_PENTAGONAL: return "Pentagonal";
        case PLANKAUDIOFILE_LAYOUT_HEXAGONAL: return "Hexagonal";
        case PLANKAUDIOFILE_LAYOUT_OCTAGONAL: return "Octagonal";
        case PLANKAUDIOFILE_LAYOUT_CUBE: return "Cube";
        case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A: return "MPEG 3.0A";
        case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B: return "MPEG 3.0B";
        case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A: return "MPEG 4.0A";
        case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B: return "MPEG 4.0B";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A: return "MPEG 5.0A";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B: return "MPEG 5.0B";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C: return "MPEG 5.0C";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D: return "MPEG 5.0D";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A: return "MPEG 5.1A";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B: return "MPEG 5.1B";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C: return "MPEG 5.1C";
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D: return "MPEG 5.1D";
        case PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A: return "MPEG 6.1A";
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A: return "MPEG 7.1A";
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B: return "MPEG 7.1B";
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C: return "MPEG 7.1C";
        case PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1: return "Emagic 7.1";
        case PLANKAUDIOFILE_LAYOUT_SMPTE_DTV: return "SMPTE DTV";
        case PLANKAUDIOFILE_LAYOUT_ITU_2_1: return "ITU 2.1";
        case PLANKAUDIOFILE_LAYOUT_ITU_2_2: return "ITU 2.2";
        case PLANKAUDIOFILE_LAYOUT_DVD_4: return "DVD 4";
        case PLANKAUDIOFILE_LAYOUT_DVD_5: return "DVD 5";
        case PLANKAUDIOFILE_LAYOUT_DVD_6: return "DVD 6";
        case PLANKAUDIOFILE_LAYOUT_DVD_10: return "DVD 10";
        case PLANKAUDIOFILE_LAYOUT_DVD_11: return "DVD 11";
        case PLANKAUDIOFILE_LAYOUT_DVD_18: return "DVD 18";
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0: return "AudioUnit 6.0";
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0: return "AudioUnit 7.0";
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT: return "AudioUnit 7.0 Front";
        case PLANKAUDIOFILE_LAYOUT_AAC_6_0: return "AAC 6.0";
        case PLANKAUDIOFILE_LAYOUT_AAC_6_1: return "AAC 6.1";
        case PLANKAUDIOFILE_LAYOUT_AAC_7_0: return "AAC 7.0";
        case PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL: return "AAC Octagonal";
        case PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD: return "TMH 10.2 Standard";
        case PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL: return "TMH 10.2 Full";
        case PLANKAUDIOFILE_LAYOUT_AC3_1_0_1: return "AC3 1.0.1";
        case PLANKAUDIOFILE_LAYOUT_AC3_3_0: return "AC3 3.0";
        case PLANKAUDIOFILE_LAYOUT_AC3_3_1: return "AC3 3.1";
        case PLANKAUDIOFILE_LAYOUT_AC3_3_0_1: return "AC3 3.0.1";
        case PLANKAUDIOFILE_LAYOUT_AC3_2_1_1: return "AC3 2.1.1";
        case PLANKAUDIOFILE_LAYOUT_AC3_3_1_1: return "AC3 3.1.1";
        case PLANKAUDIOFILE_LAYOUT_EAC_6_0_A: return "EAC 6.0A";
        case PLANKAUDIOFILE_LAYOUT_EAC_7_0_A: return "EAC 7.0A";
        case PLANKAUDIOFILE_LAYOUT_EAC3_6_1_A: return "EAC3 6.1A";
        case PLANKAUDIOFILE_LAYOUT_EAC3_6_1_B: return "EAC3 6.1B";
        case PLANKAUDIOFILE_LAYOUT_EAC3_6_1_C: return "EAC3 6.1C";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_A: return "EAC3 7.1A";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_B: return "EAC3 7.1B";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_C: return "EAC3 7.1C";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_D: return "EAC3 7.1D";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_E: return "EAC3 7.1E";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_F: return "EAC3 6.1A";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_G: return "EAC3 7.1G";
        case PLANKAUDIOFILE_LAYOUT_EAC3_7_1_H: return "EAC3 7.1H";
        case PLANKAUDIOFILE_LAYOUT_DTS_3_1: return "DTS 3.1";
        case PLANKAUDIOFILE_LAYOUT_DTS_4_1: return "DTS 4.1";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_0_A: return "DTS 6.0A";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_0_B: return "DTS 6.0B";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_0_C: return "DTS 6.0C";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_A: return "DTS 6.1A";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_B: return "DTS 6.1B";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_C: return "DTS 6.1C";
        case PLANKAUDIOFILE_LAYOUT_DTS_7_0: return "DTS 7.0";
        case PLANKAUDIOFILE_LAYOUT_DTS_7_1: return "DTS 7.1";
        case PLANKAUDIOFILE_LAYOUT_DTS_8_0_A: return "DTS 8.0A";
        case PLANKAUDIOFILE_LAYOUT_DTS_8_0_B: return "DTS 8.0B";
        case PLANKAUDIOFILE_LAYOUT_DTS_8_1_A: return "DTS 8.1A";
        case PLANKAUDIOFILE_LAYOUT_DTS_8_1_B: return "DTS 8.1B";
        case PLANKAUDIOFILE_LAYOUT_DTS_6_1_D: return "DTS 6.1D";
    }
    
    return "Unknown";
}

PlankResult pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (PlankChannelIdentifier* channelIdentifiers, const PlankChannelLayout channelLayoutTag)
{
    PlankResult result = PlankResult_OK;

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
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_TOP_BACK_LEFT;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_TOP_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
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
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_SMPTE_DTV:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL;
            break;
        case PLANKAUDIOFILE_LAYOUT_ITU_2_1:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_ITU_2_2:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_4:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_5:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_6:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_10:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_11:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_DVD_18:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_6_0:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_6_1:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_7_0:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
            break;
        case PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL:
            channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
            channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
            channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            break;
        case PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD:
            channelIdentifiers[ 0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[ 1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[ 2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[ 3] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_CENTER;
            channelIdentifiers[ 4] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
            channelIdentifiers[ 5] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
            channelIdentifiers[ 6] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[ 7] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[ 8] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT;
            channelIdentifiers[ 9] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT;
            channelIdentifiers[10] = PLANKAUDIOFILE_CHANNEL_LEFT_WIDE;
            channelIdentifiers[11] = PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE;
            channelIdentifiers[12] = PLANKAUDIOFILE_CHANNEL_CENTERSURROUND_DIRECT;
            channelIdentifiers[13] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            channelIdentifiers[14] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[15] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA;
            break;
        case PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL:
            channelIdentifiers[ 0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentifiers[ 1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            channelIdentifiers[ 2] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
            channelIdentifiers[ 3] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_CENTER;
            channelIdentifiers[ 4] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
            channelIdentifiers[ 5] = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
            channelIdentifiers[ 6] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
            channelIdentifiers[ 7] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
            channelIdentifiers[ 8] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT;
            channelIdentifiers[ 9] = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT;
            channelIdentifiers[10] = PLANKAUDIOFILE_CHANNEL_LEFT_WIDE;
            channelIdentifiers[11] = PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE;
            channelIdentifiers[12] = PLANKAUDIOFILE_CHANNEL_CENTERSURROUND_DIRECT;
            channelIdentifiers[13] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
            channelIdentifiers[14] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
            channelIdentifiers[15] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA;
            channelIdentifiers[16] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
            channelIdentifiers[17] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
            channelIdentifiers[18] = PLANKAUDIOFILE_CHANNEL_HEARING_IMPAIRED;  // HI=HearingImpaired???
            channelIdentifiers[19] = PLANKAUDIOFILE_CHANNEL_NARRATION;         // VI=VisionImpaired???
            channelIdentifiers[20] = PLANKAUDIOFILE_CHANNEL_HAPTIC;
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

PlankResult pl_AudioFileFormatInfo_LayoutToFormatChannelIdentifiers (PlankAudioFileFormatInfoRef formatInfo, const PlankChannelLayout channelLayoutTag)
{
    PlankChannelIdentifier* channelIdentifiers;    
    channelIdentifiers = (PlankChannelIdentifier*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
    return pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, channelLayoutTag);
}

inline PlankB pl_AudioFileFormatInfo_ChannelIdentifiersMatchesLayout (PlankAudioFileFormatInfoRef formatInfo, const PlankChannelLayout channelLayoutTag)
{
    PlankChannelIdentifier standardLayout[PLANKAUDIOFILE_LAYOUT_MAXSTANDARDCHANNELS];
    PlankChannelIdentifier* channelIdentifiers;
    int numChannels;
    
    pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, channelLayoutTag);
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
    
    return pl_MemoryCompare (standardLayout, channelIdentifiers, numChannels * sizeof (PlankChannelIdentifier));
}


#define PLANKAUDIOFILE_CHECKLAYOUT(FORMAT,LAYOUT) if (pl_AudioFileFormatInfo_ChannelIdentifiersMatchesLayout (FORMAT, LAYOUT)) return LAYOUT;

PlankChannelLayout pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelLayout layout;
        
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MONO) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_STEREO) else
        // etc
    layout = PLANKAUDIOFILE_LAYOUT_UNKNOWN | (PlankUI)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    /// but need to check the discreter ons.
    
    return layout;
}

#undef PLANKAUDIOFILE_CHECKLAYOUT

PlankB pl_AudioFileFormatInfoIsPair (const PlankChannelIdentifier A, const PlankChannelIdentifier B)
{
    PlankChannelIdentifier minChan, maxChan;
    
    minChan = pl_MinUI ((PlankUI)A, (PlankUI)B);
    maxChan = pl_MaxUI ((PlankUI)A, (PlankUI)B);
    
    switch (minChan)
    {
        case PLANKAUDIOFILE_CHANNEL_FRONT_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_BACK_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_BACK_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_SIDE_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_TOP_BACK_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_TOP_BACK_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_BITSTREAM_1_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_BITSTREAM_1_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_BITSTREAM_2_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_BITSTREAM_2_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_STEREO_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_STEREO_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_LEFT_WIDE:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_MS_MID:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_MS_SIDE) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_XY_X:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_XY_Y) return PLANK_TRUE; break;
        case PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT:
            if (maxChan == PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT) return PLANK_TRUE; break;
    }
    
    return PLANK_FALSE;
}

void pl_AudioFileFormatInfo_SetDiscreteLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentfiers;
    PlankUI numChannels, i;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);//(int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
    
    if (numChannels > 0)
    {
        channelIdentfiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);// (PlankChannelIdentifier*)pl_DynamicArray_GetArray (&formatInfo->channelIdentifiers);
        
        for (i = 0; i < numChannels; ++i)
            channelIdentfiers[i] = PLANKAUDIOFILE_CHANNEL_DISCRETE_N | i;
    }
}

void pl_AudioFileFormatInfo_SetSimpleLayout (PlankAudioFileFormatInfo* formatInfo)
{
    PlankChannelIdentifier* channelIdentfiers;
    PlankUI numChannels;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo); //(int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
    channelIdentfiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo); //(PlankChannelIdentifier*)pl_DynamicArray_GetArray (&formatInfo->channelIdentifiers);
    
    switch (numChannels)
    {
        case 1:
            channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_MONO;
            break;
        case 2:
            channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            break;
        default:
            pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
    }
}

void pl_AudioFileFormatInfo_WAV_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentfiers;
    PlankChannelIdentifier channelIdentfier;
    PlankUI numChannels, i;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);//(int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
    
    if (numChannels > 0)
    {
        channelIdentfiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        if (numChannels > 18)
        {
            pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
        else
        {
            for (i = 0; i < numChannels; ++i)
            {
                channelIdentfier = i + 1;
                channelIdentfiers[i] = channelIdentfier;
            }
        }
    }
}

void pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels;
    
    numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {
        channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        switch (numChannels)
        {
            case 1:
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_MONO;
                break;
            case 2:
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case 3: // AC3 3.0
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case 4: // QUAD
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case 5: // MPEG 5.0C
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case 6: // MPEG 5.1C
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case 7:
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case 8:
                channelIdentifiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                channelIdentifiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                channelIdentifiers[2] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                channelIdentifiers[3] = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                channelIdentifiers[4] = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                channelIdentifiers[5] = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                channelIdentifiers[6] = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                channelIdentifiers[7] = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            default:
                pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
    }
}

void pl_AudioFileFormatInfo_Opus_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    return pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (formatInfo);
}

