/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

#ifndef PLONK_FFTENGINE_H
#define PLONK_FFTENGINE_H

#include "plonk_FFTEngineInternal.h"

/** @ingroup PlonkOtherUserClasses */
template<class SampleType>
class FFTEngineBase
{
private:
    FFTEngineBase();
};

/** A platform independent FFT processing engine. 
 This uses Plank to decide which underlying processing engine to use (e.g., FFTReal
 or vDSP). 
 @ingroup PlonkOtherUserClasses */
template<>
class FFTEngineBase<float> : public SmartPointerContainer< FFTEngineInternal<float> >
{
public:
    typedef FFTEngineInternal<float>            Internal;
    typedef SmartPointerContainer<Internal>     Base;
    
    /** Create a new engine with a particular FFT size.
     @param length  The FFT size - this must be a power of 2 or less than 16 (where it will
                    specify the log2 FFT size e.g., 8 = pow(2,8) = 256). */
    FFTEngineBase (const long length = 0) throw()
    :   Base (new Internal (length))
    {
    }
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    FFTEngineBase (FFTEngineBase const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    /** Assignment operator. */
    FFTEngineBase& operator= (FFTEngineBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    /** Apply the FFT to the input and place the result in output.
     This can't be performed in-place (i.e., input and output must not point to the same data or overlap). */
    PLONK_INLINE_LOW void forward (float* output, const float* input) throw()
    {
        this->getInternal()->forward (output, input);
    }
    
    /** Apply the FFT to the input and place the result in output.
     This can't be performed in-place (i.e., input and output must not point to the same data or overlap). */
    PLONK_INLINE_LOW void forward (Buffer& output, Buffer const& input) throw()
    {
        plonk_assert (output.length() >= this->length());
        plonk_assert (input.length() >= this->length());
        this->getInternal()->forward (output.getArray(), input.getArray());
    }    
    
    /** Apply the inverse-FFT to the input and place the result in output.
     This can't be performed in-place (i.e., input and output must not point to the same data or overlap). */
    PLONK_INLINE_LOW void inverse (float* output, const float* input) throw()
    {
        this->getInternal()->inverse (output, input);
    }
    
    /** Apply the inverse-FFT to the input and place the result in output.
     This can't be performed in-place (i.e., input and output must not point to the same data or overlap). */
    PLONK_INLINE_LOW void inverse (Buffer& output, Buffer const& input) throw()
    {
        plonk_assert (output.length() >= this->length());
        plonk_assert (input.length() >= this->length());
        this->getInternal()->inverse (output.getArray(), input.getArray());
    }        
    
    /** Get the FFT size. */
    PLONK_INLINE_LOW long length() const
    {
        return this->getInternal()->length();
    }
    
    /** Get half FFT size. 
     This is just as a convenience as it is already cached for efficiency. */
    PLONK_INLINE_LOW long halfLength() const
    {
        return this->getInternal()->halfLength();
    }

    PLONK_INLINE_LOW long lengthLog2() const
    {
        return this->getInternal()->lengthLog2();
    }

private:
};

typedef FFTEngineBase<PLONK_TYPE_DEFAULT> FFTEngine;


#endif // PLONK_FFTENGINE_H
