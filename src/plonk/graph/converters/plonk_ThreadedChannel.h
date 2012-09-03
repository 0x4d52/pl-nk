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

#ifndef PLONK_THREADEDCHANNEL_H
#define PLONK_THREADEDCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class ThreadedChannelInternal;

PLONK_CHANNELDATA_DECLARE(FilePlayChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    int numChannels;
};      

//------------------------------------------------------------------------------

/** File player generator. */
template<class SampleType>
class ThreadedChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(ThreadedChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(ThreadedChannelInternal,SampleType)  Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef ObjectArray<ChannelType>                                    ChannelArrayType;
    typedef ThreadedChannelInternal<SampleType>                         ThreadedInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                  Internal;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;

    ThreadedChannelInternal (Inputs const& inputs, 
                             Data const& data, 
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate,
                             ChannelArrayType& channels) throw()
    :   Internal (1, 
                  inputs, data, blockSize, sampleRate,
                  channels)
    {
    }
            
    Text getName() const throw()
    {
        return "Threaded";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }    
        
    void initChannel (const int channel) throw()
    {       
        const UnitType& input = this->getInputAsUnit (IOKey::Generic);
        
        if ((channel % this->getNumChannels()) == 0)
        {
            // use sample rate and block size of the input unless another 
            // preference has been indicated on construction
            this->setBlockSize (BlockSize::decide (input.getBlockSize (0),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (input.getSampleRate (0),
                                                     this->getSampleRate()));      
        }

        this->initValue (0);
    }    
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {        
        //
    }
    
private:
    // thread..!
};

//------------------------------------------------------------------------------

/** Defer a unit's processing to a separate. 
  
 Factory functions:
 - ar (input)
 
 Inputs:
 - input: (input, multi) the input unit to defer to a separate thread

  @ingroup GeneratorUnits */
template<class SampleType>
class ThreadedUnit
{
public:    
    typedef ThreadedChannelInternal<SampleType>         ThreadedInternal;
    typedef typename ThreadedInternal::Data             Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;
        
//    static inline UnitInfos getInfo() throw()
//    {
//        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();
//        
//        return UnitInfo ("FilePlay", "A signal player generator.",
//                         
//                         // output
//                         ChannelCount::VariableChannelCount, 
//                         IOKey::Generic,            Measure::None,      0.0,        IOLimit::None,
//                         IOKey::End,
//                         
//                         // inputs
//                         IOKey::AudioFileReader,    Measure::None,
//                         IOKey::Multiply,           Measure::Factor,    1.0,        IOLimit::None,
//                         IOKey::Add,                Measure::None,      0.0,        IOLimit::None,
//                         IOKey::BlockSize,          Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
//                         IOKey::SampleRate,         Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
//                         IOKey::End);
//    }
    
    static UnitType ar (UnitType const& input) throw()
    {             
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
                        
        Data data = { { -1.0, -1.0 }, 0 };
        
        return UnitType::template proxiesFromInputs<ThreadedInternal> (inputs, 
                                                                       data, 
                                                                       preferredBlockSize, 
                                                                       preferredSampleRate);
    }
};

typedef ThreadedUnit<PLONK_TYPE_DEFAULT> Threaded;


#endif // PLONK_THREADEDCHANNEL_H

