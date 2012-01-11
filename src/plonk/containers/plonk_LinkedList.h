/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
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
 -------------------------------------------------------------------------------
 */

#ifndef PLONK_LINKEDLIST_H
#define PLONK_LINKEDLIST_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"


//------------------------------------------------------------------------------


template<class ValueType>                                               
class LinkedListElementInternal : public SmartPointer
{
public:
    typedef LinkedListElement<ValueType> Element;
      
    LinkedListElementInternal (ValueType const& initialValue) throw()
    :   value (initialValue)
    {
    }

    friend class LinkedListElement<ValueType>;
    
private:
    ValueType value;
    Element next;
};


//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType>                                               
class LinkedListElement : public SmartPointerContainer<LinkedListElementInternal<ValueType> >
{
public:
    typedef LinkedListElementInternal<ValueType>    Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef LinkedListElement<ValueType>            Element;

    LinkedListElement() throw()
    :   Internal (0)
    {
    }
    
    LinkedListElement (ValueType const& initialValue) throw()
    :   Internal (new Internal (initialValue))
    {
    }
    
    
};


//------------------------------------------------------------------------------


template<class ValueType>                                               
class LinkedListInternal : public SmartPointer
{
public:
    typedef LinkedListElement<ValueType>    Element;
    typedef LinkedList<ValueType>           ListType;

    LinkedListInternal() throw()
    {
    }
    
    friend class LinkedList<ValueType>;
    
private:
    Element head;
    Element tail;
};


//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType>                                               
class LinkedList : public SmartPointerContainer<LinkedListInternal<ValueType> >
{
public:
    typedef LinkedListInternal<ValueType>           Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef LinkedListElement<ValueType>            Element;
    typedef WeakPointerContainer<LinkedList>        Weak;    

    LinkedList()
    :   Internal (new Internal())
    {
    }
    
    explicit LinkedList (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	} 
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static LinkedList fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    LinkedList (LinkedList const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    LinkedList (Dynamic const& other) throw()
    :   Base (other.as<LinkedList>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    LinkedList& operator= (LinkedList const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    bool isEmpty() const throw()
    {
        Internal* const internal = this->getInternal();
        
        if (internal->head.isNull())
        {
            plonk_assert (internal->tail.isNull());
            return true;
        }
        
        return false;
    }
};


#endif // PLONK_LINKEDLIST_H
