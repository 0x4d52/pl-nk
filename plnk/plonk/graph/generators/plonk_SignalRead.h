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

#ifndef PLONK_SignalRead_H
#define PLONK_SignalRead_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class SignalReadChannelInternal;

PLONK_CHANNELDATA_DECLARE(SignalReadChannelInternal,SampleType)
{    
    typedef typename TypeUtility<SampleType>::IndexType PositionType;

    ChannelInternalCore::Data base;
};      

//------------------------------------------------------------------------------

/** Signal reader generator. */
template<class SampleType>
class SignalReadChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(SignalReadChannelInternal,SampleType)>
{
public:

    typedef PLONK_CHANNELDATA_NAME(SignalReadChannelInternal,SampleType)  Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef SignalReadChannelInternal<SampleType>                   SignalReadInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef SignalBase<SampleType>                                  SignalType;

    typedef typename TypeUtility<SampleType>::IndexType             PositionType;
    typedef UnitBase<PositionType>                                  PositionUnitType;
    typedef NumericalArray<PositionType>                            PositionBufferType;
    typedef InterpLinear<SampleType,PositionType>                   InterpType;

    SignalReadChannelInternal (Inputs const& inputs, 
                               Data const& data,
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
            
    Text getName() const throw()
    {
        return "Signal Read";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Signal, 
                             IOKey::Time);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new SignalReadInternal (channelInputs, 
                                       this->getState(), 
                                       this->getBlockSize(), 
                                       this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {        
        const PositionUnitType& positionUnit = ChannelInternalCore::getInputAs<PositionUnitType> (IOKey::Time);
        
        this->setBlockSize (BlockSize::decide (positionUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (positionUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (positionUnit.getOverlap (channel));
        
        this->initValue (0);//
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {                
        PositionUnitType& positionUnit = ChannelInternalCore::getInputAs<PositionUnitType> (IOKey::Time);
        const PositionBufferType& positionBuffer (positionUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const PositionType* const positionSamples = positionBuffer.getArray();
        const int positionBufferLength = positionBuffer.length();
        
        const SignalType& signal (this->getInputAsSignal (IOKey::Signal));
        const SampleType* const signalSamples = signal.getSamples (channel);         
        const unsigned int signalFrameStride = signal.getFrameStride();
        const unsigned int numSignalFrames (signal.getNumFrames());
        
        const PositionType positionScale (signal.getSampleRate().getValue());
        
        int i;
        
        if (positionBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i) 
            {
                const PositionType currentPosition = positionSamples[i] * positionScale;
                
                const unsigned int sampleA (plonk::max (PositionType (0), currentPosition));
                const unsigned int sampleB (sampleA + 1);
                const PositionType frac (plonk::frac (currentPosition));
                
                outputSamples[i] = InterpType::interp (signalSamples[(sampleA % numSignalFrames) * signalFrameStride], 
                                                       signalSamples[(sampleB % numSignalFrames) * signalFrameStride], 
                                                       frac);                
            }
        }
        else if (positionBufferLength == 1)
        {
            const PositionType currentPosition = positionSamples[0] * positionScale;
            const unsigned int sampleA (plonk::max (PositionType (0), currentPosition));
            const unsigned int sampleB (sampleA + 1);
            const PositionType frac (plonk::frac (currentPosition));
            const SampleType value = InterpType::interp (signalSamples[(sampleA % numSignalFrames) * signalFrameStride],
                                                         signalSamples[(sampleB % numSignalFrames) * signalFrameStride],
                                                         frac);
            
            NumericalArrayFiller<SampleType>::fill (outputSamples, value, outputBufferLength);
        }
        else
        {
            double positionPosition = 0.0;
            const double positionIncrement = double (positionBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                const PositionType currentPosition = positionSamples[int (positionPosition)] * positionScale;
                
                const unsigned int sampleA (plonk::max (PositionType (0), currentPosition));
                const unsigned int sampleB (sampleA + 1);
                const PositionType frac (plonk::frac (currentPosition));
                
                outputSamples[i] = InterpType::interp (signalSamples[(sampleA % numSignalFrames) * signalFrameStride], 
                                                       signalSamples[(sampleB % numSignalFrames) * signalFrameStride], 
                                                       frac);
                
                positionPosition += positionIncrement;
            }                    
        }
        
    }
    
private:
};

//------------------------------------------------------------------------------

/** Signal reader generator. 
 
 @par Factory functions:
 - ar (signal, position, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - kr (signal, position, mul=1, add=0) 
 
 @par Inputs:
 - signal: (signal, multi) the signal to play
 - position: (unit, multi) the read position in seconds
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

  @ingroup GeneratorUnits */
template<class SampleType>
class SignalReadUnit
{
public:    
    typedef SignalReadChannelInternal<SampleType>       SignalReadInternal;
    typedef typename SignalReadInternal::Data           Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;
    typedef SignalBase<SampleType>                      SignalType;
    
    typedef typename SignalReadInternal::PositionType         PositionType;
    typedef typename SignalReadInternal::PositionUnitType     PositionUnitType;
    typedef typename SignalReadInternal::PositionBufferType   PositionBufferType;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("SignalRead", "A signal reader generator",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,      0.0,                    IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Signal,     Measure::None,
                         IOKey::Time,       Measure::Seconds,   IOInfo::NoDefault,      IOLimit::Minimum,   Measure::Seconds,           0.0,
                         IOKey::Multiply,   Measure::Factor,    1.0,                    IOLimit::None,
                         IOKey::Add,        Measure::None,      0.0,                    IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,              IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate,             IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate signal player. */
    static UnitType ar (SignalType const& signal, 
                        PositionUnitType const& position,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        Inputs inputs;
        inputs.put (IOKey::Signal, signal);
        inputs.put (IOKey::Time, position);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
                        
        Data data = { { -1.0, -1.0 } };
        
        return UnitType::template createFromInputs<SignalReadInternal> (inputs, 
                                                                        data, 
                                                                        preferredBlockSize, 
                                                                        preferredSampleRate);
    }
    
    /** Create a control rate signal player.. */
    static UnitType kr (SignalType const& signal,
                        PositionUnitType const& position,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        return ar (signal, position, mul, add, 
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }        
};

typedef SignalReadUnit<PLONK_TYPE_DEFAULT> SignalRead;


#endif // PLONK_SignalRead_H

