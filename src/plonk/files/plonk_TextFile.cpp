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

TextFileInternal::TextFileInternal() throw()
{
    pl_File_Init (getPeerRef());
}

TextFileInternal::TextFileInternal (Text const& path, 
                                    const bool writable,
                                    const bool clearContents) throw()
{
    pl_File_Init (getPeerRef());

    plonk_assert ((writable == true) || (clearContents == false));
    
    ResultCode result;
    
    if (clearContents)
        pl_FileErase (path.getArray());
        
    result = writable ? pl_File_OpenTextWrite (getPeerRef(), path.getArray(), true, clearContents) : 
                        pl_File_OpenTextRead (getPeerRef(), path.getArray(), false);
    
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

TextFileInternal::TextFileInternal (FilePathArray const& fileArray, const int multiMode) throw()
{
    pl_File_Init (getPeerRef());

    const bool shouldTakeOwnership = true;
    
    ResultCode result;
    PlankDynamicArrayRef array = pl_DynamicArray_Create();
    pl_DynamicArray_InitWithItemSizeAndSize (array, sizeof (PlankFile), fileArray.length(), true);
    PlankFile* rawArray = static_cast<PlankFile*> (pl_DynamicArray_GetArray (array));
    
    for (int i = 0; i < fileArray.length(); ++i)
    {
        result = pl_File_Init (&rawArray[i]);
        plonk_assert (result == PlankResult_OK);
        result = pl_File_OpenTextRead (&rawArray[i], fileArray.atUnchecked(i).fullpath().getArray(), false);
        plonk_assert (result == PlankResult_OK);
    }
    
    PlankMulitFileReaderRef multi = pl_MultiFileReader_Create();
        
    switch (multiMode)
    {
        case TextFile::MultiFileArraySequenceOnce:
        {
            result = pl_MultiFileReader_InitArraySequence (multi, PLANKFILE_READ, array, shouldTakeOwnership, false);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case TextFile::MultiFileArraySequenceLoop:
        {
            result = pl_MultiFileReader_InitArraySequence (multi, PLANKFILE_READ, array, shouldTakeOwnership, true);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case TextFile::MultiFileArrayRandom:
        {
            result = pl_MultiFileReader_InitArrayRandom (multi, PLANKFILE_READ, array, shouldTakeOwnership, false);
            plonk_assert (result == PlankResult_OK);
        } break;
            
        case TextFile::MultiFileArrayRandomNoRepeat:
        {
            result = pl_MultiFileReader_InitArrayRandom (multi, PLANKFILE_READ, array, shouldTakeOwnership, true);
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
        result = pl_File_OpenMulti (getPeerRef(), multi, PLANKFILE_READ);
        plonk_assert (result == PlankResult_OK);
    }
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

static PlankResult plonk_TextFileInternal_TextFileQueue_NextFuntion (PlankMulitFileReaderRef p)
{
    if (!p->currentFile)
        p->currentFile = pl_File_CreateAndInit();
    
    TextFileQueue& queue = *static_cast<TextFileQueue*> (p->source);
    TextFile textFile = queue.pop();

    pl_File_DeInit (p->currentFile);
    textFile.disownPeer (p->currentFile);
    
    return PlankResult_OK;
}

static PlankResult plonk_TextFileInternal_TextFileQueue_DestroyCustomFuntion (PlankMulitFileReaderRef p)
{
    TextFileQueue* queue = static_cast<TextFileQueue*> (p->source);
    delete queue;
    
    if (p->currentFile)
    {
        pl_File_Destroy (p->currentFile);
        p->currentFile = (PlankFileRef)PLANK_NULL;
    }
    
    return PlankResult_OK;
}

TextFileInternal::TextFileInternal (TextFileQueue const& fileQueue) throw()
{
    pl_File_Init (getPeerRef());
    
    const bool shouldTakeOwnership = true;
    ResultCode result;
    PlankMulitFileReaderRef multi = pl_MultiFileReader_Create();
    
    result = pl_MultiFileReader_InitCustom (multi,
                                            PLANKFILE_READ, 
                                            new TextFileQueue (fileQueue),
                                            shouldTakeOwnership,
                                            plonk_TextFileInternal_TextFileQueue_NextFuntion,
                                            plonk_TextFileInternal_TextFileQueue_DestroyCustomFuntion);
    plonk_assert (result == PlankResult_OK);

    result = pl_File_OpenMulti (getPeerRef(), multi, PLANKFILE_READ);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

TextFileInternal::~TextFileInternal()
{
    pl_File_DeInit (getPeerRef());
}

LongLong TextFileInternal::getPosition() const throw()
{
    LongLong position;
    
    const ResultCode result = pl_File_GetPosition (getPeerRef(), &position);
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif
    
    return position;
}

void TextFileInternal::setPosition (const LongLong position) throw()
{
    const ResultCode result = pl_File_SetPosition (getPeerRef(), position);
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

void TextFileInternal::setEof() throw()
{
    const ResultCode result = pl_File_SetEOF (getPeerRef());
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

bool TextFileInternal::isEof() const throw()
{
    return pl_File_IsEOF (getPeerRef());
}

bool TextFileInternal::canRead() const throw()
{
    int mode;
    pl_File_GetMode (getPeerRef(), &mode);
    return mode & PLANKFILE_READ;
}

bool TextFileInternal::canWrite() const throw()
{
    int mode;
    pl_File_GetMode (getPeerRef(), &mode);
    return mode & PLANKFILE_WRITE;
}

Text TextFileInternal::readLine (const int maximumLength) throw()
{
    Text buffer (Text::withSize (maximumLength));
    
    const ResultCode result = pl_File_ReadLine (getPeerRef(), buffer, maximumLength);
    plonk_assert ((result == PlankResult_OK) || (result == PlankResult_FileEOF));
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
    
    return buffer.getArray(); // copies, reducing size
}

Text TextFileInternal::readAll() throw()
{
    Text buffer (Text::emptyWithAllocatedSize (512));
    ResultCode result = PlankResult_OK;
    int bytesRead;
    char temp[17];
    
    while (result == PlankResult_OK)
    {
        result = pl_File_Read (getPeerRef(), temp, 16, &bytesRead);
        temp[bytesRead] = '\0';
        buffer.add (temp);
    }
    
    return buffer;
}

Text TextFileInternal::read (const int numBytes) throw()
{
    Text buffer (Text::emptyWithAllocatedSize (numBytes));
    ResultCode result = PlankResult_OK;
    int bytesRead, bytesRemaining, bytesThisTime;
    char temp[17];
    
    bytesRemaining = numBytes;
    
    while ((result == PlankResult_OK) && (bytesRemaining > 0))
    {
        bytesThisTime = plonk::min (bytesRemaining, 16);
        result = pl_File_Read (getPeerRef(), temp, bytesThisTime, &bytesRead);
        temp[bytesRead] = '\0';
        buffer.add (temp);
        bytesRemaining -= bytesRead;
    }
    
    return buffer;
}

void TextFileInternal::writeValue (const char value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%d", value);
    write (buf);
}

void TextFileInternal::writeValue (const short value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%d", value);
    write (buf);
}

void TextFileInternal::writeValue (const int value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%d", value);
    write (buf);
}

void TextFileInternal::writeValue (const long value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%ld", value);
    write (buf);
}

void TextFileInternal::writeValue (const LongLong value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%lld", value);
    write (buf);
}

void TextFileInternal::writeValue (const unsigned char value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%u", value);
    write (buf);
}

void TextFileInternal::writeValue (const unsigned short value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%u", value);
    write (buf);
}

void TextFileInternal::writeValue (const unsigned int value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%u", value);
    write (buf);
}

void TextFileInternal::writeValue (const unsigned long value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%lu", value);
    write (buf);
}

void TextFileInternal::writeValue (const UnsignedLongLong value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%llu", value);
    write (buf);
}

void TextFileInternal::writeValue (const float value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%f", value);
    write (buf);
}

void TextFileInternal::writeValue (const double value) throw()
{
    const int size (64);
    char buf[size];
    snprintf (buf, size, "%f", value);
    write (buf);
}

void TextFileInternal::write (const char* text) throw()
{
    const ResultCode result = pl_File_WriteString (getPeerRef(), text);
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

void TextFileInternal::writeLine (const char* text) throw()
{
    const ResultCode result = pl_File_WriteLine (getPeerRef(), text);
    plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
    (void)result;
#endif    
}

void TextFileInternal::disownPeer (PlankFileRef otherFile) throw()
{
    pl_MemoryCopy (otherFile, getPeerRef(), sizeof (PlankFile));
    pl_MemoryZero (getPeerRef(), sizeof (PlankFile));
}

END_PLONK_NAMESPACE
