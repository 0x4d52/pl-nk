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

#ifndef PLANK_STANDARDHEADER_H
#define PLANK_STANDARDHEADER_H

#define _CRT_SECURE_NO_WARNINGS

//#define PLANK_FFT_VDSP 1
//#define PLANK_FFT_VDSP_FLIPIMAG 1
//#define PLANK_VEC_VDSP 1

#ifndef PLANK_API
#define PLANK_API
#endif


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

#if (defined (_WIN32) || defined (_WIN64))
    #include <windows.h>
    #include <process.h>
    #include <float.h>
    #include <intrin.h>

    #define PLANK_WIN 1
    #define PLANK_X86 1

    #ifdef _WIN32
        #define PLANK_32BIT 1
        typedef signed long PlankL;
        typedef unsigned long PlankUL;
    #endif

    #ifdef _WIN64
        typedef signed __int64 PlankL;
        typedef unsigned __int64 PlankUL;
        #define PLANK_64BIT 1
    #endif

	typedef signed __int64 PlankLL;
	typedef unsigned __int64 PlankULL;
    #define snprintf _snprintf
    //#define strncpy _strncpy
    #define PLANK_LITTLEENDIAN  1
    #define PLANK_BIGENDIAN     0
    #define PLANK_ALIGN(amount) __declspec (align (amount))
#else
    // assume unix and Apple 
	#include <unistd.h>

    typedef signed long PlankL;
    typedef unsigned long PlankUL;
    typedef signed long long PlankLL;
    typedef unsigned long long PlankULL;

    #if defined (__APPLE_CPP__) || defined(__APPLE_CC__)
        #define PLANK_APPLE 1
        #include <CoreFoundation/CoreFoundation.h> 
        #include <libkern/OSAtomic.h>
        #include <pthread.h>

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

        #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
            #define PLANK_IOS 1

            #if defined(__arm__)
                #define PLANK_ARM 1
            #else
                #define PLANK_X86 1
            #endif
        #else
            #define PLANK_MAC 1

            #if defined (__ppc__) || defined (__ppc64__)
                #define PLANK_PPC 1
                #define PLANK_NOATOMIC64BIT 1
            #else
                #define PLANK_X86 1
            #endif
        #endif

        #if LONG_MAX == 0x7fffffff
            #define PLANK_32BIT 1
        #else
            #define PLANK_64BIT 1
        #endif
    #endif
    #define PLANK_ALIGN(amount)   __attribute__ ((aligned (amount)))
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
typedef const void* PlankConstantP;
#define PLANK_NULL ((PlankP)0)

typedef PlankI PlankFourCharCode;

#include "plank_Result.h"


#ifdef PLANK_32BIT
//#warning PLANK_32BIT
#endif

#ifdef PLANK_64BIT
//#warning PLANK_64BIT
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

#include "plank_Memory.h"

// this must be after the PLANK_LITTLEENDIAN / PLANK_BIGENDIAN defs
#include "../containers/plank_ContainersMisc.h"

#define PLANK_NAMEJOININTERNAL2(a,b)        a ## b
#define PLANK_NAMEJOININTERNAL3(a,b,c)      a ## b ## c
#define PLANK_NAMEJOININTERNAL4(a,b,c,d)    a ## b ## c ## d

#define PLANK_NAMEJOIN2(a,b)       PLANK_NAMEJOININTERNAL2(a,b)
#define PLANK_NAMEJOIN3(a,b,c)     PLANK_NAMEJOININTERNAL3(a,b,c)
#define PLANK_NAMEJOIN4(a,b,c,d)   PLANK_NAMEJOININTERNAL4(a,b,c,d)

#define PLANK_MAJOR_VERSION      0
#define PLANK_MINOR_VERSION      1
#define PLANK_BUILDNUMBER        1



#endif // PLANK_STANDARDHEADER_H
