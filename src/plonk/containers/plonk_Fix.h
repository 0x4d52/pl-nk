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
            
    static const unsigned IBitsCount;
    static const unsigned FBitsCount;
    
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
    :   internal (Base (convert (other) << (FBits - FBitsOther)))
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
    inline Fix operator% (Fix const& rightOperand) const throw()    { return modop (*this, rightOperand); }
    inline Fix operator+ (float const& rightOperand) const throw()  { return addop (*this, Fix (rightOperand)); }
    inline Fix operator- (float const& rightOperand) const throw()  { return subop (*this, Fix (rightOperand)); }
    inline Fix operator* (float const& rightOperand) const throw()  { return mulop (*this, Fix (rightOperand)); }
    inline Fix operator/ (float const& rightOperand) const throw()  { return divop (*this, Fix (rightOperand)); }
    inline Fix operator% (float const& rightOperand) const throw()  { return modop (*this, Fix (rightOperand)); }
    inline Fix operator+ (double const& rightOperand) const throw() { return addop (*this, Fix (rightOperand)); }
    inline Fix operator- (double const& rightOperand) const throw() { return subop (*this, Fix (rightOperand)); }
    inline Fix operator* (double const& rightOperand) const throw() { return mulop (*this, Fix (rightOperand)); }
    inline Fix operator/ (double const& rightOperand) const throw() { return divop (*this, Fix (rightOperand)); }
    inline Fix operator% (double const& rightOperand) const throw() { return modop (*this, Fix (rightOperand)); }
    inline Fix operator+ (int const& rightOperand) const throw()    { return addop (*this, Fix (rightOperand)); }
    inline Fix operator- (int const& rightOperand) const throw()    { return subop (*this, Fix (rightOperand)); }
    inline Fix operator* (int const& rightOperand) const throw()    { return mulop (*this, Fix (rightOperand)); }
    inline Fix operator/ (int const& rightOperand) const throw()    { return divop (*this, Fix (rightOperand)); }
    inline Fix operator% (int const& rightOperand) const throw()    { return modop (*this, Fix (rightOperand)); }

    friend inline Fix operator+ (float const& leftOperand, Fix const& rightOperand) throw()  { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator- (float const& leftOperand, Fix const& rightOperand) throw()  { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator* (float const& leftOperand, Fix const& rightOperand) throw()  { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator/ (float const& leftOperand, Fix const& rightOperand) throw()  { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator% (float const& leftOperand, Fix const& rightOperand) throw()  { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator+ (double const& leftOperand, Fix const& rightOperand) throw() { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator- (double const& leftOperand, Fix const& rightOperand) throw() { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator* (double const& leftOperand, Fix const& rightOperand) throw() { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator/ (double const& leftOperand, Fix const& rightOperand) throw() { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator% (double const& leftOperand, Fix const& rightOperand) throw() { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator+ (int const& leftOperand, Fix const& rightOperand) throw()    { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator- (int const& leftOperand, Fix const& rightOperand) throw()    { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator* (int const& leftOperand, Fix const& rightOperand) throw()    { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator/ (int const& leftOperand, Fix const& rightOperand) throw()    { return Fix (leftOperand) + rightOperand; }
    friend inline Fix operator% (int const& leftOperand, Fix const& rightOperand) throw()    { return Fix (leftOperand) + rightOperand; }

    inline Fix operator<< (const int shift) const throw()           { return Fix (Internal (internal << shift)); }
    inline Fix operator>> (const int shift) const throw()           { return Fix (Internal (internal >> shift)); }
    
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
    
    // could optimise these better
    inline Fix& operator+= (Fix const& rightOperand) throw()        { return operator=  ( (*this) + rightOperand); }
    inline Fix& operator-= (Fix const& rightOperand) throw()        { return operator=  ( (*this) - rightOperand); }
    inline Fix& operator*= (Fix const& rightOperand) throw()        { return operator=  ( (*this) * rightOperand); }
    inline Fix& operator/= (Fix const& rightOperand) throw()        { return operator=  ( (*this) / rightOperand); }
    inline Fix& operator%= (Fix const& rightOperand) throw()        { return operator=  ( (*this) % rightOperand); }
    inline Fix& operator+= (float const& rightOperand) throw()      { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (float const& rightOperand) throw()      { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (float const& rightOperand) throw()      { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (float const& rightOperand) throw()      { return operator=  ( (*this) / Fix (rightOperand)); }
    inline Fix& operator%= (float const& rightOperand) throw()      { return operator=  ( (*this) % Fix (rightOperand)); }
    inline Fix& operator+= (double const& rightOperand) throw()     { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (double const& rightOperand) throw()     { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (double const& rightOperand) throw()     { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (double const& rightOperand) throw()     { return operator=  ( (*this) / Fix (rightOperand)); }
    inline Fix& operator%= (double const& rightOperand) throw()     { return operator=  ( (*this) % Fix (rightOperand)); }
    inline Fix& operator+= (int const& rightOperand) throw()        { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (int const& rightOperand) throw()        { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (int const& rightOperand) throw()        { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (int const& rightOperand) throw()        { return operator=  ( (*this) / Fix (rightOperand)); }
    inline Fix& operator%= (int const& rightOperand) throw()        { return operator=  ( (*this) % Fix (rightOperand)); }
    inline Fix& operator+= (Internal const& rightOperand) throw()   { return operator=  ( (*this) + Fix (rightOperand)); }
    inline Fix& operator-= (Internal const& rightOperand) throw()   { return operator=  ( (*this) - Fix (rightOperand)); }
    inline Fix& operator*= (Internal const& rightOperand) throw()   { return operator=  ( (*this) * Fix (rightOperand)); }
    inline Fix& operator/= (Internal const& rightOperand) throw()   { return operator=  ( (*this) / Fix (rightOperand)); }
    inline Fix& operator%= (Internal const& rightOperand) throw()   { return operator=  ( (*this) % Fix (rightOperand)); }

    inline Fix& operator<<= (const int shift) throw()                { internal << shift; return *this; }
    inline Fix& operator>>= (const int shift) throw()                { internal >> shift; return *this; }

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

    // -- unary ops --------------------------------------------------------- //
    
    friend inline Fix move (Fix const& a) throw()             
    { 
        return a; 
    }
    
    inline Fix move() const throw()             
    { 
        return *this; 
    }

    friend inline Fix neg (Fix const& a) throw()              
    { 
        return a.neg();
    }
    
    inline Fix neg() const throw()              
    { 
        return Fix  (Internal (-internal)); 
    }
    
    friend inline Fix abs (Fix const& a) throw()            
    { 
        return a.abs();
    }
    
    inline Fix abs() const throw()            
    { 
        return Fix (internal < 0 ? Internal (-internal) : Internal (internal)); 
    }
    
    friend inline Fix reciprocal (Fix const& a) throw()            
    {         
        return a.reciprocal();
    }

    inline Fix reciprocal() const throw()            
    {         
        return Fix::getOne() / *this; 
    }
    
    friend inline Fix log2 (Fix const& a) throw()            
    { 
        return a.log2();
    }
    
    inline Fix log2 () const throw()            
    { 
        plonk_assertfalse;
        return log2 (float (*this)); 
    }
    
    friend inline Fix sin (Fix const& a) throw()            
    { 
        return a.sin();
    }
    
    inline Fix sin() const throw()            
    { 
        plonk_assertfalse;
        return sin (float (*this)); 
    }

    friend inline Fix cos (Fix const& a) throw()            
    { 
        return a.cos(); 
    }
    
    inline Fix cos() const throw()            
    { 
        plonk_assertfalse;
        return cos (float (*this)); 
    }
    
    friend inline Fix tan (Fix const& a) throw()            
    { 
        return a.tan();
    }
    
    inline Fix tan() const throw()            
    { 
        plonk_assertfalse;
        return tan (float (*this)); 
    }

    friend inline Fix asin (Fix const& a) throw()            
    { 
        return a.asin();
    }
    
    inline Fix asin() const throw()            
    { 
        plonk_assertfalse;
        return asin (float (*this)); 
    }
    
    friend inline Fix acos (Fix const& a) throw()            
    { 
        return a.acos(); 
    }

    inline Fix acos() const throw()            
    { 
        plonk_assertfalse;
        return acos (float (*this)); 
    }

    friend inline Fix atan (Fix const& a) throw()            
    { 
        return a.atan(); 
    }
    
    inline Fix atan() const throw()            
    { 
        plonk_assertfalse;
        return atan (float (*this)); 
    }
    
    friend inline Fix sinh (Fix const& a) throw()            
    { 
        return a.sinh();
    }
    
    inline Fix sinh() const throw()            
    { 
        plonk_assertfalse;
        return sinh (float (*this)); 
    }
    
    friend inline Fix cosh (Fix const& a) throw()            
    { 
        return a.cosh();
    }
    
    inline Fix cosh() const throw()            
    { 
        plonk_assertfalse;
        return cosh (float (*this)); 
    }
    
    friend inline Fix tanh (Fix const& a) throw()            
    { 
        return a.tanh();
    }
    
    inline Fix tanh() const throw()            
    { 
        plonk_assertfalse;
        return tanh (float (*this)); 
    }

    friend inline Fix sqrt (Fix const& a) throw()            
    { 
        return a.sqrt();
    }
    
    inline Fix sqrt() const throw()            
    { 
        plonk_assertfalse;
        return sqrt (float (*this)); 
    }
    
    friend inline Fix log (Fix const& a) throw()            
    { 
        return a.log();
    }
    
    inline Fix log() const throw()            
    { 
        plonk_assertfalse;
        return log (float (*this)); 
    }
    
    friend inline Fix log10 (Fix const& a) throw()            
    { 
        return a.log10();
    }
    
    inline Fix log10() const throw()            
    { 
        plonk_assertfalse;
        return log10 (float (*this)); 
    }
    
    friend inline Fix exp (Fix const& a) throw()            
    {  
        return a.exp();
    }
    
    inline Fix exp() const throw()            
    {         
        const Base raw1 (Fix::getOne().getRaw());
        Base araw (internal);
        
        if (araw == 0) 
            return Fix::getOne();
        
        if (araw == raw1) 
            return Math<Fix>::getE();
        
        //	if(a >= 681391   ) return fix16_maximum;
        //	if(a <= -772243  ) return 0;
        
        bool neg (araw < 0);
        
        if (neg) 
            araw = -araw;
        
        Base result (araw + raw1);
        Fix term = Fix (Internal (araw));
        
        int i;        
        for (i = 2; i < (IBits + FBits - 2); ++i)
        {
            term *= Fix (Internal (araw)) / Fix  (i);
            
            const Base rawterm (term.getRaw());
            result += rawterm;
            
            if ((rawterm < 500) && ((i > (FBits - 1)) || (rawterm < (FBits + 4))))
                break;
        }
        
        return neg ? reciprocal (Fix (Internal (result))) : Fix (Internal (result));
    }
    
    friend inline Fix squared (Fix const& a) throw()            
    { 
        return a * a; 
    }
    
    inline Fix squared() const throw()            
    { 
        return (*this) * (*this); 
    }
    
    friend inline Fix cubed (Fix const& a) throw()            
    { 
        return a * a * a; 
    }
    
    inline Fix cubed() const throw()            
    { 
        return (*this) * (*this) * (*this); 
    }
    
    friend inline Fix floor (Fix const& a) throw()            
    { 
        return a.floor();
    }
    
    inline Fix floor() const throw()            
    { 
        return Fix (Internal (internal & Fix::getIMask().getRaw())); 
    }
    
    friend inline Fix ceil (Fix const& a) throw()             
    {
        return a.ceil();
    }
    
    inline Fix ceil() const throw()             
    { 
        const Base imask (Fix::getIMask().getRaw());
        const Base fmask (Fix::getFMask().getRaw());
        const Base one (Fix::getOne().getRaw());
        return Fix (Internal ((internal & imask) + (internal & fmask ? one : 0))); 
    }
    
    friend inline Fix frac (Fix const& a) throw()            
    { 
        return a.frac();
    }
    
    inline Fix frac() const throw()            
    { 
        return Fix (Internal (internal & Fix::getFMask())); 
    }
    
    friend inline Fix sign (Fix const& a) throw()            
    { 
        const Base araw (a.getRaw());
        return araw == 0 ? Math<Fix>::get0() : araw < 0 ? Math<Fix>::get_1() : Math<Fix>::get1(); 
    }
    
    inline Fix sign() const throw()            
    { 
        return internal == 0 ? Math<Fix>::get0() : internal < 0 ? Math<Fix>::get_1() : Math<Fix>::get1(); 
    }
    
    friend inline Fix m2f (Fix const& a) throw()            
    { 
        return a.m2f();
    }
    
    inline Fix m2f() const throw()            
    { 
        plonk_assertfalse;
        return m2f (float (*this)); 
    }
    
    friend inline Fix f2m (Fix const& a) throw()            
    { 
        return a.f2m();
    }
    
    inline Fix f2m() const throw()            
    { 
        plonk_assertfalse;
        return f2m (float (*this)); 
    }
    
    friend inline Fix a2dB (Fix const& a) throw()            
    { 
        return a.a2dB(); 
    }
    
    inline Fix a2dB() const throw()            
    { 
        plonk_assertfalse;
        return a2dB (float (*this)); 
    }
    
    friend inline Fix dB2a (Fix const& a) throw()            
    { 
        return a.dB2a();
    }
    
    inline Fix dB2a() const throw()            
    { 
        plonk_assertfalse;
        return dB2a (float (*this)); 
    }
    
    friend inline Fix d2r (Fix const& a) throw()            
    { 
        return a.d2r(); 
    }
    
    inline Fix d2r() const throw()            
    { 
        plonk_assertfalse;
        return d2r (float (*this)); 
    }
    
    friend inline Fix r2d (Fix const& a) throw()            
    { 
        return a.r2d();
    }
    
    inline Fix r2d() const throw()            
    { 
        plonk_assertfalse;
        return r2d (float (*this)); 
    }
    
    friend inline Fix distort (Fix const& a) throw()            
    { 
        return a.distort();
    }
    
    inline Fix distort() const throw()            
    { 
        return reciprocal (*this) + abs (*this); 
    }
    
    friend inline Fix zap (Fix const& a) throw()            
    { 
        return a; 
    }
    
    inline Fix zap() const throw()            
    { 
        return *this; 
    }

    // -- binary ops -------------------------------------------------------- //
    
//    friend inline Fix mulop (Fix const& a, Fix const& b) throw()              
//    { 
//        return a.mulop (b); 
//    }
        
    friend inline Fix addop (Fix const& a, Fix const& b) throw()              
    { 
        return a.addop (b); 
    }

    inline Fix addop (Fix const& b) const throw()              
    { 
        return FixType (Internal (internal + b.internal)); 
    }
    
    friend inline Fix subop (Fix const& a, Fix const& b) throw()              
    { 
        return a.subop (b); 
    }

    inline Fix subop (Fix const& b) const throw()              
    { 
        return FixType (Internal (internal - b.internal)); 
    }
    
    friend inline Fix mulop (Fix const& a, Fix const& b) throw()              
    { 
        return a.mulop (b); 
    }

    inline Fix mulop (Fix const& b) const throw()              
    { 
        const WideBase product = WideBase (internal) * WideBase (b.internal);
        return Fix (Internal (product >> FBits)); 
    }
    
    friend inline Fix divop (Fix const& a, Fix const& b) throw()              
    { 
        return a.divop (b); 
    }

    inline Fix divop (Fix const& b) const throw()              
    { 
        return FixType (Internal ((WideBase (internal) << FBits) / WideBase (b.internal))); 
    }
    
    friend inline Fix modop (Fix const& a, Fix const& b) throw()              
    { 
        return a.modop (b); 
    }

    inline Fix modop (Fix const& b) const throw()              
    { 
        return FixType (Internal (internal % b.internal)); 
    }
    
    friend inline Fix min (Fix const& a, Fix const& b) throw()              
    { 
        return a.min (b); 
    }

    inline Fix min (Fix const& b) const throw()              
    { 
        return (*this > b) ? b : *this; 
    }
    
    friend inline Fix max (Fix const& a, Fix const& b) throw()              
    { 
        return a.max (b); 
    }

    inline Fix max (Fix const& b) const throw()              
    { 
        return (*this < b) ? b : *this; 
    }
    
    friend inline Fix pow (Fix const& a, Fix const& b) throw()              
    { 
        return a.pow (b); 
    }

    inline Fix pow (Fix const& b) const throw()              
    { 
        plonk_assertfalse;
        return pow (float (*this), float (b)); 
    }
    
    friend inline Fix hypot (Fix const& a, Fix const& b) throw()              
    { 
        return a.hypot (b); 
    }

    inline Fix hypot (Fix const& b) const throw()              
    { 
        plonk_assertfalse;
        return hypot (float (*this), float (b)); 
    }
    
    friend inline Fix atan2 (Fix const& a, Fix const& b) throw()              
    { 
        return a.atan2 (b); 
    }

    inline Fix atan2 (Fix const& b) const throw()              
    { 
        plonk_assertfalse;
        return atan2 (float (*this), float (b)); 
    }
    
    friend inline Fix sumsqr (Fix const& a, Fix const& b) throw()              
    { 
        return a.sumsqr (b); 
    }

    inline Fix sumsqr (Fix const& b) const throw()              
    { 
        return squared (*this) + squared (b); 
    }
    
    friend inline Fix difsqr (Fix const& a, Fix const& b) throw()              
    { 
        return a.difsqr (b); 
    }
    
    inline Fix difsqr (Fix const& b) const throw()              
    { 
        return squared (*this) - squared (b); 
    }
    
    friend inline Fix sqrsum (Fix const& a, Fix const& b) throw()              
    { 
        return a.sqrsum (b); 
    }
    
    inline Fix sqrsum (Fix const& b) const throw()              
    { 
        return addop (*this, b).squared();
    }
    
    friend inline Fix sqrdif (Fix const& a, Fix const& b) throw()              
    { 
        return a.sqrdif (b); 
    }
    
    inline Fix sqrdif (Fix const& b) const throw()              
    { 
        return subop (*this, b).squared();
    }
    
    friend inline Fix absdif (Fix const& a, Fix const& b) throw()              
    { 
        return a.absdif (b); 
    }

    inline Fix absdif (Fix const& b) const throw()              
    { 
        return subop (*this, b).abs();
    }
    
    friend inline Fix thresh (Fix const& a, Fix const& b) throw()              
    { 
        return a.thresh (b); 
    }

    inline Fix thresh (Fix const& b) const throw()              
    { 
        return internal < b.internal ? 0 : *this;
    }
    
    friend inline Fix isEqualTo (Fix const& a, Fix const& b) throw()              
    { 
        return a.isEqualTo (b); 
    }

    inline Fix isEqualTo (Fix const& b) const throw()              
    { 
        return internal == b.internal;
    }
    
    friend inline Fix isNotEqualTo (Fix const& a, Fix const& b) throw()              
    { 
        return a.isNotEqualTo (b); 
    }

    inline Fix isNotEqualTo (Fix const& b) const throw()              
    { 
        return internal != b.internal;
    }

    friend inline Fix isGreaterThan (Fix const& a, Fix const& b) throw()              
    { 
        return a.isGreaterThan (b); 
    }

    inline Fix isGreaterThan (Fix const& b) const throw()              
    { 
        return internal > b.internal;
    }
    
    friend inline Fix isGreaterThanOrEqualTo (Fix const& a, Fix const& b) throw()              
    { 
        return a.isGreaterThanOrEqualTo (b); 
    }

    inline Fix isGreaterThanOrEqualTo (Fix const& b) const throw()              
    { 
        return internal >= b.internal;
    }
    
    friend inline Fix isLessThan (Fix const& a, Fix const& b) throw()              
    { 
        return a.isLessThan (b); 
    }

    inline Fix isLessThan (Fix const& b) const throw()              
    { 
        return internal < b.internal;
    }
    
    friend inline Fix isLessThanOrEqualTo (Fix const& a, Fix const& b) throw()              
    { 
        return a.isLessThanOrEqualTo (b); 
    }

    inline Fix isLessThanOrEqualTo (Fix const& b) const throw()              
    { 
        return internal <= b.internal;
    }

    // -- contants ---------------------------------------------------------- //
    
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

    /** Converts to a wider/narrower word depending on the conversion. */
    template<class OtherFixType>
    static inline typename FixBaseConvert<Base,typename OtherFixType::BaseType>::ConvertBase convert (OtherFixType const& other) throw()
    {
        typedef typename FixBaseConvert<Base,typename OtherFixType::BaseType>::ConvertBase ConvertBase;
        return ConvertBase (other.getRaw());
    }
};

template<class Base, unsigned IBits, unsigned FBits> const unsigned Fix<Base,IBits,FBits>::IBitsCount = IBits;
template<class Base, unsigned IBits, unsigned FBits> const unsigned Fix<Base,IBits,FBits>::FBitsCount = FBits;

#define PLONK_FIXBINARYOPFUNCTION_DEFINE(OP)\
    static inline OperandType OP (OperandType const& a, OperandType const& b) throw() { return a.OP (b); }

template<class FixType>
class FixBinaryOpFunctions
{
public:
    typedef FixType OperandType;

    PLONK_FIXBINARYOPFUNCTION_DEFINE(addop)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(subop)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(mulop)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(divop)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(modop)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(isEqualTo)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(isNotEqualTo)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(isGreaterThan)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(isGreaterThanOrEqualTo)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(isLessThan)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(isLessThanOrEqualTo)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(hypot)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(pow)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(min)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(max)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(sumsqr)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(difsqr)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(sqrsum)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(sqrdif)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(absdif)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(thresh)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(round)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(trunc)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(clip2)
    PLONK_FIXBINARYOPFUNCTION_DEFINE(decayFeedback)
};

#define PLONK_FIXUNARYOPFUNCTION_DEFINE(OP)\
    static inline OperandType OP (OperandType const& a) throw() { return a.OP(); }

template<class FixType>
class FixUnaryOpFunctions
{
public:
    typedef FixType OperandType;
    
    PLONK_FIXUNARYOPFUNCTION_DEFINE(move)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(inc)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(dec)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(abs)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(log2)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(neg)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(reciprocal)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(sin)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(cos)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(tan)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(asin)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(acos)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(atan)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(sinh)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(cosh)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(tanh)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(sqrt)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(log)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(log10)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(exp)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(squared)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(cubed)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(ceil)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(floor)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(frac)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(sign)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(m2f)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(f2m)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(a2dB)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(dB2a)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(d2r)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(r2d)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(distort)
    PLONK_FIXUNARYOPFUNCTION_DEFINE(zap)
};



#define PLONK_OPFUNCTIONSHELPERS_DEFINE(TYPE,I,F)\
    template<> class BinaryOpFunctionsHelper<Fix<TYPE,I,F> > { public: typedef FixBinaryOpFunctions<Fix<TYPE,I,F> > BinaryOpFunctionsType; };\
    template<> class UnaryOpFunctionsHelper<Fix<TYPE,I,F> > { public: typedef FixUnaryOpFunctions<Fix<TYPE,I,F> > UnaryOpFunctionsType; };

// need to specialise all the possibilities of this just in case..

PLONK_OPFUNCTIONSHELPERS_DEFINE(char,1,7)
PLONK_OPFUNCTIONSHELPERS_DEFINE(char,2,6)
PLONK_OPFUNCTIONSHELPERS_DEFINE(char,3,5)
PLONK_OPFUNCTIONSHELPERS_DEFINE(char,4,4)
PLONK_OPFUNCTIONSHELPERS_DEFINE(char,5,3)
PLONK_OPFUNCTIONSHELPERS_DEFINE(char,6,2)
PLONK_OPFUNCTIONSHELPERS_DEFINE(char,7,1)

PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 1,15)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 2,14)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 3,13)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 4,12)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 5,11)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 6,10)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 7, 9)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 8, 8)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short, 9, 7)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short,10, 6)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short,11, 5)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short,12, 4)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short,13, 3)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short,14, 2)
PLONK_OPFUNCTIONSHELPERS_DEFINE(short,15, 1)

PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 1,31)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 2,30)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 3,29)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 4,28)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 5,27)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 6,26)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 7,25)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 8,24)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int, 9,23)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,10,22)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,11,21)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,12,20)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,13,19)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,14,18)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,15,17)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,16,16)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,17,15)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,18,14)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,19,13)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,20,12)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,21,11)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,22,10)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,23, 9)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,24, 8)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,25, 7)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,26, 6)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,27, 5)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,28, 4)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,29, 3)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,30, 2)
PLONK_OPFUNCTIONSHELPERS_DEFINE(int,31, 1)




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



#endif // PLONK_FIX_H
