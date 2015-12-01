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

static RNG& getConvolveRNG()
{
    static RNG rgn;
    return rgn;
}


/** Convolve channel. */
template<class SampleType>
class ConvolveChannelInternal
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef ObjectArray<ChannelType>                            ChannelArrayType;
    typedef ConvolveChannelInternal<SampleType>                 ConvolveInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef FFTEngineBase<SampleType>                           FFTEngineType;
    
    enum InternalBuffers
    {
        FFTAltBuffer0,
        FFTAltBuffer1,
        FFTTransformBuffer,
        FFTOverlapBuffer,
        FFTTempBuffer,
        FFTCalcBuffer,
        InputBuffer
    };
    
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
        fftAltSelect (0)
    {
        const FFTBuffers& irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers));
        const FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        const int fftSize = fftEngine.length();
        const int fftSize2 = fftSize * 2;
        
        buffers.add (Buffer::withSize (fftSize2));
        buffers.add (Buffer::withSize (fftSize2));
        buffers.add (Buffer::withSize (fftSize2));
        buffers.add (Buffer::withSize (fftSize2));
        buffers.add (Buffer::withSize (fftSize2));
        buffers.add (Buffer::withSize (fftSize2));

        buffers.add (Buffer::withSize (irBuffers.getOriginalLength() * 2));
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
        
        this->initValue (SampleType (0)); // not really applicable
        
        const FFTBuffers& irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers));
        const FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        
        countDown           = getConvolveRNG().uniform ((int) (fftEngine.length() / 8)) * 4;
        position0           = fftEngine.halfLength() - countDown;
        position1           = position0 + countDown;
        fftAltSelect        = 0;
        divisionsCurrent    = 0;
        divisionsCounter    = 0;
        divisionsWritten    = 0;
        divisionsPrevious   = 0;
        divisionsRead       = 1;

        buffers.atUnchecked (FFTAltBuffer0).zero();
        buffers.atUnchecked (FFTAltBuffer1).zero();
        buffers.atUnchecked (FFTTransformBuffer).zero();
        buffers.atUnchecked (FFTOverlapBuffer).zero();
        buffers.atUnchecked (FFTTempBuffer).zero();
    }
    
    static inline void complexMultiplyAccumulate (const SampleType* left, const SampleType* right,
                                                  SampleType* output, SampleType* temp,
                                                  const int length, const int halfLength) throw()
    {
        const SampleType* const leftRealSamples   = left;
        const SampleType* const rightRealSamples  = right;
        const SampleType* const leftImagSamples   = leftRealSamples + halfLength;
        const SampleType* const rightImagSamples  = rightRealSamples + halfLength;
        SampleType* const realTempSamples         = temp;
        SampleType* const imagTempSamples         = realTempSamples + halfLength;

        const SampleType leftDC = leftRealSamples[0];
        const SampleType rightDC = rightRealSamples[0];
        const SampleType leftNyquist = leftImagSamples[0];
        const SampleType rightNyquist = rightImagSamples[0];
        
        NumericalArrayComplex<SampleType>::zmul (realTempSamples, imagTempSamples,
                                                 leftRealSamples, leftImagSamples,
                                                 rightRealSamples, rightImagSamples,
                                                 halfLength);
        
        realTempSamples[0] = leftDC * rightDC;
        imagTempSamples[0] = leftNyquist * rightNyquist;
        
        NumericalArrayBinaryOp<SampleType,plonk::addop>::calcNN (output, output, temp, length);
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        FFTBuffers& irBuffers (this->getInputAsFFTBuffers (IOKey::FFTBuffers));
        FFTEngineType& fftEngine (irBuffers.getFFTEngine());
        
        const int fftSize = fftEngine.length();
        const int fftSizeHalved = fftEngine.halfLength();
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* inputSamples = inputBuffer.getArray();
        
        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        plonk_assert (outputBufferLength == inputBuffer.length());

        SampleType* const inputBufferBase    = buffers.atUnchecked (InputBuffer).getArray();
        SampleType* const fftTransformBuffer = buffers.atUnchecked (FFTTransformBuffer).getArray();
        SampleType* const fftOverlapBuffer   = buffers.atUnchecked (FFTOverlapBuffer).getArray();
        SampleType* const fftTempBuffer      = buffers.atUnchecked (FFTTempBuffer).getArray();
        SampleType* const fftCalcBuffer      = buffers.atUnchecked (FFTCalcBuffer).getArray();
        SampleType* const fftAltBuffer0      = buffers.atUnchecked (FFTAltBuffer0).getArray();
        SampleType* const fftAltBuffer1      = buffers.atUnchecked (FFTAltBuffer1).getArray();
        SampleType* const fftAltBuffers[]    = { fftAltBuffer0, fftAltBuffer1 };
        
        int samplesRemaining = outputBufferLength;
        const int numDivisions = irBuffers.getNumDivisions();

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
                NumericalArrayUnaryOp<SampleType, plonk::move>::calc (fftAltBuffer0 + position0, inputSamples, hop);
                NumericalArrayUnaryOp<SampleType, plonk::move>::calc (fftAltBuffer1 + position1, inputSamples, hop);
                NumericalArrayUnaryOp<SampleType, plonk::move>::calc (outputSamples, fftOverlapBuffer + position0, hop);

                inputSamples  += hop;
                outputSamples += hop;
                position0     += hop;
                position1     += hop;
            }
            
            hop = (divisionsWritten >= divisionsRead - 1) ? 0 : 1;
            ++divisionsCounter;
            
            while (hop != 0)
            {
                int divisionsRemaining = (int) ((SampleType) ((divisionsCounter * (divisionsRead - 1)) / (SampleType) ((fftSizeHalved / outputBufferLength) - 1)) - divisionsWritten);
                
                if (divisionsCounter >= (fftSizeHalved / outputBufferLength) - 1)
                    divisionsRemaining = (divisionsRead - divisionsWritten) - 1;
                    
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
                
                const SampleType* irSamples = irBuffers.getDivision (channel, divisionsWritten + 1);
                const SampleType* inputBufferSamples = inputBufferBase + nextDivision * fftSize;
                
                for (int i = 0; i < divisionsRemaining; i++)
                {
                    complexMultiplyAccumulate (inputBufferSamples, irSamples, fftTempBuffer, fftCalcBuffer, fftSize, fftSizeHalved);
                    inputBufferSamples += fftSize;
                    irSamples += fftSize;
                }

                divisionsWritten += divisionsRemaining;
            }
            
            if (countDown == 0)
            {
                divisionsRead = plonk::min (divisionsRead + 1, numDivisions);
                
                const SampleType* const irSamples = irBuffers.getBuffer (channel);
                SampleType* const inputBufferSamples = inputBufferBase + divisionsCurrent * fftSize;

                fftEngine.forward (inputBufferSamples, fftAltBuffers[fftAltSelect]);
                complexMultiplyAccumulate (inputBufferSamples, irSamples, fftTempBuffer, fftCalcBuffer, fftSize, fftSizeHalved);
                fftEngine.inverse (fftTransformBuffer, fftTempBuffer);
                
                hop = fftSizeHalved;
                SampleType* overlap1 = fftOverlapBuffer + (hop * (1 - fftAltSelect));
                SampleType* overlap2 = fftOverlapBuffer + (hop * fftAltSelect);
                    
                NumericalArrayUnaryOp<SampleType, plonk::move>::calc (overlap1, fftTransformBuffer, fftSize);
                NumericalArrayBinaryOp<SampleType, plonk::addop>::calcNN (overlap2, overlap2, fftTransformBuffer + hop, fftSize);
                                    
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
                                
                divisionsCurrent    = plonk::wrap (divisionsCurrent, 0, numDivisions);
                divisionsPrevious   = divisionsCurrent + 1;
                divisionsCounter    = 0;
                divisionsWritten    = 0;
                countDown           = hop;
                
                NumericalArray<SampleType>::zeroData (fftTempBuffer, fftSize);
            }
        }
    }
    
private:
    ObjectArray<Buffer> buffers;
    int divisionsCurrent;
    int divisionsWritten;
    int divisionsCounter;
    int divisionsPrevious;
    int divisionsRead;
    int countDown;
    int position0, position1;
    int fftAltSelect;
};



//------------------------------------------------------------------------------

/** Convolve Unit.
 
 @ingroup FFTUnits */
template<class SampleType>
class ConvolveUnit
{
public:
    typedef ConvolveChannelInternal<SampleType>     ConvolveInternal;
    typedef typename ConvolveInternal::Data         Data;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    
//    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
//    {
//        return UnitInfo ("Convolve", "Convolve with an impulse response.",
//                         
//                         // output
//                         ChannelCount::VariableChannelCount,
//                         IOKey::FFTPacked,          Measure::FFTPacked,          IOInfo::NoDefault,  IOLimit::None,
//                         IOKey::End,
//
//                         // inputs
//                         IOKey::Generic,            Measure::None,
//                         IOKey::End);
//    }
    
    static PLONK_INLINE_LOW UnitType ar (UnitType const& input, FFTBuffers const& fftBuffers) throw()
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


