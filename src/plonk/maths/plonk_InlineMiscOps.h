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

#ifndef PLONK_INLINEMISCOPS_H
#define PLONK_INLINEMISCOPS_H

#include "plonk_Constants.h"
#include "plonk_InlineUnaryOps.h"

#define PLONK_TERNARYOPFUNCTION(TYPE,NAME)       TYPE (*NAME)(TYPE const&, TYPE const&, TYPE const&)
#define PLONK_QUATERNARYOPFUNCTION(TYPE,NAME)    TYPE (*NAME)(TYPE const&, TYPE const&, TYPE const&, TYPE const&)
#define PLONK_QUINARYOPFUNCTION(TYPE,NAME)       TYPE (*NAME)(TYPE const&, TYPE const&, TYPE const&, TYPE const&, TYPE const&)

//------------------------------------------------------------------------------

/** Miscellaneous arithmetic functions.
 
 @defgroup PlonkMiscArithmeticFunctions Miscellaneous arithmetic functions 
 @ingroup PlonkOtherUserFunctions
 @{
 */

template<class Type> inline Type clip (Type const& value, Type const& lower, Type const& upper) throw() { return min (max (lower, value), upper); }

inline int wrap (int value, int lower, int upper) throw()
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
inline Type wrap (Type const& value, Type const& lower, Type const& upper) throw()
{
	Type range;
    
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
inline Type linlin (Type const& input, 
                    Type const& inLow, Type const& inHigh,
                    Type const& outLow, Type const& outHigh) throw()
{
	Type inRange = inHigh - inLow;
	Type outRange = outHigh - outLow;
	return (input - inLow) * outRange / inRange + outLow;
}

template<class Type>
inline Type linlin2 (Type const& input, 
                     Type const& inLow, Type const& inRange,
                     Type const& outLow, Type const& outRange) throw()
{
	return (input - inLow) * outRange / inRange + outLow;
}

template<class Type>
inline Type linsin (Type const& input, 
                    Type const& inLow, Type const& inHigh,
                    Type const& outLow, Type const& outHigh) throw()
{
	Type inRange = inHigh - inLow;
	Type outRange = outHigh - outLow;
    
	Type inPhase = (input - inLow) * Math<Type>::getPi() / inRange + Math<Type>::getPi();
	Type cosInPhase = cos (inPhase) * Math<Type>::get0_5() + Math<Type>::get0_5();
	
	return cosInPhase * outRange + outLow;	
}

template<class Type>
inline Type linsin2 (Type const& input, 
                     Type const& inLow, Type const& inRange,
                     Type const& outLow, Type const& outRange) throw()
{	
	Type inPhase = (input - inLow) * Math<Type>::getPi() / inRange + Math<Type>::getPi();
	Type cosInPhase = cos (inPhase) * Math<Type>::get0_5() + Math<Type>::get0_5();
	return cosInPhase * outRange + outLow;	
}

template<class Type>
inline Type linexp (Type const& input, 
                    Type const& inLow, Type const& inHigh,
                    Type const& outLow, Type const& outHigh) throw()
{
	Type outRatio = outHigh / outLow;
	Type reciprocalInRange = Math<Type>::get1() / (inHigh - inLow);
	Type inLowOverInRange = reciprocalInRange * inLow;
	return outLow * pow (outRatio, input * reciprocalInRange - inLowOverInRange);	
}

template<class Type>
inline Type linexp2 (Type const& input, 
                     Type const& reciprocalInRange, Type const& inLowOverInRange,
                     Type const& outLow, Type const& outRatio) throw()
{
	return outLow * pow (outRatio, input * reciprocalInRange - inLowOverInRange);	
}

template<class Type>
inline Type linwelch (Type const& input, 
                      Type const& inLow, Type const& inHigh,
                      Type const& outLow, Type const& outHigh) throw()
{
	Type inRange = inHigh - inLow;
	Type outRange = outHigh - outLow;
	Type inPos = (input - inLow) / inRange;
    
	if (outLow < outHigh)
		return outLow + outRange * sin (Math<Type>::getPi_2() * inPos);
	else
		return outHigh - outRange * sin (Math<Type>::getPi_2() - Math<Type>::getPi_2() * inPos);
}

//template<class Type>
//inline Type explin (Type const& input, 
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
inline Type explin (Type const& input, 
                    Type const& inLow, Type const& inHigh, 
                    Type const& outLow, Type const& outHigh)
{    
    Type clipped = clip (input, inLow, inHigh);
    return log (clipped / inLow) / log (inHigh / inLow) * (outHigh - outLow) + outLow;
}

//------------------------------------------------------------------------------

template<class ValueType, class IndexType>
class InterpBase
{
};

template<class ValueType, class IndexType>
class InterpLinear
{
public:
    static inline ValueType interp (ValueType const& value0, ValueType const& value1, IndexType const& frac) throw()
    {
        return value0 + ValueType (frac * (value1 - value0));
    }
    
    static inline ValueType lookup (const ValueType* table, IndexType const& index) throw()
    {
        const int index0 = int (index);
        const int index1 = index0 + 1;
        const IndexType frac = index - IndexType (index0);
        return interp (table[index0], table[index1], frac);
    }
};

template<class ValueType>
class InterpLinear<ValueType,int>
{
public:
    typedef int IndexType;
    
    static inline ValueType interp (ValueType const& value0, ValueType const& value1, IndexType const& frac) throw()
    {
        (void)value1;
        (void)frac;
        return value0;
    }
    
    static inline ValueType lookup (const ValueType* table, IndexType const& index) throw()
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
        static inline Type countOnes             (Type const& value) throw() { return (Type)pl_CountOnesU##TypeCode ((InternalType)value);          }\
        static inline Type countLeadingZeros     (Type const& value) throw() { return (Type)pl_CountLeadingZerosU##TypeCode ((InternalType)value);  }\
        static inline Type countTrailingZeroes   (Type const& value) throw() { return (Type)pl_CountTrailingZerosU##TypeCode ((InternalType)value); }\
        static inline Type countLeadingOnes      (Type const& value) throw() { return (Type)pl_CountLeadingOnesU##TypeCode ((InternalType)value);   }\
        static inline Type countTrailingOnes     (Type const& value) throw() { return (Type)pl_CountTrailingOnesU##TypeCode ((InternalType)value);  }\
        static inline Type numBitsRequired       (Type const& value) throw() { return (Type)pl_NumBitsRequiredU##TypeCode ((InternalType)value);    }\
        static inline Type nextPowerOf2          (Type const& value) throw() { return (Type)pl_NextPowerOf2U##TypeCode ((InternalType)value);       }\
        static inline bool isPowerOf2            (Type const& value) throw() { return (Type)pl_IsPowerOf2U##TypeCode ((InternalType)value);         }\
    }

PLONK_BITSBASE_DECLARE(C,1);
PLONK_BITSBASE_DECLARE(S,2);
PLONK_BITSBASE_DECLARE(I,4);
PLONK_BITSBASE_DECLARE(LL,8);

/** Some useful bitwise functions. */
template<class Type>
class Bits : public BitsBase<Type,sizeof(Type)>
{
public:
    typedef BitsBase<Type,sizeof(Type)> Base;
    static inline Type countOnes             (Type const& value) throw() { return Base::countOnes (value);              }
    static inline Type countLeadingZeros     (Type const& value) throw() { return Base::countLeadingZeros (value);      }
    static inline Type countTrailingZeroes   (Type const& value) throw() { return Base::countTrailingZeroes (value);    }
    static inline Type countLeadingOnes      (Type const& value) throw() { return Base::countLeadingOnes (value);       }
    static inline Type countTrailingOnes     (Type const& value) throw() { return Base::countTrailingOnes (value);      }
    static inline Type numBitsRequired       (Type const& value) throw() { return Base::numBitsRequired (value);        }
    static inline Type nextPowerOf2          (Type const& value) throw() { return Base::nextPowerOf2 (value);           }
    static inline bool isPowerOf2            (Type const& value) throw() { return Base::isPowerOf2 (value);             }
};


//template<class ValueType, class IndexType>
//inline ValueType lininterp (ValueType const& value0, ValueType const& value1, IndexType const& frac) throw()
//{
//    return value0 + ValueType (frac * (value1 - value0));
//}
//
//template<class ValueType, class IndexType>
//inline ValueType lookup (const ValueType* table, IndexType const& index) throw()
//{
//    const int index0 = int (index);
//    const int index1 = index0 + 1;
//    const IndexType frac = index - IndexType (index0);
//    return lininterp (table[index0], table[index1], frac);
//}

/// @}

#endif // PLONK_INLINEMISCOPS_H