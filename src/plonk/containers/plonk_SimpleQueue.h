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

#ifndef PLONK_SIMPLEQUEUE_H
#define PLONK_SIMPLEQUEUE_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"

template<class ValueType>                                               
class SimpleQueueInternal : public SmartPointer
{
public:
    typedef SimpleQueue<ValueType>        StackType;

    SimpleQueueInternal() throw()
    {
        pl_SimpleQueue_Init (&queue);
        pl_SimpleQueue_SetFreeElementDataFunction (&queue, SimpleQueueInternal::freeElement);
    }
    
    ~SimpleQueueInternal()
    {
        ResultCode result;
        
        result = pl_SimpleQueue_Clear (&queue);
        plonk_assert (result == PlankResult_OK);

        result = pl_SimpleQueue_DeInit (&queue);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void push (ValueType const& value) throw()
    {
        PlankSimpleQueueElementRef element = pl_SimpleQueueElement_CreateAndInit();
        plonk_assert (element != 0);
        pl_SimpleQueueElement_SetData (element, new ValueType (value));
        ResultCode result = pl_SimpleQueue_Push (&queue, element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ValueType pop() throw()
    {
        ValueType returnValue;
        
        PlankSimpleQueueElementRef element;
        ResultCode result = pl_SimpleQueue_Pop (&queue, &element);
        plonk_assert (result == PlankResult_OK);

        if (element != 0)
        {
            ValueType* valuePtr = static_cast <ValueType*> (pl_SimpleQueueElement_GetData (element));
            
            if (valuePtr != 0)
            {
                returnValue = *valuePtr;
                delete valuePtr;
            }
            
            result = pl_SimpleQueueElement_Destroy (element);
            plonk_assert (result == PlankResult_OK);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return returnValue;
    }
    
    friend class SimpleQueue<ValueType>;
    
private:
    PlankSimpleQueue queue;
    
    static ResultCode freeElement (Pointer data)
    {
        delete static_cast<ValueType*> (data);
        return PlankResult_OK;
    }
};


template<class ValueType>                                               
class SimpleQueueInternal<ValueType*> : public SmartPointer
{
public:
    typedef SimpleQueue<ValueType*>        StackType;
    
    SimpleQueueInternal() throw()
    {
        pl_SimpleQueue_Init (&queue);
    }
    
    ~SimpleQueueInternal()
    {
        plonk_assert (pl_SimpleQueue_GetSize (&queue) == 0); // pointers were left in the queue
        pl_SimpleQueue_Clear (&queue);
        pl_SimpleQueue_DeInit (&queue);        
    }
        
    void push (ValueType* const value) throw()
    {
        PlankSimpleQueueElementRef element = pl_SimpleQueueElement_CreateAndInit();
        plonk_assert (element != 0);
        pl_SimpleQueueElement_SetData (element, value);
        ResultCode result = pl_SimpleQueue_Push (&queue, element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ValueType* pop() throw()
    {
        ValueType* returnValue;
        
        PlankSimpleQueueElementRef element;
        ResultCode result = pl_SimpleQueue_Pop (&queue, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            returnValue = static_cast <ValueType*> (pl_SimpleQueueElement_GetData (element));                        
            result = pl_SimpleQueueElement_Destroy (element);
            plonk_assert (result == PlankResult_OK);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return returnValue;
    }
    
    friend class SimpleQueue<ValueType*>;
    
private:
    PlankSimpleQueue queue;
};



//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType>                                               
class SimpleQueue : public SmartPointerContainer<SimpleQueueInternal<ValueType> >
{
public:
    typedef SimpleQueueInternal<ValueType>    Internal;
    typedef SmartPointerContainer<Internal>   Base;
    typedef WeakPointerContainer<SimpleQueue> Weak;    

    SimpleQueue()
    :   Base (new Internal())
    {
    }
    
    explicit SimpleQueue (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static SimpleQueue fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    SimpleQueue (SimpleQueue const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    SimpleQueue (Dynamic const& other) throw()
    :   Base (other.as<SimpleQueue>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    SimpleQueue& operator= (SimpleQueue const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    void push (ValueType const& value) throw()
    {
        this->getInternal()->push (value);
    }
    
    ValueType pop () throw()
    {
        return this->getInternal()->pop();
    }
};


#endif // PLONK_SIMPLEQUEUE_H
