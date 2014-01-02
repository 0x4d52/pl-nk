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

#ifndef PLONK_LOCKFREESTACK_H
#define PLONK_LOCKFREESTACK_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"

template<class ValueType>                                               
class LockFreeStackInternal : public SmartPointer
{
public:
    typedef LockFreeStack<ValueType>        StackType;

    LockFreeStackInternal() throw()
    {
        initStack (liveStack);
        initStack (deadStack);
    }
    
    ~LockFreeStackInternal()
    {
        deInitStack (liveStack);
        deInitStack (deadStack);
    }
    
    inline void push (ValueType const& value) throw()
    {
        PlankLockFreeStackElementRef element = createElement (value);
        ResultCode result = pl_LockFreeStack_Push (&liveStack, element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline ValueType pop() throw()
    {
        ValueType value;
        ValueType* valuePtr = popInternal (&value);
        return (valuePtr == 0) ? getNullValue() : value;
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
        ResultCode result = pl_LockFreeStack_Clear (&deadStack);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }

    void clearAll() throw()
    {
        ResultCode result = pl_LockFreeStack_Clear (&liveStack);
        plonk_assert (result == PlankResult_OK);
        
        clearCache();
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline LongLong length() throw()
    {
        return pl_LockFreeStack_GetSize (&liveStack);
    }
    
    friend class LockFreeStack<ValueType>;
    
private:
    PLONK_ALIGN(16) PlankLockFreeStack liveStack;
    PLONK_ALIGN(16) PlankLockFreeStack deadStack;
        
    static void initStack (PlankLockFreeStack& stack) throw()
    {
        pl_LockFreeStack_Init (&stack);
        pl_LockFreeStack_SetFreeElementDataFunction (&stack, LockFreeStackInternal::freeElement);
    }
    
    static void deInitStack (PlankLockFreeStack& stack) throw()
    {
        ResultCode result;
        
        result = pl_LockFreeStack_Clear (&stack);
        plonk_assert (result == PlankResult_OK);
        
        result = pl_LockFreeStack_DeInit (&stack);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline PlankLockFreeStackElementRef createElement (ValueType const& value) throw()
    {
        PlankLockFreeStackElementRef element;
        ResultCode result = pl_LockFreeStack_Pop (&deadStack, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            ValueType* const data = static_cast<ValueType*> (pl_LockFreeStackElement_GetData (element));
            plonk_assert (data != 0);
            *data = value;
        }
        else 
        {
            element = pl_LockFreeStackElement_CreateAndInit();
            plonk_assert (element != 0);
            pl_LockFreeStackElement_SetData (element, new ValueType (value));
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
        
        PlankLockFreeStackElementRef element;
        ResultCode result = pl_LockFreeStack_Pop (&liveStack, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            valuePtr = static_cast <ValueType*> (pl_LockFreeStackElement_GetData (element));
            plonk_assert (valuePtr != 0);
                        
            if (value != 0)
                *value = *valuePtr;
            
            *valuePtr = getNullValue();
            
            result = pl_LockFreeStack_Push (&deadStack, element);
            plonk_assert (result == PlankResult_OK);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return valuePtr;
    }

};


template<class ValueType>                                               
class LockFreeStackInternal<ValueType*> : public SmartPointer
{
public:
    typedef LockFreeStack<ValueType*>        StackType;
    
    LockFreeStackInternal() throw()
    {
        pl_LockFreeStack_Init (&stack);
    }
    
    ~LockFreeStackInternal()
    {
        pl_LockFreeStack_DeInit (&stack);
    }
    
    void push (ValueType* const value) throw()
    {
        PlankLockFreeStackElementRef element = pl_LockFreeStackElement_CreateAndInit();
        plonk_assert (element != 0);
        pl_LockFreeStackElement_SetData (element, value);
        ResultCode result = pl_LockFreeStack_Push (&stack, element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ValueType* pop() throw()
    {
        ValueType* returnValue;
        
        PlankLockFreeStackElementRef element;
        ResultCode result = pl_LockFreeStack_Pop (&stack, &element);
        plonk_assert (result == PlankResult_OK);
        
        if (element != 0)
        {
            returnValue = static_cast <ValueType*> (pl_LockFreeStackElement_GetData (element));                        
            result = pl_LockFreeStackElement_Destroy (element);
            plonk_assert (result == PlankResult_OK);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return returnValue;
    }
    
    void clear() throw()
    {
        ResultCode result = pl_LockFreeStack_Clear (&stack);
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
    
    LongLong length() throw()
    {
        return pl_LockFreeStack_GetSize (&stack);
    }
    
    friend class LockFreeStack<ValueType*>;
    
private:
    PLONK_ALIGN(16) PlankLockFreeStack stack;
};



//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType>                                               
class LockFreeStack : public SmartPointerContainer<LockFreeStackInternal<ValueType> >
{
public:
    typedef LockFreeStackInternal<ValueType>    Internal;
    typedef SmartPointerContainer<Internal>     Base;
    typedef WeakPointerContainer<LockFreeStack> Weak;    

    inline LockFreeStack()
    :   Base (new Internal())
    {
    }
    
    inline explicit LockFreeStack (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static LockFreeStack fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Copy constructor. */
    inline LockFreeStack (LockFreeStack const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    inline LockFreeStack (Dynamic const& other) throw()
    :   Base (other.as<LockFreeStack>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    inline LockFreeStack& operator= (LockFreeStack const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    inline void push (ValueType const& value) throw()
    {
        this->getInternal()->push (value);
    }
    
    inline ValueType pop() throw()
    {
        return this->getInternal()->pop();
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

    inline LongLong length() throw()
    {
        return this->getInternal()->length();
    }
    
    PLONK_OBJECTARROWOPERATOR(LockFreeStack);
};


#endif // PLONK_LOCKFREESTACK_H
