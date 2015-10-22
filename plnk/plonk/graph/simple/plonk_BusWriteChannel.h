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

#ifndef PLONK_BUSWRITECHANNEL_H
#define PLONK_BUSWRITECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


/** Bus write channel. 
 Writes data to a bus. */
template<class SampleType>
class BusWriteChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef BusWriteChannelInternal<SampleType>                 BusWriteInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef BusBuffer<SampleType>                               Bus;
    typedef PLONK_BUSARRAYBASETYPE<Bus>                         Busses;
        
    BusWriteChannelInternal (Inputs const& inputs, 
                             Data const& data, 
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {
        const Busses& busses (this->getInputAsBusses (IOKey::Busses));
        const int numBusses = busses.length();
        
        Text name = "Bus Write (";
        
        for (int i = 0; i < numBusses; ++i)
        {
            name += busses.atUnchecked (i).getName();
            
            if (i < (numBusses - 1))
                name += ", ";
        }
        
        name += ")";
        
        return name;
    }   
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic,
                       IOKey::Busses);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new BusWriteInternal (channelInputs, 
                                     this->getState(), 
                                     this->getBlockSize(),
                                     this->getSampleRate());
    }    
    
    void initChannel (const int channel) throw()
    {
        const Busses& busses (this->getInputAsBusses (IOKey::Busses));
        this->setSampleRate (busses.wrapAt (channel).getSampleRate());

        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel),
                                               this->getBlockSize()));
        
        plonk_assert (input.getOverlap (channel) == Math<DoubleVariable>::get1());
        
        this->initValue (SampleType (0));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {        
        int i;
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();

        Busses& busses (this->getInputAsBusses (IOKey::Busses));
        Bus& bus = busses.wrapAt (channel);

        plonk_assert (bus.getSampleRate() == this->getSampleRate());

        UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
        
        const Buffer& inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();
        
        if (inputBufferLength == outputBufferLength)
        {
            Buffer::copyData (outputSamples, inputSamples, outputBufferLength);
        }
        else
        {
            double inputPosition = 0.0;
            double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = inputSamples[int (inputPosition)];
                inputPosition += inputIncrement;
            }        
        }
        
        bus.write (info.getTimeStamp(), outputBufferLength, outputSamples);
    }

private:
};

//------------------------------------------------------------------------------

/** Write data to a Bus. 
 @ingroup MiscUnits */
template<class SampleType>
class BusWriteUnit
{
public:    
    typedef BusWriteChannelInternal<SampleType>     BusWriteInternal;
    typedef typename BusWriteInternal::Data         Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternaBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    typedef BusBuffer<SampleType>                   Bus;
    typedef PLONK_BUSARRAYBASETYPE<Bus>             Busses;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();

        return UnitInfo ("BusWrite", "Writes samples to a bus (and copies this to its output).",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None,      IOKey::End,
                         
                         // inputs
                         IOKey::Busses,     Measure::None,
                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::End);
    }    
    
    
    /** Create an audio rate bus writer. */
    static UnitType ar (Busses const& busses,
                        UnitType const& input,
                        BlockSize const& preferredBlockSize = BlockSize::noPreference()) throw()
    {                
        Inputs inputs;
        inputs.put (IOKey::Generic, input);   
        inputs.put (IOKey::Busses, busses);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<BusWriteInternal> (inputs, 
                                                                      data, 
                                                                      preferredBlockSize, 
                                                                      SampleRate::noPreference());
    }
        
    static PLONK_INLINE_LOW UnitType kr (Busses const& busses,
                               UnitType const& input) throw()
    {
        return ar (busses, input, BlockSize::getControlRateBlockSize());
    }    
    
};

typedef BusWriteUnit<PLONK_TYPE_DEFAULT> BusWrite;




#endif // PLONK_BUSWRITECHANNEL_H


