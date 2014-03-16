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

#ifndef PLONK_PATCHCHANNEL_H
#define PLONK_PATCHCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"
#include "plonk_Mixers.h"

template<class SampleType> class PatchChannelInternal;

PLONK_CHANNELDATA_DECLARE(PatchChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    int preferredNumChannels;
    SampleType fadeIncrement;
    SampleType fadeLevel;
    bool allowAutoDelete:1;
};      


/** Patch channel.
 Safe repatching of signals. */
template<class SampleType>
class PatchChannelInternal 
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(PatchChannelInternal,SampleType)> 
{
public:
    typedef PLONK_CHANNELDATA_NAME(PatchChannelInternal,SampleType)     Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef ObjectArray<ChannelType>                                    ChannelArrayType;
    typedef PatchChannelInternal<SampleType>                            PatchChannelInternalType;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                  Internal;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
    typedef Variable<UnitType&>                                         UnitVariableType;
    
    typedef typename TypeUtility<SampleType>::IndexType                 DurationType;
    typedef UnitBase<DurationType>                                      DurationUnitType;
    typedef NumericalArray<DurationType>                                DurationBufferType;

    
    PatchChannelInternal (Inputs const& inputs, 
                          Data const& data, 
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate,
                          ChannelArrayType& channels) throw()
    :   Internal (data.preferredNumChannels > 0 ? data.preferredNumChannels : numChannelsInSource (inputs), 
                  inputs, data, blockSize, sampleRate, channels)
    {
    }
        
    Text getName() const throw()
    {        
        return "Patch";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::UnitVariable);
        return keys;
    }    
        
    void initChannel (const int channel) throw()
    {
        if ((channel % this->getNumChannels()) == 0)
        {
            UnitVariableType& var = ChannelInternalCore::getInputAs<UnitVariableType> (IOKey::UnitVariable);
            
            if (var.getValue() != getDummy())
            {
                currentSource = getDummy();
                var.swapValues (currentSource);
            }
        }
        
        this->initProxyValue (channel, currentSource.getValue (channel)); 
    }    
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {       
        Data& data = this->getState();
        updateSources (info);
                
        if (data.fadeLevel > Math<SampleType>::get0())
            processFade (info);
        else
            processCopy (info);
    }
    
    void processFade (ProcessInfo& info)
    {
        Data& data = this->getState();
        const int numChannels = this->getNumChannels();

        const SampleType fadeIncrement = data.fadeIncrement;
        const SampleType fadeMax = TypeUtility<SampleType>::getTypePeak();
        const SampleType fadeMin = Math<SampleType>::get0();
        SampleType fadeOutLevel, fadeInLevel;

        for (int channel = 0; channel < numChannels; ++channel)
        {
            fadeOutLevel = data.fadeLevel;
            fadeInLevel = fadeMax - fadeOutLevel;

            Buffer& outputBuffer = this->getOutputBuffer (channel);
            SampleType* const outputSamples = outputBuffer.getArray();
            const int outputBufferLength = outputBuffer.length();        
            
            const Buffer& fadeInSourceBuffer (currentSource.process (info, channel));
            const SampleType* const fadeInSourceSamples = fadeInSourceBuffer.getArray();
            const int fadeInSourceBufferLength = fadeInSourceBuffer.length();
            
            const Buffer& fadeOutSourceBuffer (fadeSource.process (info, channel));
            const SampleType* const fadeOutSourceSamples = fadeOutSourceBuffer.getArray();
            const int fadeOutSourceBufferLength = fadeOutSourceBuffer.length();

            int i;
            
            if ((fadeInSourceBufferLength == outputBufferLength) &&
                (fadeInSourceBufferLength) == fadeOutSourceBufferLength)
            {
                for (i = 0; i < outputBufferLength; ++i)
                {
                    outputSamples[i] = fadeInSourceSamples[i] * fadeInLevel + fadeOutSourceSamples[i] * fadeOutLevel;
                    fadeInLevel = plonk::min (fadeInLevel + fadeIncrement, fadeMax);
                    fadeOutLevel = plonk::max (fadeOutLevel - fadeIncrement, fadeMin);
                }
            }
            else
            {
                double fadeInSourcePosition = 0.0;
                const double fadeInSourceIncrement = double (fadeInSourceBufferLength) / double (outputBufferLength);
                double fadeOutSourcePosition = 0.0;
                const double fadeOutSourceIncrement = double (fadeOutSourceBufferLength) / double (outputBufferLength);
                
                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] = fadeInSourceSamples[int (fadeInSourcePosition)] * fadeInLevel + 
                                       fadeOutSourceSamples[int (fadeOutSourcePosition)] * fadeOutLevel;

                    fadeInLevel = plonk::min (fadeInLevel + fadeIncrement, fadeMax);
                    fadeOutLevel = plonk::max (fadeOutLevel - fadeIncrement, fadeMin);

                    fadeInSourcePosition += fadeInSourceIncrement;
                    fadeOutSourcePosition += fadeOutSourceIncrement;
                }        
            }
            
            if (data.allowAutoDelete == false)
                info.resetShouldDelete();
        }
        
        data.fadeLevel = fadeOutLevel;
        
        if (fadeOutLevel <= fadeMin)
        {
            this->update (Text::getMessagePatchEnd(), fadeSource);
            fadeSource = UnitType::getNull();
        }
    }
    
    void processCopy (ProcessInfo& info)
    {
        Data& data = this->getState();
        const int numChannels = this->getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            Buffer& outputBuffer = this->getOutputBuffer (channel);
            SampleType* const outputSamples = outputBuffer.getArray();
            const int outputBufferLength = outputBuffer.length();        
            
            const Buffer& sourceBuffer (currentSource.process (info, channel));
            const SampleType* const sourceSamples = sourceBuffer.getArray();
            const int sourceBufferLength = sourceBuffer.length();
            
            int i;
            
            if (sourceBufferLength == outputBufferLength)
            {
                Buffer::copyData (outputSamples, sourceSamples, outputBufferLength);
            }
            else if (sourceBufferLength == 1)
            {
                NumericalArrayFiller<SampleType>::fill (outputSamples, sourceSamples[0], outputBufferLength);
            }
            else
            {
                double sourcePosition = 0.0;
                const double sourceIncrement = double (sourceBufferLength) / double (outputBufferLength);
                
                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] = sourceSamples[int (sourcePosition)];
                    sourcePosition += sourceIncrement;
                }        
            }
            
            if (data.allowAutoDelete == false)
                info.resetShouldDelete();
        }
    }

private:
    UnitType currentSource;
    UnitType fadeSource;
    
    static inline int numChannelsInSource (Inputs const& inputs) throw()
    {
        const UnitVariableType& var = inputs[IOKey::UnitVariable].asUnchecked<UnitVariableType>();
        return var.getValue().getNumChannels();
    }
    
    static const UnitType& getDummy() throw()
    {
        static UnitType dummy (0); // zero but a unique object for the Patch unit
        return dummy;
    }
    
    inline void updateSources (ProcessInfo& info) throw()
    {
        UnitVariableType& var = ChannelInternalCore::getInputAs<UnitVariableType> (IOKey::UnitVariable);
        
        if (var.getValue() != getDummy())
        {
            fadeSource = currentSource;
            currentSource = getDummy();
            var.swapValues (currentSource);
            
            this->update (Text::getMessagePatchStart(), currentSource);
            
            Data& data = this->getState();

            DurationUnitType& durationUnit = ChannelInternalCore::getInputAs<DurationUnitType> (IOKey::Duration);
            plonk_assert (durationUnit.getNumChannels() == 1);
            const DurationBufferType& durationBuffer (durationUnit.process (info, 0));
            const DurationType duration = durationBuffer.atUnchecked (0);
            
            if (duration > Math<DurationType>::get0())
            {
                const int fadeSamplesRemaining = int (duration * data.base.sampleRate + 0.5);
                data.fadeIncrement = TypeUtility<SampleType>::getTypePeak() / SampleType (fadeSamplesRemaining);
                data.fadeLevel = TypeUtility<SampleType>::getTypePeak();
            }
        }
    }
};


//------------------------------------------------------------------------------

/** Patch channel.
 Safe repatching of signals. 
 
 @par Factory functions:
 - ar (initialSource, allowAutoDelete=true, preferredNumChannels=0, fadeDuration=0.1, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 
 @par Inputs:
 - initialSource: (unitvariable, multi)
 - allowAutoDelete: (bool) whether this unit can be caused to be deleted by the unit it contains
 - preferredNumChannels: (int) force this unit to have a certain number of channels (0= the channel count in initialSource)
 - fadeDuration: (unit, mono) the crossfade time in seconds to be used when patch sources are changed
 - mul: (unit) the multiplier applied to the output (NB NOT multi)
 - add: (unit) the offset added to the output (NB NOT multi)
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @ingroup ControlUnits */
template<class SampleType>
class PatchUnit
{
public:    
    typedef PatchChannelInternal<SampleType>                        PatchChannelInternalType;
    typedef typename PatchChannelInternalType::Data                 Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternaBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef Variable<UnitType&>                                     UnitVariableType;

    typedef typename PatchChannelInternalType::DurationType         DurationType;
    typedef typename PatchChannelInternalType::DurationUnitType     DurationUnitType;
    typedef typename PatchChannelInternalType::DurationBufferType   DurationBufferType;

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Patch", "Threadsafe repatching of signals.",
                         
                         // output
                         1, 
                         IOKey::Generic,            Measure::None,      IOInfo::NoDefault,      IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::UnitVariable,           Measure::None,      IOInfo::NoDefault,  IOLimit::None, 
                         IOKey::AutoDeleteFlag,         Measure::Bool,      IOInfo::True,       IOLimit::None,
                         IOKey::PreferredNumChannels,   Measure::Count,     0.0,                IOLimit::None,
                         IOKey::Duration,               Measure::Seconds,   0.1,                IOLimit::Minimum, Measure::Seconds,             0.0,
                         IOKey::BlockSize,              Measure::Samples,   blockSize,          IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::SampleRate,             Measure::Hertz,     sampleRate,         IOLimit::Minimum, Measure::Hertz,               0.0,
                         IOKey::End);
    }    
    
    /** Create control rate variable. */
    static UnitType ar (UnitVariableType const& initialSource,
                        const bool allowAutoDelete = true,
                        const int preferredNumChannels = 0, 
                        DurationUnitType const& fadeDuration = DurationType (0.1),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::Unknown, UnitType::getNull()); // ensure getNull() is called off the audio thread for the first time
        inputs.put (IOKey::UnitVariable, initialSource);
        inputs.put (IOKey::Duration, fadeDuration);
        
        Data data = { { -1.0, -1.0 }, preferredNumChannels, 0, 0, allowAutoDelete };
            
        return UnitType::template proxiesFromInputs<PatchChannelInternalType> (inputs, 
                                                                               data, 
                                                                               preferredBlockSize, 
                                                                               preferredSampleRate);
    }   
};

typedef PatchUnit<PLONK_TYPE_DEFAULT> Patch;


#endif // PLONK_PATCHCHANNEL_H


