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

#ifndef PLANK_SIMPLELINKEDLIST_H
#define PLANK_SIMPLELINKEDLIST_H

#include "plank_SimpleLinkedListElement.h"

PLANK_BEGIN_C_LINKAGE

typedef struct PlankSimpleLinkedList* PlankSimpleLinkedListRef; 

typedef PlankResult (*PlankSimpleLinkedListFreeElementDataFunction)(PlankP);


PlankSimpleLinkedListRef pl_SimpleLinkedList_CreateAndInit();
PlankSimpleLinkedListRef pl_SimpleLinkedList_Create();
PlankResult pl_SimpleLinkedList_Init (PlankSimpleLinkedListRef p);
PlankResult pl_SimpleLinkedList_DeInit (PlankSimpleLinkedListRef p);
PlankResult pl_SimpleLinkedList_Destroy (PlankSimpleLinkedListRef p);
PlankResult pl_SimpleLinkedList_Clear (PlankSimpleLinkedListRef p);
PlankResult pl_SimpleLinkedList_SetFreeElementDataFunction (PlankSimpleLinkedListRef p, PlankSimpleLinkedListFreeElementDataFunction freeFunction);
PlankResult pl_SimpleLinkedList_Add (PlankSimpleLinkedListRef p, const PlankSimpleLinkedListElementRef element);
PlankResult pl_SimpleLinkedList_InsertAtIndex (PlankSimpleLinkedListRef p, const PlankLL index, const PlankSimpleLinkedListElementRef element);
PlankResult pl_SimpleLinkedList_AtIndex (PlankSimpleLinkedListRef p, const PlankLL index, PlankSimpleLinkedListElementRef* element);
PlankResult pl_SimpleLinkedList_GetFirst (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element);
PlankResult pl_SimpleLinkedList_GetLast (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element);
PlankResult pl_SimpleLinkedList_RemoveAtIndex (PlankSimpleLinkedListRef p, const PlankLL index, PlankSimpleLinkedListElementRef* element);
PlankResult pl_SimpleLinkedList_RemoveFirst (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element);
PlankResult pl_SimpleLinkedList_RemoveLast (PlankSimpleLinkedListRef p, PlankSimpleLinkedListElementRef* element);
PlankResult pl_SimpleLinkedList_IndexOf (PlankSimpleLinkedListRef p, const PlankSimpleLinkedListElementRef element, PlankLL* index);
PlankLL     pl_SimpleLinkedList_GetSize (PlankSimpleLinkedListRef p);

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankSimpleLinkedList
{
	PlankSimpleLinkedListElementRef first;
    PlankSimpleLinkedListFreeElementDataFunction freeFunction;
} PlankSimpleLinkedList;
#endif

#endif // PLANK_SIMPLELINKEDLIST_H