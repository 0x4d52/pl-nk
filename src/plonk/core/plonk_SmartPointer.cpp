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

BEGIN_PLONK_NAMESPACE

#include "plonk_Headers.h"

#ifndef DEBUG_SmartPointer
  #ifdef PLONK_DEBUG
    #define DEBUG_SmartPointer 1
  #else
    #define DEBUG_SmartPointer 0
  #endif
#endif

#define DEBUB_SmartPointerLog 1

//=========================== SmartPointer =====================================

#if DEBUG_SmartPointer
class DummyBase 
{
public:
    virtual ~DummyBase() { }
    void update() throw() { }
}; 

class SmartPointerCounter
{
public:
    static SmartPointerCounter& global() throw()
    {
        static SmartPointerCounter object;
        return object;
    }
    
    ~SmartPointerCounter()
    {        
        for (int i = allocations->size(); --i >= 0;)
        {
            SmartPointer* p = allocations->getArray()[i];
            (void)p;
            p = 0; // just an extra line to ease stepping through
        }
        
//        plonk_assert (allocations->size() == 0);
        
        delete allocations;
    }
        
    void add (SmartPointer* p) 
    { 
        allocations->add (p);
    }
    
    void remove (SmartPointer* p) 
    { 
        int index = allocations->indexOf (p);
        
        plonk_assert (index >= 0);
        
        if (index >= 0)
            allocations->remove (index);
    }
    
    int size() const throw() { return allocations->size(); }
        
private:
    SmartPointerCounter() throw()
    :   allocations(new ObjectArrayInternal<SmartPointer*,DummyBase> (0, false))
    {
    }
    
    ObjectArrayInternal<SmartPointer*,DummyBase>* allocations;
};
#endif


SmartPointer::SmartPointer (const bool allocateWeakPointer) throw()
:	refCount (0),
	weakPointer (0),
    active (0)
{		
	if (allocateWeakPointer)
		weakPointer = new WeakPointer (this);

    active = 1;
    
    if (weakPointer != 0)
    {
        // todo: should this be static_cast?
        WeakPointer* weak = reinterpret_cast<WeakPointer*> (weakPointer);
        weak->incrementRefCount();
    }
    
#if DEBUG_SmartPointer
    SmartPointerCounter::global().add (this);
    
    if ((long)this == 0) 
    {
        printf("stop\n");
    }
        
  #if DEBUB_SmartPointerLog
	printf("+++++++, %p, %d\n", this, SmartPointerCounter::global().size());
    
    if (SmartPointerCounter::global().size() == -1)
    {
        printf("stop\n");
    }
    
    
  #endif
#endif
}

SmartPointer::~SmartPointer()
{
#if DEBUG_SmartPointer
    SmartPointerCounter::global().remove (this);
  #if DEBUB_SmartPointerLog
	printf("-------, %p, %d\n", this, SmartPointerCounter::global().size());
  #endif
#endif
    
	plonk_assert (refCount >= 0);
    
    if (weakPointer != 0)
    {
        WeakPointer* weak = reinterpret_cast<WeakPointer*> (weakPointer);
        weak->setWeakPointer (0);
        weak->decrementRefCount();
        weakPointer = 0;
    }
}

void* SmartPointer::operator new (size_t size)
{
    void *ptr = Memory::realTime().allocateBytes (size);
    
    if (ptr == 0)
        throw std::bad_alloc();
    
    return ptr;    
}

void SmartPointer::operator delete (void* ptr)
{
    Memory::realTime().free (ptr); 
}

void SmartPointer::destroy()
{
    plonk_assert (active); // want to see if these checks can be removed...
    
    if (active)
    {
        active = 0;
        //Deleter::getDeleter()->deleteInternal (this);
        delete this; 
    }
}

void SmartPointer::incrementRefCount()  throw()
{	
    plonk_assert (active); // want to see if these checks can be removed...

	if (active) 
    {
        ++refCount;
    }
}

bool SmartPointer::decrementRefCount()  throw()
{ 
    plonk_assert (active); // want to see if these checks can be removed...

	if (active)
	{
		plonk_assert (refCount > 0);

        --refCount;
        
        if (deleteIfOnlyMutualReferencesRemain() == true)
		{
            --refCount;
			plonk_assert (refCount == 0); // once we use a deferred Deleter this may not be the case?
        }
		
        if (refCount <= 0) 
        {
            destroy();
            return true;
        }
	}
    
    return false;
}

END_PLONK_NAMESPACE
