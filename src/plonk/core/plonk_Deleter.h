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

#ifndef PLONK_DELETER_H
#define PLONK_DELETER_H

class SmartPointer;

/** A SmartPointer deleter. 
 
 A Deleter is used to delete SmartPointer objects rather than 
 calling delete directly. This default Deleter does simply call delete but other hosts
 could use another thread to delete objects so this is not done in an audio callback
 thread (for example).
 
 So instead of this:
 @code
 delete mySmartPointerObj;
 @endcode
 
 ..do this:
 @code
 Deleter::getDeleter()->deleteInternal(mySmartPointerObj);
 @endcode
  
 @see getDeleter(), setDeleter() */
class Deleter
{
public:
	Deleter() throw();
	virtual ~Deleter() throw();
	virtual void deleteInternal (SmartPointer* internalToDelete) throw();
	virtual void flush() throw() { };
};

/** A Deleter which doesn't actually delete anything.
 
 This Deleter just leaks memory by ignoring requests to delete anything. This is
 probably pretty useless but could be used to test the effect of a real memory leak
 over a long period. Initially it was used to isolate a problem where objects being deleted
 were causing a crash. This class was used so that the objects weren't actually deleted so
 the problem (which is now fixed) could be put to one side.
 
 Obviously use this with caution! */
class LeakingDeleter : public Deleter
{
public:
	void deleteInternal (SmartPointer* internalToDelete) throw() { (void)internalToDelete; } // just leak!
};


#endif // PLONK_DELETER_H