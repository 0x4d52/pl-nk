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

#ifndef PLONK_FILTER_H
#define PLONK_FILTER_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_FilterForwardDeclarations.h"


/** A generic IIR filter. */
template<class FormType>
class FilterChannelInternal 
:   public ChannelInternal<typename FormType::SampleDataType, 
                           typename FormType::Data>
{
public:
    typedef typename FormType::SampleDataType                       SampleType;
    typedef typename FormType::Data                                 Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef FilterChannelInternal<FilterFormType>                   FilterInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef NumericalArray2D<ChannelType,UnitType>                  UnitsType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
        
    FilterChannelInternal (Inputs const& inputs, 
                           Data const& data, 
                           BlockSize const& blockSize,
                           SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
            
    Text getName() const throw()
    {
        return "Filter (" + FormType::getName() + ")";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Coeffs);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {        
        Inputs channelInputs;
        
        channelInputs.put (IOKey::Generic, 
                           this->getInputAsUnit (IOKey::Generic).getChannel (index));
        
        const UnitType& coeffs = this->getInputAsUnit (IOKey::Coeffs);
        UnitsType groupedCoeffs = coeffs.group (FormType::NumCoeffs);
        UnitType channelCoeffs = groupedCoeffs.wrapAt (index);
        
//        plonk_assert (channelCoeffs.getNumChannels() == FormType::NumCoeffs);
//        
//        for (int i = 0; i < FormType::NumCoeffs; ++i)
//            channelCoeffs.put (i, channelCoeffs.atUnchecked (i).getChannel (index));
        
        channelInputs.put (IOKey::Coeffs, 
                           channelCoeffs);
        
        return new FilterChannelInternal (channelInputs, 
                                          this->getState(), 
                                          this->getBlockSize(), 
                                          this->getSampleRate());     
        
//        return this;
    }
    
    void initChannel (const int channel) throw()
    {        
        const UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        
        this->setBlockSize (BlockSize::decide (inputUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (inputUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (inputUnit.getOverlap (channel));
        
        this->initValue (SampleType (0));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {                
        FormType::process (this->getOutputSamples(),
                           this->getOutputBuffer().length(), 
                           this->getInputAsUnit (IOKey::Generic), 
                           this->getInputAsUnit (IOKey::Coeffs), 
                           this->getState(),
                           info, 
                           channel);
    }
    
private:
};

//------------------------------------------------------------------------------

/** A generic filter. 
 This is broadly for internal use. It is employed by the various standard filters.
 @see LPFUnit LPFP1Unit RLPFUnit HPFUnit HPFP1Unit RHPFUnit BPFUnit BRFUnit LowShelfUnit HighShelfUnit NotchUnit LagUnit DecayUnit
 @ingroup FilterUnits */
template<class FormType>
class FilterUnit
{
public:    
    typedef typename FormType::SampleDataType       SampleType;

    typedef FilterChannelInternal<FormType>         FilterInternal;
    typedef typename FilterInternal::Data           Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         ChannelInternalType;
    typedef UnitBase<SampleType>                    UnitType;
    typedef NumericalArray2D<ChannelType,UnitType>  UnitsType;
    typedef InputDictionary                         Inputs;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::noPreference().getValue();
        const double sampleRate = SampleRate::noPreference().getValue();
//        const double peak = (double)TypeUtility<SampleType>::getTypePeak(); // will be innaccurate for LongLong
        
        return UnitInfo ("Filter", "A generic IIR filter.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,     Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,     Measure::None,
                         IOKey::Coeffs,     Measure::Coeffs,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
                
    /** Create a generic filter from the coefficients. */
    static UnitType ar (UnitType const& input,
                        UnitType const& coeffs, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {             
        // re: full templating - could say that filters are only supported with float or double types?
        
        const int requiredCoeffs = FormType::NumCoeffs;
        const int numCoeffChannels = coeffs.getNumChannels();
        
        plonk_assert (numCoeffChannels > 0);
        plonk_assert ((numCoeffChannels % requiredCoeffs) == 0);
        
        const int numOutputChannels = plonk::max (input.getNumChannels(), numCoeffChannels / requiredCoeffs);
        UnitType result (UnitType::withSize (numOutputChannels));
        
        Data data;
        Memory::zero (data);
        data.base.sampleRate = -1.0;        
        data.base.sampleDuration = -1.0;
                
        for (int i = 0; i < numOutputChannels; ++i)
        {
            Inputs inputs;
            inputs.put (IOKey::Generic, input);
            inputs.put (IOKey::Coeffs, coeffs);
            
            ChannelInternalType* internal = new FilterInternal (inputs, 
                                                                data, 
                                                                preferredBlockSize, 
                                                                preferredSampleRate);
            internal->initChannel (i);
            
            result.put (i, ChannelType (internal));
        }
        
        return UnitType::applyMulAdd (result, mul, add);
    }
    
};


typedef FilterForm<PLONK_TYPE_DEFAULT,FilterFormType::P2Z2> FilterFormP2Z2;
typedef FilterUnit<FilterFormP2Z2> FilterP2Z2;




#endif // PLONK_FILTER_H

