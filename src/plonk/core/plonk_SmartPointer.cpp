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

#define PLONK_SMARTPOINTER_DEBUG 0

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
        WeakPointer* weak = new WeakPointer (counter);
        weak->incrementRefCount();  // for the WeakPointer object
        weak->incrementWeakCount(); // for the weak count for this object
        this->weakPointer = weak;
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
    
    // this is a good place to do this
    if (weakPointer != 0)
    {
        WeakPointer* weak = static_cast<WeakPointer*> (weakPointer.getPtrUnchecked());
        weak->decrementWeakCount(); // for the weak count for this object
        weak->decrementRefCount();  // for the WeakPointer object
        weakPointer = 0;
    }    
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

SmartPointerCounter::SmartPointerCounter (SmartPointer* smartPointer) throw()
{
    plonk_staticassert (sizeof (Parts) == (PLONK_WORDSIZE*2))
    plonk_staticassert (sizeof (Halves) == (PLONK_WORDSIZE*2))
    plonk_staticassert (sizeof (Element) == (PLONK_WORDSIZE*2))

    // no need to set atomically in the constructor
    atom.getAtomicRef()->ptr = smartPointer;
    atom.getAtomicRef()->extra = 0;
        
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
    Element e;
    e.halves.ptr = atom.getPtrUnchecked();
    return e.parts.smartPointer;
}

void SmartPointerCounter::incrementRefCount() throw()
{
    Element oldElement, newElement;
    bool success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = atom.getPtrUnchecked();
        oldElement.halves.extra = newElement.halves.extra = atom.getExtraUnchecked();
        ++newElement.parts.counts.refCount;
        
        plonk_assert (newElement.parts.counts.refCount != 0); // overflow occurred
        
        success = atom.compareAndSwap (oldElement.halves.ptr, oldElement.halves.extra, 
                                       newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
}

void SmartPointerCounter::decrementRefCount() throw()
{    
    Element oldElement, newElement;
    bool success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = atom.getPtrUnchecked();
        oldElement.halves.extra = newElement.halves.extra = atom.getExtraUnchecked();
        
        if (--newElement.parts.counts.refCount == 0)
            newElement.parts.smartPointer = 0;
        
        success = atom.compareAndSwap (oldElement.halves.ptr, oldElement.halves.extra, 
                                       newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if (newElement.parts.counts.refCount == 0)
    {
        delete oldElement.parts.smartPointer;
        
        if (newElement.parts.counts.weakCount == 0)
            delete this;
    }
}

int SmartPointerCounter::getRefCount() const throw()
{
    Element e;
    e.halves.extra = atom.getExtraUnchecked();
    return e.parts.counts.refCount;
}

void SmartPointerCounter::incrementWeakCount() throw()
{
    Element oldElement, newElement;
    bool success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = atom.getPtrUnchecked();
        oldElement.halves.extra = newElement.halves.extra = atom.getExtraUnchecked();
        ++newElement.parts.counts.weakCount;
        
        plonk_assert (newElement.parts.counts.weakCount != 0); // overflow occurred
        
        success = atom.compareAndSwap (oldElement.halves.ptr, oldElement.halves.extra, 
                                       newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
}

void SmartPointerCounter::decrementWeakCount() throw()
{    
    Element oldElement, newElement;
    bool success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = atom.getPtrUnchecked();
        oldElement.halves.extra = newElement.halves.extra = atom.getExtraUnchecked();
        --newElement.parts.counts.weakCount;
        
        success = atom.compareAndSwap (oldElement.halves.ptr, oldElement.halves.extra, 
                                       newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if ((newElement.parts.counts.refCount == 0) && (newElement.parts.counts.weakCount == 0))
        delete this;
}

int SmartPointerCounter::getWeakCount() const throw()
{
    Element e;
    e.halves.extra = atom.getExtraUnchecked();
    return e.parts.counts.weakCount;
}

void SmartPointerCounter::incrementCounts() throw()
{
    Element oldElement, newElement;
    bool success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = atom.getPtrUnchecked();
        oldElement.halves.extra = newElement.halves.extra = atom.getExtraUnchecked();
        ++newElement.parts.counts.refCount;
        ++newElement.parts.counts.weakCount;
        
        plonk_assert (newElement.parts.counts.refCount != 0); // overflow occurred
        plonk_assert (newElement.parts.counts.weakCount != 0); // overflow occurred

        success = atom.compareAndSwap (oldElement.halves.ptr, oldElement.halves.extra, 
                                       newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
}

void SmartPointerCounter::decrementCounts() throw()
{    
    Element oldElement, newElement;
    bool success;
    
    do
    {
        oldElement.halves.ptr = newElement.halves.ptr = atom.getPtrUnchecked();
        oldElement.halves.extra = newElement.halves.extra = atom.getExtraUnchecked();
        
        if (--newElement.parts.counts.refCount == 0)
            newElement.parts.smartPointer = 0;
        
        --newElement.parts.counts.weakCount;
        
        success = atom.compareAndSwap (oldElement.halves.ptr, oldElement.halves.extra, 
                                       newElement.halves.ptr, newElement.halves.extra);
    } while (!success);
    
    if (newElement.parts.counts.refCount == 0)
    {
        delete oldElement.parts.smartPointer;
        
        if (newElement.parts.counts.weakCount == 0)
            delete this;
    }
}


END_PLONK_NAMESPACE
