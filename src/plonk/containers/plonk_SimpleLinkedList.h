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

#ifndef PLONK_SIMPLELINKEDLIST_H
#define PLONK_SIMPLELINKEDLIST_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"

template<class ValueType>                                               
class SimpleLinkedListInternal : public SmartPointer
{
public:
    typedef SimpleLinkedList<ValueType>        LinkedListType;

    SimpleLinkedListInternal() throw()
    {
        pl_SimpleLinkedList_Init (&list);
        pl_SimpleLinkedList_SetFreeElementDataFunction (&list, SimpleLinkedListInternal::freeElement);
    }
    
    ~SimpleLinkedListInternal()
    {
        ResultCode result;
        
        result = pl_SimpleLinkedList_Clear (&list);
        plonk_assert (result == PlankResult_OK);

        result = pl_SimpleLinkedList_DeInit (&list);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void add (ValueType const& value) throw()
    {
        ResultCode result = pl_SimpleLinkedList_Add (&list, createElement (value));
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void insertAtIndex (const LongLong index, ValueType const& value) throw()
    {
        ResultCode result = pl_SimpleLinkedList_InsertAtIndex (&list, index, createElement (value));
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ValueType atIndex (const LongLong index) throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_AtIndex (&list, index, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return getValueFromElement (element);
    }
    
    ValueType getFirst() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_GetFirst (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return getValueFromElement (element);
    }
    
    ValueType getLast() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_GetLast (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return getValueFromElement (element);
    }
    
    ValueType removeAtIndex (const LongLong index) throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_RemoveAtIndex (&list, index, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return removeValueFromElement (element);
    }
    
    ValueType removeFirst() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_RemoveFirst (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return removeValueFromElement (element);
    }

    ValueType removeLast() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_RemoveLast (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return removeValueFromElement (element);
    }
    
    LongLong indexOf (ValueType const& value) throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_GetFirst (&list, &element);
        plonk_assert (result == PlankResult_OK);
        
        LongLong index = 0;
        
        while (element != 0)
        {
            if (value == getValueFromElement (element))
                return index;
            
            ++index;
            element = pl_SimpleLinkedListElement_GetNext (element);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return -1;
    }
    
    inline LongLong size() throw()
    {
        return pl_SimpleLinkedList_GetSize (&list);
    }
    
    friend class SimpleLinkedList<ValueType>;
    
private:
    PlankSimpleLinkedList list;
    
    inline PlankSimpleLinkedListElementRef createElement (ValueType const& value) throw()
    {
        PlankSimpleLinkedListElementRef element = pl_SimpleLinkedListElement_CreateAndInit();
        plonk_assert (element != 0);
        pl_SimpleLinkedListElement_SetData (element, new ValueType (value));
        return element;
    }
    
    inline ValueType getValueFromElement (PlankSimpleLinkedListElementRef element) throw()
    {
        if (element == 0)
            return ValueType();
        
        ValueType* const value = static_cast<ValueType*> (pl_SimpleLinkedListElement_GetData (element));
        
        if (value != 0)
            return *value;
        else
            return ValueType();
    }
    
    inline ValueType removeValueFromElement (PlankSimpleLinkedListElementRef element) throw()
    {
        if (element == 0)
            return ValueType();
        
        ValueType* const value = static_cast<ValueType*> (pl_SimpleLinkedListElement_GetData (element));
        
        ResultCode result = pl_SimpleLinkedListElement_Destroy (element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        if (value != 0)
        {
            ValueType result = *value;
            delete value;
            return result;
        }
        else
        {
            return ValueType();
        }
    }

    static ResultCode freeElement (Pointer data)
    {
        delete static_cast<ValueType*> (data);
        return PlankResult_OK;
    }
};


template<class ValueType>                                               
class SimpleLinkedListInternal<ValueType*> : public SmartPointer
{
public:
    typedef SimpleLinkedList<ValueType*>        LinkedListType;
    
    SimpleLinkedListInternal() throw()
    {
        pl_SimpleLinkedList_Init (&list);
    }
    
    ~SimpleLinkedListInternal()
    {
        plonk_assert (pl_SimpleLinkedList_GetSize (&list) == 0); // pointers were left in the list
        pl_SimpleLinkedList_Clear (&list);
        pl_SimpleLinkedList_DeInit (&list);        
    }
        
    void add (ValueType* const& value) throw()
    {
        ResultCode result = pl_SimpleLinkedList_Add (&list, createElement (value));
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    void insertAtIndex (const LongLong index, ValueType* const& value) throw()
    {
        ResultCode result = pl_SimpleLinkedList_InsertAtIndex (&list, index, createElement (value));
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ValueType* atIndex (const LongLong index) throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_AtIndex (&list, index, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return getValueFromElement (element);
    }
    
    ValueType* getFirst() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_GetFirst (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return getValueFromElement (element);
    }
    
    ValueType* getLast() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_GetLast (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return getValueFromElement (element);
    }
    
    ValueType* removeAtIndex (const LongLong index) throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_RemoveAtIndex (&list, index, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return removeValueFromElement (element);
    }
    
    ValueType* removeFirst() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_RemoveFirst (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return removeValueFromElement (element);
    }
    
    ValueType* removeLast() throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_RemoveLast (&list, &element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return removeValueFromElement (element);
    }
    
    LongLong indexOf (ValueType* const& value) throw()
    {
        PlankSimpleLinkedListElementRef element;
        ResultCode result = pl_SimpleLinkedList_GetFirst (&list, &element);
        plonk_assert (result == PlankResult_OK);
        
        LongLong index = 0;
        
        while (element != 0)
        {
            if (value == getValueFromElement (element))
                return index;
            
            ++index;
            element = pl_SimpleLinkedListElement_GetNext (element);
        }
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
        return -1;
    }
    
    inline LongLong size() throw()
    {
        return pl_SimpleLinkedList_GetSize (&list);
    }
        
    friend class SimpleLinkedList<ValueType*>;
    
private:
    PlankSimpleLinkedList list;    
    
    PlankSimpleLinkedListElementRef createElement (ValueType* const& value) throw()
    {
        PlankSimpleLinkedListElementRef element = pl_SimpleLinkedListElement_CreateAndInit();
        plonk_assert (element != 0);
        pl_SimpleLinkedListElement_SetData (element, value);
        return element;
    }
    
    inline ValueType* getValueFromElement (PlankSimpleLinkedListElementRef element) throw()
    {
        if (element == 0)
            return 0;
        
        return static_cast<ValueType*> (pl_SimpleLinkedListElement_GetData (element));        
    }
    
    inline ValueType removeValueFromElement (PlankSimpleLinkedListElementRef element) throw()
    {
        if (element == 0)
            return ValueType();
        
        ValueType* const value = static_cast<ValueType*> (pl_SimpleLinkedListElement_GetData (element));
        
        ResultCode result = pl_SimpleLinkedListElement_Destroy (element);
        plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return value;
    }
};



//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class ValueType>                                               
class SimpleLinkedList : public SmartPointerContainer<SimpleLinkedListInternal<ValueType> >
{
public:
    typedef SimpleLinkedListInternal<ValueType>     Internal;
    typedef SmartPointerContainer<Internal>         Base;
    typedef WeakPointerContainer<SimpleLinkedList>  Weak;    

    SimpleLinkedList()
    :   Base (new Internal())
    {
    }
    
    explicit SimpleLinkedList (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	} 
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static SimpleLinkedList fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    SimpleLinkedList (SimpleLinkedList const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    SimpleLinkedList (Dynamic const& other) throw()
    :   Base (other.as<SimpleLinkedList>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    SimpleLinkedList& operator= (SimpleLinkedList const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    inline void add (ValueType const& value) throw()
    {
        this->getInternal()->add (value);
    }
    
    inline void insertAtIndex (const LongLong index, ValueType const& value) throw()
    {
        this->getInternal()->insertAtIndex (index, value);
    }

    inline ValueType atIndex (const LongLong index) throw()
    {
        return this->getInternal()->atIndex (index);
    }
    
    inline ValueType getFirst() throw()
    {
        return this->getInternal()->getFirst();
    }
    
    inline ValueType getLast() throw()
    {
        return this->getInternal()->getFirst();
    }
    
    inline ValueType removeAtIndex (const LongLong index) throw()
    {
        return this->getInternal()->removeAtIndex (index);
    }
    
    inline ValueType removeFirst() throw()
    {
        return this->getInternal()->removeFirst();
    }
    
    inline ValueType removeLast() throw()
    {
        return this->getInternal()->removeFirst();
    }
    
    inline LongLong indexOf (ValueType const& value) throw()
    {
        return this->getInternal()->indexOf (value);
    }
    
    inline bool contains (ValueType const& value) throw()
    {
        return this->getInternal()->indexOf (value) >= 0;
    }
    
    inline LongLong size() throw()
    {
        return this->getInternal()->size();
    }
    
    PLONK_OBJECTARROWOPERATOR(SimpleLinkedList)

};


#endif // PLONK_SIMPLELINKEDLIST_H
