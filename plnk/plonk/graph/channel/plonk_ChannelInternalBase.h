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

#ifndef PLONK_CHANNELINTERNALBASE_H
#define PLONK_CHANNELINTERNALBASE_H

#include "../plonk_GraphForwardDeclarations.h"
#include "plonk_ChannelInternalCore.h"

/** The templated base class for all internal channel processes. 
 Inherits from ChannelInternalCore. This is a template and stores a 
 specific sample type specified by the template parameter. */
template<class SampleType>
class ChannelInternalBase : public SenderInternal<ChannelBase<SampleType> >,
                            public ChannelInternalCore,
                            public IntVariable::Receiver
{
public:    
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef UnitBase<SampleType>                    UnitType;
    typedef NumericalArray2D<ChannelType,UnitType>  UnitsType;
    typedef BusBuffer<SampleType>                   BusType;
    typedef PLONK_BUSARRAYBASETYPE<BusType>         BussesType;

    typedef NumericalArray<SampleType>              Buffer;
    typedef InputDictionary                         Inputs;    
    typedef ChannelBase<SampleType>                 Container;
    typedef Variable<SampleType>                    VariableType;
    typedef BreakpointsBase<SampleType>             BreakpointsType;
    typedef WavetableBase<SampleType>               WavetableType;
    typedef SignalBase<SampleType>                  SignalType;
    typedef LockFreeQueue<UnitType>                 QueueType;
    typedef QueueBufferBase<SampleType>             QueueBufferType;
    typedef LockFreeQueue<QueueBufferType>          BufferQueueType;
    typedef Variable<FFTBuffersBase<SampleType>&>   FFTBuffersVariableType;

    ChannelInternalBase (Inputs const& inputDictionary, 
                         BlockSize const& blockSize, 
                         SampleRate const& sampleRate) throw()
    :   ChannelInternalCore (inputDictionary, blockSize, sampleRate),
        outputBuffer (Buffer::newClear (blockSize.getValue())),
        usingExternalBuffer (false)
#ifdef PLONK_DEBUG
    ,   hasInitialisedValue (false)
#endif
    {
        this->getBlockSize().addReceiver (this);
    }
    
    ~ChannelInternalBase()
    {
        this->getBlockSize().removeReceiver (this);
    }
            
    PLONK_INLINE_LOW const UnitType& getInputAsUnit (const int key) const throw()                     { return this->template getInputAs<UnitType> (key); }
    PLONK_INLINE_LOW const UnitsType& getInputAsUnits (const int key) const throw()                   { return this->template getInputAs<UnitsType> (key); }
    PLONK_INLINE_LOW const BusType& getInputAsBus (const int key) const throw()                       { return this->template getInputAs<BusType> (key); }
    PLONK_INLINE_LOW const BussesType& getInputAsBusses (const int key) const throw()                 { return this->template getInputAs<BussesType> (key); }
    PLONK_INLINE_LOW const Buffer& getInputAsBuffer (const int key) const throw()                     { return this->template getInputAs<Buffer> (key); }
    PLONK_INLINE_LOW const VariableType& getInputAsVariable (const int key) const throw()             { return this->template getInputAs<VariableType> (key); }
    PLONK_INLINE_LOW const BreakpointsType& getInputAsBreakpoints (const int key) const throw()       { return this->template getInputAs<BreakpointsType> (key); }
    PLONK_INLINE_LOW const WavetableType& getInputAsWavetable (const int key) const throw()           { return this->template getInputAs<WavetableType> (key); }
    PLONK_INLINE_LOW const SignalType& getInputAsSignal (const int key) const throw()                 { return this->template getInputAs<SignalType> (key); }
    PLONK_INLINE_LOW const SampleRate& getInputAsSampleRate (const int key) const throw()             { return this->template getInputAs<SampleRate> (key); }
    PLONK_INLINE_LOW const BlockSize& getInputAsBlockSize (const int key) const throw()               { return this->template getInputAs<BlockSize> (key); }
    PLONK_INLINE_LOW const AudioFileReader& getInputAsAudioFileReader (const int key) const throw()   { return this->template getInputAs<AudioFileReader> (key); }
    PLONK_INLINE_LOW const QueueType& getInputAsUnitQueue (const int key) const throw()               { return this->template getInputAs<QueueType> (key); }
    PLONK_INLINE_LOW const BufferQueueType& getInputAsBufferQueue (const int key) const throw()       { return this->template getInputAs<BufferQueueType> (key); }
    PLONK_INLINE_LOW const FFTBuffersVariableType& getInputAsFFTBuffers (const int key) const throw() { return this->template getInputAs<FFTBuffersVariableType> (key); }

    PLONK_INLINE_LOW UnitType& getInputAsUnit (const int key) throw()                                 { return this->template getInputAs<UnitType> (key); }
    PLONK_INLINE_LOW UnitsType& getInputAsUnits (const int key) throw()                               { return this->template getInputAs<UnitsType> (key); }
    PLONK_INLINE_LOW BusType& getInputAsBus (const int key) throw()                                   { return this->template getInputAs<BusType> (key); }
    PLONK_INLINE_LOW BussesType& getInputAsBusses (const int key) throw()                             { return this->template getInputAs<BussesType> (key); }
    PLONK_INLINE_LOW Buffer& getInputAsBuffer (const int key) throw()                                 { return this->template getInputAs<Buffer> (key); }
    PLONK_INLINE_LOW VariableType& getInputAsVariable (const int key) throw()                         { return this->template getInputAs<VariableType> (key); }
    PLONK_INLINE_LOW BreakpointsType& getInputAsBreakpoints (const int key) throw()                   { return this->template getInputAs<BreakpointsType> (key); }
    PLONK_INLINE_LOW WavetableType& getInputAsWavetable (const int key) throw()                       { return this->template getInputAs<WavetableType> (key); }
    PLONK_INLINE_LOW SignalType& getInputAsSignal (const int key) throw()                             { return this->template getInputAs<SignalType> (key); }
    PLONK_INLINE_LOW SampleRate& getInputAsSampleRate (const int key) throw()                         { return this->template getInputAs<SampleRate> (key); }
    PLONK_INLINE_LOW BlockSize& getInputAsBlockSize (const int key) throw()                           { return this->template getInputAs<BlockSize> (key); }
    PLONK_INLINE_LOW AudioFileReader& getInputAsAudioFileReader (const int key) throw()               { return this->template getInputAs<AudioFileReader> (key); }
    PLONK_INLINE_LOW QueueType& getInputAsUnitQueue (const int key) throw()                           { return this->template getInputAs<QueueType> (key); }
    PLONK_INLINE_LOW BufferQueueType& getInputAsBufferQueue (const int key) throw()                   { return this->template getInputAs<BufferQueueType> (key); }
    PLONK_INLINE_LOW FFTBuffersVariableType& getInputAsFFTBuffers (const int key) throw()             { return this->template getInputAs<FFTBuffersVariableType> (key); }

    PLONK_INLINE_LOW const Buffer& getOutputBuffer() const throw()                                    { return outputBuffer; }
    PLONK_INLINE_LOW Buffer& getOutputBuffer() throw()                                                { return outputBuffer; }
    PLONK_INLINE_LOW const SampleType* getOutputSamples() const throw()                               { return outputBuffer.getArray(); }
    PLONK_INLINE_LOW SampleType* getOutputSamples() throw()                                           { return outputBuffer.getArray(); }
    
    PLONK_INLINE_LOW const SampleType& getValue() const throw() 
    { 
        // the underlying Channel or Unit may not have called initValue()
        plonk_assert (this->isNull() || hasInitialisedValue == true); 
        return outputBuffer.last();
    }
    
    PLONK_INLINE_LOW void setOutputBuffer (Buffer const& externalBuffer) throw()
    {
        plonk_assert (this->canUseExternalBuffer());
        plonk_assert (externalBuffer.length() == this->getBlockSize().getValue());
        usingExternalBuffer = true;
        outputBuffer = externalBuffer;
    }
    
    PLONK_INLINE_LOW bool isUsingExternalBuffer() const throw() { return usingExternalBuffer; }
    
    PLONK_INLINE_LOW void removeExternalBuffer() throw()
    {
        plonk_assert (usingExternalBuffer == true);
        usingExternalBuffer = false;
        outputBuffer = Buffer::newClear (this->getBlockSize().getValue());
    }
    
    virtual void setBlockSize (BlockSize const& newBlockSize) throw()
    {        
        if (newBlockSize != this->getBlockSize())
        {
            plonk_assert (newBlockSize.getValue() > 0);

            this->getBlockSize().removeReceiver (this);
            this->setBlockSizeInternal (newBlockSize);
            this->getBlockSize().addReceiver (this);
            this->updateBlockSize();
        }
    }
    
    virtual void setSampleRate (SampleRate const& newSampleRate) = 0;
    virtual void setOverlap (DoubleVariable const& newOverlap) = 0;
    
    virtual ChannelInternalBase* getChannel (const int index) = 0;
            
    virtual void initValue (SampleType const& value) throw()
    {
#ifdef PLONK_DEBUG
        plonk_assert (outputBuffer.length() > 0);
        hasInitialisedValue = true;
#endif                
        outputBuffer.last() = value;
    }
    
    PLONK_INLINE_LOW const Text getOutputTypeName() const throw()         { return TypeUtility<SampleType>::getTypeName(); }
    virtual const Text getInputTypeName() const throw()                   { return TypeUtility<SampleType>::getTypeName(); }
    PLONK_INLINE_LOW int getOutputTypeCode() const throw()                { return TypeUtility<SampleType>::getTypeCode(); }
    virtual int getInputTypeCode() const throw()                          { return TypeUtility<SampleType>::getTypeCode(); }
    
    void changed (IntVariable::Sender const& source, Text const& message, Dynamic const& payload) throw()
    {
        (void)message;
        (void)payload;
        
        BlockSize blockSizeSource = static_cast<BlockSize> (source);
        
        if (blockSizeSource == this->getBlockSize())
        {
            this->updateBlockSize();
            return;
        }
    }
    
    PLONK_INLINE_LOW void updateBlockSize() throw()
    {
        if (usingExternalBuffer == false)
        {
            const int size = this->getBlockSize().getValue();
            
            if (size != outputBuffer.length())
                outputBuffer.setSize (size, false);
        }
    }
    
private:
    Buffer outputBuffer;
    bool usingExternalBuffer;
    
#ifdef PLONK_DEBUG
    bool hasInitialisedValue;
#endif
    
    ChannelInternalBase();
    ChannelInternalBase (const ChannelInternalBase&);
	const ChannelInternalBase& operator= (const ChannelInternalBase&);
};


#endif // PLONK_CHANNELINTERNALBASE_H