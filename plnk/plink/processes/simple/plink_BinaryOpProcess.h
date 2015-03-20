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

#ifndef PLINK_BINARYOPPROCESS_H
#define PLINK_BINARYOPPROCESS_H

#include "../plink_Process.h"


/** Binary operator process macros.
 
 @defgroup PlinkBinaryOpMacros Plink binary operator class creation macros
 @ingroup PlinkInternalMacros
 @{
 */

#define PLINK_BINARYOPPROCESS_DEFINE_NNN(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NNN (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N; Plank##TYPECODE* output;\
        const Plank##TYPECODE* left; const Plank##TYPECODE* right;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer;\
        right = pp->buffers[2].buffer;\
        PLANK_VECTORBINARYOPVECTOR_NAME(OPNAME,TYPECODE) (output, left, right, N);\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_NN1(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NN1 (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N; Plank##TYPECODE* output;\
        const Plank##TYPECODE* left; Plank##TYPECODE right;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer;\
        right = pp->buffers[2].buffer[0];\
        PLANK_VECTORBINARYOPSCALAR_NAME(OPNAME,TYPECODE) (output, left, right, N);\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_N1N(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _N1N (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N; Plank##TYPECODE* output;\
        Plank##TYPECODE left; const Plank##TYPECODE* right;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer[0];\
        right = pp->buffers[2].buffer;\
        PLANK_SCALARBINARYOPVECTOR_NAME(OPNAME,TYPECODE) (output, left, right, N);\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_N11(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _N11 (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N; Plank##TYPECODE* output;\
        Plank##TYPECODE left, right, value;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer[0];\
        right = pp->buffers[2].buffer[0];\
        value = pl_##OPNAME##TYPECODE (left, right);\
        PLANK_VECTORFILL_NAME(TYPECODE) (output, value, N);\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_NNn(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NNn (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N, i; Plank##TYPECODE* output;\
        const Plank##TYPECODE* left; const Plank##TYPECODE* right;\
        double rightPos, rightInc;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer;\
        right = pp->buffers[2].buffer;\
        rightPos = 0.0;\
        rightInc = (double)pp->buffers[2].bufferSize / (double)N;\
        for (i = 0; i < N; ++i) {\
            output[i] = pl_##OPNAME##TYPECODE (left[i], right[(int)rightPos]);\
            rightPos += rightInc;\
        }\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_NnN(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NnN (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N, i; Plank##TYPECODE* output;\
        const Plank##TYPECODE* left; const Plank##TYPECODE* right;\
        double leftPos, leftInc;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer;\
        right = pp->buffers[2].buffer;\
        leftPos = 0.0;\
        leftInc = (double)pp->buffers[1].bufferSize / (double)N;\
        for (i = 0; i < N; ++i) {\
            output[i] = pl_##OPNAME##TYPECODE (left[(int)leftPos], right[i]);\
            leftPos += leftInc;\
        }\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_Nnn(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _Nnn (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N, i; Plank##TYPECODE* output;\
        const Plank##TYPECODE* left; const Plank##TYPECODE* right;\
        double rightPos, rightInc, leftPos, leftInc;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        output = pp->buffers[0].buffer;\
        left = pp->buffers[1].buffer;\
        right = pp->buffers[2].buffer;\
        leftPos = 0.0;\
        leftInc = (double)pp->buffers[1].bufferSize / (double)N;\
        rightPos = 0.0;\
        rightInc = (double)pp->buffers[2].bufferSize / (double)N;\
        for (i = 0; i < N; ++i) {\
            output[i] = pl_##OPNAME##TYPECODE (left[(int)leftPos], right[(int)rightPos]);\
            leftPos += leftInc;\
            rightPos += rightInc;\
        }\
    }

#define PLINK_BINARYOPPROCESS_DEFINE_SELECT(OPNAME, TYPECODE)\
    void plink_BinaryOpProcess ## OPNAME ## TYPECODE (void* ppv, BinaryState* state) {\
        PlinkProcess##TYPECODE* pp; PlankUL N, n1, n2;\
        (void)state;\
        pp = (PlinkProcess##TYPECODE*)ppv;\
        N = pp->buffers[0].bufferSize;\
        n1 = pp->buffers[1].bufferSize;\
        n2 = pp->buffers[2].bufferSize;\
        \
        if ((n1 == N) && (n2 == N))\
            plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NNN (ppv, state);\
        else if (n2 == N)\
            if (n1 == 1)\
                plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _N1N (ppv, state);\
            else\
                plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NnN (ppv, state);\
        else if (n1 == N)\
            if (n2 == 1)\
                plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NN1 (ppv, state);\
            else\
                plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NNn (ppv, state);\
        else if ((n1 == 1) && (n2 == 1))\
            plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _N11 (ppv, state);\
        else\
            plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _Nnn (ppv, state);\
    }

#define PLINK_BINARYOPPROCESS_DECLARE(OPNAME, TYPECODE)\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NNN (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NN1 (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _N1N (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _N11 (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NNn (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _NnN (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE ## _Nnn (void* pp, BinaryState* state);\
     void PLINK_API plink_BinaryOpProcess ## OPNAME ## TYPECODE (void* pp, BinaryState* state);


#define PLINK_BINARYOPPROCESS_DEFINE(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_NNN(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_NN1(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_N1N(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_N11(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_NNn(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_NnN(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_Nnn(OPNAME, TYPECODE)\
    PLINK_BINARYOPPROCESS_DEFINE_SELECT(OPNAME, TYPECODE)

/// @} // End group PlinkBinaryOpMacros


PLINK_BEGIN_C_LINKAGE

/** A binary operator processes.
 
 @defgroup PlinkBinaryOpProcessFClass Plink BinaryOpProcessF classes
 @ingroup PlinkClasses
 @{
 */

typedef PlinkState BinaryState;

PLINK_BINARYOPPROCESS_DECLARE(Add, F)
PLINK_BINARYOPPROCESS_DECLARE(Sub, F)
PLINK_BINARYOPPROCESS_DECLARE(Mul, F)
PLINK_BINARYOPPROCESS_DECLARE(Div, F)
PLINK_BINARYOPPROCESS_DECLARE(Mod, F)
PLINK_BINARYOPPROCESS_DECLARE(Min, F)
PLINK_BINARYOPPROCESS_DECLARE(Max, F)
PLINK_BINARYOPPROCESS_DECLARE(Pow, F)
PLINK_BINARYOPPROCESS_DECLARE(IsEqualTo, F)
PLINK_BINARYOPPROCESS_DECLARE(IsNotEqualTo, F)
PLINK_BINARYOPPROCESS_DECLARE(IsGreaterThan, F)
PLINK_BINARYOPPROCESS_DECLARE(IsGreaterThanOrEqualTo, F)
PLINK_BINARYOPPROCESS_DECLARE(IsLessThan, F)
PLINK_BINARYOPPROCESS_DECLARE(IsLessThanOrEqualTo, F)
PLINK_BINARYOPPROCESS_DECLARE(Hypot, F)
PLINK_BINARYOPPROCESS_DECLARE(Atan2, F)
PLINK_BINARYOPPROCESS_DECLARE(SumSqr, F)
PLINK_BINARYOPPROCESS_DECLARE(DifSqr, F)
PLINK_BINARYOPPROCESS_DECLARE(SqrSum, F)
PLINK_BINARYOPPROCESS_DECLARE(SqrDif, F)
PLINK_BINARYOPPROCESS_DECLARE(AbsDif, F)
PLINK_BINARYOPPROCESS_DECLARE(Thresh, F)

/// @} // End group PlinkBinaryOpProcessFClass

PLINK_END_C_LINKAGE

#endif // PLINK_BINARYOPPROCESS_H
