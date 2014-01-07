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

#ifndef PLONK_RESAMPLECHANNEL_H
#define PLONK_RESAMPLECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


/** Resampler. */
template<class SampleType, Interp::TypeCode InterpTypeCode>
class ResampleChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                               Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ObjectArray<ChannelType>                                ChannelArrayType;
    typedef ResampleChannelInternal<SampleType,InterpTypeCode>      ResampleInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>              Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef ObjectArray<Buffer>                                     BufferArray;
    typedef typename TypeUtility<SampleType>::IndexType             IndexType;
    
    typedef typename TypeUtility<SampleType>::IndexType             RateType;
    typedef UnitBase<RateType>                                      RateUnitType;
    typedef NumericalArray<RateType>                                RateBufferType;
    
    typedef InterpSelect<SampleType,IndexType,InterpTypeCode>       InterpSelectType;
    typedef typename InterpSelectType::InterpType                   InterpType;
    typedef typename InterpType::ExtensionBuffer                    ExtensionBuffer;
    
    ResampleChannelInternal (Inputs const& inputs,
                             Data const& data,
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate,
                             ChannelArrayType& channels) throw()
    :   Internal (inputs.getMaxNumChannels(), inputs, data, blockSize, sampleRate, channels),
        tempBuffers (BufferArray::withSize (inputs.getMaxNumChannels())),
        tempBufferPos (0),
        nextInputTimeStamp (TimeStamp::getZero())
    {
        plonk_assert (sampleRate.getValue() > 0.0);       // no need to resample a DC signal
        
        // should check the input is all the same sample rate too...
    }
    
    Text getName() const throw()
    {
        return "Resample";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Rate);
        return keys;
    }
    
    /*
    void initChannel (const int channel) throw()
    {
        if ((channel % this->getNumChannels()) == 0)
        {
            const AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
            
            double fileSampleRate = file.getSampleRate();
            
            if (fileSampleRate <= 0.0)
                fileSampleRate = file.getDefaultSampleRate();
                
                this->setSampleRate (SampleRate::decide (fileSampleRate, this->getSampleRate()));
                buffer.setSize (this->getBlockSize().getValue() * file.getNumChannels(), false);
                }
        
        this->initProxyValue (channel, 0);
    }
*/
    void initChannel (const int channel) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        const SampleType sourceValue = input.getValue (channel);
        const RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
        plonk_assert (input.getOverlap (channel) == Math<DoubleVariable>::get1());
        plonk_assert (rateUnit.getOverlap (channel) == Math<DoubleVariable>::get1());

        if (rateUnit.getBlockSize (channel).getValue() != this->getBlockSize().getValue())
            this->ratePositions.setSize (this->getNumChannels(), false);
        
        if ((channel % this->getNumChannels()) == 0)
        {
            // larger for interpolation
            resizeTempBuffer (input.getBlockSize (0).getValue() + InterpType::getExtension());
            tempBufferPos = tempBufferPosMax;
        }
        
        tempBuffers.atUnchecked (channel).zero();
        tempBuffers.atUnchecked (channel).put (tempBufferPos, sourceValue);

        this->initProxyValue (channel, sourceValue);
    }
    
    inline void resizeTempBuffer (const int inputBufferLength) throw()
    {
        if (inputBufferLength != tempBuffers.atUnchecked (0).length())
        {
            for (int i = 0; i < tempBuffers.length(); ++i)
            {
                Buffer& tempBuffer = tempBuffers.atUnchecked (i);
                tempBuffer.setSize (inputBufferLength, false);
            }
            
            tempBufferUsableLength = IndexType (inputBufferLength) - InterpType::getExtensionAsIndex();
            tempBufferPosMax = tempBufferUsableLength + InterpType::getOffsetAsIndex();
        }
    }
    
    inline void getNextInputBuffer (ProcessInfo& info) throw()
    {
        const int extension = InterpType::getExtension();
        const int numChannels = this->getNumChannels();

        info.setTimeStamp (nextInputTimeStamp);

        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));        
        
        tempBufferPos -= tempBufferUsableLength;

        for (int channel = 0; channel < numChannels; ++channel)
        {        
            const Buffer& inputBuffer (inputUnit.process (info, channel));
            const SampleType* const inputSamples = inputBuffer.getArray();
            const int inputBufferLength = inputBuffer.length();
            
            ExtensionBuffer interpolationValues;
            Buffer::copyData (interpolationValues.buffer,
                              tempBuffers.atUnchecked (channel).getArray() + tempBuffers.atUnchecked (channel).length() - extension,
                              extension);
            
            resizeTempBuffer (inputBufferLength + extension);
            
            Buffer::copyData (tempBuffers.atUnchecked (channel).getArray(),
                              interpolationValues.buffer,
                              extension);
            
            Buffer::copyData (tempBuffers.atUnchecked (channel).getArray() + extension,
                              inputSamples,
                              inputBufferLength);
        }
        
        nextInputTimeStamp = inputUnit.getNextTimeStamp (0);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        const Data& data = this->getState();
        
        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        const IndexType inputSampleRate = IndexType (inputUnit.getSampleRate (0)); // should be the same sample rate for each input channel
        
        RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
        
        const int outputBufferLength = this->getOutputBuffer (0).length();
        const int numChannels = this->getNumChannels();
        
        if (inputSampleRate <= 0.0)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                const SampleType inputValue = inputUnit.process (info, channel).atUnchecked (0);
                SampleType* const outputSamples = this->getOutputSamples (channel);
                NumericalArrayFiller<SampleType>::fill (outputSamples, inputValue, outputBufferLength);
            }
        }
        else
        {
            const TimeStamp infoTimeStamp = info.getTimeStamp();
                        
            const RateBufferType& rateBuffer (rateUnit.process (info, 0));
            const int rateBufferLength = rateBuffer.length();
                                    
            if (rateBufferLength == 1)
            {
                const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateBuffer.getArray() [0]);
                
                if (tempBufferIncrement == Math<IndexType>::get0())
                {
                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        SampleType* tempBufferSamples = this->tempBuffers.atUnchecked (channel).getArray();
                        const SampleType dcOutput = InterpType::lookup (tempBufferSamples, tempBufferPos);
                        SampleType* const outputSamples = this->getOutputSamples (channel);
                        NumericalArrayFiller<SampleType>::fill (outputSamples, dcOutput, outputBufferLength);
                    }
                }
                else
                {
                    int outputSamplePosition = 0;

                    while (outputSamplePosition < outputBufferLength)
                    {                        
                        if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
                            getNextInputBuffer (info);
                            
                        int channelSamplePosition;
                        IndexType channelBufferPos;
                        
                        for (int channel = 0; channel < numChannels; ++channel)
                        {
                            channelSamplePosition = outputSamplePosition;
                            channelBufferPos = tempBufferPos;
                            
                            const SampleType* const tempBufferSamples = tempBuffers.atUnchecked (channel).getArray();
                            SampleType* const outputSamples = this->getOutputSamples (channel);

                            for (channelSamplePosition = outputSamplePosition;
                                 (channelSamplePosition < outputBufferLength) && (channelBufferPos < tempBufferPosMax);
                                 ++channelSamplePosition)
                            {
                                outputSamples[channelSamplePosition] = InterpType::lookup (tempBufferSamples, channelBufferPos);
                                channelBufferPos += tempBufferIncrement;
                            }
                        }
                        
                        outputSamplePosition = channelSamplePosition;
                        tempBufferPos = channelBufferPos;
                    }
                }
            }
            else if (rateBufferLength == outputBufferLength)
            {
                int outputSamplePosition = 0;
                
                while (outputSamplePosition < outputBufferLength)
                {
                    if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
                        getNextInputBuffer (info);
                        
                    int channelSamplePosition;
                    IndexType channelBufferPos;
                    
                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        channelSamplePosition = outputSamplePosition;
                        channelBufferPos = tempBufferPos;
                        
                        const SampleType* const tempBufferSamples = tempBuffers.atUnchecked (channel).getArray();
                        SampleType* const outputSamples = this->getOutputSamples (channel);
                        const SampleType* const rateSamples = rateBuffer.getArray();
                        
                        for (channelSamplePosition = outputSamplePosition;
                             (channelSamplePosition < outputBufferLength) && (channelBufferPos < tempBufferPosMax);
                             ++channelSamplePosition)
                        {
                            outputSamples[channelSamplePosition] = InterpType::lookup (tempBufferSamples, channelBufferPos);
                            const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateSamples[channelSamplePosition]);
                            channelBufferPos += tempBufferIncrement;
                        }
                    }
                    
                    outputSamplePosition = channelSamplePosition;
                    tempBufferPos = channelBufferPos;
                }
            }
            else
            {
                int outputSamplePosition = 0;
                this->ratePositions.zero();
                double* const ratePositionArray = this->ratePositions.getArray();
                
                while (outputSamplePosition < outputBufferLength)
                {
                    if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
                        getNextInputBuffer (info);
                        
                    int channelSamplePosition;
                    IndexType channelBufferPos = tempBufferPos;
                    
                    for (int channel = 0; channel < numChannels; ++channel)
                    {
                        channelSamplePosition = outputSamplePosition;
                        
                        const SampleType* const tempBufferSamples = tempBuffers.atUnchecked (channel).getArray();
                        SampleType* const outputSamples = this->getOutputSamples (channel);
                        const SampleType* const rateSamples = rateBuffer.getArray();
                        
                        const double rateIncrement = double (rateBufferLength) / double (outputBufferLength);
                        
                        for (channelSamplePosition = outputSamplePosition;
                             (channelSamplePosition < outputBufferLength) && (channelBufferPos < tempBufferPosMax);
                             ++channelSamplePosition)
                        {
                            outputSamples[channelSamplePosition] = InterpType::lookup (tempBufferSamples, channelBufferPos);
                            const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateSamples[int (ratePositionArray[channel])]);
                            channelBufferPos += tempBufferIncrement;
                            ratePositionArray[channel] += rateIncrement;
                        }
                    }
                    
                    outputSamplePosition = channelSamplePosition;
                    tempBufferPos = channelBufferPos;
                }
            }
            
            info.setTimeStamp (infoTimeStamp); // reset for the parent graph
        }
    }
    
private:
    BufferArray tempBuffers;
    IndexType tempBufferPos;
    IndexType tempBufferPosMax;
    IndexType tempBufferUsableLength;
    TimeStamp nextInputTimeStamp;
    DoubleArray ratePositions;
};

///** Resampler. */
//template<class SampleType, Interp::TypeCode InterpTypeCode>
//class ResampleChannelInternal
//:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
//{
//public:
//    typedef ChannelInternalCore::Data                               Data;
//    typedef ChannelBase<SampleType>                                 ChannelType;
//    typedef ResampleChannelInternal<SampleType,InterpTypeCode>      ResampleInternal;
//    typedef ChannelInternal<SampleType,Data>                        Internal;
//    typedef ChannelInternalBase<SampleType>                         InternalBase;
//    typedef UnitBase<SampleType>                                    UnitType;
//    typedef InputDictionary                                         Inputs;
//    typedef NumericalArray<SampleType>                              Buffer;
//    typedef typename TypeUtility<SampleType>::IndexType             IndexType;
//    
//    typedef typename TypeUtility<SampleType>::IndexType             RateType;
//    typedef UnitBase<RateType>                                      RateUnitType;
//    typedef NumericalArray<RateType>                                RateBufferType;
//    
//    typedef InterpSelect<SampleType,IndexType,InterpTypeCode>       InterpSelect;
//    typedef typename InterpSelect::InterpType                       InterpType;
//    typedef typename InterpType::ExtensionBuffer                    ExtensionBuffer;
//    
//    ResampleChannelInternal (Inputs const& inputs,
//                             Data const& data,
//                             BlockSize const& blockSize,
//                             SampleRate const& sampleRate) throw()
//    :   Internal (inputs, data, blockSize, sampleRate),
//    tempBufferPos (0),
//    nextInputTimeStamp (TimeStamp::getZero())
//    {
//        plonk_assert (sampleRate.getValue() > 0.0);       // no need to resample a DC signal
//    }
//    
//    Text getName() const throw()
//    {
//        return "Resample";
//    }
//    
//    IntArray getInputKeys() const throw()
//    {
//        const IntArray keys (IOKey::Generic, IOKey::Rate);
//        return keys;
//    }
//    
//    InternalBase* getChannel (const int index) throw()
//    {
//        const Inputs channelInputs = this->getInputs().getChannel (index);
//        return new ResampleInternal (channelInputs,
//                                     this->getState(),
//                                     this->getBlockSize(),
//                                     this->getSampleRate());
//    }
//    
//    void initChannel (const int channel) throw()
//    {
//        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
//        
//#ifdef PLONK_DEBUG
//        const RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
//        plonk_assert (input.getOverlap (channel) == Math<DoubleVariable>::get1());
//        plonk_assert (rateUnit.getOverlap (channel) == Math<DoubleVariable>::get1());
//#endif
//        
//        const SampleType sourceValue = input.getValue (channel);
//        this->initValue (sourceValue);
//        
//        // larger for interpolation
//        resizeTempBuffer (input.getBlockSize (channel).getValue() + InterpType::getExtension());
//        tempBuffer.zero(); // was a bug on interpolation here..
//        tempBufferPos = tempBufferPosMax;
//        
//        tempBuffer.put (tempBufferPos, sourceValue);
//    }
//    
//    inline void resizeTempBuffer (const int inputBufferLength) throw()
//    {
//        if (inputBufferLength != tempBuffer.length())
//        {
//            tempBuffer.setSize (inputBufferLength, false);
//            tempBufferUsableLength = IndexType (inputBufferLength) - InterpType::getExtensionAsIndex();
//            tempBufferPosMax = tempBufferUsableLength + InterpType::getOffsetAsIndex();
//        }
//    }
//    
//    inline void getNextInputBuffer (ProcessInfo& info, const int channel, SampleType* &tempBufferSamples, int& tempBufferLength) throw()
//    {
//        const int extension = InterpType::getExtension();
//        
//        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
//        
//        
//        // bug here for multiple channels ??
//        // as a result, might need to make the timestamp access virtual or shared between proxies and proxyowners?
//        // won't help as the first channel getting the next buffer obliterates the remaining channels for the previous buffer
//        
//        info.setTimeStamp (nextInputTimeStamp);
//        const Buffer& inputBuffer (inputUnit.process (info, channel));
//        nextInputTimeStamp = inputUnit.getNextTimeStamp (channel);
//        
//        const SampleType* const inputSamples = inputBuffer.getArray();
//        const int inputBufferLength = inputBuffer.length();
//        
//        ExtensionBuffer interpolationValues;
//        Buffer::copyData (interpolationValues.buffer,
//                          tempBufferSamples + tempBufferLength - extension,
//                          extension);
//        
//        tempBufferPos -= tempBufferUsableLength;               // move this before resize
//        tempBufferLength = inputBufferLength + extension;      // larger for interpolation
//        resizeTempBuffer (tempBufferLength);
//        tempBufferSamples = this->tempBuffer.getArray();
//        
//        Buffer::copyData (tempBufferSamples,
//                          interpolationValues.buffer,
//                          extension);
//        
//        Buffer::copyData (tempBufferSamples + extension,
//                          inputSamples,
//                          inputBufferLength);
//    }
//    
//    void process (ProcessInfo& info, const int channel) throw()
//    {
//        const Data& data = this->getState();
//        
//        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
//        const IndexType inputSampleRate = IndexType (inputUnit.getSampleRate (channel));
//        
//        RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
//        
//        const int outputBufferLength = this->getOutputBuffer().length();
//        SampleType* outputSamples = this->getOutputSamples();
//        
//        if (inputSampleRate <= 0.0)
//        {
//            const SampleType inputValue = inputUnit.process (info, channel).atUnchecked (0);
//            NumericalArrayFiller<SampleType>::fill (outputSamples, inputValue, outputBufferLength);
//        }
//        else
//        {
//            const TimeStamp infoTimeStamp = info.getTimeStamp();
//            SampleType* const outputSamplesEnd = outputSamples + outputBufferLength;
//            
//            const RateBufferType& rateBuffer (rateUnit.process (info, channel));
//            const RateType* rateSamples = rateBuffer.getArray();
//            const int rateBufferLength = rateBuffer.length();
//            
//            if (rateBufferLength == 1)
//            {
//                const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateSamples[0]);
//                
//                if (tempBufferIncrement == Math<IndexType>::get0())
//                {
//                    SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                    const SampleType dcOutput = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                    NumericalArrayFiller<SampleType>::fill (outputSamples, dcOutput, outputBufferLength);
//                }
//                else
//                {
//                    while (outputSamples < outputSamplesEnd)
//                    {
//                        int tempBufferLength = this->tempBuffer.length();
//                        SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                        
//                        if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
//                            getNextInputBuffer (info, channel, tempBufferSamples, tempBufferLength);
//                            
//                            while ((outputSamples < outputSamplesEnd) && (tempBufferPos < tempBufferPosMax))
//                            {
//                                *outputSamples++ = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                                tempBufferPos += tempBufferIncrement;
//                            }
//                    }
//                }
//            }
//            else if (rateBufferLength == outputBufferLength)
//            {
//                while (outputSamples < outputSamplesEnd)
//                {
//                    int tempBufferLength = this->tempBuffer.length();
//                    SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                    
//                    if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
//                        getNextInputBuffer (info, channel, tempBufferSamples, tempBufferLength);
//                        
//                        while ((outputSamples < outputSamplesEnd) && (tempBufferPos < tempBufferPosMax))
//                        {
//                            *outputSamples++ = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                            const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * *rateSamples++);
//                            tempBufferPos += tempBufferIncrement;
//                        }
//                }
//            }
//            else
//            {
//                while (outputSamples < outputSamplesEnd)
//                {
//                    int tempBufferLength = this->tempBuffer.length();
//                    SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                    
//                    if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
//                        getNextInputBuffer (info, channel, tempBufferSamples, tempBufferLength);
//                        
//                        double ratePosition = 0.0;
//                        const double rateIncrement = double (rateBufferLength) / double (outputBufferLength);
//                        
//                        while ((outputSamples < outputSamplesEnd) && (tempBufferPos < tempBufferPosMax))
//                        {
//                            *outputSamples++ = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                            const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateSamples[int (ratePosition)]);
//                            tempBufferPos += tempBufferIncrement;
//                            ratePosition += rateIncrement;
//                        }
//                }
//            }
//            
//            info.setTimeStamp (infoTimeStamp); // reset for the parent graph
//        }
//    }
//    
//private:
//    Buffer tempBuffer;
//    IndexType tempBufferPos;
//    IndexType tempBufferPosMax;
//    IndexType tempBufferUsableLength;
//    TimeStamp nextInputTimeStamp;
//};

//------------------------------------------------------------------------------

/** Resample a unit to a different sample rate and/or block size.
 
 @par Factory functions:
 - ar (input, rate=1, preferredBlockSize=default, preferredSampleRate=default)
 - kr (input, rate=1)
 
 @par Inputs:
 - input: (unit, multi) the unit to resample
 - rate: (unit) a modulatable rate multiplier (e.g., for varispeed playback)
 - preferredBlockSize: the preferred output block size 
 - preferredSampleRate: the preferred output sample rate

 @ingroup ConverterUnits */
template<class SampleType, Interp::TypeCode InterpTypeCode>
class ResampleUnit
{
public:    
    typedef ResampleChannelInternal<SampleType,InterpTypeCode>      ResampleInternal;
    typedef typename ResampleInternal::Data                         Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;    

    typedef typename ResampleInternal::RateType         RateType;
    typedef typename ResampleInternal::RateUnitType     RateUnitType;
    typedef typename ResampleInternal::RateBufferType   RateBufferType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Resample", "Resamples signals in terms of block size and/or sample rate.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,      IOKey::End,

                         // inputs
                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
                         IOKey::Rate,       Measure::Factor,   1.0,                 IOLimit::Minimum,   Measure::Factor,    0.0,
                         IOKey::BlockSize,  Measure::Samples,  blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate, Measure::Hertz,    sampleRate,          IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }    
    
//    /** Create an audio rate sample rate converter. */
//    static UnitType ar (UnitType const& input,
//                        RateUnitType const& rate = Math<RateUnitType>::get1(),
//                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
//                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
//    {
//        plonk_assert (preferredSampleRate.getValue() > 0.0); // no need to resample a DC signal
//        
//        Inputs inputs;
//        inputs.put (IOKey::Generic, input);
//        inputs.put (IOKey::Rate, rate);
//        
//        Data data = { -1.0, -1.0 };
//        
//        return UnitType::template createFromInputs<ResampleInternal> (inputs,
//                                                                      data,
//                                                                      preferredBlockSize,
//                                                                      preferredSampleRate);
//    }

    /** Create an audio rate sample rate converter. */
    static UnitType ar (UnitType const& input,
                        RateUnitType const& rate = Math<RateUnitType>::get1(),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        plonk_assert (preferredSampleRate.getValue() > 0.0); // no need to resample a DC signal
        plonk_assert (rate.getNumChannels() == 1);

        bool needsResample = false;
        
        if (rate != Math<RateUnitType>::get1())
        {
            needsResample = true;
        }
        else
        {
            for (int i = 0; i < input.getNumChannels(); ++i)
            {
                if (input.getSampleRate (i) != preferredSampleRate)
                {
                    needsResample = true;
                    break;
                }
            }
        }
        
        if (!needsResample)
            return input;

        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Rate, rate);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template proxiesFromInputs<ResampleInternal> (inputs,
                                                                       data,
                                                                       preferredBlockSize,
                                                                       preferredSampleRate);
    }
    
    static inline UnitType kr (UnitType const& input, RateUnitType const& rate) throw()
    {
        return ar (input,
                   rate,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
    
    static inline UnitType kr (UnitType const& input) throw()
    {
        return ar (input,
                   Math<RateUnitType>::get1(),
                   BlockSize::getControlRateBlockSize(),
                   SampleRate::getControlRate());
    }
    
};

///** Resampler. */
//template<class SampleType, Interp::TypeCode InterpTypeCode>
//class ResampleChannelInternal
//:   public ProxyOwnerChannelInternal<SampleType, ChannelInternalCore::Data>
//{
//public:
//    typedef ChannelInternalCore::Data                               Data;
//    typedef ChannelBase<SampleType>                                 ChannelType;
//    typedef ObjectArray<ChannelType>                                ChannelArrayType;
//    typedef ResampleChannelInternal<SampleType,InterpTypeCode>      ResampleInternal;
//    typedef ProxyOwnerChannelInternal<SampleType,Data>              Internal;
//    typedef UnitBase<SampleType>                                    UnitType;
//    typedef InputDictionary                                         Inputs;
//    typedef NumericalArray<SampleType>                              Buffer;
//    typedef typename TypeUtility<SampleType>::IndexType             IndexType;
//    
//    typedef typename TypeUtility<SampleType>::IndexType             RateType;
//    typedef UnitBase<RateType>                                      RateUnitType;
//    typedef NumericalArray<RateType>                                RateBufferType;
//    
//    typedef InterpSelect<SampleType,IndexType,InterpTypeCode>       InterpSelect;
//    typedef typename InterpSelect::InterpType                       InterpType;
//    typedef typename InterpType::ExtensionBuffer                    ExtensionBuffer;
//
//    struct InputBufferPositionData
//    {
//        IndexType tempBufferPos;
//        IndexType tempBufferPosMax;
//        IndexType tempBufferUsableLength;
//        TimeStamp nextInputTimeStamp;
//    };
//    
//    struct InputBufferData
//    {
//        Buffer tempBuffer;
//        InputBufferPositionData position;
//    };
//    
//    typedef ObjectArray<InputBufferData>                            InputBufferDataArray;
//    
//    ResampleChannelInternal (Inputs const& inputs,
//                             Data const& data,
//                             BlockSize const& blockSize,
//                             SampleRate const& sampleRate,
//                             ChannelArrayType& channels) throw()
//    :   Internal (inputs.getMaxNumChannels(), inputs, data, blockSize, sampleRate, channels)
//    {
//        plonk_assert (sampleRate.getValue() > 0.0);       // no need to resample a DC signal
//        
//        buffers.setSize (this->getNumChannels());
//        
//        for (int i = 0; i < buffers.length(); ++i)
//            Memory::zero (buffers[i].position);
//    }
//    
//    Text getName() const throw()
//    {
//        return "Resample";
//    }
//    
//    IntArray getInputKeys() const throw()
//    {
//        const IntArray keys (IOKey::Generic, IOKey::Rate);
//        return keys;
//    }
//        
//    void initChannel (const int channel) throw()
//    {
//        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
//        
//#ifdef PLONK_DEBUG
//        const RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
//        plonk_assert (input.getOverlap (channel) == Math<DoubleVariable>::get1());
//        plonk_assert (rateUnit.getOverlap (channel) == Math<DoubleVariable>::get1());
//#endif
//        
//        const SampleType sourceValue = input.getValue (channel);
//        this->initProxyValue (channel, sourceValue);
//        
//        resizeTempBuffer (channel, input.getBlockSize (channel).getValue() + InterpType::getExtension());
//        buffers[channel].tempBuffer.zero();
//        
//        const int tempBufferPos (buffers[channel].position.tempBufferPosMax);
//        buffers[channel].position.tempBufferPos = IndexType (tempBufferPos);
//        buffers[channel].tempBuffer.put (tempBufferPos, sourceValue);
//    }
//    
//    inline void resizeTempBuffer (const int channel, const int inputBufferLength) throw()
//    {
//        if (inputBufferLength != tempBuffer.length())
//        {
//            buffers[channel].tempBuffer.setSize (inputBufferLength, false);
//            
//            const IndexType tempBufferUsableLength = IndexType (inputBufferLength) - InterpType::getExtensionAsIndex();
//            buffers[channel].position.tempBufferUsableLength = tempBufferUsableLength;
//            buffers[channel].position.tempBufferPosMax = tempBufferUsableLength + InterpType::getOffsetAsIndex();
//        }
//    }
//    
//    inline void getNextInputBuffer (ProcessInfo& info, const int channel, SampleType* &tempBufferSamples, int& tempBufferLength) throw()
//    {
//        const int extension = InterpType::getExtension();
//        
//        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
//        
//        info.setTimeStamp (buffers[channel].position.nextInputTimeStamp);
//        const Buffer& inputBuffer (inputUnit.process (info, channel));
//        buffers[channel].position.nextInputTimeStamp = inputUnit.getNextTimeStamp (channel);
//        
//        const SampleType* const inputSamples = inputBuffer.getArray();
//        const int inputBufferLength = inputBuffer.length();
//        
//        ExtensionBuffer interpolationValues;
//        Buffer::copyData (interpolationValues.buffer,
//                          tempBufferSamples + tempBufferLength - extension,
//                          extension);
//        
//        tempBufferPos -= tempBufferUsableLength;               // move this before resize
//        tempBufferLength = inputBufferLength + extension;      // larger for interpolation
//        resizeTempBuffer (channel, tempBufferLength);
//        tempBufferSamples = buffers[channel].tempBuffer.getArray();
//        
//        Buffer::copyData (tempBufferSamples,
//                          interpolationValues.buffer,
//                          extension);
//        
//        Buffer::copyData (tempBufferSamples + extension,
//                          inputSamples,
//                          inputBufferLength);
//    }
//    
//    void process (ProcessInfo& info, const int /*channel*/) throw()
//    {
//        const Data& data = this->getState();
//        
//        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
//        RateUnitType& rateUnit = ChannelInternalCore::getInputAs<RateUnitType> (IOKey::Rate);
//
//        
//        const IndexType inputSampleRate = IndexType (inputUnit.getSampleRate (channel));
//        
//        const int outputBufferLength = this->getOutputBuffer().length();
//        SampleType* outputSamples = this->getOutputSamples();
//        
//        if (inputSampleRate <= 0.0)
//        {
//            const SampleType inputValue = inputUnit.process (info, channel).atUnchecked (0);
//            NumericalArrayFiller<SampleType>::fill (outputSamples, inputValue, outputBufferLength);
//        }
//        else
//        {
//            const TimeStamp infoTimeStamp = info.getTimeStamp();
//            SampleType* const outputSamplesEnd = outputSamples + outputBufferLength;
//            
//            const RateBufferType& rateBuffer (rateUnit.process (info, channel));
//            const RateType* rateSamples = rateBuffer.getArray();
//            const int rateBufferLength = rateBuffer.length();
//            
//            if (rateBufferLength == 1)
//            {
//                const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateSamples[0]);
//                
//                if (tempBufferIncrement == Math<IndexType>::get0())
//                {
//                    SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                    const SampleType dcOutput = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                    NumericalArrayFiller<SampleType>::fill (outputSamples, dcOutput, outputBufferLength);
//                }
//                else
//                {
//                    while (outputSamples < outputSamplesEnd)
//                    {
//                        int tempBufferLength = this->tempBuffer.length();
//                        SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                        
//                        if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
//                            getNextInputBuffer (info, channel, tempBufferSamples, tempBufferLength);
//                            
//                        while ((outputSamples < outputSamplesEnd) && (tempBufferPos < tempBufferPosMax))
//                        {
//                            *outputSamples++ = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                            tempBufferPos += tempBufferIncrement;
//                        }
//                    }
//                }
//            }
//            else if (rateBufferLength == outputBufferLength)
//            {
//                while (outputSamples < outputSamplesEnd)
//                {
//                    int tempBufferLength = this->tempBuffer.length();
//                    SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                    
//                    if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
//                        getNextInputBuffer (info, channel, tempBufferSamples, tempBufferLength);
//                        
//                    while ((outputSamples < outputSamplesEnd) && (tempBufferPos < tempBufferPosMax))
//                    {
//                        *outputSamples++ = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                        const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * *rateSamples++);
//                        tempBufferPos += tempBufferIncrement;
//                    }
//                }
//            }
//            else
//            {
//                while (outputSamples < outputSamplesEnd)
//                {
//                    int tempBufferLength = this->tempBuffer.length();
//                    SampleType* tempBufferSamples = this->tempBuffer.getArray();
//                    
//                    if (tempBufferPos >= tempBufferPosMax) // ran out of buffer
//                        getNextInputBuffer (info, channel, tempBufferSamples, tempBufferLength);
//                        
//                    double ratePosition = 0.0;
//                    const double rateIncrement = double (rateBufferLength) / double (outputBufferLength);
//                    
//                    while ((outputSamples < outputSamplesEnd) && (tempBufferPos < tempBufferPosMax))
//                    {
//                        *outputSamples++ = InterpType::lookup (tempBufferSamples, tempBufferPos);
//                        const IndexType tempBufferIncrement (inputSampleRate * IndexType (data.sampleDuration) * rateSamples[int (ratePosition)]);
//                        tempBufferPos += tempBufferIncrement;
//                        ratePosition += rateIncrement;
//                    }
//                }
//            }
//            
//            info.setTimeStamp (infoTimeStamp); // reset for the parent graph
//        }
//    }
//    
//private:
//    InputBufferDataArray buffers;
//};
//
////------------------------------------------------------------------------------
//
///** Resample a unit to a different sample rate and/or block size.
// 
// @par Factory functions:
// - ar (input, rate=1, preferredBlockSize=default, preferredSampleRate=default)
// - kr (input, rate=1)
// 
// @par Inputs:
// - input: (unit, multi) the unit to resample
// - rate: (unit, multi) a modulatable rate multiplier (e.g., for varispeed playback)
// - preferredBlockSize: the preferred output block size
// - preferredSampleRate: the preferred output sample rate
// 
// @ingroup ConverterUnits */
//template<class SampleType, Interp::TypeCode InterpTypeCode>
//class ResampleUnit
//{
//public:
//    typedef ResampleChannelInternal<SampleType,InterpTypeCode>      ResampleInternal;
//    typedef typename ResampleInternal::Data                         Data;
//    typedef ChannelBase<SampleType>                                 ChannelType;
//    typedef ChannelInternal<SampleType,Data>                        Internal;
//    typedef UnitBase<SampleType>                                    UnitType;
//    typedef InputDictionary                                         Inputs;
//    
//    typedef typename ResampleInternal::RateType         RateType;
//    typedef typename ResampleInternal::RateUnitType     RateUnitType;
//    typedef typename ResampleInternal::RateBufferType   RateBufferType;
//    
//    static inline UnitInfos getInfo() throw()
//    {
//        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();
//        
//        return UnitInfo ("Resample", "Resamples signals in terms of block size and/or sample rate.",
//                         
//                         // output
//                         ChannelCount::VariableChannelCount,
//                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,      IOKey::End,
//                         
//                         // inputs
//                         IOKey::Generic,    Measure::None,     IOInfo::NoDefault,   IOLimit::None,
//                         IOKey::Rate,       Measure::Factor,   1.0,                 IOLimit::Minimum,   Measure::Factor,    0.0,
//                         IOKey::BlockSize,  Measure::Samples,  blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
//                         IOKey::SampleRate, Measure::Hertz,    sampleRate,          IOLimit::Minimum,   Measure::Hertz,     0.0,
//                         IOKey::End);
//    }
//    
//    /** Create an audio rate sample rate converter. */
//    static UnitType ar (UnitType const& input,
//                        RateUnitType const& rate = Math<RateUnitType>::get1(),
//                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
//                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
//    {
//        plonk_assert (preferredSampleRate.getValue() > 0.0); // no need to resample a DC signal
//        
//        Inputs inputs;
//        inputs.put (IOKey::Generic, input);
//        inputs.put (IOKey::Rate, rate);
//        
//        Data data = { -1.0, -1.0 };
//        
//        return UnitType::template createFromInputs<ResampleInternal> (inputs,
//                                                                      data,
//                                                                      preferredBlockSize,
//                                                                      preferredSampleRate);
//    }
//    
//    static inline UnitType kr (UnitType const& input,
//                               RateUnitType const& rate = Math<RateUnitType>::get1()) throw()
//    {
//        return ar (input,
//                   rate,
//                   BlockSize::getControlRateBlockSize(), 
//                   SampleRate::getControlRate());
//    }
//    
//};


typedef ResampleUnit<PLONK_TYPE_DEFAULT,Interp::Linear>     ResampleLinear;
typedef ResampleUnit<PLONK_TYPE_DEFAULT,Interp::Lagrange3>  ResampleLagrange3;


#endif // PLONK_RESAMPLECHANNEL_H


