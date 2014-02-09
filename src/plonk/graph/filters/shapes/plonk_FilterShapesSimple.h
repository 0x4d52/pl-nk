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

#ifndef PLONK_FILTERSHAPESSIMPLE_H
#define PLONK_FILTERSHAPESSIMPLE_H

#include "../plonk_FilterForwardDeclarations.h"


//------------------------------------------------------------------------------


/** One-pole low-pass filter shape. */
template<class SampleType>
class FilterShapeLPFP1Base
:   public FilterShape<SampleType, FilterFormType::P1a, FilterShapeType::LPF>
{
public:
    enum Params
    {
        Frequency,
        NumParams
    };

    typedef FilterForm<SampleType,FilterFormType::P1a>                  FormType;
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
        
    static inline SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }

    static inline void calculate (Data& data) throw()
    {
        const CalcType w0 = Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;        
        const CalcType temp = plonk::exp (-w0);
        
        plonk_assert (temp > CalcType (0));
        
        data.coeffs[FormType::CoeffB1] = temp;
    }
};
typedef FilterShapeLPFP1Base<PLONK_TYPE_DEFAULT> FilterShapeLPFP1;


/** One-pole lag shape. */
template<class SampleType>
class FilterShapeLagBase
:   public FilterShape<SampleType, FilterFormType::P1a, FilterShapeType::LPF>
{
public:
    enum Params
    {
        Duration,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P1a>                  FormType;
    typedef typename FormType::Data                                     FormData;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Duration);
        return keys;
    }
    
    static inline SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }

    static inline void calculate (Data& data) throw()
    {
        const CalcType zero (Math<CalcType>::get0());
        const CalcType log0_001 (Math<CalcType>::getLog0_001());
        
        const CalcType lag = data.params[Duration];
        const CalcType temp = (lag == zero) ? zero : plonk::exp (log0_001 / (lag * data.filterSampleRate));
        
        plonk_assert (temp >= CalcType (0));
        
        data.coeffs[FormType::CoeffB1] = temp;
    }
};
typedef FilterShapeLagBase<PLONK_TYPE_DEFAULT> FilterShapeLag;

/** One-pole aysmmetical lag shape. */
template<class SampleType>
class FilterShapeLagAsymBase
:   public FilterShape<SampleType, FilterFormType::P1c, FilterShapeType::LPF>
{
public:
    enum Params
    {
        Attack,
        Release,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P1c>                  FormType;
    typedef typename FormType::Data                                     FormData;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;
    
    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Attack, IOKey::Release);
        return keys;
    }
    
    static inline SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }
    
    static inline void calculate (Data& data) throw()
    {
        const CalcType zero (Math<CalcType>::get0());
        const CalcType log0_001 (Math<CalcType>::getLog0_001());
        
        const CalcType attack = data.params[Attack];
        const CalcType release = data.params[Release];
        data.coeffs[FormType::CoeffB1u] = (attack == zero) ? zero : plonk::exp (log0_001 / (attack * data.filterSampleRate));
        data.coeffs[FormType::CoeffB1d] = (attack == zero) ? zero : plonk::exp (log0_001 / (release * data.filterSampleRate));
        
        plonk_assert (data.coeffs[FormType::CoeffB1u] >= CalcType (0));
        plonk_assert (data.coeffs[FormType::CoeffB1d] >= CalcType (0));
    }
};
typedef FilterShapeLagAsymBase<PLONK_TYPE_DEFAULT> FilterShapeLagAsym;


/** One-pole high-pass filter shape. */
template<class SampleType>
class FilterShapeHPFP1Base
:   public FilterShape<SampleType, FilterFormType::P1, FilterShapeType::HPF>
{
public:
    enum Params
    {
        Frequency,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P1>                   FormType;
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
    
    static inline SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }

    static inline void calculate (Data& data) throw()
    {
        const CalcType one (Math<CalcType>::get1());
        const CalcType half (Math<CalcType>::get0_5());
        const CalcType w0 =  Math<CalcType>::get2Pi() * data.params[Frequency] * data.filterSampleDuration;
        
        const CalcType temp1 = plonk::exp (-w0);
        
        plonk_assert (temp1 > CalcType (0));
        
        const CalcType temp2 = (one + temp1) * half;
        
        data.coeffs[FormType::CoeffA0] = temp2;
        data.coeffs[FormType::CoeffA1] = -temp2;
        data.coeffs[FormType::CoeffB1] = temp1;
    }
};
typedef FilterShapeHPFP1Base<PLONK_TYPE_DEFAULT> FilterShapeHPFP1;


/** One-pole decay shape. */
template<class SampleType>
class FilterShapeDecayBase 
:   public FilterShape<SampleType, FilterFormType::P1, FilterShapeType::Other>
{
public:
    enum Params
    {
        Duration,
        NumParams
    };
    
    typedef FilterForm<SampleType,FilterFormType::P1>                   FormType;
    typedef typename FormType::Data                                     FormData;
    typedef FilterShapeData<SampleType,FormType::NumCoeffs,NumParams>   Data;
    typedef typename TypeUtility<SampleType>::IndexType                 CalcType;
    typedef NumericalArray<CalcType>                                    CalcTypes;
    typedef FilterUnit<FormType>                                        Unit;

    static IntArray getInputKeys() throw()
    {
        const IntArray keys (IOKey::Duration);
        return keys;
    }
    
    static inline SampleType process (SampleType const& input, Data const& data, FormData& form) throw()
    {
        return FormType::process (input, data.coeffs, form);
    }
            
    static inline void calculate (Data& data) throw()
    {
        const CalcType zero (Math<CalcType>::get0());
        const CalcType one (Math<CalcType>::get1());
        const CalcType log0_001 (Math<CalcType>::getLog0_001());
        
        const CalcType decay = data.params[Duration];
        const CalcType temp = (decay == zero) ? zero : plonk::exp (log0_001 / (decay * data.filterSampleRate));
        
        plonk_assert (temp >= CalcType (0));
        
        data.coeffs[FormType::CoeffA0] = one;
        data.coeffs[FormType::CoeffA1] = zero;
        data.coeffs[FormType::CoeffB1] = temp;
    }
};
typedef FilterShapeDecayBase<PLONK_TYPE_DEFAULT> FilterShapeDecay;



#endif // PLONK_FILTERSHAPESSIMPLE_H