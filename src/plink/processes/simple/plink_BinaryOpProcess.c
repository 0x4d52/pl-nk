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

#include "../../core/plink_StandardHeader.h"
#include "plink_BinaryOpProcess.h"

PLINK_BINARYOPPROCESS_DEFINE(Add, F)
PLINK_BINARYOPPROCESS_DEFINE(Sub, F)
PLINK_BINARYOPPROCESS_DEFINE(Mul, F)
PLINK_BINARYOPPROCESS_DEFINE(Div, F)
PLINK_BINARYOPPROCESS_DEFINE(Min, F)
PLINK_BINARYOPPROCESS_DEFINE(Max, F)
PLINK_BINARYOPPROCESS_DEFINE(Pow, F)
PLINK_BINARYOPPROCESS_DEFINE(IsEqualTo, F)
PLINK_BINARYOPPROCESS_DEFINE(IsNotEqualTo, F)
PLINK_BINARYOPPROCESS_DEFINE(IsGreaterThan, F)
PLINK_BINARYOPPROCESS_DEFINE(IsGreaterThanOrEqualTo, F)
PLINK_BINARYOPPROCESS_DEFINE(IsLessThan, F)
PLINK_BINARYOPPROCESS_DEFINE(IsLessThanOrEqualTo, F)
PLINK_BINARYOPPROCESS_DEFINE(Hypot, F)
PLINK_BINARYOPPROCESS_DEFINE(Atan2, F)
PLINK_BINARYOPPROCESS_DEFINE(SumSqr, F)
PLINK_BINARYOPPROCESS_DEFINE(DifSqr, F)
PLINK_BINARYOPPROCESS_DEFINE(SqrSum, F)
PLINK_BINARYOPPROCESS_DEFINE(SqrDif, F)
PLINK_BINARYOPPROCESS_DEFINE(AbsDif, F)
PLINK_BINARYOPPROCESS_DEFINE(Thresh, F)

