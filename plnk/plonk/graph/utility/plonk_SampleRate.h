/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLONK_SAMPLERATE_H
#define PLONK_SAMPLERATE_H

#include "../../containers/variables/plonk_VariableForwardDeclarations.h"
#include "../../containers/variables/plonk_Variable.h"
#include "../../core/plonk_Receiver.h"
#include "../../maths/plonk_InlineCommonOps.h"

class SampleRateDefault;
class SampleRateNoPreference;

/** A specialised Variable<double> for handling sample rates. 
 One or more SampleRate objects store the sample rate for different parts of 
 the audio graph. There are accessors to get the default sample rate, a zero 
 sample rate (used only by constants), and a "no preference" sample rate (which 
 is equal to -1.0). Generators (e.g., oscillators) should have a preference 
 (e.g., the default sample rate) but processors should commonly specify no 
 preference (thus, their sample rate will be decided according to the sample 
 rate of their inputs). 
 @see BlockSize */
class SampleRate : public DoubleVariable
{
public:
    typedef VariableInternal<Base>              Internal;
    typedef DoubleVariable                      Base;    
    typedef WeakPointerContainer<SampleRate>    Weak;
    
    SampleRate() throw();
    
    /** To catch a potential programming error. */
    SampleRate (BlockSize const& blockSize) throw();
    
    /** Create a SampleRate from a double. */
    SampleRate (const double initValue) throw();
    
    /** Create a SampleRate from a DoubleVariable. */
    SampleRate (DoubleVariable const& initValue) throw();
    
    /** Copy constructor. */
    SampleRate (SampleRate const& copy) throw();
    
    SampleRate (SampleRateDefault const& copy) throw();
    SampleRate (SampleRateNoPreference const& copy) throw();
    
    template<class OtherType>
    SampleRate (OtherType const& initValue) throw()
    :   Base (initValue)
    {
    }
    
    const double getSampleDurationInTicks() const throw();
        
    static SampleRate& getDefault() throw();
    static SampleRate& getControlRate() throw();
    static SampleRate& getDefaultBlockRate() throw();
    static const SampleRate getFractionOfDefault(IntVariable const& divisor) throw();
    static const SampleRate getMultipleOfDefault(IntVariable const& factor) throw();
    PLONK_INLINE_LOW static const SampleRate getMultipleOfDefault(const int factor) throw() { return getMultipleOfDefault (IntVariable (factor)); }
    static const SampleRate getMultipleOfDefault(DoubleVariable const& factor) throw();
    PLONK_INLINE_LOW static const SampleRate getMultipleOfDefault(const double factor) throw() { return getMultipleOfDefault (DoubleVariable (factor)); }
    

    static const SampleRate& noPreference() throw();
    static const SampleRate& getSampleRate0() throw();
    
    static SampleRate decide (SampleRate const& inputSampleRate, 
                              SampleRate const& preferredSampleRate) throw();
    
    PLONK_OBJECTARROWOPERATOR(SampleRate);

private:
    static const SampleRate getMultipleOfDefault(BlockSize const& blockSize) throw();

};

typedef NumericalArray<SampleRate> SampleRates;

//------------------------------------------------------------------------------

class SampleRateDefault : public SampleRate
{
public:
    SampleRateDefault() : SampleRate (SampleRate::getDefault()) { }
};

//------------------------------------------------------------------------------

class SampleRateNoPreference : public SampleRate
{
public:
    SampleRateNoPreference() : SampleRate (SampleRate::noPreference()) { }
};


#endif // PLONK_SAMPLERATE_H