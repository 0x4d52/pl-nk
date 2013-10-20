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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"

// memory stream callbacks...

PlankResult BinaryFileInternal::dynamicMemoryOpenCallback (PlankFileRef p)
{
    plonk_assert (p != 0);

    if ((p->mode & PLANKFILE_WRITE) && (p->mode & PLANKFILE_APPEND))
    {
        ByteArray* array = static_cast<ByteArray*> (p->stream);
        p->position = array->length();;
    }
    else
    {
        p->position = 0;
    }
    
    return PlankResult_OK;
}

PlankResult BinaryFileInternal::dynamicMemoryCloseCallback (PlankFileRef p)
{
    plonk_assert (p != 0);
    ByteArray* array = static_cast<ByteArray*> (p->stream);
    delete array; // deletes the container, which of course holds a ref-counted object
    return pl_File_Init (p);
}

PlankResult BinaryFileInternal::dynamicMemoryClearCallback (PlankFileRef p)
{
    plonk_assert (p != 0);
    ByteArray* array = static_cast<ByteArray*> (p->stream);
    array->setSize (0, false);
    return PlankResult_OK;
}

PlankResult BinaryFileInternal::dynamicMemoryGetStatusCallback (PlankFileRef p, int type, int* status)
{
    plonk_assert (p != 0);
    ByteArray* array = static_cast<ByteArray*> (p->stream);
    LongLong size = array->length();
        
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

PlankResult BinaryFileInternal::dynamicMemoryReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesReadOut)
{
    plonk_assert (p != 0);
    plonk_assert (ptr != 0);
    plonk_assert (maximumBytes > 0);

    PlankResult result;
    UnsignedChar* src;
    ByteArray* array = static_cast<ByteArray*> (p->stream);
    LongLong size = array->length();
    int bytesRead;
    
    result      = PlankResult_OK;
    bytesRead   = int (plonk::min (LongLong (maximumBytes), size - p->position));
    
    if (bytesRead <= 0)
    {
        result = PlankResult_FileEOF;
        goto exit;
    }
    
    src = (UnsignedChar*)array->getArray() + p->position;
    Memory::copy (ptr, src, bytesRead);
    p->position += bytesRead;
    
    if (bytesReadOut)
        *bytesReadOut = bytesRead;
    
exit:
    return result;
}

PlankResult BinaryFileInternal::dynamicMemoryWriteCallback (PlankFileRef p, const void* data, const int maximumBytes)
{
    plonk_assert (p != 0);
    plonk_assert (data != 0);
    plonk_assert (maximumBytes > 0);
    
    PlankResult result;
    UnsignedChar* dst;
    ByteArray* array = static_cast<ByteArray*> (p->stream);
    LongLong sizeNeeded;
    
    result      = PlankResult_OK;
    sizeNeeded  = p->position + maximumBytes;
    
    array->setSize (plonk::max ((int)sizeNeeded, array->length()), true);
    dst = (UnsignedChar*)array->getArray() + p->position;
    Memory::copy (dst, data, maximumBytes);
    p->position += maximumBytes;
    
exit:
    return result;
}

PlankResult BinaryFileInternal::dynamicMemorySetPositionCallback (PlankFileRef p, LongLong offset, int code)
{
    plonk_assert (p != 0);

    PlankResult result;
    ByteArray* array = static_cast<ByteArray*> (p->stream);
    LongLong size = array->length();
    LongLong newPosition;

    result = PlankResult_OK;
    
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
    else if (newPosition >= size)
    {
        newPosition = size;
    }
    
    p->position = newPosition;
    
exit:
    return result;
}

PlankResult BinaryFileInternal::dynamicMemoryGetPositionCallback (PlankFileRef p, LongLong* position)
{
    plonk_assert (position != 0);
    *position = p->position;
    return PlankResult_OK;
}

BinaryFileInternal::BinaryFileInternal() throw()
{
    pl_File_Init (getPeerRef());
}

BinaryFileInternal::BinaryFileInternal (Text const& path,
                                        const bool writable, 
                                        const bool clearContents,
                                        const bool bigEndian) throw()
{
    pl_File_Init (getPeerRef());

    plonk_assert ((writable == true) || (clearContents == false));

    ResultCode result;
    
    if (clearContents)
        pl_FileErase (path.getArray());
    
    result = writable ? pl_File_OpenBinaryWrite (getPeerRef(), path.getArray(), true, clearContents, bigEndian) : 
                        pl_File_OpenBinaryRead (getPeerRef(), path.getArray(), false, bigEndian);
    
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif
}

BinaryFileInternal::BinaryFileInternal (ByteArray const& bytes,
                                        const bool writable) throw()
{    
    setupBytes (getPeerRef(), bytes, writable);
}

bool BinaryFileInternal::setupBytes (PlankFileRef p, ByteArray const& bytes, const bool writable) throw()
{
    ResultCode result = PlankResult_OK;
    pl_File_Init (p);
    
    if (p->stream != 0)
    {
        if ((result = pl_File_Close (p)) != PlankResult_OK)
        {
            plonk_assertfalse;
            return false;
        }
    }
    
    p->path[0] = '\0';
    p->stream = new ByteArray (bytes);
    p->size = 0;
    p->mode = PLANKFILE_BINARY | PLANKFILE_NATIVEENDIAN | PLANKFILE_READ;
    
    if (writable)
        p->mode |= PLANKFILE_WRITE;
    
    p->type = PLANKFILE_STREAMTYPE_OTHER;
    
    result = pl_File_SetFunction (p,
                                  dynamicMemoryOpenCallback,
                                  dynamicMemoryCloseCallback,
                                  dynamicMemoryClearCallback,
                                  dynamicMemoryGetStatusCallback,
                                  dynamicMemoryReadCallback,
                                  dynamicMemoryWriteCallback,
                                  dynamicMemorySetPositionCallback,
                                  dynamicMemoryGetPositionCallback);
    
    plonk_assert (result == PlankResult_OK);
    result = (p->openFunction) (p);
    plonk_assert (result == PlankResult_OK);
    return result == PlankResult_OK;    
}

bool BinaryFileInternal::setupMulti (PlankFileRef p, FilePathArray const& fileArray, const int multiMode, const bool bigEndian, IntVariable* indexRef) throw()
{
    pl_File_Init (p);
    
    const bool shouldTakeOwnership = true;
    
    ResultCode result;
    PlankDynamicArrayRef array = pl_DynamicArray_Create();
    pl_DynamicArray_InitWithItemSizeAndSize (array, sizeof (PlankFile), fileArray.length(), true);
    PlankFile* rawArray = static_cast<PlankFile*> (pl_DynamicArray_GetArray (array));
    
    for (int i = 0; i < fileArray.length(); ++i)
    {
        result = pl_File_Init (&rawArray[i]);
        plonk_assert (result == PlankResult_OK);
        result = pl_File_OpenBinaryRead (&rawArray[i], fileArray.atUnchecked(i).fullpath().getArray(), false, bigEndian);
        plonk_assert (result == PlankResult_OK);
    }
    
    PlankMulitFileReaderRef multi = pl_MultiFileReader_Create();

    int mode = PLANKFILE_READ | PLANKFILE_BINARY;
    
    if (bigEndian)
        mode |= PLANKFILE_BIGENDIAN;

    switch (multiMode)
    {
        case BinaryFile::MultiFileArraySequenceOnce:
        {
            result = pl_MultiFileReader_InitArraySequence (multi, mode, array, shouldTakeOwnership, false);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case BinaryFile::MultiFileArraySequenceLoop:
        {
            result = pl_MultiFileReader_InitArraySequence (multi, mode, array, shouldTakeOwnership, true);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case BinaryFile::MultiFileArrayRandom:
        {
            result = pl_MultiFileReader_InitArrayRandom (multi, mode, array, shouldTakeOwnership, false);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case BinaryFile::MultiFileArrayRandomNoRepeat:
        {
            result = pl_MultiFileReader_InitArrayRandom (multi, mode, array, shouldTakeOwnership, true);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case BinaryFile::MultiFileArrayIndexRef:
        {
            int *indexRefPtr = indexRef ? indexRef->getValuePtr() : 0;
            result = pl_MultiFileReader_InitArrayIndexRef (multi, mode, array, shouldTakeOwnership, indexRefPtr);
            plonk_assert (result == PlankResult_OK);
        } break;

        default:
            pl_DynamicArray_Destroy (array);
            pl_MultiFileReader_Destroy (multi);
            array = (PlankDynamicArrayRef)PLANK_NULL;
            multi = (PlankMulitFileReaderRef)PLANK_NULL;
            break;
    }
    
    if (multi != PLANK_NULL)
    {        
        result = pl_File_OpenMulti (p, multi, mode);
        plonk_assert (result == PlankResult_OK);
    }
    
    return result == PlankResult_OK;
}

static PlankResult plonk_BinaryFileInternal_BinaryFileQueue_NextFuntion (PlankMulitFileReaderRef p)
{
    if (!p->currentFile)
        p->currentFile = pl_File_CreateAndInit();
    
    BinaryFileQueue& queue = *static_cast<BinaryFileQueue*> (p->source);
    BinaryFile binaryFile = queue.pop();
    
    pl_File_DeInit (p->currentFile);
    binaryFile.disownPeer (p->currentFile);
    
    return PlankResult_OK;
}

static PlankResult plonk_BinaryFileInternal_BinaryFileQueue_DestroyCustomFuntion (PlankMulitFileReaderRef p)
{
    BinaryFileQueue* queue = static_cast<BinaryFileQueue*> (p->source);
    delete queue;
    
    if (p->currentFile)
    {
        pl_File_Destroy (p->currentFile);
        p->currentFile = (PlankFileRef)PLANK_NULL;
    }
    
    return PlankResult_OK;
}

bool BinaryFileInternal::setupMulti (PlankFileRef p, BinaryFileQueue const& fileQueue, const bool bigEndian) throw()
{
    pl_File_Init (p);
    
    const bool shouldTakeOwnership = true;
    ResultCode result;
    PlankMulitFileReaderRef multi = pl_MultiFileReader_Create();
    
    int mode = PLANKFILE_READ | PLANKFILE_BINARY;
    
    if (bigEndian)
        mode |= PLANKFILE_BIGENDIAN;

    result = pl_MultiFileReader_InitCustom (multi,
                                            mode,
                                            new BinaryFileQueue (fileQueue),
                                            shouldTakeOwnership,
                                            plonk_BinaryFileInternal_BinaryFileQueue_NextFuntion,
                                            plonk_BinaryFileInternal_BinaryFileQueue_DestroyCustomFuntion);

    result = pl_File_OpenMulti (p, multi, mode);
    plonk_assert (result == PlankResult_OK);
    
    return result == PlankResult_OK;
}

BinaryFileInternal::BinaryFileInternal (FilePathArray const& fileArray, const int multiMode, const bool bigEndian) throw()
{
    setupMulti (getPeerRef(), fileArray, multiMode, bigEndian);
}

static PlankResult plonk_BinaryFileInternal_FilePathQueue_NextFuntion (PlankMulitFileReaderRef p)
{
    if (!p->currentFile)
        p->currentFile = pl_File_CreateAndInit();
    
    FilePathQueue& queue = *static_cast<FilePathQueue*> (p->source);
    FilePath path = queue.pop();
    
    pl_File_DeInit (p->currentFile);
    pl_File_Init (p->currentFile);
    pl_File_Open (p->currentFile, path.fullpath().getArray(), p->fileMode);
    
    return PlankResult_OK;
}

static PlankResult plonk_BinaryFileInternal_FilePathQueue_DestroyCustomFuntion (PlankMulitFileReaderRef p)
{
    FilePathQueue* queue = static_cast<FilePathQueue*> (p->source);
    delete queue;
    
    if (p->currentFile)
    {
        pl_File_Destroy (p->currentFile);
        p->currentFile = (PlankFileRef)PLANK_NULL;
    }
    
    return PlankResult_OK;
}

bool BinaryFileInternal::setupMulti (PlankFileRef p, FilePathQueue const& fileQueue, const bool bigEndian) throw()
{
    pl_File_Init (p);
    
    const bool shouldTakeOwnership = true;
    ResultCode result;
    PlankMulitFileReaderRef multi = pl_MultiFileReader_Create();
    
    int mode = PLANKFILE_READ | PLANKFILE_BINARY;
    
    if (bigEndian)
        mode |= PLANKFILE_BIGENDIAN;

    result = pl_MultiFileReader_InitCustom (multi,
                                            mode,
                                            new FilePathQueue (fileQueue),
                                            shouldTakeOwnership,
                                            plonk_BinaryFileInternal_FilePathQueue_NextFuntion,
                                            plonk_BinaryFileInternal_FilePathQueue_DestroyCustomFuntion);
    
    result = pl_File_OpenMulti (p, multi, mode);
    plonk_assert (result == PlankResult_OK);
    
    return result == PlankResult_OK;
}

BinaryFileInternal::BinaryFileInternal (BinaryFileQueue const& fileQueue, const bool bigEndian) throw()
{
    setupMulti (getPeerRef(), fileQueue, bigEndian);
}

BinaryFileInternal::BinaryFileInternal (FilePathQueue const& fileQueue, const bool bigEndian) throw()
{
    setupMulti (getPeerRef(), fileQueue, bigEndian);
}

BinaryFileInternal::BinaryFileInternal (PlankFileRef fileRef) throw()
{
    Memory::copy (getPeerRef(), fileRef, sizeof (*fileRef));
    Memory::zero (fileRef, sizeof (*fileRef));
    pl_File_Init (fileRef);
}

BinaryFileInternal::~BinaryFileInternal()
{
    pl_File_DeInit (getPeerRef());
}

void BinaryFileInternal::disownPeer (PlankFileRef fileRef) throw()
{
    Memory::copy (fileRef, getPeerRef(), sizeof (*fileRef));
    Memory::zero (getPeerRef(), sizeof (*fileRef));
    pl_File_Init (getPeerRef());
}

LongLong BinaryFileInternal::getPosition() const throw()
{
    LongLong position;
    
    const ResultCode result = pl_File_GetPosition (getPeerRef(), &position);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
    
    return position;
}

void BinaryFileInternal::setPosition(const LongLong position) throw()
{
    const ResultCode result = pl_File_SetPosition (getPeerRef(), position);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::setEof() throw()
{
    const ResultCode result = pl_File_SetEOF (getPeerRef());
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

bool BinaryFileInternal::isEof() const throw()
{
    return pl_File_IsEOF (getPeerRef());
}

bool BinaryFileInternal::isBigEndian() const throw()
{
    return pl_File_IsBigEndian (getPeerRef());
}

bool BinaryFileInternal::isLittleEndian() const throw()
{
    return pl_File_IsLittleEndian (getPeerRef());
}

bool BinaryFileInternal::isNativeEndian() const throw()
{
    return pl_File_IsNativeEndian (getPeerRef());
}

bool BinaryFileInternal::canRead() const throw()
{
    int mode;
    pl_File_GetMode (getPeerRef(), &mode);
    return mode & PLANKFILE_READ;
}

bool BinaryFileInternal::canWrite() const throw()
{
    int mode;
    pl_File_GetMode (getPeerRef(), &mode);
    return mode & PLANKFILE_WRITE;
}

int BinaryFileInternal::read (void* data, const int maximumBytes) throw()
{   
    int bytesRead;
    const ResultCode result = pl_File_Read (getPeerRef(), data, maximumBytes, &bytesRead);
    plonk_assert (result == PlankResult_OK || result == PlankResult_FileEOF); 
    
#ifndef PLONK_DEBUG
    (void)result;
#endif        
    
    return bytesRead;
}

void BinaryFileInternal::read (char& value) throw()
{
    const ResultCode result = pl_File_ReadC (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif            
}

void BinaryFileInternal::read (short& value) throw()
{
    const ResultCode result = pl_File_ReadS (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (int& value) throw()
{
    const ResultCode result = pl_File_ReadI (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (Int24& value) throw()
{
    const ResultCode result = pl_File_ReadI24 (getPeerRef(), 
                                               reinterpret_cast<Int24::Internal*> (&value) );
    plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
    (void)result;
#endif                    
}

/*
void BinaryFileInternal::read (long& value) throw()
{
    const ResultCode result = pl_File_ReadL (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}
*/

void BinaryFileInternal::read (LongLong& value) throw()
{
    const ResultCode result = pl_File_ReadLL (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (unsigned char& value) throw()
{
    const ResultCode result = pl_File_ReadUC (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (unsigned short& value) throw()
{
    const ResultCode result = pl_File_ReadUS (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (unsigned int& value) throw()
{
    const ResultCode result = pl_File_ReadUI (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

/*
void BinaryFileInternal::read (unsigned long& value) throw()
{
    const ResultCode result = pl_File_ReadUL (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}
*/

void BinaryFileInternal::read (UnsignedLongLong& value) throw()
{
    const ResultCode result = pl_File_ReadULL (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (float& value) throw()
{
    const ResultCode result = pl_File_ReadF (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::read (double& value) throw()
{
    const ResultCode result = pl_File_ReadD (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

void BinaryFileInternal::write (void* data, const int maximumBytes) throw()
{
    const ResultCode result = pl_File_Write (getPeerRef(), data, maximumBytes);
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

void BinaryFileInternal::write (const char value) throw()
{
    const ResultCode result = pl_File_WriteC (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const short value) throw()
{
    const ResultCode result = pl_File_WriteS (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const int value) throw()
{
    const ResultCode result = pl_File_WriteI (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const Int24 value) throw()
{
    const ResultCode result = pl_File_WriteI24 (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

void BinaryFileInternal::write (const LongLong value) throw()
{
    const ResultCode result = pl_File_WriteLL (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const unsigned char value) throw()
{
    const ResultCode result = pl_File_WriteUC (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const unsigned short value) throw()
{
    const ResultCode result = pl_File_WriteUS (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const unsigned int value) throw()
{
    const ResultCode result = pl_File_WriteUI (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const UnsignedLongLong value) throw()
{
    const ResultCode result = pl_File_WriteULL (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void BinaryFileInternal::write (const float value) throw()
{
    const ResultCode result = pl_File_WriteF (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

void BinaryFileInternal::write (const double value) throw()
{
    const ResultCode result = pl_File_WriteD (getPeerRef(), value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

//void BinaryFileInternal::disownPeer (PlankFileRef otherFile) throw()
//{
//    pl_MemoryCopy (otherFile, getPeerRef(), sizeof (PlankFile));
//    pl_MemoryZero (getPeerRef(), sizeof (PlankFile));
//}

ResultCode BinaryFileInternal::copy (BinaryFileInternal* source, const LongLong size) throw()
{
    const ResultCode result = pl_File_Copy (getPeerRef(), source->getPeerRef(), size);
    plonk_assert (result == PlankResult_OK);
    return result;
}



END_PLONK_NAMESPACE
