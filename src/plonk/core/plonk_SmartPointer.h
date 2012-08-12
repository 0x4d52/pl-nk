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

#ifndef PLONK_SMARTPOINTER_H
#define PLONK_SMARTPOINTER_H

#include "../containers/plonk_Atomic.h"

class PlonkBase
{
public:
    PlonkBase() throw() { }
    ~PlonkBase() { } // doesn't need to be virtual as we never need to call delete on a PlonkBase pointer
    static void* operator new (size_t size); 
    static void operator delete (void* ptr);
};

/** Reference counted pointer base class.
 One of the most important classes. In fact on the internal side of things this 
 perhaps is the most important class. It handles reference counting of all 
 internal objects allocated using the new operator. The object deletes itself 
 when its reference count reaches zero. This helps manage object lifetimes 
 especially with dynamically allocated audio components. A 'weak' version
 of this pointer can also be obtained which will not affect the reference
 count but will get set to 0 when its SmartPointer peer is deleted.
 @see WeakPointer, SmartPointerContainer
 */
class SmartPointer : public PlonkBase
{
public:
	
	/// @name Construction and destruction
	/// @{
	
	SmartPointer (const bool allocateWeakPointer = true) throw();
    virtual ~SmartPointer(); // MUST be virtual unless PlonkBase gains the need to be virtual
        
	void incrementRefCount() throw();
    bool decrementRefCount() throw(); 
    	
	/// @} <!-- end Construction and destruction -->
	
	/// @name Miscellaneous
	/// @{
	
	Long getRefCount() const throw()	{ return refCount.getValueUnchecked(); }
    void update() throw()               { }
    void* getWeak() const throw()       { return weakPointer; }
    
	/// @} <!-- end Miscellaneous -->
    
protected:
    //virtual void destroy();
    
    AtomicValue<Long> refCount;
    AtomicValue<void*> weakPointer;
	
private:
	SmartPointer (const SmartPointer&);
    const SmartPointer& operator= (const SmartPointer&);
};

#endif // PLONK_SMARTPOINTER_H
