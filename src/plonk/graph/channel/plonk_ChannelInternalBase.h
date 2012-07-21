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

    ChannelInternalBase (Inputs const& inputDictionary, 
                         BlockSize const& blockSize, 
                         SampleRate const& sampleRate) throw()
    :   ChannelInternalCore (inputDictionary, blockSize, sampleRate),
        outputBuffer (Buffer::newClear (blockSize.getValue())),
        usingExternalBuffer (false)
    {
#ifdef PLONK_DEBUG
        hasInitialisedValue = false;
#endif        
        this->getBlockSize().addReceiver (this);
    }
    
    ~ChannelInternalBase()
    {
        this->getBlockSize().removeReceiver (this);
    }
            
    inline const UnitType& getInputAsUnit (const int key) const throw()                 { return this->getInputAs<UnitType> (key); }
    inline const UnitsType& getInputAsUnits (const int key) const throw()               { return this->getInputAs<UnitsType> (key); }
    inline const BusType& getInputAsBus (const int key) const throw()                   { return this->getInputAs<BusType> (key); }
    inline const BussesType& getInputAsBusses (const int key) const throw()             { return this->getInputAs<BussesType> (key); }
    inline const Buffer& getInputAsBuffer (const int key) const throw()                 { return this->getInputAs<Buffer> (key); }
    inline const VariableType& getInputAsVariable (const int key) const throw()         { return this->getInputAs<VariableType> (key); }
    inline const BreakpointsType& getInputAsBreakpoints (const int key) const throw()   { return this->getInputAs<BreakpointsType> (key); }
    inline const WavetableType& getInputAsWavetable (const int key) const throw()       { return this->getInputAs<WavetableType> (key); }
    inline const SignalType& getInputAsSignal (const int key) const throw()             { return this->getInputAs<SignalType> (key); }
    inline const SampleRate& getInputAsSampleRate (const int key) const throw()         { return this->getInputAs<SampleRate> (key); }
    inline const BlockSize& getInputAsBlockSize (const int key) const throw()           { return this->getInputAs<BlockSize> (key); }

    inline UnitType& getInputAsUnit (const int key) throw()                             { return this->getInputAs<UnitType> (key); }
    inline UnitsType& getInputAsUnits (const int key) throw()                           { return this->getInputAs<UnitsType> (key); }
    inline BusType& getInputAsBus (const int key) throw()                               { return this->getInputAs<BusType> (key); }
    inline BussesType& getInputAsBusses (const int key) throw()                         { return this->getInputAs<BussesType> (key); }
    inline Buffer& getInputAsBuffer (const int key) throw()                             { return this->getInputAs<Buffer> (key); }
    inline VariableType& getInputAsVariable (const int key) throw()                     { return this->getInputAs<VariableType> (key); }
    inline BreakpointsType& getInputAsBreakpoints (const int key) throw()               { return this->getInputAs<BreakpointsType> (key); }
    inline WavetableType& getInputAsWavetable (const int key) throw()                   { return this->getInputAs<WavetableType> (key); }
    inline SignalType& getInputAsSignal (const int key) throw()                         { return this->getInputAs<SignalType> (key); }
    inline SampleRate& getInputAsSampleRate (const int key) throw()                     { return this->getInputAs<SampleRate> (key); }
    inline BlockSize& getInputAsBlockSize (const int key) throw()                       { return this->getInputAs<BlockSize> (key); }

    inline const Buffer& getOutputBuffer() const throw()                                { return outputBuffer; }
    inline Buffer& getOutputBuffer() throw()                                            { return outputBuffer; }
    inline const SampleType* getOutputSamples() const throw()                           { return outputBuffer.getArray(); }
    inline SampleType* getOutputSamples() throw()                                       { return outputBuffer.getArray(); }
    
    inline const SampleType& getValue() const throw() 
    { 
        // the underlying Channel or Unit may not have called initValue()
        plonk_assert (hasInitialisedValue == true); 
        return outputBuffer.last();
    }
    
    inline void setOutputBuffer (Buffer const& externalBuffer) throw()
    {
        plonk_assert (this->canUseExternalBuffer());
        plonk_assert (externalBuffer.length() == this->getBlockSize().getValue());
        usingExternalBuffer = true;
        outputBuffer = externalBuffer;
    }
    
    inline bool isUsingExternalBuffer() const throw() { return usingExternalBuffer; }
    
    inline void removeExternalBuffer() throw()
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
    
    inline const Text getOutputTypeName() const throw()         { return TypeUtility<SampleType>::getTypeName(); }
    virtual const Text getInputTypeName() const throw()         { return TypeUtility<SampleType>::getTypeName(); }
    inline int getOutputTypeCode() const throw()                { return TypeUtility<SampleType>::getTypeCode(); }
    virtual int getInputTypeCode() const throw()                { return TypeUtility<SampleType>::getTypeCode(); }
    
    void changed (IntVariable::Sender const& source) throw()
    {
        BlockSize blockSizeSource = static_cast<BlockSize> (source);
        
        if (blockSizeSource == this->getBlockSize())
        {
            this->updateBlockSize();
            return;
        }
    }
    
    inline void updateBlockSize() throw()
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