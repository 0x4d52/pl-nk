/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

#ifndef PLONK_BASE64_H
#define PLONK_BASE64_H


class Base64Internal : public SmartPointer
{
public:
    PLONK_INLINE_LOW Base64Internal() throw()
    {
        pl_Base64_Init (&peer);
    }
    
    PLONK_INLINE_LOW ~Base64Internal()
    {
        pl_Base64_DeInit (&peer);
    }
    
    friend class Base64;
    
private:
    PlankBase64 peer;
};


/** Base64 encoding and decoding. 
 Many of these functions return data that refers to memory allocated by
 this function so */
class Base64 : public SmartPointerContainer<Base64Internal>
{
public:
    typedef SmartPointerContainer<Base64Internal> Base;
    
    PLONK_INLINE_LOW Base64() throw()
    :   Base (new Base64Internal())
    {
    }
        
    PLONK_INLINE_LOW const char* encode (const void* binary, const Long binaryLength)
    {
        return pl_Base64_Encode (&getInternal()->peer, binary, binaryLength);
    }
    
    template<class NumericalType>
    const char* encode (NumericalArray<NumericalType> const& array) throw()
    {
        return encode (array.getArray(), array.length() * sizeof (NumericalType));
    }

    template<class PODType>
    const char* encode (PODType const& data) throw()
    {
        return encode (&data, sizeof (PODType));
    }

    PLONK_INLINE_LOW const void* decode (const char* text, Long* binaryLength)
    {
        return pl_Base64_Decode (&getInternal()->peer, text, binaryLength);
    }

    template<class NumericalType>
    NumericalArray<NumericalType> decode (Text const& text) throw()
    {
        Long length;
        const NumericalType* const array = static_cast<const NumericalType*> (decode (text.getArray(), &length));
        return NumericalArray<NumericalType>::withArray (length / sizeof (NumericalType), array);
    }
    
    template<class PODType>
    const PODType& decodePOD (Text const& text) throw()
    {
        plonk_assert (pl_Base64DecodedLength (text.length()) == sizeof (PODType));
        Long length;
        const PODType* data = static_cast<const PODType*> (decode (text.getArray(), &length));
        return *data;
    }
};

#endif // PLONK_BASE64_H
