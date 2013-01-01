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

#ifndef PLINK_STANDARDHEADER_H
#define PLINK_STANDARDHEADER_H

#include "../../plank/plank.h"

#ifndef PLINK_API
#define PLINK_API
#endif


#ifdef __cplusplus
    #define PLINK_BEGIN_C_LINKAGE extern "C" {
    #define PLINK_END_C_LINKAGE }
#else
    #define PLINK_BEGIN_C_LINKAGE
    #define PLINK_END_C_LINKAGE
#endif    

#if !defined(_NDEBUG) && !defined(NDEBUG) && !defined(_DEBUG) && !defined(DEBUG)
    #warning You should define appropriate preprocessor macros in Debug (_DEBUG=1 DEBUG=1) and Release (_NDEBUG=1 NDEBUG=1)
#endif

#ifndef PLINK_DEBUG
    #if !defined(NDEBUG) || defined(_DEBUG)
        #define PLINK_DEBUG 1
    #endif
#else
    #if !PLINK_DEBUG
        #undef PLINK_DEBUG
    #endif
#endif


// other std lib headers...
//.. no.. just do this in plank


#define PLINK_MAJOR_VERSION      0
#define PLINK_MINOR_VERSION      1
#define PLINK_BUILDNUMBER        1



#endif // PLINK_STANDARDHEADER_H
