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

#ifndef PLONK_NUMERICALARRAY_H
#define PLONK_NUMERICALARRAY_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../maths/plonk_InlineUnaryOps.h"
#include "../maths/plonk_InlineBinaryOps.h"
#include "../maths/plonk_InlineMiscOps.h"
#include "../random/plonk_RNG.h"
#include "plonk_Int24.h"
//#include "plonk_Fix.h"

    
template<class NumericalType>
class NumericalArrayFillerBase
{
public:
    static inline void fill (NumericalType* const dst, const NumericalType value, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = value;
    }
    
    static inline void line (NumericalType* const dst, const UnsignedLong size, const NumericalType start, const NumericalType end) throw()
	{
        if (size >= 2)
        {
            double inc = double (end - start) / (size - 1);
            double currentValue = start;
            
            for (int i = 0; i < size; ++i)
            {
                NumericalConverter::roundCopy (currentValue, dst[i]);
                currentValue += inc;
            }
		}
        else plonk_assertfalse;
	}
	
	static inline void series (NumericalType* const dst,
                               const UnsignedLong size, 
                               const NumericalType start, 
                               const NumericalType grow) throw()
	{
        if (size >= 2)
        {                        
            NumericalType currentValue = start;
            
            for (int i = 0; i < size; ++i)
            {
                dst[i] = currentValue;
                currentValue += grow;
            }
		}
        else plonk_assertfalse;
	}
	
	static inline void geom (NumericalType* const dst,
                             const UnsignedLong size, 
                             const NumericalType start, 
                             const NumericalType grow) throw()
	{
        if (size >= 2)
        {                        
            NumericalType currentValue = start;
            
            for (int i = 0; i < size; ++i)
            {
                dst[i] = currentValue;
                currentValue *= grow;
            }
		}
        else plonk_assertfalse;
	}
	
	static inline void rand (NumericalType* const dst,
                             const UnsignedLong size, 
                             const NumericalType lower, 
                             const NumericalType upper) throw()
	{
        if (size >= 1)
        {                                    
            for (int i = 0; i < size; ++i)
                dst[i] = plonk::rand (lower, upper);
		}
        else plonk_assertfalse;
    }
    
    static inline void rand (NumericalType* const dst,
                             const UnsignedLong size, 
                             const NumericalType upper) throw()
    {
        return rand (dst, size, NumericalType (0), upper);
    }
	
	static inline void rand2 (NumericalType* const dst,
                              const UnsignedLong size, 
                              const NumericalType positive) throw()
	{
		return rand (dst, size, -positive, positive);
	}
	
	static inline void exprand (NumericalType* const dst,
                                const UnsignedLong size, 
                                const NumericalType lower, 
                                const NumericalType upper) throw()
	{
        if (size >= 1)
        {                                    
            for (int i = 0; i < size; ++i)
                dst[i] = plonk::exprand (lower, upper);
		}
        else plonk_assertfalse;
	}
};

template<class NumericalType>
class NumericalArrayFiller : public NumericalArrayFillerBase<NumericalType>
{
};

template<>
class NumericalArrayFiller<float> : public NumericalArrayFillerBase<float>
{
public:
    static inline void fill (float* const dst, const float value, const UnsignedLong numItems) throw()
    {        
        pl_VectorFillF_N1 (dst, value, numItems);
    }
    
    static inline void line (float* const dst, const UnsignedLong size, const float start, const float end) throw()
	{
        if (size >= 2)
        {
            pl_VectorLineF_N11 (dst, start, end, size);
		}
        else plonk_assertfalse;
	}
	
	static inline void series (float* const dst,
                               const UnsignedLong size, 
                               const float start, 
                               const float grow) throw()
	{
        if (size >= 2)
        {                        
            pl_VectorRampF_N11 (dst, start, grow, size);
		}
        else plonk_assertfalse;
	}
};

template<>
class NumericalArrayFiller<double> : public NumericalArrayFillerBase<double>
{
public:
    static inline void fill (double* const dst, const double value, const UnsignedLong numItems) throw()
    {        
        pl_VectorFillD_N1 (dst, value, numItems);
    }
    
    static inline void line (double* const dst, const UnsignedLong size, const double start, const double end) throw()
	{
        if (size >= 2)
        {
            pl_VectorLineD_N11 (dst, start, end, size);
		}
        else plonk_assertfalse;
	}
	
	static inline void series (double* const dst,
                               const UnsignedLong size, 
                               const double start, 
                               const double grow) throw()
	{
        if (size >= 2)
        {                        
            pl_VectorRampD_N11 (dst, start, grow, size);
		}
        else plonk_assertfalse;
	}
};


template<class NumericalType, class OtherType>
class NumericalArrayConverterBase
{
public:
    static inline void convertDirect (NumericalType* const dst, const OtherType* const src, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            NumericalConverter::roundCopy (src[i], dst[i]);
    }
    
    static inline void convertScaled (NumericalType* const dst, const OtherType* const src, const UnsignedLong numItems) throw()
    {
        typedef typename BinaryOpTypeUtility<NumericalType, OtherType>::CalcType CalcType;
        
        const CalcType typePeak (TypeUtility<NumericalType>::getTypePeak());
        const CalcType otherTypePeak (TypeUtility<OtherType>::getTypePeak());
        const CalcType otherTypePeakFactor = Math<CalcType>::get1() / otherTypePeak;
        
		UnsignedLong i;
        
        if (typePeak == Math<CalcType>::get1())
        {
            for (i = 0; i < numItems; ++i)
            {
                NumericalType temp;
                NumericalConverter::roundCopy (src[i], temp);
                dst[i] = NumericalType (CalcType (temp) * otherTypePeakFactor);
            }
        }
        else
        {
            for (i = 0; i < numItems; ++i)
            {
                NumericalType temp;
                NumericalConverter::roundCopy (src[i], temp);
                dst[i] = NumericalType (CalcType (temp) * typePeak * otherTypePeakFactor);
            }
        }
    }        
};

template<class NumericalType>
class NumericalArrayConverterCopy
{
public:
    static inline void convertDirect (NumericalType* const dst, const NumericalType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArray<NumericalType>::copyData (dst, src, numItems);
    }
    
    static inline void convertScaled (NumericalType* const dst, const NumericalType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArray<NumericalType>::copyData (dst, src, numItems);
    }        
};

template<>
class NumericalArrayConverterBase<float, char>
{
public:
    static inline void convertDirect (float* const dst, const char* const src, const UnsignedLong numItems) throw()
    {
        pl_VectorConvertC2F_NN (dst, src, numItems);
    }
    
    static inline void convertScaled (float* const dst, const char* const src, const UnsignedLong numItems) throw()
    {
        convertDirect (dst, src, numItems);
        pl_VectorMulF_NN1 (dst, dst, 1.f / PLANK_CHARPEAK_F, numItems);
    }    
};

template<>
class NumericalArrayConverterBase<float, short>
{
public:
    static inline void convertDirect (float* const dst, const short* const src, const UnsignedLong numItems) throw()
    {
        pl_VectorConvertS2F_NN (dst, src, numItems);
    }
    
    static inline void convertScaled (float* const dst, const short* const src, const UnsignedLong numItems) throw()
    {
        convertDirect (dst, src, numItems);
        pl_VectorMulF_NN1 (dst, dst, 1.f / PLANK_SHORTPEAK_F, numItems);
    }    
};

template<>
class NumericalArrayConverterBase<float, Int24>
{
public:
    static inline void convertDirect (float* const dst, const Int24* const src, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = float (src[i]);
    }
    
    static inline void convertScaled (float* const dst, const Int24* const src, const UnsignedLong numItems) throw()
    {
        convertDirect (dst, src, numItems);
        pl_VectorMulF_NN1 (dst, dst, 1.f / PLANK_INT24PEAK_F, numItems);
    }    
};

template<>
class NumericalArrayConverterBase<float, int>
{
public:
    static inline void convertDirect (float* const dst, const int* const src, const UnsignedLong numItems) throw()
    {
        pl_VectorConvertI2F_NN (dst, src, numItems);
    }
    
    static inline void convertScaled (float* const dst, const int* const src, const UnsignedLong numItems) throw()
    {
        convertDirect (dst, src, numItems);
        pl_VectorMulF_NN1 (dst, dst, 1.f / PLANK_INTPEAK_F, numItems);
    }    
};

template<class SrcType>
class NumericalArrayConverterSIMD
{
};

template<>
class NumericalArrayConverterSIMD<float>
{
public:
    template<class DstType>
    static inline void convertScaled (DstType* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        PLANK_ALIGN (PLANK_SIMDF_SIZE) 
        float temp[PLANK_SIMDF_LENGTH];
        
        const float factor = TypeUtility<DstType>::getTypePeak();
        const UnsignedLong numSIMD = numItems >> PLANK_SIMDF_SHIFT;
        const UnsignedLong numRemain = numItems & PLANK_SIMDF_MASK;
        
        DstType* dstPtr = dst;
        const float* srcPtr = src;
        
        {
            PLONK_SHADOW (src);
            PLONK_SHADOW (dst);
            
            for (int i = 0; i < numSIMD; ++i, dstPtr += PLANK_SIMDF_LENGTH, srcPtr += PLANK_SIMDF_LENGTH)
            {
                pl_VectorMulF_NN1 (temp, srcPtr, factor, PLANK_SIMDF_LENGTH);
                NumericalArrayConverterBase<DstType,float>::convertDirect (dstPtr, temp, PLANK_SIMDF_LENGTH);
            }
            
            if (numRemain)
            {
                pl_VectorMulF_NN1 (temp, srcPtr, factor, numRemain);
                NumericalArrayConverterBase<DstType,float>::convertDirect (dstPtr, temp, numRemain);
            }
        }
    }    
};

template<>
class NumericalArrayConverterSIMD<double>
{
public:
    template<class DstType>
    static inline void convertScaled (DstType* const dst, const double* const src, const UnsignedLong numItems) throw()
    {
        PLANK_ALIGN (PLANK_SIMDD_SIZE) 
        double temp[PLANK_SIMDD_LENGTH];
        
        const double factor = TypeUtility<DstType>::getTypePeak();
        const UnsignedLong numSIMD = numItems >> PLANK_SIMDD_SHIFT;
        const UnsignedLong numRemain = numItems & PLANK_SIMDD_MASK;
        
        DstType* dstPtr = dst;
        const double* srcPtr = src;

        {
            PLONK_SHADOW (src);
            PLONK_SHADOW (dst);
            
            for (int i = 0; i < numSIMD; ++i, dstPtr += PLANK_SIMDD_LENGTH, srcPtr += PLANK_SIMDD_LENGTH)
            {
                pl_VectorMulD_NN1 (temp, srcPtr, factor, PLANK_SIMDD_LENGTH);
                NumericalArrayConverterBase<DstType,double>::convertDirect (dstPtr, temp, PLANK_SIMDD_LENGTH);
            }
            
            if (numRemain)
            {
                pl_VectorMulD_NN1 (temp, srcPtr, factor, numRemain);
                NumericalArrayConverterBase<DstType,double>::convertDirect (dstPtr, temp, numRemain);
            }
        }
    }    
};


template<>
class NumericalArrayConverterBase<char, float>
{
public:
    static inline void convertDirect (char* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        pl_VectorConvertF2C_NN (dst, src, numItems);
    }
    
    static inline void convertScaled (char* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayConverterSIMD<float>::convertScaled (dst, src, numItems);
    }    
};

template<>
class NumericalArrayConverterBase<short, float>
{
public:
    static inline void convertDirect (short* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        pl_VectorConvertF2S_NN (dst, src, numItems);
    }
    
    static inline void convertScaled (short* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayConverterSIMD<float>::convertScaled (dst, src, numItems);
    }    
};

template<>
class NumericalArrayConverterBase<Int24, float>
{
public:
    static inline void convertDirect (Int24* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = Int24 (src[i]);
    }
    
    static inline void convertScaled (Int24* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayConverterSIMD<float>::convertScaled (dst, src, numItems);
    }    
};

template<>
class NumericalArrayConverterBase<int, float>
{
public:
    static inline void convertDirect (int* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        pl_VectorConvertF2I_NN (dst, src, numItems);
    }
    
    static inline void convertScaled (int* const dst, const float* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayConverterSIMD<float>::convertScaled (dst, src, numItems);
    }    
};

template<class NumericalType, class OtherType>
class NumericalArrayConverter : public NumericalArrayConverterBase<NumericalType,OtherType>
{
public:
    typedef NumericalArrayConverterBase<NumericalType,OtherType> Base;
    
    static inline void convert (NumericalType* const dst, 
                                const OtherType* const src, 
                                const UnsignedLong numItems,
                                const bool applyScaling) throw()
    {
        if (applyScaling)   Base::convertScaled (dst, src, numItems);
        else                Base::convertDirect (dst, src, numItems);
    }
};

// copy types when src and dst types are the same
template<> class NumericalArrayConverter<float,float>       : public NumericalArrayConverterCopy<float> { };
template<> class NumericalArrayConverter<double,double>     : public NumericalArrayConverterCopy<double> { };
template<> class NumericalArrayConverter<char,char>         : public NumericalArrayConverterCopy<char> { };
template<> class NumericalArrayConverter<short,short>       : public NumericalArrayConverterCopy<short> { };
template<> class NumericalArrayConverter<int,int>           : public NumericalArrayConverterCopy<int> { };
template<> class NumericalArrayConverter<Int24,Int24>       : public NumericalArrayConverterCopy<Int24> { };
template<> class NumericalArrayConverter<LongLong,LongLong> : public NumericalArrayConverterCopy<LongLong> { };

//------------------------------------------------------------------------------

template<class NumericalType, PLONK_BINARYOPFUNCTION(NumericalType, op)>
class NumericalArrayBinaryOpBase
{
public:
    static inline void calcNN (NumericalType* dst, const NumericalType* left, const NumericalType* right, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = op (left[i], right[i]);
    }
    
    static inline void calcN1 (NumericalType* dst, const NumericalType* left, const NumericalType right, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = op (left[i], right);
    }
    
    static inline void calc1N (NumericalType* dst, const NumericalType left, const NumericalType* right, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = op (left, right[i]);
    }
};

template<class NumericalType, PLONK_BINARYOPFUNCTION(NumericalType, op)>
class NumericalArrayBinaryOp : public NumericalArrayBinaryOpBase<NumericalType,op>
{
};

#define PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE,PLONKOP,PLANKOP)\
    template<>\
    class  NumericalArrayBinaryOp    <Plank##TYPECODE, BinaryOpFunctionsHelper<Plank##TYPECODE>::BinaryOpFunctionsType::PLONKOP> \
    :\
    public NumericalArrayBinaryOpBase<Plank##TYPECODE, BinaryOpFunctionsHelper<Plank##TYPECODE>::BinaryOpFunctionsType::PLONKOP> \
    {\
    public:\
        static inline void calcNN (Plank##TYPECODE* dst, const Plank##TYPECODE* left, const Plank##TYPECODE* right, const UnsignedLong numItems) throw() {\
            pl_Vector##PLANKOP##TYPECODE##_NNN (dst, left, right, numItems);\
        }\
        \
        static inline void calcN1 (Plank##TYPECODE* dst, const Plank##TYPECODE* left, const Plank##TYPECODE right, const UnsignedLong numItems) throw() {\
            pl_Vector##PLANKOP##TYPECODE##_NN1 (dst, left, right, numItems);\
        }\
        \
        static inline void calc1N (Plank##TYPECODE* dst, const Plank##TYPECODE left, const Plank##TYPECODE* right, const UnsignedLong numItems) throw() {\
            pl_Vector##PLANKOP##TYPECODE##_N1N (dst, left, right, numItems);\
        }\
    }

#define PLONK_NUMERICALARRAYBINARYOPS_DEFINE(TYPECODE)\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, addop, Add);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, subop, Sub);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, mulop, Mul);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, divop, Div);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, modop, Mod);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, min, Min);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, max, Max);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, pow, Pow);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, isEqualTo, IsEqualTo);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, isNotEqualTo, IsNotEqualTo);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, isGreaterThan, IsGreaterThan);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, isGreaterThanOrEqualTo, IsGreaterThanOrEqualTo);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, isLessThan, IsLessThan);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, isLessThanOrEqualTo, IsLessThanOrEqualTo);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, hypot, Hypot);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, atan2, Atan2);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, sumsqr, SumSqr);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, difsqr, DifSqr);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, sqrsum, SqrSum);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, sqrdif, SqrDif);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, absdif, AbsDif);\
    PLONK_NUMERICALARRAYBINARYOP_DEFINE(TYPECODE, thresh, Thresh)

PLONK_NUMERICALARRAYBINARYOPS_DEFINE(F);
PLONK_NUMERICALARRAYBINARYOPS_DEFINE(D);




//------------------------------------------------------------------------------

template<class NumericalType, PLONK_UNARYOPFUNCTION(NumericalType, op)>
class NumericalArrayUnaryOpBase
{
public:
    static inline void calc (NumericalType* dst, const NumericalType* src, const UnsignedLong numItems) throw()
    {
        for (UnsignedLong i = 0; i < numItems; ++i)
            dst[i] = op (src[i]);
    }    
};

template<class NumericalType, PLONK_UNARYOPFUNCTION(NumericalType, op)>
class NumericalArrayUnaryOp : public NumericalArrayUnaryOpBase<NumericalType,op>
{
};



#define PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE,PLONKOP,PLANKOP)\
    template<>\
    class  NumericalArrayUnaryOp    <Plank##TYPECODE, UnaryOpFunctionsHelper<Plank##TYPECODE>::UnaryOpFunctionsType::PLONKOP> \
    :\
    public NumericalArrayUnaryOpBase<Plank##TYPECODE, UnaryOpFunctionsHelper<Plank##TYPECODE>::UnaryOpFunctionsType::PLONKOP> \
    {\
    public:\
        static inline void calc (Plank##TYPECODE* dst, const Plank##TYPECODE* src, const UnsignedLong numItems) throw() {\
            pl_Vector##PLANKOP##TYPECODE##_NN (dst, src, numItems);\
        }\
    }

#define PLONK_NUMERICALARRAYUNARYOPS_DEFINE(TYPECODE)\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, move, Move);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, neg, Neg);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, abs, Abs);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, log2, Log2);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, reciprocal, Reciprocal);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, sin, Sin);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, cos, Cos);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, tan, Tan);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, asin, Asin);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, acos, Acos);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, atan, Atan);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, sinh, Sinh);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, cosh, Cosh);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, tanh, Tanh);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, sqrt, Sqrt);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, log, Log);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, log10, Log10);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, exp, Exp);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, squared, Squared);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, cubed, Cubed);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, ceil, Ceil);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, floor, Floor);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, frac, Frac);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, sign , Sign);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, m2f, M2F);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, f2m, F2M);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, a2dB, A2dB);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, dB2a, dB2A);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, d2r, D2R);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, r2d, R2D);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, distort, Distort);\
    PLONK_NUMERICALARRAYUNARYOP_DEFINE(TYPECODE, zap, Zap)

PLONK_NUMERICALARRAYUNARYOPS_DEFINE(F);
PLONK_NUMERICALARRAYUNARYOPS_DEFINE(D);


//------------------------------------------------------------------------------

/** For specificying the initial state of a NumericalArray. */
class NumericalArraySpec
{
public:
	NumericalArraySpec (const int size, const bool zeroData) throw() 
	:	size_ (size), 
        zeroData_ (zeroData) 
	{ 
	}
	
	const int size_;
	const bool zeroData_;
    
private:
	NumericalArraySpec& operator= (NumericalArraySpec const&); // to prevent assignment and MSVC complaining!
};

/** Stores arrays of simple numerical values. 
 A subclass of ObjectArray that is designed to store numerical types (float, 
 int, and more). These respond to arithmetic operators so can perform vector 
 arithmetic. These also form the basis of sample buffers for audio in Plonk. 
 NumericalArray objects can be allocated and own their own memory or can be 
 made to point to other memory locations (useful for writing sample data to/from 
 audio drivers for example).
 @tparam NumericalType  This should be a simple numerical type e.g., @c float, 
                        @c int or at least respond to numerical operators and
                        functions. 
 @see NumericalArraySpec
 @ingroup PlonkContainerClasses 
 */
template<class NumericalType>
class NumericalArray : public ObjectArray<NumericalType>
{
public:
    typedef ObjectArrayInternal<NumericalType>          Internal;
    typedef ObjectArray<NumericalType>                  Base;   
    typedef WeakPointerContainer<NumericalArray>        Weak;
    
    typedef ObjectArray<NumericalType>                  ObjectArrayType;
    typedef ObjectArray<ObjectArrayType>                ObjectArray2DType;
    
    typedef typename BinaryOpFunctionsHelper<NumericalType>::BinaryOpFunctionsType BinaryOpFunctionsType;
    typedef typename UnaryOpFunctionsHelper<NumericalType>::UnaryOpFunctionsType UnaryOpFunctionsType;

	class InitialNumber
	{
	public:
		InitialNumber() throw() : value (0), valid (false) { }
		InitialNumber (NumericalType initialValue) throw()
		:	value (initialValue), 
            valid (true) 
		{ 
		}

		template<class OtherType>
		InitialNumber (OtherType initialValue) throw()
		:	value (static_cast<const NumericalType> (initialValue)), 
            valid (true) 
		{ 
		}
		
		const NumericalType value;
		const bool valid;

	private:
		InitialNumber& operator= (InitialNumber const&); // to prevent assignment and MSVC complaining!
	};
	
	NumericalArray() throw()
	:	Base (0, false)
	{
	}	
        
    explicit NumericalArray (Internal* internal) throw() 
	:	Base (internal)
	{
	}
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */
    static NumericalArray fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }
        
    static const NumericalArray& getNull() throw()
	{
		static NumericalArray null;
		return null;
	}	        
    
    static inline void copyData (NumericalType* const dst, const NumericalType* const src, const UnsignedLong numItems) throw()
    {
        Memory::copy (dst, src, numItems * sizeof (NumericalType));
    }
        
    static inline void convertDirect (NumericalType* const dst, const NumericalType* const src, const UnsignedLong numItems) throw()
    {
        copyData (dst, src, numItems);
    }
    
    static inline void convertScaled (NumericalType* const dst, const NumericalType* const src, const UnsignedLong numItems) throw()
    {
        copyData (dst, src, numItems);
    }

    template<class OtherType>
    static inline void convertDirect (NumericalType* const dst, const OtherType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayConverter<NumericalType,OtherType>::convertDirect (dst, src, numItems);        
    }
    
    template<class OtherType>
    static inline void convertScaled (NumericalType* const dst, const OtherType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayConverter<NumericalType,OtherType>::convertScaled (dst, src, numItems);        
    }
    
    static inline void convert (NumericalType* const dst, 
                                const NumericalType* const src, 
                                const UnsignedLong numItems,
                                const bool applyScaling) throw()
    {
        (void)applyScaling;
        copyData (dst, src, numItems);
    }    
    
    template<class OtherType>
    static inline void convert (NumericalType* const dst, 
                                const OtherType* const src, 
                                const UnsignedLong numItems,
                                const bool applyScaling) throw()
    {
        NumericalArrayConverter<NumericalType,OtherType>::convert (dst, src, numItems, applyScaling);        
    }
    
    static inline void zeroData (NumericalType* const dst, const UnsignedLong numItems) throw()
    {
        Memory::zero (dst, numItems * sizeof (NumericalType));
    }
    
    static inline void fill (NumericalType* const dst, const NumericalType value, const UnsignedLong numItems) throw()
    {
        NumericalArrayFiller<NumericalType>::fill (dst, value, numItems);
    }
    
    inline void fill (const NumericalType value) throw()
    {
        const int length = this->length();
        
        if (length > 0)
        {
            NumericalType *thisArray = this->getArray();
            NumericalArrayFiller<NumericalType>::fill (this->getArray(), value, length);
        }
    }
    
    inline NumericalArray (NumericalArray const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}    
    
    /** Assignment operator. */
    inline NumericalArray& operator= (NumericalArray const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}

    inline NumericalArray (ObjectArray<NumericalType> const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}    
       
    NumericalArray (Dynamic const& other) throw()
    :   Base (other.as<NumericalArray>().getInternal())
    {
    }    
	
public:
	/** Construct a NumericalArray by copying a NumericalArray of a different type. */
	template<class CopyType>
	NumericalArray (NumericalArray<CopyType> const& copy) throw()
	:	Base (copy.size(), copy.isNullTerminated())
	{		
		NumericalType *thisArray = this->getArray();
		const CopyType *copyArray = copy.getArray();
		
		if (thisArray != 0 && copyArray != 0)
            NumericalArray::convertDirect (thisArray, copyArray, this->size());
	}
	
	/** Construct a NumericalArray by copying a ObjectArray of a different type.
	 This would need to be a numerical type. */
	template<class CopyType>
	NumericalArray (ObjectArray<CopyType> const& copy) throw()
	:   Base (copy.size(), copy.isNullTerminated())
	{		
		NumericalType *thisArray = this->getArray();
		const CopyType *copyArray = copy.getArray();
		
		if (thisArray != 0 && copyArray != 0)
            NumericalArray::convertData (thisArray, copyArray, this->size());
	}
	
	/** Concatenate two arrays into one. */
	NumericalArray (NumericalArray const& array0, 
				    NumericalArray const& array1) throw()
	:	Base (array0, array1)
	{
	}
	
	ObjectArrayConcatOperatorsDefine(NumericalArray, NumericalType)
		
	/** Constructa a NumericalArray using a NumericalArraySpec.
	 A better idiom for most usages would be to use withSize() or newClear(). */
	NumericalArray (NumericalArraySpec const& spec) throw()
	:	Base (spec.size_, false)
	{
		if (spec.zeroData_) 
            zero();
	}
	
	/** Constructa a NumericalArray using a NumericalArraySpec with optional null termination.
	 A better idiom for most usages would be to use withSize() or newClear(). 
	 Null termination only really makes sense with integer types. */
	NumericalArray (NumericalArraySpec const& spec, const bool needsNullTermination) throw()
	:	Base (spec.size_, needsNullTermination)
	{
		if (spec.zeroData_) 
            zero();
	}
	
	/** Construct a NumericalArray that refers to some other data.
	 It is very important to note that the data is not copies so must continue
	 to exist for the lifetime of this object. 
	 @param size The size of the source data to use.
	 @param dataToUse A pointer to the data. 
	 @param needsNullTermination Whether the data is null terminated ot not. 
	 @see withArrayNoCopy */
	NumericalArray (const int size, NumericalType* dataToUse, const bool needsNullTermination) throw()
	:	Base (size, dataToUse, needsNullTermination)
	{
		plonk_assert (size > 0);
	}
	
	/** Creates a NumericalArray with a given size (length). */
	static NumericalArray<NumericalType> withSize (const int size, const bool zeroData = false) throw()
	{
		return NumericalArray<NumericalType> (NumericalArraySpec(size, zeroData));
	}
	
	/** Creates a NumericalArray with a given size (length) with all items set to zero. */
	static NumericalArray<NumericalType> newClear (const int size) throw()
	{
		return NumericalArray<NumericalType> (NumericalArraySpec(size, true));
	}
		
	/** Creates a NumericalArray with a given size (length) ramping from one value to another. */
	static NumericalArray<NumericalType> line (const int size, const NumericalType start, const NumericalType end) throw()
	{
		plonk_assert (size >= 2);
		
		const int numValues = size < 2 ? 2 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::line (newArray.getArray(), numValues, start, end);

		return newArray;
	}
	
	/** Creates a NumericalArray with a given size (length) using a series. */
	static NumericalArray<NumericalType> series (const int size, 
												 const NumericalType start, 
												 const NumericalType grow) throw()
	{
		plonk_assert (size >= 2);
		
		const int numValues = size < 2 ? 2 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::series (newArray.getArray(), numValues, start, grow);
		
		return newArray;		
	}
	
	/** Creates a NumericalArray with a given size (length) using a geometric series. */
	static NumericalArray<NumericalType> geom (const int size, 
											   const NumericalType start, 
											   const NumericalType grow) throw()
	{
		plonk_assert (size >= 2);
		
		const int numValues = size < 2 ? 2 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::geom (newArray.getArray(), numValues, start, grow);
		
		return newArray;		
	}
	
	/** Creates a NumericalArray with a given size (length) randomly distributed. 
     Values will be between @c lower and @c upper bounds and uniformly distributed. */
	static NumericalArray<NumericalType> rand (const int size, 
											   const NumericalType lower, 
											   const NumericalType upper) throw()
	{
		plonk_assert (size > 0);
		
		const int numValues = size < 1 ? 1 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::rand (newArray.getArray(), numValues, lower, upper);
		
		return newArray;
	}
    
    /** Creates a NumericalArray with a given size (length) randomly distributed. 
     Values will be between zero and the @c upper bound and uniformly distributed. */
    static NumericalArray<NumericalType> rand (const int size, 
											   const NumericalType upper) throw()
    {
		plonk_assert (size > 0);
		
		const int numValues = size < 1 ? 1 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::rand (newArray.getArray(), numValues, upper);
		
		return newArray;
    }
	
	/** Creates a NumericalArray with a given size (length) randomly distributed. 
     Values will be either side of zero upto a magnitude given by @c upper and uniformly distributed. */
	static NumericalArray<NumericalType> rand2 (const int size, 
											    const NumericalType positive) throw()
	{
		plonk_assert (size > 0);
		
		const int numValues = size < 1 ? 1 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::rand2 (newArray.getArray(), numValues, positive);
		
		return newArray;
	}
	
	/** Creates a NumericalArray with a given size (length) with an exponential random distribution. 
     Values will be between @c lower and @c upper bounds and exponentially distributed
     such that more values will appear in the lower end of the range compared to the upper part.
     This is good for selecting frequencies randomly. */
	static NumericalArray<NumericalType> exprand (const int size, 
												  const NumericalType lower, 
												  const NumericalType upper) throw()
	{
		plonk_assert (size > 0);
		
		const int numValues = size < 1 ? 1 : size;
		
		NumericalArray<NumericalType> newArray = NumericalArray<NumericalType>::withSize (numValues);
        NumericalArrayFiller<NumericalType>::exprand (newArray.getArray(), numValues, lower, upper);
		
		return newArray;
	}
	
	
	/** Creates a NumericalArray with a given size (length) containing one or more sine tables. */
	static NumericalArray<NumericalType> sineTable (const int size, 
												    const double repeats = 1.0, 
												    const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		plonk_assert (repeats > 0.f);
		return NumericalArray<double>::line (size, 0.0, Math<double>::get2Pi() * repeats).sin() * peak;
	}
	
    static NumericalArray<NumericalType> harmonicTable (const int size, 
                                                        NumericalArray<NumericalType> const& weights) throw()
	{
		plonk_assert (weights.length() > 0);
        
        NumericalArray<double> result = NumericalArray<double>::newClear (size);
        
        for (int i = 0; i < weights.length(); ++i)
        {
            const int harmonic = i + 1;
            result += NumericalArray<double>::line (size, 0.0, Math<double>::get2Pi() * harmonic).sin() * weights.atUnchecked (i);
        }
        
		return result;
	}
    
	/** Creates a NumericalArray with a given size (length) containing one or more cosine tables. */
	static NumericalArray<NumericalType> cosineTable (const int size, 
													  const double repeats = 1.0, 
													  const NumericalType peak = NumericalType(0)) throw()
	{
		plonk_assert (repeats > 0.f);
        NumericalType actualPeak = peak == NumericalType(0) ? TypeUtility<NumericalType>::getTypePeak() : peak;
		return NumericalArray<double>::line (size, 0.0, Math<double>::get2Pi() * repeats).cos() * actualPeak;
	}
	
	/** Creates a NumericalArray with a given size (length) containing one or more cosine windows. */
	static NumericalArray<NumericalType> cosineWindow (const int size, 
													   const double repeats = 1.0, 
													   const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		plonk_assert (repeats > 0.f);
		return NumericalArray<double>::line (size, 0.0, Math<double>::getPi() * repeats).sin() * peak;
	}
	
	/** Creates a NumericalArray with a given size (length) containing one or more triangle windows. */
	static NumericalArray<NumericalType> triangleWindow (const int size, 
													 	 const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		NumericalArray<NumericalType> bartlett = NumericalArray<NumericalType>::bartlettWindow (size+2, peak);
		return bartlett.range (1, size + 1);
	}
	
	/** Creates a NumericalArray with a given size (length) containing one or more Bartlett windows. */
	static NumericalArray<NumericalType> bartlettWindow (const int size, 
														 const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		return -NumericalArray<NumericalType>::line (size, -peak, peak).abs() + NumericalArray<NumericalType> (peak);
	}
	
	/** Creates a NumericalArray with a given size (length) containing one or more von Hann (Hanning) windows. */
	static NumericalArray<NumericalType> hannWindow (const int size, 
													 const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		return (-NumericalArray<double>::cosineTable (size) + 1.0) * 0.5f * peak;
	}
	
	/** Creates a NumericalArray with a given size (length) containing one or more Hamming windows. */
	static NumericalArray<NumericalType> hammingWindow (const int size, 
													    const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		return (-NumericalArray<double>::cosineTable (size) * 0.46 + 0.54) * peak;
	}
	
	/** Creates a NumericalArray with a given size (length) containing one or more Blackman windows. */
	static NumericalArray<NumericalType> blackmanWindow (const int size, 
                                                         const double alpha = 0.16, 
														 const NumericalType peak = TypeUtility<NumericalType>::getTypePeak()) throw()
	{
		double a0 = (1.0 - alpha) * 0.5;
		double a1 = 0.5;
		double a2 = alpha * 0.5;
		
		return (-NumericalArray<double>::cosineTable (size) * a1 + NumericalArray<double>::cosineTable (size, 2.0) * a2 + a0) * peak;
	}
	
	static int sourceLength (const NumericalType* nullTerminatedSourceArray) throw()
	{
		plonk_assert (nullTerminatedSourceArray != 0);
		
		int size = 0;
		
		while (nullTerminatedSourceArray[size])
			size++;
		
		return size;
	}
	
	static int sourceSize (const NumericalType* nullTerminatedSourceArray) throw()
	{
		return sourceLength (nullTerminatedSourceArray) + 1;
	}
	
	
	/** Construct a NumericalArray from a null terminated source array. */
	NumericalArray (const NumericalType* nullTerminatedSourceArray) throw()
	:	ObjectArray<NumericalType> (sourceSize (nullTerminatedSourceArray), true)
	{
		plonk_assert (nullTerminatedSourceArray != 0);
		
		NumericalType *thisArray = this->getArray();
		
		if (thisArray != 0)
            copyData (thisArray, nullTerminatedSourceArray, this->size());
	}
	
	/** Creates an array by copying data from another source. */
	static NumericalArray<NumericalType> withArray (const int size, 
												    const NumericalType* sourceArray, 
												    const bool needsNullTermination = false) throw()
	{
		plonk_assert (sourceArray != 0);
		
		NumericalArray<NumericalType> result = NumericalArray<NumericalType> (NumericalArraySpec (size, false), 
																			  needsNullTermination);
		
		NumericalType *thisArray = result.getArray();
		
		if (thisArray != 0)
		{            
			if (needsNullTermination)
			{
				const int length = size - 1;				
                copyData (thisArray, sourceArray, length);
				thisArray[length] = NumericalType (0);
			}
			else
			{
                copyData (thisArray, sourceArray, size);
			}
		}
		
		return result;
	}
	
	/** Creates an array by using data from another source. 
	 It is very important to note that the data is not copies so must continue
	 to exist for the lifetime of this object. */
	static NumericalArray<NumericalType> withArrayNoCopy (const int size, 
														  NumericalType* sourceArray, 
														  const bool needsNullTermination = false) throw()
	{
		plonk_assert (sourceArray != 0);
		return NumericalArray<NumericalType> (size, sourceArray, needsNullTermination);
	}
	
	/** Construct an array with a single value. */
    template<class OtherType>
    NumericalArray (OtherType const& value) throw()
	:	Base (NumericalType (value))
	{
	}
    
//	NumericalArray (NumericalType value) throw()
//	:	Base (value)
//	{
//	}

protected:
	static int countValidInitialisers (InitialNumber const& i03) throw()
	{
		int size = 3;
		if (i03.valid) size++; else return size;
		return size;
	}	

public:
	NumericalArray (InitialNumber const &i00,
				    InitialNumber const &i01,
				    InitialNumber const &i02,
				    InitialNumber const &i03 = InitialNumber()) throw()
	:	Base (countValidInitialisers (i03), false)
	{
		NumericalType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.value; else return;
		if (i01.valid) thisArray[ 1] = i01.value; else return;
		if (i02.valid) thisArray[ 2] = i02.value; else return;
		if (i03.valid) thisArray[ 3] = i03.value; else return;
	}
	
protected:
	static int countValidInitialisers (InitialNumber const &i05,
									   InitialNumber const &i06,
									   InitialNumber const &i07) throw()
	{
		int size = 5;
		
		if (i05.valid) size++; else return size;
		if (i06.valid) size++; else return size;
		if (i07.valid) size++; else return size;
		
		return size;
	}
	
public:
	NumericalArray (InitialNumber const &i00,
				    InitialNumber const &i01,
				    InitialNumber const &i02,
				    InitialNumber const &i03,
				    InitialNumber const &i04,
				    InitialNumber const &i05 = InitialNumber(),
				    InitialNumber const &i06 = InitialNumber(),
				    InitialNumber const &i07 = InitialNumber()) throw()
	:	Base (countValidInitialisers (i05, i06, i07), 
              false)
	{
		NumericalType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.value; else return;
		if (i01.valid) thisArray[ 1] = i01.value; else return;
		if (i02.valid) thisArray[ 2] = i02.value; else return;
		if (i03.valid) thisArray[ 3] = i03.value; else return;
		if (i04.valid) thisArray[ 4] = i04.value; else return;
		if (i05.valid) thisArray[ 5] = i05.value; else return;
		if (i06.valid) thisArray[ 6] = i06.value; else return;
		if (i07.valid) thisArray[ 7] = i07.value; else return;
	}
	
protected:
	static int countValidInitialisers (InitialNumber const &i09,
									   InitialNumber const &i10,
									   InitialNumber const &i11,
									   InitialNumber const &i12,
									   InitialNumber const &i13,
									   InitialNumber const &i14,
									   InitialNumber const &i15) throw()
	{
		int size = 9;
		
		if (i09.valid) size++; else return size;
		if (i10.valid) size++; else return size;
		if (i11.valid) size++; else return size;
		if (i12.valid) size++; else return size;
		if (i13.valid) size++; else return size;
		if (i14.valid) size++; else return size;
		if (i15.valid) size++; else return size;
		
		return size;
	}

public:
	NumericalArray (InitialNumber const &i00,
				    InitialNumber const &i01,
				    InitialNumber const &i02,
				    InitialNumber const &i03,
				    InitialNumber const &i04,
				    InitialNumber const &i05,
				    InitialNumber const &i06,
				    InitialNumber const &i07,
				    InitialNumber const &i08,
				    InitialNumber const &i09 = InitialNumber(),
				    InitialNumber const &i10 = InitialNumber(),
				    InitialNumber const &i11 = InitialNumber(),
				    InitialNumber const &i12 = InitialNumber(),
				    InitialNumber const &i13 = InitialNumber(),
				    InitialNumber const &i14 = InitialNumber(),
				    InitialNumber const &i15 = InitialNumber()) throw()
	:	Base (countValidInitialisers (i09, i10, i11, i12, i13, i14, i15), 
              false)
	{
		NumericalType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.value; else return;
		if (i01.valid) thisArray[ 1] = i01.value; else return;
		if (i02.valid) thisArray[ 2] = i02.value; else return;
		if (i03.valid) thisArray[ 3] = i03.value; else return;
		if (i04.valid) thisArray[ 4] = i04.value; else return;
		if (i05.valid) thisArray[ 5] = i05.value; else return;
		if (i06.valid) thisArray[ 6] = i06.value; else return;
		if (i07.valid) thisArray[ 7] = i07.value; else return;
		if (i08.valid) thisArray[ 8] = i08.value; else return;
		if (i09.valid) thisArray[ 9] = i09.value; else return;
		if (i10.valid) thisArray[10] = i10.value; else return;
		if (i11.valid) thisArray[11] = i11.value; else return;
		if (i12.valid) thisArray[12] = i12.value; else return;
		if (i13.valid) thisArray[13] = i13.value; else return;
		if (i14.valid) thisArray[14] = i14.value; else return;
		if (i15.valid) thisArray[15] = i15.value; else return;
	}
	
protected:
	static int countValidInitialisers (InitialNumber const &i17,
									   InitialNumber const &i18,
									   InitialNumber const &i19,
									   InitialNumber const &i20,
									   InitialNumber const &i21,
									   InitialNumber const &i22,
									   InitialNumber const &i23,
									   InitialNumber const &i24,
									   InitialNumber const &i25,
									   InitialNumber const &i26,
									   InitialNumber const &i27,
									   InitialNumber const &i28,
									   InitialNumber const &i29,
									   InitialNumber const &i30,
									   InitialNumber const &i31) throw()
	{
		int size = 17;
		
		if (i17.valid) size++; else return size;
		if (i18.valid) size++; else return size;
		if (i19.valid) size++; else return size;
		if (i20.valid) size++; else return size;
		if (i21.valid) size++; else return size;
		if (i22.valid) size++; else return size;
		if (i23.valid) size++; else return size;
		if (i24.valid) size++; else return size;
		if (i25.valid) size++; else return size;
		if (i26.valid) size++; else return size;
		if (i27.valid) size++; else return size;
		if (i28.valid) size++; else return size;
		if (i29.valid) size++; else return size;
		if (i30.valid) size++; else return size;
		if (i31.valid) size++; else return size;
		
		return size;
	}

public:
	NumericalArray (InitialNumber const &i00,
				    InitialNumber const &i01,
				    InitialNumber const &i02,
				    InitialNumber const &i03,
				    InitialNumber const &i04,
				    InitialNumber const &i05,
				    InitialNumber const &i06,
				    InitialNumber const &i07,
				    InitialNumber const &i08,
				    InitialNumber const &i09,
				    InitialNumber const &i10,
				    InitialNumber const &i11,
				    InitialNumber const &i12,
				    InitialNumber const &i13,
				    InitialNumber const &i14,
				    InitialNumber const &i15,
				    InitialNumber const &i16,
				    InitialNumber const &i17 = InitialNumber(),
				    InitialNumber const &i18 = InitialNumber(),
				    InitialNumber const &i19 = InitialNumber(),
				    InitialNumber const &i20 = InitialNumber(),
				    InitialNumber const &i21 = InitialNumber(),
				    InitialNumber const &i22 = InitialNumber(),
				    InitialNumber const &i23 = InitialNumber(),
				    InitialNumber const &i24 = InitialNumber(),
				    InitialNumber const &i25 = InitialNumber(),
				    InitialNumber const &i26 = InitialNumber(),
				    InitialNumber const &i27 = InitialNumber(),
				    InitialNumber const &i28 = InitialNumber(),
				    InitialNumber const &i29 = InitialNumber(),
				    InitialNumber const &i30 = InitialNumber(),
				    InitialNumber const &i31 = InitialNumber()) throw()
	:	Base (countValidInitialisers (     i17, i18, i19, i20, i21, i22, i23,
                                      i24, i25, i26, i27, i28, i29, i30, i31), 
								      false)
	{
		NumericalType *thisArray = this->getArray();
		
		if (i00.valid) thisArray[ 0] = i00.value; else return;
		if (i01.valid) thisArray[ 1] = i01.value; else return;
		if (i02.valid) thisArray[ 2] = i02.value; else return;
		if (i03.valid) thisArray[ 3] = i03.value; else return;
		if (i04.valid) thisArray[ 4] = i04.value; else return;
		if (i05.valid) thisArray[ 5] = i05.value; else return;
		if (i06.valid) thisArray[ 6] = i06.value; else return;
		if (i07.valid) thisArray[ 7] = i07.value; else return;
		if (i08.valid) thisArray[ 8] = i08.value; else return;
		if (i09.valid) thisArray[ 9] = i09.value; else return;
		if (i10.valid) thisArray[10] = i10.value; else return;
		if (i11.valid) thisArray[11] = i11.value; else return;
		if (i12.valid) thisArray[12] = i12.value; else return;
		if (i13.valid) thisArray[13] = i13.value; else return;
		if (i14.valid) thisArray[14] = i14.value; else return;
		if (i15.valid) thisArray[15] = i15.value; else return;
		if (i16.valid) thisArray[16] = i16.value; else return;
		if (i17.valid) thisArray[17] = i17.value; else return;
		if (i18.valid) thisArray[18] = i18.value; else return;
		if (i19.valid) thisArray[19] = i19.value; else return;
		if (i20.valid) thisArray[20] = i20.value; else return;
		if (i21.valid) thisArray[21] = i21.value; else return;
		if (i22.valid) thisArray[22] = i22.value; else return;
		if (i23.valid) thisArray[23] = i23.value; else return;
		if (i24.valid) thisArray[24] = i24.value; else return;
		if (i25.valid) thisArray[25] = i25.value; else return;
		if (i26.valid) thisArray[26] = i26.value; else return;
		if (i27.valid) thisArray[27] = i27.value; else return;
		if (i28.valid) thisArray[28] = i28.value; else return;
		if (i29.valid) thisArray[29] = i29.value; else return;
		if (i30.valid) thisArray[30] = i30.value; else return;
		if (i31.valid) thisArray[31] = i31.value; else return;
	}
		
	NumericalArray& operator= (ObjectArray<NumericalType> const& other) throw()
	{
		return operator= (static_cast<NumericalArray const&> (other) );
	}
    
    template<PLONK_BINARYOPFUNCTION(NumericalType, op)>
    NumericalArray binary (NumericalArray const& rightOperand) const throw() 
    {
        const unsigned int leftSize = this->size();														
        if (leftSize == 0) 
            return rightOperand;	
        
        const unsigned int rightSize = rightOperand.size();												
        if (rightSize == 0) 
            return *this;																
        
        const bool eitherNullTerminated = this->isNullTerminated() || rightOperand.isNullTerminated();	
        
        const unsigned int leftLength = this->length();													
        const unsigned int rightLength = rightOperand.length();											
        const unsigned int newLength = leftLength > rightLength ? leftLength : rightLength;				
        const unsigned int newSize = leftSize > rightSize ? leftSize : rightSize;	
        
        const NumericalType *leftArray = this->getArray();												
        const NumericalType *rightArray = rightOperand.getArray();
        
        NumericalArray result (NumericalArraySpec (newSize, false), eitherNullTerminated);	
        NumericalType *resultArray = result.getArray();
        
        if (eitherNullTerminated) 
            resultArray[newLength] = 0;											
        
        unsigned int i;
        
        if (leftLength == rightLength)
        {
            NumericalArrayBinaryOp<NumericalType,op>::calcNN (resultArray, leftArray, rightArray, newLength);            
        }
        else if (rightLength == 1)
        {
            NumericalArrayBinaryOp<NumericalType,op>::calcN1 (resultArray, leftArray, rightArray[0], newLength);            
        }
        else if (leftLength == 1)
        {
            NumericalArrayBinaryOp<NumericalType,op>::calc1N (resultArray, leftArray[0], rightArray, newLength);            
        }
        else
        {
            for (i = 0; i < newLength; ++i)	
                resultArray[i] = op (leftArray[i % leftLength], rightArray[i % rightLength]);			
        }
        
        return result;        
    }    
	
    template<PLONK_UNARYOPFUNCTION(NumericalType, op)>
    NumericalArray unary() const throw() 
    {
        const int newSize = this->size();
        const int newLength = this->length();
        
        if (newSize == 0) 
            return *this;
        
        const bool needsNull = this->isNullTerminated();
        NumericalArray result (NumericalArraySpec (newSize, false), needsNull);
        const NumericalType *thisArray = this->getArray();
        NumericalType *resultArray = result.getArray();
        
        NumericalArrayUnaryOp<NumericalType,op>::calc (resultArray, thisArray, newLength); // was newSize but ure that was a bug
        
        if (needsNull) 
            resultArray[newLength] = 0;
        
        return result;
    }    
    
    PLONK_BINARYOPS(NumericalArray);
    PLONK_UNARYOPS(NumericalArray);
	
	inline void zero() throw()
	{
		NumericalType *array = this->getArray();
		
		if (array != 0)
            zeroData (array, this->size());            
	}
    
    NumericalType findMaximum() const throw()
    {
		const NumericalType *array = this->getArray();
		NumericalType result (0);
        
		if (array != 0)
		{            
			const int length = this->length();
			
            if (length > 0)
                result = array[0];
            
            for (int i = 1; i < length; ++i)
                result = BinaryOpFunctionsType::max (result, array[i]);
		}		
        
        return result;
    }
    
    NumericalType findMaximumAbs() const throw()
    {
		const NumericalType *array = this->getArray();
		NumericalType result (0);
        
		if (array != 0)
		{            
			const int length = this->length();
			
            if (length > 0)
                result = array[0];
            
            for (int i = 1; i < length; ++i)
                result = BinaryOpFunctionsType::max (result, UnaryOpFunctionsType::abs (array[i]));
		}		
        
        return result;
    }

    
    NumericalType findMinimum() const throw()
    {
		const NumericalType *array = this->getArray();
		NumericalType result (0);
        
		if (array != 0)
		{            
			const int length = this->length();
			
            if (length > 0)
                result = array[0];
            
            for (int i = 1; i < length; ++i)
                result = BinaryOpFunctionsType::min (result, array[i]);
		}		
        
        return result;
    }    
    
    int indexOfMaximum() const throw()
    {
		const NumericalType *array = this->getArray();
		NumericalType currentMax (0);
        int index = -1;
        
		if (array != 0)
		{            
			const int length = this->length();
			
            if (length > 0)
            {
                index = 0;
                currentMax = array[0];
            }
            
            for (int i = 1; i < length; ++i)
            {
                if (array[i] > currentMax)
                {
                    currentMax = array[i];
                    index = i;
                }
            }            
		}		
        
        return index;
    }
    
    int indexOfMinimum() const throw()
    {
		const NumericalType *array = this->getArray();
		NumericalType currentMin (0);
        int index = -1;
        
		if (array != 0)
		{            
			const int length = this->length();
			
            if (length > 0)
            {
                index = 0;
                currentMin = array[0];
            }
            
            for (int i = 1; i < length; ++i)
            {
                if (array[i] < currentMin)
                {
                    currentMin = array[i];
                    index = i;
                }
            }            
		}		
        
        return index;
    }
    
    NumericalArray& normalise() throw()
    {
        NumericalType *array = this->getArray();
		
		if (array != 0)
		{
            const NumericalType maximum = this->findMaximumAbs();
            
            if (maximum > NumericalType (0))
            {
                const NumericalType factor = UnaryOpFunctionsType::reciprocal (maximum);
                const int size = this->size();
                
                for (int i = 0; i < size; ++i)
                    array[i] *= factor;
            }
		}		
        
        return *this;
    }
	
	bool contains (const NumericalType* items) const throw()
	{
		plonk_assert (items != 0);
		return Base::contains (NumericalArray (items));
	}
	
	int indexOf (NumericalType const& itemToSearchFor, const int startIndex = 0) const throw()
	{
		return Base::indexOf (itemToSearchFor, startIndex);
	}
	
	int indexOf (NumericalArray const& itemsToSearchFor, const int startIndex = 0) const throw()
	{
		return Base::indexOf (itemsToSearchFor, startIndex);
	}
	
	int indexOfAny (NumericalArray const& itemsToSearchFor, const int startIndex = 0) const throw()
	{
		return Base::indexOfAny (itemsToSearchFor, startIndex);
	}
	
	/** Search for a particular sub sequence withing the array. */
	int indexOf (const NumericalType* items, const int startIndex = 0) const throw()
	{
		plonk_assert (items != 0);
		plonk_assert (startIndex >= 0);
		return Base::indexOf (NumericalArray (items), startIndex);
	}
	
	/** Search for any items in a given array. */
	int indexOfAny (const NumericalType *items, const int startIndex = 0) const throw()
	{
		plonk_assert (items != 0);
		plonk_assert (startIndex >= 0);
		return Base::indexOfAny (NumericalArray (items), startIndex);
	}
	
	Base replace (NumericalArray const& find, const NumericalType *substitute) const throw()
	{
		plonk_assert (substitute != 0);
		return Base::replace (find, NumericalArray (substitute));
	}
	
	Base replace (const NumericalType *find, NumericalArray const& substitute) const throw()
	{
		plonk_assert (find != 0);
		return Base::replace(NumericalArray (find), substitute);
	}
	
	Base replace (const NumericalType *find, const NumericalType *substitute) const throw()
	{
		plonk_assert (find != 0);
		plonk_assert (substitute != 0);
		return Base::replace (NumericalArray (find), NumericalArray (substitute));
	}
	
	ObjectArray2DType split (const NumericalType *delimiters) const throw()
	{
		plonk_assert (delimiters != 0);
		return Base::split (NumericalArray(delimiters));
	}

	ObjectArray2DType splitSequence (const NumericalType *delimitingSequence) const throw()
	{
		plonk_assert (delimitingSequence != 0);
		return Base::splitSequence (NumericalArray (delimitingSequence));
	}	
	
	void print (const char *prefix = 0, const bool oneLine = false) const throw()
	{
        int i;
        
		if (oneLine)
		{
			if (prefix) std::cout << prefix << ": ";
			
			for (i = 0; i < this->size(); i++)
			{
				std::cout << this->at (i) << " ";
			}
			
			std::cout << std::endl;
		}
		else
		{
			for (i = 0; i < this->size(); i++)
			{
				if (prefix) std::cout << prefix;
				
				std::cout << "[" << i << "] = " << this->at (i) << std::endl;
			}
		}
	}
    
    int getTypeCode() const throw()
    {
        return TypeUtility<NumericalArray>::getTypeCode();
    }
    
    int getValueTypeCode() const throw()
    {
        return TypeUtility<NumericalType>::getTypeCode();
    }
    
    PLONK_OBJECTARROWOPERATOR(NumericalArray);

};

PLONK_BINARYOPGLOBALS_TEMPLATE(NumericalArray,NumericalType); // declares global functions with the same name as the binary operators
PLONK_UNARYOPGLOBALS_TEMPLATE(NumericalArray,NumericalType);  // declares global functions with the same name as the unary operators



#endif // PLONK_NUMERICALARRAY_H
