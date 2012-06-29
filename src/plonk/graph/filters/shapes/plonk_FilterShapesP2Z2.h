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

#ifndef PLONK_FILTERSHAPESP2Z2_H
#define PLONK_FILTERSHAPESP2Z2_H

#include "../plonk_FilterShapes.h"

//------------------------------------------------------------------------------

/** Two-pole, two-zero, resonant low-pass filter shape. */
template<class SampleType>
class FilterShapeRLPFBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::LPF>
{
public:
    enum Params
    {
        Frequency,
        Q,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::Q);
        return keys;
    }
        
//    static CalcTypes calculate (const CalcType frequency, const CalcType q, 
//                                const double sampleRate = SampleRate::getDefault()) throw()
//    {
//        Data data;
//        data.params[Frequency] = frequency;
//        data.params[Q] = q;
//        data.filterSampleRate = sampleRate;
//        data.filterSampleDuration = 1.0 / sampleRate;
//        calculate (data);
//        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
//    }

    static inline void calculate (Data& data) throw()
    {
        const CalcType one (Math<CalcType>::get1());
        const CalcType half (Math<CalcType>::get0_5());
        const CalcType two (Math<CalcType>::get2());
        const CalcType w0 =  Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		
        const CalcType alpha = sin_w0 * half / data.params[Q];
        const CalcType temp1 = one / (one + alpha);
        const CalcType temp2 = one - cos_w0;
        
        data.coeffs[FormType::CoeffA0] = temp2 * half * temp1;
        data.coeffs[FormType::CoeffA1] = temp2 * temp1;
        data.coeffs[FormType::CoeffA2] = data.coeffs[FormType::CoeffA0];
        data.coeffs[FormType::CoeffB1] = two * cos_w0 * temp1;
        data.coeffs[FormType::CoeffB2] = (one - alpha) * -temp1;
    }
};
typedef FilterShapeRLPFBase<PLONK_TYPE_DEFAULT> FilterShapeRLPF;


//------------------------------------------------------------------------------

/** Two-pole, two-zero, resonant high-pass filter shape. */
template<class SampleType>
class FilterShapeRHPFBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::HPF>
{
public:
    enum Params
    {
        Frequency,
        Q,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::Q);
        return keys;
    }
    
//    static CalcTypes calculate (const CalcType frequency, const CalcType q, 
//                                const double sampleRate = SampleRate::getDefault()) throw()
//    {
//        Data data;
//        data.params[Frequency] = frequency;
//        data.params[Q] = q;
//        data.filterSampleRate = sampleRate;
//        data.filterSampleDuration = 1.0 / sampleRate;
//        calculate (data);
//        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
//    }
    
    static inline void calculate (Data& data) throw()
    {        
        const CalcType one (Math<CalcType>::get1());
        const CalcType half (Math<CalcType>::get0_5());
        const CalcType two (Math<CalcType>::get2());
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		
        const CalcType alpha = sin_w0 * half / data.params[Q];
        const CalcType temp1 = one / (one + alpha);
        const CalcType temp2 = one + cos_w0;
        
        data.coeffs[FormType::CoeffA0] = temp2 * half * temp1;
        data.coeffs[FormType::CoeffA1] = -temp2 * temp1;
        data.coeffs[FormType::CoeffA2] = data.coeffs[FormType::CoeffA0];
        data.coeffs[FormType::CoeffB1] = two * cos_w0 * temp1;
        data.coeffs[FormType::CoeffB2] = (one - alpha) * -temp1;
    }
};
typedef FilterShapeRHPFBase<PLONK_TYPE_DEFAULT> FilterShapeRHPF;


//------------------------------------------------------------------------------

/** Two-pole, two-zero, low-shelving filter shape. */
template<class SampleType>
class FilterShapeLowShelfBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::LowShelf>
{
public:
    enum Params
    {
        Frequency,
        S,
        Gain,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::S,
                             IOKey::Gain);
        return keys;
    }
    
//    static CalcTypes calculate (const CalcType frequency, const CalcType s, const CalcType gain, 
//                                const double sampleRate = SampleRate::getDefault()) throw()
//    {
//        Data data;
//        data.params[Frequency] = frequency;
//        data.params[S] = s;
//        data.params[Gain] = gain;
//        data.filterSampleRate = sampleRate;
//        data.filterSampleDuration = 1.0 / sampleRate;
//        calculate (data);
//        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
//    }
    
    static inline void calculate (Data& data) throw()
    {
        const CalcType one (Math<CalcType>::get1());
        const CalcType half (Math<CalcType>::get0_5());
        const CalcType two (Math<CalcType>::get2());
        const CalcType ten (Math<CalcType>::get10());
                
        const CalcType a = plonk::pow(ten, data.params[Gain] * (Math<CalcType>::get1_40()));
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		

        const CalcType r2ss = half / plonk::squared (data.params[S]);
        const CalcType alpha = sin_w0 * plonk::sqrt (a) * r2ss;
        const CalcType ap1 = a + one;
        const CalcType am1 = a - one;
        const CalcType temp1 = ap1 * cos_w0;
        const CalcType temp2 = am1 * cos_w0;
        const CalcType temp3 = temp2 + alpha;
        const CalcType temp4 = temp2 - alpha;
        const CalcType temp5 = one / (ap1 + temp3);

        data.coeffs[FormType::CoeffA0] = a * (ap1 - temp3) * temp5;
        data.coeffs[FormType::CoeffA1] = two * a * (am1 - temp1) * temp5;
        data.coeffs[FormType::CoeffA2] = a * (ap1 - temp4) * temp5;
        data.coeffs[FormType::CoeffB1] = two * (am1 + temp1) * temp5;
        data.coeffs[FormType::CoeffB2] = -(ap1 + temp4) * temp5;        
    }
};
typedef FilterShapeLowShelfBase<PLONK_TYPE_DEFAULT> FilterShapeLowShelf;

//------------------------------------------------------------------------------

/** Two-pole, two-zero, high-shelving filter shape. */
template<class SampleType>
class FilterShapeHighShelfBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::HighShelf>
{
public:
    enum Params
    {
        Frequency,
        S,
        Gain,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::S,
                             IOKey::Gain);
        return keys;
    }
    
    static CalcTypes calculate (const CalcType frequency, const CalcType s, const CalcType gain, 
                                const double sampleRate = SampleRate::getDefault()) throw()
    {
        Data data;
        data.params[Frequency] = frequency;
        data.params[S] = s;
        data.params[Gain] = gain;
        data.filterSampleRate = sampleRate;
        data.filterSampleDuration = 1.0 / sampleRate;
        calculate (data);
        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
    }

    static inline void calculate (Data& data) throw()
    {        
        const CalcType one (Math<CalcType>::get1());
        const CalcType half (Math<CalcType>::get0_5());
        const CalcType two (Math<CalcType>::get2());
        const CalcType ten (Math<CalcType>::get10());
        
        const CalcType a = plonk::pow(ten, data.params[Gain] * (Math<CalcType>::get1_40()));
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		
        
        const CalcType r2ss = half / plonk::squared (data.params[S]);
        const CalcType alpha = sin_w0 * plonk::sqrt (a) * r2ss;
        const CalcType ap1 = (a + one);
        const CalcType am1 = (a - one);
        const CalcType temp1 = ap1 * cos_w0;
        const CalcType temp2 = am1 * cos_w0;
        const CalcType temp3 = one / (ap1 - temp2 + alpha);
                
        data.coeffs[FormType::CoeffA0] = a * (ap1 + temp2 + alpha) * temp3;
        data.coeffs[FormType::CoeffA1] = -two * a * (am1 + temp1) * temp3;
        data.coeffs[FormType::CoeffA2] = a * (ap1 + temp2 - alpha) * temp3;
        data.coeffs[FormType::CoeffB1] = -two * (am1 - temp1) * temp3;
        data.coeffs[FormType::CoeffB2] = -(ap1 - temp2 - alpha) * temp3;   
    }
};
typedef FilterShapeHighShelfBase<PLONK_TYPE_DEFAULT> FilterShapeHighShelf;


//------------------------------------------------------------------------------

/** Two-pole, two-zero, notch EQ filter shape. */
template<class SampleType>
class FilterShapeNotchBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::Notch>
{
public:
    enum Params
    {
        Frequency,
        Q,
        Gain,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::Q, 
                             IOKey::Gain);
        return keys;
    }
        
//    static CalcTypes calculate (const CalcType frequency, const CalcType q, const CalcType gain, 
//                                const double sampleRate = SampleRate::getDefault()) throw()
//    {
//        Data data;
//        data.params[Frequency] = frequency;
//        data.params[Q] = q;
//        data.params[Gain] = gain;
//        data.filterSampleRate = sampleRate;
//        data.filterSampleDuration = 1.0 / sampleRate;
//        calculate (data);
//        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
//    }

    static inline void calculate (Data& data) throw()
    {
        const CalcType one (Math<CalcType>::get1());
        const CalcType half (Math<CalcType>::get0_5());
        const CalcType two (Math<CalcType>::get2());
        const CalcType ten (Math<CalcType>::get10());
                
        const CalcType a = plonk::pow(ten, data.params[Gain] * (Math<CalcType>::get1_40()));
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		
        const CalcType alpha = sin_w0 * half / data.params[Q];
        const CalcType temp1 = alpha * a;
        const CalcType temp2 = alpha / a;
        const CalcType temp3 = one / (one + temp2);	
        
        data.coeffs[FormType::CoeffB1] = two * temp3 * cos_w0; 
        data.coeffs[FormType::CoeffA0] = (one + temp1) * temp3; 
        data.coeffs[FormType::CoeffA1] = -data.coeffs[FormType::CoeffB1];
        data.coeffs[FormType::CoeffA2] = (one - temp1) * temp3; 
        data.coeffs[FormType::CoeffB2] = (one - temp2) * -temp3; 
    }
};
typedef FilterShapeNotchBase<PLONK_TYPE_DEFAULT> FilterShapeNotch;

//------------------------------------------------------------------------------

/** Two-pole, two-zero, band-pass filter shape. */
template<class SampleType>
class FilterShapeBPFBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::BPF>
{
public:
    enum Params
    {
        Frequency,
        Bandwidth,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::Bandwidth);
        return keys;
    }
    
//    static CalcTypes calculate (const CalcType frequency, const CalcType bandwidth, 
//                                const double sampleRate = SampleRate::getDefault()) throw()
//    {
//        Data data;
//        data.params[Frequency] = frequency;
//        data.params[Bandwidth] = bandwidth;
//        data.filterSampleRate = sampleRate;
//        data.filterSampleDuration = 1.0 / sampleRate;
//        calculate (data);
//        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
//    }
    
    static inline void calculate (Data& data) throw()
    {   
        const CalcType zero (Math<CalcType>::get0());
        const CalcType one (Math<CalcType>::get1());
        const CalcType two (Math<CalcType>::get2());
        const CalcType logSqrt2 (Math<CalcType>::getLogSqrt2());
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		
        const CalcType alpha = sin_w0 * plonk::sinh (logSqrt2 * (data.params[Bandwidth] * w0) / sin_w0);
        const CalcType temp1 = one / (one + alpha);
                
        data.coeffs[FormType::CoeffA0] = alpha * temp1;
        data.coeffs[FormType::CoeffA1] = zero;
        data.coeffs[FormType::CoeffA2] = -data.coeffs[FormType::CoeffA0];
        data.coeffs[FormType::CoeffB1] = two * cos_w0 * temp1;
        data.coeffs[FormType::CoeffB2] = (one - alpha) * -temp1;
    }
};
typedef FilterShapeBPFBase<PLONK_TYPE_DEFAULT> ShapeBPF;

//------------------------------------------------------------------------------

/** Two-pole, two-zero, band-reject filter shape. */
template<class SampleType>
class FilterShapeBRFBase
:   public FilterShape<SampleType, FilterFormType::P2Z2, FilterShapeType::BRF>
{
public:
    enum Params
    {
        Frequency,
        Bandwidth,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P2Z2>                 FormType;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Frequency, 
                             IOKey::Bandwidth);
        return keys;
    }
    
//    static CalcTypes calculate (const CalcType frequency, const CalcType bandwidth, 
//                                const double sampleRate = SampleRate::getDefault()) throw()
//    {
//        Data data;
//        data.params[Frequency] = frequency;
//        data.params[Bandwidth] = bandwidth;
//        data.filterSampleRate = sampleRate;
//        data.filterSampleDuration = 1.0 / sampleRate;
//        calculate (data);
//        return CalcTypes::withArray (FormType::NumCoeffs, data.coeffs);
//    }
    
    static inline void calculate (Data& data) throw()
    {   
        const CalcType one (Math<CalcType>::get1());
        const CalcType two (Math<CalcType>::get2());
        const CalcType logSqrt2 (Math<CalcType>::getLogSqrt2());
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        const CalcType cos_w0 = plonk::cos (w0);
        const CalcType sin_w0 = plonk::sin (w0);		
        const CalcType alpha = sin_w0 * plonk::sinh (logSqrt2 * (data.params[Bandwidth] * w0) / sin_w0);
        const CalcType temp1 = one / (one + alpha);
        const CalcType temp2 = two * cos_w0 * temp1;
        
        data.coeffs[FormType::CoeffA0] = temp1;
        data.coeffs[FormType::CoeffA1] = -temp2;
        data.coeffs[FormType::CoeffA2] = temp1;
        data.coeffs[FormType::CoeffB1] = temp2;
        data.coeffs[FormType::CoeffB2] = (one - alpha) * -temp1;
    }
};
typedef FilterShapeBRFBase<PLONK_TYPE_DEFAULT> FilterShapeBRF;



#endif // PLONK_FILTERSHAPESP2Z2_H