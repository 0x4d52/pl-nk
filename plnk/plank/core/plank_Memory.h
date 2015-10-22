/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLANK_MEMORY_H
#define PLANK_MEMORY_H

PLANK_BEGIN_C_LINKAGE


typedef struct PlankMemory* PlankMemoryRef; 

typedef void* (*PlankMemoryAllocateBytesFunction)(PlankP, PlankUL);
typedef void (*PlankMemoryFreeFunction)(PlankP, void*);

void* pl_MemoryDefaultAllocateBytes (PlankP p, PlankUL size);
void pl_MemoryDefaultFree (PlankP p, void* ptr);

static PlankResult pl_MemoryZero (PlankP ptr, const PlankUL numBytes);
static PlankResult pl_MemoryCopy (PlankP dst, PlankConstantP src, const PlankUL numBytes);

PlankMemoryRef pl_MemoryGlobal();

PlankMemoryRef pl_Memory_CreateAndInit();
PlankMemoryRef pl_Memory_Create();
PlankResult pl_Memory_Init (PlankMemoryRef p);
PlankResult pl_Memory_DeInit (PlankMemoryRef p);
PlankResult pl_Memory_Destroy (PlankMemoryRef p);

/** Set a user data pointer.
 This allows the %Memory to access data via the pointer using 
 pl_Memory_GetUserData().
 @param p The <i>Plank %Memory</i> object. 
 @param userData The user data pointer to store.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Memory_SetUserData (PlankMemoryRef p, PlankP userData);

/** Get the user data pointer.
 This allows the %Memory to access data while it is running.
 @param p The <i>Plank %Memory</i> object. 
 @return The user data pointer. */
PlankP pl_Memory_GetUserData (PlankMemoryRef p);

PlankResult pl_Memory_SetFunctions (PlankMemoryRef p, 
                                    PlankMemoryAllocateBytesFunction allocateBytesFunction, 
                                    PlankMemoryFreeFunction freeFunction);
static PlankP pl_Memory_AllocateBytes (PlankMemoryRef p, PlankUL numBytes);
static PlankResult pl_Memory_Free (PlankMemoryRef p, PlankP ptr);


PLANK_END_C_LINKAGE

#define PLANK_INLINING_FUNCTIONS 1
#include "plank_MemoryInline.h"
#undef PLANK_INLINING_FUNCTIONS

#endif // PLANK_MEMORY_H
