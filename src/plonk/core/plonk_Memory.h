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



class Memory
{
public:
    typedef PlankMemoryAllocateBytesFunction AllocateBytesFunction;
    typedef PlankMemoryFreeFunction FreeFunction;
    
    static Memory& global() throw()
    {
        static Memory memory (pl_MemoryGlobal());
        return memory;
    }
    
    Memory() throw()
    :   internal (pl_Memory_CreateAndInit())
    {
    }
    
    Memory (PlankMemory* const internalToUse) throw()
    :   internal (internalToUse)
    {
    }
    
    ~Memory()
    {        
        if (internal != pl_MemoryGlobal())
            pl_Memory_Destroy (internal);
    }
    
    inline void* allocateBytes (const UnsignedLong numBytes) throw()
    {
        void* const ptr = pl_Memory_AllocateBytes (internal, numBytes);
        plonk_assert (ptr != 0);
        return ptr;
    }
    
    inline void free (void* ptr) throw()
    {
        const ResultCode result = pl_Memory_Free (internal, ptr);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    template<class Type>
    void setUserData (Type* userData) throw()
    {
        const ResultCode result = pl_Memory_SetUserData (internal, userData);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    template<class Type>
    void getUserData (Type& userData) throw()
    {
        userData = *static_cast<Type*> (pl_Memory_GetUserData (internal));
    }
    
    inline void* getUserData() throw()
    {
        return pl_Memory_GetUserData (internal);
    }
    
    void resetUserData() throw()
    {
        setUserData (internal);
    }

    void setFunctions (AllocateBytesFunction allocateFunction, FreeFunction freeFunction) throw()
    {
        const ResultCode result = pl_Memory_SetFunctions (internal, allocateFunction, freeFunction);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif        
    }
    
    void resetFunctions() throw()
    {
        setFunctions (0, 0);
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

    static FreeFunction defaultFree;
    static AllocateBytesFunction defaultAllocateBytes;
    
private:
    PlankMemory* const internal;
    
    Memory (Memory const&);
    Memory& operator= (Memory const&);
};

//Memory::FreeFunction Memory::defaultFree = pl_Memory_DefaultFree;
//Memory::AllocateBytesFunction Memory::defaultAllocateBytes = pl_Memory_DefaultAllocateBytes;

//------------------------------------------------------------------------------

class ObjectMemoryBase
{
public:
    ObjectMemoryBase (Memory& m) throw() : memory (m) { }
    ~ObjectMemoryBase() { }
    
    inline Memory& getMemory() throw() { return memory; }
    
    virtual void init() = 0;
    
private:
    Memory& memory;
    
    ObjectMemoryBase();
    ObjectMemoryBase& operator= (const ObjectMemoryBase&);
};

class ObjectMemoryDefault : public ObjectMemoryBase
{
public:     
    ObjectMemoryDefault (Memory& m) throw() 
    :   ObjectMemoryBase (m)
    { 
        getMemory().resetUserData(); 
        getMemory().resetFunctions(); 
    }
    
    void init() throw() { }
};

//------------------------------------------------------------------------------

template<class Type>
class ArrayAllocator
{
public:
    static Type* allocate (const int numItems) throw()  { return new Type[numItems]; }
    static void free (Type* const ptr) throw()          { delete [] ptr; }
};

template<class Type>
class ArrayAllocatorBuiltIn
{
public:
    static Type* allocate (const int numItems) throw()  { return static_cast<Type*> (Memory::global().allocateBytes (numItems * sizeof (Type))); }    
    static void free (Type* const ptr) throw()          { Memory::global().free (static_cast<void*> (ptr)); }
};

template<> class ArrayAllocator<Float>         : public ArrayAllocatorBuiltIn<Float>         { public: typedef Float Type; };
template<> class ArrayAllocator<Double>        : public ArrayAllocatorBuiltIn<Double>        { public: typedef Double Type; };
template<> class ArrayAllocator<Int>           : public ArrayAllocatorBuiltIn<Int>           { public: typedef Int Type; };
template<> class ArrayAllocator<UnsignedInt>   : public ArrayAllocatorBuiltIn<UnsignedInt>   { public: typedef UnsignedInt Type; };
template<> class ArrayAllocator<Short>         : public ArrayAllocatorBuiltIn<Short>         { public: typedef Short Type; };
template<> class ArrayAllocator<UnsignedShort> : public ArrayAllocatorBuiltIn<UnsignedShort> { public: typedef UnsignedShort Type; };
template<> class ArrayAllocator<Long>          : public ArrayAllocatorBuiltIn<Long>          { public: typedef Long Type; };
template<> class ArrayAllocator<UnsignedLong>  : public ArrayAllocatorBuiltIn<UnsignedLong>  { public: typedef UnsignedLong Type; };


#if ! (PLANK_WIN && PLANK_64BIT)
template<> class ArrayAllocator<LongLong>         : public ArrayAllocatorBuiltIn<LongLong>         { public: typedef LongLong Type; };
template<> class ArrayAllocator<UnsignedLongLong> : public ArrayAllocatorBuiltIn<UnsignedLongLong> { public: typedef UnsignedLongLong Type; };
#endif

template<> class ArrayAllocator<Char>          : public ArrayAllocatorBuiltIn<Char>          { public: typedef Char Type; };
template<> class ArrayAllocator<UnsignedChar>  : public ArrayAllocatorBuiltIn<UnsignedChar>  { public: typedef UnsignedChar Type; };
template<> class ArrayAllocator<Bool>          : public ArrayAllocatorBuiltIn<Bool>          { public: typedef Bool Type; };






#endif // PLONK_MEMORY_H
