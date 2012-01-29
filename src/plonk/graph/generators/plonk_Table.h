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

#ifndef PLONK_TABLE_H
#define PLONK_TABLE_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class TableChannelInternal;

CHANNELDATA_DECLARE(TableChannelInternal,SampleType)
{    
    typedef typename TypeUtility<SampleType>::IndexType FrequencyType;

    ChannelInternalCore::Data base;
    FrequencyType currentPosition;
};      

//------------------------------------------------------------------------------

/** Wavetable oscillator. */
template<class SampleType>
class TableChannelInternal 
:   public ChannelInternal<SampleType, CHANNELDATA_NAME(TableChannelInternal,SampleType)>
{
public:

    typedef CHANNELDATA_NAME(TableChannelInternal,SampleType)   Data;
    typedef ChannelBase<SampleType>                             ChannelType;
    typedef TableChannelInternal<SampleType>                    TableInternal;
    typedef ChannelInternal<SampleType,Data>                    Internal;
    typedef ChannelInternalBase<SampleType>                     InternalBase;
    typedef UnitBase<SampleType>                                UnitType;
    typedef InputDictionary                                     Inputs;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef WavetableBase<SampleType>                           WavetableType;
    
    typedef typename TypeUtility<SampleType>::IndexType         FrequencyType;
    typedef UnitBase<FrequencyType>                             FrequencyUnitType;
    typedef NumericalArray<FrequencyType>                       FrequencyBufferType;


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
        const double sampleRate = data.base.sampleRate;
        
        FrequencyUnitType& frequencyUnit = ChannelInternalCore::getInputAs<FrequencyUnitType> (IOKey::Frequency);
        const FrequencyBufferType frequencyBuffer (frequencyUnit.process (info, channel));
        
        SampleType* const outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();
        
        const FrequencyType* const frequencySamples = frequencyBuffer.getArray();
        const int frequencyBufferLength = frequencyBuffer.length();
        
        const WavetableType& table (this->getInputAsWavetable (IOKey::Wavetable));
        const SampleType* const tableSamples = table.getArray();
        const FrequencyType tableLength = FrequencyType (table.length());
        
        double tableLengthOverSampleRate = tableLength / sampleRate;

        int i;
        
        if (frequencyBufferLength == outputBufferLength)
        {
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = plonk::lookup (tableSamples, data.currentPosition);
                data.currentPosition += frequencySamples[i] * tableLengthOverSampleRate;
                
                if (data.currentPosition >= tableLength)
                    data.currentPosition -= tableLength;
                else if (data.currentPosition < SampleType (0))	
                    data.currentPosition += tableLength;                
            }                    
        }
        else if (frequencyBufferLength == 1)
        {
            const FrequencyType valueIncrement (frequencySamples[0] * tableLengthOverSampleRate);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = plonk::lookup (tableSamples, data.currentPosition);
                data.currentPosition += valueIncrement;
                
                if (data.currentPosition >= tableLength)
                    data.currentPosition -= tableLength;
                else if (data.currentPosition < SampleType (0))	
                    data.currentPosition += tableLength;                
            }                    
        }
        else
        {
            double frequencyPosition = 0.0;
            const double frequencyIncrement = double (frequencyBufferLength) / double (outputBufferLength);
            
            for (i = 0; i < outputBufferLength; ++i) 
            {
                outputSamples[i] = plonk::lookup (tableSamples, data.currentPosition);
                data.currentPosition += frequencySamples[int (frequencyPosition)] * tableLengthOverSampleRate;
                
                if (data.currentPosition >= tableLength)
                    data.currentPosition -= tableLength;
                else if (data.currentPosition < SampleType (0))	
                    data.currentPosition += tableLength;                
                
                frequencyPosition += frequencyIncrement;
            }        
        }
        
    }
    
private:
};

//------------------------------------------------------------------------------

/** Wavetable oscillator. 
  @ingroup OscillatorUnits ControlUnits */
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
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Table", "A wavetable oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Signal,     Measure::None,      0.0,        IOLimit::None,
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
    
    /** Create an audio rate sawtooth oscillator. */
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
                        
        Data data = { { -1.0, -1.0 }, 0 };
        
        return UnitType::template createFromInputs<TableInternal> (inputs, 
                                                                   data, 
                                                                   preferredBlockSize, 
                                                                   preferredSampleRate);
    }
    
    /** Create a control rate sawtooth oscillator. */
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
 @see TableUnit
 @ingroup OscillatorUnits ControlUnits */
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

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("Sine", "A wavetable-based sine oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Signal,     Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::Frequency,  Measure::Hertz,     440.0,      IOLimit::Clipped,   Measure::SampleRateRatio,   0.0, 0.5,
                         IOKey::Multiply,   Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,        Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,  Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate, Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate sawtooth oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {     
        return TableType::ar (WavetableType::sine8192(), frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate sawtooth oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0)) throw()
    {
        return TableType::kr (WavetableType::sine8192(), frequency, mul, add);
    }        
};

typedef SineUnit<PLONK_TYPE_DEFAULT> Sine;

/** Harmonic sawtooth wave oscillator. 
 @see TableUnit
 @ingroup OscillatorUnits */
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

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HarmonicSaw", "A wavetable-based sawtooth oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Signal,     Measure::None,      0.0,        IOLimit::None,
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
    
    /** Create an audio rate sawtooth oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {          
        WeightsType weights = WeightsType::series (numHarmonics, 1, 1).reciprocal();
        return TableType::ar (WavetableType::harmonic (8192, weights).normalise(), frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate sawtooth oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21) throw()
    {
        WeightsType weights = WeightsType::series (numHarmonics, 1, 1).reciprocal();
        return TableType::kr (WavetableType::harmonic (8192, weights).normalise(), frequency, mul, add);
    }        
};

typedef HarmonicSawUnit<PLONK_TYPE_DEFAULT> HarmonicSaw;


/** Harmonic square wave oscillator. 
 @see TableUnit
 @ingroup OscillatorUnits */
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

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("HarmonicSquare", "A wavetable-based square wave oscillator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Signal,     Measure::None,      0.0,        IOLimit::None,
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
    
    /** Create an audio rate sawtooth oscillator. */
    static UnitType ar (FrequencyUnitType const& frequency = FrequencyType (440), 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {            
        WeightsType weights = WeightsType::series (numHarmonics, 1, 1).reciprocal() * WeightsType (SampleType (1), SampleType (0));
        return TableType::ar (WavetableType::harmonic (8192, weights).normalise(), frequency, mul, add, preferredBlockSize, preferredSampleRate);
    }
    
    /** Create a control rate sawtooth oscillator. */
    static UnitType kr (FrequencyUnitType const& frequency, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const int numHarmonics = 21) throw()
    {
        WeightsType weights = WeightsType::series (numHarmonics, 1, 1).reciprocal() * WeightsType (SampleType (1), SampleType (0));
        return TableType::kr (WavetableType::harmonic (8192, weights).normalise(), frequency, mul, add);
    }        
};

typedef HarmonicSquareUnit<PLONK_TYPE_DEFAULT> HarmonicSquare;


#endif // PLONK_TABLE_H

