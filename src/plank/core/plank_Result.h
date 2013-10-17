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

#ifndef PLANK_RESULT_H
#define PLANK_RESULT_H

typedef PlankI PlankResult;

enum PlankResultIdentifiers
{
    PlankResult_OK = 0,             ///< No error occurred during the operation.
    PlankResult_UnknownError,       ///< An error of an unknown type occurred.
    PlankResult_ResultInvalid,      ///< The error code is itself invalid !
    
    PlankResult_MemoryError,        ///< A memory error occured e.g., out of memory.
    PlankResult_NullPointerError,   ///< A null pointer was passed to a function where this is invalid.
    PlankResult_ArrayParameterError, ///< There was an error in a parameter to an array function.
    
    PlankResult_FileModeInvalid,            ///< The requested file mode is invalid.
    PlankResult_FileOpenFailed,             ///< A request to open a file failed for some reason.
    PlankResult_FileCloseFailed,            ///< A request to close a file failed for some reason
    PlankResult_FileAlreadyOpen,            ///< A request to open a file failed because a file is already open.
    PlankResult_FileInvalid,                ///< A file handle is invalid, probably null.
    PlankResult_FilePathInvalid,            ///< The file path passed to the function is invalid.
    PlankResult_FileReadError,              ///< A file read operation failed with an error.
    PlankResult_FileWriteError,             ///< A file write operation failed with an error.
    PlankResult_FileEOF,                    ///< A file position pointer hit the end of the file.
    PlankResult_FileSeekFailed,             ///< The file failed to seek.
    PlankResult_FileRemoveFailed,           ///< A request to remove a file from the filesystem failed.
    PlankResult_FileMakeDirectoryFailed,    ///< Failed to make a directory.
    
    PlankResult_IffFileReaderChunkNotFound,                   ///< The specified chunk ID was not found.
    
    PlankResult_AudioFileInavlidType,                   ///< Invalid data while accessing an audio file.
    PlankResult_AudioFileUnsupportedType,               ///< A potentially valid audio file but not yet supported.
    PlankResult_AudioFileNotReady,                      ///< The file is not ready for this operation.
    PlankResult_AudioFileDataChunkInvalid,              ///< The data chunks appears to be invalid.
    PlankResult_AudioFileInvalidFilePosition,           ///< A file operation was attempted when the file was at an invalid position.
    PlankResult_AudioFileFrameFormatChanged,            ///< The audio file frame format changed mid-stream.
    
    PlankResult_ThreadSetFunctionFailed,    ///< Setting the run function failed, probably because the thread is already running.
    PlankResult_ThreadSetUserDataFailed,    ///< Setting the user data failed, probably because the thread is already running.
    PlankResult_ThreadAlreadyRunning,       ///< An attempt to run a Thread that is already running was made.
    PlankResult_ThreadStartFailed,          ///< Starting a Thread failed for some reason.
    PlankResult_ThreadInvalid,              ///< An operation on an invalid (possibly null) thread was attempted.
    PlankResult_ThreadCancelFailed,         ///< Cancelling a Thread failed.
    PlankResult_ThreadWaitFailed,           ///< Waiting a Thread failed
    PlankResult_ThreadFunctionInvalid,      ///< The Thread function is invalid.
    PlankResult_ThreadShouldExitAlreadySet, ///< Signaling a thread to exit that has already set set to exit.
    PlankResult_ThreadSetPriorityFailed,    ///< Setting the thread's priority failed.
    PlankResult_ThreadSetAffinityFailed,    ///< Setting the thread's affinity failed.

    PlankResult_FunctionsInvalid,           ///< One or more callback functions were null
    
    PlankResult_IndexOutOfRange,            ///< An index for a list, array etc was out of range.
    PlankResult_ItemCountInvalid,           ///< An item count was invalid (e.g., 0 or too small for the context).
    PlankResult_ContainerNotEmptyOnDeInit,  ///< A container (list, queue, stack) is being de-initialised but is non-empty.
    
    PlankResult_ThreadLocalStorageMaximumIdentifiersReached, ///< The maximum number of identifiers for thread-local storage has been reached.
    PlankResult_JSONError,                  ///< A generic JSON error occurred.
    PlankResult_JSONFileError,              ///< A JSON error occurred with a file.
    PlankResult_ZipError,                   ///< A generic Zip error occurred.

    PlankNumResults
};

PLANK_BEGIN_C_LINKAGE

const char* pl_ResultString (PlankResult result);

PLANK_END_C_LINKAGE



#endif // PLANK_RESULT_H
