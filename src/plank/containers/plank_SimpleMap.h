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

#ifndef PLANK_SIMPLEMAP_H
#define PLANK_SIMPLEMAP_H

#include "plank_SimpleLinkedList.h"

PLANK_BEGIN_C_LINKAGE

typedef struct PlankSimpleMap* PlankSimpleMapRef; 

typedef PlankResult (*PlankSimpleMapFreeElementDataFunction)(PlankP);


PlankSimpleMapRef pl_SimpleMap_CreateAndInit();
PlankSimpleMapRef pl_SimpleMap_Create();
PlankResult pl_SimpleMap_Init (PlankSimpleMapRef p);
PlankResult pl_SimpleMap_DeInit (PlankSimpleMapRef p);
PlankResult pl_SimpleMap_Destroy (PlankSimpleMapRef p);
PlankResult pl_SimpleMap_Clear (PlankSimpleMapRef p);
PlankResult pl_SimpleMap_SetFreeElementDataFunction (PlankSimpleMapRef p, PlankSimpleMapFreeElementDataFunction freeFunction);
PlankResult pl_SimpleMap_ContainsKey (PlankSimpleMapRef p, const PlankLL key, PlankB* flag);
PlankResult pl_SimpleMap_SetKey (PlankSimpleMapRef p, const PlankLL key, PlankP data);
PlankResult pl_SimpleMap_GetKey (PlankSimpleMapRef p, const PlankLL key, PlankP* data);
PlankResult pl_SimpleMap_RemoveKey (PlankSimpleMapRef p, const PlankLL key, PlankP* data);
PlankLL     pl_SimpleMap_GetSize (PlankSimpleMapRef p);

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankSimpleMap
{
	PlankSimpleLinkedList list;
    PlankSimpleMapFreeElementDataFunction freeFunction;
} PlankSimpleMap;
#endif

#endif // PLANK_SIMPLEMAP_H