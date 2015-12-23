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

#ifndef PLONK_CHANNELINTERNALCORE_H
#define PLONK_CHANNELINTERNALCORE_H

#include "../plonk_GraphForwardDeclarations.h"
#include "../utility/plonk_ProcessInfo.h"
#include "../utility/plonk_BlockSize.h"


template<>
struct ChannelData<ChannelInternalCore>
{
    double sampleRate;
    double sampleDuration;
    
    static ChannelData create (double initSampleRate) 
    { 
        ChannelData data = { initSampleRate, 1.0 / initSampleRate };
        return data;
    }     
};        


/** The base, core class for all internal channel processes. 
 Contains the core functionality for an audio processing channel independent 
 of sample type (float, short, Int24, int etc).*/
class ChannelInternalCore
{
public:
    /** To store channel state data.
     Sublasses should inherit from this, add their own members, and
     store an instance. Where applicable the structure should match
     the equivalents in the Plink library. */
    
    typedef struct ChannelData<ChannelInternalCore> Data;
    typedef InputDictionary                         Inputs;    
    
    ChannelInternalCore (Inputs const& inputs,
                         BlockSize const& blockSize, 
                         SampleRate const& sampleRate) throw();
    virtual ~ChannelInternalCore() { }
    
    const TimeStamp& getNextTimeStamp() const throw() { return nextTimeStamp; }
    void setNextTimeStamp (TimeStamp const& newTimeStamp) throw();
    void setLastTimeStamp (TimeStamp const& newTimeStamp) throw();
    void setExpiryTimeStamp (TimeStamp const& newTimeStamp) throw();
    bool shouldBeDeletedNow (TimeStamp const& time) const throw();
    
    PLONK_INLINE_HIGH const Inputs& getInputs() const throw()                                      { return this->inputs; }
    PLONK_INLINE_HIGH Inputs& getInputs() throw()                                                  { return this->inputs; }
    
    template<class Type> PLONK_INLINE_MID const Type& getInputAs (const int key) const throw()    { return this->inputs[key].asUnchecked<Type>(); }
    template<class Type> PLONK_INLINE_MID Type& getInputAs (const int key) throw()                { return this->inputs[key].asUnchecked<Type>(); }
    
    const BlockSize& getBlockSize() const throw()    { return blockSize; }
    const SampleRate& getSampleRate() const throw()  { return sampleRate; }    
    const DoubleVariable& getOverlap() const throw() { return overlap; }
    BlockSize& getBlockSize() throw()                 { return blockSize; }
    SampleRate& getSampleRate() throw()               { return sampleRate; }    
    DoubleVariable& getOverlap() throw()              { return overlap; }

    double getSampleDurationInTicks() const throw()  { return cachedSampleDurationTicks; }
    double getBlockDurationInTicks() const throw();
    void updateTimeStamp() throw();
        
    virtual bool isNull() const throw()                 { return false; }
    virtual bool isConstant() const throw()             { return false; }
    virtual bool isProxyOwner() const throw()           { return false; }
    virtual bool isProxy() const throw()                { return false; }
    virtual bool isTypeConverter() const throw()        { return false; }
    virtual bool canUseExternalBuffer() const throw()   { return true;  }
    virtual double getLatency() const throw()           { return 0.0;   }
    virtual int getNumChannels() const throw()          { return 1; }
    
    virtual Text getLabel() const throw()               { return identifier; }  // virtual due to proxies
    virtual void setLabel (Text const& newId) throw();                          // virtual due to proxies

    /** Called to initialise the channel's initial value.
     The ChannelInternal subclass should initialise itself using this 
     channel of its inputs, calling initValue(). */
    virtual void initChannel (const int channel) = 0;
            
    virtual Text getName() const = 0;
    virtual IntArray getInputKeys() const = 0;
    TextArray getInputNames() const throw();
    IntArray getInputTypes() const throw();
    TextArray getInputTypeNames() const throw();
    
    /** The DSP function.
     This function will do all the processing for derived class. */
    virtual void process (ProcessInfo& info, const int channel) = 0;
        
protected:
    void setBlockSizeInternal (BlockSize const& newBlockSize) throw();
    void setSampleRateInternal (SampleRate const& newSampleRate) throw();
    void setOverlapInternal (DoubleVariable const& newOverlap) throw();
    
private:    
    Text identifier;
    TimeStamp lastTimeStamp;
    TimeStamp nextTimeStamp;
    TimeStamp expiryTimeStamp;
    Inputs inputs;
    BlockSize blockSize;
    SampleRate sampleRate;
    DoubleVariable overlap;
    mutable double cachedSampleDurationTicks;
    
    void cacheSampleDurationTicks() const throw();
    
    ChannelInternalCore();
    ChannelInternalCore (const ChannelInternalCore&);
	const ChannelInternalCore& operator= (const ChannelInternalCore&);    
};

//------------------------------------------------------------------------------

PLONK_INLINE_HIGH void ChannelInternalCore::setNextTimeStamp (TimeStamp const& time) throw()
{
//    plonk_assert ((nextTimeStamp.isInfinite() && time.isInfinite()) || (time > nextTimeStamp)); // gone backwards or wraparound
    nextTimeStamp = time;
}

PLONK_INLINE_HIGH void ChannelInternalCore::setLastTimeStamp (TimeStamp const& time) throw()
{
    plonk_assert (time.isFinite());
    lastTimeStamp = time;
}

PLONK_INLINE_HIGH void ChannelInternalCore::setExpiryTimeStamp (TimeStamp const& time) throw()
{
    expiryTimeStamp = time;
}

PLONK_INLINE_HIGH bool ChannelInternalCore::shouldBeDeletedNow (TimeStamp const& time) const throw()
{
    return time >= expiryTimeStamp;
}

PLONK_INLINE_MID double ChannelInternalCore::getBlockDurationInTicks() const throw()
{ 
    return this->getSampleDurationInTicks() * double (this->getBlockSize().getValue()) * overlap.getValue();
}


#endif // PLONK_CHANNELINTERNALCORE_H