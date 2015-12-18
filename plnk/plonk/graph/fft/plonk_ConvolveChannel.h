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


template<class SampleType, typename FFTBuffersAccess>
class ConvolveHelper
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

    ConvolveHelper (const int fftSize) throw()
    :   processBuffers (Buffer::withSize (fftSize * 6)),
        currentProcessBuffersDivision (0),
        writtenIRDivisions (0),
        countIRDivisions (0),
        previousProcessBufferDivision (0),
        readIRDivisions (0),
        countDown (0),
        position0 (0),
        position1 (0),
        fftAltSelect (0)
    {
        SampleType* const processBuffersBase = processBuffers.getArray();
        
        fftAltBuffer0      = processBuffersBase + fftSize * 0;
        fftAltBuffer1      = processBuffersBase + fftSize * 1;
        fftTransformBuffer = processBuffersBase + fftSize * 2;
        fftOverlapBuffer   = processBuffersBase + fftSize * 3;
        fftTempBuffer      = processBuffersBase + fftSize * 4;
        fftCalcBuffer      = processBuffersBase + fftSize * 5;
    }
    
    void reset (const int channel) throw()
    {
        countDown                       = irBuffers.getCountDownStart (channel);
        position0                       = irBuffers.getFFTEngine().halfLength() - countDown;
        position1                       = position0 + countDown;
        fftAltSelect                    = 0;
        currentProcessBuffersDivision   = 0;
        countIRDivisions                = 0;
        writtenIRDivisions              = 0;
        previousProcessBufferDivision   = 0;
        readIRDivisions                 = 1;
        
        const UnsignedLong fftSize = (UnsignedLong) irBuffers.getFFTEngine().length();
        
        zeroSamples (fftAltBuffer0,      fftSize);
        zeroSamples (fftAltBuffer1,      fftSize);
        zeroSamples (fftTransformBuffer, fftSize);
        zeroSamples (fftOverlapBuffer,   fftSize);
        zeroSamples (fftTempBuffer,      fftSize);
    }
    
    void setIRBuffers (FFTBuffersType const& newIRBuffers) throw()
    {
        irBuffers = newIRBuffers;
    }
    
    void process (ConvolveInternal* owner, ProcessInfo& info, const int channel,
                  SampleType* outputSamples, const SampleType* inputSamples, const UnsignedLong outputBufferLength,
                  const bool pullsInput, const bool replacesOutput) throw()
    {
        FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        
        const int numIRDivisions      = irBuffers.getNumIRDivisions (channel);
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
        SampleType* const fftAltBuffers[]    = { fftAltBuffer0, fftAltBuffer1 };
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
                if (pullsInput)
                {
                    moveSamples (fftAltBuffer0 + position0, inputSamples, hop);
                    moveSamples (fftAltBuffer1 + position1, inputSamples, hop);
                }
                
                if (replacesOutput)
                    moveSamples (outputSamples, fftOverlapBuffer + position0, hop);
                else
                    accumulateSamples (outputSamples, fftOverlapBuffer + position0, hop);
                
                inputSamples  += hop;
                outputSamples += hop;
                position0     += hop;
                position1     += hop;
            }
            
            hop = (writtenIRDivisions >= readIRDivisions - 1) ? 0 : 1;
            ++countIRDivisions;
            
            while (hop != 0)
            {
                int divisionsRemaining = countIRDivisions >= divisionRatio1
                                       ? (readIRDivisions - writtenIRDivisions) - 1
                                       : (int) ((SampleType) ((countIRDivisions * (readIRDivisions - 1)) / (SampleType) (divisionRatio1)) - writtenIRDivisions);
                
                const int nextProcessBufferDivision = previousProcessBufferDivision >= numProcessDivisions ? 0 : previousProcessBufferDivision;
                previousProcessBufferDivision = nextProcessBufferDivision + divisionsRemaining;
                
                if (previousProcessBufferDivision > numProcessDivisions)
                {
                    previousProcessBufferDivision = numProcessDivisions;
                    divisionsRemaining = previousProcessBufferDivision - nextProcessBufferDivision;
                }
                else
                {
                    hop = 0;
                }
                
                irBuffers.complexMultiplyAccumulate (fftTempBuffer, nextProcessBufferDivision, writtenIRDivisions + 1, channel, divisionsRemaining);
                
                writtenIRDivisions += divisionsRemaining;
            }
            
            if (countDown == 0)
            {
                readIRDivisions = plonk::min (readIRDivisions + 1, numIRDivisions);
                
                const SampleType* const irSamples      = irBuffers.getIRDivision (channel, 0);;
                SampleType* const processBufferSamples = irBuffers.getProcessDivision (channel, currentProcessBuffersDivision);;
                
                if (pullsInput)
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
                
                previousProcessBufferDivision = currentProcessBuffersDivision;
                currentProcessBuffersDivision = plonk::wrap (currentProcessBuffersDivision - 1, 0, numProcessDivisions);
                countIRDivisions    = 0;
                writtenIRDivisions  = 0;
                countDown           = hop;
                
                zeroSamples (fftTempBuffer, fftSize);
            }
        }
    }
    
    static PLONK_INLINE_MID void complexMultiplyAccumulate (SampleType* const output,
                                                            const SampleType* const left, const SampleType* const right,
                                                            const UnsignedLong halfLength) throw()
    {
        NumericalArrayComplex<SampleType>::zpmulaccum (output, left, right, halfLength);
    }
    
    static PLONK_INLINE_MID void moveSamples (SampleType* const dst, const SampleType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayUnaryOp<SampleType, plonk::move>::calc (dst, src, numItems);
    }
    
    static PLONK_INLINE_MID void accumulateSamples (SampleType* const dst, const SampleType* const src, const UnsignedLong numItems) throw()
    {
        NumericalArrayBinaryOp<SampleType, plonk::addop>::calcNN (dst, dst, src, numItems);
    }
    
    static PLONK_INLINE_MID void zeroSamples (SampleType* const dst, const UnsignedLong numItems) throw()
    {
        NumericalArray<SampleType>::zeroData (dst, numItems);
    }
    
    void copyState (ConvolveHelper* other) throw()
    {
        this->currentProcessBuffersDivision = other->currentProcessBuffersDivision;
//        this->writtenIRDivisions            = other->writtenIRDivisions;
//        this->countIRDivisions              = other->countIRDivisions;
//        this->previousProcessBufferDivision = other->previousProcessBufferDivision;
//        this->readIRDivisions               = other->readIRDivisions;
//        this->countDown                     = other->countDown;
//        this->position0                     = other->position0;
//        this->position1                     = other->position1;
//        this->fftAltSelect                  = other->fftAltSelect;
    }
    
private:
    FFTBuffersType irBuffers;
    Buffer processBuffers;
    
    SampleType* fftAltBuffer0;
    SampleType* fftAltBuffer1;
    SampleType* fftTransformBuffer;
    SampleType* fftOverlapBuffer;
    SampleType* fftTempBuffer;
    SampleType* fftCalcBuffer;
    
    int currentProcessBuffersDivision;
    int writtenIRDivisions;
    int countIRDivisions;
    int previousProcessBufferDivision;
    int readIRDivisions;
    int countDown;
    int position0, position1;
    int fftAltSelect;
};


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
    typedef ConvolveHelper<SampleType, FFTBuffersAccess>            ConvolveHelperType;
    typedef typename FFTBuffersType::DivisionsType                  DivisionsType;
    typedef typename TypeUtility<SampleType>::ScaleType             ScaleType;
    
    ConvolveChannelInternal (Inputs const& inputs,
                             Data const& data,
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate),
        dummyIRBuffers(),
        previousIRBuffers (dummyIRBuffers),
        currentIRBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue())
    {
        swapBuffers.setSize (currentIRBuffers.getNumChannels(), true); // so we can swap buffers about when switching IRs
        currentConvolver  = new ConvolveHelperType (currentIRBuffers.getFFTEngine().length());
        previousConvolver = new ConvolveHelperType (currentIRBuffers.getFFTEngine().length());
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
                
        FFTBuffersAccessType irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());
        currentConvolver->setIRBuffers (irBuffers);
        currentConvolver->reset (channel);
    }
    
    void reset (const int channel) throw()
    {
        currentConvolver->reset (channel);
    }
        
    void process (ProcessInfo& info, const int channel) throw()
    {
        FFTBuffersAccessType irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());
        FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        
        SampleType* outputSamples             = this->getOutputSamples();
        const UnsignedLong outputBufferLength = (UnsignedLong) this->getOutputBuffer().length();
        
        plonk_assert (outputBufferLength == inputBuffer.length());

        if (previousIRBuffers == dummyIRBuffers && irBuffers == currentIRBuffers)
        {
            currentConvolver->process (this, info, channel, outputSamples, inputSamples, outputBufferLength, true, true);
        }
        else if (fftEngine.length() != currentIRBuffers.getFFTEngine().length())
        {
            plonk_assertfalse; // new FFT buffers need to use the same FFT size as the old one
            currentConvolver->process (this, info, channel, outputSamples, inputSamples, outputBufferLength, true, true);
        }
        else
        {
            previousIRBuffers = currentIRBuffers;
            currentIRBuffers = irBuffers;
            
            currentConvolver.swapWith (previousConvolver);
            previousConvolver->process (this, info, channel, outputSamples, inputSamples, outputBufferLength, true, true);
            
            ScaleType level (TypeUtility<SampleType>::getTypePeak());
            const ScaleType slope (level / outputBufferLength);
            
            for (int i = 0; i < outputBufferLength; ++i)
            {
                outputSamples[i] *= level;
                level -= slope;
            }
            
            currentConvolver->setIRBuffers (currentIRBuffers);
            currentConvolver->reset (channel);
//            currentConvolver->copyState (previousConvolver);
            
//            currentIRBuffers.swapProcessBuffers (previousIRBuffers, true);
            currentConvolver->process (this, info, channel, outputSamples, 0, outputBufferLength, false, false);
            
            previousIRBuffers = dummyIRBuffers;
        }
    }

private:
    ScopedPointerContainer<ConvolveHelperType> currentConvolver;
    ScopedPointerContainer<ConvolveHelperType> previousConvolver;
    
    DivisionsType swapBuffers;
    FFTBuffersType dummyIRBuffers;
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


