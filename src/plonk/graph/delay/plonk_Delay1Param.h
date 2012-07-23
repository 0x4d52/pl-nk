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

#ifndef PLONK_DELAY1PARAM_H
#define PLONK_DELAY1PARAM_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_DelayForwardDeclarations.h"


/** Delay processor. */
template<class FormType>
class Delay1ParamChannelInternal
:   public DelayChannelInternalBase<FormType>
{
public:
    typedef typename FormType::SampleDataType               SampleType;
    typedef typename FormType::Data                         Data;
    typedef typename FormType::DelayState                   DelayState;
    
    typedef ObjectArray<DelayState>                         DelayStateArray;
    
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ObjectArray<ChannelType>                        ChannelArrayType;
    typedef Delay1ParamChannelInternal<FormType>            DelayInternal;
    typedef DelayChannelInternalBase<FormType>              Internal;
    typedef ChannelInternalBase<SampleType>                 InternalBase;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType     DurationType;
    
    typedef typename FormType::Param1Type                   Param1Type;
    typedef UnitBase<Param1Type>                            Param1UnitType;
    typedef NumericalArray<Param1Type>                      Param1BufferType;
        
    typedef typename FormType::InputFunction                InputFunction;
    typedef typename FormType::ReadFunction                 ReadFunction;
    typedef typename FormType::WriteFunction                WriteFunction;
    typedef typename FormType::OutputFunction               OutputFunction;
    typedef typename FormType::Param1Function               Param1Function;
    
    enum Params
    {
        Param1,
        NumParams
    };
    
    Delay1ParamChannelInternal (Inputs const& inputs, 
                                Data const& data, 
                                BlockSize const& blockSize,
                                SampleRate const& sampleRate,
                                ChannelArrayType& channels) throw()
    :   Internal (getNumChannelsFromInputs (inputs), // num proxies
                  inputs, data, blockSize, sampleRate, channels),
        inputKeys (FormType::getInputKeys())
    {
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        Data& data = this->getState();
        DelayStateArray& delayStates = this->getDelayStates();
        
        UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        const Buffer& inputBuffer (inputUnit.process (info, 0));
        Param1UnitType& param1Unit = ChannelInternalCore::getInputAs<Param1UnitType> (inputKeys.atUnchecked (1));
                        
        plonk_assert (inputBuffer.length() == this->getOutputBuffer (0).length());

        const int writePosition = process<FormType::inputRead, 
                                          FormType::readRead, 
                                          FormType::writeWrite, 
                                          FormType::outputWrite,
                                          FormType::param1Process>
                                    (this->getOutputSamples (0), 
                                     this->getOutputBuffer (0).length(), 
                                     inputBuffer.getArray(), 
                                     param1Unit, 
                                     data, delayStates.atUnchecked (0), info, 0);
        
        const int numChannels = this->getNumChannels();

        for (int i = 1; i < numChannels; ++i)
        {
            plonk_assert (inputBuffer.length() == this->getOutputBuffer (i).length());

            process<FormType::inputIgnore, 
                    FormType::readRead, 
                    FormType::writeIgnore, 
                    FormType::outputWrite,
                    FormType::param1Process>
                (this->getOutputSamples (i), 
                 this->getOutputBuffer (i).length(), 
                 0, 
                 param1Unit, 
                 data, delayStates.atUnchecked (i), info, i);
        }
        
        data.writePosition = writePosition; // update the write position from the first channel write
    }
    
private:
    IntArray inputKeys;
    
    static inline int getNumChannelsFromInputs (Inputs const& inputs) throw()
    {
        return inputs[FormType::getInputKeys().atUnchecked (1)].template asUnchecked<Param1UnitType>().getNumChannels();
    }
        
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction,
             Param1Function param1Function>
    static int process (SampleType* const outputSamples,
                        const int outputBufferLength,
                        const SampleType* inputSamples,
                        Param1UnitType& param1Unit,
                        Data& data,
                        DelayState& state,
                        ProcessInfo& info, 
                        const int channel) throw()
    {                
        state.outputSamples = outputSamples;
        state.outputBufferLength = outputBufferLength;
        state.inputSamples = inputSamples;
        state.writePosition = data.writePosition;
        
        const Param1BufferType& param1Buffer (param1Unit.process (info, channel));
        const Param1Type* param1Samples = param1Buffer.getArray();
        const int param1BufferLength = param1Buffer.length();
        
        int numSamplesToProcess = outputBufferLength;
        
        if (param1BufferLength == outputBufferLength)
        {            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = state.bufferLength - state.writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {
                    param1Function (data, state, *param1Samples++);                    
                    FormType::template tick<inputFunction, readFunction, writeFunction, outputFunction> (data, state);                    
                }
                
                if (state.writePosition >= state.bufferLength)
                    state.writePosition = 0;
            }
        }
        else if (param1BufferLength == 1)
        {
            param1Function (data, state, param1Samples[0]);                    
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = state.bufferLength - state.writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                    FormType::template tick<inputFunction, readFunction, writeFunction, outputFunction> (data, state);                    
                
                if (state.writePosition >= state.bufferLength)
                    state.writePosition = 0;
            }
        }
        else
        {                        
            double param1Position = 0.0;
            const double param1Increment = double (param1BufferLength) / double (outputBufferLength);
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = state.bufferLength - state.writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {                    
                    param1Function (data, state, param1Samples[int (param1Position)]);                    
                    FormType::template tick<inputFunction, readFunction, writeFunction, outputFunction> (data, state);
                    param1Position += param1Increment;
                }
                
                if (state.writePosition >= state.bufferLength)
                    state.writePosition = 0;
            }
        }
        
        return state.writePosition;
    }
};





#endif // PLONK_DELAY1PARAM_H

