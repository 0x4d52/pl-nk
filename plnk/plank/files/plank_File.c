/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#include <sys/stat.h>
#include "../core/plank_StandardHeader.h"
#include "plank_File.h"
#include "../maths/plank_Maths.h"
#include "plank_MultiFileReader.h"

// file callbacks

PlankResult pl_FileDefaultOpenCallback (PlankFileRef p)
{
    if (p->mode == PLANKFILE_READ)
        p->stream = fopen (p->path, "r");
    else if (p->mode == (PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "w");
    else if (p->mode == (PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "a");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE))
        p->stream = fopen (p->path, "r+");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "w+");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "a+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ))
        p->stream = fopen (p->path, "rb");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "wb");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "ab");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE))
        p->stream = fopen (p->path, "rb+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "wb+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "ab+");
    else
        return PlankResult_FileModeInvalid;
    
    if (! p->stream)
    {
        int error = errno;
        printf("error:%d\n", error);
    }
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultCloseCallback (PlankFileRef p)
{
    int err;
    
    err = fclose ((FILE*)p->stream);
    p->stream = 0;
    
    if (err != 0)
        return PlankResult_FileCloseFailed;
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultClearCallback (PlankFileRef p)
{
    int err;
    PlankResult result;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    err = fclose ((FILE*)p->stream);
    p->stream = 0;
    
    if (err != 0)
        return PlankResult_FileCloseFailed;
    
    if ((result = pl_FileErase (p->path)) != PlankResult_OK)
        return result;
    
    return pl_FileDefaultOpenCallback (p);
}

PlankResult pl_FileDefaultGetStatusCallback (PlankFileRef p, int type, int* status)
{
    switch (type)
    {
        case PLANKFILE_STATUS_EOF:              *status = feof ((FILE*)p->stream); break;
        case PLANKFILE_STATUS_ISPOSITIONABLE:   *status = PLANK_TRUE; break;
            
        default: return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesRead)
{
    PlankResult result;
    int size, err;
    
    result = PlankResult_OK;
    
    size = (int)fread (ptr, 1, (size_t)maximumBytes, (FILE*)p->stream);
    
    if (size != maximumBytes)
    {
        err = ferror ((FILE*)p->stream);
        
        if (err != 0)
        {
            result = PlankResult_FileReadError;
            goto exit;
        }
        
        err = feof ((FILE*)p->stream);
        
        if (err != 0)
        {
            result = PlankResult_FileEOF;
            goto exit;
        }
        
        result = PlankResult_UnknownError;
    }
    
exit:
    if (bytesRead != PLANK_NULL)
        *bytesRead = size;
    
    return result;
}

PlankResult pl_FileDefaultWriteCallback (PlankFileRef p, const void* data, const int maximumBytes)
{
    int size;
    
    size = (int)fwrite (data, 1, maximumBytes, p->stream);
    
    if (size != maximumBytes)
        return PlankResult_FileWriteError;
    
#if PLANK_DEBUG
    fflush (p->stream);
#endif
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultSetPositionCallback (PlankFileRef p, PlankLL offset, int code)
{
#if PLANK_WIN
    int err;
    
    clearerr ((FILE*)p->stream);
    err = _fseeki64 ((FILE*)p->stream, offset, code);
    
    if (err != 0)
        return PlankResult_FileSeekFailed;
    
    return PlankResult_OK;
#else
    int err;
    off_t temp;
    
    clearerr ((FILE*)p->stream);
    temp = offset;
    err = fseeko ((FILE*)p->stream, temp, code);
    
    if (err != 0)
        return PlankResult_FileSeekFailed;
    
    return PlankResult_OK;
#endif
}

//PlankResult pl_FileDefaultGetPositionCallback (PlankFileRef p, PlankLL* position)
//{
//    int err;
//    fpos_t temp;
//    err = fgetpos ((FILE*)p->stream, &temp);
//    
//    if (err != 0)
//        return PlankResult_FileReadError;
//    
//    p->position = temp;
//    
//    if (position != 0)
//        *position = temp;
//    
//    return PlankResult_OK;
//}

PlankResult pl_FileDefaultGetPositionCallback (PlankFileRef p, PlankLL* position)
{
#if PLANK_WIN
	__int64 result;
	result = _ftelli64 ((FILE*)p->stream);
#else
    off_t result;
    result = ftello ((FILE*)p->stream);
#endif

    if (result < 0)
        return PlankResult_FileReadError;
    
    p->position = (PlankLL)result;
    
    if (position != 0)
        *position = p->position;
    
    return PlankResult_OK;
}

// memory callbacks

static PlankResult pl_FileMemoryOpenCallback (PlankFileRef p)
{
    if (p->mode & PLANKFILE_APPEND)
        return PlankResult_FileModeInvalid;
    
//    p->position = 0;
    return PlankResult_OK;
}

static PlankResult pl_FileMemoryCloseCallback (PlankFileRef p)
{
    return pl_File_Init (p);
}

static PlankResult pl_FileMemoryClearCallback (PlankFileRef p)
{
    return pl_MemoryZero (p->stream, (PlankUL)p->size);
}

static PlankResult pl_FileMemoryGetStatusCallback (PlankFileRef p, int type, int* status)
{
    switch (type)
    {
        case PLANKFILE_STATUS_EOF:
            *status = (p->position < p->size) ? PLANK_FALSE : PLANK_TRUE;
            break;
        case PLANKFILE_STATUS_ISPOSITIONABLE:
            *status = PLANK_TRUE;
            break;
            
        default: return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

static PlankResult pl_FileMemoryReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesReadOut)
{
    PlankResult result;
    PlankUC* src;
    int bytesRead;
    
    result = PlankResult_OK;
    bytesRead = (int)pl_MinLL (maximumBytes, p->size - p->position);

    if (bytesRead <= 0)
    {
        result = PlankResult_FileEOF;
        goto exit;
    }
    
    src = (PlankUC*)p->stream + p->position;
    
    if ((result = pl_MemoryCopy (ptr, src, bytesRead)) != PlankResult_OK) goto exit;
    
    p->position += bytesRead;
        
    if (bytesReadOut)
        *bytesReadOut = bytesRead;
    
exit:
    return result;
}

static PlankResult pl_FileMemoryWriteCallback (PlankFileRef p, const void* data, const int maximumBytes)
{
    PlankResult result;
    PlankUC* dst;
    
    result = PlankResult_OK;
    dst = (PlankUC*)p->stream + p->position;
    
    if ((p->size - p->position) < (PlankLL)maximumBytes)
    {
        result = PlankResult_FileWriteError;
        goto exit;
    }
    
    if ((result = pl_MemoryCopy (dst, data, maximumBytes)) != PlankResult_OK) goto exit;
    
    p->position += maximumBytes;
    
exit:
    return result;
}

static PlankResult pl_FileMemorySetPositionCallback (PlankFileRef p, PlankLL offset, int code)
{
    PlankResult result;
    PlankLL newPosition;
    
    result = PlankResult_OK;
    
    switch (code)
    {
        case PLANKFILE_SETPOSITION_ABSOLUTE: newPosition = offset; break;
        case PLANKFILE_SETPOSITION_RELATIVE: newPosition = p->position + offset; break;
        case PLANKFILE_SETPOSITION_RELATIVEEND: newPosition = p->size + offset; break;
        default: newPosition = 0;
    }
    
    if (newPosition < 0)
    {
        newPosition = 0;
        result = PlankResult_FileSeekFailed;
    }
    
    if (newPosition >= p->size)
        newPosition = p->size;
    
    p->position = newPosition;

//exit:
    return result;
}

static PlankResult pl_FileMemoryGetPositionCallback (PlankFileRef p, PlankLL* position)
{
    *position = p->position;
    return PlankResult_OK;
}

// dynamic array callbacks

static PlankResult pl_FileDynamicArrayOpenCallback (PlankFileRef p)
{
    PlankDynamicArrayRef array;
    PlankLL size;

    if ((p->mode & PLANKFILE_WRITE) && (p->mode & PLANKFILE_APPEND))
    {
        array = (PlankDynamicArrayRef)p->stream;
        size = (PlankLL)pl_DynamicArray_GetSize (array) * (PlankLL)pl_DynamicArray_GetItemSize (array);
        p->position = size;
    }
    else
    {
//        p->position = 0;
    }
    
    return PlankResult_OK;
}

static PlankResult pl_FileDynamicArrayCloseCallback (PlankFileRef p)
{
    PlankResult result;
    
    result = PlankResult_OK;
    
    if (p->mode & PLANKFILE_DYNAMICARRAYOWNED)
        result = pl_DynamicArray_Destroy ((PlankDynamicArrayRef)p->stream);
    
    if (result == PlankResult_OK)
        result = pl_File_Init (p);

//exit:
    return result;
}

static PlankResult pl_FileDynamicArrayClearCallback (PlankFileRef p)
{
    return pl_DynamicArray_SetSize ((PlankDynamicArrayRef)p->stream, 0);
}

static PlankResult pl_FileDynamicArrayGetStatusCallback (PlankFileRef p, int type, int* status)
{
    PlankDynamicArrayRef array;
    PlankLL size;
    
    array = (PlankDynamicArrayRef)p->stream;
    size = (PlankLL)pl_DynamicArray_GetSize (array) * (PlankLL)pl_DynamicArray_GetItemSize (array);
    
    switch (type)
    {
        case PLANKFILE_STATUS_EOF:
            *status = (p->position < size) ? PLANK_FALSE : PLANK_TRUE;
            break;
        case PLANKFILE_STATUS_ISPOSITIONABLE:
            *status = PLANK_TRUE;
            break;
            
        default: return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

static PlankResult pl_FileDynamicArrayReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesReadOut)
{
    PlankResult result;
    PlankUC* src;
    PlankDynamicArrayRef array;
    PlankLL size;
    int bytesRead;
    
    result      = PlankResult_OK;
    array       = (PlankDynamicArrayRef)p->stream;
    size        = (PlankLL)pl_DynamicArray_GetSize (array) * (PlankLL)pl_DynamicArray_GetItemSize (array);
    bytesRead   = (int)pl_MinLL (maximumBytes, size - p->position);

    if (bytesRead <= 0)
    {
        result = PlankResult_FileEOF;
        goto exit;
    }
    
    src = (PlankUC*)pl_DynamicArray_GetArray (array) + p->position;
    
    if ((result = pl_MemoryCopy (ptr, src, bytesRead)) != PlankResult_OK) goto exit;
    
    p->position += bytesRead;
        
    if (bytesReadOut)
        *bytesReadOut = bytesRead;
    
exit:
    return result;
}

static PlankResult pl_FileDynamicArrayWriteCallback (PlankFileRef p, const void* data, const int maximumBytes)
{
    PlankResult result;
    PlankUC* dst;
    PlankDynamicArrayRef array;
    PlankLL size, sizeNeeded;
    PlankL itemSize, capacity;
    
    result      = PlankResult_OK;
    array       = (PlankDynamicArrayRef)p->stream;
    itemSize    = pl_DynamicArray_GetItemSize (array);
    size        = (PlankLL)pl_DynamicArray_GetSize (array) * (PlankLL)itemSize;
    sizeNeeded  = p->position + maximumBytes;
    
    if (sizeNeeded > size)
    {
        capacity = (PlankL)((sizeNeeded % itemSize) ? sizeNeeded / itemSize + 1 : sizeNeeded / itemSize);
        if ((result = pl_DynamicArray_SetSize (array, capacity)) != PlankResult_OK) goto exit;
    }
    
    dst = (PlankUC*)pl_DynamicArray_GetArray (array) + p->position;
    
    if ((result = pl_MemoryCopy (dst, data, maximumBytes)) != PlankResult_OK) goto exit;
    
    p->position += maximumBytes;
    
exit:
    return result;
}

static PlankResult pl_FileDynamicArraySetPositionCallback (PlankFileRef p, PlankLL offset, int code)
{
    PlankResult result;
    PlankLL newPosition;
    PlankDynamicArrayRef array;
    PlankLL size;

    result = PlankResult_OK;
    array = (PlankDynamicArrayRef)p->stream;
    size = (PlankLL)pl_DynamicArray_GetSize (array) * (PlankLL)pl_DynamicArray_GetItemSize (array);
    
    switch (code)
    {
        case PLANKFILE_SETPOSITION_ABSOLUTE: newPosition = offset; break;
        case PLANKFILE_SETPOSITION_RELATIVE: newPosition = p->position + offset; break;
        case PLANKFILE_SETPOSITION_RELATIVEEND: newPosition = size + offset; break;
        default: newPosition = 0;
    }
    
    if (newPosition < 0)
    {
        newPosition = 0;
        result = PlankResult_FileSeekFailed;
    }
    
    if (newPosition >= size)
        newPosition = size;
    
    p->position = newPosition;
    
//exit:
    return result;
}

static PlankResult pl_FileDynamicArrayGetPositionCallback (PlankFileRef p, PlankLL* position)
{
    *position = p->position;
    return PlankResult_OK;
}

static PlankResult pl_FileMultiOpenCallback (PlankFileRef p)
{
    PlankMulitFileReaderRef multi;    
    multi  = (PlankMulitFileReaderRef)p->stream;
    return pl_MultiFileReader_Open (multi);
}

static PlankResult pl_FileMultiCloseCallback (PlankFileRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMulitFileReaderRef multi;
    int mode;
    
    pl_File_GetMode (p, &mode);
    
    if (mode & PLANKFILE_OWNMULTI)
    {
        multi  = (PlankMulitFileReaderRef)p->stream;
        result = pl_MultiFileReader_Destroy (multi);
        p->stream = PLANK_NULL;
    }
    
    return result;
}

static PlankResult pl_FileMultiClearCallback (PlankFileRef p)
{
    (void)p;
    return PlankResult_FileWriteError;
}

static PlankResult pl_FileMultiGetStatusCallback (PlankFileRef p, int type, int* status)
{
    PlankMulitFileReaderRef multi;
    
    multi = (PlankMulitFileReaderRef)p->stream;
    
    switch (type)
    {
        case PLANKFILE_STATUS_EOF:
            *status = (multi->currentFile == PLANK_NULL) ? PLANK_TRUE : PLANK_FALSE;
            break;
        case PLANKFILE_STATUS_ISPOSITIONABLE:
            *status = ((multi->multiMode == PLANKMULITFILE_MODE_ARRAYSEQUENCELOOP) || (multi->multiMode == PLANKMULITFILE_MODE_ARRAYSEQUENCEONCE)) ? PLANK_TRUE : PLANK_FALSE;
            break;
            
        default: return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

static PlankResult pl_FileMultiReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesRead)
{
    PlankResult result;
    PlankMulitFileReaderRef multi;
    
    result = PlankResult_OK;
    multi  = (PlankMulitFileReaderRef)p->stream;
    result = pl_MultiFileReader_Read (multi, ptr, maximumBytes, bytesRead);
    
//exit:
    return result;
}

static PlankResult pl_FileMultiWriteCallback (PlankFileRef p, const void* data, const int maximumBytes)
{
    (void)p;
    (void)data;
    (void)maximumBytes;
    return PlankResult_FileWriteError;
}

static PlankResult pl_FileMultiSetPositionCallback (PlankFileRef p, PlankLL offset, int code)
{
    PlankResult result;
    PlankMulitFileReaderRef multi;
    
    multi = (PlankMulitFileReaderRef)p->stream;
    
    if ((code == PLANKFILE_SETPOSITION_RELATIVE) || (code == PLANKFILE_SETPOSITION_RELATIVEEND))
    {
        result = PlankResult_FileSeekFailed;
        goto exit;
    }
    else if (((multi->multiMode == PLANKMULITFILE_MODE_ARRAYSEQUENCELOOP) || (multi->multiMode == PLANKMULITFILE_MODE_ARRAYSEQUENCEONCE)) && (offset == 0))
    {    
        result = PlankResult_OK;
        p->position = 0;
    }
    else
    {
        result = PlankResult_FileSeekFailed;
        goto exit;
    }
    
exit:
    return result;
}

static PlankResult pl_FileMultiGetPositionCallback (PlankFileRef p, PlankLL* position)
{
    (void)p;
    *position = 0;
    return PlankResult_FileSeekFailed;
}



// global

PlankResult pl_FileErase (const char* filepath)
{
    int err;
    
    err = remove (filepath);
    
    if (err != 0)
        return PlankResult_FileRemoveFailed;
    
    return PlankResult_OK;
}

PlankB pl_FileExists (const char* filepath, const PlankB isDirectory)
{
    struct stat st;
    pl_MemoryZero (&st, sizeof (st));
    stat (filepath, &st);
    return isDirectory ? (st.st_mode & S_IFDIR ? PLANK_TRUE : PLANK_FALSE) : (st.st_mode & S_IFREG ? PLANK_TRUE : PLANK_FALSE);
}

static int pl_mkdir (const char* filepath)
{
#if (defined (_WIN32) || defined (_WIN64) || defined (WIN64))
    return _mkdir (filepath);
#else
//# if _POSIX_C_SOURCE
//    return mkdir (filepath);
//# else
    return mkdir (filepath, 0755); // not sure if this works on mac
//# endif
#endif
}

PlankResult pl_FileMakeDirectory (const char* filepath)
{
    PlankResult result;
    PlankDynamicArray pathBuild;
    PlankFile pathBuildStream;
    PlankFile pathSourceStream;
    PlankL pathLength;
    PlankC chr;
    const char* currentLevel;
    
    result = PlankResult_OK;
    
    pl_File_Init (&pathBuildStream);
    pl_File_Init (&pathSourceStream);
    
    pathLength = strlen (filepath);

    if ((result = pl_DynamicArray_InitWithItemSizeAndCapacity (&pathBuild, 1, pathLength + 1)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_Zero (&pathBuild)) != PlankResult_OK) goto exit;
    
    if ((pathLength < 1) || (pathLength >= PLANKPATH_MAXLENGTH))
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
    if (filepath[pathLength - 1] != '/')
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
    if ((result = pl_File_OpenMemory (&pathSourceStream, (void*)filepath, pathLength, PLANKFILE_READ)) != PlankResult_OK) goto exit;
    if ((result = pl_File_OpenDynamicArray (&pathBuildStream, &pathBuild, PLANKFILE_WRITE)) != PlankResult_OK) goto exit;
    
    while ((result = pl_File_ReadC (&pathSourceStream, &chr)) == PlankResult_OK)
    {
        if ((result = pl_File_WriteC (&pathBuildStream, chr)) != PlankResult_OK) goto exit;
        
        if (chr == '/')
        {
            currentLevel = (const char*)pl_DynamicArray_GetArray (&pathBuild);
            
            if (! pl_FileExists (currentLevel, PLANK_TRUE))
            {
                if (pl_mkdir (currentLevel) != 0)
                {
                    result = PlankResult_FileMakeDirectoryFailed;
                    goto exit;
                }
            }
        }
    }
    
    if (result != PlankResult_FileEOF)
    {
        result = PlankResult_FileMakeDirectoryFailed;
        goto exit;
    }
    
exit:
    pl_File_DeInit (&pathSourceStream);
    pl_File_DeInit (&pathBuildStream);
    pl_DynamicArray_DeInit (&pathBuild);
    
    return result;
}

// class

PlankFileRef pl_File_CreateAndInit()
{
    PlankFileRef p;
    p = pl_File_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_File_Init (p) != PlankResult_OK)
            pl_File_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankFileRef pl_File_Create()
{
    PlankMemoryRef m;
    PlankFileRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankFileRef)pl_Memory_AllocateBytes (m, sizeof (PlankFile));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankFile));
    
    return p;
}

PlankResult pl_File_Init (PlankFileRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
        
    p->stream = 0;
    p->size = 0;
    p->mode = 0;
    p->type = PLANKFILE_STREAMTYPE_UNKNOWN;
    p->position = 0;
    p->path[0] = '\0';    
    
    // initialise to default functions
    pl_File_SetFunction (p, 0, 0, 0, 0, 0, 0, 0, 0);
    
    return PlankResult_OK;
}

PlankResult pl_File_DeInit (PlankFileRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p->stream != PLANK_NULL)
        result = pl_File_Close (p);    

    pl_MemoryZero (p, sizeof (PlankFile));

//exit:
    return result;
}

PlankResult pl_File_Destroy (PlankFileRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_File_DeInit (p)) != PlankResult_OK) 
        goto exit;
    
    result = pl_Memory_Free (m, p);
            
exit:
    return result;       
}

PlankResult pl_File_SetFunction (PlankFileRef p, 
                                 PlankFileOpenFunction openFunction, 
                                 PlankFileCloseFunction closeFunction,
                                 PlankFileClearFunction clearFunction,
                                 PlankFileGetStatusFunction statusFunction,
                                 PlankFileReadFunction readFunction,
                                 PlankFileWriteFunction writeFunction,
                                 PlankFileSetPositionFunction setPositionFunction,
                                 PlankFileGetPositionFunction getPositionFunction)
{
    if (openFunction != PLANK_NULL) p->openFunction = openFunction; 
    else p->openFunction = pl_FileDefaultOpenCallback;
    
    if (closeFunction != PLANK_NULL) p->closeFunction = closeFunction;
    else p->closeFunction = pl_FileDefaultCloseCallback;

    if (clearFunction != PLANK_NULL) p->clearFunction = clearFunction;
    else p->clearFunction = pl_FileDefaultClearCallback;
    
    if (statusFunction != PLANK_NULL) p->statusFunction = statusFunction; 
    else p->statusFunction = pl_FileDefaultGetStatusCallback;
    
    if (readFunction != PLANK_NULL) p->readFunction = readFunction; 
    else p->readFunction = pl_FileDefaultReadCallback;
    
    if (writeFunction != PLANK_NULL) p->writeFunction = writeFunction; 
    else p->writeFunction = pl_FileDefaultWriteCallback;
    
    if (setPositionFunction != PLANK_NULL) p->setPositionFunction = setPositionFunction; 
    else p->setPositionFunction = pl_FileDefaultSetPositionCallback;
    
    if (getPositionFunction != PLANK_NULL) p->getPositionFunction = getPositionFunction; 
    else p->getPositionFunction = pl_FileDefaultGetPositionCallback;
    
    return PlankResult_OK;
}

PlankResult pl_File_Open (PlankFileRef p, const char* filepath, const int mode)
{
    PlankResult result;
    
    if (p->stream != 0)
    {
        result = pl_File_Close (p);
        
        if (result != PlankResult_OK)
            return result;
    }
    
    if ((filepath == 0) || (filepath[0] == 0))
        return PlankResult_FilePathInvalid;
    
    strncpy (p->path, filepath, PLANKPATH_MAXLENGTH);
    
    p->mode = mode & PLANKFILE_MASK;    // without the big endian flag
    result = (p->openFunction) (p);
    p->mode = mode & PLANKFILE_ALL;     // including big endian flag
    
    p->type = PLANKFILE_STREAMTYPE_FILE;
    
    if (result != PlankResult_OK)
        return result;
    
    if (p->stream == 0)
        return PlankResult_FileOpenFailed;
    
    if (pl_File_IsPositionable (p))
    {
        result = pl_File_SetPosition (p, p->position);
        
        if (result != PlankResult_OK)
            return result;
    }
    
    return PlankResult_OK;
}

PlankResult pl_File_OpenTextRead (PlankFileRef p, const char* filepath, const PlankB andWrite)
{
    int mode = PLANKFILE_READ;
    
    if (andWrite)
        mode |= (PLANKFILE_WRITE | PLANKFILE_APPEND);
    
    return pl_File_Open (p, filepath, mode);
}

PlankResult pl_File_OpenTextWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile)
{
    int mode = PLANKFILE_WRITE;
    
    if (andRead)
        mode |= PLANKFILE_READ;
    
    if (replaceExistingFile)
        mode |= PLANKFILE_NEW;
    else
        mode |= PLANKFILE_APPEND;
    
    return pl_File_Open (p, filepath, mode);
}

PlankResult pl_File_OpenBinaryRead (PlankFileRef p, const char* filepath, const PlankB andWrite, const PlankB isBigEndian)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_READ;
    
    if (andWrite)
        mode |= (PLANKFILE_WRITE | PLANKFILE_APPEND);
    
    if (isBigEndian)
        mode |= PLANKFILE_BIGENDIAN;
    
    return pl_File_Open (p, filepath, mode);    
}

PlankResult pl_File_OpenBinaryWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile, const PlankB isBigEndian)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_WRITE;
    
    if (andRead)
        mode |= PLANKFILE_READ;
    
    if (replaceExistingFile)
        mode |= PLANKFILE_NEW;
    else
        mode |= PLANKFILE_APPEND;
    
    if (isBigEndian)
        mode |= PLANKFILE_BIGENDIAN;
    
    return pl_File_Open (p, filepath, mode);    
}

PlankResult pl_File_OpenBinaryNativeEndianRead (PlankFileRef p, const char* filepath, const PlankB andWrite)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_READ;
    
    if (andWrite)
        mode |= (PLANKFILE_WRITE | PLANKFILE_APPEND);
    
#if PLANK_BIGENDIAN
    mode |= PLANKFILE_BIGENDIAN;
#endif
    
    return pl_File_Open (p, filepath, mode);        
}

PlankResult pl_File_OpenBinaryNativeEndianWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_WRITE;
    
    if (andRead)
        mode |= PLANKFILE_READ;
    
    if (replaceExistingFile)
        mode |= PLANKFILE_NEW;
    else
        mode |= PLANKFILE_APPEND;
    
#if PLANK_BIGENDIAN
    mode |= PLANKFILE_BIGENDIAN;
#endif
    
    return pl_File_Open (p, filepath, mode);        
}

PlankResult pl_File_OpenMemory (PlankFileRef p, void* memory, const PlankLL size, const int mode)
{
    PlankResult result;
    
    if (p->stream != 0)
    {
        if ((result = pl_File_Close (p)) != PlankResult_OK)
            goto exit;
    }
    
    if ((memory == 0) || (size < 1))
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->path[0] = '\0';
    p->stream = memory;
    p->size = size;
    p->mode = mode;
    p->type = PLANKFILE_STREAMTYPE_MEMORY;

    result = pl_File_SetFunction (p,
                                  pl_FileMemoryOpenCallback,
                                  pl_FileMemoryCloseCallback,
                                  pl_FileMemoryClearCallback,
                                  pl_FileMemoryGetStatusCallback,
                                  pl_FileMemoryReadCallback,
                                  pl_FileMemoryWriteCallback,
                                  pl_FileMemorySetPositionCallback,
                                  pl_FileMemoryGetPositionCallback);

    if (result != PlankResult_OK) goto exit;
    
    result = (p->openFunction) (p);
    if (result != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_File_OpenDynamicArray (PlankFileRef p, PlankDynamicArrayRef memory, const int mode)
{
    PlankResult result = PlankResult_OK;
    
    if (p->stream != 0)
    {
        if ((result = pl_File_Close (p)) != PlankResult_OK)
            goto exit;
    }
            
    p->path[0] = '\0';
    p->stream = memory;
    p->size = 0;
    p->mode = mode;
    p->type = PLANKFILE_STREAMTYPE_DYNAMICARRAY;
    
    result = pl_File_SetFunction (p,
                                  pl_FileDynamicArrayOpenCallback,
                                  pl_FileDynamicArrayCloseCallback,
                                  pl_FileDynamicArrayClearCallback,
                                  pl_FileDynamicArrayGetStatusCallback,
                                  pl_FileDynamicArrayReadCallback,
                                  pl_FileDynamicArrayWriteCallback,
                                  pl_FileDynamicArraySetPositionCallback,
                                  pl_FileDynamicArrayGetPositionCallback);
    
    if (result != PlankResult_OK) goto exit;
    
    result = (p->openFunction) (p);
    if (result != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_File_OpenMulti (PlankFileRef p, PlankMulitFileReaderRef multi, const int mode)
{
    PlankResult result;
    
    if (mode & PLANKFILE_WRITE)
    {
        result = PlankResult_FileModeInvalid; // can't write to a multi
        goto exit;
    }
    
    if (p->stream != 0)
    {
        if ((result = pl_File_Close (p)) != PlankResult_OK)
            goto exit;
    }
    
    p->path[0] = '\0';
    p->stream = multi;
    p->size = 0;
    p->mode = mode;
    p->type = PLANKFILE_STREAMTYPE_MULTI;
    
    result = pl_File_SetFunction (p,
                                  pl_FileMultiOpenCallback,
                                  pl_FileMultiCloseCallback,
                                  pl_FileMultiClearCallback,
                                  pl_FileMultiGetStatusCallback,
                                  pl_FileMultiReadCallback,
                                  pl_FileMultiWriteCallback,
                                  pl_FileMultiSetPositionCallback,
                                  pl_FileMultiGetPositionCallback);

    if (result != PlankResult_OK) goto exit;
    
    result = (p->openFunction) (p);
    if (result != PlankResult_OK) goto exit;
    
exit:
    return result;    
}

#define PLANKFILE_COPYCHUNKSIZE 512

PlankResult pl_File_Copy (PlankFileRef p, PlankFileRef source, const PlankLL size)
{
    PlankResult result;
    PlankLL remaining;
    PlankUC buffer[PLANKFILE_COPYCHUNKSIZE];
    int bytesRead;
    
    result = PlankResult_OK;

    remaining = (size <= 0) ? (((PlankULL)1 << 63) - 1) : size;
    
    while (! pl_File_IsEOF (source))
    {
        result = pl_File_Read (source, buffer, PLANKFILE_COPYCHUNKSIZE, &bytesRead);
        
        if (((result != PlankResult_OK) && (result != PlankResult_FileEOF)) || (bytesRead == 0)) goto exit;
        if ((result = pl_File_Write (p, buffer, bytesRead)) != PlankResult_OK) goto exit;
        
        remaining -= bytesRead;
    }
    
exit:
    return result;
}

PlankResult pl_File_Clear (PlankFileRef p)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;

    return (p->clearFunction) (p);
}

PlankB pl_File_IsBigEndian (PlankFileRef p)
{
    return !! (p->mode & PLANKFILE_BIGENDIAN);
}

PlankB pl_File_IsLittleEndian (PlankFileRef p)
{
    return ! (p->mode & PLANKFILE_BIGENDIAN);
}

PlankB pl_File_IsNativeEndian (PlankFileRef p)
{
#if PLANK_BIGENDIAN
    return !! (p->mode & PLANKFILE_BIGENDIAN);
#elif PLANK_LITTLEENDIAN
    return  ! (p->mode & PLANKFILE_BIGENDIAN);
#else
    #error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}

PlankB pl_File_IsPositionable (PlankFileRef p)
{
    int isPositionable;
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    return isPositionable ? PLANK_TRUE : PLANK_FALSE;
}

PlankResult pl_File_Close (PlankFileRef p)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    return (p->closeFunction) (p);
}

PlankResult pl_File_SetPosition (PlankFileRef p, const PlankLL position)
{
    int isPositionable;
    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (! isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, position, SEEK_SET);
}

PlankResult pl_File_OffsetPosition (PlankFileRef p, const PlankLL offset)
{
    int isPositionable;

    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (! isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, offset, SEEK_CUR);
}

PlankResult pl_File_ResetPosition (PlankFileRef p)
{
    return pl_File_SetPosition (p, 0);
}

PlankResult pl_File_SetPositionEnd (PlankFileRef p)
{
    int isPositionable;
    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (! isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, 0, SEEK_END);
}

PlankB pl_File_IsValid (PlankFileRef p)
{
    return p ? p->stream != 0 : PLANK_FALSE;
}

PlankB pl_File_IsEOF (PlankFileRef p)
{
    int status;
    
    if (p->stream == 0)
        return PLANK_FALSE;

    status = PLANK_FALSE;
    (p->statusFunction) (p, PLANKFILE_STATUS_EOF, &status);
    
    return status != 0;
}

PlankResult pl_File_SetEOF (PlankFileRef p)
{
    int isPositionable;

    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (! isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, 0L, SEEK_END);  
}

PlankResult pl_File_GetPosition (PlankFileRef p, PlankLL* position)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    return (p->getPositionFunction) (p, position);
}

PlankResult pl_File_SetMode (PlankFileRef p, const int mode)
{
    PlankResult result;
    
    if (p->stream == 0)
        return PlankResult_FileInvalid;

    if (p->mode == mode)
        return PlankResult_OK;
    
    result = pl_File_Close (p);
    
    if (result != PlankResult_OK)
        return result;
    
    result = pl_File_Open (p, p->path, mode);
    
    if (result != PlankResult_OK)
        return result;

    return PlankResult_OK;
}

PlankResult pl_File_GetMode (PlankFileRef p, int* mode)
{
    *mode = p->mode;
    return PlankResult_OK;
}

PlankResult pl_File_GetStreamType (PlankFileRef p, int* type)
{
    *type = p->type;
    return PlankResult_OK;
}

PlankResult pl_File_SetEndian (PlankFileRef p, const PlankB isBigEndian)
{
    if (isBigEndian)
        p->mode |= PLANKFILE_BIGENDIAN;  
    else    
        p->mode &= ~PLANKFILE_BIGENDIAN;
        
    return PlankResult_OK;
}

PlankResult pl_File_Read (PlankFileRef p, PlankP data, const int maximumBytes, int* bytesRead)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;
        
    if (! (p->mode & PLANKFILE_READ))
        return PlankResult_FileReadError;
        
    return (p->readFunction) (p, data, maximumBytes, bytesRead);    
}

PlankResult pl_File_ReadC (PlankFileRef p, char* data)
{
    PlankResult result;
    result = pl_File_Read (p, data, sizeof (char), PLANK_NULL);
    return result;
}

PlankResult pl_File_ReadS (PlankFileRef p, short* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (short), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianS (data);
    
    return result;    
}

PlankResult pl_File_ReadI (PlankFileRef p, int* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (int), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI (data);
    
    return result;        
}

PlankResult pl_File_ReadI24 (PlankFileRef p, PlankI24* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankI24), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI24 (data);
    
    return result;            
}

/*
PlankResult pl_File_ReadL (PlankFileRef p, PlankL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankL), (int*)PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianL (data);
    
    return result;        
}
*/

PlankResult pl_File_ReadLL (PlankFileRef p, PlankLL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankLL), (int*)PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianLL (data);
    
    return result;            
}

PlankResult pl_File_ReadUC (PlankFileRef p, unsigned char* data)
{
    PlankResult result;
    result = pl_File_Read (p, data, sizeof (unsigned char), PLANK_NULL);
    return result;    
}

PlankResult pl_File_ReadUS (PlankFileRef p, unsigned short* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (unsigned short), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUS (data);
    
    return result;        
}

PlankResult pl_File_ReadUI (PlankFileRef p, unsigned int* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (unsigned int), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI (data);
    
    return result;            
}

PlankResult pl_File_ReadUI24 (PlankFileRef p, PlankUI24* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankUI24), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI24 (data);
    
    return result;                
}

PlankResult pl_File_ReadUL (PlankFileRef p, PlankUL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankUL), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUL (data);
    
    return result;                
}

PlankResult pl_File_ReadULL (PlankFileRef p, PlankULL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankULL), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianULL (data);
    
    return result;                
}

PlankResult pl_File_ReadF (PlankFileRef p, float* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (float), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianF (data);
    
    return result;                
}

PlankResult pl_File_ReadD (PlankFileRef p, double* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (double), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianD (data);
    
    return result;                
}

PlankResult pl_File_ReadFourCharCode (PlankFileRef p, PlankFourCharCode* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (*data), PLANK_NULL);
    
#if PLANK_BIGENDIAN
    pl_SwapEndianI ((PlankI*) data);
#endif
    
    return result;    
}

PlankResult pl_File_ReadGUID (PlankFileRef p, PlankGUIDRef data)
{
    PlankResult result;
    result = pl_File_Read (p, data, sizeof (PlankGUID), PLANK_NULL);
    return result;
}

PlankResult pl_File_ReadPascalString255 (PlankFileRef p, PlankPascalString255* string)
{
    PlankResult result = PlankResult_OK;
    PlankUC length;
    
    if ((result = pl_File_ReadUC (p, &length)) != PlankResult_OK) goto exit;
    
    string->data[0] = length;
    
    if ((result = pl_File_Read (p, &string->data[1], length, PLANK_NULL)) != PlankResult_OK) goto exit;
   
exit:
    return result;        
}

//PlankResult pl_File_ReadLine (PlankFileRef p, char* text, const int maximumLength)
//{
//    PlankResult result;
//    int i;
//    char temp;
//    
//    text[0] = '\0';
//
//    if (p->stream == 0)
//        return PlankResult_FileInvalid;
//    
//    if (p->mode & PLANKFILE_BINARY)
//        return PlankResult_FileReadError;
//    
//    if (! (p->mode & PLANKFILE_READ))
//        return PlankResult_FileReadError;
//    
//    result = PlankResult_OK;
//    
//    do 
//    {
//        result = pl_File_ReadC (p, &temp);
//    } while ((result == PlankResult_OK) && ((temp != '\r') || (temp != '\n')));
//
//    if (result == PlankResult_OK)
//        text[0] = temp;
//
//    i = 1;
//    
//    while ((i < (maximumLength - 1)) && (result == PlankResult_OK))
//    {
//        result = pl_File_ReadC (p, &temp);
//        
//        if ((result != PlankResult_OK) || (temp == '\r') || (temp == '\n') || (temp == '\0'))
//        {
//            text[i] = '\0';
//            break;
//        }
//        
//        text[i] = temp;
//        i++;
//    }
//    
//    return result;
//}

PlankResult pl_File_ReadLine (PlankFileRef p, char* text, const int maximumLength)
{
    PlankResult result;
    int i;
    char temp;
    
    text[0] = '\0';
    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    if (p->mode & PLANKFILE_BINARY)
        return PlankResult_FileReadError;
    
    if (! (p->mode & PLANKFILE_READ))
        return PlankResult_FileReadError;
    
    result = PlankResult_OK;
    
    do
    {
        result = pl_File_ReadC (p, &temp);
    } while ((result == PlankResult_OK) && ((temp == '\r') || (temp == '\n')));
    
    if (result == PlankResult_OK)
        text[0] = temp;
    
    i = 1;
    
    while ((i < (maximumLength - 1)) && (result == PlankResult_OK))
    {
        result = pl_File_ReadC (p, &temp);
        
        if ((result != PlankResult_OK) || (temp == '\r') || (temp == '\n') || (temp == '\0'))
        {
            text[i] = '\0';
            break;
        }
        
        text[i] = temp;
        i++;
    }
    
    return result;
}

#define PLANKFILE_SKIPBYTES_SIZE 256

PlankResult pl_File_SkipBytes (PlankFileRef p, const int numBytes)
{
    char temp[PLANKFILE_SKIPBYTES_SIZE];
    PlankResult result;
    int bytesRemaining, bytesThisTime;

    result = PlankResult_OK;
    bytesRemaining = numBytes;
    
    while (bytesRemaining > 0)
    {
        bytesThisTime = pl_MinI (bytesRemaining, PLANKFILE_SKIPBYTES_SIZE);
        bytesRemaining -= bytesThisTime;
        
        if ((result = pl_File_Read (p, temp, bytesThisTime, PLANK_NULL)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_File_Write (PlankFileRef p, const void* data, const int maximumBytes)
{    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    return (p->writeFunction) (p, data, maximumBytes);
}

#define PLANKFILE_WRITEBYTES_SIZE 4096

PlankResult pl_File_WriteDynamicArray (PlankFileRef p, PlankDynamicArrayRef array)
{
    PlankResult result;
    PlankL remaining;
    PlankUC* ptr;
    int thisTime;
    
    result    = PlankResult_OK;
    remaining = pl_DynamicArray_GetItemSize (array) * pl_DynamicArray_GetSize (array);
    ptr       = (PlankUC*)pl_DynamicArray_GetArray (array);

    while (remaining > 0)
    {
        thisTime = (int)pl_MinL (remaining, PLANKFILE_WRITEBYTES_SIZE);
        
        if ((result = pl_File_Write (p, ptr, thisTime)) != PlankResult_OK) goto exit;
        
        remaining -= thisTime;
    }
    
exit:
    return result;
}

PlankResult pl_File_WriteZeros (PlankFileRef p, const int numBytes)
{
    char temp[PLANKFILE_SKIPBYTES_SIZE];
    PlankResult result;
    int bytesRemaining, bytesThisTime;
    
    result = PlankResult_OK;
    bytesRemaining = numBytes;
    pl_MemoryZero (temp, pl_MinI (numBytes, PLANKFILE_SKIPBYTES_SIZE));
    
    while (bytesRemaining > 0)
    {
        bytesThisTime = pl_MinI (bytesRemaining, PLANKFILE_SKIPBYTES_SIZE);
        bytesRemaining -= bytesThisTime;
        
        if ((result = pl_File_Write (p, temp, bytesThisTime)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;    
}

PlankResult pl_File_WriteC (PlankFileRef p, char data)
{
    PlankResult result;
    result = pl_File_Write (p, &data, sizeof (char));
    return result;    
}

PlankResult pl_File_WriteS (PlankFileRef p, short data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianS (&data);
    
    result = pl_File_Write (p, &data, sizeof (short));
    
    return result;        
}

PlankResult pl_File_WriteI (PlankFileRef p, int data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI (&data);

    result = pl_File_Write (p, &data, sizeof (int));
    
    return result;        
}

PlankResult pl_File_WriteI24 (PlankFileRef p, PlankI24 data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI24 (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankI24));
    
    return result;            
}

/*
PlankResult pl_File_WriteL (PlankFileRef p, PlankL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankL));
    
    return result;            
}
*/

PlankResult pl_File_WriteLL (PlankFileRef p, PlankLL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianLL (&data);

    result = pl_File_Write (p, &data, sizeof (PlankLL));
    
    return result;        
}

PlankResult pl_File_WriteUC (PlankFileRef p, unsigned char data)
{
    PlankResult result;
    result = pl_File_Write (p, &data, sizeof (unsigned char));
    return result;        
}

PlankResult pl_File_WriteUS (PlankFileRef p, unsigned short data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUS (&data);
    
    result = pl_File_Write (p, &data, sizeof (unsigned short));
    
    return result;            
}

PlankResult pl_File_WriteUI (PlankFileRef p, unsigned int data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI (&data);
    
    result = pl_File_Write (p, &data, sizeof (unsigned int));
    
    return result;                
}

PlankResult pl_File_WriteUI24 (PlankFileRef p, PlankUI24 data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI24 (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankUI24));
    
    return result;                    
}

PlankResult pl_File_WriteUL (PlankFileRef p, PlankUL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankUL));
    
    return result;                
}

PlankResult pl_File_WriteULL (PlankFileRef p, PlankULL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianULL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankULL));
    
    return result;                
}

PlankResult pl_File_WriteF (PlankFileRef p, float data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianF (&data);
    
    result = pl_File_Write (p, &data, sizeof (float));
    
    return result;        
}

PlankResult pl_File_WriteD (PlankFileRef p, double data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianD (&data);
    
    result = pl_File_Write (p, &data, sizeof (double));
    
    return result;        
}

PlankResult pl_File_WriteFourCharCode (PlankFileRef p, PlankFourCharCode data)
{
    return pl_File_Write (p, &data, sizeof (data));
}

PlankResult pl_File_WriteGUID (PlankFileRef p, const PlankGUID* guid)
{
    return pl_File_Write (p, guid, sizeof (PlankGUID));
}

PlankResult pl_File_WriteString (PlankFileRef p, const char* text)
{    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
//    if (p->mode & PLANKFILE_BINARY)
//        return PlankResult_FileWriteError;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    return (p->writeFunction) (p, text, (int)strlen (text));
}

PlankResult pl_File_WriteLine (PlankFileRef p, const char* text)
{
    PlankResult result;
    
    result = pl_File_WriteString (p, text);
    
    if (result != PlankResult_OK)
        return result;
    
    result = pl_File_WriteString (p, "\r\n");
    
    if (result != PlankResult_OK)
        return result;
    
    return PlankResult_OK;
}

PlankResult pl_File_WritePascalString255 (PlankFileRef p, PlankPascalString255* string)
{
    PlankResult result = PlankResult_OK;
    PlankUC length;
    
    length = string->data[0];
    
    if ((result = pl_File_WriteUC (p, length)) != PlankResult_OK) goto exit;
    
    if (length > 0)
        if ((result = pl_File_Write (p, &string->data[1], length)) != PlankResult_OK) goto exit;
    
exit:
    return result;        
}
