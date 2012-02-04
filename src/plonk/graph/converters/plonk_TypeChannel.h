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

#ifndef PLONK_TYPECONVERTCHANNEL_H
#define PLONK_TYPECONVERTCHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"



/** For converting sample types. */
template<class OutputSampleType, class InputSampleType>
class TypeChannelInternal
:   public ChannelInternal<OutputSampleType,ChannelInternalCore::Data>
{
public:
    typedef typename ChannelInternalCore::Data          Data;

    typedef TypeChannelInternal<OutputSampleType,InputSampleType> TypeInternal;
    
    typedef ChannelBase<OutputSampleType>               OutputChannelType;
    typedef ChannelInternal<OutputSampleType,Data>      OutputChannelInternalType;
    typedef ChannelInternalBase<OutputSampleType>       OutputChannelInternalBase;
    typedef UnitBase<OutputSampleType>                  OutputUnitType;
    typedef NumericalArray<OutputSampleType>            OutputBuffer;

    typedef ChannelBase<InputSampleType>                InputChannelType;
    typedef ChannelInternal<InputSampleType,Data>       InputChannelInternalType;
    typedef UnitBase<InputSampleType>                   InputUnitType;
    typedef NumericalArray<InputSampleType>             InputBuffer;
            
    typedef InputDictionary                             Inputs;
    
    TypeChannelInternal (Inputs const& inputs, 
                         Data const& data, 
                         BlockSize const& blockSize,
                         SampleRate const& sampleRate) throw()
    :   OutputChannelInternalType (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        Text name ("Type");
        name += " (";
        name += TypeUtility<InputSampleType>::getTypeName();
        name += " to ";
        name += TypeUtility<OutputSampleType>::getTypeName();
        name += ")";
        return name;
    }    
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Generic);
        return keys;
    }    
    
    bool isTypeConverter() const throw() 
    { 
        return true; 
    }
    
    const Text getInputTypeName() const throw()
    { 
        return TypeUtility<InputSampleType>::getTypeName(); 
    }
    
    int getInputTypeCode() const throw()
    { 
        return TypeUtility<InputSampleType>::getTypeCode(); 
    }
    
    InputUnitType getInputUnit() const throw()
    {
        const InputUnitType& input (this->template getInputAs<InputUnitType> (IOKey::Generic));
        return input;
    }
    
    OutputChannelInternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new TypeInternal (channelInputs, 
                                 this->getState(), 
                                 this->getBlockSize(), 
                                 this->getSampleRate());
    }                
    
    void initChannel (const int channel) throw()
    {        
        InputUnitType& input (this->template getInputAs<InputUnitType> (IOKey::Generic));

        this->setBlockSize (BlockSize::decide (input.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (input.getSampleRate (channel),
                                                 this->getSampleRate()));      
                
        this->setOverlap (input.getOverlap (channel));
        
        InputSampleType sourceValue = input.getValue (channel);
        this->initValue (OutputSampleType (sourceValue));
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        InputUnitType& input (this->template getInputAs<InputUnitType> (IOKey::Generic));
        const InputBuffer& inputBuffer (input.process (info, channel));
        
        OutputSampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const InputSampleType* const inputSamples = inputBuffer.getArray();
        const int inputBufferLength = inputBuffer.length();
        
        int i;
        
        if (inputBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i) 
                outputSamples[i] = OutputSampleType (inputSamples[i]);
        }
        else if (inputBufferLength == 1)
        {
            OutputSampleType value = OutputSampleType (inputSamples[0]); 
            
            for (i = 0; i < outputBufferLength; ++i) 
                outputSamples[i] = value;
        }
        else
        {
            double inputPosition = 0.0;
            const double inputIncrement = double (inputBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = OutputSampleType (inputSamples[int (inputPosition)]);
                inputPosition += inputIncrement;
            }        
        }
    }
    
private:
};



/** Convert from one sample type to another.
 E.g., from float to short (i.e, 16bit)
 @ingroup ConverterUnits */
template<class OutputSampleType, class InputSampleType>
class TypeUnitBase
{
public:
    typedef typename ChannelInternalCore::Data  Data;

    typedef ChannelBase<OutputSampleType>           OutputChannelType;
    typedef ChannelInternal<OutputSampleType,Data>  OutputChannelInternalType;
    typedef ChannelInternalBase<OutputSampleType>   OutputChannelInternalBaseType;
    typedef UnitBase<OutputSampleType>              OutputUnitType;    
    typedef UnitBase<InputSampleType>               InputUnitType;
    typedef InputDictionary                         Inputs;
    
    typedef TypeChannelInternal<OutputSampleType,InputSampleType> TypeInternal;
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::noPreference().getValue();
        const double sampleRate = SampleRate::noPreference().getValue();

        return UnitInfo ("Type", "Resamples signals to a different sample type.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                         IOKey::End,

                         // inputs
                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,   1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,     0.0,
                         IOKey::End);
    }    
    

    static OutputUnitType ar (InputUnitType const& input, 
                              BlockSize const& blockSize = BlockSize::noPreference(), 
                              SampleRate const& sampleRate = SampleRate::noPreference()) throw()
    {        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        
        Data data = { -1.0, -1.0 };
        
        return OutputUnitType::template createFromInputs<TypeInternal> (inputs, 
                                                                        data, 
                                                                        blockSize, 
                                                                        sampleRate);
    }
    
    static inline OutputUnitType kr (InputUnitType const& input) throw()
    {
        return ar (input, 
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }
};


template<class OutputSampleType, class InputSampleType>
class TypeUnit : public TypeUnitBase<OutputSampleType,InputSampleType>
{
};


// this provides quick returns for conversions to the same type of units

#define PLONK_TYPEUNIT_SAMEIOTYPES_DEFINE(SampleType)\
    template<> class TypeUnit<SampleType,SampleType>\
    {\
    public:\
        typedef UnitBase<SampleType> UnitType;\
        \
        static inline UnitType ar (UnitType const& input) throw() {\
            return input;\
        }\
        \
        static inline UnitType ar (UnitType const& input, BlockSize const& blockSize) throw() {\
            return TypeUnitBase<SampleType,SampleType>::ar (input, blockSize, SampleRate::noPreference());\
        }\
        \
        static inline UnitType ar (UnitType const& input, BlockSize const& blockSize, SampleRate const& sampleRate) throw() {\
            return TypeUnitBase<SampleType,SampleType>::ar (input, blockSize, sampleRate);\
        }\
    }

PLONK_TYPEUNIT_SAMEIOTYPES_DEFINE(float);
PLONK_TYPEUNIT_SAMEIOTYPES_DEFINE(double);
PLONK_TYPEUNIT_SAMEIOTYPES_DEFINE(int);
PLONK_TYPEUNIT_SAMEIOTYPES_DEFINE(short);
PLONK_TYPEUNIT_SAMEIOTYPES_DEFINE(Int24);


#endif // PLONK_TYPECONVERTCHANNEL_H