/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

        default: return "Invalid";
    }
}



//static PLANK_INLINE_LOW PlankB pl_AudioFileFormatInfo_IsChannelMapClear (const PlankAudioFileFormatInfo* info)
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
    formatInfo->channelLayout = pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (formatInfo);
    
//exit:
    return result;
}

PlankChannelLayout pl_AudioFileFormatInfo_GetChannelLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    return formatInfo->channelLayout;
}

int pl_AudioFileFormatInfo_GetNumChannels (PlankAudioFileFormatInfoRef formatInfo)
{
    return (int)pl_DynamicArray_GetSize (&formatInfo->channelIdentifiers);
}

PlankResult pl_AudioFileFormatInfo_SetNumChannels (PlankAudioFileFormatInfoRef formatInfo, const int numChannels, const PlankB useCoords)
{
    if (numChannels == pl_AudioFileFormatInfo_GetNumChannels (formatInfo))
        return PlankResult_OK;
    
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
    
//    formatInfo->channelLayout = pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (formatInfo);
    formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_UNDEFINED; // until other boiler plate is finished
    
    return PlankResult_OK;
}

void pl_AudioFileFormatInfo_CAF_ChannelMaskToFormat (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask)
{
    PlankUI bit, mask;
    PlankChannelIdentifier* channelIdentfiers;
    PlankChannelIdentifier  channelIdentfier;
    int channel, numChannels;
    
    if (!channelMask)
    {
        pl_AudioFileFormatInfo_SetSimpleLayout (formatInfo);
    }
    else
    {
        numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
        formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_UNDEFINED;

        if (numChannels > 0)
        {
            channelIdentfiers = (PlankUI*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
            
            pl_MemoryZero (channelIdentfiers, numChannels * sizeof (PlankUI));
            
            bit     = 0;
            channel = 0;
            
            while ((bit < 31) && (channel < numChannels))
            {
                mask = ((PlankUI)1) << bit;
                
                if (mask & PLANKAUDIOFILE_CAF_SPEAKER_VALIDBITS)
                {
                    if (mask & channelMask)
                    {
                        channelIdentfier = bit + 1;
                        channelIdentfiers[channel] = channelIdentfier;
                        ++channel;
                    }
                }
                
                ++bit;
            }
            
            formatInfo->channelLayout = pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (formatInfo);
        }
    }
}

void pl_AudioFileFormatInfo_WAV_ChannelMaskToFormat (PlankAudioFileFormatInfoRef formatInfo, const PlankUI channelMask)
{
    PlankUI bit, mask;
    PlankChannelIdentifier* channelIdentfiers;
    PlankChannelIdentifier  channelIdentfier;
    int channel, numChannels;
    
    if (!channelMask)
    {
        pl_AudioFileFormatInfo_SetSimpleLayout (formatInfo);
    }
    else
    {
        numChannels = (int)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
        formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_UNDEFINED;
        
        if (numChannels > 0)
        {
            channelIdentfiers = (PlankUI*)pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
            
            pl_MemoryZero (channelIdentfiers, numChannels * sizeof (PlankUI));
            
            bit     = 0;
            channel = 0;
            
            while ((bit < 31) && (channel < numChannels))
            {            
                mask = ((PlankUI)1) << bit;
                
                if (mask & PLANKAUDIOFILE_WAV_SPEAKER_VALIDBITS)
                {
                    if (mask & channelMask)
                    {
                        channelIdentfier = bit + 1;
                        channelIdentfiers[channel] = channelIdentfier;
                        ++channel;
                    }
                }
                
                ++bit;
            }
            
            formatInfo->channelLayout = pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (formatInfo);
        }
    }
}

const char* pl_AudioFileFormatInfoDiscreteIndexToName (const PlankUS discreteIndex)
{
    static char names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS + 1][16];
    
    if (discreteIndex >= PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS)
    {
        snprintf (names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS], 16, "Discrete %d+", PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS);
        return names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS];
    }
    else
    {
        snprintf (names[discreteIndex], 16, "Discrete %d", discreteIndex);
        return names[discreteIndex];
    }
}

const char* pl_AudioFileFormatInfoUnknownIndexToName (const PlankUS unknownIndex)
{
    static char names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS + 1][16];
    
    if (unknownIndex >= PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS)
    {
        snprintf (names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS], 16, "Unknown %d+", PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS);
        return names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS];
    }
    else
    {
        snprintf (names[unknownIndex], 16, "Unknown %d", unknownIndex);
        return names[unknownIndex];
    }
}

const char* pl_AudioFileFormatInfoPairsIndexToName (const PlankUS pairs)
{
    static char names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS + 1][24];
    
    if (pairs >= PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS)
    {
        snprintf (names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS], 24, "Stereo Pairs %d+", PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS / 2);
        return names[PLANKAUDIOFILE_CHANNEL_MAXINDEXSTRINGS];
    }
    else
    {
        snprintf (names[pairs], 24, "Stereo Pairs %d", pairs / 2);
        return names[pairs];
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
    int i, a, length;
    
    i = 0;
    a = 0;
    length = (int)strlen (name);
    
    while ((i < length) && (i < (abbrevLength - 1)))
    {
        if (isupper (name[i]) || isdigit (name[i]))
            abbrev[a++] = name[i];
        
        ++i;
    }
    
    abbrev[i] = '\0';
    
    return abbrev;
}

const char* pl_AudioFileFormatInfoChannelLayoutToName (const PlankChannelLayout channelLayoutTag)
{
    if ((channelLayoutTag & 0xFFFF0000) == PLANKAUDIOFILE_LAYOUT_UNKNOWN)
    {
        return pl_AudioFileFormatInfoUnknownIndexToName (channelLayoutTag & 0x0000FFFF);
    }
    else if ((channelLayoutTag & PLANKAUDIOFILE_LAYOUT_DISCRETE) == PLANKAUDIOFILE_LAYOUT_DISCRETE)
    {
        return pl_AudioFileFormatInfoDiscreteIndexToName (channelLayoutTag & 0x0000FFFF);
    }
    else if ((channelLayoutTag & PLANKAUDIOFILE_LAYOUT_STEREOPAIRS) == PLANKAUDIOFILE_LAYOUT_STEREOPAIRS)
    {
        return pl_AudioFileFormatInfoPairsIndexToName (channelLayoutTag & 0x0000FFFF);
    }
    else
    {
        switch (channelLayoutTag)
        {
            case PLANKAUDIOFILE_LAYOUT_OGGVORBIS_6_1: return "Vorbis 6.1";
            case PLANKAUDIOFILE_LAYOUT_OGGVORBIS_7_1: return "Vorbis 7.1";
            case PLANKAUDIOFILE_LAYOUT_AIFF_6_0: return "AIFF 6.0";

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
    }
    
    return "None";
}

PlankB pl_AudioFileFormatInfo_LayoutToFormatChannelIdentifiers (PlankAudioFileFormatInfoRef formatInfo, const PlankChannelLayout channelLayoutTag)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels;
    
    numChannels = (PlankUI)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if ((numChannels > 0) && (numChannels == (channelLayoutTag & 0x0000FFFF)))
    {
        channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        if (pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, channelLayoutTag))
        {
            formatInfo->channelLayout = channelLayoutTag;
            return PLANK_TRUE;
        }
    }
    
    return PLANK_FALSE;
}

PlankB pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (PlankChannelIdentifier* channelIdentifiers, const PlankChannelLayout channelLayoutTag)
{
    PlankUI numChannels, i;

    numChannels = (channelLayoutTag & 0x0000FFFF);

    if ((channelLayoutTag & 0xFFFF0000) == PLANKAUDIOFILE_LAYOUT_UNKNOWN)
    {
        for (i = 0; i < numChannels; i++)
        {
            channelIdentifiers[i] = PLANKAUDIOFILE_CHANNEL_NONE;
        }
        
        return PLANK_TRUE;
    }
    else if ((channelLayoutTag & PLANKAUDIOFILE_LAYOUT_DISCRETE) == PLANKAUDIOFILE_LAYOUT_DISCRETE)
    {
        for (i = 0; i < numChannels; i++)
        {
            channelIdentifiers[i] = PLANKAUDIOFILE_CHANNEL_DISCRETE_N | i;
        }
        
        return PLANK_TRUE;
    }
    else if ((channelLayoutTag & PLANKAUDIOFILE_LAYOUT_STEREOPAIRS) == PLANKAUDIOFILE_LAYOUT_STEREOPAIRS)
    {
        for (i = 0; i < (numChannels & 0x0000FFFE); i++)
        {
            channelIdentifiers[i] = (i & 1) ? PLANKAUDIOFILE_CHANNEL_STEREO_RIGHT : PLANKAUDIOFILE_CHANNEL_STEREO_LEFT;
        }
        
        if (numChannels & 1)
        {
            channelIdentifiers[numChannels - 1] = PLANKAUDIOFILE_CHANNEL_MONO;
        }
        
        return PLANK_TRUE;
    }
    else
    {
        switch (channelLayoutTag)
        {
            // custom ones first so they're easier to spot...
            case PLANKAUDIOFILE_LAYOUT_OGGVORBIS_6_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_OGGVORBIS_7_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_AIFF_6_0:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            // standard CAF
            case PLANKAUDIOFILE_LAYOUT_MONO:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_MONO;
                break;
            case PLANKAUDIOFILE_LAYOUT_STEREO:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_STEREOHEADPHONES:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MATRIXSTEREO:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL;
                break;
            case PLANKAUDIOFILE_LAYOUT_MIDSIDE:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_MS_MID;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_MS_SIDE;
                break;
            case PLANKAUDIOFILE_LAYOUT_XY:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_XY_X;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_XY_Y;
                break;
            case PLANKAUDIOFILE_LAYOUT_BINAURAL:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_HEADPHONES_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_HEADPHONES_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_AMBISONIC_B_FORMAT:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_AMBISONIC_W;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_AMBISONIC_X;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_AMBISONIC_Y;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_AMBISONIC_Z;
                break;
            case PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_PENTAGONAL:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_HEXAGONAL:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_OCTAGONAL:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_SIDE_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_SIDE_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_CUBE:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_SMPTE_DTV:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LEFT_TOTAL;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_RIGHT_TOTAL;
                break;
            case PLANKAUDIOFILE_LAYOUT_ITU_2_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_ITU_2_2:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_DVD_4:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_DVD_5:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_DVD_6:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_DVD_10:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_DVD_11:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_DVD_18:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_AAC_6_0:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_AAC_6_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_AAC_7_0:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LEFT_WIDE;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_CENTERSURROUND_DIRECT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA;
                break;
            case PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_REAR_SURROUND_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_TOP_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LEFT_WIDE;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_RIGHT_WIDE;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_CENTERSURROUND_DIRECT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY_EXTRA;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT_OF_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_HEARING_IMPAIRED;  // HI=HearingImpaired???
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_NARRATION;         // VI=VisionImpaired???
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_HAPTIC;
                break;
            case PLANKAUDIOFILE_LAYOUT_AC3_1_0_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_AC3_3_0:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                break;
            case PLANKAUDIOFILE_LAYOUT_AC3_3_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                break;
            case PLANKAUDIOFILE_LAYOUT_AC3_3_0_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_AC3_2_1_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_AC3_3_1_1:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_LOW_FREQUENCY;
                break;
            case PLANKAUDIOFILE_LAYOUT_EAC_6_0_A:
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_CENTER;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_LEFT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_RIGHT;
                *channelIdentifiers++ = PLANKAUDIOFILE_CHANNEL_BACK_CENTER;
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
            default:
                return PLANK_FALSE;
        }
    }
    
    return PLANK_TRUE;
}

PlankB pl_AudioFileFormatInfo_ChannelIdentifiersAreDiscrete (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels, i;
        
    numChannels = (PlankUI)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (!numChannels)
        return PLANK_FALSE;

    channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);

    for (i = 0; i < numChannels; ++i)
    {
        if (channelIdentifiers[i] != (PLANKAUDIOFILE_CHANNEL_DISCRETE_N & i))
            return PLANK_FALSE;
    }
    
    return PLANK_TRUE;
}

PlankB pl_AudioFileFormatInfo_ChannelIdentifiersAreStereoPairs (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels, i;
    
    numChannels = (PlankUI)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (!numChannels)
        return PLANK_FALSE;
    
    channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
    
    for (i = 0; i < (numChannels & 0x0000FFFE); ++i)
    {
        if (channelIdentifiers[i] != (PlankUI)((i & 1) ? PLANKAUDIOFILE_CHANNEL_STEREO_RIGHT : PLANKAUDIOFILE_CHANNEL_STEREO_LEFT))
            return PLANK_FALSE;
    }
    
    if (numChannels & 1)
    {
        if (channelIdentifiers[numChannels - 1] != PLANKAUDIOFILE_CHANNEL_MONO)
            return PLANK_FALSE;
    }
    
    return PLANK_TRUE;
}

PlankB pl_AudioFileFormatInfo_ChannelIdentifiersMatchesLayout (PlankAudioFileFormatInfoRef formatInfo, const PlankChannelLayout channelLayoutTag)
{
    PlankChannelIdentifier standardLayout[PLANKAUDIOFILE_LAYOUT_MAXSTANDARDCHANNELS];
    PlankChannelIdentifier* channelIdentifiers;
    int numChannels;

    pl_MemoryZero (standardLayout, PLANKAUDIOFILE_LAYOUT_MAXSTANDARDCHANNELS * sizeof (PlankChannelIdentifier));
    pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (standardLayout, channelLayoutTag);

    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
    return pl_MemoryCompare (standardLayout, channelIdentifiers, numChannels * sizeof (PlankChannelIdentifier));
}

// evil macro but less error prone that type all this boilerplate!
#define PLANKAUDIOFILE_CHECKLAYOUT(FORMAT,LAYOUT) if (pl_AudioFileFormatInfo_ChannelIdentifiersMatchesLayout (FORMAT, LAYOUT)) return LAYOUT;

PlankChannelLayout pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankUI numChannels;
    
    numChannels = (PlankUI)pl_AudioFileFormatInfo_GetNumChannels (formatInfo);

    // need to check for duplicates
    
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_UNDEFINED) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MONO) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_STEREO) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_STEREOHEADPHONES) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MATRIXSTEREO) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MIDSIDE) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_XY) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_BINAURAL) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AMBISONIC_B_FORMAT) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_PENTAGONAL) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_HEXAGONAL) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_OCTAGONAL) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_CUBE) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_1_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_2_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_3_0_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_4_0_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_4_0_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_0_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_0_D) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_1_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_1_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_5_1_D) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_6_1_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_7_1_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_7_1_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_MPEG_7_1_C) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EMAGIC_DEFAULT_7_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_SMPTE_DTV) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_1_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_2_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_2_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_2_2) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_3_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_3_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_3_2) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_3_2_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_ITU_3_4_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_2) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_3) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_4) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_5) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_6) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_7) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_8) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_9) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_10) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_11) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_12) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_13) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_14) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_15) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_16) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_17) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_18) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_19) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DVD_20) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_4) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_8) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_0_FRONT) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_5_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_6_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AUDIOUNIT_7_1_FRONT) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_3_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_QUADRAPHONIC) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_4_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_5_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_5_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_6_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_6_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_7_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_7_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AAC_OCTAGONAL) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_TMH_10_2_STD) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_TMH_10_2_FULL) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AC3_1_0_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AC3_3_0) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AC3_3_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AC3_3_0_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AC3_2_1_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_AC3_3_1_1) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC_6_0_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC_7_0_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_6_1_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_6_1_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_6_1_C) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_A) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_B) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_C) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_D) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_E) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_F) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_G) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_EAC3_7_1_H) else
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_3_1) else                
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_4_1) else                
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_0_A) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_0_B) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_0_C) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_1_A) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_1_B) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_1_C) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_7_0) else                
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_7_1) else                
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_8_0_A) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_8_0_B) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_8_1_A) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_8_1_B) else              
    PLANKAUDIOFILE_CHECKLAYOUT (formatInfo, PLANKAUDIOFILE_LAYOUT_DTS_6_1_D)
    else if (pl_AudioFileFormatInfo_ChannelIdentifiersAreDiscrete (formatInfo))
        return PLANKAUDIOFILE_LAYOUT_DISCRETE | numChannels;
    else if (pl_AudioFileFormatInfo_ChannelIdentifiersAreStereoPairs (formatInfo))
        return PLANKAUDIOFILE_LAYOUT_STEREOPAIRS | numChannels;
        
    return PLANKAUDIOFILE_LAYOUT_UNKNOWN | numChannels;
}

#undef PLANKAUDIOFILE_CHECKLAYOUT

PlankB pl_AudioFileFormatInfoChannelIdentifiersArePair (const PlankChannelIdentifier A, const PlankChannelIdentifier B)
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
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {
        channelIdentfiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        for (i = 0; i < numChannels; ++i)
            channelIdentfiers[i] = PLANKAUDIOFILE_CHANNEL_DISCRETE_N | i;
        
        formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_DISCRETE | numChannels;
    }
}

void pl_AudioFileFormatInfo_SetSimpleLayout (PlankAudioFileFormatInfo* formatInfo)
{
    PlankChannelIdentifier* channelIdentfiers;
    PlankUI numChannels;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    channelIdentfiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
    
    switch (numChannels)
    {
        case 1:
            channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_MONO;
            formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MONO;
            break;
        case 2:
            channelIdentfiers[0] = PLANKAUDIOFILE_CHANNEL_FRONT_LEFT;
            channelIdentfiers[1] = PLANKAUDIOFILE_CHANNEL_FRONT_RIGHT;
            formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_STEREO;
            break;
        default:
            pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
    }
}

//void pl_AudioFileFormatInfo_WAV_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
//{
//    PlankChannelIdentifier* channelIdentfiers;
//    PlankChannelIdentifier channelIdentfier;
//    PlankUI numChannels, i;
//    
//    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
//    
//    if (numChannels > 0)
//    {
//        channelIdentfiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
//        
//        if (numChannels > 18)
//        {
//            pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
//        }
//        else
//        {
//            for (i = 0; i < numChannels; ++i)
//            {
//                channelIdentfier = i + 1;
//                channelIdentfiers[i] = channelIdentfier;
//            }
//            
//            formatInfo->channelLayout = pl_AudioFileFormatInfo_ChannelIdentifiersToLayout (formatInfo);
//        }
//    }
//}

void pl_AudioFileFormatInfo_WAV_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {
        channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        switch (numChannels)
        {
            case 1:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MONO); break;
            case 2:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_STEREO); break;
            case 3:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_AC3_3_0); break;
            case 4:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC); break;
            case 5:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C); break;
            case 6:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C); break;
            case 7:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = (PlankUI)PLANKAUDIOFILE_LAYOUT_OGGVORBIS_6_1); break;
            case 8:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = (PlankUI)PLANKAUDIOFILE_LAYOUT_OGGVORBIS_7_1); break;
            default: pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
    }
}

void pl_AudioFileFormatInfo_AIFF_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {
        channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        switch (numChannels)
        {
            case 1:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MONO); break;
            case 2:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_STEREO); break;
            case 3:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MPEG_3_0_A); break;
            case 4:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC); break;
            case 5:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = (PlankUI)PLANKAUDIOFILE_LAYOUT_MPEG_5_0_A); break; //?? not specified!!
            case 6:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = (PlankUI)PLANKAUDIOFILE_LAYOUT_AIFF_6_0); break;
            default: pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
    }
}

void pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    PlankChannelIdentifier* channelIdentifiers;
    PlankUI numChannels;
    
    numChannels = pl_AudioFileFormatInfo_GetNumChannels (formatInfo);
    
    if (numChannels > 0)
    {
        channelIdentifiers = pl_AudioFileFormatInfo_GetChannelIdentifiers (formatInfo);
        
        switch (numChannels)
        {
            case 1:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MONO); break;
            case 2:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_STEREO); break;
            case 3:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_AC3_3_0); break;
            case 4:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_QUADRAPHONIC); break;
            case 5:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MPEG_5_0_C); break;
            case 6:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = PLANKAUDIOFILE_LAYOUT_MPEG_5_1_C); break;
            case 7:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = (PlankUI)PLANKAUDIOFILE_LAYOUT_OGGVORBIS_6_1); break;
            case 8:  pl_AudioFileFormatInfoLayoutToFormatChannelIdentifiers (channelIdentifiers, formatInfo->channelLayout = (PlankUI)PLANKAUDIOFILE_LAYOUT_OGGVORBIS_7_1); break;
            default: pl_AudioFileFormatInfo_SetDiscreteLayout (formatInfo);
        }
    }
}

void pl_AudioFileFormatInfo_Opus_SetDefaultLayout (PlankAudioFileFormatInfoRef formatInfo)
{
    pl_AudioFileFormatInfo_OggVorbis_SetDefaultLayout (formatInfo);
}

