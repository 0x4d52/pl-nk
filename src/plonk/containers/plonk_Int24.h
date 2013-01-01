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
    int toShort() const throw();
    float toFloat() const throw();
    double toDouble() const throw();
    inline const Internal& getRaw() const throw() { return data; }
    
    Int24 operator~() const throw();
    Int24 operator-() const throw();
    Int24& operator++() throw();
    Int24& operator--() throw();
    Int24 operator++ (int) throw();
    Int24 operator-- (int) throw();
    
    int operator+ (Int24 const& rightOperand) const throw();
    int operator- (Int24 const& rightOperand) const throw();
    int operator* (Int24 const& rightOperand) const throw();
    int operator/ (Int24 const& rightOperand) const throw();
    Int24 operator% (Int24 const& rightOperand) const throw();
    Int24 operator| (Int24 const& rightOperand) const throw();
    Int24 operator& (Int24 const& rightOperand) const throw();
    Int24 operator^ (Int24 const& rightOperand) const throw();
    
    float operator+ (const float rightOperand) const throw();
    float operator- (const float rightOperand) const throw();
    float operator* (const float rightOperand) const throw();
    float operator/ (const float rightOperand) const throw();

    double operator+ (double const& rightOperand) const throw();
    double operator- (double const& rightOperand) const throw();
    double operator* (double const& rightOperand) const throw();
    double operator/ (double const& rightOperand) const throw();

    int operator+ (const int rightOperand) const throw();
    int operator- (const int rightOperand) const throw();
    int operator* (const int rightOperand) const throw();
    int operator/ (const int rightOperand) const throw();

    long operator+ (const long rightOperand) const throw();
    long operator- (const long rightOperand) const throw();
    long operator* (const long rightOperand) const throw();
    long operator/ (const long rightOperand) const throw();

    int operator+ (const short rightOperand) const throw();
    int operator- (const short rightOperand) const throw();
    int operator* (const short rightOperand) const throw();
    int operator/ (const short rightOperand) const throw();
    

    LongLong operator+ (LongLong const& rightOperand) const throw();
    LongLong operator- (LongLong const& rightOperand) const throw();
    LongLong operator* (LongLong const& rightOperand) const throw();
    LongLong operator/ (LongLong const& rightOperand) const throw();

    Int24 pow (Int24 const& rightOperand) const throw();
    
    Int24& operator+= (Int24 const& rightOperand) throw();
    Int24& operator-= (Int24 const& rightOperand) throw();
    Int24& operator*= (Int24 const& rightOperand) throw();
    Int24& operator/= (Int24 const& rightOperand) throw();
    Int24& operator%= (Int24 const& rightOperand) throw();
    Int24& operator|= (Int24 const& rightOperand) throw();
    Int24& operator&= (Int24 const& rightOperand) throw();
    Int24& operator^= (Int24 const& rightOperand) throw();
    
    Int24& operator+= (const float rightOperand) throw();
    Int24& operator-= (const float rightOperand) throw();
    Int24& operator*= (const float rightOperand) throw();
    Int24& operator/= (const float rightOperand) throw();

    Int24& operator+= (double const& rightOperand) throw();
    Int24& operator-= (double const& rightOperand) throw();
    Int24& operator*= (double const& rightOperand) throw();
    Int24& operator/= (double const& rightOperand) throw();

    Int24& operator+= (const short rightOperand) throw();
    Int24& operator-= (const short rightOperand) throw();
    Int24& operator*= (const short rightOperand) throw();
    Int24& operator/= (const short rightOperand) throw();

    Int24& operator+= (const int rightOperand) throw();
    Int24& operator-= (const int rightOperand) throw();
    Int24& operator*= (const int rightOperand) throw();
    Int24& operator/= (const int rightOperand) throw();

    Int24& operator+= (const long rightOperand) throw();
    Int24& operator-= (const long rightOperand) throw();
    Int24& operator*= (const long rightOperand) throw();
    Int24& operator/= (const long rightOperand) throw();
    
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

#if PLANK_WIN
#pragma pack (pop)
#endif

#undef PLONK_PACKED

template<> class BinaryOpTypeUtility<double,Int24>   { public: typedef double CalcType; };
template<> class BinaryOpTypeUtility<Int24,double>   { public: typedef double CalcType; };
template<> class BinaryOpTypeUtility<char,Int24>     { public: typedef int CalcType; };
template<> class BinaryOpTypeUtility<Int24,char>     { public: typedef int CalcType; };
template<> class BinaryOpTypeUtility<short,Int24>    { public: typedef LongLong CalcType; };
template<> class BinaryOpTypeUtility<Int24,short>    { public: typedef LongLong CalcType; };
template<> class BinaryOpTypeUtility<Int24,LongLong> { public: typedef LongLong CalcType; };
template<> class BinaryOpTypeUtility<LongLong,Int24> { public: typedef LongLong CalcType; };
template<> class BinaryOpTypeUtility<Int24,Int24>    { public: typedef LongLong CalcType; };

template<class OtherType>
class Int24Ops
{
public:
    typedef typename BinaryOpTypeUtility<OtherType,Int24>::CalcType CalcType;
    typedef CalcType ReturnType;

    static inline ReturnType addop (OtherType const& leftOperand, Int24 const& rightOperand) throw()
    {
        return CalcType (leftOperand) + CalcType (rightOperand);
    }
    
    static inline ReturnType subop (OtherType const& leftOperand, Int24 const& rightOperand) throw()
    {
        return CalcType (leftOperand) - CalcType (rightOperand);
    }

    static inline ReturnType mulop (OtherType const& leftOperand, Int24 const& rightOperand) throw()
    {
        return CalcType (leftOperand) * CalcType (rightOperand);
    }

    static inline ReturnType divop (OtherType const& leftOperand, Int24 const& rightOperand) throw()
    {
        return CalcType (leftOperand) / CalcType (rightOperand);
    }

};

std::istream& operator>> (std::istream &inputStream, Int24& value);
std::ostream& operator<< (std::ostream &outputStream, Int24 const& value);


float operator+ (const float leftOperand, Int24 const& rightOperand) throw();
float operator- (const float leftOperand, Int24 const& rightOperand) throw();
float operator* (const float leftOperand, Int24 const& rightOperand) throw();
float operator/ (const float leftOperand, Int24 const& rightOperand) throw();

double operator+ (double const& leftOperand, Int24 const& rightOperand) throw();
double operator- (double const& leftOperand, Int24 const& rightOperand) throw();
double operator* (double const& leftOperand, Int24 const& rightOperand) throw();
double operator/ (double const& leftOperand, Int24 const& rightOperand) throw();

int operator+ (const int leftOperand, Int24 const& rightOperand) throw();
int operator- (const int leftOperand, Int24 const& rightOperand) throw();
int operator* (const int leftOperand, Int24 const& rightOperand) throw();
int operator/ (const int leftOperand, Int24 const& rightOperand) throw();

int operator+ (const short leftOperand, Int24 const& rightOperand) throw();
int operator- (const short leftOperand, Int24 const& rightOperand) throw();
int operator* (const short leftOperand, Int24 const& rightOperand) throw();
int operator/ (const short leftOperand, Int24 const& rightOperand) throw();

long operator+ (const long leftOperand, Int24 const& rightOperand) throw();
long operator- (const long leftOperand, Int24 const& rightOperand) throw();
long operator* (const long leftOperand, Int24 const& rightOperand) throw();
long operator/ (const long leftOperand, Int24 const& rightOperand) throw();

LongLong operator+ (LongLong const& leftOperand, Int24 const& rightOperand) throw();
LongLong operator- (LongLong const& leftOperand, Int24 const& rightOperand) throw();
LongLong operator* (LongLong const& leftOperand, Int24 const& rightOperand) throw();
LongLong operator/ (LongLong const& leftOperand, Int24 const& rightOperand) throw();



#endif // PLONK_INT24_H
