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

#ifndef PLINK_H
#define PLINK_H

/* 10/1/2012
 http://cloc.sourceforge.net v 1.52  T=0.5 s (18.0 files/s, 1524.0 lines/s)
 -------------------------------------------------------------------------------
 Language                     files          blank        comment           code
 -------------------------------------------------------------------------------
 C/C++ Header                     6             98             99            356
 C                                3             34             18            157
 -------------------------------------------------------------------------------
 SUM:                             9            132            117            513
 -------------------------------------------------------------------------------
 */ 

// this must be before the standard header incase we want it different in user code?
#define PLINK_API 

#include "core/plink_StandardHeader.h"

#include "processes/plink_Process.h"
#include "processes/simple/plink_UnaryOpProcess.h"
#include "processes/simple/plink_BinaryOpProcess.h"
#include "processes/oscillators/plink_Saw.h"

/** Plink modules....
 @defgroup PlinkModules Plink */

//-------------------------------------------------------------------------plink
/** Plink fundamentals...
 @defgroup PlinkFundamentals Fundamentals 
 @ingroup PlinkModules*/

/** Plink classes...
 @defgroup PlinkClasses Classes 
 @ingroup PlinkModules */

/** Plink internal macros...
 @defgroup PlinkInternalMacros Plink internal macros 
 @ingroup PlinkModules */
//-------------------------------------------------------------------------plink

//------------------------------------------------------------plink/fundamentals
/** An outline of the core concepts used in Plink.
 @defgroup PlinkCoreConcepts	Core Plink concepts. 
 @ingroup PlinkFundamentals */
//------------------------------------------------------------plink/fundamentals


#endif // PLINK_H