/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class SignalPlayChannelInternal;

PLONK_CHANNELDATA_DECLARE(SignalPlayChannelInternal,SampleType)
{    
    typedef typename TypeUtility<SampleType>::IndexType RateType;

    ChannelInternalCore::Data base;
    RateType currentPosition;
    
    bool done:1;
    bool deleteWhenDone:1;
};

//------------------------------------------------------------------------------

/** Signal player generator. */
template<class SampleType>
class SignalPlayChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(SignalPlayChannelInternal,SampleType)>
{
public:

    typedef PLONK_CHANNELDATA_NAME(SignalPlayChannelInternal,SampleType)  Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef SignalPlayChannelInternal<SampleType>                   SignalPlayInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef SignalBase<SampleType>                                  SignalType;

    typedef typename TypeUtility<SampleType>::IndexType             RateType;
    typedef UnitBase<RateType>                                      RateUnitType;
    typedef NumericalArray<RateType>                                RateBufferType;
    typedef InterpLinear<SampleType,RateType>                       InterpType;

    SignalPlayChannelInternal (Inputs const& inputs, 
                               Data const& data, 
                               BlockSize const& blockSize,
                               SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
            
    Text getName() const throw()
    {
        return "Signal Play";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Signal, 
                             IOKey::Rate);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new SignalPlayInternal (channelInputs, 
                                       this->getState(), 
                                       this->getBlockSize(), 
                                       this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {        
        const RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
        
        this->setBlockSize (BlockSize::decide (rateUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (rateUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (rateUnit.getOverlap (channel));
        
        this->initValue (0);//this->getState().currentPosition);
    }
    
    inline bool checkPosition (ProcessInfo& info, Data& data, const int numSignalFrames, const bool loop) throw()
    {        
        if (data.currentPosition >= numSignalFrames)
        {
            if (loop)
            {
                data.currentPosition -= numSignalFrames;
                this->update (Text::getMessageLooped(), Dynamic::getNull());
            }
            else
            {
                data.done = true;
                this->update (Text::getMessageDone(), Dynamic::getNull());
            }
        }
        else if (data.currentPosition < RateType (0))
        {
            if (loop)
            {
                data.currentPosition += numSignalFrames;
                this->update (Text::getMessageLooped(), Dynamic::getNull());
            }
            else
            {
                data.done = true;
                this->update (Text::getMessageDone(), Dynamic::getNull());
            }
        }
        
        return data.done;
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {        
        Data& data = this->getState();
        
        RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
        const RateBufferType& rateBuffer (rateUnit.process (info, channel));
        
        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const RateType* rateSamples = rateBuffer.getArray();
        const int rateBufferLength = rateBuffer.length();
        
        UnitType& loop (this->getInputAsUnit (IOKey::Loop));
        const Buffer& loopBuffer (loop.process (info, channel));
        const SampleType* const loopSamples = loopBuffer.getArray();
        const bool loopFlag = loopSamples[0] >= SampleType (0.5);
        
        const SignalType& signal (this->getInputAsSignal (IOKey::Signal));
        const SampleType* const signalSamples = signal.getSamples (channel);         
        const unsigned int signalFrameStride = signal.getFrameStride();
        const unsigned int numSignalFrames (signal.getNumFrames());
        const RateType rateScale (signal.getSampleRate().getValue() * data.base.sampleDuration);
        
        int numSamplesRemaining = outputBufferLength;
        
        if (rateBufferLength == outputBufferLength)
        {
            while (numSamplesRemaining--)
            {
                const unsigned int sampleA (data.currentPosition);
                const unsigned int sampleB (sampleA + 1);
                const RateType frac (plonk::frac (data.currentPosition));
                
                *outputSamples++ = InterpType::interp (signalSamples[(sampleA % numSignalFrames) * signalFrameStride],
                                                       signalSamples[(sampleB % numSignalFrames) * signalFrameStride], 
                                                       frac);
                
                data.currentPosition += *rateSamples++ * rateScale;
                
                if (checkPosition (info, data, numSignalFrames, loopFlag))
                    break;
            }
        }
        else if (rateBufferLength == 1)
        {
            const RateType increment = rateSamples[0] * rateScale;
            
            while (numSamplesRemaining--)
            {
                const unsigned int sampleA (data.currentPosition);
                const unsigned int sampleB (sampleA + 1);
                const RateType frac (plonk::frac (data.currentPosition));
                
                *outputSamples++ = InterpType::interp (signalSamples[(sampleA % numSignalFrames) * signalFrameStride],
                                                       signalSamples[(sampleB % numSignalFrames) * signalFrameStride], 
                                                       frac);
                data.currentPosition += increment;
                
                if (checkPosition (info, data, numSignalFrames, loopFlag))
                    break;
            }
        }
        else
        {
            double ratePosition = 0.0;
            const double rateIncrement = double (rateBufferLength) / double (outputBufferLength);
            
            while (numSamplesRemaining--)
            {
                const unsigned int sampleA (data.currentPosition);
                const unsigned int sampleB (sampleA + 1);
                const RateType frac (plonk::frac (data.currentPosition));
                
                *outputSamples++ = InterpType::interp (signalSamples[(sampleA % numSignalFrames) * signalFrameStride],
                                                       signalSamples[(sampleB % numSignalFrames) * signalFrameStride], 
                                                       frac);
                
                data.currentPosition += rateSamples[int (ratePosition)] * rateScale;
                
                if (checkPosition (info, data, numSignalFrames, loopFlag))
                    break;
                
                ratePosition += rateIncrement;
            }                    
        }
        
        if (numSamplesRemaining > 0)
        {
            while (numSamplesRemaining--)
                *outputSamples++ = SampleType (0);
        }
        
        if (data.done && data.deleteWhenDone)
            info.setShouldDelete();
    }
    
private:
};

//------------------------------------------------------------------------------

/** Signal player generator. 
 
 @par Factory functions:
 - ar (signal, rate=1, loop=1, mul=1, add=0, allowAutoDelete=true, preferredBlockSize=default, preferredSampleRate=default)
 - kr (signal, rate=1, loop=1, mul=1, add=0, allowAutoDelete=true) 
 
 @par Inputs:
 - signal: (signal, multi) the signal to play
 - rate: (unit, multi) the rate of playback (1= normal speed)
 - loop: (unit, multi) a flag to tell the file player to loop
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset aded to the output
 - allowAutoDelete: (bool) whether this unit can be caused to be deleted by the unit it contains
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

  @ingroup GeneratorUnits */
template<class SampleType>
class SignalPlayUnit
{
public:    
    typedef SignalPlayChannelInternal<SampleType>       SignalPlayInternal;
    typedef typename SignalPlayInternal::Data           Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;
    typedef SignalBase<SampleType>                      SignalType;
    
    typedef typename SignalPlayInternal::RateType         RateType;
    typedef typename SignalPlayInternal::RateUnitType     RateUnitType;
    typedef typename SignalPlayInternal::RateBufferType   RateBufferType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("SignalPlay", "A signal player generator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,        Measure::None,      0.0,            IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Signal,         Measure::None,
                         IOKey::Rate,           Measure::Factor,    1.0,            IOLimit::None,
                         IOKey::Loop,           Measure::Bool,      1.0,            IOLimit::Clipped,   Measure::NormalisedUnipolar,    0.0, 1.0,
                         IOKey::Multiply,       Measure::Factor,    1.0,            IOLimit::None,
                         IOKey::Add,            Measure::None,      0.0,            IOLimit::None,
                         IOKey::AutoDeleteFlag, Measure::Bool,      IOInfo::True,   IOLimit::None,
                         IOKey::BlockSize,      Measure::Samples,   blockSize,      IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,     Measure::Hertz,     sampleRate,     IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate signal player. */
    static UnitType ar (SignalType const& signal, 
                        RateUnitType const& rate = RateType (1), 
                        UnitType const& loop = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const bool deleteWhenDone = true,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::Signal, signal);
        inputs.put (IOKey::Rate, rate);
        inputs.put (IOKey::Loop, loop);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
                        
        Data data = { { -1.0, -1.0 }, RateType (0), false, deleteWhenDone };
        
        return UnitType::template createFromInputs<SignalPlayInternal> (inputs, 
                                                                        data, 
                                                                        preferredBlockSize, 
                                                                        preferredSampleRate);
    }
    
    /** Create a control rate signal player.. */
    static UnitType kr (SignalType const& signal,
                        RateUnitType const& rate, 
                        UnitType const& loop = SampleType (1),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const bool deleteWhenDone = true) throw()
    {
        return ar (signal, rate, loop, mul, add, deleteWhenDone,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }        
};

typedef SignalPlayUnit<PLONK_TYPE_DEFAULT> SignalPlay;


#endif // PLONK_SIGNALPLAY_H

