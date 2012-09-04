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

#ifndef PLANK_VECTORS_H
#define PLANK_VECTORS_H

#include "../plank_Maths.h"

/** Vector processing macros.
 
 @defgroup PlankVectorMacros Plank vector function creation macros
 @ingroup PlankInternalMacros
 @{
 */

#define PLANK_VECTOR_NAMEINTERNAL(OP,TYPECODE,SUFFIX) pl_Vector##OP##TYPECODE##SUFFIX

#define PLANK_VECTORUNARYOP_NAME(OP,TYPECODE) PLANK_VECTOR_NAMEINTERNAL(OP,TYPECODE,_NN)

#define PLANK_VECTORUNARYOP_DEFINE(OP,TYPECODE) \
    /** Applies the function pl_##OP##TYPECODE##() to each item in a vector.
    This processes vector @e a applying the function pl_##OP##TYPECODE##() to each 
    item and returns the result in the vector @e result. 
    @param result The output vector is placed here.
    @param a The input vector.
    @param N The number of items in the input/output vectors. */\
    static inline void PLANK_VECTORUNARYOP_NAME(OP,TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* a, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_##OP##TYPECODE (a[i]); }\
    }

#define PLANK_VECTORBINARYOPVECTOR_NAME(OP,TYPECODE) PLANK_VECTOR_NAMEINTERNAL(OP,TYPECODE,_NNN)
#define PLANK_VECTORBINARYOPSCALAR_NAME(OP,TYPECODE) PLANK_VECTOR_NAMEINTERNAL(OP,TYPECODE,_NN1)
#define PLANK_SCALARBINARYOPVECTOR_NAME(OP,TYPECODE) PLANK_VECTOR_NAMEINTERNAL(OP,TYPECODE,_N1N)

#define PLANK_VECTORBINARYOPVECTOR_DEFINE(OP,TYPECODE) \
    /** Applies the binary operator function pl_##OP##TYPECODE##() to each item in two input vectors.
    This processes vectors @e a and @e b applying the function pl_##OP##TYPECODE##() to each 
    of their items in turn and returns the result in the vector @e result. 
    @param result The output vector is placed here.
    @param a Input vector @e a.
    @param b Input vector @e b.
    @param N The number of items in the input/output vectors. */\
    static inline void PLANK_VECTORBINARYOPVECTOR_NAME(OP,TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* a, const Plank##TYPECODE* b, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_##OP##TYPECODE (a[i], b[i]); }\
    }
    
#define PLANK_VECTORBINARYOPSCALAR_DEFINE(OP,TYPECODE) \
    /** Applies the binary operator function pl_##OP##TYPECODE##() to an input vector and a scalar value.
    This processes vector @e a and applies the function pl_##OP##TYPECODE##() to each 
    item and the scalar value @e b and returns the result in the vector @e result. 
    @param result The output vector is placed here.
    @param a Input vector @e a and the source of the left operand of the operation.
    @param b A scalar value and the right operand of the operation.
    @param N The number of items in the input/output vectors. */\
    static inline void PLANK_VECTORBINARYOPSCALAR_NAME(OP,TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* a, Plank##TYPECODE b, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_##OP##TYPECODE (a[i], b); }\
    }

#define PLANK_SCALARBINARYOPVECTOR_DEFINE(OP,TYPECODE) \
    /** Applies the binary operator function pl_##OP##TYPECODE##() to an input vector and a scalar value.
    This processes vector @e b and applies the function pl_##OP##TYPECODE##() to each 
    item and the scalar value @e a and returns the result in the vector @e result. 
    @param result The output vector is placed here.
    @param a A scalar value and the left operand of the operation.
    @param b Input vector @e b and the source of the right operand of the operation.
    @param N The number of items in the input/output vectors. */\
    static inline void PLANK_SCALARBINARYOPVECTOR_NAME(OP,TYPECODE) (Plank##TYPECODE *result, Plank##TYPECODE a, const Plank##TYPECODE* b, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_##OP##TYPECODE (a, b[i]); }\
    }


#define PLANK_VECTORBINARYOP_DEFINE(OP,TYPECODE) \
    PLANK_VECTORBINARYOPVECTOR_DEFINE(OP,TYPECODE)\
    PLANK_VECTORBINARYOPSCALAR_DEFINE(OP,TYPECODE)\
    PLANK_SCALARBINARYOPVECTOR_DEFINE(OP,TYPECODE)

#define PLANK_VECTORMULADD_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(MulAdd,TYPECODE,_NNNN)
#define PLANK_VECTORMULADDINPLACE_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(MulAdd,TYPECODE,_NNN)
#define PLANK_VECTORMULSCALARADD_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(MulAdd,TYPECODE,_NNN1)
#define PLANK_VECTORSCALARMULSCALARADD_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(MulAdd,TYPECODE,_NN11)
#define PLANK_VECTORSCALARMULADD_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(MulAdd,TYPECODE,_NN1N)

#define PLANK_VECTORMULADD_DEFINE(TYPECODE) \
    /** Applies the multiply and add operator to a set of vectors.
    @f$ result = input \times mul + add @f$
    @param result The output vector.
    @param input The input vector.
    @param mul A vector containing the value to multiply the input by.
    @param add A vector containing the value to add after the multiply.
    @param N The number of items in the vectors. */\
    static inline void PLANK_VECTORMULADD_NAME(TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* input, const Plank##TYPECODE* mul, const Plank##TYPECODE* add, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_Add##TYPECODE (pl_Mul##TYPECODE (input[i], mul[i]), add[i]); }\
    }


#define PLANK_VECTORMULADDINPLACE_DEFINE(TYPECODE) \
    /** Applies the multiply and add operator to a set of vectors.
    @f$ io = io \times mul + add @f$
    @param io The input and output vector.
    @param mul A vector containing the value to multiply the input by.
    @param add A vector containing the value to add after the multiply.
    @param N The number of items in the vectors. */\
    static inline void PLANK_VECTORMULADDINPLACE_NAME(TYPECODE) (Plank##TYPECODE *io, const Plank##TYPECODE* mul, const Plank##TYPECODE* add, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { io[i] = pl_Add##TYPECODE (pl_Mul##TYPECODE (io[i], mul[i]), add[i]); }\
    }

#define PLANK_VECTORMULSCALARADD_DEFINE(TYPECODE) \
    /** Applies the multiply and add operator to a set of vectors.
    @f$ result= input \times mul + add @f$
    @param io The input and output vector.
    @param mul A vector containing the value to multiply the input by.
    @param add A scalar to add after the multiply.
    @param N The number of items in the vectors. */\
    static inline void PLANK_VECTORMULSCALARADD_NAME(TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* input, const Plank##TYPECODE* mul, Plank##TYPECODE add, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_Add##TYPECODE (pl_Mul##TYPECODE (input[i], mul[i]), add); }\
    }

#define PLANK_VECTORSCALARMULSCALARADD_DEFINE(TYPECODE) \
    /** Applies the multiply and add operator to a set of vectors.
    @f$ result= input \times mul + add @f$
    @param io The input and output vector.
    @param mul A scalar to multiply the input by.
    @param add A scalar to add after the multiply.
    @param N The number of items in the vectors. */\
    static inline void PLANK_VECTORSCALARMULSCALARADD_NAME(TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* input, Plank##TYPECODE mul, Plank##TYPECODE add, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_Add##TYPECODE (pl_Mul##TYPECODE (input[i], mul), add); }\
    }

#define PLANK_VECTORSCALARMULADD_DEFINE(TYPECODE) \
    /** Applies the multiply and add operator to a set of vectors.
    @f$ result= input \times mul + add @f$
    @param io The input and output vector.
    @param mul A scalar to multiply the input by.
    @param add A vector containing the value to add after the multiply.
    @param N The number of items in the vectors. */\
    static inline void PLANK_VECTORSCALARMULADD_NAME(TYPECODE) (Plank##TYPECODE *result, const Plank##TYPECODE* input, Plank##TYPECODE mul, const Plank##TYPECODE* add, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_Add##TYPECODE (pl_Mul##TYPECODE (input[i], mul), add[i]); }\
    }


#define PLANK_VECTORFILL_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(Fill,TYPECODE,_N1)

#define PLANK_VECTORFILL_DEFINE(TYPECODE) \
    /** Fills a vector with a constant. */\
    static inline void PLANK_VECTORFILL_NAME(TYPECODE) (Plank##TYPECODE *result, Plank##TYPECODE value, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = value; }\
    }

#define PLANK_VECTORCLEAR_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(Clear,TYPECODE,_N)

#define PLANK_VECTORCLEAR_DEFINE(TYPECODE) \
    /** Fills a vector with zeros. */\
    static inline void PLANK_VECTORCLEAR_NAME(TYPECODE) (Plank##TYPECODE *result, PlankUL N) {\
		PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = (Plank##TYPECODE)0; }\
    }

#define PLANK_VECTORRAMP_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(Ramp,TYPECODE,_N11)

#define PLANK_VECTORRAMP_DEFINE(TYPECODE) \
    /** Fills a vector with a ramp.
    The vector will start at value @e a and increment by @e b for each item in the vector. */\
    static inline void PLANK_VECTORRAMP_NAME(TYPECODE) (Plank##TYPECODE *result, Plank##TYPECODE a, Plank##TYPECODE b, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = a; a = a + b; }\
    }

#define PLANK_VECTORLINE_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(Line,TYPECODE,_N11)

#define PLANK_VECTORLINE_DEFINE(TYPECODE) \
    /** Fills a vector with a line.
    The vector will start at value @e a and end at @e b. */\
    static inline void PLANK_VECTORLINE_NAME(TYPECODE) (Plank##TYPECODE *result, Plank##TYPECODE a, Plank##TYPECODE b, PlankUL N) {\
        PLANK_VECTORRAMP_NAME(TYPECODE) (result, a, (b - a) / (Plank##TYPECODE)(N - 1), N);\
    }

#define PLANK_VECTORLOOKUP_NAME(TYPECODE) PLANK_VECTOR_NAMEINTERNAL(Lookup,TYPECODE,_NnN)

#define PLANK_VECTORLOOKUP_DEFINE(TYPECODE) \
    /** Generate a vector by using another vector as the indices into a lookup table.
    Fractional indices are used to interpolate between value in the lookup table.
    @param result The output vectir
    @param table A look up table.
    @param n The size of the lookup table.
    @param index A vector containing the indices.
    @param N The size of the output and index vectors. */\
    static inline void PLANK_VECTORLOOKUP_NAME(TYPECODE) (Plank##TYPECODE *result, Plank##TYPECODE *table, PlankUL n, Plank##TYPECODE *index, PlankUL N) {\
        PlankUL i; PLANK_UNUSED(n); for (i = 0; i < N; PLANK_INC(i)) { result[i] = pl_Lookup##TYPECODE (table, index[i]); }\
    }


#define PLANK_VECTORCONVERT_NAME(DSTTYPECODE,SRCTYPECODE)\
    PLANK_VECTOR_NAMEINTERNAL(Convert,SRCTYPECODE##2##DSTTYPECODE,_NN)

#define PLANK_VECTORCONVERTROUND_NAME(DSTTYPECODE,SRCTYPECODE)\
    PLANK_VECTOR_NAMEINTERNAL(ConvertRound,SRCTYPECODE##2##DSTTYPECODE,_NN)


#define PLANK_VECTORCONVERT_DEFINE(DSTTYPECODE,SRCTYPECODE) \
    /** Convert a vector of one data type to another. */\
    static inline void PLANK_VECTORCONVERT_NAME(DSTTYPECODE,SRCTYPECODE) (Plank##DSTTYPECODE *result, const Plank##SRCTYPECODE* a, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = (Plank##DSTTYPECODE)a[i]; }\
    }

#define PLANK_VECTORCONVERTROUNDF_DEFINE(SRCTYPECODE) \
    /** Convert a vector of one data type to a float. */\
    static inline void PLANK_VECTORCONVERTROUND_NAME(F,SRCTYPECODE) (PlankF *result, const Plank##SRCTYPECODE* a, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = (PlankF)a[i] + 0.5f; }\
    }

#define PLANK_VECTORCONVERTROUNDD_DEFINE(SRCTYPECODE) \
    /** Convert a vector of one data type to a float. */\
    static inline void PLANK_VECTORCONVERTROUND_NAME(D,SRCTYPECODE) (PlankD *result, const Plank##SRCTYPECODE* a, PlankUL N) {\
        PlankUL i; for (i = 0; i < N; PLANK_INC(i)) { result[i] = (PlankD)a[i] + 0.5; }\
    }

#define PLANK_VECTORCONVERTERS_DEFINE\
    PLANK_VECTORCONVERT_DEFINE(I,C)\
    PLANK_VECTORCONVERT_DEFINE(S,C)\
    PLANK_VECTORCONVERT_DEFINE(F,C)\
    PLANK_VECTORCONVERT_DEFINE(D,C)\
    PLANK_VECTORCONVERT_DEFINE(C,I)\
    PLANK_VECTORCONVERT_DEFINE(S,I)\
    PLANK_VECTORCONVERT_DEFINE(F,I)\
    PLANK_VECTORCONVERT_DEFINE(D,I)\
    PLANK_VECTORCONVERT_DEFINE(C,S)\
    PLANK_VECTORCONVERT_DEFINE(I,S)\
    PLANK_VECTORCONVERT_DEFINE(F,S)\
    PLANK_VECTORCONVERT_DEFINE(D,S)\
    PLANK_VECTORCONVERT_DEFINE(C,F)\
    PLANK_VECTORCONVERT_DEFINE(I,F)\
    PLANK_VECTORCONVERT_DEFINE(S,F)\
    PLANK_VECTORCONVERT_DEFINE(D,F)\
    PLANK_VECTORCONVERT_DEFINE(C,D)\
    PLANK_VECTORCONVERT_DEFINE(I,D)\
    PLANK_VECTORCONVERT_DEFINE(S,D)\
    PLANK_VECTORCONVERT_DEFINE(F,D)\

#define PLANK_VECTORCONVERTERSROUND_DEFINE\
    PLANK_VECTORCONVERTROUNDF_DEFINE(C)\
    PLANK_VECTORCONVERTROUNDF_DEFINE(I)\
    PLANK_VECTORCONVERTROUNDF_DEFINE(S)\
    PLANK_VECTORCONVERTROUNDD_DEFINE(C)\
    PLANK_VECTORCONVERTROUNDD_DEFINE(I)\
    PLANK_VECTORCONVERTROUNDD_DEFINE(S)


#define PLANK_VECTOR_OPS(TYPECODE) \
    PLANK_VECTORFILL_DEFINE(TYPECODE)\
    PLANK_VECTORCLEAR_DEFINE(TYPECODE)\
    PLANK_VECTORRAMP_DEFINE(TYPECODE)\
    PLANK_VECTORLINE_DEFINE(TYPECODE)\
    \
    PLANK_VECTORUNARYOP_DEFINE(Move,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Neg,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Abs,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Log2,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Reciprocal,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Sin,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Cos,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Tan,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Asin,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Acos,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Atan,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Sinh,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Cosh,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Tanh,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Sqrt,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Log,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Log10,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Exp,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Squared,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Cubed,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Ceil,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Floor,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Frac,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Sign,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(M2F,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(F2M,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(A2dB,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(dB2A,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(D2R,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(R2D,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Distort,TYPECODE)\
    PLANK_VECTORUNARYOP_DEFINE(Zap,TYPECODE)\
    \
    PLANK_VECTORBINARYOP_DEFINE(Add,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Sub,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Mul,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Div,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Min,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Max,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Pow,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(IsEqualTo,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(IsNotEqualTo,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(IsGreaterThan,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(IsGreaterThanOrEqualTo,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(IsLessThan,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(IsLessThanOrEqualTo,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Hypot,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Atan2,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(SumSqr,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(DifSqr,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(SqrSum,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(SqrDif,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(AbsDif,TYPECODE)\
    PLANK_VECTORBINARYOP_DEFINE(Thresh,TYPECODE)\
    \
    PLANK_VECTORMULADD_DEFINE(TYPECODE)\
    PLANK_VECTORMULADDINPLACE_DEFINE(TYPECODE)\
    PLANK_VECTORMULSCALARADD_DEFINE(TYPECODE)\
    PLANK_VECTORSCALARMULSCALARADD_DEFINE(TYPECODE)\
    PLANK_VECTORSCALARMULADD_DEFINE(TYPECODE)\
    \
    PLANK_VECTORLOOKUP_DEFINE(TYPECODE)

/// @} End group PlankVectorMacros

#if defined(PLANK_VEC_VDSP) //&& !DOXYGEN
    #include "plank_vDSP.h"
#elif defined(PLANK_VEC_OTHERLIB) //etc!
    #include "some other vector lib" // must define PLANK_VEC_CUSTOM
#endif

#if !defined(PLANK_VEC_CUSTOM) || DOXYGEN

/** Vector processing functions.
 These process vectors (arrays) of data applying common maths functions to the inputs.
 The default is to use the scalar processing functions (implmented with a loop in C)
 but many of these operations can be performed with faster, optimised libraries on some
 platforms (e.g., vDSP on Mac OS X and iOS).
 
 The naming convention is to prefix all functions with 'pl_Vector'. This is followed by
 the name of the operation which is commonly the name of an equivalent scalar function.
 Finally the name is suffixed with a code to identify the type of vectors and/or
 scalar values passed to the function. The code starts with the number of items in the
 output vector and is followed by codes for the sizes of the other input vectors. For example,
 all the unary operators are suffixed '_NN' so show the input and output vectors are the same
 size 'N' (which is the N argument passed the the function). Some binary operators are suffixed 
 '_NNN', '_NN1' or '_N1N'. The first is where there are two input vectors and the output vector
 that are all the same size. '_NN1' has an input vector with the same size as the output and
 a scalar value '1' (i.e., a scalar) as the other operand.
 
 @defgroup PlankVectorFunctions Plank vector functions
 @ingroup PlankFunctions
 @{
 */

PLANK_VECTOR_OPS(F)
PLANK_VECTOR_OPS(D)
PLANK_VECTORCONVERTERS_DEFINE
PLANK_VECTORCONVERTERSROUND_DEFINE

/// @} // End group PlankVectorFunctions

#endif // PLANK_VEC_CUSTOM

/** Swap the endianness of a vector of unsigned short elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianUS (PlankUS* data, PlankUL N)
{
    PlankUL i;
    
    for (i = 0; i < N; PLANK_INC (i), PLANK_INC (data))
        pl_SwapEndianUS (data);
}

/** Swap the endianness of a vector of short elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianS (PlankS* data, PlankUL N)
{
    pl_VectorSwapEndianUS ((PlankUS*)data, N);
}

/** Swap the endianness of a vector of unsigned 24-bit int elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianUI24 (PlankUI24* data, PlankUL N)
{
    PlankUL i;
    
    for (i = 0; i < N; PLANK_INC (i), PLANK_INC (data))
        pl_SwapEndianUI24 (data);
}

/** Swap the endianness of a vector of 24-bit int elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianI24 (PlankI24* data, PlankUL N)
{
    pl_VectorSwapEndianUI24 ((PlankUI24*)data, N);
}

/** Swap the endianness of a vector of unsigned int elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianUI (PlankUI* data, PlankUL N)
{
    PlankUL i;
    
    for (i = 0; i < N; PLANK_INC (i), PLANK_INC (data))
        pl_SwapEndianUI (data);
}

/** Swap the endianness of a vector of int elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianI (PlankI* data, PlankUL N)
{
    pl_VectorSwapEndianUI ((PlankUI*)data, N);
}

/** Swap the endianness of a vector of unsigned long elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianUL (PlankUL* data, PlankUL N)
{
    PlankUL i;
    
    for (i = 0; i < N; PLANK_INC (i), PLANK_INC (data))
        pl_SwapEndianUL (data);
}

/** Swap the endianness of a vector of long elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianL (PlankL* data, PlankUL N)
{
    pl_VectorSwapEndianUL ((PlankUL*)data, N);
}

/** Swap the endianness of a vector of unsigned long long (64-bit) elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianULL (PlankULL* data, PlankUL N)
{
    PlankUL i;
    
    for (i = 0; i < N; PLANK_INC (i), PLANK_INC (data))
        pl_SwapEndianULL (data);
}

/** Swap the endianness of a vector of long long (64-bit) elements.
 @ingroup PlankEndianFunctions */
static inline void pl_VectorSwapEndianLL (PlankLL* data, PlankUL N)
{
    pl_VectorSwapEndianULL ((PlankULL*)data, N);
}


#endif // PLANK_VECTORS_H

