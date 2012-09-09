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

#ifndef PLONK_INLINEUNARYOPS_H
#define PLONK_INLINEUNARYOPS_H

#include "plonk_Constants.h"
#include "plonk_InlineCommonOps.h"

//------------------------------------------------------------------------------

#define PLONK_UNARYOPFUNCTION(TYPE,NAME) TYPE (*NAME)(TYPE const&)

#define PLONK_UNARYOP(CLASSNAME, OP) \
        /** Create a new CLASSNAME by applying the unary '##OP##' function to this one. */\
        inline CLASSNAME OP() const throw() { return unary<plonk::OP>(); }


#define PLONK_UNARYOPS(CLASSNAME) \
        PLONK_UNARYOP(CLASSNAME, move)\
        PLONK_UNARYOP(CLASSNAME, inc)\
        PLONK_UNARYOP(CLASSNAME, dec)\
        PLONK_UNARYOP(CLASSNAME, abs)\
        PLONK_UNARYOP(CLASSNAME, log2)\
        PLONK_UNARYOP(CLASSNAME, neg)\
        PLONK_UNARYOP(CLASSNAME, reciprocal)\
        PLONK_UNARYOP(CLASSNAME, sin)\
        PLONK_UNARYOP(CLASSNAME, cos)\
        PLONK_UNARYOP(CLASSNAME, tan)\
        PLONK_UNARYOP(CLASSNAME, asin)\
        PLONK_UNARYOP(CLASSNAME, acos)\
        PLONK_UNARYOP(CLASSNAME, atan)\
        PLONK_UNARYOP(CLASSNAME, sinh)\
        PLONK_UNARYOP(CLASSNAME, cosh)\
        PLONK_UNARYOP(CLASSNAME, tanh)\
        PLONK_UNARYOP(CLASSNAME, sqrt)\
        PLONK_UNARYOP(CLASSNAME, log)\
        PLONK_UNARYOP(CLASSNAME, log10)\
        PLONK_UNARYOP(CLASSNAME, exp)\
        PLONK_UNARYOP(CLASSNAME, squared)\
        PLONK_UNARYOP(CLASSNAME, cubed)\
        PLONK_UNARYOP(CLASSNAME, ceil)\
        PLONK_UNARYOP(CLASSNAME, floor)\
        PLONK_UNARYOP(CLASSNAME, frac)\
        PLONK_UNARYOP(CLASSNAME, sign)\
        PLONK_UNARYOP(CLASSNAME, m2f)\
        PLONK_UNARYOP(CLASSNAME, f2m)\
        PLONK_UNARYOP(CLASSNAME, a2dB)\
        PLONK_UNARYOP(CLASSNAME, dB2a)\
        PLONK_UNARYOP(CLASSNAME, d2r)\
        PLONK_UNARYOP(CLASSNAME, r2d)\
        PLONK_UNARYOP(CLASSNAME, distort)\
        PLONK_UNARYOP(CLASSNAME, zap)\
        /** Create a new CLASSNAME by applying the unary '-' operator to this one. **/\
        inline CLASSNAME operator-() const throw()  { return unary<plonk::neg>(); }\
        /** Create a new CLASSNAME by applying the unary '++' operator to this one. **/\
        inline CLASSNAME operator++() const throw()  { return unary<plonk::inc>(); }\
        /** Create a new CLASSNAME by applying the unary '++' operator to this one. **/\
        inline CLASSNAME operator++ (int) const throw()  { CLASSNAME temp = *this; operator= (unary<plonk::inc>()); return temp; }


#define PLONK_UNARYOPGLOBAL(CLASSNAME,OP)\
    /** Create a new CLASSNAME by applying the unary '##OP##' function to @e operand. */\
    CLASSNAME OP (CLASSNAME const& operand) throw() { return operand.OP(); }

#define PLONK_UNARYOPGLOBAL_TEMPLATE(CLASSNAME,OP)\
    template<class T> PLONK_UNARYOPGLOBAL(CLASSNAME<T>,OP)

#define PLONK_UNARYOPGLOBALS_DEFINE(UNARYOPGLOBAL,CLASSNAME) \
        UNARYOPGLOBAL(CLASSNAME, move)\
        UNARYOPGLOBAL(CLASSNAME, inc)\
        UNARYOPGLOBAL(CLASSNAME, dec)\
        UNARYOPGLOBAL(CLASSNAME, abs)\
        UNARYOPGLOBAL(CLASSNAME, log2)\
        UNARYOPGLOBAL(CLASSNAME, neg)\
        UNARYOPGLOBAL(CLASSNAME, reciprocal)\
        UNARYOPGLOBAL(CLASSNAME, sin)\
        UNARYOPGLOBAL(CLASSNAME, cos)\
        UNARYOPGLOBAL(CLASSNAME, tan)\
        UNARYOPGLOBAL(CLASSNAME, asin)\
        UNARYOPGLOBAL(CLASSNAME, acos)\
        UNARYOPGLOBAL(CLASSNAME, atan)\
        UNARYOPGLOBAL(CLASSNAME, sinh)\
        UNARYOPGLOBAL(CLASSNAME, cosh)\
        UNARYOPGLOBAL(CLASSNAME, tanh)\
        UNARYOPGLOBAL(CLASSNAME, sqrt)\
        UNARYOPGLOBAL(CLASSNAME, log)\
        UNARYOPGLOBAL(CLASSNAME, log10)\
        UNARYOPGLOBAL(CLASSNAME, exp)\
        UNARYOPGLOBAL(CLASSNAME, squared)\
        UNARYOPGLOBAL(CLASSNAME, cubed)\
        UNARYOPGLOBAL(CLASSNAME, ceil)\
        UNARYOPGLOBAL(CLASSNAME, floor)\
        UNARYOPGLOBAL(CLASSNAME, frac)\
        UNARYOPGLOBAL(CLASSNAME, sign)\
        UNARYOPGLOBAL(CLASSNAME, m2f)\
        UNARYOPGLOBAL(CLASSNAME, f2m)\
        UNARYOPGLOBAL(CLASSNAME, a2dB)\
        UNARYOPGLOBAL(CLASSNAME, dB2a)\
        UNARYOPGLOBAL(CLASSNAME, d2r)\
        UNARYOPGLOBAL(CLASSNAME, r2d)\
        UNARYOPGLOBAL(CLASSNAME, distort)\
        UNARYOPGLOBAL(CLASSNAME, zap)

#define PLONK_UNARYOPGLOBALS(CLASSNAME) PLONK_UNARYOPGLOBALS_DEFINE(PLONK_UNARYOPGLOBAL,CLASSNAME)
#define PLONK_UNARYOPGLOBALS_TEMPLATE(CLASSNAME) PLONK_UNARYOPGLOBALS_DEFINE(PLONK_UNARYOPGLOBAL_TEMPLATE,CLASSNAME)

//------------------------------------------------------------------------------

/** Unary arithmetic operator functions.
 
 @defgroup PlonkUnaryOpFunctions Unary arithmetic operator functions 
 @ingroup PlonkOtherUserFunctions
 @{
 */

/** Just returns the input argument. */
template<class Type> inline Type move (Type const& a) throw()          { return a; }

/** Increment by 1. */
template<class Type> inline Type inc (Type const& a) throw()           { return a + Math<Type>::get1(); }

/** Decrement by 1. */
template<class Type> inline Type dec (Type const& a) throw()           { return a - Math<Type>::get1(); }

/** Returns the logarithm base 2 of the input argument. */
template<class Type> inline Type log2 (Type const& a) throw()          { return static_cast<Type> (pl_Log2D (double (a)));}

inline float log2 (float const& a) throw()                             { return pl_Log2F (a); }
inline double log2 (double const& a) throw()                           { return pl_Log2D (a); }

/** Returns the negative of the input argument. */
template<class Type> inline Type neg (Type const& a) throw()           { return -a; }

/** Returns the reciprocal of the input argument (i.e, @f$ \frac{1}{a} @f$). */
template<class Type> inline Type reciprocal (Type const& a) throw()    { return Math<Type>::get1() / a; }

/** Returns the sine of the input argument. */
template<class Type> inline Type sin (Type const& a) throw()           { return static_cast<Type> (::sin (double (a))); }

inline float sin (float const& a) throw()                              { return pl_SinF (a); }
inline double sin (double const& a) throw()                            { return pl_SinD (a); }

/** Returns the cosine of the input argument. */
template<class Type> inline Type cos (Type const& a) throw()           { return static_cast<Type> (::cos (double (a))); }

inline float cos (float const& a) throw()                              { return pl_CosF (a); }
inline double cos (double const& a) throw()                            { return pl_CosD (a); }

/** Returns the tangent of the input argument. */
template<class Type> inline Type tan (Type const& a) throw()           { return static_cast<Type> (::tan (double (a))); }

inline float tan (float const& a) throw()                              { return pl_TanF (a); }
inline double tan (double const& a) throw()                            { return pl_TanD (a); }

/** Returns the arcsine of the input argument. */
template<class Type> inline Type asin (Type const& a) throw()          { return static_cast<Type> (::asin (double (a))); }

inline float asin (float const& a) throw()                              { return pl_AsinF (a); }
inline double asin (double const& a) throw()                            { return pl_AsinD (a); }

/** Returns the arcosine of the input argument. */
template<class Type> inline Type acos (Type const& a) throw()          { return static_cast<Type> (::acos (double (a))); }

inline float acos (float const& a) throw()                              { return pl_AcosF (a); }
inline double acos (double const& a) throw()                            { return pl_AcosD (a); }

/** Returns the arctangent of the input argument. */
template<class Type> inline Type atan (Type const& a) throw()          { return static_cast<Type> (::atan (double (a))); }

inline float atan (float const& a) throw()                              { return pl_AtanF (a); }
inline double atan (double const& a) throw()                            { return pl_AtanD (a); }

/** Returns the hyperbolic sine of the input argument. */
template<class Type> inline Type sinh (Type const& a) throw()          { return static_cast<Type> (::sinh (double (a))); }

inline float sinh (float const& a) throw()                              { return pl_SinhF (a); }
inline double sinh (double const& a) throw()                            { return pl_SinhD (a); }

/** Returns the hyperbolic cosine of the input argument. */
template<class Type> inline Type cosh (Type const& a) throw()          { return static_cast<Type> (::cosh (double (a))); }

inline float cosh (float const& a) throw()                              { return pl_CoshF (a); }
inline double cosh (double const& a) throw()                            { return pl_CoshD (a); }

/** Returns the hyperbolic tangent of the input argument. */
template<class Type> inline Type tanh (Type const& a) throw()          { return static_cast<Type> (::tanh (double (a))); }

inline float tanh (float const& a) throw()                              { return pl_TanhF (a); }
inline double tanh (double const& a) throw()                            { return pl_TanhD (a); }

/** Returns the square root of the input argument. */
template<class Type> inline Type sqrt (Type const& a) throw()          { return static_cast<Type> (::sqrt (double (a))); }

inline float sqrt (float const& a) throw()                              { return pl_SqrtF (a); }
inline double sqrt (double const& a) throw()                            { return pl_SqrtD (a); }

/** Returns the natural logarithm of the input argument. */
template<class Type> inline Type log (Type const& a) throw()           { return static_cast<Type> (::log (double (a))); }

inline float log (float const& a) throw()                              { return pl_LogF (a); }
inline double log (double const& a) throw()                            { return pl_LogD (a); }

/** Returns the logarithm base 10 of the input argument. */
template<class Type> inline Type log10 (Type const& a) throw()         { return static_cast<Type> (::log10 (double (a))); }

inline float log10 (float const& a) throw()                              { return pl_Log10F (a); }
inline double log10 (double const& a) throw()                            { return pl_Log10D (a); }

/** Returns the exponent of the input argument. */
template<class Type> inline Type exp (Type const& a) throw()           { return static_cast<Type> (::exp (double (a))); }

inline float exp (float const& a) throw()                              { return pl_ExpF (a); }
inline double exp (double const& a) throw()                            { return pl_ExpD (a); }

/** Returns the input argument rounded up to the next highest integer. */
template<class Type> inline Type ceil (Type const& a) throw()          { const long n = long (a); return Type (n + 1); }

/** Returns the input argument rounded down to the next lowest integer. */
template<class Type> inline Type floor (Type const& a) throw()         { const long n = long (a); return Type (n); }

/** Returns the fractional part of the input argument. */
template<class Type> inline Type frac (Type const& a) throw()          { const long n = long (a); return a - Type (n); }

/** Returns 0 if the input is 0, -1 if the input is negative or 1 if the input argument is positive. */
template<class Type> inline Type sign (Type const& a) throw()          { const long n = (a == Math<Type>::get0()) ? 0 : (a < Math<Type>::get0()) ? -1 : 1; return Type (n); }

/** Returns the input argument converted from MIDI note numbers to frequency (in Hz). */
template<class Type> inline Type m2f (Type const& a) throw()           { return Math<Type>::get440() * pow(Math<Type>::get2(), (a - Math<Type>::get69()) * Math<Type>::get1_12()); }

/** Returns the input argument converted from frequency (in Hz) to MIDI note numbers. */
template<class Type> inline Type f2m (Type const& a) throw()           { return log2 (a * Math<Type>::get1_440()) * Math<Type>::get12() + Math<Type>::get69(); }

/** Returns the input argument converted from linear amplitude to decibels where 0dB is an amplitude of 1. */
template<class Type> inline Type a2dB (Type const& amp) throw()        { return log10 (amp) * Math<Type>::get20(); }

/** Returns the input argument converted from decibels to linear amplitude where 0dB is an amplitude of 1. */
template<class Type> inline Type dB2a (Type const& db) throw()         { return pow (Math<Type>::get10(), db / Math<Type>::get20()); }

/** Returns the input argument converted from degrees to radians. */
template<class Type> inline Type d2r (Type const& deg) throw()         { return deg * Math<Type>::get1_360() * Math<Type>::get2Pi(); }

/** Returns the input argument converted from radians to degrees. */
template<class Type> inline Type r2d (Type const& deg) throw()         { return deg * Math<Type>::get1_2Pi() * Math<Type>::get360(); }

/** Returns the input argument distorted. */
template<class Type> inline Type distort (Type const& a) throw()       { return a / (Math<Type>::get1() + abs (a)); }

/** Returns the input argument with infinities, NaNs and denormallised numbers removed. 
 from music-dsp list */
template<class Type>
inline Type zap (Type const& x) throw()
{
    static const Type smallest (static_cast<Type> (1e-15));
    static const Type biggest (static_cast<Type> (1e15));
    const Type absx = abs (x);
    return ((absx > smallest) && (absx < biggest)) ? x : Math<Type>::get0();
}

//template<class Type>
//class ClipNormalHelper
//{
//public:
//    static inline Type clipNormal (Type const& a) throw()
//    {
//        return a; // integer type wrap around peak anyway;
//    }
//};
//
//template<>
//class ClipNormalHelper<float>
//{
//public:
//    static inline float clipNormal (float const& a) throw()
//    {
//        return pl_ClipNormalF (a);
//    }
//};
//
//template<>
//class ClipNormalHelper<double>
//{
//public:
//    static inline double clipNormal (double const& a) throw()
//    {
//        return pl_ClipNormalD (a);
//    }
//};
//
//
//template<class Type> 
//inline Type clipNormal (Type const& a) throw()       
//{ 
//    return ClipNormalHelper<Type>::clipNormal (a);
//}


/// @}

#endif // PLONK_INLINEUNARYOPS_H