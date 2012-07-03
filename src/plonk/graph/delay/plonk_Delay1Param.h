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
#include "../plonk_GraphForwardDeclarations.h"


/** Delay processor. */
template<class FormType>
class Delay1ParamChannelInternal
:   public ProxyOwnerChannelInternal<typename FormType::SampleDataType, 
                                     typename FormType::Data>
{
public:
    typedef typename FormType::SampleDataType               SampleType;
    typedef typename FormType::Data                         Data;
    
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef Delay1ParamChannelInternal<FormType>            DelayInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>      Internal;
    typedef ChannelInternalBase<SampleType>                 InternalBase;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
    
    typedef typename TypeUtility<SampleType>::IndexType     DurationType;
    
    typedef typename FormType::Param1Type                   Param1Type;
    typedef typename FormType::Param1UnitType               Param1UnitType;
    typedef typename FormType::Param1BufferType             Param1BufferType;
        
    typedef void (*InputFunction)(Data&);
    typedef void (*ReadFunction)(Data&, const int);
    typedef void (*WriteFunction)(Data&, const int);
    typedef void (*OutputFunction)(Data&, int&);
    typedef void (*Param1Function)(Data&, const Param1Type);
    
    enum Params
    {
        Param1,
        NumParams
    };
    
    Delay1ParamChannelInternal (Inputs const& inputs, 
                                Data const& data, 
                                BlockSize const& blockSize,
                                SampleRate const& sampleRate) throw()
    :   Internal (1, inputs, data, blockSize, sampleRate)
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
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new DelayInternal (channelInputs, 
                                  this->getState(), 
                                  this->getBlockSize(), 
                                  this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {        
        const UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        
        this->setBlockSize (BlockSize::decide (inputUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (inputUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (inputUnit.getOverlap (channel));
        
        this->initValue (SampleType (0));
        
        Data& data = this->getState();
        this->circularBuffer = Buffer::newClear (int (data.maximumDuration * data.base.sampleRate + 0.5));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        Data& data = this->getState();
        
        UnitType& inputUnit = this->getInputAsUnit (IOKey::Generic);
        const Buffer inputBuffer (inputUnit.process (info, channel));
        data.inputSamples = inputBuffer.getArray();
        
        data.outputSamples = this->getOutputSamples (0);
        const int outputBufferLength = this->getOutputBuffer (0).length();
        plonk_assert (inputBuffer.length() == outputBufferLength);

        const int writePosition = process<FormType::inputRead, 
                                          FormType::readRead, 
                                          FormType::writeWrite, 
                                          FormType::outputWrite,
                                          FormType::param1Process>
                                    (outputBufferLength, 
                                     this->getInputAsUnit (FormType::getInputKeys().atUnchecked (1)), 
                                     this->circularBuffer,
                                     data,
                                     info, 
                                     channel);     
        
        data.writePosition = writePosition;
    }
    
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction>
    static inline void tick (Data& data, int& writePosition) throw()
    {
        inputFunction (data);
        readFunction (data, writePosition);  
        writeFunction (data, writePosition);
        outputFunction (data, writePosition);
    }
    
    template<InputFunction inputFunction, 
             ReadFunction readFunction,
             WriteFunction writeFunction,
             OutputFunction outputFunction,
             Param1Function param1Function>
    static int process (const int outputBufferLength,
                        Param1UnitType& param1Unit,
                        Buffer& buffer,
                        Data& data,
                        ProcessInfo& info, 
                        const int channel) throw()
    {        
        const Param1BufferType param1Buffer (param1Unit.process (info, channel));
        const Param1Type* param1Samples = param1Buffer.getArray();
        const int param1BufferLength = param1Buffer.length();
        
        data.bufferSamples = buffer.getArray();
        const int bufferLength = buffer.length();
        data.bufferLengthIndex = Param1Type (bufferLength);
        data.buffer0 = Param1Type (0);
        
        int writePosition = data.writePosition;
        
        int numSamplesToProcess = outputBufferLength;
        
        if (param1BufferLength == outputBufferLength)
        {            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = bufferLength - writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {
                    param1Function (data, *param1Samples++);                    
                    tick<inputFunction, readFunction, writeFunction, outputFunction> (data, writePosition);                    
                }
                
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }
        }
        else if (param1BufferLength == 1)
        {
            param1Function (data, param1Samples[0]);                    
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = bufferLength - writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                    tick<inputFunction, readFunction, writeFunction, outputFunction> (data, writePosition);                    
                
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }
        }
        else
        {                        
            double param1Position = 0.0;
            const double param1Increment = double (param1BufferLength) / double (outputBufferLength);
            
            while (numSamplesToProcess > 0)
            {
                int bufferSamplesRemaining = bufferLength - writePosition;
                int numSamplesThisTime = plonk::min (bufferSamplesRemaining, numSamplesToProcess);
                numSamplesToProcess -= numSamplesThisTime;
                
                while (numSamplesThisTime--) 
                {                    
                    param1Function (data, param1Samples[int (param1Position)]);                    
                    tick<inputFunction, readFunction, writeFunction, outputFunction> (data, writePosition);
                    param1Position += param1Increment;
                }
                
                if (writePosition >= bufferLength)
                    writePosition = 0;
            }
        }
        
        return writePosition;
    }

        
private:
    Buffer circularBuffer;
};

//------------------------------------------------------------------------------

/** Simple delay processor. */
template<class SampleType>
class DelayUnit
{
public:    
    typedef DelayForm<SampleType, DelayFormType::Delay, 1>  FormType;

    typedef Delay1ParamChannelInternal<FormType>            DelayInternal;
    typedef typename DelayInternal::Data                    Data;
    typedef ChannelBase<SampleType>                         ChannelType;
    typedef ChannelInternal<SampleType,Data>                Internal;
    typedef UnitBase<SampleType>                            UnitType;
    typedef InputDictionary                                 Inputs;
    typedef NumericalArray<SampleType>                      Buffer;
    
    typedef typename DelayInternal::Param1Type              DurationType;
    typedef typename DelayInternal::Param1UnitType          DurationUnitType;
    typedef typename DelayInternal::Param1BufferType        DurationBufferType;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Delay", "A simple delay processor.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,                IOLimit::None,                         
                         IOKey::End,
                         
                         // inputs
                         IOKey::Generic,            Measure::None,      IOInfo::NoDefault,  IOLimit::Minimum,   Measure::Seconds,   0.0,
                         IOKey::Duration,           Measure::Seconds,   0.5,                IOLimit::None,
                         IOKey::MaximumDuration,    Measure::Seconds,   1.0,                IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,                IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,                IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,          IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate,         IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate wavetable oscillator. */
    static UnitType ar (UnitType const& input,
                        DurationUnitType const& duration = DurationType (0.5),
                        const DurationType maximumDuration = DurationType (1.0),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        inputs.put (IOKey::Duration, duration);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
          
        Data data = { { -1.0, -1.0 }, maximumDuration, 0, 
                      0, 0, 0, 0,
                      0, 0, 0, 
                      0, 0, { 0 }};
        
        //proxiesFromInputs // <--- can use this anyway for just one channel...
        return UnitType::template createFromInputs<DelayInternal> (inputs, 
                                                                   data, 
                                                                   preferredBlockSize, 
                                                                   preferredSampleRate);
    }
};

typedef DelayUnit<PLONK_TYPE_DEFAULT> Delay;







#endif // PLONK_DELAY1PARAM_H

