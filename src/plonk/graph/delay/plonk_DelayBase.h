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

#ifndef PLONK_DELAYBASE_H
#define PLONK_DELAYBASE_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_DelayForwardDeclarations.h"


template<class FormType>
class DelayChannelInternalBase
:   public ProxyOwnerChannelInternal<typename FormType::SampleDataType, 
                                     typename FormType::Data>
{
public:
    typedef typename FormType::SampleDataType                       SampleType;
    typedef typename FormType::Data                                 Data;
    typedef typename FormType::DelayState                           DelayState;
    typedef ObjectArray<DelayState>                                 DelayStateArray;
    
    typedef typename DelayFormData<SampleType,0,1,1>::DelayState    DelayStateBase;
    
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef Delay1ParamChannelInternal<FormType>                    DelayInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>              Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef ObjectArray<Buffer>                                     BufferArray;
    
    typedef typename TypeUtility<SampleType>::IndexType             DurationType;
                    
    DelayChannelInternalBase (const int numOutputs, 
                              Inputs const& inputs, 
                              Data const& data, 
                              BlockSize const& blockSize,
                              SampleRate const& sampleRate) throw()
    :   Internal (numOutputs, inputs, data, blockSize, sampleRate)
    {
        delayStates = DelayStateArray::withSize (this->getNumChannels());
        circularBuffers = BufferArray::withSize (FormType::getNumCircularBuffers());
    }
    
    Text getName() const throw()
    {
        return FormType::getName();
    }       
    
    IntArray getInputKeys() const throw()
    {
        return FormType::getInputKeys();
    }    
        
    void initChannel (const int channel) throw()
    {        
        int i; 
        
        const UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        
        if ((channel % this->getNumChannels()) == 0)
        {
            this->setBlockSize (BlockSize::decide (inputUnit.getBlockSize (channel),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (inputUnit.getSampleRate (channel),
                                                     this->getSampleRate()));
            
            this->setOverlap (inputUnit.getOverlap (channel));
            
            Data& data = this->getState();
            
            for (i = 0; i < FormType::getNumCircularBuffers(); ++i)
                circularBuffers.put (i, Buffer::newClear (int (data.maximumDuration * data.base.sampleRate + 0.5) + 1));
            
            for (i = 0; i < this->getNumChannels(); ++i)
            {
                this->initProxyValue (i, SampleType (0));            

                DelayState& delayState = delayStates.atUnchecked (i);
                Buffer& circularBuffer = circularBuffers.wrapAt (i);
                
                Memory::zero (delayState);
                
                DelayStateBase& delayStateBase (reinterpret_cast<DelayStateBase&>(delayState));
                delayStateBase.bufferSamples = circularBuffer.getArray();
                delayStateBase.bufferLength = circularBuffer.length() - 1,
                delayStateBase.bufferLengthIndex = DurationType (circularBuffer.length()) - Math<DurationType>::get1(),
                delayStateBase.buffer0 = DurationType (0);
            }
        }
    }    
    
    inline BufferArray& getCircularBuffers() { return circularBuffers; }
    inline DelayStateArray& getDelayStates() { return delayStates; }
    
private:
    BufferArray circularBuffers;
    DelayStateArray delayStates;
};



#endif // PLONK_DELAYBASE_H

