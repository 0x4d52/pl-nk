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

#ifndef PLONK_SIMPLEARRAY_H
#define PLONK_SIMPLEARRAY_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "plonk_SmartPointerContainer.h"
#include "plonk_ObjectArrayInternal.h"
//#include "plonk_DynamicContainer.h"

template<class ObjectType>
class SimpleArray : public SmartPointerContainer< ObjectArrayInternal<ObjectType,SmartPointer> >
{
public:
    typedef ObjectArrayInternal<ObjectType,SmartPointer>    Internal;
    typedef SmartPointerContainer<Internal>                 Base;
    
    SimpleArray()
    :	SmartPointerContainer<Internal> (new Internal (0, false))
    {
    }
    
    explicit SimpleArray (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	} 
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    SimpleArray (SimpleArray const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
//    SimpleArray (Dynamic const& other) throw()
//    :   Base (other.as<SimpleArray>().getInternal())
//    {
//    }    
//    
    /** Assignment operator. */
    SimpleArray& operator= (SimpleArray const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    PLONK_OBJECTARROWOPERATOR(SimpleArray)

};

//class Globals
//{
//public:
//    static void add (const SmartPointer* p) throw()
//    {
//        Globals* g = Globals::get (false);
//        
//        if (g->globals.getInternal()->indexOf (const_cast<SmartPointer*> (p)) < 0); // no duplicates
//            g->globals.getInternal()->add (const_cast<SmartPointer*> (p));
//    }
//    
//    static void destroy() throw()
//    {
//        Globals* g = Globals::get (true); // returns and resets so we must delete it here
//        
//        SmartPointer** array = g->globals.getInternal()->getArray();
//        const int numItems = g->globals.getInternal()->length();
//        
//        for (int i = numItems; --i >= 0;)
//            array[i]->decrementRefCount();
//        
//        delete g;
//    }
//    
//private:
//    Globals() throw()
//    {
//    }
//    
//    static Globals* get (bool reset) throw()
//    {
//        static Globals* g = new Globals();
//        Globals* temp = g;
//        
//        plonk_assert (g != 0); // exit has already happened once
//        
//        if (reset)
//            g = 0;
//        
//        return temp;
//    }
//    
//    SimpleArray<SmartPointer*> globals;
//};



#endif // PLONK_SIMPLEARRAY_H
