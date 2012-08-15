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

#include "plonk_StandardHeader.h"

#if PLONK_DEBUG
// not many cpp files to put these in
// used mainly to help see all allocs/deallocs in debugging
// must be at global scope
void* operator new (size_t size) throw (std::bad_alloc) 
{
    void *ptr = pl_MemoryDefaultAllocateBytes (0, size);
    
    if (ptr == 0)
        throw std::bad_alloc();
    
    return ptr;    
}

void* operator new (size_t size, const std::nothrow_t&) throw() 
{
    return pl_MemoryDefaultAllocateBytes (0, size);    
}

void operator delete (void* ptr) throw()
{
    pl_MemoryDefaultFree (0, ptr);
}

void* operator new[] (size_t size) throw (std::bad_alloc) 
{
    void *ptr = pl_MemoryDefaultAllocateBytes (0, size);
    
    if (ptr == 0)
        throw std::bad_alloc();
    
    return ptr;    
}

void* operator new[] (size_t size, const std::nothrow_t&) throw() 
{
    return pl_MemoryDefaultAllocateBytes (0, size);    
}

void operator delete[] (void* ptr) throw()
{
    pl_MemoryDefaultFree (0, ptr);
}
#endif // PLONK_DEBUG==1

//------------------------------------------------------------------------------

BEGIN_PLONK_NAMESPACE

#include "plonk_Headers.h"

#ifndef PLONK_SMARTPOINTER_DEBUG
  #ifdef PLONK_DEBUG
    #define PLONK_SMARTPOINTER_DEBUGLOG 1
    #define PLONK_SMARTPOINTER_DEBUG 1
  #else
    #define PLONK_SMARTPOINTER_DEBUGLOG 0
    #define PLONK_SMARTPOINTER_DEBUG 0
  #endif
#endif


//=========================== SmartPointer =====================================

#if PLONK_SMARTPOINTER_DEBUG
AtomicLong totalSmartPointers;
AtomicLong totalSmartPointerCounters;
#endif


void* PlonkBase::operator new (size_t size)
{
    void *ptr = Memory::global().allocateBytes (size);
    
    if (ptr == 0)
        throw std::bad_alloc();
            
    return ptr;    
}

void PlonkBase::operator delete (void* ptr)
{
    Memory::global().free (ptr); 
}

void* PlonkBase::operator new[] (size_t size)
{
    void *ptr = Memory::global().allocateBytes (size);
    
    if (ptr == 0)
        throw std::bad_alloc();
    
    return ptr;    
}

void PlonkBase::operator delete[] (void* ptr)
{
    Memory::global().free (ptr); 
}

SmartPointer::SmartPointer (const bool allocateWeakPointer) throw()
:	counter (0),
    weakPointer (0)
{		
    counter = new SmartPointerCounter (this);
    
	if (allocateWeakPointer)
    {
		weakPointer = new WeakPointer (counter);
        WeakPointer* weak = static_cast<WeakPointer*> (weakPointer.getPtrUnchecked());
        weak->incrementRefCount();
        weak->incrementWeakCount();
    }
    
#if PLONK_SMARTPOINTER_DEBUG
    ++totalSmartPointers;
#if PLONK_SMARTPOINTER_DEBUGLOG
	printf("++SmartPointer++ %p, %ld\n", this, totalSmartPointers.getValueUnchecked());    
#endif
#endif
}

SmartPointer::~SmartPointer()
{
#if PLONK_SMARTPOINTER_DEBUG
    --totalSmartPointers;
#if PLONK_SMARTPOINTER_DEBUGLOG
	printf("--SmartPointer-- %p, %ld\n", this, totalSmartPointers.getValueUnchecked());    
#endif
#endif
    
	plonk_assert (counter->getRefCount() == 0);
    
    if (weakPointer != 0)
    {
        WeakPointer* weak = static_cast<WeakPointer*> (weakPointer.getPtrUnchecked());
        weak->decrementWeakCount();
        weak->decrementRefCount();
        weakPointer = 0;
    }
    
    delete counter; // <-- will be done by SmartPointerCounter itself when refCount and weakCount both hit zero
}

void SmartPointer::incrementRefCount()  throw()
{	
    counter->incrementRefCount();
}

void SmartPointer::decrementRefCount()  throw()
{ 
    plonk_assert (counter->getRefCount() > 0);
    counter->decrementRefCount(); 
}

void* SmartPointer::getWeak() const throw()               
{ 
    return weakPointer.getPtrUnchecked(); 
}

int SmartPointer::getRefCount() const throw()            
{ 
    return counter->getRefCount(); 
}

///-----------------------------------------------------------------------------

SmartPointerCounter::SmartPointerCounter (SmartPointer* smartPointerToUse) throw()
:   smartPointer (smartPointerToUse),
    refCount (0),
    weakCount (0)
{
#if PLONK_SMARTPOINTER_DEBUG
    ++totalSmartPointerCounters;
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf("++SmartPointerCounter++ %p, %ld\n", this, totalSmartPointerCounters.getValueUnchecked());    
#endif
#endif
}

SmartPointerCounter::~SmartPointerCounter()
{
#if PLONK_SMARTPOINTER_DEBUG
    --totalSmartPointerCounters;
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf("--SmartPointerCounter-- %p, %ld\n", this, totalSmartPointerCounters.getValueUnchecked());    
#endif
#endif
}

SmartPointer* SmartPointerCounter::getSmartPointer() const throw()
{
    return smartPointer.getPtrUnchecked();
}

void SmartPointerCounter::incrementRefCount() throw()
{
    //const int count = 
    ++refCount;  
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf ("+R SmartPointerCounter %p refCount=%d weakCount=%d\n", 
            this, 
            refCount.getValueUnchecked(), 
            weakCount.getValueUnchecked());
#endif
//    return count;
}

void SmartPointerCounter::decrementRefCount() throw()
{
    //const int count = 
    ;  
    
    // need to do the refCount dec and clear of weak atomically if it hits zero!!
    // and if both refCount and weakCount hits zero we can delete this SmartPointerCounter
    
    if (--refCount == 0)
    {
        SmartPointer* expired = smartPointer.getValueUnchecked();
        smartPointer.setPtr (0);
        delete expired;
    }
    
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf ("-R SmartPointerCounter %p refCount=%d weakCount=%d\n", 
            this, 
            refCount.getValueUnchecked(), 
            weakCount.getValueUnchecked());
#endif    
//    return count;
}

int SmartPointerCounter::getRefCount() const throw()
{
    return refCount.getValueUnchecked();
}

void SmartPointerCounter::incrementWeakCount() throw()
{
    //const int count = 
    ++weakCount;  
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf ("+W SmartPointerCounter %p refCount=%d weakCount=%d\n", 
            this, 
            refCount.getValueUnchecked(), 
            weakCount.getValueUnchecked());
#endif 
//    return count;
}

void SmartPointerCounter::decrementWeakCount() throw()
{
    //const int count = 
    --weakCount;  
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf ("-W SmartPointerCounter %p refCount=%d weakCount=%d\n", 
            this, 
            refCount.getValueUnchecked(), 
            weakCount.getValueUnchecked());
#endif    
//    return count;
}

int SmartPointerCounter::getWeakCount() const throw()
{
    return weakCount.getValueUnchecked();
}

void SmartPointerCounter::incrementCounts() throw()
{
    // can ultimately be atomic
    incrementRefCount();
    incrementWeakCount();
}

void SmartPointerCounter::decrementCounts() throw()
{
    // can ultimately be atomic
    decrementWeakCount();
    decrementRefCount();
}

END_PLONK_NAMESPACE
