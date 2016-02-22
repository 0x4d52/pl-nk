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

#include "plonk_StandardHeader.h"

#if PLONK_DEBUG // PLONK_USE_DEFAULT_GLOBAL_NEW_DELETE
// not many cpp files to put these in
// used mainly to help see all allocs/deallocs in debugging
// must be at global scope
//void* operator new (size_t size) throw (std::bad_alloc) 
//{
//    void *ptr = pl_MemoryDefaultAllocateBytes (0, size);
//    
//    if (ptr == 0)
//        throw std::bad_alloc();
//    
//    return ptr;    
//}
//
//void* operator new (size_t size, const std::nothrow_t&) throw() 
//{
//    return pl_MemoryDefaultAllocateBytes (0, size);    
//}
//
//void operator delete (void* ptr) throw()
//{
//    pl_MemoryDefaultFree (0, ptr);
//}
//
//void* operator new[] (size_t size) throw (std::bad_alloc) 
//{
//    void *ptr = pl_MemoryDefaultAllocateBytes (0, size);
//    
//    if (ptr == 0)
//        throw std::bad_alloc();
//    
//    return ptr;    
//}
//
//void* operator new[] (size_t size, const std::nothrow_t&) throw() 
//{
//    return pl_MemoryDefaultAllocateBytes (0, size);    
//}
//
//void operator delete[] (void* ptr) throw()
//{
//    pl_MemoryDefaultFree (0, ptr);
//}
#endif // PLONK_DEBUG==1

//------------------------------------------------------------------------------

BEGIN_PLONK_NAMESPACE

#include "plonk_Headers.h"


//=========================== SmartPointer =====================================

#if PLONK_SMARTPOINTER_DEBUG
static AtomicLong& getTotalSmartPointersAtom() throw()
{
    static AtomicLong totalSmartPointers;
    return totalSmartPointers;
}

static AtomicLong& getTotalSmartPointerCountersAtom() throw()
{
    static AtomicLong totalSmartPointerCounters;
    return totalSmartPointerCounters;
}

const Long getTotalSmartPointers() throw()
{
    return getTotalSmartPointersAtom().getValue();
}

const Long getTotalSmartPointerCounters() throw()
{
    return getTotalSmartPointerCountersAtom().getValue();
}
#endif


void* PlonkBase::operator new (size_t size)
{
    void *ptr = Memory::global().allocateBytes (size);
    
#if !PLONK_ANDROID
    if (ptr == 0)
        throw std::bad_alloc();
#endif
    
    return ptr;
}

void PlonkBase::operator delete (void* ptr)
{
    Memory::global().free (ptr); 
}

void* PlonkBase::operator new[] (size_t size)
{
    void *ptr = Memory::global().allocateBytes (size);
    
#if !PLONK_ANDROID
    if (ptr == 0)
        throw std::bad_alloc();
#endif
    
    return ptr;
}

void PlonkBase::operator delete[] (void* ptr)
{
    Memory::global().free (ptr); 
}

SmartPointer::SmartPointer (const bool allocateWeakPointer) throw()
:	counter (0), weakPointer (0)
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
    ++getTotalSmartPointersAtom();
#if PLONK_SMARTPOINTER_DEBUGLOG
	printf("++SmartPointer++ %p, %ld\n", this, totalSmartPointers.getValueUnchecked());    
#endif
#endif
}

SmartPointer::~SmartPointer()
{
#if PLONK_SMARTPOINTER_DEBUG
    --getTotalSmartPointersAtom();
#if PLONK_SMARTPOINTER_DEBUGLOG
	printf("--SmartPointer-- %p, %ld\n", this, totalSmartPointers.getValueUnchecked());    
#endif
#endif
    
    // actually OK now as the weak ref would be nulled on the when the refcount hit zero
    // another thread getting the weak ref must have intervened and inc'd it
	//plonk_assert (counter->getRefCount() == 0); 
    
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
:   atom (smartPointer, 0)
{
    
#if PLONK_SMARTPOINTER_DEBUG
    ++getTotalSmartPointerCountersAtom();
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf("++SmartPointerCounter++ %p, %ld\n", this, totalSmartPointerCounters.getValueUnchecked());    
#endif
#endif
}

SmartPointerCounter::~SmartPointerCounter()
{
#if PLONK_SMARTPOINTER_DEBUG
    --getTotalSmartPointerCountersAtom();
#if PLONK_SMARTPOINTER_DEBUGLOG
    printf("--SmartPointerCounter-- %p, %ld\n", this, totalSmartPointerCounters.getValueUnchecked());    
#endif
#endif
}

SmartPointer* SmartPointerCounter::getSmartPointer() const throw()
{
    return atom.getPtr();
}

void SmartPointerCounter::incrementRefCount() throw()
{
    SmartPointer *ptr;
    UnsignedLong oldExtra, refCount;
    bool success;
    
    do
    {
        ptr      = atom.getPtrUnchecked();
        oldExtra = atom.getExtraUnchecked();
        refCount = PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) + 1;
        
        plonk_assert ((refCount & PLANK_ATOMIC_XREFCOUNTMAX) != 0); // overflow occurred
        
        success = atom.compareAndSwap (ptr, oldExtra,
                                       ptr, PLANK_SHAREDPTR_XREFCOUNT(oldExtra, refCount));
    } while (! success);
}

void SmartPointerCounter::decrementRefCount() throw()
{    
    SmartPointer *oldPtr, *newPtr;
    UnsignedLong oldExtra, refCount;
    bool success;
    
    do
    {
        oldPtr   = atom.getPtrUnchecked();
        oldExtra = atom.getExtraUnchecked();
        refCount = PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) - 1;
        newPtr   = refCount == 0 ? static_cast<SmartPointer*> (0) : oldPtr;
        
        success = atom.compareAndSwap (oldPtr, oldExtra,
                                       newPtr, PLANK_SHAREDPTR_XREFCOUNT(oldExtra, refCount));
    } while (! success);
    
    if (refCount == 0)
    {
        delete oldPtr; // might be zero in multithreaded if we inc'd and dec'd again but that's OK
        
        if (PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) == 0)
            delete this;
    }
}

int SmartPointerCounter::getRefCount() const throw()
{
    return PLANK_SHAREDPTR_XGETREFCOUNT(atom.getExtraUnchecked());
}

void SmartPointerCounter::incrementWeakCount() throw()
{
    SmartPointer *ptr;
    UnsignedLong oldExtra, weakCount;
    bool success;
    
    do
    {
        ptr       = atom.getPtrUnchecked();
        oldExtra  = atom.getExtraUnchecked();
        weakCount = PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) + 1;
        
        plonk_assert ((weakCount & PLANK_ATOMIC_XWEAKCOUNTMAX) != 0); // overflow occurred
        
        success = atom.compareAndSwap (ptr, oldExtra,
                                       ptr, PLANK_SHAREDPTR_XWEAKCOUNT(oldExtra, weakCount));
    } while (! success);
}

void SmartPointerCounter::decrementWeakCount() throw()
{    
    SmartPointer *ptr;
    UnsignedLong oldExtra, weakCount;
    bool success;
    
    do
    {
        ptr       = atom.getPtrUnchecked();
        oldExtra  = atom.getExtraUnchecked();
        weakCount = PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) - 1;
        
        success = atom.compareAndSwap (ptr, oldExtra,
                                       ptr, PLANK_SHAREDPTR_XWEAKCOUNT(oldExtra, weakCount));
    } while (! success);
    
    if ((weakCount == 0) && (PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) == 0))
        delete this;
}

int SmartPointerCounter::getWeakCount() const throw()
{
    return PLANK_SHAREDPTR_XGETWEAKCOUNT(atom.getExtraUnchecked());
}

void SmartPointerCounter::incrementCounts() throw()
{
    SmartPointer *ptr;
    UnsignedLong oldExtra, refCount, weakCount;
    bool success;
    
    do
    {
        ptr       = atom.getPtrUnchecked();
        oldExtra  = atom.getExtraUnchecked();

        refCount  = PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) + 1;
        weakCount = PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) + 1;

        plonk_assert ((refCount & PLANK_ATOMIC_XREFCOUNTMAX) != 0); // overflow occurred
        plonk_assert ((weakCount & PLANK_ATOMIC_XWEAKCOUNTMAX) != 0); // overflow occurred

        success = atom.compareAndSwap (ptr, oldExtra,
                                       ptr, PLANK_SHAREDPTR_XCOUNTS(refCount, weakCount));
    } while (! success);
}

void SmartPointerCounter::decrementCounts() throw()
{    
    SmartPointer *oldPtr, *newPtr;
    UnsignedLong oldExtra, refCount, weakCount;
    bool success;
    
    do
    {
        oldPtr    = atom.getPtrUnchecked();
        oldExtra  = atom.getExtraUnchecked();

        refCount  = PLANK_SHAREDPTR_XGETREFCOUNT(oldExtra) - 1;
        weakCount = PLANK_SHAREDPTR_XGETWEAKCOUNT(oldExtra) - 1;
        
        newPtr    = refCount == 0 ? static_cast<SmartPointer*> (0) : oldPtr;
        
        success = atom.compareAndSwap (oldPtr, oldExtra,
                                       newPtr, PLANK_SHAREDPTR_XCOUNTS(refCount, weakCount));
    } while (! success);
    
    if (refCount == 0)
    {
        delete oldPtr; // might be zero in multithreaded if we inc'd and dec'd again but that's OK
        
        if (weakCount == 0)
            delete this;
    }
}


END_PLONK_NAMESPACE
