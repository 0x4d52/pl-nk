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

#ifndef PLANK_H
#define PLANK_H

/* 14/12/2013 
 http://cloc.sourceforge.net v 1.52  T=0.5 s (190.0 files/s, 81070.0 lines/s)
 -------------------------------------------------------------------------------
 Language                     files          blank        comment           code
 -------------------------------------------------------------------------------
 C                               40           5435           2871          18951
 C/C++ Header                    54           2550           4379           6284
 C++                              1             11             31             23
 -------------------------------------------------------------------------------
 SUM:                            95           7996           7281          25258
 -------------------------------------------------------------------------------
 */

// this must be before the standard header incase we want it different in user code?
#define PLANK_API 

#include "core/plank_StandardHeader.h"
#include "core/plank_Lock.h"
#include "core/plank_SpinLock.h"
#include "core/plank_ThreadSpinLock.h"
#include "core/plank_Thread.h"
#include "core/plank_LockFreeMemory.h"

#include "containers/atomic/plank_Atomic.h"
#include "containers/plank_ContainersMisc.h"
#include "containers/plank_SharedPtr.h"
#include "containers/plank_DynamicArray.h"
#include "containers/plank_LockFreeDynamicArray.h"
#include "containers/plank_LockFreeQueue.h"
#include "containers/plank_LockFreeStack.h"
#include "containers/plank_SimpleQueue.h"
#include "containers/plank_SimpleStack.h"
#include "containers/plank_SimpleLinkedList.h"
#include "containers/plank_SimpleMap.h"
#include "containers/plank_LockFreeLinkedListElement.h"
#include "containers/plank_ThreadLocalStorage.h"

#include "maths/plank_Maths.h"
#include "maths/vectors/plank_Vectors.h"

#include "misc/nn/plank_NeuralNode.h"
#include "misc/nn/plank_NeuralLayer.h"
#include "misc/nn/plank_NeuralNetwork.h"

#include "misc/json/plank_JSON.h"
#include "misc/base64/plank_Base64.h"
#include "misc/zip/plank_Zip.h"

#include "files/plank_Path.h"
#include "files/plank_File.h"
#include "files/plank_MultiFileReader.h"
#include "files/plank_IffFileReader.h"
#include "files/plank_IffFileWriter.h"

#include "files/audio/plank_AudioFileCommon.h"
#include "files/audio/plank_AudioFileReader.h"
#include "files/audio/plank_AudioFileWriter.h"
#include "files/audio/plank_AudioFileMetaData.h"
#include "files/audio/plank_AudioFileCuePoint.h"
#include "files/audio/plank_AudioFileRegion.h"

#include "random/plank_RNG.h"
#include "fft/plank_FFT.h"

/** Plank modules....
 Plank is a cross-platform C API for many underlying functioanlity. This inlcudes
 a @link PlankRNGClass random number generator @endlink, @link PlankFFTFClass FFT 
 processing @endlink, @link PlankVectorFunctions vector processing @endlink and
 @link PlankFileClass file IO @endlink.
 
 Plank is broadly written to the C89 standard in order to be compatible with the MSVC
 compilers. Plank does make use of inline functions on all platforms by using the 
 appropriate qualifier via a macro if necessary.
 
 @defgroup PlankModules Plank */

//-------------------------------------------------------------------------plank
/** Plank fundamentals
 @section PlankFundamentalsNamingScheme Naming scheme
 All Plank data types are prefixed with the word 'Plank' (uppercase 'P'). A range of built-in
 types are typdef'd in Plank using the following abbreviations:
 
 - C char (8-bit integer)
 - S short (16-bit integer)
 - I int (32-bit integer)
 - L long (native width integer i.e., 32-bit on 32 bit architectures, 64 on 64-bit).
 - LL long long (64-bit integer)
 - F float (32-bit floating point)
 - D double (64-bit floating point)
 - B bool (Boolean value)
 - P pointer (a void*)
 
 Unsigned version (where applicable) are prefixed with 'U'. So for example, a PlankUI 
 type is just an unsigned int. This naming scheme is also employed in functions.
 
 All functions are prefxed with 'pl_'. This is followed by further specification with
 each word generally starting with an uppercase character (i.e., camel case) and a
 data type code if applicable. For example the equivalent of the 'fabsf' function 
 in Plank is pl_AbsF().
 
 .. [more examples]
 
 @section PlankFundamentalsObjectSystem Pseudo-object-based class system
 Although the API for Plank is entirely C-based it uses a system similar to an 
 object-oriented language in order to perform certain actions. This is usually 
 through the creation of an "object" that is referred to by an opaque pointer 
 (which internally is a C structure). Examples of this are the 
 @link PlankRNGClass random number generator @endlink, @link PlankFFTFClass FFT 
 processing @endlink, and @link PlankFileClass file IO @endlink .
 
 The convention here is to have pl_XXX_Create() and pl_XXX_Destroy() functions to 
 allocate and deallocate the object respectively. In addition there are pl_XXX_Init()
 and pl_XXX_DeInit() functions for initialising and deinitialising the objects. Create
 and destroy functions can be avoided with many of the "classes" by using static allocation
 of the structures. The rule of thumb is that the create functions do not intialise, 
 this must be done in a separate step or using the pl_XXX_CreateAndInit() funciton. 
 Conversely the destroy function DOES call the pl_XXX_DeInit() function.
 
 Here 'XXX' will be the name of the Plank class in question. 
 Then other functions that operate on the object will be in the form pl_XXX_YYY() 
 where 'YYY' is effectively the "member function" name.
 
 @defgroup PlankFundamentals Fundamentals 
 @ingroup PlankModules*/

/** Plank classes...
 @defgroup PlankClasses Plank classes 
 @ingroup PlankModules */

/** Plank functions...
 @defgroup PlankFunctions Plank functions 
 @ingroup PlankModules */

/** Plank misc functions...
 @defgroup PlankMiscFunctions Plank misc functions 
 @ingroup PlankFunctions */

/** Plank internal macros...
 @defgroup PlankInternalMacros Plank internal macros 
 @ingroup PlankModules */
//-------------------------------------------------------------------------plank


//------------------------------------------------------------plank/fundamentals
/** An outline of the core concepts used in Plank.
 @defgroup PlankCoreConcepts	Core Plank concepts. 
 @ingroup PlankFundamentals */
//------------------------------------------------------------plank/fundamentals



#endif // PLANK_H