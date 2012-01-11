/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
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
 -------------------------------------------------------------------------------
 */

#ifndef PLONK_CHANNELINTERNAL_H
#define PLONK_CHANNELINTERNAL_H

#include "../plonk_GraphForwardDeclarations.h"
#include "plonk_ChannelInternalCore.h"


/** The base class from which most ChannelInternals should be derived.
 
 DataType should be a class which stores a ChannelInternal's
 current state (e.g., current phase, filter coefficients etc). It should be 
 a POD (plain old data) type so it can be usefully used to pass to C code
 if neccesary. 
 
 Normally a ChannelInternal will process only one channel of output buffer 
 samples. Multichannel operation is achieved by arrays of these or in some 
 cases the proxy system is used (see below).
 */
template<class SampleType, class DataType>
class ChannelInternal : public ChannelInternalBase<SampleType>,
                        public DoubleVariable::Receiver
{
public:            
    typedef ChannelBase<SampleType>         ChannelType;
    typedef ChannelInternalBase<SampleType> Internal;
    typedef UnitBase<SampleType>            UnitType;
    typedef NumericalArray<SampleType>      Buffer;
    typedef InputDictionary                 Inputs;    
    typedef ChannelBase<SampleType>         Container;
    typedef ChannelInternalCore::Data       BaseData;
        
    ChannelInternal (Inputs const& inputDictionary, 
                     DataType const& initState, 
                     BlockSize const& blockSize,
                     SampleRate const& sampleRate) throw()
    :   Internal (inputDictionary, blockSize, sampleRate),
        state (initState)
    {
        this->getSampleRate().addReceiver (this);
        this->updateSampleRateInData();
    }
    
    ~ChannelInternal()
    {
        this->getSampleRate().removeReceiver (this);
    }
    
    const DataType& getState() const throw() { return state; }
    DataType& getState() throw() { return state; }
    
    void setSampleRate (SampleRate const& newSampleRate) throw()
    {
        if (newSampleRate != this->getSampleRate())
        {
            plonk_assert(newSampleRate.getValue() >= 0.0);

            this->getSampleRate().removeReceiver (this);
            this->setSampleRateInternal (newSampleRate);
            this->getSampleRate().addReceiver (this);
            
            this->updateSampleRateInData();
        }
    }
    
    void setOverlap (DoubleVariable const& newOverlap) throw()
    {        
        if (newOverlap != this->getOverlap())
        {
            plonk_assert(newOverlap.getValue() > 0.0);
            plonk_assert(newOverlap.getValue() <= 1.0);

            this->getOverlap().removeReceiver (this);
            this->setOverlapInternal (newOverlap);
            this->getOverlap().addReceiver (this);
            
            this->updateTimeStamp(); // changes the next time due to chnaged overlap..
        }
    }
    
    void changed (DoubleVariable::Sender const& source) throw()
    {
        SampleRate sampleRateSource = static_cast<SampleRate> (source);
        
        if (sampleRateSource == this->getSampleRate())
        {
            this->updateSampleRateInData();
            return;
        }

        DoubleVariable overlapSource = static_cast<DoubleVariable> (source);
            
        if (overlapSource == this->getOverlap())
        {
            this->updateTimeStamp();
            return;
        }
    }
    
    void updateSampleRateInData() throw()
    {
        BaseData& baseData (reinterpret_cast<BaseData&> (state));
        
        const double newSampleRate = this->getSampleRate().getValue();
        
        if (baseData.sampleRate != newSampleRate)
        {
            baseData.sampleRate = newSampleRate;
            baseData.sampleDuration = 1.0 / newSampleRate;
        }
    }
    
private:
    DataType state;
    
    ChannelInternal();
    ChannelInternal (const ChannelInternal&);
	const ChannelInternal& operator= (const ChannelInternal&);
};





#endif // PLONK_CHANNELINTERNAL_H