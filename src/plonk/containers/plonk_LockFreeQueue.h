/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_LOCKFREEQUEUE_H
#define PLONK_LOCKFREEQUEUE_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"

template<class ValueType>                                               
class LockFreeQueueInternal : public SmartPointer
{
public:
    typedef LockFreeQueue<ValueType>        QueueType;
    
    LockFreeQueueInternal() throw()
    {
        initQueue (liveQueue);
        initQueue (deadQueue);
    }
    
    ~LockFreeQueueInternal()
    {
        deInitQueue (liveQueue);
        deInitQueue (deadQueue);
    }
    
    inline void push (ValueType const& value) throw()
    {
        PlankLockFreeQueueElementRef element = createElement (value);
        ResultCode result = pl_LockFreeQueue_Push (&liveQueue, element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline ValueType pop() throw()
    {
        ValueType value (getNullValue());
        ValueType* valuePtr = popInternal (&value);
        return (valuePtr == 0) ? getNullValue() : value;
    }
    
    template<class OtherType>
    inline bool pop (OtherType& value)
    {
        ValueType tmp (getNullValue());
        ValueType* valuePtr = popInternal (&tmp);
 
        if (valuePtr != 0)
        {
            value = tmp;
            return true;
        }
        else return false;
    }

    void clear() throw()
    {
        ValueType* valuePtr;
        do 
        {
            valuePtr = popInternal (0);
        } while (valuePtr != 0);
    }
    
    void clearCache() throw()
    {
        ResultCode result = pl_LockFreeQueue_Clear (&deadQueue);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void clearAll() throw()
    {
        ResultCode result = pl_LockFreeQueue_Clear (&liveQueue);
        plonk_assert (result == PlankResult_OK);
        
        clearCache();
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline int length() throw()
    {
        return pl_LockFreeQueue_GetSize (&liveQueue);
    }
    
    friend class LockFreeQueue<ValueType>;
    
private:
    PLONK_ALIGN(16) PlankLockFreeQueue liveQueue;
    PLONK_ALIGN(16) PlankLockFreeQueue deadQueue;
    
    static void initQueue (PlankLockFreeQueue& queue) throw()
    {
        pl_LockFreeQueue_Init (&queue);
        pl_LockFreeQueue_SetFreeElementDataFunction (&queue, LockFreeQueueInternal::freeElement);
    }
    
    static void deInitQueue (PlankLockFreeQueue& queue) throw()
    {
        ResultCode result;
        
        result = pl_LockFreeQueue_Clear (&queue);
        plonk_assert (result == PlankResult_OK);
        
        result = pl_LockFreeQueue_DeInit (&queue);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline PlankLockFreeQueueElementRef createElement (ValueType const& value) throw()
    {
        PlankLockFreeQueueElementRef element;
        ResultCode result = pl_LockFreeQueue_Pop (&deadQueue, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            ValueType* const data = static_cast<ValueType*> (pl_LockFreeQueueElement_GetData (element));
            plonk_assert (data != 0);
            *data = value;
        }
        else 
        {
            element = pl_LockFreeQueueElement_CreateAndInit();
            plonk_assert (element != 0);
            pl_LockFreeQueueElement_SetData (element, new ValueType (value));
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return element;
    }
    
    static ResultCode freeElement (Pointer data)
    {
        delete static_cast<ValueType*> (data);
        return PlankResult_OK;
    }
    
    static inline ValueType getNullValue() throw()
    {
        static ValueType null = ValueType();
        return null;
    }
    
    ValueType* popInternal (ValueType* value) throw()
    {
        ValueType* valuePtr = 0;
        
        PlankLockFreeQueueElementRef element;
        ResultCode result = pl_LockFreeQueue_Pop (&liveQueue, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            valuePtr = static_cast <ValueType*> (pl_LockFreeQueueElement_GetData (element));
            plonk_assert (valuePtr != 0);
            
            if (value != 0)
                *value = *valuePtr;
            
            *valuePtr = getNullValue();
            
            result = pl_LockFreeQueue_Push (&deadQueue, element);
            plonk_assert (result == PlankResult_OK);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return valuePtr;
    }
    
};



template<class ValueType>                                               
class LockFreeQueueInternal<ValueType*> : public SmartPointer
{
public:
    typedef LockFreeQueue<ValueType*>        QueueType;
    
    LockFreeQueueInternal() throw()
    {
        pl_LockFreeQueue_Init (&queue);
    }
    
    ~LockFreeQueueInternal()
    {
        pl_LockFreeQueue_DeInit (&queue);
    }
    
    static inline ValueType* getNullValue() throw()
    {
        return 0;
    }
    
    void push (ValueType* const value) throw()
    {
        PlankLockFreeQueueElementRef element = pl_LockFreeQueueElement_CreateAndInit();
        plonk_assert (element != 0);
        pl_LockFreeQueueElement_SetData (element, value);
        ResultCode result = pl_LockFreeQueue_Push (&queue, element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ValueType* pop() throw()
    {
        ValueType* returnValue;
        
        PlankLockFreeQueueElementRef element;
        ResultCode result = pl_LockFreeQueue_Pop (&queue, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            returnValue = static_cast <ValueType*> (pl_LockFreeQueueElement_GetData (element));                        
            result = pl_LockFreeQueueElement_Destroy (element);
            plonk_assert (result == PlankResult_OK);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return returnValue;
    }
    
    template<class OtherType>
    inline bool pop (OtherType& value)
    {
        return value = pop() ? true : false;
    }
    
    void clear() throw()
    {
        ResultCode result = pl_LockFreeQueue_Clear (&queue);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void clearCache() throw()
    {
        // not applicable
    }
    
    void clearAll() throw()
    {
        clear();
    }
        
    int length() throw()
    {
        return pl_LockFreeQueue_GetSize (&queue);
    }

    friend class LockFreeQueue<ValueType*>;
    
private:
    PLONK_ALIGN(16) PlankLockFreeQueue queue;
};



//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType>                                               
class LockFreeQueue : public SmartPointerContainer<LockFreeQueueInternal<ValueType> >
{
public:
    typedef LockFreeQueueInternal<ValueType>    Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef WeakPointerContainer<LockFreeQueue> Weak;
    typedef ValueType                           Value;

    inline LockFreeQueue()
    :   Base (new Internal())
    {
    }
    
    inline explicit LockFreeQueue (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static LockFreeQueue fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Copy constructor. */
    inline LockFreeQueue (LockFreeQueue const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    inline LockFreeQueue (Dynamic const& other) throw()
    :   Base (other.as<LockFreeQueue>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    inline LockFreeQueue& operator= (LockFreeQueue const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    static ValueType getNullValue() throw()
    {
        return Internal::getNullValue();
    }
    
    inline void push (ValueType const& value) throw()
    {
        this->getInternal()->push (value);
    }
    
    inline ValueType pop() throw()
    {
        return this->getInternal()->pop();
    }
    
    template<class OtherType>
    inline bool pop (OtherType& value)
    {
        return this->getInternal()->pop (value);
    }
    
    inline void clear() throw()
    {
        return this->getInternal()->clear();
    }
    
    inline void clearCache() throw()
    {
        return this->getInternal()->clearCache();
    }
    
    inline void clearAll() throw()
    {
        return this->getInternal()->clearAll();
    }
    
    void increaseCache (const int count) throw()
    {
        // dirty hack for now..
        
        plonk_assert (count > 0);
        
        for (int i = 0; i < count; ++i)
            push (ValueType());
        
        clear(); // moves items to the dead queue
    }
    
    inline int length() throw()
    {
        return this->getInternal()->length();
    }
    
    PLONK_OBJECTARROWOPERATOR(LockFreeQueue);

};


#endif // PLONK_LOCKFREEQUEUE_H
