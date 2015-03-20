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

#ifndef PLINK_H
#define PLINK_H

/* 29/01/2013
 http://cloc.sourceforge.net v 1.52  T=0.5 s (30.0 files/s, 3470.0 lines/s)
 -------------------------------------------------------------------------------
 Language                     files          blank        comment           code
 -------------------------------------------------------------------------------
 C/C++ Header                     9            195            412            405
 C                                6            134            186            403
 -------------------------------------------------------------------------------
 SUM:                            15            329            598            808
 -------------------------------------------------------------------------------
 */

// this must be before the standard header incase we want it different in user code?
#define PLINK_API 

#include "core/plink_StandardHeader.h"

#include "processes/plink_Process.h"
#include "processes/simple/plink_UnaryOpProcess.h"
#include "processes/simple/plink_BinaryOpProcess.h"
#include "processes/simple/plink_MulAddProcess.h"
#include "processes/generators/plink_Saw.h"
#include "processes/generators/plink_Table.h"
#include "processes/generators/plink_WhiteNoise.h"

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
 
 Plink has the concept of processes, which are data structures that contain audio 
 and control data buffers and information about the buffers such as size and the 
 number of input and/or output buffers.

 @code
 typedef struct PlinkBufferF
 {
    int bufferSize;
    float* buffer;
 } PlinkBufferF;
 
 typedef struct PlinkProcessBase
 {
    void* userData;
    int numOutputs;
    int numInputs;
    int numBuffers;
 } PlinkProcessBase;
 
 typedef struct PlinkProcessF
 {
    PlinkProcessBase base;
    PlinkBufferF buffers[1];
 } PlinkProcessF;
 
 typedef struct PlinkState
 {
    double sampleRate;
    double sampleDuration;
 } PlinkState;
 @endcode
 
 <b>Processing functions</b>
 
 Processing functions operate on process structs. Processing is achieved using a 
 primary function, which branches into subfunctions according to the input and 
 output block sizes specified in the process struct. Thus for a sawtooth 
 generator we have the following prototypes:
 
 @code
 void plink_SawProcessF_NN (void* pp, SawProcessStateF* state);
 void plink_SawProcessF_N1 (void* pp, SawProcessStateF* state);
 void plink_SawProcessF_Nn (void* pp, SawProcessStateF* state);
 void plink_SawProcessF    (void* pp, SawProcessStateF* state);
 @endcode
 
 plink_SawProcessF() is the primary function. The subfunctions use a function 
 suffix notation to indicate the number and size of the buffers they expect to 
 process.
 
 - @c N indicates the output buffer's block size in samples.
 -    1 indicates block size is 1 sample.
 - @c n indicates some other arbitrary blocksize that is neither @c N nor 1. 
        @c n appearing in the list more than once might be a different value 
        of @c n, @c n could be larger than @c N.
 
 So in the above plink_SawProcessF() example, we can see that the function 
 expects to have 2 buffers to process. NN indicates that both buffers have the 
 same size. @c N1 indicates that the input buffer is of size 1, and @c Nn 
 indicates that the input buffer is of some arbitrary size. e.g. for N=512:
 
 - Nn could be 512, 64
 - NN would be 512, 512

 @defgroup PlinkCoreConcepts	Core Plink concepts. 
 @ingroup PlinkFundamentals */
//------------------------------------------------------------plink/fundamentals


#endif // PLINK_H