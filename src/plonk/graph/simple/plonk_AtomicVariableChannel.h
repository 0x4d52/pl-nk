/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_ATOMICVARIABLECHANNEL_H
#define PLONK_ATOMICVARIABLECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

/** Atomic variable channel. */
template<class SampleType>
class AtomicVariableChannelInternal 
:   public ChannelInternal<SampleType, ChannelInternalCore::Data>
{
public:
    typedef ChannelInternalCore::Data                           Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef AtomicVariableChannelInternal<SampleType>           AtomicVariableChannelInternalType;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef AtomicValue<SampleType>                             AtomicType;
    typedef Variable<AtomicType&>                               AtomicVariableType;
    
    AtomicVariableChannelInternal (Inputs const& inputs, 
                                   Data const& data, 
                                   BlockSize const& blockSize,
                                   SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "Atomic Variable";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::AtomicVariable);
        return keys;
    }    
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }        
    
    void initChannel (const int /*channel*/) throw()
    {
        AtomicVariableType& atomicVariable = ChannelInternalCore::getInputAs<AtomicVariableType> (IOKey::AtomicVariable);
        AtomicType& atomicValue = atomicVariable.getValue();

        this->initValue (atomicValue.getValue());
    }    
    
    void process (ProcessInfo& /*info*/, const int /*channel*/) throw()
    {                
        AtomicVariableType& atomicVariable = ChannelInternalCore::getInputAs<AtomicVariableType> (IOKey::AtomicVariable);
        AtomicType& atomicValue = atomicVariable.getValue();
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();        
        
        for (int i = 0; i < outputBufferLength; ++i) 
            outputSamples[i] = atomicValue.getValue();          
    }

private:
};



//------------------------------------------------------------------------------

/** Atomic variable unit. 
 
 @par Factory functions:
 - ar (atomicVariable=0, preferredBlockSize=default)
 - kr (atomicVariable=0) 
 
 @par Inputs:
 - atomicVariable: (atomicvariable) the atomicvariable to use as a unit, changes to the value are threadsafe
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)

 @ingroup ControlUnits */
template<class SampleType>
class AtomicVariableUnit
{
public:    
    typedef AtomicVariableChannelInternal<SampleType>               AtomicVariableChannelInternalType;
    typedef typename AtomicVariableChannelInternalType::Data        Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternaBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef NumericalArray<SampleType>                              Buffer;
    typedef AtomicValue<SampleType>                                 AtomicType;
    typedef Variable<AtomicType&>                                   AtomicVariableType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
//        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Atomic Variable", "An atomic variable value.",
                         
                         // output
                         1, 
                         IOKey::Generic,            Measure::None,      IOInfo::NoDefault,  IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::AtomicVariable,     Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,  IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::End);
    }    
    
    /** Create control rate variable. */
    static UnitType ar (AtomicVariableType const& atomicVariable = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::AtomicVariable, atomicVariable);
        
        Data data = { -1.0, -1.0 };
        
        const SampleRate preferredSampleRate = (preferredBlockSize == BlockSize::getDefault()) ?
                                                SampleRate::getDefault() :
                                                (SampleRate::getDefault() / BlockSize::getDefault()) * preferredBlockSize;
        
        return UnitType::template createFromInputs<AtomicVariableChannelInternalType> (inputs, 
                                                                                       data, 
                                                                                       preferredBlockSize, 
                                                                                       preferredSampleRate);
    }   

    static UnitType kr (AtomicVariableType const& atomicVariable = SampleType (0)) throw()
    {
        Inputs inputs;
        inputs.put (IOKey::AtomicVariable, atomicVariable);
        
        Data data = { -1.0, -1.0 };
                
        return UnitType::template createFromInputs<AtomicVariableChannelInternalType> (inputs, 
                                                                                       data, 
                                                                                       BlockSize::getControlRateBlockSize(), 
                                                                                       SampleRate::getControlRate());        
    }
};

// can't have a default typedef yet just in case the user uses a default type that can't be atomic.


#endif // PLONK_ATOMICVARIABLECHANNEL_H


