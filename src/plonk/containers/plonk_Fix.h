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

/* Algorithm from libfixmath - see doc/license.txt included in the distribution */

#ifndef PLONK_FIXED_H
#define PLONK_FIXED_H

#include "../core/plonk_CoreForwardDeclarations.h"

template<class Base>
class FixBase
{
private:
    FixBase(); // to prevent using unsupported Base internal types (must be char, short or int).
};

// supported Base type specialisations
template<> class FixBase<char>        { public: FixBase(){} };
template<> class FixBase<short>       { public: FixBase(){} };
template<> class FixBase<int>         { public: FixBase(){} };

/** A generic fixed point numerical class. 
 This does not detect overflows, and does not use saturating arithmetic. */
template<class Base, unsigned IBits, unsigned FBits>                
class Fix : public FixBase<Base>
{
public:
    typedef Math<Fix> MathType;
    typedef typename TypeUtility<Base>::UnsignedType UnsignedBase;
    typedef typename TypeUtility<Base>::WideType WideBase;
    typedef typename TypeUtility<Base>::UnsignedWideType UnsignedWideBase;
    
    union UnsignedConversion
    {
        Base b;
        UnsignedBase u;
    };
    
    class Internal
    {
    public:
        inline Internal (Base const& value) throw() 
        :   internal (value)
        {
        }
        
        Base internal;
        
    private:
        Internal();
    };
            
    Fix() throw()
    {
    }
    
    inline Fix (const char value) throw()
    :   internal (value * getOne().internal)
    {
    }
    
    inline Fix (const short value) throw()
    :   internal (value * getOne().internal)
    {
    }
    
    inline Fix (const int value) throw()
    :   internal (value * getOne().internal)
    {
    }
    
    inline Fix (const float value) throw()
    :   internal (value * getOneFloat())
    {
    }
    
    inline Fix (const double value) throw()
    :   internal (value * getOneDouble())
    {
    }
    
    inline explicit Fix (Internal const& value) throw()
    :   internal (value.internal)
    {
    }
    
    inline ~Fix()
    {
        plonk_staticassert ((sizeof (Base) * 8) == (IBits + FBits));
    }

    inline operator int () const throw()
    {
        return internal >> FBits;
    }
    
    inline operator float () const throw()
    {
        return float (internal) / getOneFloat();
    }

    inline operator double () const throw()
    {
        return double (internal) / getOneDouble();
    }
    
    inline Fix operator+ (Fix const& rightOperand) const throw()    { return addop (*this, rightOperand); }
    inline Fix operator- (Fix const& rightOperand) const throw()    { return subop (*this, rightOperand); }
    inline Fix operator* (Fix const& rightOperand) const throw()    { return mulop (*this, rightOperand); }
    inline Fix operator/ (Fix const& rightOperand) const throw()    { return divop (*this, rightOperand); }
    inline Fix operator+ (float const& rightOperand) const throw()  { return addop (*this, Fix (rightOperand)); }
    inline Fix operator- (float const& rightOperand) const throw()  { return subop (*this, Fix (rightOperand)); }
    inline Fix operator* (float const& rightOperand) const throw()  { return mulop (*this, Fix (rightOperand)); }
    inline Fix operator/ (float const& rightOperand) const throw()  { return divop (*this, Fix (rightOperand)); }
    inline Fix operator+ (double const& rightOperand) const throw() { return addop (*this, Fix (rightOperand)); }
    inline Fix operator- (double const& rightOperand) const throw() { return subop (*this, Fix (rightOperand)); }
    inline Fix operator* (double const& rightOperand) const throw() { return mulop (*this, Fix (rightOperand)); }
    inline Fix operator/ (double const& rightOperand) const throw() { return divop (*this, Fix (rightOperand)); }
    inline Fix operator+ (int const& rightOperand) const throw()    { return addop (*this, Fix (rightOperand)); }
    inline Fix operator- (int const& rightOperand) const throw()    { return subop (*this, Fix (rightOperand)); }
    inline Fix operator* (int const& rightOperand) const throw()    { return mulop (*this, Fix (rightOperand)); }
    inline Fix operator/ (int const& rightOperand) const throw()    { return divop (*this, Fix (rightOperand)); }

    inline Fix& operator+= (Fix const& rightOperand) throw()    { return operator=  ( (*this) + rightOperand); }
    inline Fix& operator-= (Fix const& rightOperand) throw()    { return operator=  ( (*this) - rightOperand); }
    inline Fix& operator*= (Fix const& rightOperand) throw()    { return operator=  ( (*this) * rightOperand); }
    inline Fix& operator/= (Fix const& rightOperand) throw()    { return operator=  ( (*this) / rightOperand); }
    inline Fix& operator+= (float const& rightOperand) throw()  { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (float const& rightOperand) throw()  { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (float const& rightOperand) throw()  { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (float const& rightOperand) throw()  { return operator=  ( (*this) / Fix (rightOperand)); }
    inline Fix& operator+= (double const& rightOperand) throw() { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (double const& rightOperand) throw() { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (double const& rightOperand) throw() { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (double const& rightOperand) throw() { return operator=  ( (*this) / Fix (rightOperand)); }
    inline Fix& operator+= (int const& rightOperand) throw()    { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (int const& rightOperand) throw()    { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (int const& rightOperand) throw()    { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (int const& rightOperand) throw()    { return operator=  ( (*this) / Fix (rightOperand)); }

    
    inline static const Fix& getOne() throw()
    {
        static Fix v (Internal (Base (1) << FBits));
        return v;
    }
        
    inline static const Fix& getMinimum() throw()
    {
        static Fix v (Internal (Base (1) << (IBits + FBits - 1)));
        return v;
    }
    
    inline static const Fix& getMaximum() throw()
    {
        static Fix v (Internal (~(Base (1) << (IBits + FBits - 1))));
        return v;
    }
    
    inline static const Base& getIMask() throw()
    {
        static Base v (getIMaskInternal());
        return v;
    }

    inline static const Base& getFMask() throw()
    {
        static Base v (getFMaskInternal());
        return v;
    }
    
    inline const Base& getRaw() const throw() { return internal; }
    
private:
    Base internal;
    
    inline static const float& getOneFloat() throw()
    {
        static float v (float (getOne().internal));
        return v;
    }
    
    inline static const double& getOneDouble() throw()
    {
        static double v (double (getOne().internal));
        return v;
    }
    
    inline static Base getIMaskInternal() throw()
    {
        Base v;
        
        for (int i = 0; i < IBits; ++i)
            v = (v << 1) | 1;
        
        return v << FBits;
    }
        
    inline static Base getFMaskInternal() throw()
    {
        Base v;
        
        for (int i = 0; i < FBits; ++i)
            v = (v << 1) | 1;
        
        return v;
    }

};


// unary ops

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> move (Fix<Base,IBits,FBits> const& a) throw()             
{ 
    return a; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> neg (Fix<Base,IBits,FBits> const& a) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    return FixType (Internal (-a.getRaw())); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> abs (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    const Base internal (a.getRaw());
    return FixType (internal < 0 ? Internal (-internal) : Internal (internal)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> reciprocal (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;

    return FixType::getOne() / a; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> log2 (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sin (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> cos (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> tan (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> asin (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> acos (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> atan (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sinh (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> cosh (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> tanh (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sqrt (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> log (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> log10 (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> exp (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    
    const Base raw1 (FixType::getOne().getRaw());
    Base araw (a.getRaw());
    
    if(araw == 0) 
        return FixType::getOne();
    
	if(araw == raw1) 
        return Math<FixType>::getE();
    
//	if(a >= 681391   ) return fix16_maximum;
//	if(a <= -772243  ) return 0;
    
	bool neg (araw < 0);
	
    if (neg) 
        araw = -araw;
    
	Base result (araw + raw1);
	FixType term = FixType (Internal (araw));
    
	int i;        
	for (i = 2; i < (IBits + FBits - 2); ++i)
	{
		term *= FixType (Internal (araw)) / FixType (i);
        
        const Base rawterm (term.getRaw());
		result += rawterm;
        
		if ((rawterm < 500) && ((i > (FBits - 1)) || (rawterm < (FBits + 4))))
			break;
	}
    
	return neg ? reciprocal (FixType (Internal (result))) : FixType (Internal (result));
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> squared (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> cubed (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> floor (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    return FixType (Internal (a.getRaw() & FixType::getIMask().getRaw())); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> ceil (Fix<Base,IBits,FBits> const& a) throw()             
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    const Base internal (a.getRaw());
    const Base imask (FixType::getIMask().getRaw());
    const Base fmask (FixType::getFMask().getRaw());
    const Base one (FixType::getOne().getRaw());
    return FixType (Internal ((internal & imask) + (internal & fmask ? one : 0))); 
}


template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> frac (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sign (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> m2f (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> f2m (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> a2dB (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> dB2a (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> d2r (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> r2d (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> distort (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return 0; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> zap (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    return a; 
}

// binary ops

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> addop (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    return FixType (Internal (a.getRaw() + b.getRaw())); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> subop (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    return FixType (Internal (a.getRaw() - b.getRaw())); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> mulop (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    typedef typename FixType::WideBase WideBase;

    const WideBase product = WideBase (a.getRaw()) * WideBase (b.getRaw());
    return FixType (Internal (product >> FBits)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> divop (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    typedef typename FixType::WideBase WideBase;
    
    return FixType (Internal ((WideBase (a.getRaw()) << FBits) / WideBase (b.getRaw()))); 
}


//template<class Base, unsigned IBits, unsigned FBits> 
//inline Fix<Base,IBits,FBits> divop (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
//{ 
//    typedef Fix<Base,IBits,FBits> FixType;
//    typedef typename FixType::Internal Internal;
//    typedef typename FixType::WideBase WideType;
//    typedef typename FixType::UnsignedBase UnsignedType;
//    typedef typename FixType::UnsignedWideBase UnsignedWideType;
//    
//    const Base araw (a.getRaw());
//    const Base braw (b.getRaw());
//    const Base imask (FixType::getIMask());
//    const Base dmask (imask << 4);
//    const Base signmask (FixType::getMinimum().getRaw());
//
//    if (braw == 0)
//        return FixType::getMinimum();
//	
//	UnsignedType remainder = (araw >= 0) ? araw : (-araw);
//	UnsignedType divider = (braw >= 0) ? braw : (-braw);
//	UnsignedType quotient = 0;
//	int bitPos = FBits + 1; // unsigned?
//	
//    // Kick-start the division a bit.
//	// This improves speed in the worst-case scenarios where N and D are large
//	// It gets a lower estimate for the result by N/(D >> 17 + 1).
//	if (divider & dmask)
//	{
//		UnsignedType shifted_div = ((divider >> bitPos) + 1);
//		quotient = remainder / shifted_div;
//		remainder -= (UnsignedWideType (quotient) * divider) >> bitPos;
//	}
//	
//	// If the divider is divisible by 2^n, take advantage of it.
//	while (!(divider & 0xF) && bitPos >= 4)
//	{
//		divider >>= 4;
//		bitPos -= 4;
//	}
//	
//	while (remainder && bitPos >= 0)
//	{
//		// Shift remainder as much as we can without overflowing
//		int shift = Bits::countLeadingZeros (remainder);
//		
//        if (shift > bitPos) 
//            shift = bitPos;
//        
//		remainder <<= shift;
//		bitPos -= shift;
//		
//		UnsignedType div = remainder / divider;
//		remainder = remainder % divider;
//		quotient += div << bitPos;
//        		
//		remainder <<= 1;
//		bitPos--;
//	}
//		
//	Base result = quotient >> 1;
//	
//	// Figure out the sign of the result
//	if ((araw ^ braw) & signmask)
//		result = -result;
//	
//	return FixType (Internal (result));
//}


typedef Fix<Char,6,2> FixI6F2;
typedef Fix<Short,8,8> FixI8F8;
typedef Fix<Int,16,16> FixI16F16;
typedef Fix<Int,8,24> FixI8F24;


/*
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
*/


#endif // PLONK_FIXED_H
