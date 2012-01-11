/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
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
 -------------------------------------------------------------------------------
 */

#ifndef PLANK_AUDIOFILEREADER_H
#define PLANK_AUDIOFILEREADER_H

#include "plank_AudioFileCommon.h"

PLANK_BEGIN_C_LINKAGE

/** An audio file reader.
  
 @defgroup PlankAudioFileReaderClass Plank AudioFileReader class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank AudioFileReader</i> object. */
typedef struct PlankAudioFileReader* PlankAudioFileReaderRef; 

/** Create and initialise a <i>Plank AudioFileReader</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileReader</i> object as an opaque reference or PLANK_NULL. */
PlankAudioFileReaderRef pl_AudioFileReader_CreateAndInit();

/** Create a <i>Plank AudioFileReader</i> object and return an oqaque reference to it.
 @return A <i>Plank AudioFileReader</i> object as an opaque reference or PLANK_NULL. */
PlankAudioFileReaderRef pl_AudioFileReader_Create();

/** Initialise a <i>Plank AudioFileReader</i> object. 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_Init (PlankAudioFileReaderRef p);

/** Deinitialise a <i>Plank AudioFileReader</i> object. 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_DeInit (PlankAudioFileReaderRef p);

/** Destroy a <i>Plank AudioFileReader</i> object. 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_Destroy (PlankAudioFileReaderRef p);

/** Gets the underlying <i>Plank %File</i> object. 
 This is the raw file object the is performing the fundamental file access operations.
 @param p The <i>Plank AudioFileReader</i> object.
 @return The <i>Plank %File</i> object. */
PlankFileRef pl_AudioFileReader_GetFile (PlankAudioFileReaderRef p);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_Open (PlankAudioFileReaderRef p, const char* filepath);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_Close (PlankAudioFileReaderRef p);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetFormat (PlankAudioFileReaderRef p, int *format);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetEncoding (PlankAudioFileReaderRef p, int *encoding);

/** 
 The result of this will be invalid if the file is a compressed format.
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetBitsPerSample (PlankAudioFileReaderRef p, int *bitsPerSample);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetNumChannels (PlankAudioFileReaderRef p, int *numChannels);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetSampleRate (PlankAudioFileReaderRef p, double *sampleRate);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetNumFrames (PlankAudioFileReaderRef p, int *numFrames);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_SetFramePosition (PlankAudioFileReaderRef p, const int frameIndex);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_ResetFramePosition (PlankAudioFileReaderRef p);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_GetFramePosition (PlankAudioFileReaderRef p, int *frameIndex);

/** 
 @param p The <i>Plank AudioFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_AudioFileReader_ReadFrames (PlankAudioFileReaderRef p, const int numFrames, void* data);


/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankAudioFileReader
{
    PlankIffFileReader iff;    
    PlankAudioFileFormatInfo formatInfo;
    
    PlankLL dataLength;
    PlankLL numFrames;
    PlankLL dataPosition;
} PlankAudioFileReader;
#endif

#endif // PLANK_AUDIOFILEREADER_H
