/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_TABLE_H
#define PLONK_TABLE_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class TableChannelInternal;

PLONK_CHANNELDATA_DECLARE(TableChannelInternal,SampleType)
{    
    typedef typename TypeUtility<SampleType>::IndexType FrequencyType;

    ChannelInternalCore::Data base;
    FrequencyType currentPosition;
};      

//------------------------------------------------------------------------------

/** Wavetable oscillator. */
template<class SampleType>
class TableChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(TableChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(TableChannelInternal,SampleType)     Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef TableChannelInternal<SampleType>                            TableInternal;
    typedef ChannelInternal<SampleType,Data>                            Internal;
    typedef ChannelInternalBase<SampleType>                             InternalBase;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;
    typedef WavetableBase<SampleType>                                   WavetableType;
    
    typedef typename TypeUtility<SampleType>::IndexType         FrequencyType;
    typedef UnitBase<FrequencyType>                             FrequencyUnitType;
    typedef NumericalArray<FrequencyType>                       FrequencyBufferType;
    typedef InterpLinear<SampleType,FrequencyType>              InterpType;


    TableChannelInternal (Inputs const& inputs, 
                          Data const& data, 
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
            
    Text getName() const throw()
    {
        return "Table";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Wavetable, 
                             IOKey::Frequency);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new TableInternal (channelInputs, 
                                  this->getState(), 
                                  this->getBlockSize(), 
                                  this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {        
        const FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        
        this->setBlockSize (BlockSize::decide (frequencyUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (frequencyUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (frequencyUnit.getOverlap (channel));
        
        this->initValue (this->getState().currentPosition);
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {        
        Data& data = this->getState();
        const double sampleDuration = data.base.sampleDuration;

        FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        const FrequencyBufferType& frequencyBuffer (frequencyUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const FrequencyType* const frequencySamples = frequencyBuffer.getArray();
        const int frequencyBufferLength = frequencyBuffer.length();
        
        const WavetableType& table (this->getInputAsWavetable (IOKey::Wavetable));
        const SampleType* const tableSamples = table.getArray();
        const FrequencyType tableLength = FrequencyType (table.length());
        const FrequencyType table0 (0);
        const FrequencyType tableLengthOverSampleRate = FrequencyType (tableLength * sampleDuration); 
        
        FrequencyType currentPosition = data.currentPosition;
        int i;
        
        if (frequencyBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = InterpType::lookup (tableSamples, currentPosition);
                currentPosition += frequencySamples[i] * tableLengthOverSampleRate;
                
                if (currentPosition >= tableLength)
                    currentPosition -= tableLength;
                else if (currentPosition < table0)	
                    currentPosition += tableLength;                
            }                    
        }
        else if (frequencyBufferLength == 1)
        {
            const FrequencyType valueIncrement (frequencySamples[0] * tableLengthOverSampleRate);
            
            if (valueIncrement > table0)
            {
                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] = InterpType::lookup (tableSamples, currentPosition);
                    currentPosition += valueIncrement;
                    
                    if (currentPosition >= tableLength)
                        currentPosition -= tableLength;
                }            
            }
            else
            {
                for (i = 0; i < outputBufferLength; ++i) 
                {
                    outputSamples[i] = InterpType::lookup (tableSamples, currentPosition);
                    currentPosition += valueIncrement;
                    
                    if (currentPosition < table0)	
                        currentPosition += tableLength;                
                }            
            }
        }
        else
        {
            double frequencyPosition = 0.0;
            const double frequencyIncrement = double (frequencyBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = InterpType::lookup (tableSamples, currentPosition);
                currentPosition += frequencySamples[int (frequencyPosition)] * tableLengthOverSampleRate;
                
                if (currentPosition >= tableLength)
                    currentPosition -= tableLength;
                else if (currentPosition < table0)	
                    currentPosition += tableLength;                
                
                frequencyPosition += frequencyIncrement;
            }        
        }
        
        data.currentPosition = currentPosition;
    }
};

//------------------------------------------------------------------------------

#ifdef PLONK_USEPLINK

template<>
class TableChannelInternal<float> :   public ChannelInternal<float, TableProcessStateF>
{
public:
    typedef TableProcessStateF              Data;
    typedef ChannelBase<float>              ChannelType;
    typedef TableChannelInternal<float>     TableInternal;
    typedef ChannelInternal<float,Data>     Internal;
    typedef ChannelInternalBase<float>      InternalBase;
    typedef UnitBase<float>                 UnitType;
    typedef InputDictionary                 Inputs;
    typedef NumericalArray<float>           Buffer;
    typedef WavetableBase<float>            WavetableType;
    
    typedef float                           FrequencyType;
    typedef UnitBase<float>                 FrequencyUnitType;
    typedef NumericalArray<float>           FrequencyBufferType;
    
    enum Outputs { Output, NumOutputs };
    enum InputIndices  { Frequency, NumInputs };
    enum Buffers { OutputBuffer, FrequencyBuffer, TableBuffer, NumBuffers };
    
    typedef PlinkProcess<NumBuffers>        Process;
    
    TableChannelInternal (Inputs const& inputs, 
                          Data const& data, 
                          BlockSize const& blockSize,
                          SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
        plonk_staticassert (NumBuffers == (NumInputs + NumOutputs + 1));
        plonk_assert (Bits::isPowerOf2 (this->getInputAsWavetable (IOKey::Wavetable).length())); // wavetable must be a power of 2 length for Plink
        
        Process::init (&p, this, NumOutputs, NumInputs);
    }
    
    Text getName() const throw()
    {
        return "Table";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Wavetable, 
                             IOKey::Frequency);
        return keys;
    }    
    
    InternalBase* getChannel (const int index) throw()
    {
        const Inputs channelInputs = this->getInputs().getChannel (index);
        return new TableInternal (channelInputs, 
                                  this->getState(), 
                                  this->getBlockSize(), 
                                  this->getSampleRate());
    }
    
    void initChannel (const int channel) throw()
    {        
        const FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        
        this->setBlockSize (BlockSize::decide (frequencyUnit.getBlockSize (channel),
                                               this->getBlockSize()));
        this->setSampleRate (SampleRate::decide (frequencyUnit.getSampleRate (channel),
                                                 this->getSampleRate()));
        
        this->setOverlap (frequencyUnit.getOverlap (channel));
        
        this->initValue (this->getState().currentPosition);
    }    
    
    void process (ProcessInfo& info, const int channel) throw()
    {                
        FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        const FrequencyBufferType& frequencyBuffer (frequencyUnit.process (info, channel));
        const WavetableType& table (this->getInputAsWavetable (IOKey::Wavetable));

        p.buffers[0].bufferSize = this->getOutputBuffer().length();
        p.buffers[0].buffer     = this->getOutputSamples();
        p.buffers[1].bufferSize = frequencyBuffer.length();
        p.buffers[1].buffer     = frequencyBuffer.getArray();
        p.buffers[2].bufferSize = table.length();
        p.buffers[2].buffer     = table.getArray();
        
        plink_TableProcessF (&p, &this->getState());
    }
    
private:
    Process p;
};    

#endif // PLONK_USEPLINK

//------------------------------------------------------------------------------


/** Wavetable oscillator. 
 
 @par Factory functions:
 - ar (table, frequency=440, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - kr (table, frequency=440, mul=1, add=0) 
 
 @par Inputs:
 - table: (wavetable) the wavetable to use for the oscillator
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 
  @ingroup GeneratorUnits ControlUnits */
template<class SampleType>
class TableUnit
{
public:    
    typedef TableChannelInternal<SampleType>        TableInternal;
    typedef typename TableInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef WavetableBase<SampleType>               WavetableType;
    
    typedef typename TableInternal::FrequencyType         FrequencyType;
    typedef typename TableInternal::FrequencyUnitType     FrequencyUnitType;
    typedef typename TableInternal::FrequencyBufferType   FrequencyBufferType;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Table", "A wavetable oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Wavetable,  Measure::None,
                         IOKey::Frequency,  Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate wavetable oscillator. */
    static UnitType ar (WavetableType const& table, 
                        FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {             
        Inputs inputs;
        inputs.put (IOKey::Wavetable, table);
        inputs.put (IOKey::Frequency, frequency);
        inputs.put (IOKey::Multiply, mul);
        inputs.put (IOKey::Add, add);
                        
        Data data;
        Memory::zero (data);
        data.base.sampleRate = -1.0;
        data.base.sampleDuration = -1.0;
        
        return UnitType::template createFromInputs<TableInternal> (inputs, 
                                                                   data, 
                                                                   preferredBlockSize, 
                                                                   preferredSampleRate);
    }
    
    /** Create a control rate wavetable oscillator. */
    static UnitType kr (WavetableType const& table,
                        FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        return ar (table, frequency, mul, add, 
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }        
};

typedef TableUnit<PLONK_TYPE_DEFAULT> Table;

//------------------------------------------------------------------------------

/** Sine oscillator. 
 
 @par Factory functions:
 - ar (frequency=440, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - kr (frequency=440, mul=1, add=0) 
 
 @par Inputs:
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)
 
 @see TableUnit
 @ingroup GeneratorUnits ControlUnits */
template<class SampleType>
class SineUnit
{
public:    
    typedef TableChannelInternal<SampleType>        TableInternal;
    typedef typename TableInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef WavetableBase<SampleType>               WavetableType;
    typedef TableUnit<SampleType>                   TableType;
    
    typedef typename TableInternal::FrequencyType         FrequencyType;
    typedef typename TableInternal::FrequencyUnitType     FrequencyUnitType;
    typedef typename TableInternal::FrequencyBufferType   FrequencyBufferType;

    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Sine", "A wavetable-based sine oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,    Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Frequency,  Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,  -0.5, 0.5,
                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate sine oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {     
        return TableType::ar (WavetableType::sine8192(), frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate sine oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        return TableType::kr (WavetableType::sine8192(), frequency, mul, add);
    }        
};

typedef SineUnit<PLONK_TYPE_DEFAULT> Sine;

/** Harmonic sawtooth wave oscillator. 
 
 @par Factory functions:
 - ar (frequency=440, mul=1, add=0, numHarmonics=21, preferredBlockSize=default, preferredSampleRate=default)
 - kr (frequency=440, mul=1, add=0, numHarmonics=21) 
 
 @par Inputs:
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - numHarmonics: (int) the number of harmonics to generate
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @see TableUnit
 @ingroup GeneratorUnits */
template<class SampleType>
class HarmonicSawUnit
{
public:    
    typedef TableChannelInternal<SampleType>        TableInternal;
    typedef typename TableInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef WavetableBase<SampleType>               WavetableType;
    typedef TableUnit<SampleType>                   TableType;
    typedef NumericalArray<SampleType>              WeightsType;
    
    typedef typename TableInternal::FrequencyType         FrequencyType;
    typedef typename TableInternal::FrequencyUnitType     FrequencyUnitType;
    typedef typename TableInternal::FrequencyBufferType   FrequencyBufferType;

    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HarmonicSaw", "A wavetable-based sawtooth oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Frequency,      Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,       Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,            Measure::None,      0.0,        IOLimit::None,
                         IOKey::HarmonicCount,  Measure::Count,     21,         IOLimit::Minimum,   Measure::Count,             1,
                         IOKey::BlockSize,      Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,     Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate harmonic sawtooth oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {          
        const WavetableType table = (numHarmonics == 21) ? WavetableType::harmonicSaw() : WavetableType::harmonicSaw (8192, numHarmonics);
        return TableType::ar (table, frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate harmonic sawtooth oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21) throw()
    {
        const WavetableType table = (numHarmonics == 21) ? WavetableType::harmonicSaw() : WavetableType::harmonicSaw (8192, numHarmonics);
        return TableType::kr (table, frequency, mul, add);
    }        
};

typedef HarmonicSawUnit<PLONK_TYPE_DEFAULT> HarmonicSaw;


/** Harmonic square wave oscillator. 
 
 @par Factory functions:
 - ar (frequency=440, mul=1, add=0, numHarmonics=21, preferredBlockSize=default, preferredSampleRate=default)
 - kr (frequency=440, mul=1, add=0, numHarmonics=21) 
 
 @par Inputs:
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - numHarmonics: (int) the number of harmonics to generate
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 @see TableUnit
 @ingroup GeneratorUnits */
template<class SampleType>
class HarmonicSquareUnit
{
public:    
    typedef TableChannelInternal<SampleType>        TableInternal;
    typedef typename TableInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef WavetableBase<SampleType>               WavetableType;
    typedef TableUnit<SampleType>                   TableType;
    typedef NumericalArray<SampleType>              WeightsType;
    
    typedef typename TableInternal::FrequencyType         FrequencyType;
    typedef typename TableInternal::FrequencyUnitType     FrequencyUnitType;
    typedef typename TableInternal::FrequencyBufferType   FrequencyBufferType;

    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HarmonicSquare", "A wavetable-based square wave oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Frequency,      Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,       Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,            Measure::None,      0.0,        IOLimit::None,
                         IOKey::HarmonicCount,  Measure::Count,     21,         IOLimit::Minimum,   Measure::Count,             1,
                         IOKey::BlockSize,      Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,     Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate harmonic square wave oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {            
        const WavetableType table = (numHarmonics == 21) ? WavetableType::harmonicSquare() : WavetableType::harmonicSquare (8192, numHarmonics);
        return TableType::ar (table, frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate harmonic square wave oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21) throw()
    {
        const WavetableType table = (numHarmonics == 21) ? WavetableType::harmonicSquare() : WavetableType::harmonicSquare (8192, numHarmonics);
        return TableType::kr (table, frequency, mul, add);
    }        
};

typedef HarmonicSquareUnit<PLONK_TYPE_DEFAULT> HarmonicSquare;

/** Harmonic triangle wave oscillator.
 
 @par Factory functions:
 - ar (frequency=440, mul=1, add=0, numHarmonics=21, preferredBlockSize=default, preferredSampleRate=default)
 - kr (frequency=440, mul=1, add=0, numHarmonics=21)
 
 @par Inputs:
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - numHarmonics: (int) the number of harmonics to generate
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)
 
 @see TableUnit
 @ingroup GeneratorUnits */
template<class SampleType>
class HarmonicTriUnit
{
public:
    typedef TableChannelInternal<SampleType>        TableInternal;
    typedef typename TableInternal::Data            Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef WavetableBase<SampleType>               WavetableType;
    typedef TableUnit<SampleType>                   TableType;
    typedef NumericalArray<SampleType>              WeightsType;
    
    typedef typename TableInternal::FrequencyType         FrequencyType;
    typedef typename TableInternal::FrequencyUnitType     FrequencyUnitType;
    typedef typename TableInternal::FrequencyBufferType   FrequencyBufferType;
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HarmonicTri", "A wavetable-based triangle wave oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount,
                         IOKey::Generic,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Frequency,      Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,       Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,            Measure::None,      0.0,        IOLimit::None,
                         IOKey::HarmonicCount,  Measure::Count,     21,         IOLimit::Minimum,   Measure::Count,             1,
                         IOKey::BlockSize,      Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,     Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate harmonic triangle wave oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440),
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        const WavetableType table = (numHarmonics == 21) ? WavetableType::harmonicTri() : WavetableType::harmonicTri (8192, numHarmonics);
        return TableType::ar (table, frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate harmonic triangle wave oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21) throw()
    {
        const WavetableType table = (numHarmonics == 21) ? WavetableType::harmonicTri() : WavetableType::harmonicTri (8192, numHarmonics);
        return TableType::kr (table, frequency, mul, add);
    }        
};

typedef HarmonicTriUnit<PLONK_TYPE_DEFAULT> HarmonicTri;

#endif // PLONK_TABLE_H

