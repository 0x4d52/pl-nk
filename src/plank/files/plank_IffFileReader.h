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

#ifndef PLANK_IFFFILEREADER_H
#define PLANK_IFFFILEREADER_H

#include "plank_IffFileCommon.h"
#include "plank_File.h"

PLANK_BEGIN_C_LINKAGE

/** A generic IFF/RIFF file reader helper.
  
 @defgroup PlankIffFileReaderClass Plank IffFileReader class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank IffFileReader</i> object. */
typedef struct PlankIffFileReader* PlankIffFileReaderRef; 

/** Create and initialise a <i>Plank IffFileReader</i> object and return an oqaque reference to it.
 @return A <i>Plank IffFileReader</i> object as an opaque reference or PLANK_NULL. */
PlankIffFileReaderRef pl_IffFileReader_CreateAndInit();

/** Create a <i>Plank IffFileReader</i> object and return an oqaque reference to it.
 @return A <i>Plank IffFileReader</i> object as an opaque reference or PLANK_NULL. */
PlankIffFileReaderRef pl_IffFileReader_Create();

/** Initialise a <i>Plank IffFileReader</i> object. 
 @param p The <i>Plank IffFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_Init (PlankIffFileReaderRef p);

/** Deinitialise a <i>Plank IffFileReader</i> object. 
 @param p The <i>Plank IffFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_DeInit (PlankIffFileReaderRef p);

/** Destroy a <i>Plank IffFileReader</i> object. 
 @param p The <i>Plank IffFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_Destroy (PlankIffFileReaderRef p);

/** Gets the underlying <i>Plank %File</i> object. 
 This is the raw file object the is performing the fundamental file access operations.
 @param p The <i>Plank IffFileReader</i> object.
 @return The <i>Plank %File</i> object. */
PlankFileRef pl_IffFileReader_GetFile (PlankIffFileReaderRef p);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_Open (PlankIffFileReaderRef p, const char* filepath);

/** Open from a file object.
 Takes ownership and zeros the input file. The file must be little endian to start with. */
PlankResult pl_IffFileReader_OpenWithFile (PlankIffFileReaderRef p, PlankFileRef file);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_Close (PlankIffFileReaderRef p);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_GetMainID (PlankIffFileReaderRef p, PlankFourCharCode* mainID);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_GetFormatID (PlankIffFileReaderRef p, PlankFourCharCode* formatID);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_GetMainLength (PlankIffFileReaderRef p, PlankLL* mainLength);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_GetMainEnd (PlankIffFileReaderRef p, PlankLL* mainEnd);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_IsBigEndian (PlankIffFileReaderRef p, PlankB* isBigEndian);

/** 
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_SetEndian (PlankIffFileReaderRef p, const PlankB isBigEndian);

/** 
 @param p The <i>Plank IffFileReader</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_SeekChunk (PlankIffFileReaderRef p, const PlankFourCharCode chunkID, PlankUI* chunkLength, PlankLL* chunkDataPos);

/**
 @param p The <i>Plank IffFileReader</i> object.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_IffFileReader_ParseChunkHeader (PlankIffFileReaderRef p, PlankFourCharCode* chunkID, PlankUI* chunkLength, PlankLL* chunkEnd, PlankLL* posOut);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankIffFileReader
{
    PlankFile file;
    PlankIffFileHeaderInfo headerInfo;
    
} PlankIffFileReader;
#endif

#endif // PLANK_IFFFILEREADER_H
