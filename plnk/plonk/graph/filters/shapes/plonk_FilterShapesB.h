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

#ifndef PLONK_FILTERSHAPESB_H
#define PLONK_FILTERSHAPESB_H

#include "../plonk_FilterShapes.h"


//------------------------------------------------------------------------------

/** Butterworth 2nd order low-pass filter shape. */
template<class SampleType>
class FilterShapeLPFBase
:   public FilterShape<SampleType, FilterFormType::B2, FilterShapeType::LPF>
{
public:
    enum Params
    {
        Frequency,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::B2>                   FormType;
    typedef typename FormType::Data                                     FormData;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency);
        return keys;
    }
    
    static PLONK_INLINE_LOW SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }
                
    static PLONK_INLINE_LOW void calculate (Data& data) throw()
    {        
        const CalcType one (Math<CalcType>::get1());
        const CalcType two (Math<CalcType>::get2());
        const CalcType negtwo (-two);
        const CalcType w0 = Math<CalcType>::getPi() * data.params[Frequency] * data.filterSampleDuration;
        
		const CalcType temp1 = one / plonk::tan (w0);
		const CalcType temp2 = temp1 * temp1;
		const CalcType temp3 = temp1 * Math<CalcType>::getSqrt2();
        
        data.coeffs[FormType::CoeffA0] = one / (one + temp3 + temp2);
        data.coeffs[FormType::CoeffA1] = two;
        data.coeffs[FormType::CoeffA2] = one;
        data.coeffs[FormType::CoeffB1] = negtwo * (one - temp2) * data.coeffs[FormType::CoeffA0];
        data.coeffs[FormType::CoeffB2] = -(one - temp3 + temp2) * data.coeffs[FormType::CoeffA0];
        
    }
};
typedef FilterShapeLPFBase<PLONK_TYPE_DEFAULT> FilterShapeLPF;

//------------------------------------------------------------------------------

/** Butterworth 2nd order high-pass filter shape. */
template<class SampleType>
class FilterShapeHPFBase
:   public FilterShape<SampleType, FilterFormType::B2, FilterShapeType::HPF>
{
public:
    enum Params
    {
        Frequency,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::B2>                   FormType;
    typedef typename FormType::Data                                     FormData;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency);
        return keys;
    }
    
    static PLONK_INLINE_LOW SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }
        
    static PLONK_INLINE_LOW void calculate (Data& data) throw()
    {
        const CalcType one (Math<CalcType>::get1());
        const CalcType two (Math<CalcType>::get2());
        const CalcType negtwo (-two);
        const CalcType w0 = Math<CalcType>::getPi() * data.params[Frequency] * data.filterSampleDuration;
        
		const CalcType temp1 = plonk::tan (w0);
		const CalcType temp2 = temp1 * temp1;
		const CalcType temp3 = temp1 * Math<CalcType>::getSqrt2();
        
        data.coeffs[FormType::CoeffA0] = one / (one + temp3 + temp2);
        data.coeffs[FormType::CoeffA1] = negtwo;
        data.coeffs[FormType::CoeffA2] = one;
        data.coeffs[FormType::CoeffB1] = two * (one - temp2) * data.coeffs[FormType::CoeffA0];
        data.coeffs[FormType::CoeffB2] = -(one - temp3 + temp2) * data.coeffs[FormType::CoeffA0];
    }
};
typedef FilterShapeHPFBase<PLONK_TYPE_DEFAULT> FilterShapeHPF;




#endif // PLONK_FILTERSHAPESB_H