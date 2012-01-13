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

#define BINARYOPFUNCTION(TYPE,NAME) TYPE (*NAME)(TYPE, TYPE)

#define BINARYOP(CLASSNAME, OP) \
        /** Create a new CLASSNAME by applying the binary '##OP##' function to this one and the @e right argument. */\
        CLASSNAME OP (CLASSNAME const& right) const throw() { return binary<plonk::OP> (right); }


#define BINARYOPS(CLASSNAME) \
        BINARYOP(CLASSNAME, addop)\
        BINARYOP(CLASSNAME, subop)\
        BINARYOP(CLASSNAME, mulop)\
        BINARYOP(CLASSNAME, divop)\
        BINARYOP(CLASSNAME, isEqualTo)\
        BINARYOP(CLASSNAME, isNotEqualTo)\
        BINARYOP(CLASSNAME, isGreaterThan)\
        BINARYOP(CLASSNAME, isGreaterThanOrEqualTo)\
        BINARYOP(CLASSNAME, isLessThan)\
        BINARYOP(CLASSNAME, isLessThanOrEqualTo)\
        BINARYOP(CLASSNAME, hypot)\
        BINARYOP(CLASSNAME, pow)\
        BINARYOP(CLASSNAME, atan2)\
        BINARYOP(CLASSNAME, min)\
        BINARYOP(CLASSNAME, max)\
        BINARYOP(CLASSNAME, sumsqr)\
        BINARYOP(CLASSNAME, difsqr)\
        BINARYOP(CLASSNAME, sqrsum)\
        BINARYOP(CLASSNAME, sqrdif)\
        BINARYOP(CLASSNAME, absdif)\
        BINARYOP(CLASSNAME, thresh)\
        BINARYOP(CLASSNAME, round)\
        BINARYOP(CLASSNAME, trunc)\
        BINARYOP(CLASSNAME, clip2)\
        /** Create a new CLASSNAME by applying the binary '+' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator+   (RightType const& right) const throw() { return binary<plonk::addop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '-' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator-   (RightType const& right) const throw() { return binary<plonk::subop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '*' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator*   (RightType const& right) const throw() { return binary<plonk::mulop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '/' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator/   (RightType const& right) const throw() { return binary<plonk::divop> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '+' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator+= (RightType const& right) throw() { return operator= (*this + right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '-' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator-= (RightType const& right) throw() { return operator= (*this - right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '*' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator*= (RightType const& right) throw() { return operator= (*this * right); }\
        /** Create a new CLASSNAME and assign it to this one by applying the binary '/' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME& operator/= (RightType const& right) throw() { return operator= (*this / right); }\
        /** Create a new CLASSNAME by applying the binary '<' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator<   (RightType const& right) const throw() { return binary<plonk::isLessThan> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '<=' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator<=  (RightType const& right) const throw() { return binary<plonk::isLessThanOrEqualTo> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '>' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator>   (RightType const& right) const throw() { return binary<plonk::isGreaterThan> (CLASSNAME (right)); }\
        /** Create a new CLASSNAME by applying the binary '>=' operator to this and the @e right argument. **/\
        template<class RightType> inline CLASSNAME operator>=  (RightType const& right) const throw() { return binary<plonk::isGreaterThanOrEqualTo> (CLASSNAME (right)); }


#define BINARYOPGLOBAL(CLASSNAME,OP)\
    /** Create a new CLASSNAME by applying the binary '##OP##' function to the @e left and @e right inputs. */\
    template<class T> CLASSNAME<T> OP (CLASSNAME<T> const& left, CLASSNAME<T> const& right) throw() { return left.binary<plonk::OP> (right); }

#define BINARYOPGLOBALS(CLASSNAME) \
        BINARYOPGLOBAL(CLASSNAME, addop)\
        BINARYOPGLOBAL(CLASSNAME, subop)\
        BINARYOPGLOBAL(CLASSNAME, mulop)\
        BINARYOPGLOBAL(CLASSNAME, divop)\
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
        BINARYOPGLOBAL(CLASSNAME, clip2)


//------------------------------------------------------------------------------

/** Binary arithmetic operator functions.
  
 @defgroup PlonkBinaryOpFunctions Binary arithmetic operator functions 
 @ingroup PlonkOtherUserFunctions
 @{
 */

/** Convenient inline function for the '+' operator. */
template<class Type> inline Type addop (Type a, Type b) throw() { return a + b; }

/** Convenient inline function for the '-' operator. */
template<class Type> inline Type subop (Type a, Type b) throw() { return a - b; }

/** Convenient inline function for the '*' operator. */
template<class Type> inline Type mulop (Type a, Type b) throw() { return a * b; }

/** Convenient inline function for the '/' operator. */
template<class Type> inline Type divop (Type a, Type b) throw() { return a / b; }

/** Returns 1 if the inputs are equal otherwise returns 0.  */
template<class Type> inline Type isEqualTo              (Type a, Type b) throw()    { return (a == b) ? Type (1) : Type (0); }

/** Returns 1 if the inputs are not equal otherwise returns 0.  */
template<class Type> inline Type isNotEqualTo           (Type a, Type b) throw()	{ return (a != b) ? Type (1) : Type (0); }

/** Returns 1 if the @e a is greater than @e b otherwise returns 0.  */
template<class Type> inline Type isGreaterThan          (Type a, Type b) throw()	{ return (a > b)  ? Type (1) : Type (0); }

/** Returns 1 if the @e a is greater than or equal to @e b otherwise returns 0.  */
template<class Type> inline Type isGreaterThanOrEqualTo (Type a, Type b) throw()	{ return (a >= b) ? Type (1) : Type (0); }

/** Returns 1 if the @e a is less than @e b otherwise returns 0.  */
template<class Type> inline Type isLessThan             (Type a, Type b) throw()	{ return (a < b)  ? Type (1) : Type (0); }

/** Returns 1 if the @e a is less than or equal to @e b otherwise returns 0.  */
template<class Type> inline Type isLessThanOrEqualTo    (Type a, Type b) throw()	{ return (a <= b) ? Type (1) : Type (0); }

/** Returns @f$ \sqrt{ a^2 + b^2 } @f$.  */
template<class Type> inline Type hypot  (Type a, Type b) throw() { return static_cast<Type> (::hypot (double (a), double (b))); }

/** Returns @f$ atan2(a,b) @f$.  */
template<class Type> inline Type atan2  (Type a, Type b) throw() { return static_cast<Type> (::atan2 (double (a), double (b))); }

/** Returns @f$ a * a + b * b @f$.  */
template<class Type> inline Type sumsqr (Type a, Type b) throw() { return a * a + b * b; }

/** Returns @f$ a * a - b * b @f$.  */
template<class Type> inline Type difsqr (Type a, Type b) throw() { return a * a - b * b; }

/** Returns @f$ (a + b) * (a + b) @f$.  */
template<class Type> inline Type sqrsum (Type a, Type b) throw() { const Type c = a + b; return c * c; }

/** Returns @f$ (a - b) * (a - b) @f$.  */
template<class Type> inline Type sqrdif (Type a, Type b) throw() { const Type c = a - b; return c * c; }

/** Returns the absolute difference between the input arguments.  */
template<class Type> inline Type absdif (Type a, Type b) throw() { return abs (a - b); }

/** If @e a is less than @e b returns 0 otherwise return @e a. */
template<class Type> inline Type thresh (Type a, Type b) throw() { return (a < b) ? Type (0) : a; }

template<class Type>
inline Type round (Type a, Type b) throw()
{
	const Type offset = a < 0 ? Type (-0.5) : Type (0.5);
	const int n = int (a / b + offset);
	return b * Type (n);
}

template<class Type>
inline Type trunc (Type a, Type b) throw()
{
	const int n = int (a / b);
	return b * Type (n);
}

template<class Type> inline Type clip2 (Type value, Type range) throw()		{ return clip<Type> (value, -range, range); }
template<class Type> inline Type wrap (Type value, Type upper) throw()      { return wrap (value, Type (0), upper); }

/// @}

#endif // PLONK_INLINEBINARYOPS_H

