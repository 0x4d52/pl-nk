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

#ifndef PLANK_IFFFILEWRITER_H
#define PLANK_IFFFILEWRITER_H

#include "plank_File.h"
#include "../containers/plank_DynamicArray.h"
#include "../containers/plank_LockFreeQueue.h"


PLANK_BEGIN_C_LINKAGE

/** Holds references to multiple files.
  
 @defgroup PlankMultiFileClass Plank MultiFile class
 @ingroup PlankClasses
 @{
 */

#define PLANKMULITFILE_MODE_UNKNOWN                 0
#define PLANKMULITFILE_MODE_ARRAYSEQUENCEONCE       1       // DynamicArray of File objects
#define PLANKMULITFILE_MODE_ARRAYSEQUENCELOOP       2       // DynamicArray of File objects
#define PLANKMULITFILE_MODE_ARRAYRANDOM             3       // DynamicArray of File objects randomly chosen
#define PLANKMULITFILE_MODE_ARRAYRANDOMNOREPEAT     4       // DynamicArray of File objects randomly chosen, not repeating
//#define PLANKMULITFILE_MODE_ARRAYCALLBACK           5       // DynamicArray of File objects chosen using a callback
#define PLANKMULITFILE_MODE_ARRAYINDEXREF           5       // DynamicArray of File objects chosen using a reference to an index
#define PLANKMULITFILE_MODE_QUEUE                   6       // LockFreeQueue of File objects
#define PLANKMULITFILE_MODE_CUSTOM                  7       // Custom



typedef PlankResult (*PlankMultiFileNextFunction)(PlankMulitFileReaderRef);
typedef PlankResult (*PlankMultiFileNextIndexFunction)(PlankMulitFileReaderRef);
typedef PlankResult (*PlankMultiFileDestroyCustomFunction)(PlankMulitFileReaderRef);


PlankMulitFileReaderRef pl_MultiFileReader_Create();

PlankResult pl_MultiFileReader_InitArraySequence (PlankMulitFileReaderRef p, const int fileMode,  PlankDynamicArrayRef array, const PlankB ownArray,  const PlankB loop);
PlankResult pl_MultiFileReader_InitArrayRandom (PlankMulitFileReaderRef p, const int fileMode, PlankDynamicArrayRef array, const PlankB ownArray, const PlankB noRepeat);
PlankResult pl_MultiFileReader_InitArrayIndexRef (PlankMulitFileReaderRef p, const int fileMode, PlankDynamicArrayRef array, const PlankB ownArray, int* indexRef);
PlankResult pl_MultiFileReader_InitQueue (PlankMulitFileReaderRef p, const int fileMode, PlankLockFreeQueueRef queue, const PlankB ownQueue);
PlankResult pl_MultiFileReader_InitCustom (PlankMulitFileReaderRef p, const int fileMode, PlankP custom, const PlankB ownCustom, PlankMultiFileNextFunction nextFuntion, PlankMultiFileDestroyCustomFunction destroyCustomFunction);

PlankResult pl_MultiFileReader_DeInit (PlankMulitFileReaderRef p);
PlankResult pl_MultiFileReader_Destroy (PlankMulitFileReaderRef p);

PlankResult pl_MultiFileReader_GetMultiMode (PlankMulitFileReaderRef p, int* multiMode);
PlankResult pl_MultiFileReader_GetArray (PlankMulitFileReaderRef p, PlankDynamicArrayRef* array);
PlankResult pl_MultiFileReader_GetQueue (PlankMulitFileReaderRef p, PlankLockFreeQueueRef* queue);
PlankResult pl_MultiFileReader_GetCurrentFile (PlankMulitFileReaderRef p, PlankFileRef* currentFile);
PlankResult pl_MultiFileReader_GetIndex (PlankMulitFileReaderRef p, int* index);
PlankResult pl_MultiFileReader_SetIndex (PlankMulitFileReaderRef p, const int index);
PlankResult pl_MultiFileReader_GetNumFiles (PlankMulitFileReaderRef p, int* count);


PlankResult pl_MultiFileReader_Open (PlankMulitFileReaderRef p);
PlankResult pl_MultiFileReader_Read (PlankMulitFileReaderRef p, PlankP ptr, int maximumBytes, int* bytesRead);

PlankResult pl_MultiFileReader_Queue_FreeCurrentFile (PlankMulitFileReaderRef p);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankMulitFileReader
{
    void* source;
    PlankFileRef currentFile;
    PlankMultiFileNextFunction nextFunction;
    PlankMultiFileNextIndexFunction nextIndexFunction;
    PlankP otherRef;

    int multiMode;
    PlankB ownSource;
    int index;
    int fileMode;
} PlankMulitFileReader;
#endif

//------------------------------------------------------------------------------



#endif // PLANK_IFFFILEWRITER_H
