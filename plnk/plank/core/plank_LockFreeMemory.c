/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

// unlikely to implement now... other more straightforward solutions used...

//#include "plank_StandardHeader.h"
//#include "plank_LockFreeMemory.h"
//
//PlankLockFreeMemoryRef pl_LockFreeMemory_CreateAndInit()
//{
//    PlankLockFreeMemoryRef p;
//    p = pl_LockFreeMemory_Create();
//    
//    if (p != PLANK_NULL)
//    {
//        if (pl_LockFreeMemory_Init (p) != PlankResult_OK)
//            pl_LockFreeMemory_Destroy (p);
//        else
//            return p;
//    }
//    
//    return PLANK_NULL;  
//}
//
//PlankLockFreeMemoryRef pl_LockFreeMemory_Create()
//{
//    PlankMemoryRef m;
//    PlankLockFreeMemoryRef p;
//    
//    m = pl_MemoryGlobal(); // OK, creation of this isn't itself necessarily lock free
//    p = (PlankLockFreeMemoryRef)pl_Memory_AllocateBytes (m, sizeof (PlankLockFreeMemory));
//    
//    if (p != PLANK_NULL)
//        pl_MemoryZero (p, sizeof (PlankLockFreeMemory));
//    
//    return p;
//}
//
//PlankResult pl_LockFreeMemory_Init (PlankLockFreeMemoryRef p)
//{
//    PlankResult result = PlankResult_OK;
//    int i, size;
//        
//    for (i = 0, size = 8; i < PLANKLOCKFREEMEMORY_NUMSIZECLASSES; ++i, size += 8)
//    {
//        PlankLockFreeMemorySizeClassRef sizeClass = &p->sizeClasses[i];
//        pl_LockFreeStack_Init (&sizeClass->partial);
//        sizeClass->size = size;
//        sizeClass->superBlockSize = PLANKLOCKFREEMEMORY_SUPERBLOCKSIZE;
//    }
//
//    return result;
//}
//
//PlankResult pl_LockFreeMemory_DeInit (PlankLockFreeMemoryRef p)
//{
//    PlankResult result = PlankResult_OK;
//    int i;
//
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }    
//    
//    for (i = 0; i < PLANKLOCKFREEMEMORY_NUMSIZECLASSES; ++i)
//    {
//        PlankLockFreeMemorySizeClassRef sizeClass = &p->sizeClasses[i];
//        pl_LockFreeStack_DeInit (&sizeClass->partial);
//    }
//
//exit:
//    return result;
//}
//
//PlankResult pl_LockFreeMemory_Destroy (PlankLockFreeMemoryRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m = pl_MemoryGlobal();
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    if ((result = pl_LockFreeMemory_DeInit (p)) != PlankResult_OK)
//        goto exit;
//    
//    result = pl_Memory_Free (m, p);   
//    
//exit:
//    return result;        
//}
//
//void* pl_LockFreeMemory_AllocateBytes (PlankLockFreeMemoryRef p, PlankUL numBytes)
//{
//	(void)p;
//	(void)numBytes;
//    return PLANK_NULL;
//}
//
//PlankResult pl_LockFreeMemory_Free (PlankLockFreeMemoryRef p, void* ptr)
//{
//	(void)p;
//	(void)ptr;
//    return PlankResult_UnknownError;
//}
