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

#ifndef PLONK_CONSTANTCHANNEL_H
#define PLONK_CONSTANTCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"
#include "../utility/plonk_BlockSize.h"


/** For generating constants. */
template<class SampleType>
class ConstantChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data           Data;
    typedef NumericalArray<SampleType>          Buffer;
    typedef ChannelInternal<SampleType,Data>    Internal;
    typedef ChannelInternalBase<SampleType>     InternalBase;
    typedef InputDictionary                     Inputs;    

    enum Outputs { Output, NumOutputs };
    
    template<class ValueType>
    ConstantChannelInternal (ValueType const& valueInit) throw()
    :   Internal (Inputs(),
                  Data::create(-1.0),
                  BlockSize::getBlockSize1(), 
                  SampleRate::getSampleRate0()),
        value (valueInit)
    {
    }
    
    Text getName() const throw()
    {
        return "Constant";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Constant);
        return keys;
    }    
        
    bool isConstant() const throw() { return true; }
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    } 
        
    void initChannel (const int /*channel*/) throw()
    {
        // only ever one channel from here upwards
        this->initValue (value);
    }
    
    void process (ProcessInfo& /*info*/, const int /*channel*/) throw()
    {
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();        
        
        for (int i = 0; i < outputBufferLength; ++i) 
            outputSamples[i] = value;          
    }
            
private:
    const SampleType value;
};

//------------------------------------------------------------------------------

/** For generating 0 (null) specifically. */
template<class SampleType>
class NullChannelInternal : public ConstantChannelInternal<SampleType>
{
public:
    typedef NullChannelInternal<SampleType> Internal;
    typedef ConstantChannelInternal<SampleType> Base;
        
    NullChannelInternal () throw()
    :   Base (SampleType (0))
    {
    }
    
    bool isNull() const throw() { return true; }
    
    Text getName() const throw()
    {
        return "Null (0)";
    }            
};



#endif // PLONK_CONSTANTCHANNEL_H
