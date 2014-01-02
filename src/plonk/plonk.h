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

#ifndef PLONK_H
#define PLONK_H

/* 14/12/2013
 http://cloc.sourceforge.net v 1.52  T=0.5 s (384.0 files/s, 133372.0 lines/s)
 -------------------------------------------------------------------------------
 Language                     files          blank        comment           code
 -------------------------------------------------------------------------------
 C/C++ Header                   159           9837          10882          36611
 C++                             33           1791           1320           6245
 -------------------------------------------------------------------------------
 SUM:                           192          11628          12202          42856
 -------------------------------------------------------------------------------
 */

/* config macors
 PLONK_USEPLINK=1   -   Use Plink for float processes where implemented (also uses optimisations if enabled in Plank)
 */

// this must be before the standard header incase we want it different in user code?
#define PLONK_API 

#include "core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "core/plonk_Headers.h"

#ifdef PLONK_USEPLINK
    #include "graph/simple/plonk_BinaryOpPlink.h"
    #include "graph/simple/plonk_UnaryOpPlink.h"
#endif

END_PLONK_NAMESPACE

using namespace PLONK_NAMESPACE;




/** Plonk modules....
 @defgroup PlonkModules Plonk */

//-------------------------------------------------------------------------plonk
/** Plonk fundamentals...
 @defgroup PlonkFundamentals Fundamentals 
 @ingroup PlonkModules*/

/** Plonk units...
 @defgroup PlonkUnitCategogies Unit categories 
 @ingroup PlonkModules */

/** Other Plonk container classes...
 @defgroup PlonkContainerClasses Plonk container classes 
 @ingroup PlonkModules */

/** Other Plonk user classes...
 @defgroup PlonkOtherUserClasses Other Plonk user classes 
 @ingroup PlonkModules */

/** Other Plonk user functions...
 @defgroup PlonkOtherUserFunctions Other Plonk user functions 
 @ingroup PlonkModules */


/** Plonk extra topics...
 @defgroup PlonkExtras Extras 
 @ingroup PlonkModules */
//-------------------------------------------------------------------------plonk



//------------------------------------------------------------plonk/fundamentals
/** An outline of the core concepts used in Plonk.
 @defgroup PlonkCoreConcepts	Core Plonk concepts. 
 @ingroup PlonkFundamentals */
//------------------------------------------------------------plonk/fundamentals



//-------------------------------------------------------------------plonk/units
/** A list of all oscillator unit classes and their brief descriptions.
 @defgroup GeneratorUnits	Oscillator unit classes. 
 @ingroup PlonkUnitCategogies */

/** A list of all filtering unit classes and their brief descriptions.
 This category contains a list of all the unit classes which filter/EQ a signal.
 This also includes other kinds of signal shaping (e.g., lag and decay).
 @defgroup FilterUnits		Filter unit classes.  
 @ingroup PlonkUnitCategogies */

/** A list of all noise/random-based unit classes and their brief descriptions.
 @defgroup NoiseUnits		Noise unit classes.  
 @ingroup PlonkUnitCategogies */

/** Alist of FFT related UGens.
 @defgroup FFTUnits			FFT-based unit classes.  
 @ingroup PlonkUnitCategogies */

/** A list of all mathematical operator type unit classes and their brief descriptions.
 This listas all the arithmetic and mathematical operators which may be applied to unit
 instances. This includes the (overloaded) arithmentic operators such as +, -, * and /.
 @defgroup MathsUnits		Mathemetical unit classes.	 
 @ingroup PlonkUnitCategogies */

/** A list of all envelope type unit classes and their brief descriptions.
 @defgroup EnvelopeUnits	Envelope unit classes.	 
 @ingroup PlonkUnitCategogies */

/** A list of all delay-based unit classes and their brief descriptions.
 @defgroup DelayUnits		Delay units classes.	 
 @ingroup PlonkUnitCategogies */

/** A list of all control units classes and their brief descriptions.
 This lists unit classes useful for control purposes.
 @defgroup ControlUnits		Control unit classes.	 
 @ingroup PlonkUnitCategogies */

/** A list of all soundfile-based unit classes and their brief descriptions.
 @defgroup SoundFileUnits	Sound file unit classes.  
 @ingroup PlonkUnitCategogies */

/** A list of all converter unit classes and their brief descriptions.
 E.g., for converting sample rates, block sizes and sample types.
 @defgroup ConverterUnits	Converter unit classes. 
 @ingroup PlonkUnitCategogies */

/** A list of other various units.
 @defgroup MiscUnits        Miscellaneous unit class.  
 @ingroup PlonkUnitCategogies */
//-------------------------------------------------------------------plonk/units


#endif // PLONK_H