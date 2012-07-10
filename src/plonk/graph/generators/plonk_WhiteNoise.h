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

#ifndef PLONK_WHITENOISE_H
#define PLONK_WHITENOISE_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class WhiteNoiseChannelInternal;

PLONK_CHANNELDATA_DECLARE(WhiteNoiseChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    
    SampleType minValue;
    SampleType maxValue;
};              

PLONK_CHANNELDATA_SPECIAL(WhiteNoiseChannelInternal,short)
{    
    ChannelInternalCore::Data base;
    
    int minValue;
    int maxValue;
};      

//------------------------------------------------------------------------------

/** White noise generator. */
template<class SampleType>
class WhiteNoiseChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(WhiteNoiseChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(WhiteNoiseChannelInternal,SampleType)    Data;
    typedef InputDictionary                                                 Inputs;
    typedef ChannelBase<SampleType>                                         ChannelType;
    typedef WhiteNoiseChannelInternal<SampleType>                           WhiteNoiseInternal;
    typedef ChannelInternal<SampleType,Data>                                Internal;
    typedef ChannelInternalBase<SampleType>                                 InternalBase;
    typedef UnitBase<SampleType>                                            UnitType;
    
    WhiteNoiseChannelInternal (Inputs const& inputs, 
                               Data const& data, 
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
            
    Text getName() const throw()
    {
        return "White Noise";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys;
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        return this;
    }
    
    void initChannel (const int channel) throw()
    {                        
        this->initValue (SampleType (0));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {        
        Data& data = this->getState();        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();

        RNG& rng (RNG::audio());
        
        for (int i = 0; i < outputBufferLength; ++i) 
            outputSamples[i] = SampleType (rng.uniform (data.minValue, data.maxValue));
    }
    
private:
};
//------------------------------------------------------------------------------

#ifdef PLONK_USEPLINK

template<>
class WhiteNoiseChannelInternal<float> : public ChannelInternal<float, WhiteNoiseProcessStateF>
{
public:
    typedef WhiteNoiseProcessStateF             Data;
    typedef InputDictionary                     Inputs;
    typedef ChannelBase<float>                  ChannelType;
    typedef WhiteNoiseChannelInternal<float>    WhiteNoiseInternal;
    typedef ChannelInternal<float,Data>         Internal;
    typedef ChannelInternalBase<float>          InternalBase;
    typedef UnitBase<float>                     UnitType;
        
    
    enum Outputs { Output, NumOutputs };
    enum InputIndices  { NumInputs };
    enum Buffers { OutputBuffer, NumBuffers };
    
    typedef PlinkProcess<NumBuffers> Process;
    
    WhiteNoiseChannelInternal (Inputs const& inputs, 
                               Data const& data, 
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
        plonk_assert (NumBuffers == (NumInputs + NumOutputs));
        
        Process::init (&p, this, NumOutputs, NumInputs);
    }
    
    Text getName() const throw()
    {
        return "White Noise";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys;
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        return this;
    }    
    
    void initChannel (const int channel) throw()
    {        
        this->initValue (0.f);
    }        
    
    void process (ProcessInfo& info, const int channel) throw()
    {                        
        p.buffers[0].bufferSize = this->getOutputBuffer().length();;
        p.buffers[0].buffer     = this->getOutputSamples();
        
        plink_WhiteNoiseProcessF (&p, &this->getState());
    }
    
private:
    Process p;
};

#endif

//------------------------------------------------------------------------------

/** A white noise generator. 
 @ingroup GeneratorUnits ControlUnits */
template<class SampleType>
class WhiteNoiseUnit
{
public:    
    typedef WhiteNoiseChannelInternal<SampleType>   WhiteNoiseInternal;
    typedef typename WhiteNoiseInternal::Data       Data;
    typedef InputDictionary                         Inputs;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         ChannelInternalType;
    typedef UnitBase<SampleType>                    UnitType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        const double peak = (double)TypeUtility<SampleType>::getTypePeak(); // will be innaccurate for LongLong
        
        return UnitInfo ("WhiteNoise", "A white noise generator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,      0.0,        IOLimit::Clipped,   Measure::NormalisedBipolar, -peak, peak,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate white noise generator. */
    static UnitType ar (UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {                                
        const LongLong peak = SampleType (TypeUtility<SampleType>::getTypePeak());
        Data data = { { -1.0, -1.0 }, -peak, peak };
        
        const int numChannels = plonk::max (mul.getNumChannels(), add.getNumChannels());
        UnitType result (UnitType::withSize (numChannels));
        
        Inputs inputs;
        
        for (int i = 0; i < numChannels; ++i) 
        {
            ChannelInternalType* internal = new WhiteNoiseInternal (inputs, 
                                                                    data, 
                                                                    preferredBlockSize, 
                                                                    preferredSampleRate);
            internal->initChannel (i);
            result.put (i, ChannelType (internal));
        }
        
        return UnitType::applyMulAdd (result, mul, add);
    }
    
    /** Create a control rate white noise generator. */
    static UnitType kr (UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        return ar (mul, add, 
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }        
};

typedef WhiteNoiseUnit<PLONK_TYPE_DEFAULT> WhiteNoise;


#endif // PLONK_WHITENOISE_H

