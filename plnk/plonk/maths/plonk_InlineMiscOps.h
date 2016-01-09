/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_INLINEMISCOPS_H
#define PLONK_INLINEMISCOPS_H

#include "plonk_Constants.h"
#include "plonk_InlineUnaryOps.h"

//------------------------------------------------------------------------------

/** Miscellaneous arithmetic functions.
 
 @defgroup PlonkMiscArithmeticFunctions Miscellaneous arithmetic functions 
 @ingroup PlonkOtherUserFunctions
 @{
 */

template<class Type> PLONK_INLINE_HIGH Type clip (Type const& value, Type const& lower, Type const& upper) throw() { return min (max (lower, value), upper); }

PLONK_INLINE_HIGH int wrap (int value, int lower, int upper) throw()
{
	int range;
    
	if (value >= upper) 
	{
		range = upper - lower;
		value -= range;
        
		if (value < upper) 
            return value;
	} 
	else if (value < lower) 
	{
		range = upper - lower;
		value += range;
        
		if (value >= lower) 
            return value;
	} 
	else
    {
        return value;
	}
    
	if (upper == lower) 
        return lower;
	else
        return value % range + lower;
}

template<class Type>
PLONK_INLINE_HIGH Type wrap (Type const& v, Type const& lower, Type const& upper) throw()
{
	Type range;
    Type value = v;
    
	if (value >= upper) 
	{
		range = upper - lower;
		value -= range;
        
		if (value < upper) 
            return value;
	} 
	else if (value < lower) 
	{
		range = upper - lower;
		value += range;
        
		if (value >= lower) 
            return value;
	} 
	else
    {
        return value;
	}
    
	if (upper == lower) 
        return lower;
    else
        return value - range * floor ((value - lower) / range); 
}



template<class Type>
PLONK_INLINE_HIGH Type linlin (Type const& input, 
                    Type const& inLow, Type const& inHigh,
                    Type const& outLow, Type const& outHigh) throw()
{
	const Type inRange = inHigh - inLow;
	const Type outRange = outHigh - outLow;
	return (input - inLow) * outRange / inRange + outLow;
}

template<class Type>
PLONK_INLINE_HIGH Type linlin2 (Type const& input, 
                     Type const& inLow, Type const& inRange,
                     Type const& outLow, Type const& outRange) throw()
{
	return (input - inLow) * outRange / inRange + outLow;
}

template<class Type>
PLONK_INLINE_HIGH Type linsin (Type const& input, 
                    Type const& inLow, Type const& inHigh,
                    Type const& outLow, Type const& outHigh) throw()
{
	const Type inRange = inHigh - inLow;
	const Type outRange = outHigh - outLow;
    
	const Type inPhase = (input - inLow) * Math<Type>::getPi() / inRange + Math<Type>::getPi();
	const Type cosInPhase = cos (inPhase) * Math<Type>::get0_5() + Math<Type>::get0_5();
	
	return cosInPhase * outRange + outLow;	
}

template<class Type>
PLONK_INLINE_HIGH Type linsin2 (Type const& input, 
                     Type const& inLow, Type const& inRange,
                     Type const& outLow, Type const& outRange) throw()
{	
	const Type inPhase = (input - inLow) * Math<Type>::getPi() / inRange + Math<Type>::getPi();
	const Type cosInPhase = cos (inPhase) * Math<Type>::get0_5() + Math<Type>::get0_5();
	return cosInPhase * outRange + outLow;	
}

template<class Type>
PLONK_INLINE_HIGH Type linexp (Type const& input, 
                    Type const& inLow, Type const& inHigh,
                    Type const& outLow, Type const& outHigh) throw()
{
	const Type outRatio = outHigh / outLow;
	const Type reciprocalInRange = Math<Type>::get1() / (inHigh - inLow);
	const Type inLowOverInRange = reciprocalInRange * inLow;
	return outLow * pow (outRatio, input * reciprocalInRange - inLowOverInRange);	
}

template<class Type>
PLONK_INLINE_HIGH Type linexp2 (Type const& input, 
                     Type const& reciprocalInRange, Type const& inLowOverInRange,
                     Type const& outLow, Type const& outRatio) throw()
{
	return outLow * pow (outRatio, input * reciprocalInRange - inLowOverInRange);	
}

template<class Type>
PLONK_INLINE_HIGH Type linwelch (Type const& input, 
                      Type const& inLow, Type const& inHigh,
                      Type const& outLow, Type const& outHigh) throw()
{
	const Type inRange = inHigh - inLow;
	const Type outRange = outHigh - outLow;
	const Type inPos = (input - inLow) / inRange;
    
	if (outLow < outHigh)
		return outLow + outRange * sin (Math<Type>::getPi_2() * inPos);
	else
		return outHigh - outRange * sin (Math<Type>::getPi_2() - Math<Type>::getPi_2() * inPos);
}

//template<class Type>
//PLONK_INLINE_HIGH Type explin (Type const& input, 
//                    Type const& inLow, Type const& inHigh, 
//                    Type const& outLow, Type const& outHigh)
//{
//    if (input <= inLow) 
//        return outLow;
//    
//    if (input >= inHigh) 
//        return outHigh;
//    
//    return log (input / inLow) / log (inHigh / inLow) * (outHigh - outLow) + outLow;
//}

template<class Type>
PLONK_INLINE_HIGH Type explin (Type const& input, 
                    Type const& inLow, Type const& inHigh, 
                    Type const& outLow, Type const& outHigh)
{    
    const Type clipped = clip (input, inLow, inHigh);
    return log (clipped / inLow) / log (inHigh / inLow) * (outHigh - outLow) + outLow;
}

//------------------------------------------------------------------------------

template<class ValueType, class IndexType, signed Extension, signed Offset> class InterpBase;
template<class ValueType, class IndexType> class InterpNone;
template<class ValueType, class IndexType> class InterpLinear;
template<class ValueType, class IndexType> class InterpLagrange3;

class Interp
{
public:
    enum TypeCode
    {
        None,
        Linear,
        Lagrange3,
        NumTypes
    };
};

template<class ValueType, class IndexType, Interp::TypeCode TypeCode>
class InterpSelect
{
};

template<class ValueType, class IndexType>
class InterpSelect<ValueType, IndexType, Interp::None>
{
public:
    typedef InterpNone<ValueType,IndexType> InterpType;
};

template<class ValueType, class IndexType>
class InterpSelect<ValueType, IndexType, Interp::Linear>
{
public:
    typedef InterpLinear<ValueType,IndexType> InterpType;
};

template<class ValueType, class IndexType>
class InterpSelect<ValueType, IndexType, Interp::Lagrange3>
{
public:
    typedef InterpLagrange3<ValueType,IndexType> InterpType;
};


template<class ValueType, class IndexType, signed Extension, signed Offset>
class InterpBase : public Interp
{
public:
    struct ExtensionBuffer
    {
        ValueType buffer[Extension];
    };

    static PLONK_INLINE_HIGH int getExtension() throw() { return Extension; }
    static PLONK_INLINE_HIGH int getOffset() throw() { return Offset; }
    static PLONK_INLINE_HIGH const IndexType& getExtensionAsIndex() throw() { static const IndexType v (Extension); return v; }
    static PLONK_INLINE_HIGH const IndexType& getOffsetAsIndex() throw() { static const IndexType v (Offset); return v; }    
};

template<class ValueType, class IndexType>
class InterpNone : public InterpBase<ValueType,IndexType,0,0>
{
public:    
    static PLONK_INLINE_HIGH ValueType lookup (const ValueType* table, IndexType const& index) throw()
    {
        return table[int (index)];
    }
};

template<class ValueType, class IndexType>
class InterpLinear : public InterpBase<ValueType,IndexType,1,0>
{
public:    
    static PLONK_INLINE_HIGH ValueType interp (ValueType const& value0, ValueType const& value1, IndexType const& frac) throw()
    {
        return value0 + ValueType ((value1 - value0) * frac);
    }
    
    static PLONK_INLINE_HIGH ValueType lookup (const ValueType* table, IndexType const& index) throw()
    {
        const int index0 = int (index);
        const int index1 = index0 + 1;
        const IndexType frac = index - IndexType (index0);
        return interp (table[index0], table[index1], frac);
    }
};

template<class ValueType>
class InterpLinear<ValueType,int> : public InterpBase<ValueType,int,1,0>
{
public:
    typedef int IndexType;
    
    static PLONK_INLINE_HIGH ValueType interp (ValueType const& value0, ValueType const& value1, IndexType const& frac) throw()
    {
        (void)value1;
        (void)frac;
        return value0;
    }
    
    static PLONK_INLINE_HIGH ValueType lookup (const ValueType* table, IndexType const& index) throw()
    {
        return table[index];
    }
};

template<class ValueType, class IndexType>
class InterpLagrange3 : public InterpBase<ValueType,IndexType,4,1>
{
public:
    static PLONK_INLINE_HIGH ValueType interp (ValueType const& value_1,
                                    ValueType const& value0,
                                    ValueType const& value1,
                                    ValueType const& value2,
                                    IndexType const& frac) throw()
    {        
        const ValueType half = Math<ValueType>::get0_5();
        const ValueType third = Math<ValueType>::get1_3();
        const ValueType sixth = Math<ValueType>::get1_6();
        const ValueType c0 = value0;
        const ValueType c1 = value1 - third * value_1 - half * value0 - sixth * value2;
        const ValueType c2 = half * (value_1 + value1) - value0;
        const ValueType c3 = sixth * (value2 - value_1) + half * (value0 - value1);
        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }
    
    static PLONK_INLINE_HIGH ValueType lookup (const ValueType* table, IndexType const& index) throw()
    {
        const int index0 = int (index);
        const int index1 = index0 + 1;
        const int index_1 = index0 - 1;
        const int index2 = index1 + 1;
        const IndexType frac = index - IndexType (index0);
        return interp (table[index_1], table[index0], table[index1], table[index2], frac);
    }
};

template<class ValueType>
class InterpLagrange3<ValueType,int> : public InterpBase<ValueType,int,4,1>
{
public:
    typedef int IndexType;

    static PLONK_INLINE_HIGH ValueType interp (ValueType const& value_1,
                                    ValueType const& value0,
                                    ValueType const& value1,
                                    ValueType const& value2,
                                    IndexType const& frac) throw()
    {
        (void)value_1;
        (void)value1;
        (void)value2;
        (void)frac;
        return value0;
    }
    
    static PLONK_INLINE_HIGH ValueType lookup (const ValueType* table, IndexType const& index) throw()
    {
        return table[index];
    }
};


template<class Type, signed NumBits>
class BitsBase
{
public:
};


#define PLONK_BITSBASE_DECLARE(TypeCode,TypeSize)\
    template<class Type>\
    class BitsBase<Type,TypeSize>\
    {\
    public:\
        typedef PlankU##TypeCode InternalType;\
        \
        static PLONK_INLINE_HIGH Type countOnes             (Type const& value) throw() { return (Type)pl_CountOnesU##TypeCode ((InternalType)value);          }\
        static PLONK_INLINE_HIGH Type countLeadingZeros     (Type const& value) throw() { return (Type)pl_CountLeadingZerosU##TypeCode ((InternalType)value);  }\
        static PLONK_INLINE_HIGH Type countTrailingZeroes   (Type const& value) throw() { return (Type)pl_CountTrailingZerosU##TypeCode ((InternalType)value); }\
        static PLONK_INLINE_HIGH Type countLeadingOnes      (Type const& value) throw() { return (Type)pl_CountLeadingOnesU##TypeCode ((InternalType)value);   }\
        static PLONK_INLINE_HIGH Type countTrailingOnes     (Type const& value) throw() { return (Type)pl_CountTrailingOnesU##TypeCode ((InternalType)value);  }\
        static PLONK_INLINE_HIGH Type numBitsRequired       (Type const& value) throw() { return (Type)pl_NumBitsRequiredU##TypeCode ((InternalType)value);    }\
        static PLONK_INLINE_HIGH Type nextPowerOf2          (Type const& value) throw() { return (Type)pl_NextPowerOf2U##TypeCode ((InternalType)value);       }\
        static PLONK_INLINE_HIGH bool isPowerOf2            (Type const& value) throw() { return (Type)pl_IsPowerOf2U##TypeCode ((InternalType)value);         }\
    }

PLONK_BITSBASE_DECLARE(C,1);
PLONK_BITSBASE_DECLARE(S,2);
PLONK_BITSBASE_DECLARE(I,4);
PLONK_BITSBASE_DECLARE(LL,8);


/** Some useful bitwise functions. */
class Bits
{
public:
    template<class Type> static PLONK_INLINE_HIGH Type countOnes             (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::countOnes (value);              }
    template<class Type> static PLONK_INLINE_HIGH Type countLeadingZeros     (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::countLeadingZeros (value);      }
    template<class Type> static PLONK_INLINE_HIGH Type countTrailingZeroes   (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::countTrailingZeroes (value);    }
    template<class Type> static PLONK_INLINE_HIGH Type countLeadingOnes      (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::countLeadingOnes (value);       }
    template<class Type> static PLONK_INLINE_HIGH Type countTrailingOnes     (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::countTrailingOnes (value);      }
    template<class Type> static PLONK_INLINE_HIGH Type numBitsRequired       (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::numBitsRequired (value);        }
    template<class Type> static PLONK_INLINE_HIGH Type nextPowerOf2          (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::nextPowerOf2 (value);           }
    template<class Type> static PLONK_INLINE_HIGH bool isPowerOf2            (Type const& value) throw() { return BitsBase<Type,sizeof(Type)>::isPowerOf2 (value);             }
};


/** For rounding values from float/double to int types. */
class NumericalConverter
{
public:
    template<class InType, class OutType>
    static PLONK_INLINE_HIGH void roundCopy (const InType inValue, OutType& outValue) throw()     { outValue = OutType (inValue); }	
    
    static PLONK_INLINE_HIGH void roundCopy (const double inValue, char& outValue) throw()        { outValue = char (inValue + 0.5); }
	static PLONK_INLINE_HIGH void roundCopy (const double inValue, short& outValue) throw()       { outValue = short (inValue + 0.5); }
	static PLONK_INLINE_HIGH void roundCopy (const double inValue, int& outValue) throw()         { outValue = int (inValue + 0.5); }
	static PLONK_INLINE_HIGH void roundCopy (const double inValue, Int24& outValue) throw()       { outValue = Int24 (inValue + 0.5); }
    static PLONK_INLINE_HIGH void roundCopy (const double inValue, LongLong& outValue) throw()    { outValue = LongLong (inValue + 0.5); }    
    
    static PLONK_INLINE_HIGH void roundCopy (const float inValue, char& outValue) throw()         { outValue = char (inValue + 0.5f); }
	static PLONK_INLINE_HIGH void roundCopy (const float inValue, short& outValue) throw()        { outValue = short (inValue + 0.5f); }
	static PLONK_INLINE_HIGH void roundCopy (const float inValue, int& outValue) throw()          { outValue = int (inValue + 0.5f); }
    static PLONK_INLINE_HIGH void roundCopy (const float inValue, Int24& outValue) throw()        { outValue = Int24 (inValue + 0.5f); }
    static PLONK_INLINE_HIGH void roundCopy (const float inValue, LongLong& outValue) throw()     { outValue = LongLong (inValue + 0.5f); }
};



/// @}

#endif // PLONK_INLINEMISCOPS_H