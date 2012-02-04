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

#ifndef PLONK_FILTERCOEFFS2PARAM_H
#define PLONK_FILTERCOEFFS2PARAM_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_FilterForwardDeclarations.h"



/** Filter coefficient generator from two control parameters. */
template<class ShapeType>
class FilterCoeffs2ParamChannelInternal 
:   public ProxyOwnerChannelInternal<typename ShapeType::SampleDataType, 
                                     typename ShapeType::Data>
{
public:
    typedef typename ShapeType::SampleDataType                  SampleType;
    typedef typename ShapeType::Data                            Data;
    typedef typename ShapeType::FormType                        FormType;
    
    typedef ChannelBase<SampleType>                             ChannelType;
        
    typedef ProxyOwnerChannelInternal<SampleType,Data>          Internal;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
        
    FilterCoeffs2ParamChannelInternal (Inputs const& inputs, 
                                       Data const& data, 
                                       BlockSize const& blockSize,
                                       SampleRate const& sampleRate) throw()
    :   Internal (FormType::NumCoeffs, inputs, data, blockSize, sampleRate),
        inputKeys (ShapeType::getInputKeys())
    {
        plonk_assert (ShapeType::NumParams == 2);

        SampleRate& filterSampleRate = this->getInputAsSampleRate (IOKey::FilterSampleRate);
        filterSampleRate.addReceiver (this);
        
        updateFilterSampleRateInData();
    }
    
    ~FilterCoeffs2ParamChannelInternal()
    {
        SampleRate& filterSampleRate = this->getInputAsSampleRate (IOKey::FilterSampleRate);
        filterSampleRate.removeReceiver (this);
    }
            
    Text getName() const throw()
    {
        return "Filter Coeffs 2 Param (" + ShapeType::getFormName() + " / " + ShapeType::getShapeName() + ")";
    }       
    
    IntArray getInputKeys() const throw()
    {
        return inputKeys;
    }    
        
    void initChannel (const int channel) throw()
    {        
        plonk_assert (ShapeType::NumParams == 2);

        UnitType& param0Unit = this->getInputAsUnit (inputKeys.atUnchecked (0));
        UnitType& param1Unit = this->getInputAsUnit (inputKeys.atUnchecked (1));

        plonk_assert (param0Unit.getOverlap (0) == Math<DoubleVariable>::get1());
        plonk_assert (param1Unit.getOverlap (0) == Math<DoubleVariable>::get1());

        if ((channel % this->getNumChannels()) == 0)
        {
            this->setBlockSize (BlockSize::decide (param0Unit.getBlockSize (0),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (param0Unit.getSampleRate (0),
                                                     this->getSampleRate()));
            
            Data& data = this->getState();
            data.params[0] = param0Unit.getValue (0);
            data.params[1] = param1Unit.getValue (0);
            
            ShapeType::calculate (data);
            
            int i;
            for (i = 0; i < FormType::NumCoeffs; ++i)
                this->initProxyValue (i, data.coeffs[i]);            
        }
    }    
    
    void changed (DoubleVariable::Sender const& source) throw()
    {
        SampleRate sampleRateSource = static_cast<SampleRate> (source);
        
        if (sampleRateSource == this->getInputAsSampleRate (IOKey::FilterSampleRate))
        {
            this->updateFilterSampleRateInData();
            return;
        }
                
        Internal::changed (source); // process others
    }    
    
    void updateFilterSampleRateInData() throw()
    {
        const SampleRate& filterSampleRate = this->getInputAsSampleRate (IOKey::FilterSampleRate);
        Data& data = this->getState();

        data.filterSampleRate = filterSampleRate.getValue();
        data.filterSampleDuration = 1.0 / data.filterSampleRate;
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {   
        int i, j;

        Data& data = this->getState();
                            
        const int outputLength = this->getOutputBuffer (0).length();
            
        UnitType& param0Unit = this->getInputAsUnit (inputKeys.atUnchecked (0));
        UnitType& param1Unit = this->getInputAsUnit (inputKeys.atUnchecked (1));
        
        const Buffer param0Buffer (param0Unit.process (info, 0));
        const Buffer param1Buffer (param1Unit.process (info, 0));
        const SampleType* const param0Samples = param0Buffer.getArray();
        const SampleType* const param1Samples = param1Buffer.getArray();
        const int param0Length = param0Buffer.length();
        const int param1Length = param1Buffer.length();
        
        if (outputLength == param0Length)
        {
            if (outputLength == param1Length)
            {
                // full rate (although for brest performance in many cases this should be at a lower rate 
                // than the hardware sample rate and resampled before using with the filter)
                
                for (i = 0; i < outputLength; ++i)
                {
                    data.params[0] = param0Samples[i];
                    data.params[1] = param1Samples[i];
                    
                    ShapeType::calculate (data);
                    
                    for (j = 0; j < FormType::NumCoeffs; ++j)
                        this->getOutputSamples (j) [i] = data.coeffs[j];
                }
            }
            else
            {
                // just use the first sample in the params
                
                data.params[0] = param0Samples[0];
                data.params[1] = param1Samples[0];

                ShapeType::calculate (data);
                
                for (j = 0; j < FormType::NumCoeffs; ++j)
                {
                    SampleType* const samples = this->getOutputSamples (j);
                    const SampleType coeff = data.coeffs[j];

                    for (i = 0; i < outputLength; ++i)
                        samples[i] = coeff;
                }
            }
        }
        else if (outputLength == param1Length)
        {
            // just use the first sample in the first param and the other one at full rate
            data.params[0] = param0Samples[0];

            for (i = 0; i < outputLength; ++i)
            {
                data.params[1] = param1Samples[i];
                
                ShapeType::calculate (data);
                
                for (j = 0; j < FormType::NumCoeffs; ++j)
                    this->getOutputSamples (j) [i] = data.coeffs[j];
            }
        }
        else
        {
            // just use the first sample in the params

            data.params[0] = param0Samples[0];
            data.params[1] = param1Samples[0];
            
            ShapeType::calculate (data);
            
            for (j = 0; j < FormType::NumCoeffs; ++j)
            {
                SampleType* const samples = this->getOutputSamples (j);
                const SampleType coeff = data.coeffs[j];
                
                for (i = 0; i < outputLength; ++i)
                    samples[i] = coeff;
            }            
        }
    }
    
private:
    IntArray inputKeys;
};





/** Filter coefficients generator for an IIR filter using two control parameters. 
 @ingroup FilterUnits ControlUnits */
template<class ShapeType>
class FilterCoeffs2ParamUnit
{
public:        
    typedef typename ShapeType::SampleDataType              SampleType;
    typedef typename ShapeType::Data                        Data;
    typedef typename ShapeType::FormType                    FormType;
        
    typedef FilterCoeffs2ParamChannelInternal<ShapeType>    FilterCoeffsInternal;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    
    static inline UnitInfos getInfo() throw()
    {
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Filter Coeffs 2 Param", "Filter coefficients generator for an IIR filter using two control parameters.",
                         
                         // output
                         FormType::NumCoeffs, 
                         IOKey::Coeffs,             Measure::Coeffs,    0.0,                IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,             Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::Generic,             Measure::Unknown,   IOInfo::NoDefault,  IOLimit::None,
                         IOKey::FilterSampleRate,   Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::BlockSize,          Measure::Samples,   0.0,                IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,     -1.0,               IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    
    /** Filter coefficients from two control parameters. 
     This will generally be for some of the second order filters (e.g., with frequency and Q controls). */
    static UnitType ar (UnitType const& param0,
                        UnitType const& param1,
                        SampleRate const filterSampleRate = SampleRate::getDefault(),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {                
        const IntArray inputKeys = ShapeType::getInputKeys();
        
        plonk_assert (inputKeys.length() == 2);
        
        const int numInputChannels = plonk::max (param0.getNumChannels(), param1.getNumChannels());
        UnitType result (UnitType::emptyWithAllocatedSize (numInputChannels * FormType::NumCoeffs));
        
        Data data;
        Memory::zero (data);
        data.base.sampleRate = -1.0;
        data.base.sampleDuration = -1.0;

        for (int i = 0; i < numInputChannels; ++i)
        {
            Inputs inputs;
            inputs.put (inputKeys.atUnchecked (0), param0[i]);
            inputs.put (inputKeys.atUnchecked (1), param1[i]);
            inputs.put (IOKey::FilterSampleRate, filterSampleRate);
            
            result.add (UnitType::template proxiesFromInputs<FilterCoeffsInternal> (inputs, 
                                                                                    data, 
                                                                                    preferredBlockSize, 
                                                                                    preferredSampleRate));
        }
        
        return result;
    }
};




#endif // PLONK_FILTERCOEFFS2PARAM_H

