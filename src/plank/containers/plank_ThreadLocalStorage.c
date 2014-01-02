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

// difficult to do properly on all versions of windows

//#include "../core/plank_StandardHeader.h"
//#include "plank_ThreadLocalStorage.h"
//#include "plank_Atomic.h"
//
///*
// Windows TLS stuff...
// 
// void __cdecl on_process_enter(void);
// void __cdecl on_process_exit(void);
// void __cdecl on_thread_enter(void);
// void __cdecl on_thread_exit(void);
// 
// http://stackoverflow.com/questions/3802244/hooking-thread-creation-termination
// The best way is to call WaitForSingleObject with the HANDLE of the thread (call OpenThread using the thread id to get the HANDLE).
// - how will one know the thread exists in the first place!
//
// */
//
//// private functions
//void pl_ThreadLocalStorageFree (PlankP ptr);
//PlankUL pl_ThreadLocalStorageCreateIdentifier();
//void pl_ThreadLocalStorageDestroyIdentifier (PlankUL identifier);
//PlankAtomicLRef pl_ThreadLocalStorageGetNumActive();
//PlankResult pl_ThreadLocalStorageIncrementNumActive();
//PlankResult pl_ThreadLocalStorageDecrementNumActive();
//
//void pl_ThreadLocalStorageFree (PlankP ptr)
//{
//    PlankMemoryRef m = pl_MemoryGlobal();
//    pl_Memory_Free (m, ptr);
//}
//
//#if PLANK_APPLE
//PlankUL pl_ThreadLocalStorageCreateIdentifier()
//{
//    pthread_key_t key;
//    pthread_key_create (&key, pl_ThreadLocalStorageFree);
//    return (PlankUL)key;
//}
//
//void pl_ThreadLocalStorageDestroyIdentifier (PlankUL identifier)
//{
//    pthread_key_delete ((pthread_key_t)identifier);
//}
//#endif
//
//#if PLANK_WIN
//PlankUL pl_ThreadLocalStorageCreateIdentifier()
//{
//    return (PlankUL)TlsAlloc(); // FlsAlloc() (W7 onwards) has a free function but not TlsAlloc!!
//}
//
//void pl_ThreadLocalStorageDestroyIdentifier (PlankUL identifier)
//{
//    TlsFree ((DWORD)identifier);
//}
//#endif
//
//PlankAtomicLRef pl_ThreadLocalStorageGetNumActive()
//{
//    static PlankAtomicL counter = { 0 };
//    return &counter;
//}
//
//PlankResult pl_ThreadLocalStorageIncrementNumActive()
//{
//    if (pl_AtomicL_Get (pl_ThreadLocalStorageGetNumActive()) >= PLANKTHREADLOCALSTORAGE_MAXIMUMIDENTIFIERS)
//        return PlankResult_ThreadLocalStorageMaximumIdentifiersReached;
//    
//    pl_AtomicL_Increment (pl_ThreadLocalStorageGetNumActive());
//    return PlankResult_OK;
//}
//
//PlankResult pl_ThreadLocalStorageDecrementNumActive()
//{
//    pl_AtomicL_Decrement (pl_ThreadLocalStorageGetNumActive());
//    return PlankResult_OK;
//}
//
////------------------------------------------------------------------------------
//
//PlankThreadLocalStorageRef pl_ThreadLocalStorage_CreateAndInit()
//{
//    PlankThreadLocalStorageRef p;
//    p = pl_ThreadLocalStorage_Create();
//    
//    if (p != PLANK_NULL)
//    {
//        if (pl_ThreadLocalStorage_Init (p) != PlankResult_OK)
//            pl_ThreadLocalStorage_Destroy (p);
//        else
//            return p;
//    }
//    
//    return PLANK_NULL;
//}
//
//PlankThreadLocalStorageRef pl_ThreadLocalStorage_Create()
//{
//    PlankMemoryRef m;
//    PlankThreadLocalStorageRef p;
//    
//    m = pl_MemoryGlobal(); // OK, creation of the queue isn't itself lock free
//    p = (PlankThreadLocalStorageRef)pl_Memory_AllocateBytes (m, sizeof (PlankThreadLocalStorage));
//    
//    if (p != PLANK_NULL)
//        pl_MemoryZero (p, sizeof (PlankThreadLocalStorage));
//    
//    return p;
//}
//
//PlankResult pl_ThreadLocalStorage_Init (PlankThreadLocalStorageRef p)
//{
//    return pl_ThreadLocalStorage_InitWithNumBytes (p, PLANKTHREADLOCALSTORAGE_DEFAULTNUMBYTES);
//}
//
//PlankResult pl_ThreadLocalStorage_InitWithNumBytes (PlankThreadLocalStorageRef p, const PlankL numBytes)
//{
//    PlankResult result = PlankResult_OK;
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    if ((result = pl_ThreadLocalStorageIncrementNumActive()) != PlankResult_OK)
//        goto exit;
//    
//    p->numBytes = numBytes;
//    p->identifier = pl_ThreadLocalStorageCreateIdentifier();
//            
//exit:
//    return result;    
//}
//
//PlankResult pl_ThreadLocalStorage_DeInit (PlankThreadLocalStorageRef p)
//{
//    PlankResult result = PlankResult_OK;
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//        
//    pl_ThreadLocalStorageDestroyIdentifier (p->identifier);
//    
//    if ((result = pl_ThreadLocalStorageDecrementNumActive()) != PlankResult_OK)
//        goto exit;
//    
//exit:
//    return result;    
//}
//
//PlankResult pl_ThreadLocalStorage_Destroy (PlankThreadLocalStorageRef p)
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
//    if ((result = pl_ThreadLocalStorage_DeInit (p)) != PlankResult_OK)
//        goto exit;
//    
//    result = pl_Memory_Free (m, p);   
//    
//exit:
//    return result;    
//}
//
//PlankUL pl_ThreadLocalStorage_GetNumBytes (PlankThreadLocalStorageRef p)
//{
//    return p->numBytes;
//}
//
//#if PLANK_APPLE
//PlankP pl_ThreadLocalStorage_GetData (PlankThreadLocalStorageRef p)
//{
//    PlankP returnValue = PLANK_NULL;
//    PlankMemoryRef m = pl_MemoryGlobal();
//
//    returnValue = pthread_getspecific ((pthread_key_t)p->identifier);
//    
//    if (returnValue == PLANK_NULL) 
//    {
//        returnValue = pl_Memory_AllocateBytes (m, p->numBytes);
//        
//        if (returnValue == PLANK_NULL)
//            goto exit;
//        
//        pthread_setspecific ((pthread_key_t)p->identifier, returnValue);
//    }
//    
//exit:
//    return returnValue;
//}
//#endif
//
//#if PLANK_WIN    
//PlankP pl_ThreadLocalStorage_GetData (PlankThreadLocalStorageRef p)
//{
//    PlankP returnValue = PLANK_NULL;
//    PlankMemoryRef m = pl_MemoryGlobal();
//
//    returnValue = (PlankP)TlsGetValue ((DWORD)p->identifier);
//    
//    if (returnValue == PLANK_NULL)
//    {
//        returnValue = pl_Memory_AllocateBytes (m, p->numBytes);
//        
//        if (returnValue == PLANK_NULL)
//            goto exit;
//        
//        if (! TlsSetValue ((DWORD)p->identifier, returnValue))
//        {
//            pl_Memory_Free (m, returnValue);
//            returnValue = PLANK_NULL;
//        }
//    }
//    
//exit:
//    return returnValue;
//}
//
//#endif


