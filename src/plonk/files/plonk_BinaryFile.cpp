/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
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
 
 This software makes use of third party libraries. For more information see:
 doc/license.txt included in the distribution.
 -------------------------------------------------------------------------------
 */

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"

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

BinaryFileInternal::~BinaryFileInternal()
{
    pl_File_DeInit (getPeerRef());
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

void BinaryFileInternal::read (long& value) throw()
{
    const ResultCode result = pl_File_ReadL (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

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

void BinaryFileInternal::read (unsigned long& value) throw()
{
    const ResultCode result = pl_File_ReadUL (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
    
#ifndef PLONK_DEBUG
    (void)result;
#endif                
}

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

void BinaryFileInternal::write (const long value) throw()
{
    const ResultCode result = pl_File_WriteL (getPeerRef(), value);
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

void BinaryFileInternal::write (const unsigned long value) throw()
{
    const ResultCode result = pl_File_WriteUL (getPeerRef(), value);
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



END_PLONK_NAMESPACE
