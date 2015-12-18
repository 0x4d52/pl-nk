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

#define PLONK_DEBUG_CONVOLVE 0

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
        divisionsCurrent (0),
        divisionsWritten (0),
        divisionsCounter (0),
        divisionsPrevious (0),
        divisionsRead (0),
        countDown (0),
        position0 (0),
        position1 (0),
        fftAltSelect (0),
        currentIRBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue())
    {
        const FFTBuffers& irBuffers (currentIRBuffers);
        const FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        const int fftSize = fftEngine.length();

        processBuffers = Buffer::withSize (fftSize * 6);
        SampleType* const processBuffersBase = processBuffers.getArray();
        
        fftAltBuffer0      = processBuffersBase + fftSize * 0;
        fftAltBuffer1      = processBuffersBase + fftSize * 1;
        fftTransformBuffer = processBuffersBase + fftSize * 2;
        fftOverlapBuffer   = processBuffersBase + fftSize * 3;
        fftTempBuffer      = processBuffersBase + fftSize * 4;
        fftCalcBuffer      = processBuffersBase + fftSize * 5;
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
        countDown           = 0;//irBuffers.getCountDownStart (channel);
        position0           = fftEngine.halfLength() - countDown;
        position1           = position0 + countDown;
        fftAltSelect        = 0;
        divisionsCurrent    = 0;
        divisionsCounter    = 0;
        divisionsWritten    = 0;
        divisionsPrevious   = 0;
        divisionsRead       = 1;
        
        processBuffers.zero();
        
//        const UnsignedLong fftSize = (UnsignedLong) fftEngine.length();
        
//        zeroSamples (fftAltBuffer0,      fftSize);
//        zeroSamples (fftAltBuffer1,      fftSize);
//        zeroSamples (fftTransformBuffer, fftSize);
//        zeroSamples (fftOverlapBuffer,   fftSize);
//        zeroSamples (fftTempBuffer,      fftSize);
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
    
    static inline void fadeSamples (SampleType* const dst, const UnsignedLong numItems) throw()
    {
        typedef typename TypeUtility<SampleType>::ScaleType ScaleType;
        ScaleType level (TypeUtility<SampleType>::getTypePeak());
        const ScaleType ramp (level / numItems);
        
        for (UnsignedLong i = 0; i < numItems; ++i)
        {
            dst[i] *= level;
            level  -= ramp;
        }
    }

    
    typedef void (*OutputFunction)(SampleType* const, const SampleType* const, const UnsignedLong);
    
    void processContinue (ProcessInfo& info, const int channel, FFTBuffersType& irBuffers, FFTEngineType& fftEngine, OutputFunction outputFunction) throw()
    {
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        
        SampleType* outputSamples             = this->getOutputSamples();
        const UnsignedLong outputBufferLength = (UnsignedLong) this->getOutputBuffer().length();
        
        plonk_assert (outputBufferLength == inputBuffer.length());
        
        const int numDivisions = irBuffers.getNumDivisions();
        
        if (numDivisions == 0)
        {
            plonk_assertfalse;
            zeroSamples (outputSamples, outputBufferLength);
            return;
        }
        
        const UnsignedLong fftSize           = (UnsignedLong) fftEngine.length();
        const UnsignedLong fftSizeHalved     = (UnsignedLong) fftEngine.halfLength();
        const int divisionRatio1             = (fftSizeHalved / outputBufferLength) - 1;
        SampleType* const fftAltBuffers[]    = { fftAltBuffer0, fftAltBuffer1 };
        SampleType* const processBufferBase  = irBuffers.getProcessBuffer (channel, 0);
        const SampleType* const irBufferBase = irBuffers.getDivision (channel, 0);

        int samplesRemaining                 = outputBufferLength;

        while (samplesRemaining > 0)
        {
            int hop;
            
            if ((samplesRemaining - countDown) > 0)
            {
                hop = countDown;
                samplesRemaining -= countDown;
                countDown = 0;
            }
            else
            {
                hop = samplesRemaining;
                countDown -= samplesRemaining;
                samplesRemaining = 0;
            }
    
            if (hop > 0)
            {
                moveSamples (fftAltBuffer0 + position0, inputSamples, hop);
                moveSamples (fftAltBuffer1 + position1, inputSamples, hop);
                outputFunction (outputSamples, fftOverlapBuffer + position0, hop);
                
                inputSamples  += hop;
                outputSamples += hop;
                position0     += hop;
                position1     += hop;
            }
            
            hop = (divisionsWritten >= divisionsRead - 1) ? 0 : 1;
            ++divisionsCounter;
            
            while (hop != 0)
            {
                int divisionsRemaining = divisionsCounter >= divisionRatio1
                                       ? (divisionsRead - divisionsWritten) - 1
                                       : (int) ((SampleType) ((divisionsCounter * (divisionsRead - 1)) / (SampleType) (divisionRatio1)) - divisionsWritten);
                
                const int nextDivision = divisionsPrevious >= numDivisions ? 0 : divisionsPrevious;
                divisionsPrevious = nextDivision + divisionsRemaining;
                
                if (divisionsPrevious > numDivisions)
                {
                    divisionsPrevious = numDivisions;
                    divisionsRemaining = divisionsPrevious - nextDivision;
                }
                else
                {
                    hop = 0;
                }
                
                if (divisionsRemaining > 0)
                {
                    const SampleType* irSamples            = irBufferBase + (divisionsWritten + 1) * fftSize;
                    const SampleType* processBufferSamples = processBufferBase + nextDivision * fftSize;
                    
                    for (int i = 0; i < divisionsRemaining; i++)
                    {
                        complexMultiplyAccumulate (fftTempBuffer, processBufferSamples, irSamples, fftSizeHalved);
                        
                        processBufferSamples += fftSize;
                        irSamples          += fftSize;
                    }
                    
                    divisionsWritten += divisionsRemaining;
                }
            }
            
            if (countDown == 0)
            {
                divisionsRead = plonk::min (divisionsRead + 1, numDivisions);
                
                const SampleType* const irSamples      = irBufferBase;
                SampleType* const processBufferSamples = processBufferBase + divisionsCurrent * fftSize;
                
                fftEngine.forward (processBufferSamples, fftAltBuffers[fftAltSelect]);
                complexMultiplyAccumulate (fftTempBuffer, processBufferSamples, irSamples, fftSizeHalved);
                
                fftEngine.inverse (fftTransformBuffer, fftTempBuffer);
                
                hop = fftSizeHalved;
                SampleType* const overlap1 = fftOverlapBuffer + (hop * (1 - fftAltSelect));
                SampleType* const overlap2 = fftOverlapBuffer + (hop * fftAltSelect);
                
                moveSamples (overlap1, fftTransformBuffer, fftSizeHalved);
                accumulateSamples (overlap2, fftTransformBuffer + hop, fftSizeHalved);
                
                if (fftAltSelect != 0)
                {
                    position0    = 0;
                    fftAltSelect = 0;
                }
                else
                {
                    position1    = 0;
                    fftAltSelect = 1;
                }
                
                divisionsPrevious   = divisionsCurrent;
                divisionsCurrent    = plonk::wrap (divisionsCurrent - 1, 0, numDivisions);
                divisionsCounter    = 0;
                divisionsWritten    = 0;
                countDown           = hop;
                
                zeroSamples (fftTempBuffer, fftSize);
            }
        }

    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        FFTBuffersAccessType newIRBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());
        FFTEngineType& newFFTEngine (newIRBuffers.getFFTEngine());
        
        if (currentIRBuffers == newIRBuffers)
        {
            processContinue (info, channel, newIRBuffers, newFFTEngine, moveSamples);
        }
        else if (newFFTEngine.length() != currentIRBuffers.getFFTEngine().length())
        {
            plonk_assertfalse;
            processContinue (info, channel, newIRBuffers, newFFTEngine, moveSamples);
        }
        else
        {
            processContinue (info, channel, currentIRBuffers, newFFTEngine, moveSamples);
            fadeSamples (this->getOutputSamples(), (UnsignedLong) this->getOutputBuffer().length());
            
            reset (newIRBuffers, newFFTEngine, channel);
            processContinue (info, channel, newIRBuffers, newFFTEngine, accumulateSamples);
            
            currentIRBuffers = newIRBuffers;
        }
    }

private:
    Buffer processBuffers;
    
    SampleType* fftAltBuffer0;
    SampleType* fftAltBuffer1;
    SampleType* fftTransformBuffer;
    SampleType* fftOverlapBuffer;
    SampleType* fftTempBuffer;
    SampleType* fftCalcBuffer;
    
    int divisionsCurrent;
    int divisionsWritten;
    int divisionsCounter;
    int divisionsPrevious;
    int divisionsRead;
    int countDown;
    int position0, position1;
    int fftAltSelect;
    
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


