/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
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

#ifndef PLANK_THREAD_H
#define PLANK_THREAD_H

#include "../containers/atomic/plank_Atomic.h"

PLANK_BEGIN_C_LINKAGE

/** A crossplatform thread implementation.
  
 [example tba]
 
 @defgroup PlankThreadClass Plank Thread class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %Thread</i> object. */
typedef struct PlankThread* PlankThreadRef; 

/** Thread ID. */

#if PLANK_WIN
typedef PlankUI PlankThreadID;
#else
typedef PlankUL PlankThreadID;
#endif

/** The Thread run function.
 This should have the declaration:
 @code
 PlankResult myThreadFunction (PlankThreadRef p);
 @endcode 
 
 Your will be passed a reference to the Thread calling your function. 
 In there you should check if your thread should exit regularly. You 
 should return the result PlankResult_OK. E.g.,
 
 @code
 PlankResult myThreadFunction (PlankThreadRef p)
 {
    while (! pl_Thread_GetShouldExit (p))
    {
        //.. do some stuff ..
    }
 
    return PlankResult_OK;
 }
 @endcode 
 
 In another Thread you can call pl_Thread_SetShouldExit() to tell your
 Thread to exit gracefully. E.g.
 
 @code
 ...
 pl_Thread_SetShouldExit (myThreadRef, PLANK_TRUE);
 ...
 @endcode
 
 */
typedef PlankResult (*PlankThreadFunction)(PlankThreadRef);

/** Sleeps the calling thread.
 @param seconds The duration to sleep in seconds.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_ThreadSleep (PlankD seconds);

/** Yield the calling thread.
 This signals to the operating system that other threads could have some
 scheduler time.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_ThreadYield();

/** Get the thread ID of the calling thread. 
 @return The thread's ID. */
PlankThreadID pl_ThreadCurrentID();

/** Create and initialise a <i>Plank %Thread</i> object and return an oqaque reference to it.
 @return A <i>Plank %Thread</i> object as an opaque reference or PLANK_NULL. */
PlankThreadRef pl_Thread_CreateAndInit();

/** Create a <i>Plank %Thread</i> object and return an oqaque reference to it.
 @return A <i>Plank %Thread</i> object as an opaque reference or PLANK_NULL. */
PlankThreadRef pl_Thread_Create();

/** Initialise a <i>Plank %Thread</i> object. 
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_Init (PlankThreadRef p);

/** Deinitialise a <i>Plank %Thread</i> object. 
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_DeInit (PlankThreadRef p);

/** Destroy a <i>Plank %Thread</i> object. 
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_Destroy (PlankThreadRef p);

/** Get the thread ID. 
 @param p The <i>Plank %Thread</i> object. 
 @return The thread's ID. */
PlankThreadID pl_Thread_GetID (PlankThreadRef p);

/** Set the name of the thread.
 Useful for debugging.
 This must be called before pl_Thread_Start().
 Not implemented on all platforms. */
PlankResult pl_Thread_SetName (PlankThreadRef p, const char* name);

/** Set the thread run function.
 This must be called before pl_Thread_Start().
 @param p The <i>Plank %Thread</i> object. 
 @param function The function to use as the %Thread run function.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_SetFunction (PlankThreadRef p, PlankThreadFunction function);

/** Set a user data pointer.
 This allows the %Thread to access data via the pointer using 
 pl_Thread_GetUserData() while it is running.
 This must be called before pl_Thread_Start().
 @param p The <i>Plank %Thread</i> object. 
 @param userData The user data pointer to store.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_SetUserData (PlankThreadRef p, PlankP userData);

/** Get the user data pointer.
 This allows the %Thread to access data while it is running.
 @param p The <i>Plank %Thread</i> object. 
 @return The user data pointer. */
PlankP pl_Thread_GetUserData (PlankThreadRef p);

/** Starts the %Thread.
 The calls the function set using pl_Thread_SetFunction().
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_Start (PlankThreadRef p);

/** Cancels the %Thread.
 Ideally you should provide a method of checking the return value of
 pl_Thread_GetShouldExit() regularly within your %Thread run function and
 return from the function if it signals that the thread should exit.
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_Cancel (PlankThreadRef p);

/** Wait for the %Thread.
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_Wait (PlankThreadRef p);

PlankResult pl_Thread_Pause (PlankThreadRef p);
PlankResult pl_Thread_PauseWithTimeout (PlankThreadRef p, double duration);
PlankResult pl_Thread_Resume (PlankThreadRef p);


/** Determines if the %Thread is still running.
 @param p The <i>Plank %Thread</i> object. 
 @return @c true if the %Thread is still running, otherwise @c false. */
PlankB pl_Thread_IsRunning (PlankThreadRef p);

/** Signals that the %Thread should exit.
 This relies on you checking the return value of pl_Thread_GetShouldExit() 
 regularly within your %Thread run function and return from the function if 
 it signals that the thread should exit.
 @param p The <i>Plank %Thread</i> object. 
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_Thread_SetShouldExit (PlankThreadRef p);

/** Checks if the %Thread has been asked to exit.
 This relies on you checking the return value of this function 
 regularly within your %Thread run function and return from the function if 
 it signals that the thread should exit.
 @param p The <i>Plank %Thread</i> object. 
 @return @c true if the %Thread should exit, otherwise @c false. */
PlankB pl_Thread_GetShouldExit (PlankThreadRef p);

PlankResult pl_Thread_SetPriority (PlankThreadRef p, int priority);
PlankResult pl_Thread_SetPriorityAudio (PlankThreadRef p, int blockSize, double sampleRate);
PlankResult pl_Thread_SetAffinity (PlankThreadRef p, int affinity);

/** @} */

PLANK_END_C_LINKAGE

#if !DOXYGEN
#if PLANK_APPLE || PLANK_LINUX
typedef pthread_t PlankThreadNativeHandle;
typedef void* PlankThreadNativeReturn;
#define PLANK_THREADCALL
#endif // PLANK_APPLE

#if PLANK_ANDROID
typedef pthread_t PlankThreadNativeHandle;
typedef void* PlankThreadNativeReturn;
#define PLANK_THREADCALL
#endif // PLANK_ANDROID

#if PLANK_WIN
typedef uintptr_t PlankThreadNativeHandle;
typedef unsigned PlankThreadNativeReturn;
#define PLANK_THREADCALL __stdcall
#endif // PLANK_WINDOWS

#define PLANK_THREAD_MAXNAMELENGTH 256

typedef struct PlankThread
{
    // could pack thread, shouldExitAtom and isRunningAtom into a PX
    PlankThreadNativeHandle thread;
    PlankThreadID threadID;
    PlankThreadFunction function;
    PLANK_ALIGN(PLANK_WIDESIZE) PlankAtomicPX userDataAtom;
    PLANK_ALIGN(4) PlankAtomicI shouldExitAtom;
    PLANK_ALIGN(4) PlankAtomicI isRunningAtom;
    PLANK_ALIGN(4) PlankAtomicI paused;
    char name[PLANK_THREAD_MAXNAMELENGTH];
    int priority;
    int affinity;
} PlankThread;
#endif


#endif // PLANK_THREAD_H
