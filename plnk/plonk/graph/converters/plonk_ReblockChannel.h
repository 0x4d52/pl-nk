/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_REBLOCKCHANNEL_H
#define PLONK_REBLOCKCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Reblock. */
template<class SampleType>
class ReblockChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                               Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ReblockChannelInternal<SampleType>                      ReblockInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    
    ReblockChannelInternal (Inputs const& inputs, 
                            Data const& data,
                            BlockSize const& blockSize,
                            SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        return "Reblock";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic, IOKey::Busses);
        return keys;
    }
    
    InternalBase* getChannel (const int index) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        const Busses& busses = this->getInputAsBusses (IOKey::Busses);

        Inputs channelInputs;
        channelInputs.put (IOKey::Generic, input[index]);
        channelInputs.put (IOKey::Busses, busses); // must be all of them in this case

        return new ReblockChannelInternal (channelInputs, 
                                           this->getState(),
                                           this->getBlockSize(),
                                           this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);        
        plonk_assert (input.getOverlap (channel) == Math<DoubleVariable>::get1());
        this->setSampleRate (input.getSampleRate (channel));        
        this->initValue (input.getValue (channel));        
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        Busses& busses = this->getInputAsBusses (IOKey::Busses);
        
        plonk_assert (busses.length() == inputUnit.getNumChannels());
        
        const int outputBufferLength = this->getOutputBuffer().length();
        SampleType* const outputSamples = this->getOutputSamples();
        
        TimeStamp infoTimeStamp = info.getTimeStamp();
        const TimeStamp latestTimeNeeded = infoTimeStamp + this->getSampleDurationInTicks() * outputBufferLength;

        while (busses.atUnchecked (0).getLatestValidTime() <= latestTimeNeeded)
        {
            const TimeStamp& thisTimeStamp = inputUnit.wrapAt (0).getNextTimeStamp();
            info.setTimeStamp (thisTimeStamp);

            for (int i = 0; i < busses.length(); ++i)
            {
                const Buffer& inputBuffer (inputUnit.process (info, i));
                const SampleType* const inputSamples = inputBuffer.getArray();
                const int inputBufferLength = inputBuffer.length();

                busses.atUnchecked (i).write (thisTimeStamp, inputBufferLength, inputSamples);
            }
        }
        
        info.setTimeStamp (infoTimeStamp); // ensure it is reset for parent graph
        
        // output one channel..
        busses.atUnchecked (channel).read (nextValidReadTime, outputBufferLength, outputSamples);
    }
    
private:
    TimeStamp nextValidReadTime;
};

//------------------------------------------------------------------------------

/** Re-buffer to a different block size. 
 
 @par Factory functions:
 - ar (input, preferredBlockSize=default)
 
 @par Inputs:
 - input: (unit, multi) the unit to re-buffer
 - preferredBlockSize: the preferred output block size

 
 @ingroup ConverterUnits */
template<class SampleType>
class ReblockUnit
{
public:    
    typedef ReblockChannelInternal<SampleType>          ReblockInternal;
    typedef typename ReblockInternal::Data              Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;    
    typedef NumericalArray<SampleType>                  Buffer;
    typedef BusBuffer<SampleType>                       Bus;
    typedef PLONK_BUSARRAYBASETYPE<Bus>                 Busses;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();

        return UnitInfo ("Reblock", "Re-buffer to a different block size.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,         Measure::None,      IOInfo::NoDefault,   IOLimit::None,      IOKey::End,
                         
                         // inputs
                         IOKey::Generic,         Measure::None,      IOInfo::NoDefault,   IOLimit::None,
                         IOKey::BlockSize,       Measure::Samples,   blockSize,           IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::End);
    }    
    
    /**  */
    static UnitType ar (UnitType const& input,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault()) throw()
    {
        plonk_assert (input.channelsHaveSameBlockSize());
        plonk_assert (input.channelsHaveSameSampleRate());
        plonk_assert (input.channelsHaveSameOverlap());
        
        const int numChannels = input.getNumChannels();
        Busses busses (Busses::emptyWithAllocatedSize (numChannels));
        
        BlockSize inputBlockSize = input.getBlockSize (0);
        BlockSize bufferSize = (preferredBlockSize.max (inputBlockSize) * 2).getValue();
        SampleRate sampleRate = input.getSampleRate (0);
        
        for (int i = 0; i < numChannels; ++i)
            busses.add (Bus (bufferSize, inputBlockSize, sampleRate));
        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Busses, busses);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<ReblockInternal> (inputs, 
                                                                     data,
                                                                     preferredBlockSize,
                                                                     SampleRate::noPreference());
    }
};

typedef ReblockUnit<PLONK_TYPE_DEFAULT> Reblock;


#endif // PLONK_REBLOCKCHANNEL_H
