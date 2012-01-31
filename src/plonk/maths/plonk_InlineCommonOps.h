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

#ifndef PLONK_INLINECOMMONOPS_H
#define PLONK_INLINECOMMONOPS_H

#include <math.h>
#include "plonk_Constants.h"


// common unary

/** Returns the absolute of the input argument. 
 @ingroup PlonkUnaryOpFunctions*/
template<class Type> inline Type abs (Type const& a) throw() { return (a < Math<Type>::get0()) ? -a : a; }

// common binary

/** Returns minimum of @e a and @e b. 
 @ingroup PlonkBinaryOpFunctions*/
template<class Type> inline Type min (Type const& a, Type const& b) throw() { return (a > b) ? b : a; }

/** Returns maximum of @e a and @e b. 
 @ingroup PlonkBinaryOpFunctions*/
template<class Type> inline Type max (Type const& a, Type const& b) throw() { return (a < b) ? b : a; }

/** Returns @e a raised to the power @e b. 
 @ingroup PlonkBinaryOpFunctions*/
template<class Type> inline Type pow (Type const& a, Type const& b) throw() { return ::pow ((double)a, (double)b);    }

/** Returns the input argument squared. */
template<class Type> inline Type squared (Type const& a) throw()       { return a * a; }

/** Returns the input argument cubed. */
template<class Type> inline Type cubed (Type const& a) throw()         { return a * a * a;	}


#endif // PLONK_INLINECOMMONOPS_H
