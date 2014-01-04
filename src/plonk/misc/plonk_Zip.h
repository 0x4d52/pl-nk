/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_ZIP_H
#define PLONK_ZIP_H


class ZipInternal : public SmartPointer
{
public:
    inline ZipInternal() throw()
    {
        pl_Zip_Init (&peer);
    }
    
    inline ~ZipInternal()
    {
        pl_Zip_DeInit (&peer);
    }
    
    friend class Zip;
    
private:
    PlankZip peer;
};


class Zip : public SmartPointerContainer<ZipInternal>
{
public:
    typedef SmartPointerContainer<ZipInternal> Base;
    
    inline Zip() throw()
    :   Base (new ZipInternal())
    {
    }
        
    inline const void* deflate (const int amount, const void* dataInput, const Long dataInputLength, Long* dataOutputLength)
    {
        plonk_assert (dataOutputLength != 0);
        return pl_Zip_DeflateData (&getInternal()->peer, amount, dataInput, dataInputLength, dataOutputLength);
    }
    
    template<class NumericalType>
    ByteArray deflate (const int amount, NumericalArray<NumericalType> const& array) throw()
    {        
        Long dataOutputLength;
        const void* data = deflate (amount, array.getArray(), array.length() * sizeof (NumericalType), &dataOutputLength);
        return ByteArray::withArray ((int)dataOutputLength, static_cast<const UnsignedChar*> (data), false);
    }

    inline const void* inflate (const void* dataInput, const Long dataInputLength, Long* dataOutputLength)
    {
        return pl_Zip_InflateData (&getInternal()->peer, dataInput, dataInputLength, dataOutputLength);
    }

    template<class NumericalType>
    NumericalArray<NumericalType> inflate (ByteArray const& bytes) throw()
    {
        Long dataOutputLength;
        const void* data = inflate (bytes.getArray(), bytes.length(), &dataOutputLength);
        plonk_assert ((dataOutputLength % sizeof (NumericalType)) == 0);
        return NumericalArray<NumericalType>::withArray (dataOutputLength / sizeof (NumericalType), static_cast<const float*> (data), false);
    }    
};

#endif // PLONK_ZIP_H
