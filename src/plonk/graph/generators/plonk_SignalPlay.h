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

#ifndef PLONK_SIGNALPLAY_H
#define PLONK_SIGNALPLAY_H

//#include "../channel/plonk_ChannelInternalCore.h"
//#include "../plonk_GraphForwardDeclarations.h"
//
//template<class SampleType> class SignalPlayChannelInternal;
//
//CHANNELDATA_DECLARE(SignalPlayChannelInternal,SampleType)
//{    
//    typedef typename TypeUtility<SampleType>::IndexType RateType;
//
//    ChannelInternalCore::Data base;
//    RateType currentPosition;
//};      
//
////------------------------------------------------------------------------------
//
///** Wavetable oscillator. */
//template<class SampleType>
//class SignalPlayChannelInternal 
//:   public ChannelInternal<SampleType, CHANNELDATA_NAME(SignalPlayChannelInternal,SampleType)>
//{
//public:
//
//    typedef CHANNELDATA_NAME(SignalPlayChannelInternal,SampleType)  Data;
//    typedef ChannelBase<SampleType>                                 ChannelType;
//    typedef SignalPlayChannelInternal<SampleType>                   SignalPlayInternal;
//    typedef ChannelInternal<SampleType,Data>                        Internal;
//    typedef ChannelInternalBase<SampleType>                         InternalBase;
//    typedef UnitBase<SampleType>                                    UnitType;
//    typedef InputDictionary                                         Inputs;
//    typedef NumericalArray<SampleType>                              Buffer;
//    typedef SignalBase<SampleType>                                  WavetableType;
//    
//    typedef typename TypeUtility<SampleType>::IndexType             RateType;
//    typedef UnitBase<RateType>                                      RateUnitType;
//    typedef NumericalArray<RateType>                                RateBufferType;
//
//
//    SignalPlayChannelInternal (Inputs const& inputs, 
//                               Data const& data, 
//                               BlockSize const& blockSize,
//                               SampleRate const& sampleRate) throw()
//    :   Internal (inputs, data, blockSize, sampleRate)
//    {
//    }
//            
//    Text getName() const throw()
//    {
//        return "SignalPlay";
//    }       
//    
//    IntArray getInputKeys() const throw()
//    {
//        const IntArray keys (IOKey::Generic, 
//                             IOKey::Frequency);
//        return keys;
//    }    
//    
//    InternalBase* getChannel (const int index) throw()
//    {
//        const Inputs channelInputs = this->getInputs().getChannel (index);
//        return new SignalPlayInternal (channelInputs, 
//                                  this->getState(), 
//                                  this->getBlockSize(), 
//                                  this->getSampleRate());
//    }
//    
//    void initChannel (const int channel) throw()
//    {        
//        const RateUnitType& rateUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
//        
//        this->setBlockSize (BlockSize::decide (frequencyUnit.getBlockSize (channel),
//                                               this->getBlockSize()));
//        this->setSampleRate (SampleRate::decide (frequencyUnit.getSampleRate (channel),
//                                                 this->getSampleRate()));
//        
//        this->setOverlap (frequencyUnit.getOverlap (channel));
//        
//        this->initValue (this->getState().currentPosition);
//    }    
//    
//    void process (ProcessInfo& info, const int channel) throw()
//    {        
//        Data& data = this->getState();
//        const double sampleRate = data.base.sampleRate;
//        
//        FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
//        const FrequencyBufferType frequencyBuffer (frequencyUnit.process (info, channel));
//        
//        SampleType* const outputSamples = this->getOutputSamples();
//        const int outputBufferLength = this->getOutputBuffer().length();
//        
//        const FrequencyType* const frequencySamples = frequencyBuffer.getArray();
//        const int frequencyBufferLength = frequencyBuffer.length();
//        
//        const WavetableType& SignalPlay (this->getInputAsWavetable (IOKey::Wavetable));
//        const SampleType* const SignalPlaySamples = SignalPlay.getArray();
//        const FrequencyType SignalPlayLength = FrequencyType (SignalPlay.length());
//        
//        double SignalPlayLengthOverSampleRate = SignalPlayLength / sampleRate;
//
//        int i;
//        
//        if (frequencyBufferLength == outputBufferLength)
//        {
//            for (i = 0; i < outputBufferLength; ++i) 
//            {
//                outputSamples[i] = plonk::lookup (SignalPlaySamples, data.currentPosition);
//                data.currentPosition += frequencySamples[i] * SignalPlayLengthOverSampleRate;
//                
//                if (data.currentPosition >= SignalPlayLength)
//                    data.currentPosition -= SignalPlayLength;
//                else if (data.currentPosition < SampleType (0))	
//                    data.currentPosition += SignalPlayLength;                
//            }                    
//        }
//        else if (frequencyBufferLength == 1)
//        {
//            const FrequencyType valueIncrement (frequencySamples[0] * SignalPlayLengthOverSampleRate);
//            
//            for (i = 0; i < outputBufferLength; ++i) 
//            {
//                outputSamples[i] = plonk::lookup (SignalPlaySamples, data.currentPosition);
//                data.currentPosition += valueIncrement;
//                
//                if (data.currentPosition >= SignalPlayLength)
//                    data.currentPosition -= SignalPlayLength;
//                else if (data.currentPosition < SampleType (0))	
//                    data.currentPosition += SignalPlayLength;                
//            }                    
//        }
//        else
//        {
//            double frequencyPosition = 0.0;
//            const double frequencyIncrement = double (frequencyBufferLength) / double (outputBufferLength);
//            
//            for (i = 0; i < outputBufferLength; ++i) 
//            {
//                outputSamples[i] = plonk::lookup (SignalPlaySamples, data.currentPosition);
//                data.currentPosition += frequencySamples[int (frequencyPosition)] * SignalPlayLengthOverSampleRate;
//                
//                if (data.currentPosition >= SignalPlayLength)
//                    data.currentPosition -= SignalPlayLength;
//                else if (data.currentPosition < SampleType (0))	
//                    data.currentPosition += SignalPlayLength;                
//                
//                frequencyPosition += frequencyIncrement;
//            }        
//        }
//        
//    }
//    
//private:
//};
//
////------------------------------------------------------------------------------
//
///** Wavetable oscillator. 
//  @ingroup OscillatorUnits ControlUnits */
//template<class SampleType>
//class SignalPlayUnit
//{
//public:    
//    typedef SignalPlayChannelInternal<SampleType>       SignalPlayInternal;
//    typedef typename SignalPlayInternal::Data           Data;
//    typedef ChannelBase<SampleType>                     ChannelType;
//    typedef ChannelInternal<SampleType,Data>            Internal;
//    typedef UnitBase<SampleType>                        UnitType;
//    typedef InputDictionary                             Inputs;
//    typedef WavetableBase<SampleType>              WavetableType;
//    
//    typedef typename SignalPlayInternal::FrequencyType         FrequencyType;
//    typedef typename SignalPlayInternal::FrequencyUnitType     FrequencyUnitType;
//    typedef typename SignalPlayInternal::FrequencyBufferType   FrequencyBufferType;
//    
//    static inline UnitInfos getInfo() throw()
//    {
//        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();
//        
//        return UnitInfo ("SignalPlay", "A Wavetable oscillator.",
//                         
//                         // output
//                         ChannelCount::VariableChannelCount, 
//                         IOKey::Generic,     Measure::None,      0.0,        IOLimit::None,
//                         IOKey::End,
//                         
//                         // inputs
//                         IOKey::Wavetable,  Measure::None,
//                         IOKey::Frequency,  Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
//                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
//                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
//                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
//                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
//                         IOKey::End);
//    }
//    
//    /** Create an audio rate sawtooth oscillator. */
//    static UnitType ar (WavetableType const& SignalPlay, 
//                        FrequencyUnitType const& frequency = FrequencyType (440), 
//                        UnitType const& mul = SampleType (1),
//                        UnitType const& add = SampleType (0),
//                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
//                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
//    {             
//        Inputs inputs;
//        inputs.put (IOKey::Wavetable, SignalPlay);
//        inputs.put (IOKey::Frequency, frequency);
//        inputs.put (IOKey::Multiply, mul);
//        inputs.put (IOKey::Add, add);
//                        
//        Data data = { { -1.0, -1.0 }, 0 };
//        
//        return UnitType::template createFromInputs<SignalPlayInternal> (inputs, 
//                                                                   data, 
//                                                                   preferredBlockSize, 
//                                                                   preferredSampleRate);
//    }
//    
//    /** Create a control rate sawtooth oscillator. */
//    static UnitType kr (WavetableType const& SignalPlay,
//                        FrequencyUnitType const& frequency, 
//                        UnitType const& mul = SampleType (1),
//                        UnitType const& add = SampleType (0)) throw()
//    {
//        return ar (SignalPlay, frequency, mul, add, 
//                   BlockSize::getControlRateBlockSize(), 
//                   SampleRate::getControlRate());
//    }        
//};
//
//typedef SignalPlayUnit<PLONK_TYPE_DEFAULT> SignalPlay;


#endif // PLONK_SIGNALPLAY_H

