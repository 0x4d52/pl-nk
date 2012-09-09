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

#ifndef PLONK_INLINEBINARYOPS_H
#define PLONK_INLINEBINARYOPS_H

#include "plonk_Constants.h"
#include "plonk_InlineCommonOps.h"
#include "plonk_InlineUnaryOps.h"
#include "plonk_InlineMiscOps.h"

//------------------------------------------------------------------------------


#define PLONK_BINARYOP(CLASSNAME, OP) \
        /** Create a new CLASSNAME by applying the binary '##OP##' function to this one and the @e right argument. */\
        inline CLASSNAME OP (CLASSNAME const& right) const throw() { return binary<plonk::OP> (right); }


#define PLONK_BINARYOPS(CLASSNAME) \
        PLONK_BINARYOP(CLASSNAME, addop)\
        PLONK_BINARYOP(CLASSNAME, subop)\
        PLONK_BINARYOP(CLASSNAME, mulop)\
        PLONK_BINARYOP(CLASSNAME, divop)\
        PLONK_BINARYOP(CLASSNAME, modop)\
        PLONK_BINARYOP(CLASSNAME, isEqualTo)\
        PLONK_BINARYOP(CLASSNAME, isNotEqualTo)\
        PLONK_BINARYOP(CLASSNAME, isGreaterThan)\
        PLONK_BINARYOP(CLASSNAME, isGreaterThanOrEqualTo)\
        PLONK_BINARYOP(CLASSNAME, isLessThan)\
        PLONK_BINARYOP(CLASSNAME, isLessThanOrEqualTo)\
        PLONK_BINARYOP(CLASSNAME, hypot)\
        PLONK_BINARYOP(CLASSNAME, pow)\
        PLONK_BINARYOP(CLASSNAME, atan2)\
        PLONK_BINARYOP(CLASSNAME, min)\
        PLONK_BINARYOP(CLASSNAME, max)\
        PLONK_BINARYOP(CLASSNAME, sumsqr)\
        PLONK_BINARYOP(CLASSNAME, difsqr)\
        PLONK_BINARYOP(CLASSNAME, sqrsum)\
        PLONK_BINARYOP(CLASSNAME, sqrdif)\
        PLONK_BINARYOP(CLASSNAME, absdif)\
        PLONK_BINARYOP(CLASSNAME, thresh)\
        PLONK_BINARYOP(CLASSNAME, round)\
        PLONK_BINARYOP(CLASSNAME, trunc)\
        PLONK_BINARYOP(CLASSNAME, clip2)\
        PLONK_BINARYOP(CLASSNAME, decayFeedback)\
        /** Create a new CLASSNAME by applying the binary '+' operator to this and the @e right argument. **/\
        inline CLASSNAME operator+   (CLASSNAME const& right) const throw() { return binary<plonk::addop> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '-' operator to this and the @e right argument. **/\
        inline CLASSNAME operator-   (CLASSNAME const& right) const throw() { return binary<plonk::subop> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '*' operator to this and the @e right argument. **/\
        inline CLASSNAME operator*   (CLASSNAME const& right) const throw() { return binary<plonk::mulop> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '/' operator to this and the @e right argument. **/\
        inline CLASSNAME operator/   (CLASSNAME const& right) const throw() { return binary<plonk::divop> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '%' operator to this and the @e right argument. **/\
        inline CLASSNAME operator%   (CLASSNAME const& right) const throw() { return binary<plonk::modop> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '<' operator to this and the @e right argument. **/\
        inline CLASSNAME operator<   (CLASSNAME const& right) const throw() { return binary<plonk::isLessThan> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '<=' operator to this and the @e right argument. **/\
        inline CLASSNAME operator<=  (CLASSNAME const& right) const throw() { return binary<plonk::isLessThanOrEqualTo> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '>' operator to this and the @e right argument. **/\
        inline CLASSNAME operator>   (CLASSNAME const& right) const throw() { return binary<plonk::isGreaterThan> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '>=' operator to this and the @e right argument. **/\
        inline CLASSNAME operator>=  (CLASSNAME const& right) const throw() { return binary<plonk::isGreaterThanOrEqualTo> ((right)); }\
        /** Create a new CLASSNAME by applying the binary '+' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator+   (RightType const& right) const throw() { return binary<plonk::addop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '-' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator-   (RightType const& right) const throw() { return binary<plonk::subop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '*' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator*   (RightType const& right) const throw() { return binary<plonk::mulop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '/' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator/   (RightType const& right) const throw() { return binary<plonk::divop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '%' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator%   (RightType const& right) const throw() { return binary<plonk::modop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '+' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator+= (RightType const& right) throw() { return operator= (*this + right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '-' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator-= (RightType const& right) throw() { return operator= (*this - right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '*' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator*= (RightType const& right) throw() { return operator= (*this * right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '/' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator/= (RightType const& right) throw() { return operator= (*this / right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '%' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator%= (RightType const& right) throw() { return operator= (*this % right); }\
        /** Create a new CLASSNAME by applying the binary '<' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator<   (RightType const& right) const throw() { return binary<plonk::isLessThan> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '<=' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator<=  (RightType const& right) const throw() { return binary<plonk::isLessThanOrEqualTo> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '>' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator>   (RightType const& right) const throw() { return binary<plonk::isGreaterThan> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '>=' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator>=  (RightType const& right) const throw() { return binary<plonk::isGreaterThanOrEqualTo> (CLASSNAME (right)); }


#define PLONK_BINARYOPGLOBAL(CLASSNAME,OP)\
    /** Create a new CLASSNAME by applying the binary '##OP##' function to the @e left and @e right inputs. */\
    inline CLASSNAME OP (CLASSNAME const& left, CLASSNAME const& right) throw() { return left.OP (right); }

#define PLONK_BINARYOPGLOBAL_TEMPLATE(CLASSNAME,OP)\
    template<class T> PLONK_BINARYOPGLOBAL(CLASSNAME<T>,OP)

#define PLONK_BINARYOPGLOBALS_DEFINE(BINARYOPGLOBAL,CLASSNAME) \
        BINARYOPGLOBAL(CLASSNAME, addop)\
        BINARYOPGLOBAL(CLASSNAME, subop)\
        BINARYOPGLOBAL(CLASSNAME, mulop)\
        BINARYOPGLOBAL(CLASSNAME, divop)\
        BINARYOPGLOBAL(CLASSNAME, modop)\
        BINARYOPGLOBAL(CLASSNAME, isEqualTo)\
        BINARYOPGLOBAL(CLASSNAME, isNotEqualTo)\
        BINARYOPGLOBAL(CLASSNAME, isGreaterThan)\
        BINARYOPGLOBAL(CLASSNAME, isGreaterThanOrEqualTo)\
        BINARYOPGLOBAL(CLASSNAME, isLessThan)\
        BINARYOPGLOBAL(CLASSNAME, isLessThanOrEqualTo)\
        BINARYOPGLOBAL(CLASSNAME, hypot)\
        BINARYOPGLOBAL(CLASSNAME, pow)\
        BINARYOPGLOBAL(CLASSNAME, atan2)\
        BINARYOPGLOBAL(CLASSNAME, min)\
        BINARYOPGLOBAL(CLASSNAME, max)\
        BINARYOPGLOBAL(CLASSNAME, sumsqr)\
        BINARYOPGLOBAL(CLASSNAME, difsqr)\
        BINARYOPGLOBAL(CLASSNAME, sqrsum)\
        BINARYOPGLOBAL(CLASSNAME, sqrdif)\
        BINARYOPGLOBAL(CLASSNAME, absdif)\
        BINARYOPGLOBAL(CLASSNAME, thresh)\
        BINARYOPGLOBAL(CLASSNAME, round)\
        BINARYOPGLOBAL(CLASSNAME, trunc)\
        BINARYOPGLOBAL(CLASSNAME, clip2)\
        BINARYOPGLOBAL(CLASSNAME, decayFeedback)


#define PLONK_BINARYOPGLOBALS(CLASSNAME) PLONK_BINARYOPGLOBALS_DEFINE(PLONK_BINARYOPGLOBAL,CLASSNAME)
#define PLONK_BINARYOPGLOBALS_TEMPLATE(CLASSNAME) PLONK_BINARYOPGLOBALS_DEFINE(PLONK_BINARYOPGLOBAL_TEMPLATE,CLASSNAME)


//------------------------------------------------------------------------------

/** Binary arithmetic operator functions.
  
 @defgroup PlonkBinaryOpFunctions Binary arithmetic operator functions 
 @ingroup PlonkOtherUserFunctions
 @{
 */

/** Convenient inline function for the '+' operator. */
template<class Type> inline Type addop (Type const& a, Type const& b) throw() { return a + b; }

/** Convenient inline function for the '-' operator. */
template<class Type> inline Type subop (Type const& a, Type const& b) throw() { return a - b; }

/** Convenient inline function for the '*' operator. */
template<class Type> inline Type mulop (Type const& a, Type const& b) throw() { return a * b; }

/** Convenient inline function for the '/' operator. */
template<class Type> inline Type divop (Type const& a, Type const& b) throw() { return a / b; }

/** Convenient inline function for the '%' operator. */
template<class Type> inline Type modop (Type const& a, Type const& b) throw() { return (LongLong)a % (LongLong)b; }

/** Convenient inline function for the '%' operator (float). */
inline float modop (float const& a, float const& b) throw() { return pl_ModF (a, b); }

/** Convenient inline function for the '%' operator (double). */
inline double modop (double const& a, double const& b) throw() { return pl_ModD (a, b); }

/** Returns 1 if the inputs are equal otherwise returns 0.  */
template<class Type> inline Type isEqualTo              (Type const& a, Type const& b) throw()  { return (a == b) ? Math<Type>::get1() : Math<Type>::get0(); }

/** Returns 1 if the inputs are not equal otherwise returns 0.  */
template<class Type> inline Type isNotEqualTo           (Type const& a, Type const& b) throw()	{ return (a != b) ? Math<Type>::get1() : Math<Type>::get0(); }

/** Returns 1 if the @e a is greater than @e b otherwise returns 0.  */
template<class Type> inline Type isGreaterThan          (Type const& a, Type const& b) throw()	{ return (a > b)  ? Math<Type>::get1() : Math<Type>::get0(); }

/** Returns 1 if the @e a is greater than or equal to @e b otherwise returns 0.  */
template<class Type> inline Type isGreaterThanOrEqualTo (Type const& a, Type const& b) throw()	{ return (a >= b) ? Math<Type>::get1() : Math<Type>::get0(); }

/** Returns 1 if the @e a is less than @e b otherwise returns 0.  */
template<class Type> inline Type isLessThan             (Type const& a, Type const& b) throw()	{ return (a < b)  ? Math<Type>::get1() : Math<Type>::get0(); }

/** Returns 1 if the @e a is less than or equal to @e b otherwise returns 0.  */
template<class Type> inline Type isLessThanOrEqualTo    (Type const& a, Type const& b) throw()	{ return (a <= b) ? Math<Type>::get1() : Math<Type>::get0(); }

/** Returns @f$ \sqrt{ a^2 + b^2 } @f$.  */
template<class Type> inline Type hypot  (Type const& a, Type const& b) throw() { return static_cast<Type> (::hypot (double (a), double (b))); }

inline float hypot  (float const& a, float const& b) throw() { return pl_HypotF (a, b); }
inline double hypot  (double const& a, double const& b) throw() { return pl_HypotD (a, b); }

/** Returns @f$ atan2(a,b) @f$.  */
template<class Type> inline Type atan2  (Type const& a, Type const& b) throw() { return static_cast<Type> (::atan2 (double (a), double (b))); }

inline float atan2  (float const& a, float const& b) throw() { return pl_Atan2F (a, b); }
inline double atan2  (double const& a, double const& b) throw() { return pl_Atan2D (a, b); }

/** Returns @f$ a * a + b * b @f$.  */
template<class Type> inline Type sumsqr (Type const& a, Type const& b) throw() { return squared (a) + squared (b); }

/** Returns @f$ a * a - b * b @f$.  */
template<class Type> inline Type difsqr (Type const& a, Type const& b) throw() { return squared (a) - squared (b);  }

/** Returns @f$ (a + b) * (a + b) @f$.  */
template<class Type> inline Type sqrsum (Type const& a, Type const& b) throw() { const Type c (a + b); return squared (c); }

/** Returns @f$ (a - b) * (a - b) @f$.  */
template<class Type> inline Type sqrdif (Type const& a, Type const& b) throw() { const Type c (a - b); return squared (c); }

/** Returns the absolute difference between the input arguments.  */
template<class Type> inline Type absdif (Type const& a, Type const& b) throw() { return abs (a - b); }

/** If @e a is less than @e b returns 0 otherwise return @e a. */
template<class Type> inline Type thresh (Type const& a, Type const& b) throw() { return (a < b) ? Math<Type>::get0() : a; }

template<class Type>
inline Type round (Type const& a, Type const& b) throw()
{
	const Type offset = a < Math<Type>::get0() ? Math<Type>::get_0_5() : Math<Type>::get0_5();
	const int n = int (a / b + offset);
	return b * Type (n);
}

template<class Type>
inline Type trunc (Type const& a, Type const& b) throw()
{
	const int n = int (a / b);
	return b * Type (n);
}

template<class Type> inline Type clip2 (Type const& value, Type const& range) throw()     { return clip<Type> (value, -range, range); }
template<class Type> inline Type wrap (Type const& value, Type const& upper) throw()      { return wrap (value, Math<Type>::get0(), upper); }

template<class Type> inline Type decayFeedback (Type const& delayTime, Type const& decayTime) throw()
{
    const Type zero (Math<Type>::get0());
    const Type log001 (Math<Type>::getLog0_001());
        
    if (delayTime > zero)       return Type (plonk::exp (log001 * delayTime / decayTime));
    else if (delayTime < zero)  return -Type (plonk::exp (log001 * delayTime / -decayTime));
    else                        return zero;

}

/// @}

#endif // PLONK_INLINEBINARYOPS_H

