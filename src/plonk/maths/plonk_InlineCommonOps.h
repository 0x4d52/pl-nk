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
template<class Type> inline Type abs (Type a) throw() { return (a < Type (0)) ? -a : a; }

// common binary

/** Returns minimum of @e a and @e b. 
 @ingroup PlonkBinaryOpFunctions*/
template<class Type> inline Type min (Type a, Type b) throw() { return (a > b) ? b : a; }

/** Returns maximum of @e a and @e b. 
 @ingroup PlonkBinaryOpFunctions*/
template<class Type> inline Type max (Type a, Type b) throw() { return (a < b) ? b : a; }

/** Returns @e a raised to the power @e b. 
 @ingroup PlonkBinaryOpFunctions*/
template<class Type> inline Type pow (Type a, Type b) throw() { return ::pow ((double)a, (double)b);    }

//inline float pow (float a, float b) throw() { return ::powf (a, b);    }
//inline double pow (double a, double b) throw() { return ::pow (a, b);    }
//inline int pow (int a, int b) throw() { return ::pow (double (a), double (b));    }
//inline short pow (short a, short b) throw() { return ::pow (double (a), double (b));    }
//inline Long pow (Long a, Long b) throw() { return ::pow (double (a), double (b));    }
//template<class Type> inline Type pow (Type const& a, Type const& b) throw() { return a.pow (b);    }



#endif // PLONK_INLINECOMMONOPS_H
