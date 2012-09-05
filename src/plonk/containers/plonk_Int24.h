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

#ifndef PLONK_INT24_H
#define PLONK_INT24_H

#include "../core/plonk_CoreForwardDeclarations.h"

#if PLONK_WIN
    #pragma pack (push, 1)
    #define PLONK_PACKED
#elif PLONK_GCC || PLONK_LLVM || PLANK_APPLE_LLVM
    #define PLONK_PACKED __attribute__((packed))
#else
    #warning Data packing macros haven't been handled for this build system
#endif

/** An emulated 24-bit integer type.
 This overloads all the required operators and function to behave like
 an integer type but uses only 24 bits of storage.
 @ingroup PlonkContainerClasses
 */
class Int24
{
public:
    typedef PlankI24 Internal;
    
    Int24() throw();
    Int24 (const char value) throw();
    Int24 (const short value) throw();
    Int24 (const int value) throw();
    Int24 (const LongLong value) throw();
    Int24 (const float value) throw();
    Int24 (const double value) throw();

    Int24 (const Internal value) throw();
    
    template<class OtherType>
    Int24 (const OtherType value) throw()
    :   data (pl_ConvertIToI24 (int (value)))
    {
    }
    
    Int24 (Int24 const& copy) throw();
    Int24& operator= (Int24 const& other) throw();
    
    template<class OtherType>
    Int24& operator= (const OtherType other) throw()
    {
        data = pl_ConvertIToI24 (int (other));
        return *this;        
    }

    operator char() const throw();
    operator short() const throw();
    operator int() const throw();
    operator LongLong() const throw();
    operator float() const throw();
    operator double() const throw();
    operator Internal() const throw();
    
    int toInt() const throw();
    float toFloat() const throw();
    double toDouble() const throw();
    Internal getRaw() const throw();
    
    Int24 operator~() const throw();
    Int24 operator-() const throw();
    Int24& operator++() throw();
    Int24& operator--() throw();
    Int24 operator++ (int) throw();
    Int24 operator-- (int) throw();
    
    Int24 operator+ (Int24 const& rightOperand) const throw();
    Int24 operator- (Int24 const& rightOperand) const throw();
    Int24 operator* (Int24 const& rightOperand) const throw();
    Int24 operator/ (Int24 const& rightOperand) const throw();
    Int24 operator% (Int24 const& rightOperand) const throw();
    Int24 operator| (Int24 const& rightOperand) const throw();
    Int24 operator& (Int24 const& rightOperand) const throw();
    Int24 operator^ (Int24 const& rightOperand) const throw();

    Int24 pow (Int24 const& rightOperand) const throw();
    
    Int24& operator+= (Int24 const& rightOperand) throw();
    Int24& operator-= (Int24 const& rightOperand) throw();
    Int24& operator*= (Int24 const& rightOperand) throw();
    Int24& operator/= (Int24 const& rightOperand) throw();
    Int24& operator%= (Int24 const& rightOperand) throw();
    Int24& operator|= (Int24 const& rightOperand) throw();
    Int24& operator&= (Int24 const& rightOperand) throw();
    Int24& operator^= (Int24 const& rightOperand) throw();
    
    bool operator== (Int24 const& rightOperand) const throw();
    bool operator!= (Int24 const& rightOperand) const throw();
    bool operator< (Int24 const& rightOperand) const throw();
    bool operator<= (Int24 const& rightOperand) const throw();
    bool operator> (Int24 const& rightOperand) const throw();
    bool operator>= (Int24 const& rightOperand) const throw();

    bool operator&& (Int24 const& rightOperand) const throw();
    bool operator|| (Int24 const& rightOperand) const throw();

    Int24 operator<< (const int shift) const throw();
    Int24 operator>> (const int shift) const throw();
    Int24& operator<<= (const int shift) throw();
    Int24& operator>>= (const int shift) throw();
    
    friend std::istream& operator>> (std::istream &inputStream, Int24& value);
    friend std::ostream& operator<< (std::ostream &outputStream, Int24 const& value);
    
private:
    Internal data;
} PLONK_PACKED;

//template<class OtherType>
//const typename BinaryOpTypeUtility<OtherType,Int24>::CalcType operator+ (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    typedef typename BinaryOpTypeUtility<OtherType,Int24>::CalcType CalcType;
//    return CalcType (leftOperand) + CalcType (rightOperand);
//}
//
//template<class OtherType>
//const typename BinaryOpTypeUtility<OtherType,Int24>::CalcType operator- (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    typedef typename BinaryOpTypeUtility<OtherType,Int24>::CalcType CalcType;
//    return CalcType (leftOperand) - CalcType (rightOperand);
//}
//
//template<class OtherType>
//const typename BinaryOpTypeUtility<OtherType,Int24>::CalcType operator* (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    typedef typename BinaryOpTypeUtility<OtherType,Int24>::CalcType CalcType;
//    return CalcType (leftOperand) * CalcType (rightOperand);
//}
//
//template<class OtherType>
//const typename BinaryOpTypeUtility<OtherType,Int24>::CalcType operator/ (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    typedef typename BinaryOpTypeUtility<OtherType,Int24>::CalcType CalcType;
//    return CalcType (leftOperand) / CalcType (rightOperand);
//}
//
//template<class OtherType>
//const typename BinaryOpTypeUtility<OtherType,Int24>::CalcType operator% (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    typedef typename BinaryOpTypeUtility<OtherType,Int24>::CalcType CalcType;
//    return CalcType (leftOperand) % CalcType (rightOperand);
//}
//
//template<class OtherType>
//const Int24 operator| (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    return Int24 (LongLong (leftOperand) | LongLong (rightOperand));
//}
//
//template<class OtherType>
//const Int24 operator& (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    return Int24 (LongLong (leftOperand) & LongLong (rightOperand));
//}
//
//template<class OtherType>
//const Int24 operator^ (OtherType const& leftOperand, Int24 const& rightOperand) throw()
//{
//    return Int24 (LongLong (leftOperand) ^ LongLong (rightOperand));
//}



#if PLANK_WIN
    #pragma pack (pop)
#endif

#undef PLONK_PACKED

std::istream& operator>> (std::istream &inputStream, Int24& value);
std::ostream& operator<< (std::ostream &outputStream, Int24 const& value);


#endif // PLONK_INT24_H
