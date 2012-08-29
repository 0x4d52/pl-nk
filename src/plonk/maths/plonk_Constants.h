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

#ifndef PLONK_CONSTANTS_H
#define PLONK_CONSTANTS_H


/** Some useful maths constants. 
 @ingroup PlonkOtherUserClasses */
template<class Type>
class Math
{
public:
    static inline const Type& get0() throw()          { static const Type v (0); return v; }
    static inline const Type& get1() throw()          { static const Type v (1); return v; }
    static inline const Type& get2() throw()          { static const Type v (2); return v; }
    static inline const Type& get4() throw()          { static const Type v (4); return v; }
    static inline const Type& get10() throw()         { static const Type v (10); return v; }
    static inline const Type& get20() throw()         { static const Type v (20); return v; }

    static inline const Type& get_1() throw()         { static const Type v (-1); return v; }
    static inline const Type& get0_5() throw()        { static const Type v (0.5); return v; }
    static inline const Type& get_0_5() throw()       { static const Type v (-0.5); return v; }
    static inline const Type& get0_25() throw()       { static const Type v (0.25); return v; }
    static inline const Type& get1_5() throw()        { static const Type v (1.5); return v; }
    
    static inline const Type& get40() throw()         { static const Type v (40); return v; }    
    static inline const Type& get440() throw()        { static const Type v (440); return v; }
    static inline const Type& get12() throw()         { static const Type v (12); return v; }
    static inline const Type& get69() throw()         { static const Type v (69); return v; }
    static inline const Type& get360() throw()        { static const Type v (360); return v; }    
    
    static inline const Type& get1_40() throw()       { static const Type v (get1() / get40()); return v; }
    static inline const Type& get1_440() throw()      { static const Type v (get1() / get440()); return v; }
    static inline const Type& get1_12() throw()       { static const Type v (get1() / get12()); return v; }
    static inline const Type& get1_360() throw()      { static const Type v (get1() / get360()); return v; }
    
    static inline const Type& getPi() throw()         { static const Type v (std::acos (-1.0)); return v; }
    static inline const Type& getLog0_001() throw()   { static const Type v (std::log (0.001)); return v; }
    static inline const Type& getLog0_01() throw()    { static const Type v (std::log (0.01)); return v; }
    static inline const Type& getLog0_1() throw()     { static const Type v (std::log (0.1)); return v; }
    static inline const Type& getLog2() throw()       { static const Type v (std::log (2.0)); return v; }
    static inline const Type& get1_Log2() throw()     { static const Type v (1.0 / std::log (2.0)); return v; }
    static inline const Type& getSqrt2() throw()      { static const Type v (std::sqrt (2.0)); return v; }
    static inline const Type& get1_Sqrt2() throw()    { static const Type v (1.0 / getSqrt2()); return v; }
    static inline const Type& getSqrt2_2() throw()    { static const Type v (getSqrt2() * get0_5()); return v; }
    static inline const Type& getSqrt2_1() throw()    { static const Type v (getSqrt2() - get1()); return v; }
    
    static inline const Type& getLogSqrt2() throw()   { static const Type v (std::log (std::sqrt (2.0))); return v; }
    static inline const Type& getE() throw()          { static const Type v (std::exp (1.0)); return v; }

    static inline const Type& get2Pi() throw()        { static const Type v (getPi() * get2()); return v; }
    static inline const Type& get4Pi() throw()        { static const Type v (getPi() * get4()); return v; }
    
    static inline const Type& getPi_2() throw()       { static const Type v (getPi() * get0_5()); return v; }
    static inline const Type& getPi_4() throw()       { static const Type v (getPi() * get0_25()); return v; }
    static inline const Type& get3Pi_2() throw()      { static const Type v (getPi() * get1_5()); return v; }
    static inline const Type& get1_2Pi() throw()      { static const Type v (1.0 / get2Pi()); return v; }
};

typedef Math<Float> FloatMath;
typedef Math<Double> DoubleMath;


#endif // PLONK_CONSTANTS_H
