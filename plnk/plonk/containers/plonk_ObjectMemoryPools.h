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

#ifndef PLONK_OBJECTMEMORYPOOLS_H
#define PLONK_OBJECTMEMORYPOOLS_H

/** Pooled memory for objects and raw arrays.
 This replaces memory allocation functions for objects in the library
 and raw arrays of simple types.
 
 This maintains a series of queues that contain memory blocks that are 2**N.
 This means that objects will be rounded up to the nearest power of 2. (A more
 efficient algorithm would use smaller blcoks above a certain size and round to
 something like 4K.)
 
 When memory is allocated it is first requested from the appropriate queue. If a
 free memory block is available in the queue then that is used for the requested
 memory and returned to the caller. When memory is freed, the memory block is simply
 placed back onto the appropriate queue.
 
 This means that if you preallocate pools that are large enough then both allocation
 and deallocation are thread safe.
 
 @note This algorithm NEVER actaully deallocates the memory from the operating system
 until this object is destroyed (usually as you app is closed down).
 
 To use this allocate one in your application set up code making sure that this happens
 before any other Plonk/Plank objects:
 
 In your class members or a global variable:
 @code
 ScopedPointerContainer<ObjectMemoryBase> memoryManager;
 @endcode
 
 In the set-up code:
 @code
 memoryManager = new ObjectMemoryPools (Memory::global());
 @endcode
 
 Then after all objects have been deleted (in your application tear down):
 @code
 memoryManager = nullptr;
 @endcode
 
 @warning Only use one ObjectMemoryBase subclass for the entire lifetime of your application!
 */
class ObjectMemoryPools :   public ObjectMemoryBase,
                            public Threading::Thread
{
public:   
    
    enum Constants
    {
        NumQueues = 64
    };
    
    class Element : public PlonkBase
    {
    public:
        Element() : ptr (0) { }
        Element (void* p) : ptr (p) { }

        void* ptr;
    };
            
    ObjectMemoryPools (Memory& memory) throw();
    ~ObjectMemoryPools();
    
    PLONK_INLINE_LOW void init() throw() { start(); }
    ResultCode run() throw();
    
    static void* staticAlloc (void* userData, PlankUL size);
    static void staticFree (void* userData, void* ptr);
    
    void* allocateBytes (PlankUL size);
    void free (void* ptr);
    
private:
    LockFreeQueue<Element>* queues;    
};

#endif // PLONK_OBJECTMEMORYPOOLS_H