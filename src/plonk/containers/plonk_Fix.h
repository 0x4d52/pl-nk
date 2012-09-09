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

/* Algorithms from libfixmath and Fixed Point Math Library
 - see doc/license.txt included in the distribution */

#ifndef PLONK_FIX_H
#define PLONK_FIX_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

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

template<class DstBase, class SrcBase>
class FixBaseConvert
{
    // prevent unsupported conversions
};

// supported conversions
template<> class FixBaseConvert<char,char>      { public: typedef char  ConvertBase; };
template<> class FixBaseConvert<char,short>     { public: typedef short ConvertBase; };
template<> class FixBaseConvert<char,int>       { public: typedef int   ConvertBase; };
template<> class FixBaseConvert<short,short>    { public: typedef short ConvertBase; };
template<> class FixBaseConvert<short,char>     { public: typedef short ConvertBase; };
template<> class FixBaseConvert<short,int>      { public: typedef int   ConvertBase; };
template<> class FixBaseConvert<int,int>        { public: typedef int   ConvertBase; };
template<> class FixBaseConvert<int,char>       { public: typedef int   ConvertBase; };
template<> class FixBaseConvert<int,short>      { public: typedef int   ConvertBase; };

/** A generic fixed point numerical class. 
 This does not detect overflows, and does not use saturating arithmetic. */
template<class Base, unsigned IBits, unsigned FBits>                
class Fix : public FixBase<Base>
{
public:
    typedef Fix<Base,IBits,FBits> FixType;
    typedef Math<FixType> MathType;
    typedef Base BaseType;
    typedef typename TypeUtility<Base>::UnsignedType UnsignedBase;
    typedef typename TypeUtility<Base>::WideType WideBase;
    typedef typename TypeUtility<Base>::UnsignedWideType UnsignedWideBase;
            
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
    
    inline Fix (double const& value) throw()
    :   internal (value * getOneDouble())
    {
    }
    
    inline explicit Fix (Internal const& value) throw()
    :   internal (value.internal)
    {
    }
    
    inline Fix (Fix const& copy) throw()
    :   internal (copy.internal)
    {
    }
    
    template<class BaseOther, unsigned IBitsOther, unsigned FBitsOther>
    inline Fix (Fix<BaseOther,IBitsOther,FBitsOther> const& other) throw()
    :   internal (Base (convert (other) << convertShift (FBits, FBitsOther)))
    {
    }
            
    inline ~Fix()
    {
        plonk_staticassert (IBits != 0);
        plonk_staticassert (FBits != 0);
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
    
    template<class BaseOther, unsigned IBitsOther, unsigned FBitsOther>
    inline operator Fix<BaseOther,IBitsOther,FBitsOther> () const throw()
    {
        return Fix<BaseOther,IBitsOther,FBitsOther> (*this);
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

    inline Fix& operator= (Fix const& other) throw()            { if (this != &other) internal = other.internal; return *this; }
    inline Fix& operator= (float const& other) throw()          { internal = Fix (other).internal; return *this; }
    inline Fix& operator= (double const& other) throw()         { internal = Fix (other).internal; return *this; }
    inline Fix& operator= (int const& other) throw()            { internal = Fix (other).internal; return *this; }
    inline Fix& operator= (Internal const& other) throw()       { internal = other; return *this; }
    
    template<class BaseOther, unsigned IBitsOther, unsigned FBitsOther>
    inline Fix& operator= (Fix<BaseOther,IBitsOther,FBitsOther> const& other) throw()
    { 
        const Fix copy (other);
        return operator= (copy); 
    }
    
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
    inline Fix& operator+= (Internal const& rightOperand) throw()    { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (Internal const& rightOperand) throw()    { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (Internal const& rightOperand) throw()    { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (Internal const& rightOperand) throw()    { return operator=  ( (*this) / Fix (rightOperand)); }

    inline bool operator== (Fix const& rightOperand) const throw()      { return internal == rightOperand.internal; }
    inline bool operator== (float const& rightOperand) const throw()    { return internal == Fix (rightOperand).internal; }
    inline bool operator== (double const& rightOperand) const throw()   { return internal == Fix (rightOperand).internal; }
    inline bool operator== (int const& rightOperand) const throw()      { return internal == Fix (rightOperand).internal; }
    inline bool operator== (Internal const& rightOperand) const throw() { return internal == rightOperand; }

    inline bool operator!= (Fix const& rightOperand) const throw()      { return internal != rightOperand.internal; }
    inline bool operator!= (float const& rightOperand) const throw()    { return internal != Fix (rightOperand).internal; }
    inline bool operator!= (double const& rightOperand) const throw()   { return internal != Fix (rightOperand).internal; }
    inline bool operator!= (int const& rightOperand) const throw()      { return internal != Fix (rightOperand).internal; }
    inline bool operator!= (Internal const& rightOperand) const throw() { return internal != rightOperand; }

    inline bool operator<  (Fix const& rightOperand) const throw()      { return internal <  rightOperand.internal; }
    inline bool operator<  (float const& rightOperand) const throw()    { return internal <  Fix (rightOperand).internal; }
    inline bool operator<  (double const& rightOperand) const throw()   { return internal <  Fix (rightOperand).internal; }
    inline bool operator<  (int const& rightOperand) const throw()      { return internal <  Fix (rightOperand).internal; }
    inline bool operator<  (Internal const& rightOperand) const throw() { return internal <  rightOperand; }

    inline bool operator<= (Fix const& rightOperand) const throw()      { return internal <= rightOperand.internal; }
    inline bool operator<= (float const& rightOperand) const throw()    { return internal <= Fix (rightOperand).internal; }
    inline bool operator<= (double const& rightOperand) const throw()   { return internal <= Fix (rightOperand).internal; }
    inline bool operator<= (int const& rightOperand) const throw()      { return internal <= Fix (rightOperand).internal; }
    inline bool operator<= (Internal const& rightOperand) const throw() { return internal <= rightOperand; }

    inline bool operator>  (Fix const& rightOperand) const throw()      { return internal >  rightOperand.internal; }
    inline bool operator>  (float const& rightOperand) const throw()    { return internal >  Fix (rightOperand).internal; }
    inline bool operator>  (double const& rightOperand) const throw()   { return internal >  Fix (rightOperand).internal; }
    inline bool operator>  (int const& rightOperand) const throw()      { return internal >  Fix (rightOperand).internal; }
    inline bool operator>  (Internal const& rightOperand) const throw() { return internal >  rightOperand; }

    inline bool operator>= (Fix const& rightOperand) const throw()      { return internal >= rightOperand.internal; }
    inline bool operator>= (float const& rightOperand) const throw()    { return internal >= Fix (rightOperand).internal; }
    inline bool operator>= (double const& rightOperand) const throw()   { return internal >= Fix (rightOperand).internal; }
    inline bool operator>= (int const& rightOperand) const throw()      { return internal >= Fix (rightOperand).internal; }
    inline bool operator>= (Internal const& rightOperand) const throw() { return internal >= rightOperand; }

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
        static Base v (getIMaskInternal()); // getIMaskInternal() is slow but only happens once at init
        return v;
    }

    inline static const Base& getFMask() throw()
    {
        static Base v (getFMaskInternal()); // getFMaskInternal() is slow but only happens once at init
        return v;
    }
    
    inline const Base& getRaw() const throw() { return internal; }
    
    friend std::istream& operator>> (std::istream &inputStream, Fix& value)
    {
        float floatValue;
        inputStream >> floatValue;
        value = Fix (floatValue);
        return inputStream;
    }
    
    friend std::ostream& operator<< (std::ostream &outputStream, Fix const& value)
    {
        outputStream << float (value);
        return outputStream;
    }

    
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
        Base v = 0;
        
        for (int i = 0; i < IBits; ++i)
            v = (v << 1) | 1;
        
        return v << FBits;
    }
        
    inline static Base getFMaskInternal() throw()
    {
        Base v = 0;
        
        for (int i = 0; i < FBits; ++i)
            v = (v << 1) | 1;
        
        return v;
    }

    template<class OtherFixType>
    static inline typename FixBaseConvert<Base,typename OtherFixType::BaseType>::ConvertBase convert (OtherFixType const& other) throw()
    {
        return other.getRaw();
    }

    static inline unsigned convertShift (const unsigned dstBits, const unsigned srcBits) throw()
    {
        return dstBits - srcBits;
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
    return log2 (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sin (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return sin (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> cos (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return cos (float (a)); 
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
    return asin (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> acos (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return acos (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> atan (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return atan (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sinh (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return sinh (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> cosh (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return cosh (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> tanh (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return tanh (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sqrt (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    typedef typename FixType::WideBase WideBase;
    typedef typename FixType::UnsignedBase UnsignedBase;
    plonk_assertfalse;
    return sqrt (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> log (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return log (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> log10 (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return log10 (float (a)); 
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
    return a * a; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> cubed (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    return a * a * a; 
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
    return FixType (Internal (a.getRaw() & FixType::getFMask())); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sign (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    const Base araw (a.getRaw());
    return araw == 0 ? Math<FixType>::get0() : araw < 0 ? Math<FixType>::get_1() : Math<FixType>::get1(); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> m2f (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return m2f (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> f2m (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return f2m (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> a2dB (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return a2dB (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> dB2a (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return dB2a (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> d2r (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return d2r (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> r2d (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return r2d (float (a)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> distort (Fix<Base,IBits,FBits> const& a) throw()            
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    return reciprocal (a) + abs (a); 
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

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> fmod (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    return FixType (Internal (a.getRaw() % b.getRaw())); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> min (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return (a > b) ? b : a; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> max (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return (a < b) ? b : a; 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> pow (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return pow (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> hypot (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return hypot (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> atan2 (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return atan2 (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sumsqr (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return pow (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> difsqr (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return difsqr (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sqrsum (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return sqrsum (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> sqrdif (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return sqrdif (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> absdif (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return absdif (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> thresh (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    typedef Fix<Base,IBits,FBits> FixType;
    typedef typename FixType::Internal Internal;
    plonk_assertfalse;
    return thresh (float (a), float (b)); 
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> isEqualTo (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return a == b;
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> isNotEqualTo (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return a != b;
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> isGreaterThan (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return a > b;
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> isGreaterThanOrEqualTo (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return a >= b;
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> isLessThan (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return a < b;
}

template<class Base, unsigned IBits, unsigned FBits> 
inline Fix<Base,IBits,FBits> isLessThanOrEqualTo (Fix<Base,IBits,FBits> const& a, Fix<Base,IBits,FBits> const& b) throw()              
{ 
    return a <= b;
}


// stuff like this will work for vector specialisations too..
//template<>
//class NumericalArrayBinaryOp< FixI8F8,plonk::mulop<FixI8F8> >
//{
//public:    
//    static inline void calcNN (FixI8F8* dst, const FixI8F8* left, const FixI8F8* right, const UnsignedLong numItems) throw()
//    {
//        for (UnsignedLong i = 0; i < numItems; ++i)
//            dst[i] = plonk::mulop (left[i], right[i]);
//    }
//    
//    static inline void calcN1 (FixI8F8* dst, const FixI8F8* left, const FixI8F8 right, const UnsignedLong numItems) throw()
//    {
//        for (UnsignedLong i = 0; i < numItems; ++i)
//            dst[i] = plonk::mulop (left[i], right);
//    }
//    
//    static inline void calc1N (FixI8F8* dst, const FixI8F8 left, const FixI8F8* right, const UnsignedLong numItems) throw()
//    {
//        for (UnsignedLong i = 0; i < numItems; ++i)
//            dst[i] = plonk::mulop (left, right[i]);
//    }
//
//};

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


#endif // PLONK_FIX_H
