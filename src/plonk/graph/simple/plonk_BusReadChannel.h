/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-12
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

#ifndef PLONK_BUSREADCHANNEL_H
#define PLONK_BUSREADCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Bus read channel. 
 Reads data from a bus. */
template<class SampleType>
class BusReadChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef BusReadChannelInternal<SampleType>                  BusReadInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef BusBuffer<SampleType>                               Bus;
    typedef PLONK_BUSARRAYBASETYPE<Bus>                         Busses;
    
    BusReadChannelInternal (Inputs const& inputs, 
                            Data const& data, 
                            BlockSize const& blockSize,
                            SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate),
        latency (0.0)
    {
    }
    
    Text getName() const throw()
    {
        const Busses& busses (this->getInputAsBusses (IOKey::Busses));
        const int numBusses = busses.length();

        Text name = "Bus Read (";
            
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
        const IntArray keys (IOKey::Busses);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new BusReadInternal (channelInputs, 
                                    this->getState(), 
                                    this->getBlockSize(),
                                    this->getSampleRate());
    }        
    
    void initChannel (const int channel) throw()
    {
        const Busses& busses (this->getInputAsBusses (IOKey::Busses));
        this->setSampleRate (busses.wrapAt (channel).getSampleRate());
        this->initValue (SampleType (0));
    }    
    
    double getLatency() const throw()
    {
        return latency;
    }
    
    void process (ProcessInfo& info, const int channel) throw()
    {                
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        Busses& busses (this->getInputAsBusses (IOKey::Busses));
        Bus& bus = busses.wrapAt (channel);
        
        plonk_assert (bus.getSampleRate() == this->getSampleRate());
        
        bus.read (nextValidReadTime, 
                  outputBufferLength, 
                  outputSamples);
        
        latency = (info.getTimeStamp() - nextValidReadTime).getValue();
    }
    

private:
    TimeStamp nextValidReadTime;
    double latency;
};



//------------------------------------------------------------------------------

/** Reads data from a Bus. 
 @ingroup MiscUnits */
template<class SampleType>
class BusReadUnit
{
public:    
    typedef BusReadChannelInternal<SampleType>      BusReadInternal;
    typedef typename BusReadInternal::Data          Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternaBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    typedef BusBuffer<SampleType>                   Bus;
    typedef PLONK_BUSARRAYBASETYPE<Bus>             Busses;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();

        return UnitInfo ("BusRead", "Reads samples from a bus.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,      IOInfo::NoDefault,   IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::Busses,     Measure::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,           IOLimit::Minimum, Measure::Samples, 1.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate bus reader. */
    static UnitType ar (Busses const& busses,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::Busses, busses);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<BusReadInternal> (inputs, 
                                                                     data, 
                                                                     preferredBlockSize, 
                                                                     SampleRate::noPreference());
    }
        
    static inline UnitType kr (Busses const& busses) throw()
    {
        return ar (busses, BlockSize::getControlRateBlockSize());
    }
      
};

typedef BusReadUnit<PLONK_TYPE_DEFAULT> BusRead;




#endif // PLONK_BUSREADCHANNEL_H


