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

#ifndef PLONK_FFTCHANNEL_H
#define PLONK_FFTCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

///** FFT channel. */
//template<class SampleType>
//class FFTChannelInternal 
//:   public ProxyOwnerChannelInternal<SampleType, ChannelInternalCore::Data>
//{
//public:
//    typedef ChannelInternalCore::Data                           Data;
//    typedef ChannelBase<SampleType>                             ChannelType;
//    typedef ObjectArray<ChannelType>                            ChannelArrayType;
//    typedef FFTChannelInternal<SampleType>                      FFTInternal;
//    typedef ProxyOwnerChannelInternal<SampleType,Data>          Internal;
//    typedef UnitBase<SampleType>                                UnitType;
//    typedef InputDictionary                                     Inputs;
//    typedef NumericalArray<SampleType>                          Buffer;
//    typedef FFTEngineBase<SampleType>                           FFTEngineType;
//    
//    enum OutputIndices { RealOutput, ImagOutput, NumOutputs };
//    
//    FFTChannelInternal (Inputs const& inputs, 
//                        Data const& data, 
//                        BlockSize const& blockSize,
//                        SampleRate const& sampleRate,
//                        ChannelArrayType& channels) throw()
//    :   Internal (NumOutputs, inputs, data, blockSize, sampleRate, channels)
//    {
//    }
//    
//    Text getName() const throw()
//    {        
//        return "FFT";
//    }        
//    
//    IntArray getInputKeys() const throw()
//    {
//        const IntArray keys (IOKey::Generic);
//        return keys;
//    }    
//            
//    void initChannel (const int channel) throw()
//    {
//        if ((channel % this->getNumChannels()) == 0)
//        {
//            const UnitType& input = this->getInputAsUnit (IOKey::Generic);
//
//            this->setBlockSize (input.getBlockSize (0));
//            this->setSampleRate (input.getSampleRate (0));    
//            this->setOverlap (input.getOverlap (0));   
//            
//            this->fft = FFTEngineType (input.getBlockSize (0));
//        }
//        
//        this->initProxyValue (channel, SampleType (0)); // not really applicable with an FFT output
//    }    
//    
//    void process (ProcessInfo& info, const int channel) throw()
//    {                
//        /* Be careful optimising this with the new NumericalArray vector stuff */
//
//        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
//        const Buffer& inputBuffer (inputUnit.process (info, channel));
//        const SampleType* const inputSamples = inputBuffer.getArray();
//        
//        SampleType* const realOutputSamples = this->getOutputSamples (RealOutput);
//        SampleType* const imagOutputSamples = this->getOutputSamples (ImagOutput);
//        const int outputBufferLength = this->getOutputBuffer (0).length();
//        
//        plonk_assert (outputBufferLength == inputBuffer.length());
//        
//        if (outputBufferLength != this->fft.length())
//            this->fft = FFTEngineType (outputBufferLength);
//        
//        // transform
//        this->fft.forward (realOutputSamples, inputSamples);
//                
//        imagOutputSamples[0] = SampleType (0);
//        
//        int i, j;
//        
//        // unpack, assuming samples that should be zero are already zero.. (as they should be).
//        for (i = this->fft.halfLength() + 1, j = 1; i < outputBufferLength; ++i, ++j)
//        {
//            imagOutputSamples[j] = realOutputSamples[i];
//            realOutputSamples[i] = SampleType (0);
//        }
//    }
//    
//private:
//    FFTEngineType fft;
//};
//
//
//
////------------------------------------------------------------------------------
//
///** FFT Unit. 
// Takes a time domain real signal as its input and outputs frequency domain data in real/imaginary format. 
// 
// Each input channel generates two output channels. The even number channel is the real data and 
// the odd numbered channel contains the imaginary data. (The IFFTUnit expects the data in this format.)
// 
// @par Factory functions:
// - ar (input)
// 
// @par Inputs:
// - input: (unit, multi) the input unit
// 
//
// @ingroup ConverterUnits FFTUnits */
//template<class SampleType>
//class FFTUnit
//{
//public:    
//    typedef FFTChannelInternal<SampleType>          FFTInternal;
//    typedef typename FFTInternal::Data              Data;
//    typedef UnitBase<SampleType>                    UnitType;
//    typedef InputDictionary                         Inputs;
//    
//    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
//    {
//        return UnitInfo ("FFT", "Transforms time domain to frequency domain data.",
//                         
//                         // output
//                         2, 
//                         IOKey::Real,           Measure::Real,          IOInfo::NoDefault,  IOLimit::None, 
//                         IOKey::Imaginary,      Measure::Imaginary,     IOInfo::NoDefault,  IOLimit::None, 
//                         IOKey::End,
//                         
//                         // inputs
//                         IOKey::Generic,         Measure::None,
//                         IOKey::End);
//    }    
//    
//    /** FFTs a signal. */
//    static PLONK_INLINE_LOW UnitType ar (UnitType const& input) throw()
//    {                     
//        // re: full templating - could say that FFT/IFTT are only supported with float (and eventually double)?
//
//        const int numInputChannels = input.getNumChannels();
//        UnitType result (UnitType::emptyWithAllocatedSize (numInputChannels * 2));
//        Data data = { -1.0, -1.0 };
//
//        for (int i = 0; i < numInputChannels; ++i)
//        {
//            Inputs inputs;
//            inputs.put (IOKey::Generic, input[i]);
//            
//            result.add (UnitType::template proxiesFromInputs<FFTInternal> (inputs, 
//                                                                           data, 
//                                                                           BlockSize::noPreference(), 
//                                                                           SampleRate::noPreference()));
//        }
//        
//        return result;
//    }
//};

/** FFT channel. */
template<class SampleType>
class FFTChannelInternal
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef ObjectArray<ChannelType>                            ChannelArrayType;
    typedef FFTChannelInternal<SampleType>                      FFTInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef FFTEngineBase<SampleType>                           FFTEngineType;
        
    FFTChannelInternal (Inputs const& inputs,
                        Data const& data,
                        BlockSize const& blockSize,
                        SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "FFT";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new FFTInternal (channelInputs,
                                this->getState(),
                                this->getBlockSize(),
                                this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        
        this->setBlockSize (input.getBlockSize (channel));
        this->setSampleRate (input.getSampleRate (channel));
        this->setOverlap (input.getOverlap (channel));
        this->fft = FFTEngineType (input.getBlockSize (channel));
        
        this->initValue (SampleType (0)); // not really applicable with an FFT output
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        /* Be careful optimising this with the new NumericalArray vector stuff */
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        plonk_assert (outputBufferLength == inputBuffer.length());
        
        if (outputBufferLength != this->fft.length())
            this->fft = FFTEngineType (outputBufferLength);
            
        // transform
        this->fft.forward (outputSamples, inputSamples);
    }
    
private:
    FFTEngineType fft;
};



//------------------------------------------------------------------------------

/** FFT Unit.
 Takes a time domain real signal as its input and outputs frequency domain data in real/imaginary format.
 
 Each input channel generates two output channels. The even number channel is the real data and
 the odd numbered channel contains the imaginary data. (The IFFTUnit expects the data in this format.)
 
 @par Factory functions:
 - ar (input)
 
 @par Inputs:
 - input: (unit, multi) the input unit
 
 
 @ingroup ConverterUnits FFTUnits */
template<class SampleType>
class FFTUnit
{
public:
    typedef FFTChannelInternal<SampleType>          FFTInternal;
    typedef typename FFTInternal::Data              Data;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        return UnitInfo ("FFT", "Transforms time domain to frequency domain data.",
                         
                         // output
                         ChannelCount::VariableChannelCount,
                         IOKey::FFTPacked,          Measure::FFTPacked,          IOInfo::NoDefault,  IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::None,
                         IOKey::End);
    }
    
    /** FFTs a signal. */
    static PLONK_INLINE_LOW UnitType ar (UnitType const& input) throw()
    {
        // re: full templating - could say that FFT/IFFT are only supported with float (and eventually double)?
                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<FFTInternal> (inputs,
                                                                 data,
                                                                 BlockSize::noPreference(),
                                                                 SampleRate::noPreference());
    }
};


typedef FFTUnit<PLONK_TYPE_DEFAULT> FFT;

#endif // PLONK_FFTCHANNEL_H


