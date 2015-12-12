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

#ifndef PLONK_CONVOLVECHANNEL_H
#define PLONK_CONVOLVECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

#define PLONK_DEBUG_CONVOLVE 0//1

/** Convolve channel. */
template<class SampleType, typename FFTBuffersAccess>
class ConvolveChannelInternal
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef FFTBuffersAccess                                        FFTBuffersAccessType;

    typedef ChannelInternalCore::Data                               Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ObjectArray<ChannelType>                                ChannelArrayType;
    typedef ConvolveChannelInternal<SampleType,FFTBuffersAccess>    ConvolveInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef FFTEngineBase<SampleType>                               FFTEngineType;
    typedef FFTBuffersBase<SampleType>                              FFTBuffersType;
    typedef Variable<FFTBuffersType&>                               FFTBuffersVariableType;
    
    ConvolveChannelInternal (Inputs const& inputs,
                             Data const& data,
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate),
        dummyIRBuffers(),
        previousIRBuffers (dummyIRBuffers),
        currentIRBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue())
    {
        ///...
    }
    
    Text getName() const throw()
    {
        return "Convolve";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::FFTBuffers);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new ConvolveInternal (channelInputs,
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
        
        this->initValue (SampleType (0));
        
        const FFTBuffersAccessType irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());
        const FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        
        reset (irBuffers, fftEngine, channel);
    }
    
    void reset (FFTBuffersType const& irBuffers, FFTEngineType const& fftEngine, const int channel) throw()
    {
        ///...
    }

    static inline void complexMultiplyAccumulate (SampleType* const output,
                                                  const SampleType* const left, const SampleType* const right,
                                                  const UnsignedLong halfLength) throw()
    {
        NumericalArrayComplex<SampleType>::zpmulaccum (output, left, right, halfLength);
    }

    static inline void moveSamples (SampleType* const dst, const SampleType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayUnaryOp<SampleType, plonk::move>::calc (dst, src, numItems);
    }
    
    static inline void accumulateSamples (SampleType* const dst, const SampleType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayBinaryOp<SampleType, plonk::addop>::calcNN (dst, dst, src, numItems);
    }
    
    static inline void zeroSamples (SampleType* const dst, const UnsignedLong numItems) throw()
    {
        NumericalArray<SampleType>::zeroData (dst, numItems);
    }
    
    void processContinue (ProcessInfo& info, const int channel, FFTBuffersType& irBuffers, FFTEngineType& fftEngine) throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        
        SampleType* outputSamples             = this->getOutputSamples();
        const UnsignedLong outputBufferLength = (UnsignedLong) this->getOutputBuffer().length();
        
        plonk_assert (outputBufferLength == inputBuffer.length());
        
        const int numIRDivisions      = irBuffers.getNumIRDivisions();
        const int numProcessDivisions = irBuffers.getNumProcessDivisions (channel);
        
        if (numIRDivisions == 0)
        {
            plonk_assertfalse;
            zeroSamples (outputSamples, outputBufferLength);
            return;
        }
        
        const UnsignedLong fftSize           = (UnsignedLong) fftEngine.length();
        const UnsignedLong fftSizeHalved     = (UnsignedLong) fftEngine.halfLength();
        const int divisionRatio1             = (fftSizeHalved / outputBufferLength) - 1;
        SampleType* const processBufferBase  = irBuffers.getProcessDivision (channel, 0);
        const SampleType* const irBufferBase = irBuffers.getIRDivision (channel, 0);

        int samplesRemaining                 = outputBufferLength;

        ///...
    }
    
    void processChanged (ProcessInfo& info, const int channel, FFTBuffersType& irBuffers, FFTEngineType& fftEngine) throw()
    {
        processContinue (info, channel, irBuffers, fftEngine);
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        FFTBuffersAccessType irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());
        FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        
        if (previousIRBuffers == dummyIRBuffers && irBuffers == currentIRBuffers)
        {
            processContinue (info, channel, irBuffers, fftEngine);
        }
        else if (fftEngine.length() != currentIRBuffers.getFFTEngine().length())
        {
            plonk_assertfalse; // new FFT buffers need to use the same FFT size as the old one
            processContinue (info, channel, currentIRBuffers, currentIRBuffers.getFFTEngine());
        }
        else
        {
            processContinue (info, channel, irBuffers, fftEngine);
        }
    }
    
private:
    FFTBuffers dummyIRBuffers;
    FFTBuffersAccessType previousIRBuffers;
    FFTBuffersAccessType currentIRBuffers;

};



//------------------------------------------------------------------------------

/** Convolve Unit.
 
 @par Factory functions:
 - ar (input, fftBuffers)
 
 @par Inputs:
 - input: (unit, multi) the unit to convolve
 - fftBuffers: (fft-buffers) the impulse response

 
 @ingroup FFTUnits */
template<class SampleType, typename FFTBuffersAccess = FFTBuffersBase<SampleType>&>
class ConvolveUnit
{
public:
    typedef ConvolveChannelInternal<SampleType,FFTBuffersAccess>    ConvolveInternal;
    typedef typename ConvolveInternal::Data                         Data;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef FFTBuffersBase<SampleType>                              FFTBuffersType;
    typedef Variable<FFTBuffersType&>                               FFTBuffersVariableType;

    typedef ConvolveUnit<SampleType,FFTBuffersType>                 Dyn;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        return UnitInfo ("Convolve", "Convolve with an impulse response.",
                         
                         // output
                         ChannelCount::VariableChannelCount,
                         IOKey::Generic,            Measure::None,      0.0,            IOLimit::None,
                         IOKey::End,

                         // inputs
                         IOKey::Generic,            Measure::None,
                         IOKey::FFTBuffers,         Measure::FFTPacked,
                         IOKey::End);
    }
    
    static PLONK_INLINE_LOW UnitType ar (UnitType const& input, FFTBuffersVariableType const& fftBuffers) throw()
    {
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::FFTBuffers, fftBuffers);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<ConvolveInternal> (inputs,
                                                                      data,
                                                                      BlockSize::noPreference(),
                                                                      SampleRate::noPreference());
    }
};


typedef ConvolveUnit<PLONK_TYPE_DEFAULT> Convolve;

//#if PLONK_INSTANTIATE_TEMPLATES
template class ConvolveUnit<Float>;
//#endif


#endif // PLONK_CONVOLVECHANNEL_H


