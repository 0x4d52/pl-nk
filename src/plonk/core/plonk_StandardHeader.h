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

#ifndef PLONK_STANDARDHEADER_H
#define PLONK_STANDARDHEADER_H

#include "../../plank/plank.h"

#define PLONK_BIGENDIAN     PLANK_BIGENDIAN
#define PLONK_LITTLEENDIAN  PLANK_LITTLEENDIAN

#if PLANK_WIN
#define PLONK_WIN 1
#endif

#if PLANK_APPLE
#define PLONK_APPLE 1
#endif

#if PLANK_GCC
#define PLONK_GCC 1
#endif

#if PLANK_LLVN
#define PLONK_LLVM 1
#endif

#if PLANK_APPLE_LLVM
#define PLONK_APPLE_LLVM 1
#endif

#if PLANK_ARM
#define PLONK_ARM 1
#endif

#if PLANK_X86
#define PLONK_X86 1
#endif

#if PLANK_PPC
#define PLONK_PPC 1
#endif

#if PLANK_32BIT
#define PLONK_32BIT 1
#endif

#if PLANK_64BIT
#define PLONK_64BIT 1
#endif


#define PLONK_USEPLINK
#ifdef PLONK_USEPLINK
    #include "../../plink/plink.h"
#endif

#define PLONK_NAMESPACE plonk

#ifdef PLONK_NAMESPACE
    #define BEGIN_PLONK_NAMESPACE    namespace PLONK_NAMESPACE {
    #define END_PLONK_NAMESPACE      }
#else
    #define BEGIN_PLONK_NAMESPACE 
    #define END_PLONK_NAMESPACE   
#endif

#if !defined(_NDEBUG) && !defined(NDEBUG) && !defined(_DEBUG) && !defined(DEBUG)
    #warning You should define appropriate preprocessor macros in Debug (_DEBUG=1 DEBUG=1) and Release (_NDEBUG=1 NDEBUG=1)
#endif

#ifndef PLONK_DEBUG
    #if !defined(NDEBUG) || defined(_DEBUG)
        #define PLONK_DEBUG 1
    #endif
#else
    #if !PLONK_DEBUG
        #undef PLONK_DEBUG
    #endif
#endif


#include <cstdlib>
#include <cstdarg>
#include <climits>
#include <cfloat>
#include <cmath>
#include <cwchar>
#include <stdexcept>
#include <typeinfo>
#include <cstring>
#include <cstdio>
#include <iostream>


#define PLONK_MAJOR_VERSION      0
#define PLONK_MINOR_VERSION      1
#define PLONK_BUILDNUMBER        1


#ifdef PLONK_DEBUG
    #include <cassert>
    #define plonk_assert        assert
	#define plonk_assertfalse	(assert (false))
#else
	#define plonk_assert(x)     { }
	#define plonk_assertfalse	{ }
#endif

/* if you're getting your first compile error here on iOS
 it's probably because the file your compiling which includes
 plonk headers is not a C++ file. To make an Obj-C file (.m) 
 an Obj-C++ file use .mm as the file extension. */
BEGIN_PLONK_NAMESPACE

typedef PlankResult ResultCode;

typedef PlankF Float;
typedef PlankD Double;

typedef PlankC Char;
typedef PlankI Int; 
typedef PlankS Short;
typedef PlankL Long;
typedef PlankLL LongLong;

typedef PlankUC UnsignedChar;
typedef PlankUI UnsignedInt; 
typedef PlankUS UnsignedShort;
typedef PlankUL UnsignedLong;
typedef PlankULL UnsignedLongLong;

typedef bool Bool;
typedef PlankP Pointer;

#define PLONK_INT24_MAX PLANK_INT24_MAX

#if (defined (_WIN32) || defined (_WIN64))
//    typedef __int64 LongLong;
//    typedef unsigned __int64 UnsignedLongLong;
	#pragma warning(disable : 4244) // loss of precision
	#pragma warning(disable : 4996) // security of snprintf etc
	#pragma warning(disable : 4800) // bool performance warning
	#define _CRT_SECURE_NO_WARNINGS
//#else
//    typedef long long LongLong;
//	typedef unsigned long long UnsignedLongLong;
#endif


/** To help raise compiler errors when function prototypes change.
 This can be used as the function return type in place of the original
 type to force the compiler to identify places in the source tree where the
 function (normally a virtual) changed in the base class but derived classes 
 are still using the old version (this may otherwise cause strange behaviour). */
struct ForceErrorStruct { int dummy; };
#define FORCE_COMPILER_ERROR_FUNCTION_PROTOTYPE_CHANGED ForceErrorStruct


#ifdef PLONK_USEPLINK

template<class SampleType = float>
struct PlinkBuffer
{
    int bufferSize;
    const SampleType* buffer;
};

template<int N, class SampleType = float>
struct PlinkProcess
{        
    PlinkProcessBase base;
    PlinkBuffer<SampleType> buffers[N];
    
    static void init (PlinkProcess* pp, void* userData, int numOutputs, int numInputs)
    {
        plonk_assert (N >= (numInputs + numOutputs));
        
        pp->base.userData = userData;
        pp->base.numOutputs = numOutputs;
        pp->base.numInputs = numInputs;
        pp->base.numBuffers = N;        
    }
};

#endif

#include "plonk_Memory.h"

END_PLONK_NAMESPACE

#if 0//!PLONK_USE_DEFAULT_GLOBAL_NEW_DELETE
inline void* operator new (size_t size)
{
    void *ptr = plonk::Memory::global().allocateBytes (size);
    
    if (ptr == 0)
        throw std::bad_alloc();
    
    return ptr;
}

inline void operator delete (void *ptr)
{
    plonk::Memory::global().free (ptr);
}
#endif

#ifndef INFINITY
union MSVC_EVIL_FLOAT_HACK
{
   unsigned __int8 Bytes[4];
   float Value;
};
static union MSVC_EVIL_FLOAT_HACK INFINITY_HACK = {{0x00, 0x00, 0x80, 0x7F}};
#define INFINITY (INFINITY_HACK.Value)
#endif


//inline int quantiseUp(const int a, const int q) 
//{ 
//	if(a % q)
//		return a / q * q + q;
//	else
//		return a / q * q;
//}
//
//inline int quantiseDown(const int a, const int q) 
//{ 
//	return a / q * q; 
//}

//#ifndef numElementsInArray
//	#define numElementsInArray(a)   ((int) (sizeof (a) / sizeof ((a)[0])))
//#endif


#ifndef PLONK_ALIGN
    #define PLONK_ALIGN PLANK_ALIGN
#endif

#ifndef PLONK_TYPE_DEFAULT
    #define PLONK_TYPE_DEFAULT Float
#endif

#ifndef LONG_LONG_MAX
	#define LONG_LONG_MAX LLONG_MAX
#endif

#endif // PLONK_STANDARDHEADER_H