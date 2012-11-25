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

#ifndef PLONK_FILEPLAY_H
#define PLONK_FILEPLAY_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class FilePlayChannelInternal;

PLONK_CHANNELDATA_DECLARE(FilePlayChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    int numChannels;
};      

//------------------------------------------------------------------------------

/** File player generator. */
template<class SampleType>
class FilePlayChannelInternal
:   public ProxyOwnerChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(FilePlayChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(FilePlayChannelInternal,SampleType)  Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef ObjectArray<ChannelType>                                    ChannelArrayType;
    typedef FilePlayChannelInternal<SampleType>                         FilePlayInternal;
    typedef ProxyOwnerChannelInternal<SampleType,Data>                  Internal;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef NumericalArray<SampleType>                                  Buffer;

    FilePlayChannelInternal (Inputs const& inputs, 
                             Data const& data, 
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate,
                             ChannelArrayType& channels) throw()
    :   Internal (decideNumChannels (inputs, data), 
                  inputs, data, blockSize, sampleRate,
                  channels)
    {
        AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
        file.setOwner (this);
    }
    
    ~FilePlayChannelInternal()
    {
        AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
        file.setOwner (0);
    }
            
    Text getName() const throw()
    {
        return "File Play";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::AudioFileReader);
        return keys;
    }    
        
    void initChannel (const int channel) throw()
    {       
        if ((channel % this->getNumChannels()) == 0)
        {
            const AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
            this->setSampleRate (SampleRate::decide (file.getSampleRate(), this->getSampleRate()));
            buffer.setSize (this->getBlockSize().getValue() * file.getNumChannels(), false);
        }
        
        this->initProxyValue (channel, 0);
    }    
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {        
        AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
        
        plonk_assert (file.getOwner() == this);
        
        const int numChannels = this->getNumChannels();
        const int fileNumChannels = file.getNumChannels();
        const int bufferSize = this->getBlockSize().getValue() * fileNumChannels;
        buffer.setSize (bufferSize, false);
        
        const bool hitEOF = file.readFrames (buffer, true); // loop could be a flag but would need to check for a partial buffer
                        
        int channel;
        
        if (! hitEOF)
        {
            for (channel = 0; channel < numChannels; ++channel)
            {
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                SampleType* const outputSamples = outputBuffer.getArray();
                const int outputBufferLength = outputBuffer.length();        

                const SampleType* bufferSamples = buffer.getArray() + ((unsigned int)channel % (unsigned int)fileNumChannels);         

                for (int i = 0; i < outputBufferLength; ++i, bufferSamples += fileNumChannels)
                    outputSamples[i] = *bufferSamples;
            }
        }
        else
        {
            // zero
            for (channel = 0; channel < numChannels; ++channel)
            {
                Buffer& outputBuffer = this->getOutputBuffer (channel);
                outputBuffer.zero();
            }
            
            const int bufferAvailable = buffer.length();
            
            // copy partial buffer
            if (bufferAvailable > 0)
            {
                const int bufferFramesAvailable = bufferAvailable / fileNumChannels;
                
                for (channel = 0; channel < numChannels; ++channel)
                {
                    Buffer& outputBuffer = this->getOutputBuffer (channel);
                    SampleType* const outputSamples = outputBuffer.getArray();
                    const int outputBufferLength = plonk::min (bufferFramesAvailable, outputBuffer.length());        
                    
                    const SampleType* bufferSamples = buffer.getArray() + ((unsigned int)channel % (unsigned int)fileNumChannels);         
                    
                    for (int i = 0; i < outputBufferLength; ++i, bufferSamples += fileNumChannels)
                        outputSamples[i] = *bufferSamples;
                }
            }
        }
    }
    
private:
    Buffer buffer; // might need to use a signal...
    
    static const int decideNumChannels (Inputs const& inputs, Data const& data) throw()
    {
        if (data.numChannels > 0)
        {
            return data.numChannels;
        }
        else
        {
            const AudioFileReader& file = inputs[IOKey::AudioFileReader].asUnchecked<AudioFileReader>();
            return file.getNumChannels();
        }
    }
};

//------------------------------------------------------------------------------

/** Audio file player generator. 
 
 The AudioFileReader object passed in must not be used by any other code.
 
 The sample rate of the unit is by default set to the sample rate of the audio file.
 
 NB This should not be used directly in a real-time audio thread. It should
 be wrapped in a TaskUnit which buffers the audio on a separate thread.
 
 @par Factory functions:
 - ar (file, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=noPref)
 - kr (file, mul=1, add=0) 
 
 @par Inputs:
 - file: (audiofilereader, multi) the audio file reader to use
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset aded to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

  @ingroup GeneratorUnits */
template<class SampleType>
class FilePlayUnit
{
public:    
    typedef FilePlayChannelInternal<SampleType>         FilePlayInternal;
    typedef typename FilePlayInternal::Data             Data;
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ChannelInternal<SampleType,Data>            Internal;
    typedef UnitBase<SampleType>                        UnitType;
    typedef InputDictionary                             Inputs;
        
    
    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("FilePlay", "A signal player generator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,        IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::AudioFileReader,    Measure::None,
                         IOKey::Multiply,           Measure::Factor,    1.0,        IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,        IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,  IOLimit::Minimum,   Measure::Samples,           1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate, IOLimit::Minimum,   Measure::Hertz,             0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate audio file player. */
    static UnitType ar (AudioFileReader const& file, 
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {             
        if (file.isReady() && !file.isOwned())
        {
            Inputs inputs;
            inputs.put (IOKey::AudioFileReader, file);
            inputs.put (IOKey::Multiply, mul);
            inputs.put (IOKey::Add, add);
                            
            Data data = { { -1.0, -1.0 }, 0 };
            
            return UnitType::template proxiesFromInputs<FilePlayInternal> (inputs, 
                                                                           data, 
                                                                           preferredBlockSize, 
                                                                           preferredSampleRate);
        } 
        else return UnitType::getNull();
    }
    
//    /** Create a control rate audio file player. */
//    static UnitType kr (SignalType const& signal,
//                        RateUnitType const& rate, 
//                        UnitType const& mul = SampleType (1),
//                        UnitType const& add = SampleType (0)) throw()
//    {
//        return ar (signal, rate, mul, add, 
//                   BlockSize::getControlRateBlockSize(), 
//                   SampleRate::getControlRate());
//    }        
    
    /** A simple file player to handle buffering and sample rate conversion.
     This just adds a Task and Resample unit to the chain to buffer the 
     file playing on a background thread and resample the audio back to the
     default block size and sample rate. */
    class Simple
    {
    public:
        
        static UnitType ar (AudioFileReader const& file, 
                            const int blockSizeMultiplier = 0,
                            const int numBuffers = 8)
        {
            const DoubleVariable multiplier = blockSizeMultiplier <= 0 ? 
                                              (DoubleVariable (file.getSampleRate()) / SampleRate::getDefault()).ceil() * 2.0 :
                                              DoubleVariable (blockSizeMultiplier);

//            const DoubleVariable multiplier (4);
            
            return Resample::ar (Task::ar (FilePlayUnit::ar (file, 
                                                             SampleType (1), 
                                                             SampleType (0), 
                                                             BlockSize::getMultipleOfDefault (multiplier)), 
                                           numBuffers));
        }
    };
};

typedef FilePlayUnit<PLONK_TYPE_DEFAULT> FilePlay;


#endif // PLONK_FILEPLAY_H

