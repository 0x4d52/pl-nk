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

#ifndef PLINK_UNARYOPPROCESS_H
#define PLINK_UNARYOPPROCESS_H

#include "../plink_Process.h"


/** Unary operator process macros.
 
 @defgroup PlinkUnaryOpMacros Plink unary operator class creation macros
 @ingroup PlinkInternalMacros
 @{
 */

#define PLINK_UNARYOPPROCESS_DEFINE_NN(OPNAME, TYPECODE)\
    void plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _NN (void* ppv, UnaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N; Plank##TYPECODE* output; const Plank##TYPECODE* input;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        input = pp->buffers[1].buffer;\
        PLANK_VECTORUNARYOP_NAME(OPNAME,TYPECODE) (output, input, N);\
    }

#define PLINK_UNARYOPPROCESS_DEFINE_N1(OPNAME, TYPECODE)\
    void plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _N1 (void* ppv, UnaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N; Plank##TYPECODE* output; Plank##TYPECODE value;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        value = pl_##OPNAME##TYPECODE (pp->buffers[1].buffer[0]);\
        PLANK_VECTORFILL_NAME(TYPECODE) (output, value, N);\
    }

#define PLINK_UNARYOPPROCESS_DEFINE_Nn(OPNAME, TYPECODE)\
    void plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _Nn (void* ppv, UnaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N, i; Plank##TYPECODE* output; const Plank##TYPECODE* input;\
        double inputPos, inputInc;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        input = pp->buffers[1].buffer;\
        inputPos = 0.0;\
        inputInc = (double)pp->buffers[1].bufferSize / (double)N;\
        for (i = 0; i < N; ++i) {\
            output[i] = pl_##OPNAME##TYPECODE (input[(int)inputPos]);\
            inputPos += inputInc;\
        }\
    }

#define PLINK_UNARYOPPROCESS_DEFINE_SELECT(OPNAME, TYPECODE)\
    void plink_UnaryOpProcess ## OPNAME ## TYPECODE (void* ppv, UnaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N, n;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        n = pp->buffers[1].bufferSize;\
        if (n == N)         plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _NN (ppv, 0);\
        else if (n == 1)    plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _N1 (ppv, 0);\
        else                plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _Nn (ppv, 0);\
    }


#define PLINK_UNARYOPPROCESS_DECLARE(OPNAME, TYPECODE)\
    void PLINK_API plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _NN (void* pp, UnaryState* state);\
    void PLINK_API plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _N1 (void* pp, UnaryState* state);\
    void PLINK_API plink_UnaryOpProcess ## OPNAME ## TYPECODE ## _Nn (void* pp, UnaryState* state);\
    void PLINK_API plink_UnaryOpProcess ## OPNAME ## TYPECODE (void* pp, UnaryState* state);

#define PLINK_UNARYOPPROCESS_DEFINE(OPNAME, TYPECODE)\
    PLINK_UNARYOPPROCESS_DEFINE_NN(OPNAME, TYPECODE)\
    PLINK_UNARYOPPROCESS_DEFINE_N1(OPNAME, TYPECODE)\
    PLINK_UNARYOPPROCESS_DEFINE_Nn(OPNAME, TYPECODE)\
    PLINK_UNARYOPPROCESS_DEFINE_SELECT(OPNAME, TYPECODE)

/// @} // End group PlinkUnaryOpMacros


PLINK_BEGIN_C_LINKAGE

/** A unary operator processes.
 
 @defgroup PlinkUnaryOpProcessFClass Plink UnaryOpProcessF classes
 @ingroup PlinkClasses
 @{
 */

typedef PlinkState UnaryState;

PLINK_UNARYOPPROCESS_DECLARE(Move, F)
PLINK_UNARYOPPROCESS_DECLARE(Neg, F)
PLINK_UNARYOPPROCESS_DECLARE(Abs, F)
PLINK_UNARYOPPROCESS_DECLARE(Log2, F)
PLINK_UNARYOPPROCESS_DECLARE(Reciprocal, F)
PLINK_UNARYOPPROCESS_DECLARE(Sin, F)
PLINK_UNARYOPPROCESS_DECLARE(Cos, F)
PLINK_UNARYOPPROCESS_DECLARE(Tan, F)
PLINK_UNARYOPPROCESS_DECLARE(Asin, F)
PLINK_UNARYOPPROCESS_DECLARE(Acos, F)
PLINK_UNARYOPPROCESS_DECLARE(Atan, F)
PLINK_UNARYOPPROCESS_DECLARE(Sinh, F)
PLINK_UNARYOPPROCESS_DECLARE(Cosh, F)
PLINK_UNARYOPPROCESS_DECLARE(Tanh, F)
PLINK_UNARYOPPROCESS_DECLARE(Sqrt, F)
PLINK_UNARYOPPROCESS_DECLARE(Log, F)
PLINK_UNARYOPPROCESS_DECLARE(Log10, F)
PLINK_UNARYOPPROCESS_DECLARE(Exp, F)
PLINK_UNARYOPPROCESS_DECLARE(Squared, F)
PLINK_UNARYOPPROCESS_DECLARE(Cubed, F)
PLINK_UNARYOPPROCESS_DECLARE(Ceil, F)
PLINK_UNARYOPPROCESS_DECLARE(Floor, F)
PLINK_UNARYOPPROCESS_DECLARE(Frac, F)
PLINK_UNARYOPPROCESS_DECLARE(Sign, F)
PLINK_UNARYOPPROCESS_DECLARE(M2F, F)
PLINK_UNARYOPPROCESS_DECLARE(F2M, F)
PLINK_UNARYOPPROCESS_DECLARE(A2dB, F)
PLINK_UNARYOPPROCESS_DECLARE(dB2A, F)
PLINK_UNARYOPPROCESS_DECLARE(D2R, F)
PLINK_UNARYOPPROCESS_DECLARE(R2D, F)
PLINK_UNARYOPPROCESS_DECLARE(Distort, F)
PLINK_UNARYOPPROCESS_DECLARE(Zap, F)
//PLINK_UNARYOPPROCESS_DECLARE(ClipNormal, F)

/// @} // End group PlinkUnaryOpProcessFClass

PLINK_END_C_LINKAGE

#endif // PLINK_UNARYOPPROCESS_H
