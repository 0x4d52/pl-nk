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

#ifndef PLONK_DELAY3PARAM_H
#define PLONK_DELAY3PARAM_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "plonk_DelayForwardDeclarations.h"


/** Delay processor with two control parameters. */
template<class FormType>
class Delay3ParamChannelInternal
:   public ProxyOwnerChannelInternal<typename FormType::SampleDataType, 
                                     typename FormType::Data>
{
public:
    typedef typename FormType::SampleDataType               SampleType;
    typedef typename FormType::Data                         Data;
    typedef typename Data::DelayState                       DelayState;

    typedef ChannelBase<SampleType>                         ChannelType;
    typedef Delay3ParamChannelInternal<FormType>            DelayInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>      Internal;
    typedef ChannelInternalBase<SampleType>                 InternalBase;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType     DurationType;
    
    typedef typename FormType::Param1Type                   Param1Type;
    typedef UnitBase<Param1Type>                            Param1UnitType;
    typedef NumericalArray<Param1Type>                      Param1BufferType;

    typedef typename FormType::Param2Type                   Param2Type;
    typedef UnitBase<Param2Type>                            Param2UnitType;
    typedef NumericalArray<Param2Type>                      Param2BufferType;

    typedef typename FormType::Param3Type                   Param3Type;
    typedef UnitBase<Param3Type>                            Param3UnitType;
    typedef NumericalArray<Param3Type>                      Param3BufferType;

    typedef typename FormType::InputFunction                InputFunction;
    typedef typename FormType::ReadFunction                 ReadFunction;
    typedef typename FormType::WriteFunction                WriteFunction;
    typedef typename FormType::OutputFunction               OutputFunction;
    typedef typename FormType::Param1Function               Param1Function;
    typedef typename FormType::Param2Function               Param2Function;
    typedef typename FormType::Param3Function               Param3Function;
    
    enum Params
    {
        Param1,
        Param2, 
        Param3,
        NumParams
    };
    
    Delay3ParamChannelInternal (Inputs const& inputs, 
                                Data const& data, 
                                BlockSize const& blockSize,
                                SampleRate const& sampleRate) throw()
    :   Internal (getNumChannelsFromInputs (inputs), // num proxies
                  inputs, data, blockSize, sampleRate)
    {
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
        const UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        
        if ((channel % this->getNumChannels()) == 0)
        {
            this->setBlockSize (BlockSize::decide (inputUnit.getBlockSize (channel),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (inputUnit.getSampleRate (channel),
                                                     this->getSampleRate()));
            
            this->setOverlap (inputUnit.getOverlap (channel));
            
            for (int i = 0; i < this->getNumChannels(); ++i)
                this->initProxyValue (i, SampleType (0));            
            
            Data& data = this->getState();
            this->circularBuffer = Buffer::newClear (int (data.maximumDuration * data.base.sampleRate + 0.5) + 1);
        }
    }    
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        Data& data = this->getState();
        
        UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        const Buffer inputBuffer (inputUnit.process (info, 0));
        Param1UnitType& param1Unit = ChannelInternalCore::getInputAs<Param1UnitType> (FormType::getInputKeys().atUnchecked (1));
        Param2UnitType& param2Unit = ChannelInternalCore::getInputAs<Param2UnitType> (FormType::getInputKeys().atUnchecked (2));
        Param3UnitType& param3Unit = ChannelInternalCore::getInputAs<Param3UnitType> (FormType::getInputKeys().atUnchecked (3));
        
        plonk_assert (inputBuffer.length() == this->getOutputBuffer (0).length());
        
        const int writePosition = process<FormType::inputRead, 
                                          FormType::readRead, 
                                          FormType::writeWrite, 
                                          FormType::outputWrite,
                                          FormType::param1Process,
                                          FormType::param2Process,
                                          FormType::param3Process>
                                    (this->getOutputSamples (0), 
                                     this->getOutputBuffer (0).length(), 
                                     inputBuffer.getArray(), 
                                     param1Unit,
                                     param2Unit,
                                     param3Unit,
                                     this->circularBuffer, data, info, 0);
        
        const int numChannels = this->getNumChannels();
        
        for (int i = 1; i < numChannels; ++i)
        {
            plonk_assert (inputBuffer.length() == this->getOutputBuffer (i).length());
            
            process<FormType::inputIgnore, 
                    FormType::readRead, 
                    FormType::writeIgnore, 
                    FormType::outputWrite,
                    FormType::param1Process,
                    FormType::param2Process,
                    FormType::param3Process>
                (this->getOutputSamples (i), 
                 this->getOutputBuffer (i).length(), 
                 0, 
                 param1Unit, 
                 param2Unit,
                 param3Unit,
                 this->circularBuffer, data, info, i);
        }
        
        data.writePosition = writePosition; // update the write position from the first channel write
    }
    

private:
    static inline int getNumChannelsFromInputs (Inputs const& inputs) throw()
    {
        const int p1 = inputs[FormType::getInputKeys().atUnchecked (1)].template asUnchecked<Param1UnitType>().getNumChannels();
        const int p2 = inputs[FormType::getInputKeys().atUnchecked (2)].template asUnchecked<Param2UnitType>().getNumChannels();
        const int p3 = inputs[FormType::getInputKeys().atUnchecked (3)].template asUnchecked<Param3UnitType>().getNumChannels();
        return plonk::max (p1, p2, p3);
    }
        
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction,
             Param1Function param1Function,
             Param2Function param2Function,
             Param3Function param3Function>
    static int process (SampleType* const outputSamples,
                        const int outputBufferLength,
                        const SampleType* inputSamples,
                        Param1UnitType& param1Unit,
                        Param2UnitType& param2Unit,
                        Param3UnitType& param3Unit,
                        Buffer& buffer,
                        Data& data,
                        ProcessInfo& info, 
                        const int channel) throw()
    {        
        DelayState state = {
            { data.base.sampleRate, data.base.sampleDuration },
            outputSamples,
            outputBufferLength,
            inputSamples,
            buffer.getArray(),
            buffer.length() - 1 ,
            DurationType (buffer.length()) - Math<DurationType>::get1(),
            DurationType (0),
            data.writePosition,
            SampleType (0), SampleType (0), SampleType (0), SampleType (0),
            { 0, 0, 0 }, { 0, 0, 0 }
        };

        const Param1BufferType param1Buffer (param1Unit.process (info, channel));
        const Param1Type* param1Samples = param1Buffer.getArray();
        const int param1BufferLength = param1Buffer.length();
        
        const Param2BufferType param2Buffer (param2Unit.process (info, channel));
        const Param2Type* param2Samples = param2Buffer.getArray();
        const int param2BufferLength = param2Buffer.length();

        const Param3BufferType param3Buffer (param3Unit.process (info, channel));
        const Param3Type* param3Samples = param3Buffer.getArray();
        const int param3BufferLength = param3Buffer.length();

        int numSamplesToProcess = outputBufferLength;
        
        if ((param1BufferLength == outputBufferLength) && 
            (param2BufferLength == outputBufferLength) &&
            (param3BufferLength == outputBufferLength))
        {            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = state.bufferLength - state.writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {
                    param1Function (state, *param1Samples++);  
                    param2Function (state, *param2Samples++);  
                    param3Function (state, *param3Samples++);                    

                    FormType::template tick<inputFunction, readFunction, writeFunction, outputFunction> (state);                    
                }
                
                if (state.writePosition >= state.bufferLength)
                    state.writePosition = 0;
            }
        }
        else if ((param1BufferLength == 1) && 
                 (param2BufferLength == 1) && 
                 (param3BufferLength == 1))
        {
            param1Function (state, param1Samples[0]);                    
            param2Function (state, param2Samples[0]);  
            param3Function (state, param3Samples[0]);                    
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = state.bufferLength - state.writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                    FormType::template tick<inputFunction, readFunction, writeFunction, outputFunction> (state);
                
                if (state.writePosition >= state.bufferLength)
                    state.writePosition = 0;
            }
        }
        else
        {                        
            double param1Position = 0.0;
            const double param1Increment = double (param1BufferLength) / double (outputBufferLength);
            double param2Position = 0.0;
            const double param2Increment = double (param2BufferLength) / double (outputBufferLength);
            double param3Position = 0.0;
            const double param3Increment = double (param3BufferLength) / double (outputBufferLength);
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = state.bufferLength - state.writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {                    
                    param1Function (state, param1Samples[int (param1Position)]);   
                    param2Function (state, param2Samples[int (param2Position)]);       
                    param3Function (state, param3Samples[int (param3Position)]);                    
                    FormType::template tick<inputFunction, readFunction, writeFunction, outputFunction> (state);
                    param1Position += param1Increment;
                    param2Position += param2Increment;
                    param3Position += param3Increment;
                }
                
                if (state.writePosition >= state.bufferLength)
                    state.writePosition = 0;
            }
        }
        
        return state.writePosition;
    }

        
private:
    Buffer circularBuffer;
};


#endif // PLONK_DELAY3PARAM_H

