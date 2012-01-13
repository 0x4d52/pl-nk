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

#ifndef PLANK_SIMPLELINKEDLISTELEMENT_H
#define PLANK_SIMPLELINKEDLISTELEMENT_H

PLANK_BEGIN_C_LINKAGE

typedef struct PlankSimpleLinkedListElement*   PlankSimpleLinkedListElementRef;
typedef PlankSimpleLinkedListElementRef PlankSimpleQueueElementRef;
typedef PlankSimpleLinkedListElementRef PlankSimpleStackElementRef;

PlankSimpleLinkedListElementRef pl_SimpleLinkedListElement_CreateAndInit();
PlankSimpleLinkedListElementRef pl_SimpleLinkedListElement_Create();
PlankResult pl_SimpleLinkedListElement_Init (PlankSimpleLinkedListElementRef p);
PlankResult pl_SimpleLinkedListElement_DeInit (PlankSimpleLinkedListElementRef p);
PlankResult pl_SimpleLinkedListElement_Destroy (PlankSimpleLinkedListElementRef p);
PlankP pl_SimpleLinkedListElement_GetData (PlankSimpleLinkedListElementRef p);
void pl_SimpleLinkedListElement_SetData (PlankSimpleLinkedListElementRef p, const PlankP data);
PlankSimpleLinkedListElementRef pl_SimpleLinkedListElement_GetNext (PlankSimpleLinkedListElementRef p);
void pl_SimpleLinkedListElement_SetNext (PlankSimpleLinkedListElementRef p, const PlankSimpleLinkedListElementRef next);

PLANK_END_C_LINKAGE

#define PLANK_INLINING_FUNCTIONS 1
#include "plank_SimpleLinkedListElementInline.h"
#undef PLANK_INLINING_FUNCTIONS

// make these inline at some point..

#define pl_SimpleQueueElement_CreateAndInit  pl_SimpleLinkedListElement_CreateAndInit
#define pl_SimpleQueueElement_Create  pl_SimpleLinkedListElement_Create
#define pl_SimpleQueueElement_Init  pl_SimpleLinkedListElement_Init
#define pl_SimpleQueueElement_DeInit  pl_SimpleLinkedListElement_DeInit
#define pl_SimpleQueueElement_Destroy pl_SimpleLinkedListElement_Destroy
#define pl_SimpleQueueElement_GetData pl_SimpleLinkedListElement_GetData
#define pl_SimpleQueueElement_SetData pl_SimpleLinkedListElement_SetData
#define pl_SimpleQueueElement_GetNext pl_SimpleLinkedListElement_GetNext
#define pl_SimpleQueueElement_SetNext pl_SimpleLinkedListElement_SetNext

#define pl_SimpleStackElement_CreateAndInit  pl_SimpleLinkedListElement_CreateAndInit
#define pl_SimpleStackElement_Create  pl_SimpleLinkedListElement_Create
#define pl_SimpleStackElement_Init  pl_SimpleLinkedListElement_Init
#define pl_SimpleStackElement_DeInit  pl_SimpleLinkedListElement_DeInit
#define pl_SimpleStackElement_Destroy pl_SimpleLinkedListElement_Destroy
#define pl_SimpleStackElement_GetData pl_SimpleLinkedListElement_GetData
#define pl_SimpleStackElement_SetData pl_SimpleLinkedListElement_SetData
#define pl_SimpleStackElement_GetNext pl_SimpleLinkedListElement_GetNext
#define pl_SimpleStackElement_SetNext pl_SimpleLinkedListElement_SetNext

#endif // PLANK_SIMPLELINKEDLISTELEMENT_H
