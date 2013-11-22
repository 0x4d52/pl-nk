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

#ifndef PLANK_AUDIOFILEWRITER_H
#define PLANK_AUDIOFILEWRITER_H

#include "plank_AudioFileCommon.h"

PLANK_BEGIN_C_LINKAGE

/** An audio file writer.
  
 @defgroup PlankAudioFileWriterClass Plank AudioFileWriter class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank AudioFileWriter</i> object. */
typedef struct PlankAudioFileWriter* PlankAudioFileWriterRef; 

/** Create and intitialise a <i>Plank AudioFileWriter</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileWriter</i> object as an opaque reference or PLANK_NULL. */
PlankAudioFileWriterRef pl_AudioFileWriter_CreateAndInit();

/** Create a <i>Plank AudioFileWriter</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileWriter</i> object as an opaque reference or PLANK_NULL. */
PlankAudioFileWriterRef pl_AudioFileWriter_Create();

/** Inititalise a <i>Plank AudioFileWriter</i> object. 
 @param p The <i>Plank AudioFileWriter</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileWriter_Init (PlankAudioFileWriterRef p);

/** Deinitialise a <i>Plank AudioFileWriter</i> object. 
 @param p The <i>Plank AudioFileWriter</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileWriter_DeInit (PlankAudioFileWriterRef p);

/** Destroy a <i>Plank AudioFileWriter</i> object. 
 @param p The <i>Plank AudioFileWriter</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileWriter_Destroy (PlankAudioFileWriterRef p);

/** Gets the underlying <i>Plank %File</i> object. 
 This is the raw file object the is performing the fundamental file access operations.
 @param p The <i>Plank AudioFileWriter</i> object.
 @return The <i>Plank %File</i> object. */
PlankFileRef pl_AudioFileWriter_GetFile (PlankAudioFileWriterRef p);

/** Get format before open. 
 Returns null if the file is already open. */
PlankAudioFileFormatInfo* pl_AudioFileWriter_GetFormatInfo (PlankAudioFileWriterRef p);

const PlankAudioFileFormatInfo* pl_AudioFileWriter_GetFormatInfoReadOnly (PlankAudioFileWriterRef p);

PlankAudioFileMetaDataRef pl_AudioFileWriter_GetMetaData (PlankAudioFileWriterRef p);
PlankResult pl_AudioFileWriter_SetMetaData (PlankAudioFileWriterRef p, PlankAudioFileMetaDataRef metaData);


PlankResult pl_AudioFileWriter_SetFormatWAV (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat);
PlankResult pl_AudioFileWriter_SetFormatAIFF (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate);
PlankResult pl_AudioFileWriter_SetFormatAIFC (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat, const PlankB isLittleEndian);
PlankResult pl_AudioFileWriter_SetFormatCAF (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat, const PlankB isLittleEndian);
PlankResult pl_AudioFileWriter_SetFormatW64 (PlankAudioFileWriterRef p, const int bitsPerSample, const PlankChannelLayout channelLayout, const double sampleRate, const PlankB isFloat);
PlankResult pl_AudioFileWriter_SetFormatOggVorbis (PlankAudioFileWriterRef p, const float quality, const PlankChannelLayout channelLayout, const double sampleRate);
PlankResult pl_AudioFileWriter_SetFormatOggVorbisManaged (PlankAudioFileWriterRef p, const int minBitRate, const int nominalBitRate, const int maxBitRate, const PlankChannelLayout channelLayout, const double sampleRate);
PlankResult pl_AudioFileWriter_SetFormatOpus (PlankAudioFileWriterRef p, const float quality, const PlankChannelLayout channelLayout, const double sampleRate, const double frameDuration);
PlankResult pl_AudioFileWriter_SetFormatOpusManaged (PlankAudioFileWriterRef p, const int nominalBitRate, const PlankChannelLayout channelLayout, const double sampleRate, const double frameDuration);

/** */
PlankResult pl_AudioFileWriter_Open (PlankAudioFileWriterRef p, const char* filepath);

PlankResult pl_AudioFileWriter_OpenWithFile (PlankAudioFileWriterRef p, PlankFileRef file);

/** */
PlankResult pl_AudioFileWriter_Close (PlankAudioFileWriterRef p);

PlankResult pl_AudioFileWriter_WriteHeader (PlankAudioFileWriterRef p);

PlankResult pl_AudioFileWriter_WriteFrames (PlankAudioFileWriterRef p, const PlankB convertByteOrder, const int numFrames, const void* data);

PlankB pl_AudioFileWriter_IsEncodingNativeEndian (PlankAudioFileWriterRef p);

PlankResult pl_AudioFileWriter_SetHeaderPad (PlankAudioFileWriterRef p, const PlankUI headerPad);

PlankResult pl_AudioFileWriter_SetChannelItentifier (PlankAudioFileWriterRef p, const int channel, const PlankChannelIdentifier channelIdentifier);
PlankResult pl_AudioFileWriter_GetChannelItentifier (PlankAudioFileWriterRef p, const int channel, PlankChannelIdentifier* identifier);
PlankResult pl_AudioFileWriter_SetChanneLayout (PlankAudioFileWriterRef p, const PlankChannelLayout layout);
PlankResult pl_AudioFileWriter_GetChannelLayout (PlankAudioFileWriterRef p, PlankChannelLayout* layout);
PlankResult pl_AudioFileWriter_GetNumChannels (PlankAudioFileWriterRef p, int* numChannels);


/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankAudioFileWriter
{    
    PlankP peer;
    PlankAudioFileFormatInfo formatInfo;
    
    PlankLL numFrames;
    PlankLL dataPosition;
    PlankLL metaDataChunkPosition;  // this is the position of the chunk header of the first optional metadata chunk after
    PlankUI headerPad;              // this is the number of bytes exlcuding the chunk header to pad the header with
    PlankUC dataOffset; // into data chunk
    PlankUC reserved1;
    PlankUC reserved2;
    PlankUC reserved3;
    
    PlankAudioFileMetaDataRef metaData;
    
    PlankP writeFramesFunction;
    PlankP writeHeaderFunction;

} PlankAudioFileWriter;
#endif

#endif // PLANK_AUDIOFILEWRITER_H
