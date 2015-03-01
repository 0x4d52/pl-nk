/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_CONSTANTS_H
#define PLONK_CONSTANTS_H


/** Some useful maths constants. 
 @ingroup PlonkOtherUserClasses */
template<class Type>
class Math
{
public:
    static PLONK_INLINE_LOW const Type& get0() throw()          { static const Type v ((Type)0); return v; }
    static PLONK_INLINE_LOW const Type& get1() throw()          { static const Type v ((Type)1); return v; }
    static PLONK_INLINE_LOW const Type& get2() throw()          { static const Type v ((Type)2); return v; }
    static PLONK_INLINE_LOW const Type& get3() throw()          { static const Type v ((Type)3); return v; }
    static PLONK_INLINE_LOW const Type& get4() throw()          { static const Type v ((Type)4); return v; }
    static PLONK_INLINE_LOW const Type& get10() throw()         { static const Type v ((Type)10); return v; }
    static PLONK_INLINE_LOW const Type& get20() throw()         { static const Type v ((Type)20); return v; }
    static PLONK_INLINE_LOW const Type& get30() throw()         { static const Type v ((Type)30); return v; }
    static PLONK_INLINE_LOW const Type& get40() throw()         { static const Type v ((Type)40); return v; }
    static PLONK_INLINE_LOW const Type& get50() throw()         { static const Type v ((Type)50); return v; }
    static PLONK_INLINE_LOW const Type& get100() throw()        { static const Type v ((Type)100); return v; }

    static PLONK_INLINE_LOW const Type& get_1() throw()         { static const Type v ((Type)-1); return v; }
    static PLONK_INLINE_LOW const Type& get0_5() throw()        { static const Type v ((Type)0.5); return v; }
    static PLONK_INLINE_LOW const Type& get_0_5() throw()       { static const Type v ((Type)-0.5); return v; }
    static PLONK_INLINE_LOW const Type& get0_25() throw()       { static const Type v ((Type)0.25); return v; }
    static PLONK_INLINE_LOW const Type& get1_5() throw()        { static const Type v ((Type)1.5); return v; }
    
    static PLONK_INLINE_LOW const Type& get440() throw()        { static const Type v ((Type)440); return v; }
    static PLONK_INLINE_LOW const Type& get12() throw()         { static const Type v ((Type)12); return v; }
    static PLONK_INLINE_LOW const Type& get69() throw()         { static const Type v ((Type)69); return v; }
    static PLONK_INLINE_LOW const Type& get360() throw()        { static const Type v ((Type)360); return v; }    

    static PLONK_INLINE_LOW const Type& get1_3() throw()       { static const Type v ((Type)(1.0 / 3.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_6() throw()       { static const Type v ((Type)(1.0 / 6.0)); return v; }

    static PLONK_INLINE_LOW const Type& get1_40() throw()       { static const Type v ((Type)(1.0 / 40.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_440() throw()      { static const Type v ((Type)(1.0 / 440.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_12() throw()       { static const Type v ((Type)(1.0 / 12.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_360() throw()      { static const Type v ((Type)(1.0 / 360.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_1200() throw()     { static const Type v ((Type)(1.0 / 1200.0)); return v; }
    
    static PLONK_INLINE_LOW const Type& getPi() throw()         { static const Type v ((Type)std::acos (-1.0)); return v; }
    static PLONK_INLINE_LOW const Type& getLog0_001() throw()   { static const Type v ((Type)std::log (0.001)); return v; }
    static PLONK_INLINE_LOW const Type& getLog0_01() throw()    { static const Type v ((Type)std::log (0.01)); return v; }
    static PLONK_INLINE_LOW const Type& getLog0_1() throw()     { static const Type v ((Type)std::log (0.1)); return v; }
    static PLONK_INLINE_LOW const Type& getLog2() throw()       { static const Type v ((Type)std::log (2.0)); return v; }
    static PLONK_INLINE_LOW const Type& getLog10() throw()      { static const Type v ((Type)std::log (10.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_Log2() throw()     { static const Type v ((Type)(1.0 / std::log (2.0))); return v; }
    static PLONK_INLINE_LOW const Type& get1_Log10() throw()    { static const Type v ((Type)(1.0 /std::log (10.0))); return v; }
    static PLONK_INLINE_LOW const Type& getSqrt2() throw()      { static const Type v ((Type)std::sqrt (2.0)); return v; }
    static PLONK_INLINE_LOW const Type& get1_Sqrt2() throw()    { static const Type v ((Type)(1.0 / Math<double>::getSqrt2())); return v; }
    static PLONK_INLINE_LOW const Type& getSqrt2_2() throw()    { static const Type v ((Type)(Math<double>::getSqrt2() * 0.5)); return v; }
    static PLONK_INLINE_LOW const Type& getSqrt2_1() throw()    { static const Type v ((Type)(Math<double>::getSqrt2() - 1.0)); return v; }

    static PLONK_INLINE_LOW const Type& getLogSqrt2() throw()   { static const Type v ((Type)std::log (Math<double>::getSqrt2())); return v; }
    static PLONK_INLINE_LOW const Type& getE() throw()          { static const Type v ((Type)std::exp (1.0)); return v; }
    static PLONK_INLINE_LOW const Type& getE4() throw()         { static const Type v ((Type)(std::exp (1.0) * std::exp (1.0) * std::exp (1.0) * std::exp (1.0))); return v; }

    static PLONK_INLINE_LOW const Type& get2Pi() throw()        { static const Type v ((Type)(Math<double>::getPi() * 2.0)); return v; }
    static PLONK_INLINE_LOW const Type& get4Pi() throw()        { static const Type v ((Type)(Math<double>::getPi() * 4.0)); return v; }
    
    static PLONK_INLINE_LOW const Type& getPi_2() throw()       { static const Type v ((Type)(Math<double>::getPi() * 0.5)); return v; }
    static PLONK_INLINE_LOW const Type& getPi_4() throw()       { static const Type v ((Type)(Math<double>::getPi() * 0.25)); return v; }
    static PLONK_INLINE_LOW const Type& get3Pi_2() throw()      { static const Type v ((Type)(Math<double>::getPi() * 1.5)); return v; }
    static PLONK_INLINE_LOW const Type& get3Pi_4() throw()      { static const Type v ((Type)(Math<double>::getPi() * 0.75)); return v; }
    static PLONK_INLINE_LOW const Type& get1_2Pi() throw()      { static const Type v ((Type)(1.0 / Math<double>::getPi())); return v; }
    static PLONK_INLINE_LOW const Type& get4_Pi() throw()       { static const Type v ((Type)(4.0 / Math<double>::getPi())); return v; }
    static PLONK_INLINE_LOW const Type& get_4_Pi() throw()      { static const Type v ((Type)(-4.0 / Math<double>::getPi())); return v; }

};

template<class Type, unsigned N>
class Series
{
public:
    static PLONK_INLINE_LOW const Type& getFactorial() throw()        { static const Type v ((Type)getFactorialInternal()); return v; }
    static PLONK_INLINE_LOW const Type& get1_Factorial() throw()      { static const Type v ((Type)(1.0 / getFactorialInternal())); return v; }
    static PLONK_INLINE_LOW const Type& getSum() throw()              { static const Type v ((Type)getSumInternal()); return v; }
    static PLONK_INLINE_LOW const Type& get1_Sum() throw()            { static const Type v ((Type)(1.0 / getSumInternal())); return v; }
    
    static PLONK_INLINE_LOW const Type& getFibonacciSum() throw()     { static const Type v ((Type)getFibonacciInternal<double> (N)); return v; }
    static PLONK_INLINE_LOW const Type& getGolden() throw()           
    { 
        static const Type v ((Type)(getFibonacciInternal<double> (N - 1) / getFibonacciInternal<double> (N)));
        return v;
    }
    
private:
#if PLONK_DEBUG
    static void checkN() { plonk_staticassert (N != 0); }
#endif
    
    static PLONK_INLINE_LOW const double getFactorialInternal() throw() 
    { 
        double v = 1.0;
        
        for (int i = 2; i <= N; ++i)
            v *= double (i);
        
        return v;
    }
    
    template<class CalcType>
    static PLONK_INLINE_LOW const double getFibonacciInternal (const int n) throw() 
    { 
        CalcType y0 = 1;
        CalcType y1 = 1;
        
        for (int i = 2; i <= N; ++i)
        {
            const CalcType temp = y0 + y1;
            y1 = y0;
            y0 = temp;
        }
        
        return y0;
    }

    
    static PLONK_INLINE_LOW const double getSumInternal() throw() 
    { 
        double v = 1.0;
        
        for (int i = 2; i <= N; ++i)
            v += double (i);
        
        return v;
    }

};

typedef Math<Float> FloatMath;
typedef Math<Double> DoubleMath;


#endif // PLONK_CONSTANTS_H
