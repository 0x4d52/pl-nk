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

#ifndef PLANK_THREADLOCALSTORAGE_H
#define PLANK_THREADLOCALSTORAGE_H

#define PLANKTHREADLOCALSTORAGE_MAXIMUMIDENTIFIERS 64
#define PLANKTHREADLOCALSTORAGE_DEFAULTNUMBYTES 4

PLANK_BEGIN_C_LINKAGE

typedef struct PlankThreadLocalStorage* PlankThreadLocalStorageRef; 

PlankThreadLocalStorageRef pl_ThreadLocalStorage_CreateAndInit();
PlankThreadLocalStorageRef pl_ThreadLocalStorage_Create();
PlankResult pl_ThreadLocalStorage_Init (PlankThreadLocalStorageRef p);
PlankResult pl_ThreadLocalStorage_InitWithNumBytes (PlankThreadLocalStorageRef p, const PlankL numBytes);
PlankResult pl_ThreadLocalStorage_DeInit (PlankThreadLocalStorageRef p);
PlankResult pl_ThreadLocalStorage_Destroy (PlankThreadLocalStorageRef p);
PlankUL pl_ThreadLocalStorage_GetNumBytes (PlankThreadLocalStorageRef p);
PlankP pl_ThreadLocalStorage_GetData (PlankThreadLocalStorageRef p);

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankThreadLocalStorage
{
    PlankUL numBytes;
    PlankUL identifier;
} PlankThreadLocalStorage;
#endif

#endif // PLANK_THREADLOCALSTORAGE_H