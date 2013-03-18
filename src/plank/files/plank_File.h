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

#ifndef PLANK_TEXT_H
#define PLANK_TEXT_H

#include "../containers/plank_DynamicArray.h"
#include "plank_Path.h"

PLANK_BEGIN_C_LINKAGE

/** A crossplatform file reading/writing utility.
 
 Most of the "member functions" return a PlankResult which is a status and/or
 error code.
 
 @defgroup PlankFileClass Plank File class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %File</i> object. */
typedef struct PlankFile* PlankFileRef; 

/** Flag to identify 'read' mode. */
#define PLANKFILE_READ                  (1 << 0)

/** Flag to identify 'write' mode. */
#define PLANKFILE_WRITE                 (1 << 1)

/** Flag to identify 'binary' mode. */
#define PLANKFILE_BINARY                (1 << 2)

/** Flag to identify 'append' mode. */
#define PLANKFILE_APPEND                (1 << 3)

/** Flag to identify 'new' mode. */
#define PLANKFILE_NEW                   (1 << 4)

/** Flag to identify big endian mode for binary files (otherwise it will be little endian). */
#define PLANKFILE_BIGENDIAN             (1 << 5)

#if PLANK_BIGENDIAN
    #define PLANKFILE_NATIVEENDIAN      PLANKFILE_BIGENDIAN
#elif PLANK_LITTLEENDIAN
    #define PLANKFILE_NATIVEENDIAN      (0)
#endif

/** Flag to identify when in DynamciArray mode that the array is owned by this object. */
#define PLANKFILE_DYNAMICARRAYOWNED     (1 << 6)

/** Mode mask with all the mode flags except PLANKFILE_BIGENDIAN. */
#define PLANKFILE_MASK                  (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_BINARY | PLANKFILE_APPEND | PLANKFILE_NEW)

/** Mode mask with all the mode flags. */
#define PLANKFILE_ALL                   (PLANKFILE_MASK | PLANKFILE_BIGENDIAN)

#define PLANKFILE_STATUS_EOF                1
#define PLANKFILE_STATUS_ISPOSITIONABLE     2

#define PLANKFILE_STREAMTYPE_UNKNOWN        0
#define PLANKFILE_STREAMTYPE_FILE           1
#define PLANKFILE_STREAMTYPE_MEMORY         2
#define PLANKFILE_STREAMTYPE_DYNAMICARRAY   3
#define PLANKFILE_STREAMTYPE_NETWORK        4
#define PLANKFILE_STREAMTYPE_OTHER          5

#define PLANKFILE_SETPOSITION_ABSOLUTE       SEEK_SET
#define PLANKFILE_SETPOSITION_RELATIVE       SEEK_CUR
#define PLANKFILE_SETPOSITION_RELATIVEEND    SEEK_END

/** Delete a file with the given path form the filesystem. */
PlankResult pl_FileErase (const char* filepath);

PlankB pl_FileExists (const char* filepath, const PlankB isDirectory);
PlankResult pl_FileMakeDirectory (const char* filepath);



typedef PlankResult (*PlankFileOpenFunction)(PlankFileRef);
typedef PlankResult (*PlankFileCloseFunction)(PlankFileRef);
typedef PlankResult (*PlankFileClearFunction)(PlankFileRef);
typedef PlankResult (*PlankFileGetStatusFunction)(PlankFileRef, int, int*);
typedef PlankResult (*PlankFileReadFunction)(PlankFileRef, PlankP ptr, int, int*);
typedef PlankResult (*PlankFileWriteFunction)(PlankFileRef, const void*, const int);
typedef PlankResult (*PlankFileSetPositionFunction)(PlankFileRef, PlankLL, int);
typedef PlankResult (*PlankFileGetPositionFunction)(PlankFileRef, PlankLL*);

/** Create and initialise a <i>Plank %File</i> object and return an oqaque reference to it.
 @return A <i>Plank %File</i> object as an opaque reference or PLANK_NULL. */
PlankFileRef pl_File_CreateAndInit();

/** Create a <i>Plank %File</i> object and return an oqaque reference to it.
 @return A <i>Plank %File</i> object as an opaque reference or PLANK_NULL. */
PlankFileRef pl_File_Create();

/** Initialise a <i>Plank %File</i> object. 
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_Init (PlankFileRef p);

/** Deinitialise a <i>Plank %File</i> object. 
 This also closes the file if it is still open.
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_DeInit (PlankFileRef p);

/** Destroy a <i>Plank %File</i> object. 
 This also closes the file if it is still open.
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_Destroy (PlankFileRef p);

PlankResult pl_File_SetFunction (PlankFileRef p, 
                                 PlankFileOpenFunction openFunction, 
                                 PlankFileCloseFunction closeFunction,
                                 PlankFileClearFunction clearFunction,
                                 PlankFileGetStatusFunction statusFunction,
                                 PlankFileReadFunction readFunction,
                                 PlankFileWriteFunction writeFunction,
                                 PlankFileSetPositionFunction setPosFunction,
                                 PlankFileGetPositionFunction getPosFunction);

/** Open a file.
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param mode A bit mask code to identify the mode in which to open the file. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_Open (PlankFileRef p, const char* filepath, const int mode);

/** Open a text file for reading and optionally writing.
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param andWrite If @c true the file will also be opened for writing.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OpenTextRead (PlankFileRef p, const char* filepath, const PlankB andWrite);

/** Open a text file for writing and optionally reading.
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param andRead If @c true the file will also be opened for reading.
 @param replaceExistingFile If @c true any exising file will be erased otherwise 
                            data will be appended to the existing file.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OpenTextWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile);

/** Open a binary file for reading and optionally writing.
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param andWrite If @c true the file will also be opened for writing.
 @param isBigEndian Set to @c true to write multibyte values in big endian 
                    format, otherwise write in little endian format.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OpenBinaryRead (PlankFileRef p, const char* filepath, const PlankB andWrite, const PlankB isBigEndian);

/** Open a binary file for writing and optionally reading.
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param andRead If @c true the file will also be opened for reading.
 @param replaceExistingFile If @c true any exising file will be erased otherwise 
                            data will be appended to the existing file.
 @param isBigEndian Set to @c true to write multibyte values in big endian 
                    format, otherwise write in little endian format.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OpenBinaryWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile, const PlankB isBigEndian);

/** Open a binary file for reading and optionally writing in the native endian format.
 This will open a file for reading/writing in little endian format on little endian 
 machines (x86, x86_64, ARM etc) and big endian format on big endian machines (e.g., PPC).
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param andWrite If @c true the file will also be opened for writing.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OpenBinaryNativeEndianRead (PlankFileRef p, const char* filepath, const PlankB andWrite);

/** Open a binary file for writing and optionally reading in the native endian format.
 This will open a file for reading/writing in little endian format on little endian 
 machines (x86, x86_64, ARM etc) and big endian format on big endian machines (e.g., PPC).
 @param p The <i>Plank %File</i> object. 
 @param filepath The filepath of the file to open.
 @param andRead If @c true the file will also be opened for reading.
 @param replaceExistingFile If @c true any exising file will be erased otherwise 
                            data will be appended to the existing file.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OpenBinaryNativeEndianWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile);

PlankResult pl_File_OpenMemory (PlankFileRef p, void* memory, const PlankLL size, const int mode);

PlankResult pl_File_OpenDynamicArray (PlankFileRef p, PlankDynamicArrayRef memory, const int mode);

PlankResult pl_File_Copy (PlankFileRef p, PlankFileRef source, const PlankLL size);

PlankResult pl_File_Clear (PlankFileRef p);

/** Determine if a file object is in big endian format.
 @param p The <i>Plank %File</i> object. 
 @return @c true if the file object refers to a file opened in big endian format,
         othewise returns @c false (i.e., it is litte endian). */
PlankB pl_File_IsBigEndian (PlankFileRef p);

/** Determine if a file object is in little endian format.
 @param p The <i>Plank %File</i> object. 
 @return @c true if the file object refers to a file opened in little endian format,
         othewise returns @c false (i.e., it is big endian). */
PlankB pl_File_IsLittleEndian (PlankFileRef p);

/** Determine if a file object is in the native endian format.
 @param p The <i>Plank %File</i> object. 
 @return @c true if the file object refers to a file opened in endian format of 
         the machine on which this is running, othewise returns @c false. */
PlankB pl_File_IsNativeEndian (PlankFileRef p);

PlankB pl_File_IsPositionable (PlankFileRef p);

/** Close a file.
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_Close (PlankFileRef p);

/** Set the current position of the file read/write pointer.
 @param p The <i>Plank %File</i> object. 
 @param position The position within the file in bytes.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_SetPosition (PlankFileRef p, const PlankLL position);

/** Offset the current position of the file read/write pointer.
 @param p The <i>Plank %File</i> object. 
 @param offset The offset by which to move the file read/write pointer.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_OffsetPosition (PlankFileRef p, const PlankLL offset);

/** Reset the current position of the file read/write pointer to the start of the file.
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ResetPosition (PlankFileRef p);

/** Set the current position of the file read/write pointer to the end of the file.
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_SetPositionEnd (PlankFileRef p);

/** Determine if a file object is valid.
 This currently checks only if the file object is null.
 @param p The <i>Plank %File</i> object. 
 @return @c true if the file object is not null. */
PlankB pl_File_IsValid (PlankFileRef p);

/** Determine if a file object read/write pointer has reached the end of the file.
 @param p The <i>Plank %File</i> object. 
 @return @c true if the file read/write pointer has reached the end of the file,
         otherwise @c false. */
PlankB pl_File_IsEOF (PlankFileRef p);

/** Sets the file object read/write pointer to the end of the file.
 @param p The <i>Plank %File</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_SetEOF (PlankFileRef p);

/** Get the current position of the file read/write pointer.
 @param p The <i>Plank %File</i> object. 
 @param position A pointer to a variable that will receive the result.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_GetPosition (PlankFileRef p, PlankLL* position);

/** Reopen the file with a different mode (e.g, read/write/binary etc).
 @param p The <i>Plank %File</i> object. 
 @param mode A bit mask code to identify the mode in which to open the file. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_SetMode (PlankFileRef p, const int mode);
PlankResult pl_File_GetMode (PlankFileRef p, int* mode);

/** */
PlankResult pl_File_SetEndian (PlankFileRef p, const PlankB isBigEndian);

/** Read an array of bytes from the file.
 NB as this reads raw bytes it takes no account of the likely endian format
 of any data read.
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @param maximumBytes The number of bytes to read.
 @param bytesRead On return contains the number of bytes read (pass PLANK_NULL to ignore this).
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_Read (PlankFileRef p, PlankP data, const int maximumBytes, int* bytesRead);

/** Read one signed byte from the file.
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadC (PlankFileRef p, char* data);

/** Read a short from the file (16-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadS (PlankFileRef p, short* data);

/** Read an int from the file (32-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadI (PlankFileRef p, int* data);

/** Read a 24-bit int from the file (i.e. 3 bytes).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadI24 (PlankFileRef p, PlankI24* data);


/** Read a long from the file.
 This will be a 32-bit int on 32-bit systems (i.e., 4 bytes) and a 64-bit int on
 64-bit systems (i.e., 8 bytes) even on Windows where a 'long' is normally 32 bits
 on both architectures.
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_File_ReadL (PlankFileRef p, PlankL* data);

/** Read a 64-bit int from the file (i.e. 8 bytes).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadLL (PlankFileRef p, PlankLL* data);

/** Read one unsigned byte from the file.
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadUC (PlankFileRef p, unsigned char* data);

/** Read an unsigned short from the file (16-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadUS (PlankFileRef p, unsigned short* data);

/** Read an unsigned int from the file (32-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadUI (PlankFileRef p, unsigned int* data);

/** Read an unsigned 24-bit int from the file (i.e. 3 bytes).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadUI24 (PlankFileRef p, PlankUI24* data);

/** Read a long from the file.
 This will be a 32-bit int on 32-bit systems (i.e., 4 bytes) and a 64-bit int on
 64-bit systems (i.e., 8 bytes) even on Windows where a 'long' is normally 32 bits
 on both architectures.
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadUL (PlankFileRef p, PlankUL* data);

/** Read an unsigned 64-bit int from the file (i.e. 8 bytes).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadULL (PlankFileRef p, PlankULL* data);

/** Read a float from the file (32-bit float).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadF (PlankFileRef p, float* data);

/** Read a double from the file (64-bit float).
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that will receive the data.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadD (PlankFileRef p, double* data);

/** */
PlankResult pl_File_ReadFourCharCode (PlankFileRef p, PlankFourCharCode* data);

/** */
PlankResult pl_File_ReadPascalString255 (PlankFileRef p, PlankPascalString255* string);


/** Read an array of bytes as text upto the next line ending (or the end of the file if sooner).
 @param p The <i>Plank %File</i> object. 
 @param text A pointer to the memory location that will receive the data.
 @param maximumLength The maximum number of bytes to read.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_ReadLine (PlankFileRef p, char* text, const int maximumLength);

/** Skip over a number of bytes in the file.
 @param p The <i>Plank %File</i> object.
 @param numBytes The number of bytes to skip.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_SkipBytes (PlankFileRef p, const int numBytes);


/** Write an array of bytes to the file.
 NB as this reads raw bytes it takes no account of the likely endian format
 of any data read.
 @param p The <i>Plank %File</i> object. 
 @param data A pointer to the memory location that is the source of the data.
 @param maximumBytes The number of bytes to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_Write (PlankFileRef p, const void* data, const int maximumBytes);

PlankResult pl_File_WriteDynamicArray (PlankFileRef p, PlankDynamicArrayRef array);

PlankResult pl_File_WriteZeros (PlankFileRef p, const int numBytes);

/** Write one signed byte to the file.
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteC (PlankFileRef p, char data);

/** Write a short to the file (16-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteS (PlankFileRef p, short data);

/** Write an int to the file (32-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteI (PlankFileRef p, int data);

/** Write a 24-bit int to the file (i.e., 3 bytes).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteI24 (PlankFileRef p, PlankI24 data);

/** Write a long to the file.
 This will be a 32-bit int on 32-bit systems (i.e., 4 bytes) and a 64-bit int on
 64-bit systems (i.e., 8 bytes) even on Windows where a 'long' is normally 32 bits
 on both architectures.
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
//PlankResult pl_File_WriteL (PlankFileRef p, PlankL data);

/** Write a 64-bit int to the file.
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteLL (PlankFileRef p, PlankLL data);

/** Write one unsigned byte to the file.
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteUC (PlankFileRef p, unsigned char data);

/** Write an unsigned short to the file (16-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteUS (PlankFileRef p, unsigned short data);

/** Write an unsigned int to the file (32-bit int).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteUI (PlankFileRef p, unsigned int data);

/** Write an unsigned 24-bit int to the file (i.e., 3 bytes).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteUI24 (PlankFileRef p, PlankUI24 data);

/** Write an unisigned long to the file.
 This will be a 32-bit int on 32-bit systems (i.e., 4 bytes) and a 64-bit int on
 64-bit systems (i.e., 8 bytes) even on Windows where a 'long' is normally 32 bits
 on both architectures.
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteUL (PlankFileRef p, PlankUL data);

/** Write an unsigned 64-bit int to the file.
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteULL (PlankFileRef p, PlankULL data);

/** Write a float to the file (32-bit float).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteF (PlankFileRef p, float data);

/** Write a double to the file (64-bit float).
 @param p The <i>Plank %File</i> object. 
 @param data The value to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteD (PlankFileRef p, double data);

/** */
PlankResult pl_File_WriteFourCharCode (PlankFileRef p, PlankFourCharCode data);

/** Write string to the file.
 This writes until it hits the first null character in the string.
 @param p The <i>Plank %File</i> object. 
 @param text The string to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteString (PlankFileRef p, const char* text);

/** Write line of text to the file.
 This writes until it hits the first null character in the string and adds
 carriage return and line feed characters.
 @param p The <i>Plank %File</i> object. 
 @param text The string to write.
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_File_WriteLine (PlankFileRef p, const char* text);

/** */
PlankResult pl_File_WritePascalString255 (PlankFileRef p, PlankPascalString255* string);

PlankResult pl_FileDefaultOpenCallback (PlankFileRef p);
PlankResult pl_FileDefaultCloseCallback (PlankFileRef p);
PlankResult pl_FileDefaultGetStatusCallback (PlankFileRef p, int type, int* status);
PlankResult pl_FileDefaultReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesRead);
PlankResult pl_FileDefaultWriteCallback (PlankFileRef p, const void* data, const int maximumBytes);
PlankResult pl_FileDefaultSetPositionCallback (PlankFileRef p, PlankLL offset, int code);
PlankResult pl_FileDefaultGetPositionCallback (PlankFileRef p, PlankLL* position);


/// @} // End group PlankFileClass

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankFile
{
    void *stream; // must be first?
    PlankUS mode;
    PlankUS type;
    PlankLL size;
    PlankLL position;
    char path[PLANKPATH_MAXLENGTH];
        
    PlankFileOpenFunction           openFunction;
    PlankFileCloseFunction          closeFunction;
    PlankFileClearFunction          clearFunction;
    PlankFileGetStatusFunction      statusFunction;
    PlankFileReadFunction           readFunction;
    PlankFileWriteFunction          writeFunction;
    PlankFileSetPositionFunction    setPositionFunction;
    PlankFileGetPositionFunction    getPositionFunction;
} PlankFile;
#endif

#endif // PLANK_TEXT_H
