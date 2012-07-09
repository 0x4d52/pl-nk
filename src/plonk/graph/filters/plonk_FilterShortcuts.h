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

#ifndef PLONK_FILTERSHORTCUTS_H
#define PLONK_FILTERSHORTCUTS_H

#include "plonk_FilterForwardDeclarations.h"
#include "shapes/plonk_FilterShapesB.h"
#include "shapes/plonk_FilterShapesP2Z2.h"
#include "shapes/plonk_FilterShapesSimple.h"

// one pole

/** One-pole low-pass filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class LPFP1Unit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeLPFP1Base<SampleType>                FilterShape;
    typedef FilterCoeffs1ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("LPFP1", "One-pole low-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }    
    
    /** Create a one-pole low-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create control rate a one-pole low-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef LPFP1Unit<PLONK_TYPE_DEFAULT> LPFP1;

/** Exponential lag filter. 
 Useful for smoothing control signals. 
 @ingroup AllUnits FilterUnits ControlUnits */
template<class SampleType>
class LagUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeLagBase<SampleType>                  FilterShape;
    typedef FilterCoeffs1ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Lag", "One-pole exponential lag filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount,  
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Duration,           Measure::Seconds,   0.2,                IOLimit::Minimum,   Measure::Seconds,   0.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }        
    
    /** Creates an exponential lag filter (one-pole low-pass filter).
     @param input The input signal to filter.
     @param duration  The -60dB lag time. 
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */    
    static UnitType ar (UnitType const& input,
                        UnitType const& duration = SampleType (0.2),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (duration, input.getSampleRates());

        if (duration.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Creates a control rate exponential lag filter (one-pole low-pass filter).
     @param input The input signal to filter.
     @param duration  The -60dB lag time. 
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& duration = SampleType (0.2),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (duration, input.getSampleRates());
        
        if (duration.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input.kr(), coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef LagUnit<PLONK_TYPE_DEFAULT> Lag;

/** One-pole high-pass filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class HPFP1Unit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeHPFP1Base<SampleType>                FilterShape;
    typedef FilterCoeffs1ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HPFP1", "One-pole high-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }        
    
    /** Create a one-pole high-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */    
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());

        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate one-pole high-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef HPFP1Unit<PLONK_TYPE_DEFAULT> HPFP1;

/** Exponential decay filter. 
 Useful for creating simple decaying envlopes from impulses. 
 @ingroup AllUnits FilterUnits ControlUnits*/
template<class SampleType>
class DecayUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeDecayBase<SampleType>                FilterShape;
    typedef FilterCoeffs1ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Decay", "One-pole deacy filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,    0.0,               IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Duration,           Measure::Seconds,   0.2,                IOLimit::Minimum,   Measure::Seconds,   0.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }            
    
    /** Creates an exponential deacy filter.
     @param input The input signal to filter.
     @param duration  The -60dB decay time. 
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */        
    static UnitType ar (UnitType const& input,
                        UnitType const& duration = SampleType (0.2),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (duration, input.getSampleRates());

        if (duration.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Creates a control rate exponential deacy filter.
     @param input The input signal to filter.
     @param duration  The -60dB decay time. 
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& duration = SampleType (0.2),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (duration, input.getSampleRates());
        
        if (duration.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef DecayUnit<PLONK_TYPE_DEFAULT> Decay;


// biquads

/** Resonant low-pass filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class RLPFUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeRLPFBase<SampleType>                 FilterShape;
    typedef FilterCoeffs2ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;

    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
                        
        return UnitInfo ("RLPF", "Resonant low-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Q,                  Measure::Q,         1.0,                IOLimit::Clipped,   Measure::Q,                 0.01, 100.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);    
    }
    
    /** Create a resonant low-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param q The Q factor.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */        
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& q = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, q, input.getSampleRates());

        if ((frequency.isConstant() == false) || (q.isConstant() == false))
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate resonant low-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param q The Q factor.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& q = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, q, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef RLPFUnit<PLONK_TYPE_DEFAULT> RLPF;

/** Resonant high-pass filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class RHPFUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeRHPFBase<SampleType>                 FilterShape;
    typedef FilterCoeffs2ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("RHPF", "Resonant high-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Q,                  Measure::Q,         1.0,                IOLimit::Clipped,   Measure::Q,                 0.01, 100.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);    
    }
    
    /** Create a resonant high-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param q The Q factor.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */            
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& q = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, q, input.getSampleRates());
        
        if ((frequency.isConstant() == false) || (q.isConstant() == false))
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate resonant high-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param q The Q factor.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& q = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, q, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef RHPFUnit<PLONK_TYPE_DEFAULT> RHPF;

/** Low-shelving (bass) filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class LowShelfUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeLowShelfBase<SampleType>             FilterShape;
    typedef FilterCoeffs3ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("LowShelf", "Low shelving filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,               IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::S,                  Measure::Slope,     1.0,                IOLimit::Clipped,   Measure::Slope,             0.01, 100.0,
                         IOKey::Gain,               Measure::Decibels,  0.0,                IOLimit::None,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);   
    }
    
    /** Create a low-shelving filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point turnover frequency. 
     @param s The slope factor (keep to 1 or below for no strange resonances).
     @param gain The boost/cut gain in dB.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */                
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& s = SampleType (1),
                        UnitType const& gain = SampleType (0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, s, gain, input.getSampleRates());
        
        if ((frequency.isConstant() == false) || 
            (s.isConstant() == false) ||
            (gain.isConstant() == false))
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate low-shelving filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point turnover frequency. 
     @param s The slope factor (keep to 1 or below for no strange resonances).
     @param gain The boost/cut gain in dB.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& s = SampleType (1),
                        UnitType const& gain = SampleType (0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, s, gain, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef LowShelfUnit<PLONK_TYPE_DEFAULT> LowShelf;

/** High-shelving (treble) filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class HighShelfUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeHighShelfBase<SampleType>            FilterShape;
    typedef FilterCoeffs3ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HighShelf", "High shelving filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::S,                  Measure::Slope,     1.0,                IOLimit::Clipped,   Measure::Slope,             0.01, 100.0,
                         IOKey::Gain,               Measure::Decibels,  0.0,                IOLimit::None,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);   
    }
    
    /** Create a high-shelving filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point turnover frequency. 
     @param s The slope factor (keep to 1 or below for no strange resonances).
     @param gain The boost/cut gain in dB.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */                    
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& s = SampleType (1),
                        UnitType const& gain = SampleType (0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, s, gain, input.getSampleRates());
        
        if ((frequency.isConstant() == false) || 
            (s.isConstant() == false) ||
            (gain.isConstant() == false))
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate high-shelving filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point turnover frequency. 
     @param s The slope factor (keep to 1 or below for no strange resonances).
     @param gain The boost/cut gain in dB.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& s = SampleType (1),
                        UnitType const& gain = SampleType (0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, s, gain, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef HighShelfUnit<PLONK_TYPE_DEFAULT> HighShelf;

/** Notch filter (i.e, parametric mid-EQ). 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class NotchUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeNotchBase<SampleType>                FilterShape;
    typedef FilterCoeffs3ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Notch", "Notch filter.",
                     
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Q,                  Measure::Q,         1.0,                IOLimit::Clipped,   Measure::Q,                 0.01, 100.0,
                         IOKey::Gain,               Measure::Decibels,  0.0,                IOLimit::None,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);   
    }
    
    /** Create a notch filter.
     @param input The input signal to filter.
     @param frequency  The centre frequency. 
     @param q The Q factor.
     @param gain The boost/cut gain in dB.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */                
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& q = SampleType (1),
                        UnitType const& gain = SampleType (0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, q, gain, input.getSampleRates());

        if ((frequency.isConstant() == false) || 
            (q.isConstant() == false) ||
            (gain.isConstant() == false))
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate notch filter.
     @param input The input signal to filter.
     @param frequency  The centre frequency. 
     @param q The Q factor.
     @param gain The boost/cut gain in dB.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& q = SampleType (1),
                        UnitType const& gain = SampleType (0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, q, gain, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef NotchUnit<PLONK_TYPE_DEFAULT> Notch;

/** Bandpass filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class BPFUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeBPFBase<SampleType>                  FilterShape;
    typedef FilterCoeffs2ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("BPF", "Band-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Bandwidth,          Measure::Octaves,   1.0,                IOLimit::Clipped,   Measure::Octaves,           0.001, 10.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);    
    }
    
    /** Create a bandpass filter.
     @param input The input signal to filter.
     @param frequency  The centre frequency. 
     @param bandwidth The bandwidth of the passband in octaves.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */                    
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& bandwidth = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, bandwidth, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate bandpass filter.
     @param input The input signal to filter.
     @param frequency  The centre frequency. 
     @param bandwidth The bandwidth of the passband in octaves.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& bandwidth = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, bandwidth, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef BPFUnit<PLONK_TYPE_DEFAULT> BPF;

/** Bandreject (band-elimination) filter. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class BRFUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeBRFBase<SampleType>                  FilterShape;
    typedef FilterCoeffs2ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("BRF", "Band-reject filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Bandwidth,          Measure::Octaves,   1.0,                IOLimit::Clipped,   Measure::Octaves,           0.001, 10.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);    
    }
    
    /** Create a bandreject filter.
     @param input The input signal to filter.
     @param frequency  The centre frequency. 
     @param bandwidth The bandwidth of the stopband in octaves.
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */                        
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& bandwidth = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, bandwidth, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate bandreject filter.
     @param input The input signal to filter.
     @param frequency  The centre frequency. 
     @param bandwidth The bandwidth of the stopband in octaves.
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& bandwidth = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, bandwidth, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef BRFUnit<PLONK_TYPE_DEFAULT> BRF;


// butterworth

/** Butterworth 2nd-order low-pass filter.
 This has good linearity in the passband. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class LPFUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeLPFBase<SampleType>                  FilterShape;
    typedef FilterCoeffs1ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    typedef typename FormType::Data                         FormData;
    typedef typename FilterShape::Data                      FilterShapeData;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
                
        return UnitInfo ("LPF", "Butterworth 2nd order low-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);    
    }
        
    static inline SampleType process (SampleType const& input, 
                                      FilterShapeData const& shape, 
                                      FormData& form) throw()
    {
        return FormType::process (input, shape.coeffs, form);
    }
    
    /** Create a simple Butterworth 2nd-order low-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */    
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());

        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a simple control rate Butterworth 2nd-order low-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef LPFUnit<PLONK_TYPE_DEFAULT> LPF;

/** Butterworth 2nd-order high-pass filter.
 This has good linearity in the passband. 
 @ingroup AllUnits FilterUnits */
template<class SampleType>
class HPFUnit
{
public:
    typedef UnitBase<SampleType>                            UnitType;
    typedef FilterShapeHPFBase<SampleType>                  FilterShape;
    typedef FilterCoeffs1ParamUnit<FilterShape>             FilterCoeffsType;
    typedef typename FilterCoeffsType::FormType             FormType;
    typedef FilterUnit<FormType>                            FilterType;
    typedef ResampleUnit<SampleType>                        ResampleType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HPF", "Butterworth 2nd order high-pass filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::Unknown,   0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Frequency,          Measure::Hertz,     1200.0,             IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,    -1.0,                IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);    
    }
    
    /** Create a simple Butterworth 2nd-order high-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset.
     @param preferredBlockSize (Optional) The preferred block size for the process.
     @param preferredSampleRate (Optional) The preferred sample rate for the process. */        
    static UnitType ar (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::ar (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a simple control rate Butterworth 2nd-order high-pass filter.
     @param input The input signal to filter.
     @param frequency  The -3dB point cut-off frequency. 
     @param mul An optional multiplier.
     @param add An optional offset. */
    static UnitType kr (UnitType const& input,
                        UnitType const& frequency = SampleType (1200),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        UnitType coeffs = FilterCoeffsType::ar (frequency, input.getSampleRates());
        
        if (frequency.isConstant() == false)
            for (int i = 0; i < coeffs.getNumChannels(); ++i)
                coeffs.put (i, ResampleType::kr (coeffs[i]));
        
        return FilterType::ar (input, coeffs, mul, add, 
                               BlockSize::getControlRateBlockSize(), 
                               SampleRate::getControlRate());
    }    
};
typedef HPFUnit<PLONK_TYPE_DEFAULT> HPF;



#endif // PLONK_FILTERSHORTCUTS_H