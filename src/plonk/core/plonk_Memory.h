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

#ifndef PLONK_MEMORY_H
#define PLONK_MEMORY_H

template<class Type>
class ObjectMemoryAllocator;



class Memory
{
public:
    typedef PlankMemoryAllocateBytesFunction AllocateBytesFunction;
    typedef PlankMemoryFreeFunction FreeFunction;
    
    static Memory& global() throw()
    {
        static Memory memory;
        return memory;
    }
    
    static Memory& realTime() throw()
    {
        static Memory memory;
        return memory;
    }
    
    Memory() throw()
    {
        pl_Memory_Init (&internal);
    }
    
    inline void* allocateBytes (const UnsignedLong numBytes) throw()
    {
        void* const ptr = pl_Memory_AllocateBytes (&internal, numBytes);
        plonk_assert (ptr != 0);
        return ptr;
    }
    
    inline void free (void* ptr) throw()
    {
        const ResultCode result = pl_Memory_Free (&internal, ptr);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void setFunctions (AllocateBytesFunction allocateFunction, FreeFunction freeFunction) throw()
    {
        const ResultCode result = pl_Memory_SetFunctions (&internal, allocateFunction, freeFunction);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif        
    }
    
    static inline void zero (void* const ptr, const UnsignedLong numBytes) throw()
    {
        const ResultCode result = pl_MemoryZero (ptr, numBytes);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    template<class Type>
    static inline void zero (Type& object) throw()
    {
        Memory::zero (&object, sizeof (Type));
    }
    
    static inline void copy (void* const dst, const void* const src, const UnsignedLong numBytes) throw()
    {
        const ResultCode result = pl_MemoryCopy (dst, src, numBytes);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }

    
private:
    PlankMemory internal;
    
    Memory (Memory const&);
    Memory& operator= (Memory const&);
};

template<class Type>
class ArrayAllocator
{
public:
    static Type* allocate (const int numItems) throw()  { return new Type[numItems]; }
    static void free (Type* const ptr) throw()          { delete [] ptr; }
};

template<>
class ArrayAllocator<Float>
{
public:
    typedef Float Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

template<>
class ArrayAllocator<Double>
{
public:
    typedef Double Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

template<>
class ArrayAllocator<Int>
{
public:
    typedef Int Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

template<>
class ArrayAllocator<Short>
{
public:
    typedef Short Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

template<>
class ArrayAllocator<Long>
{
public:
    typedef Long Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

#if ! (PLANK_WIN && PLANK_64BIT)
template<>
class ArrayAllocator<LongLong>
{
public:
    typedef LongLong Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};
#endif

template<>
class ArrayAllocator<Char>
{
public:
    typedef Char Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

template<>
class ArrayAllocator<Bool>
{
public:
    typedef Bool Type;
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};







#endif // PLONK_MEMORY_H
