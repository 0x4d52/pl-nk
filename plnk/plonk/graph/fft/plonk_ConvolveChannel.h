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

//------------------------------------------------------------------------------

template<class SampleType>
static PLONK_INLINE_HIGH void complexMultiplyAccumulate (SampleType* const output,
                                                         const SampleType* const left, const SampleType* const right,
                                                         const UnsignedLong halfLength) throw()
{
    NumericalArrayComplex<SampleType>::zpmulaccum (output, left, right, halfLength);
}

template<class SampleType>
static PLONK_INLINE_HIGH void complexMultiply (SampleType* const output,
                                               const SampleType* const left, const SampleType* const right,
                                               const UnsignedLong halfLength) throw()
{
    NumericalArrayComplex<SampleType>::zpmul (output, left, right, halfLength);
}

template<class SampleType>
static PLONK_INLINE_HIGH void moveSamples (SampleType* const dst, const SampleType* const src, const UnsignedLong numItems) throw()
{
//    NumericalArrayUnaryOp<SampleType, UnaryOpFunctions<SampleType>::move>::calc (dst, src, numItems);
    memcpy (dst, src, numItems * sizeof (SampleType));
}

template<class SampleType>
static PLONK_INLINE_HIGH void moveZeroSamples (SampleType* const dst, const SampleType* const /*src*/, const UnsignedLong numItems) throw()
{
    NumericalArray<SampleType>::zeroData (dst, numItems);
}

template<class SampleType>
static PLONK_INLINE_HIGH void accumulateSamples (SampleType* const dst, const SampleType* const src, const UnsignedLong numItems) throw()
{
    NumericalArrayBinaryOp<SampleType, BinaryOpFunctions<SampleType>::addop>::calcNN (dst, dst, src, numItems);
}

template<class SampleType>
static PLONK_INLINE_HIGH void zeroSamples (SampleType* const dst, const UnsignedLong numItems) throw()
{
    NumericalArray<SampleType>::zeroData (dst, numItems);
}

template<class SampleType>
static PLONK_INLINE_HIGH void fadeSamples (SampleType* const dst, SampleType& level, const SampleType slope, const UnsignedLong numItems) throw()
{
    NumericalArrayBinaryOp<SampleType, BinaryOpFunctions<SampleType>::rampmul>::calc11 (dst, level, -slope, numItems);
    level -= slope * numItems;
}

//------------------------------------------------------------------------------

template<class SampleType>
class ConvolveHelper //: public PlonkBase
{
public:
    typedef UnitBase<SampleType>                                    UnitType;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef FFTEngineBase<SampleType>                               FFTEngineType;
    typedef FFTBuffersBase<SampleType>                              FFTBuffersType;

    typedef void (*OutputFunction)(SampleType* const, const SampleType* const, const UnsignedLong);
    typedef void (*ZMulFunction)(SampleType* const, const SampleType* const, const SampleType* const, const UnsignedLong);
    typedef void (*InputFunction)(SampleType* const, const SampleType* const, const UnsignedLong);

    enum Constants
    {
        NumProcessBuffers = 6
    };
    
    ConvolveHelper (const int maxFFTSizeToAllow) throw()
    :   maxFFTSize (maxFFTSizeToAllow),
        processBuffers (Buffer::withSize (maxFFTSize * NumProcessBuffers))
    {
        SampleType* const processBuffersBase = processBuffers.getArray();
        
        fftAltBuffer0      = processBuffersBase + maxFFTSize * 0;
        fftAltBuffer1      = processBuffersBase + maxFFTSize * 1;
        fftTransformBuffer = processBuffersBase + maxFFTSize * 2;
        fftOverlapBuffer   = processBuffersBase + maxFFTSize * 3;
        fftTempBuffer      = processBuffersBase + maxFFTSize * 4;
        fftCalcBuffer      = processBuffersBase + maxFFTSize * 5;
    }
    
    void reset (FFTBuffersType const& newIRBuffers, const int channel) throw()
    {
        irBuffers = newIRBuffers;
        const FFTEngineType& fftEngine (irBuffers.getFFTEngine());

        countDown           = 0;//irBuffers.getCountDownStart (channel);
        position0           = fftEngine.halfLength() - countDown;
        position1           = position0 + countDown;
        fftAltSelect        = 0;
        divisionsCurrent    = 0;
        divisionsCounter    = 0;
        divisionsWritten    = 0;
        divisionsPrevious   = 0;
        divisionsRead       = 1;
        
        SampleType* processBuffersBase = processBuffers.getArray();
        const UnsignedLong fftSize = fftEngine.length();

        for (int i = 0; i < NumProcessBuffers; ++i)
        {
            zeroSamples (processBuffersBase, fftSize); // only zero what we need
            processBuffersBase += maxFFTSize;          // ensure we offset this by maxFFTSize though
        }
    }
    
    void process (SampleType* outputSamples, const SampleType* inputSamples, const int outputBufferLength, const int channel, OutputFunction outputFunction, InputFunction inputFunction) throw()
    {
        FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        const int numDivisions = irBuffers.getNumDivisions();
        const int numChannels = irBuffers.getNumChannels();
        
        // if we have no IR or teh channel requested is out of range then just zero the output
        if (numDivisions == 0 || channel >= numChannels)
        {
            zeroSamples (outputSamples, outputBufferLength);
            return;
        }
        
        const int fftSize                    = (int) fftEngine.length();
        const int fftSizeHalved              = (int) fftEngine.halfLength();
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
                inputFunction (fftAltBuffer0 + position0, inputSamples, hop);
                inputFunction (fftAltBuffer1 + position1, inputSamples, hop);
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
                        irSamples            += fftSize;
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
    
private:
    const int maxFFTSize;
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
    
    FFTBuffersType irBuffers;
};

//------------------------------------------------------------------------------

struct ConvolveChannelData
{
    ChannelInternalCore::Data base;
    int numChannels;
    int maxFFTSize;
    int fadeSamples;
    bool keepPreviousTail;
};

//------------------------------------------------------------------------------

/** Convolve channel. */
template<class SampleType>
class ConvolveChannelInternal : public ProxyOwnerChannelInternal<SampleType, ConvolveChannelData>
{
public:
    typedef ConvolveChannelData                                     Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ObjectArray<ChannelType>                                ChannelArrayType;
    typedef ConvolveChannelInternal<SampleType>                     ConvolveInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>              Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef FFTEngineBase<SampleType>                               FFTEngineType;
    typedef FFTBuffersBase<SampleType>                              FFTBuffersType;
    typedef Variable<FFTBuffersType&>                               FFTBuffersVariableType;
    typedef ConvolveHelper<SampleType>                              ConvolveHelperType;
    
    ConvolveChannelInternal (Inputs const& inputs,
                             Data const& data,
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate,
                             ChannelArrayType& channels) throw()
    :   Internal (decideNumChannels (inputs, data), inputs, data, blockSize, sampleRate, channels),
        currentIRBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue()),
        thisSync (0),
        fadePreviousInputSamplesRemaining (0),
        fadePreviousOutputSamplesRemaining (0),
        holdPreviousOutputSamplesRemaining (0)
    {
    }
    
    ~ConvolveChannelInternal()
    {
        for (int i = 0; i < convolveHelpers.length(); ++i)
            delete convolveHelpers.atUnchecked (i);
    }
    
    Text getName() const throw()
    {
        return "Convolve";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::FFTBuffers, IOKey::Sync);
        return keys;
    }
    
    void initChannel (const int channel) throw()
    {
        const int numChannels = this->getNumChannels();

        if ((channel % numChannels) == 0)
        {
            const UnitType& input = this->getInputAsUnit (IOKey::Generic);
            
            this->setBlockSize (input.getBlockSize (channel));
            this->setSampleRate (input.getSampleRate (channel));
            this->setOverlap (input.getOverlap (channel));
            
            const FFTBuffersType& irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());

            const Data& data = this->getState();
            const int fftSize = data.maxFFTSize <= 0 ? (int) irBuffers.getFFTEngine().length() : data.maxFFTSize;
            
            for (int i = 0; i < numChannels; ++i)
            {
                ConvolverPair* convolvePair = new ConvolverPair (fftSize);
                convolvePair->currentConvolver->reset (irBuffers, channel);
                convolveHelpers.add (convolvePair);
            }
            
            if (data.fadeSamples > 0)
                fadeBuffer = Buffer::withSize (data.fadeSamples);
                
            IntVariable& sync = ChannelInternalCore::getInputAs<IntVariable> (IOKey::Sync);
            thisSync = sync.getValue();
        }
        
        this->initProxyValue (channel, SampleType (0));
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        const Data& data = this->getState();
        FFTBuffersType newIRBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers).getValue());
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const int outputBufferLength = this->getOutputBuffer (0).length();
        
        const int numChannels = this->getNumChannels();
        
        int numSamplesRemaining = outputBufferLength;
        int pos = 0;
        
        SampleType channelLevel (0);
        SampleType channelSlope (0);

        while (numSamplesRemaining > 0)
        {
            int numSamplesThisTime = 0;
            
            if (fadePreviousInputSamplesRemaining | holdPreviousOutputSamplesRemaining | fadePreviousOutputSamplesRemaining)
            {
                if (fadePreviousInputSamplesRemaining != 0)
                {
                    SampleType* const fadeBufferSamples = fadeBuffer.getArray();
                    
                    numSamplesThisTime = plonk::min (fadePreviousInputSamplesRemaining, numSamplesRemaining);
                    
                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        const Buffer& inputBuffer (inputUnit.process (info, channel));
                        const SampleType* const inputSamples = inputBuffer.getArray() + pos;
                        SampleType* const outputSamples = this->getOutputSamples (channel) + pos;
                        plonk_assert (outputBufferLength == inputBuffer.length());

                        channelLevel = level;
                        channelSlope = slope;
                        
                        // copy input samples for fade length into the fade buffer
                        moveSamples (fadeBufferSamples, inputSamples, numSamplesThisTime);
                        fadeSamples (fadeBufferSamples, channelLevel, channelSlope, numSamplesThisTime);
                        
                        ConvolverPair& convolvePair = *convolveHelpers.atUnchecked (channel);
                        
                        // process faded input through previous
                        convolvePair.previousConvolver->process (outputSamples, fadeBufferSamples, numSamplesThisTime, channel, moveSamples, moveSamples);

                        // accumulate current as normal
                        convolvePair.currentConvolver->process (outputSamples, inputSamples, numSamplesThisTime, channel, accumulateSamples, moveSamples);
                    }
                    
                    level = channelLevel;
                    slope = channelSlope;
                    
                    fadePreviousInputSamplesRemaining -= numSamplesThisTime;
                }
                else if (holdPreviousOutputSamplesRemaining != 0)
                {
                    numSamplesThisTime = plonk::min (holdPreviousOutputSamplesRemaining, numSamplesRemaining);
                    
                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        const Buffer& inputBuffer (inputUnit.process (info, channel));
                        const SampleType* const inputSamples = inputBuffer.getArray() + pos;
                        SampleType* const outputSamples = this->getOutputSamples (channel) + pos;
                        plonk_assert (outputBufferLength == inputBuffer.length());

                        ConvolverPair& convolvePair = *convolveHelpers.atUnchecked (channel);

                        // process silence through previous
                        convolvePair.previousConvolver->process (outputSamples, 0, numSamplesThisTime, channel, moveSamples, moveZeroSamples);
                        
                        // accumulate current as normal
                        convolvePair.currentConvolver->process (outputSamples, inputSamples, numSamplesThisTime, channel, accumulateSamples, moveSamples);
                    }
                    
                    holdPreviousOutputSamplesRemaining -= numSamplesThisTime;
                    
                    if (holdPreviousOutputSamplesRemaining == 0)
                    {
                        level = SampleType (1);
                        slope = level / fadePreviousOutputSamplesRemaining;
                    }
                }
                else if (fadePreviousOutputSamplesRemaining != 0)
                {
                    numSamplesThisTime = plonk::min (fadePreviousOutputSamplesRemaining, numSamplesRemaining);
                    
                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        const Buffer& inputBuffer (inputUnit.process (info, channel));
                        const SampleType* const inputSamples = inputBuffer.getArray() + pos;
                        SampleType* const outputSamples = this->getOutputSamples (channel) + pos;
                        plonk_assert (outputBufferLength == inputBuffer.length());
                        
                        ConvolverPair& convolvePair = *convolveHelpers.atUnchecked (channel);

                        // process silence through previous
                        convolvePair.previousConvolver->process (outputSamples, 0, numSamplesThisTime, channel, moveSamples, moveZeroSamples);
                        
                        channelLevel = level;
                        channelSlope = slope;

                        // fade previous out
                        fadeSamples (outputSamples, channelLevel, channelSlope, numSamplesThisTime);
                        
                        // accumulate current as normal
                        convolvePair.currentConvolver->process (outputSamples, inputSamples, numSamplesThisTime, channel, accumulateSamples, moveSamples);
                    }
                    
                    level = channelLevel;
                    slope = channelSlope;

                    fadePreviousOutputSamplesRemaining -= numSamplesThisTime;
                }
            }
            else if (currentIRBuffers == newIRBuffers)
            {
                numSamplesThisTime = numSamplesRemaining;
                
                for (int channel = 0; channel < numChannels; ++channel)
                {
                    const Buffer& inputBuffer (inputUnit.process (info, channel));
                    const SampleType* const inputSamples = inputBuffer.getArray() + pos;
                    SampleType* const outputSamples = this->getOutputSamples (channel) + pos;
                    plonk_assert (outputBufferLength == inputBuffer.length());
                    
                    ConvolverPair& convolvePair = *convolveHelpers.atUnchecked (channel);
                    convolvePair.currentConvolver->process (outputSamples, inputSamples, numSamplesThisTime, channel, moveSamples, moveSamples);
                }
            }
            else
            {
                for (int channel = 0; channel < numChannels; ++channel)
                {
                    ConvolverPair& convolvePair = *convolveHelpers.atUnchecked (channel);
                    convolvePair.currentConvolver.swapWith (convolvePair.previousConvolver);
                }
                
                fadePreviousInputSamplesRemaining  = data.fadeSamples;
                holdPreviousOutputSamplesRemaining = data.keepPreviousTail
                                                   ? currentIRBuffers.getNumDivisions() * currentIRBuffers.getFFTEngine().halfLength()
                                                   : newIRBuffers.getFFTEngine().halfLength(); // minimum to avoid a gap due to latency
                fadePreviousOutputSamplesRemaining = data.fadeSamples;
                
                if (fadePreviousInputSamplesRemaining > 0)
                {
                    level = SampleType (1);
                    slope = level / fadePreviousInputSamplesRemaining;
                }
                
                currentIRBuffers = newIRBuffers;
                
                for (int channel = 0; channel < numChannels; ++channel)
                {
                    ConvolverPair& convolvePair = *convolveHelpers.atUnchecked (channel);
                    convolvePair.currentConvolver->reset (newIRBuffers, channel);
                }
                
                //... and back round again...
            }
            
            numSamplesRemaining -= numSamplesThisTime;
            pos                 += numSamplesThisTime;
        }
    }

private:
    PLONK_INLINE_HIGH static int decideNumChannels (Inputs const& inputs, Data const& data) throw()
    {
        return data.numChannels > 0
             ? data.numChannels
             : inputs[IOKey::FFTBuffers].asUnchecked<FFTBuffersVariableType>().getValue().getNumChannels();
    }

    //--------------------------------------------------------------------------
    
    Buffer fadeBuffer;
    FFTBuffersType currentIRBuffers;
    int thisSync;
    
    int fadePreviousInputSamplesRemaining;
    int fadePreviousOutputSamplesRemaining;
    int holdPreviousOutputSamplesRemaining;
    SampleType level;
    SampleType slope;
    
    struct ConvolverPair
    {
        ConvolverPair (const int maxFFTSize) throw()
        :   currentConvolver (new ConvolveHelperType (maxFFTSize)),
            previousConvolver (new ConvolveHelperType (maxFFTSize))
        {
        }
        
        ScopedPointerContainer<ConvolveHelperType> currentConvolver;
        ScopedPointerContainer<ConvolveHelperType> previousConvolver;
    };
    
    ObjectArray<ConvolverPair*> convolveHelpers;
};



//------------------------------------------------------------------------------

/** Convolve Unit.
 
 Latency is half the FFT size.
 
 @par Factory functions:
 - ar (input, fftBuffers)
 
 @par Inputs:
 - input: (unit, multi) the unit to convolve
 - fftBuffers: (fft-buffers) the impulse response

 
 @ingroup FFTUnits */
template<class SampleType>
class ConvolveUnit
{
public:
    typedef ConvolveChannelInternal<SampleType>                     ConvolveInternal;
    typedef typename ConvolveInternal::Data                         Data;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef FFTBuffersBase<SampleType>                              FFTBuffersType;
    typedef Variable<FFTBuffersType&>                               FFTBuffersVariableType;
    
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
    
    static PLONK_INLINE_LOW UnitType ar (UnitType const& input,
                                         FFTBuffersVariableType const& fftBuffers,
                                         const bool keepPreviousTail = false,
                                         const int numChannels = 0,
                                         const int maxFFTSize = 0,
                                         const int fadeSamples = 64,
                                         IntVariable const& sync = IntVariable()) throw()
    {
        plonk_assert ((fadeSamples % 4) == 0);
        plonk_assert ((maxFFTSize % 4) == 0);
        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::FFTBuffers, fftBuffers);
        inputs.put (IOKey::Sync, sync);
        
        Data data = { { -1.0, -1.0 }, numChannels, maxFFTSize, fadeSamples, keepPreviousTail };
        
        return UnitType::template proxiesFromInputs<ConvolveInternal> (inputs,
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


