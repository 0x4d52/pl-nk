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

Text TextFileInternal::readLine (const int maximumLength) throw()
{
    char buffer[maximumLength];
    
    const ResultCode result = pl_File_ReadLine (getPeerRef(), buffer, maximumLength);
    plonk_assert ((result == PlankResult_OK) || (result == PlankResult_FileEOF));
    
#ifndef PLONK_DEBUG
    (void)result;
#endif
    
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


END_PLONK_NAMESPACE
