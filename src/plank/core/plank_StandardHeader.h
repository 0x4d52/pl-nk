/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#ifndef PLANK_STANDARDHEADER_H
#define PLANK_STANDARDHEADER_H

#define _CRT_SECURE_NO_WARNINGS

/*
 config - define these in preprocessor macros to enable special options
 PLANK_FFT_VDSP=1           -   use vDSP on Mac OS X for FFT routines
 PLANK_FFT_VDSP_FLIPIMAG=1  -   flip the imag part of the FFT to match FFTReal data closely
 PLANK_VEC_VDSP 1           -   use vDSP on Mac OS X for vector ops
*/

#ifndef PLANK_API
#define PLANK_API
#endif

#define PLANK_CDECL

#ifdef __cplusplus
	#define PLANK_UNUSED(a) (void)a
    #define PLANK_BEGIN_C_LINKAGE extern "C" {
    #define PLANK_END_C_LINKAGE }
#else
    #if (defined (_WIN32) || defined (_WIN64))
        #define inline __inline
		#define PLANK_UNUSED(a) (void)a
    #else
		#define PLANK_UNUSED(a)
	#endif
    #define PLANK_BEGIN_C_LINKAGE
    #define PLANK_END_C_LINKAGE
#endif        

#define PLANK_NAMESPACE plank

#ifdef PLANK_NAMESPACE
    #define BEGIN_PLANK_NAMESPACE    namespace PLANK_NAMESPACE {
    #define END_PLANK_NAMESPACE      }
#else
    #define BEGIN_PLANK_NAMESPACE 
    #define END_PLANK_NAMESPACE   
#endif

#if !defined(_NDEBUG) && !defined(NDEBUG) && !defined(_DEBUG) && !defined(DEBUG)
    #warning You should define appropriate preprocessor macros in Debug (_DEBUG=1 DEBUG=1) and Release (_NDEBUG=1 NDEBUG=1)
#endif

#ifndef PLANK_DEBUG
    #if !defined(NDEBUG) || defined(_DEBUG)
        #define PLANK_DEBUG 1
    #endif
#else
    #if !PLANK_DEBUG
        #undef PLANK_DEBUG
    #endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#if (defined (_WIN32) || defined (_WIN64) || defined (WIN64))
    #include <windows.h>
    #include <process.h>
    #include <float.h>
    #include <intrin.h>
    #include <direct.h>

	#undef min
	#undef max

	#pragma warning(disable : 4244) // loss of precision
	#pragma warning(disable : 4996) // security of snprintf etc
	#pragma warning(disable : 4324) // structure padding / alignment
	#pragma warning(disable : 4214) // bit field types other than int.. must revisit this
	#pragma warning(disable : 4054) // type cast function pointer to data pointer
	#pragma warning(disable : 4055) // type cast data pointer to function pointer
	#pragma warning(disable : 4127) // conditional expression is constant (e.g., while(1){...} idiom)
	#pragma warning(disable : 4706) // assignment within conditional expression
	#pragma warning(disable : 4206) // empty compilation unit

    #define PLANK_WIN 1
    #define PLANK_X86 1

	#if defined(_WIN64) || defined(WIN64)
        typedef signed __int64 PlankL;
        typedef unsigned __int64 PlankUL;
        #define PLANK_64BIT 1
    #elif defined(_WIN32)
        #define PLANK_32BIT 1
        typedef signed long PlankL;
        typedef unsigned long PlankUL;
    #endif

	typedef signed __int64 PlankLL;
	typedef unsigned __int64 PlankULL;
	typedef PlankULL PlankBits;

    #define PLANK_LL_MAX   _I64_MAX 
    #define PLANK_ULL_MAX  _UI64_MAX 

    #define snprintf _snprintf
    //#define strncpy _strncpy
    #define PLANK_LITTLEENDIAN  1
    #define PLANK_BIGENDIAN     0
    #define PLANK_ALIGN(amount) __declspec(align(amount))
#else
    // assume unix and Apple 
	#include <unistd.h>
    #include <pthread.h>
    #include <sys/time.h>

    typedef signed long PlankL;
    typedef unsigned long PlankUL;
    typedef signed long long PlankLL;
    typedef unsigned long long PlankULL;
	typedef PlankULL PlankBits;

    #define PLANK_LL_MAX LLONG_MAX
    #define PLANK_ULL_MAX ULLONG_MAX

    #if defined (__APPLE_CPP__) || defined(__APPLE_CC__)
        #define PLANK_APPLE 1
        #include <CoreFoundation/CoreFoundation.h> 
        #include <libkern/OSAtomic.h>

        #ifdef __llvm__
            #ifdef __clang__
                #define PLANK_APPLE_LLVM 1
            #else
                #define PLANK_GCC 1
                #define PLANK_LLVM 1
            #endif
        #else
            #define PLANK_GCC 1
        #endif

        // no __has_feature on xcode3.2
        #if defined(__has_feature)
            #if __has_feature(objc_arc)
                #define PLANK_OBJC_BRIDGE           __bridge
                #define PLANK_OBJC_BRIDGE_RETAINED  __bridge_retained
                #define PLANK_OBJC_BRIDGE_TRANSFER  __bridge_transfer
                #define PLANK_OBJC_STRONG           __strong
                #define PLANK_OBJC_WEAK             __weak
                #define PLANK_OBJC_UNSAFE_RETAINED  __unsafe_unretained
                #define PLANK_OBJC_AUTORELEASING    __autoreleasing
            #endif
        #endif

        #ifndef PLANK_OBJC_BRIDGE
        #define PLANK_OBJC_BRIDGE
        #endif

        #ifndef PLANK_OBJC_BRIDGE_RETAINED
        #define PLANK_OBJC_BRIDGE_RETAINED
        #endif

        #ifndef PLANK_OBJC_BRIDGE_TRANSFER
        #define PLANK_OBJC_BRIDGE_TRANSFER
        #endif 

        #ifndef PLANK_OBJC_STRONG
        #define PLANK_OBJC_STRONG
        #endif

        #ifndef PLANK_OBJC_WEAK
        #define PLANK_OBJC_WEAK
        #endif

        #ifndef PLANK_OBJC_UNSAFE_RETAINED
        #define PLANK_OBJC_UNSAFE_RETAINED
        #endif

        #ifndef PLANK_OBJC_AUTORELEASING
        #define PLANK_OBJC_AUTORELEASING
        #endif

        #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
            #define PLANK_IOS 1

            #if TARGET_IPHONE_SIMULATOR
                #define PLANK_X86 1
            #else
                #define PLANK_ARM 1
            #endif
        #else
            #define PLANK_MAC 1

            #include <mach/mach.h>
            #include <mach/mach_init.h>
            #include <mach/thread_policy.h>

            #if defined (__ppc__) || defined (__ppc64__)
                #define PLANK_PPC 1
            #else
                #define PLANK_X86 1
            #endif
        #endif
    #elif defined(__ANDROID__)
        #define PLANK_ANDROID 1

        #ifdef __i386__
            #define PLANK_X86 1
        #endif
        #ifdef __arm__
            #define PLANK_ARM 1
        #endif
        #ifdef _MIPS_ARCH
            #define PLANK_MIPS 1
        #endif
    #endif

    #if LONG_MAX == 0x7fffffff
        #define PLANK_32BIT 1
    #else
        #define PLANK_64BIT 1
    #endif

    #define PLANK_ALIGN(amount)   __attribute__ ((aligned (amount)))
#endif

#if PLANK_32BIT
#define PLANK_WORDBITS 32
#define PLANK_HALFWORDBITS 16
#define PLANK_WORDSIZE 4
#define PLANK_WIDESIZE 8
#endif

#if PLANK_64BIT
#define PLANK_WORDBITS 64
#define PLANK_HALFWORDBITS 32
#define PLANK_WORDSIZE 8
#define PLANK_WIDESIZE 16
#endif


typedef float PlankF;
typedef double PlankD;

typedef char PlankC;
typedef int PlankI;
typedef short PlankS;

typedef unsigned char PlankUC;
typedef unsigned int PlankUI;
typedef unsigned short PlankUS;

typedef PlankUL PlankB;
#define PLANK_FALSE ((PlankB)0)
#define PLANK_TRUE ((PlankB)(!0))

typedef void* PlankP;
typedef void (*PlankM) (void); // method
typedef const void* PlankConstantP;
#define PLANK_NULL ((PlankP)0)

typedef PlankUI PlankFourCharCode; // hope this didn't break

typedef struct PlankFourCharCodeString
{
    char string[5];
} PlankFourCharCodeString;

#ifdef INFINITY
    #define PLANK_INFINITY (INFINITY)
#else
    union PLANK_MSVC_EVIL_FLOAT_HACK
    {
        unsigned __int8 Bytes[4];
        float Value;
    };
    static union PLANK_MSVC_EVIL_FLOAT_HACK PLANK_INFINITY_HACK = {{0x00, 0x00, 0x80, 0x7F}};
    #define PLANK_INFINITY (PLANK_INFINITY_HACK.Value)
#endif

#define PLANK_FLOAT_ONE         0x3f800000UL
#define PLANK_FLOAT_ONEMASK     0x007fffffUL
#define PLANK_DOUBLE_ONE        0x3ff0000000000000ULL
#define PLANK_DOUBLE_ONEMASK    0x000fffffffffffffULL
#define PLANK_INT24_MAX         0x7fffffUL
#define PLANK_CHARBITS          8


#include "plank_Result.h"


#ifdef PLANK_32BIT
    #define PLANK_WORDSIZE 4
#endif

#ifdef PLANK_64BIT
    #define PLANK_WORDSIZE 8
#endif

#ifndef PLANK_LITTLEENDIAN
    #ifdef __LITTLE_ENDIAN__
        #define PLANK_LITTLEENDIAN  1
        #define PLANK_BIGENDIAN     0
    #else
        #define PLANK_LITTLEENDIAN  0
        #define PLANK_BIGENDIAN     1
    #endif
#endif


// 

#include "plank_Memory.h"

// this must be after the PLANK_LITTLEENDIAN / PLANK_BIGENDIAN defs
#include "../containers/plank_ContainersMisc.h"

#define PLANK_NAMEJOININTERNAL2(a,b)        a ## b
#define PLANK_NAMEJOININTERNAL3(a,b,c)      a ## b ## c
#define PLANK_NAMEJOININTERNAL4(a,b,c,d)    a ## b ## c ## d

#define PLANK_NAMEJOIN2(a,b)       PLANK_NAMEJOININTERNAL2(a,b)
#define PLANK_NAMEJOIN3(a,b,c)     PLANK_NAMEJOININTERNAL3(a,b,c)
#define PLANK_NAMEJOIN4(a,b,c,d)   PLANK_NAMEJOININTERNAL4(a,b,c,d)

#define PLANK_PRESTRING2(a) #a
#define PLANK_PRESTRING(a) PLANK_PRESTRING2(a)

#define PLANK_MAJOR_VERSION      0
#define PLANK_MINOR_VERSION      4
#define PLANK_BUILDNUMBER        3
#define PLANK_VERSION           "v" PLANK_PRESTRING(PLANK_MAJOR_VERSION) "." PLANK_PRESTRING(PLANK_MINOR_VERSION) "." PLANK_PRESTRING(PLANK_BUILDNUMBER)

static inline double pl_TimeNow()
{
#if PLANK_APPLE || PLANK_ANDROID
    struct timeval now;
    gettimeofday (&now, 0);
    return (double)now.tv_sec + now.tv_usec * 0.000001;
#else
    return 0.0;
#endif
}

#if PLANK_APPLE || PLANK_ANDROID
static inline void pl_TimeToTimeSpec (struct timespec* time, double seconds)
{
    time->tv_sec = (long)seconds;
    time->tv_nsec = (long)((seconds - time->tv_sec) * 0.000000001);
}
#endif


#endif // PLANK_STANDARDHEADER_H
