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

#ifndef PLONK_ENDIAN_H
#define PLONK_ENDIAN_H

#include "../containers/plonk_ContainerForwardDeclarations.h"

template<UnsignedLong length>
class EndianUtility
{
public:
    static inline void swap (void* bits) throw()
    {
        unsigned char* data = reinterpret_cast<unsigned char*> (bits);
        const unsigned int numSwap = length / 2;
        const unsigned int end = length - 1;
                
        for (int i = 0; i < numSwap; ++i)
        {
            const unsigned char temp = data[i];
            data[i] = data[end - i];
            data[end - i] = temp;
        }
    }    
    
    static inline void swap (void* bits, 
                             const unsigned int itemSize, 
                             const unsigned long numItems) throw()
    {
        unsigned char* data = reinterpret_cast<unsigned char*> (bits);

        for (int i = 0; i < numItems; ++i, data += itemSize)
            swap (reinterpret_cast<void*> (data));
    }
};

template<>
class EndianUtility<1>
{
public:
    static inline void swap (void* bits) throw()
    {
        (void)bits;
    }
    
    static inline void swap (void* bits, 
                             const unsigned int itemSize, 
                             const unsigned long numItems) throw()
    {
        (void)bits;
        (void)itemSize;
        (void)numItems;
    }
};

template<>
class EndianUtility<2>
{
public:
    static inline void swap (void* bits) throw()
    {
        pl_SwapEndianUS (reinterpret_cast<PlankUS*> (bits));
    }
    
    static inline void swap (void* bits, 
                             const unsigned int itemSize, 
                             const unsigned long numItems) throw()
    {
        (void)itemSize;
        pl_VectorSwapEndianUS (reinterpret_cast<PlankUS*> (bits), numItems);
    }
};

template<>
class EndianUtility<3>
{
public:
    static inline void swap (void* bits) throw()
    {
        pl_SwapEndianUI24 (reinterpret_cast<PlankUI24*> (bits));
    }
    
    static inline void swap (void* bits, 
                             const unsigned int itemSize, 
                             const unsigned long numItems) throw()
    {
        (void)itemSize;
        pl_VectorSwapEndianUI24 (reinterpret_cast<PlankUI24*> (bits), numItems);
    }    
};

template<>
class EndianUtility<4>
{
public:
    static inline void swap (void* bits) throw()
    {
        pl_SwapEndianUI (reinterpret_cast<PlankUI*> (bits));
    }
    
    static inline void swap (void* bits, 
                             const unsigned int itemSize, 
                             const unsigned long numItems) throw()
    {
        (void)itemSize;
        pl_VectorSwapEndianUI (reinterpret_cast<PlankUI*> (bits), numItems);
    }    
};

template<>
class EndianUtility<8>
{
public:
    static inline void swap (void* bits) throw()
    {
        pl_SwapEndianULL (reinterpret_cast<PlankULL*> (bits));
    }
    
    static inline void swap (void* bits, 
                             const unsigned int itemSize, 
                             const unsigned long numItems) throw()
    {
        (void)itemSize;
        pl_VectorSwapEndianULL (reinterpret_cast<PlankULL*> (bits), numItems);
    }    
};


/** A utility class to swap the endiannes (byte-order) of one or more data elements. 
 @ingroup PlonkOtherUserClasses */
class Endian
{
public:        
    template<class Type>
    static inline void swap (Type& data) throw()
    {
        typedef EndianUtility<sizeof (Type)> EndianUtilityType;
        
        EndianUtilityType::swap (reinterpret_cast<void*> (&data));
    }
    
    template<class Type>
    static inline void swap (Type* data) throw()
    {
        typedef EndianUtility<sizeof (Type)> EndianUtilityType;
        
        EndianUtilityType::swap (reinterpret_cast<void*> (data));
    }

    template<class Type>
    static inline void swap (Type* data, const int numItems) throw()
    {
        typedef EndianUtility<sizeof (Type)> EndianUtilityType;
        
        EndianUtilityType::swap (reinterpret_cast<void*> (data), 
                                 sizeof (Type), 
                                 numItems);
    }
    
    template<class Type>
    static void swap (NumericalArray<Type>& array) throw()
    {
        typedef EndianUtility<sizeof (Type)> EndianUtilityType;
        
        EndianUtilityType::swap (reinterpret_cast<void*> (array.getArray()), 
                                 sizeof (Type), 
                                 array.length());
    }
    
};

/** A utility class to swap the endiannes (byte-order) of one or more data elements. 
 The swap functions here only swap the data if the code is running on a big endian architecture. */
class EndianIfBig : public Endian
{
#if PLONK_LITTLEENDIAN
public:
    template<class Type>
    static inline void swap (Type& /*data*/) throw()
    {
    }
    
    template<class Type>
    static inline void swap (Type* /*data*/) throw()
    {
    }
    
    template<class Type>
    static inline void swap (Type* /*data*/, const int /*numItems*/) throw()
    {
    }

    template<class Type>
    static void swap (NumericalArray<Type>& /*array*/) throw()
    {
    }
#endif
};

/** A utility class to swap the endiannes (byte-order) of one or more data elements. 
 The swap functions here only swap the data if the code is running on a little endian architecture. */
class EndianIfLittle : public Endian
{
#if PLONK_BIGENDIAN
public:
    template<class Type>
    static inline void swap (Type& /*data*/) throw()
    {
    }
    
    template<class Type>
    static inline void swap (Type* /*data*/) throw()
    {
    }
    
    template<class Type>
    static inline void swap (Type* /*data*/, const int /*numItems*/) throw()
    {
    }
    
    template<class Type>
    static void swap (NumericalArray<Type>& /*array*/) throw()
    {
    }
#endif
};



#endif // PLONK_ENDIAN_H
