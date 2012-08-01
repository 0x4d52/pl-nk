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

#ifndef PLONK_THREAD_H
#define PLONK_THREAD_H


/** Crossplatform threading support. */
class Threading
{
public:
    typedef PlankThreadID ID;

    /** Sleeps the current thread. 
     This is specified in seconds. */
    static ResultCode sleep (const double seconds) throw();
    
    /** Yield the current thread.
     This signals that other threads can use the processing time. */
    static ResultCode yield() throw();
    
    /** Get the calling thread ID. */
    static Threading::ID getCurrentThreadID() throw();
    
    static Threading::ID getAudioThreadID() throw();
    
    static void setAudioThreadID (const Threading::ID theID) throw();
    
    static bool currentThreadIsAudioThread() throw();
    
    /** The Thread class itself.
     You must inherit form this and implement the run() function. Then
     call start().
     
     @code
     class MyThread: public Threading::Thread
     {
     public:
        ResultCode run()
        {
            while (getShouldExit() == false)
            {
                //... do stuff here ... 
            }
     
            return 0; // success
        }
     };
     @endcode 
     
     Then from another thread (e.g., in main()):
     
     @code
     ...
     MyThread myThread;
     myThread.start();
     ...
     @endcode
     @ingroup PlonkOtherUserClasses 
     */
    class Thread
    {
    public:
        Thread() throw();
        virtual ~Thread();
        
        ResultCode start() throw();
        ResultCode cancel() throw();
        
        /** Waits for the thread to exit.
         You should call setShouldExit() then call this function to block until
         the thread exits. Or you could run your own loop checking isRunning() 
         until that returns false to simulate the same behviour. */
        ResultCode wait() throw();
        
        /** Returns @c true if the thread is running, otherwise @c false.
         This is thread-safe. */
        bool isRunning() throw();
        
        /** Signals that the thread should exit.
         The run() function should be checking getShouldExit() regularly to determine
         if it should exit. This is thread-safe. */
        ResultCode setShouldExit() throw();
        
        /** Signals that the thread should exit then waits for it to exit.
         This is thread-safe but blocks the calling thread until the other
         thread as exited. 
         @param interval The time interval between checks for the thread to exit. */
        ResultCode setShouldExitAndWait (const double interval = 0.001) throw();
        
        /** Returns @c true if the thread has been signalled to exit, otherwise @c false.
         This is thread-safe. */
        bool getShouldExit() throw();
        
        /** Get this thread's ID. */
        Threading::ID getID() throw();
        
        /** The run() function: you must implement this in your sublcass. */
        virtual ResultCode run() = 0;
        
    protected:
        /** A shortcut for Threading::sleep() */
        static ResultCode sleep (const double seconds) throw();
        
        /** A shortcut for Threading::yield() */
        static ResultCode yield() throw();
        
    private:
        inline PlankThreadRef getPeerRef() { return &thread; }
        
        PLONK_ALIGN(16) 
        PlankThread thread;
        
        Thread (Thread const&);
        Thread& operator= (Thread const&);
    };
    
    friend class Thread;
    
private:
    static PlankResult run (PlankThreadRef plankThread) throw();
};





#endif // PLONK_THREAD_H
