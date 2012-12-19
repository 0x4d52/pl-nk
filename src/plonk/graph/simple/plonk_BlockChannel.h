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

#ifndef PLONK_BLOCKCHANNEL_H
#define PLONK_BLOCKCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Block channel. 
 Copies a buffer to a output block. */
template<class SampleType>
class BlockChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef BlockChannelInternal<SampleType>                    BlockInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    
    BlockChannelInternal (Inputs const& inputs, 
                          Data const& data, 
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "Block";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Buffer);
        return keys;
    }    
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }        
    
    void initChannel (const int /*channel*/) throw()
    {
        const Buffer& buffer = this->getInputAsBuffer (IOKey::Buffer);
        
        if (buffer.length() == BlockSize::getDefault().getValue())
            this->setBlockSize (BlockSize::getDefault());
        else
            this->setBlockSize (buffer.length());
        
        this->initValue (SampleType (0));
        this->copyBlock();        
    }    
    
    inline void copyBlock() throw()
    {
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const Buffer& buffer = this->getInputAsBuffer (IOKey::Buffer);
        const SampleType* const bufferSamples = buffer.getArray();
        
        plonk_assert (outputBufferLength == buffer.length());
        
        Buffer::copyData (outputSamples, bufferSamples, outputBufferLength);
    }
        
    void process (ProcessInfo& /*info*/, const int /*channel*/) throw()
    {                
        this->copyBlock(); // can avoid doing this every time...
    }

private:
};



//------------------------------------------------------------------------------

/** Copies data from a buffer to an output block. 
 Can be used to generate windowing functions for example. 
 
 @par Factory functions:
 - ar (buffer, overlap=1, preferredBlockSize=default, preferredSampleRate=default)
 - ar (buffers, overlap=1, preferredBlockSize=default, preferredSampleRate=default)
 - kr (buffer, overlap=1)
 
 @par Inputs:
 - buffer: (buffer) the buffer to use
 - buffers: (buffers) an array of buffers to use
 - overlap: (doublevariable) the overlap to use 1= no overlap, 0.5= blocks overlap by half their length
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @ingroup ControlUnits FFTUnits */
template<class SampleType>
class BlockUnit
{
public:    
    typedef BlockChannelInternal<SampleType>        BlockInternal;
    typedef typename BlockInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternaBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef NumericalArray<SampleType>              Buffer;
    typedef NumericalArray2D<SampleType>            BufferArray;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Block", "Copies data from a buffer to an output block. Useful for windowing functions.",
                         
                         // output
                         1, 
                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::Buffer,         Measure::None,
                         IOKey::OverlapMake,    Measure::Factor,    0.5,        IOLimit::Clipped, Measure::NormalisedUnipolar,  0.0, 1.0,
                         IOKey::BlockSize,      Measure::Samples,   blockSize,  IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::SampleRate,     Measure::Hertz,     sampleRate, IOLimit::Minimum, Measure::Hertz,               0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate block generator. */
    static inline UnitType ar (Buffer const& buffer,
                               DoubleVariable const& overlap = Math<DoubleVariable>::get1(),
                               BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                               SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::Buffer, buffer);
        inputs.put (IOKey::OverlapMake, overlap);
        
        Data data = { -1.0, -1.0 };
        
        return UnitType::template createFromInputs<BlockInternal> (inputs, 
                                                                   data, 
                                                                   preferredBlockSize, 
                                                                   preferredSampleRate);
    }
    
    /** Create an audio rate block generator. */
    static inline UnitType ar (BufferArray const& buffers,
                               DoubleVariable const& overlap = Math<DoubleVariable>::get1(),
                               BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                               SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        const int numChannels = buffers.length();
        UnitType result (UnitType::emptyWithAllocatedSize (numChannels));
        Data data = { -1.0, -1.0 };
        
        for (int i = 0; i < numChannels; ++i)
        {
            Inputs inputs;
            inputs.put (IOKey::Buffer, buffers[i]);
            inputs.put (IOKey::OverlapMake, overlap);
            
            result.add (UnitType::template createFromInputs<BlockInternal> (inputs,
                                                                            data,
                                                                            preferredBlockSize,
                                                                            preferredSampleRate));
        }
        
        return result;
    }
    
    static inline UnitType kr (Buffer const& buffer,                                
                               DoubleVariable const& overlap = Math<DoubleVariable>::get1()) throw()
    {
        return ar (buffer, 
                   overlap,
                   BlockSize::noPreference(), // ??
                   SampleRate::getControlRate());
    }
      
};

typedef BlockUnit<PLONK_TYPE_DEFAULT> Block;




#endif // PLONK_BLOCKCHANNEL_H


