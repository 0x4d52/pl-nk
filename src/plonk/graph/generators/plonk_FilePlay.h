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

#ifndef PLONK_FILEPLAY_H
#define PLONK_FILEPLAY_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"

template<class SampleType> class FilePlayChannelInternal;

PLONK_CHANNELDATA_DECLARE(FilePlayChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    int numChannels;
    int cueIndex;
    
    bool done:1;
    bool deleteWhenDone:1;
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
                  channels),
        zero (0)
    {
//        AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
//        file.setOwner (this);
    }
    
    ~FilePlayChannelInternal()
    {
//        AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
//        file.setOwner (0);
    }
            
    Text getName() const throw()
    {
        return "File Play";
    }       
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::AudioFileReader, IOKey::Loop);
        return keys;
    }    
        
    void initChannel (const int channel) throw()
    {       
        if ((channel % this->getNumChannels()) == 0)
        {
            const AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);

            double fileSampleRate = file.getSampleRate();
            
            if (fileSampleRate <= 0.0)
                fileSampleRate = file.getDefaultSampleRate();

            this->setSampleRate (SampleRate::decide (fileSampleRate, this->getSampleRate()));
            buffer.setSize (this->getBlockSize().getValue() * file.getNumChannels(), false);
        }
        
        this->initProxyValue (channel, 0);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        Data& data = this->getState();
        
        IntVariable& loopCount (this->template getInputAs<IntVariable> (IOKey::LoopCount));
        int channel;
        int offset = 0;
        const int blockSize = this->getBlockSize().getValue();
        int blockRemain = blockSize;

        while (offset < blockSize)
        {
            AudioFileReader& file = this->getInputAsAudioFileReader (IOKey::AudioFileReader);
            
            const int numChannels = this->getNumChannels();
            const int fileNumChannels = file.getNumChannels();
            int bufferSize = blockRemain * fileNumChannels;
            
            const LongLong numFrames = file.getNumFrames();
            LongLong filePosition = file.getFramePosition();
            bool willHitEOF = false;
            
            if (numFrames > 0)
            {
                LongLong framesRemaining = numFrames - filePosition;
                
                if (framesRemaining == 0)
                {
                    if ((loopCount.getValue() == 0) || (loopCount.getValue() > 1))
                    {
                        file.resetFramePosition();
                        data.cueIndex = 0;
                        
                        if (loopCount.getValue() > 1)
                            loopCount.setValue (loopCount.getValue() - 1);
                    }
                    
                    filePosition = file.getFramePosition();
                    framesRemaining = numFrames - filePosition;
                }
                
                if (framesRemaining <= LongLong (blockRemain))
                {
                    bufferSize = int (plonk::min (LongLong (bufferSize), framesRemaining * fileNumChannels));
                    willHitEOF = true;
                }
            }
            
            const AudioFileMetaData metaData = file.getMetaData();
            
            if (metaData.isNotNull())
            {
                const AudioFileCuePointArray cuePoints = metaData.getCuePoints();
                AudioFileCuePoint cue = cuePoints[data.cueIndex];
                
                bool renderToNextCue = false;
                
                if (cue.isNotNull())
                {
                    if (cue.getFramePosition (file.getSampleRate()) == filePosition)
                    {
                        this->update (Text::getMessageCuePoint(), Text (cue.getLabel()));
                        
                        ++data.cueIndex;
                        cue = cuePoints[data.cueIndex];
                        
                        if (cue.isNotNull())
                            renderToNextCue = true;
                    }
                    else
                    {
                        renderToNextCue = true;
                    }
                    
                    if (renderToNextCue)
                    {
                        const LongLong framesToNextCue = cue.getFramePosition (file.getSampleRate()) - filePosition;
                        
                        if (framesToNextCue < LongLong (bufferSize / fileNumChannels))
                        {
                            bufferSize = int (plonk::min (LongLong (bufferSize), framesToNextCue * fileNumChannels));
                            willHitEOF = false;
                        }
                    }
                }
            }
            
            buffer.setSize (bufferSize, false);
            file.readFrames (buffer, zero);
            const bool changedNumChannels = file.didNumChannelsChange();
            const bool audioFileChanged = file.didAudioFileChange();
            const bool hitEOF = file.didHitEOF();
            const int bufferAvailable = buffer.length();
            
            if ((bufferAvailable == 0) || data.done)
            {
                for (channel = 0; channel < numChannels; ++channel)
                {
                    Buffer& outputBuffer = this->getOutputBuffer (channel);
                    SampleType* const outputSamples = outputBuffer.getArray() + offset;
                    const int outputBufferLength = outputBuffer.length() - offset;
                    
                    for (int i = 0; i < outputBufferLength; ++i)
                        outputSamples[i] = SampleType (0);
                }
                
                offset = blockSize;
                blockRemain = 0;
            }
            else if (willHitEOF || hitEOF)
            {
                const int bufferFramesAvailable = bufferAvailable / fileNumChannels;
                
                for (channel = 0; channel < numChannels; ++channel)
                {
                    Buffer& outputBuffer = this->getOutputBuffer (channel);
                    SampleType* const outputSamples = outputBuffer.getArray() + offset;
                    const int outputBufferLength = outputBuffer.length() - offset;
                    const int outputLengthToWrite = plonk::min (bufferFramesAvailable, outputBufferLength);
                    
                    const SampleType* bufferSamples = buffer.getArray() + ((unsigned int)channel % (unsigned int)fileNumChannels);
                    
                    for (int i = 0; i < outputLengthToWrite; ++i, bufferSamples += fileNumChannels)
                        outputSamples[i] = *bufferSamples;
                }
                
                if ((loopCount.getValue() == 0) || (loopCount.getValue() > 1))
                {
                    file.resetFramePosition();
                    data.cueIndex = 0;
                    
                    if (loopCount.getValue() > 1)
                        loopCount.setValue (loopCount.getValue() - 1);
                }
                else if (!data.done)
                {
                    data.done = true;
                    this->update (Text::getMessageDone(), Dynamic::getNull());
                }
                
                offset += bufferFramesAvailable;
                blockRemain -= bufferFramesAvailable;
            }
            else 
            {                
                const int bufferFramesAvailable = bufferAvailable / fileNumChannels;
                
                for (channel = 0; channel < numChannels; ++channel)
                {
                    Buffer& outputBuffer = this->getOutputBuffer (channel);
                    SampleType* const outputSamples = outputBuffer.getArray() + offset;
                    const int outputBufferLength = outputBuffer.length() - offset;
                    const int outputLengthToWrite = plonk::min (bufferFramesAvailable, outputBufferLength);
                    
                    const SampleType* bufferSamples = buffer.getArray() + ((unsigned int)channel % (unsigned int)fileNumChannels);
                    
                    for (int i = 0; i < outputLengthToWrite; ++i, bufferSamples += fileNumChannels)
                        outputSamples[i] = *bufferSamples;
                }
                                
                offset += bufferFramesAvailable;
                blockRemain -= bufferFramesAvailable;
            }
            
            if (audioFileChanged)
                this->update (Text::getMessageAudioFileChanged(), file);
                
            if (changedNumChannels)
                this->update (Text::getMessageNumChannelsChanged(), IntVariable (fileNumChannels));
        }
        
        if (data.done && data.deleteWhenDone)
            info.setShouldDelete();
    }

private:
    Buffer buffer; // might need to use a signal...
    IntVariable zero;
    
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
 - ar (file, loopCount=0, mul=1, add=0, allowAutoDelete=true, preferredBlockSize=default, preferredSampleRate=noPref)
 - kr (file, loopCount=0, mul=1, add=0, allowAutoDelete=true)
 
 @par Inputs:
 - file: (audiofilereader, multi) the audio file reader to use
 - loopCount: (value) a control to tell the file player how many times to loop (0=infinite)
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset added to the output
 - allowAutoDelete: (bool) whether this unit can be caused to be deleted by the unit it contains
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
        
    
    static PLONK_INLINE_LOW UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();
        
        return UnitInfo ("FilePlay", "A signal player generator.",
                         
                         // output
                         ChannelCount::VariableChannelCount, 
                         IOKey::Generic,            Measure::None,      0.0,            IOLimit::None,
                         IOKey::End,
                         
                         // inputs
                         IOKey::AudioFileReader,    Measure::None,
                         IOKey::LoopCount,          Measure::Count,     0.0,            IOLimit::Minimum,   Measure::Count,                 0.0,
                         IOKey::Multiply,           Measure::Factor,    1.0,            IOLimit::None,
                         IOKey::Add,                Measure::None,      0.0,            IOLimit::None,
                         IOKey::AutoDeleteFlag,     Measure::Bool,      IOInfo::True,   IOLimit::None,
                         IOKey::BlockSize,          Measure::Samples,   blockSize,      IOLimit::Minimum,   Measure::Samples,               1.0,
                         IOKey::SampleRate,         Measure::Hertz,     sampleRate,     IOLimit::Minimum,   Measure::Hertz,                 0.0,
                         IOKey::End);
    }
    
    /** Create an audio rate audio file player. */
    static UnitType ar (AudioFileReader const& file,
                        IntVariable const& loopCount = 0,
                        UnitType const& mul = SampleType (1),
                        UnitType const& add = SampleType (0),
                        const bool deleteWhenDone = true,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {             
        if (file.isReady() && !file.isOwned())
        {            
            Inputs inputs;
            inputs.put (IOKey::AudioFileReader, file);
            inputs.put (IOKey::LoopCount, loopCount);
            inputs.put (IOKey::Multiply, mul);
            inputs.put (IOKey::Add, add);
                            
            Data data = { { -1.0, -1.0 }, file.getDefaultNumChannels(), 0, false, deleteWhenDone };
            
            return UnitType::template proxiesFromInputs<FilePlayInternal> (inputs, 
                                                                           data, 
                                                                           preferredBlockSize, 
                                                                           preferredSampleRate);
        } 
        else return UnitType::getNull();
    }
        
    /** A simple file player to handle buffering and sample rate conversion.
     This just adds a Task and Resample unit to the chain to buffer the 
     file playing on a background thread and resample the audio back to the
     default block size and sample rate. */
    class Simple
    {
    public:
        typedef InputTaskUnit<SampleType,Interp::Linear>        TaskType;
        typedef ResampleUnit<SampleType,Interp::Linear>         ResampleType;
        typedef typename ResampleType::RateType                 RateType;
        typedef typename ResampleType::RateUnitType             RateUnitType;
        
        static UnitType ar (AudioFileReader const& file,
                            RateUnitType const& rate = Math<RateUnitType>::get1(),
                            IntVariable const& loopCount = 0,
                            const int blockSizeMultiplier = 0,
                            const int numBuffers = 16)
        {
            double fileSampleRate = file.getSampleRate();
            
            if (fileSampleRate <= 0.0)
                fileSampleRate = file.getDefaultSampleRate();
            
            const DoubleVariable multiplier = blockSizeMultiplier <= 0 ? 
                                              (DoubleVariable (fileSampleRate) / SampleRate::getDefault()).ceil() * 2.0 :
                                              DoubleVariable (blockSizeMultiplier);
            
            UnitType play = FilePlayUnit::ar (file, loopCount,
                                              SampleType (1), SampleType (0),
                                              true,
                                              BlockSize::getMultipleOfDefault (multiplier));
            
            UnitType task = TaskType::ar (play, numBuffers);
            
            return ResampleType::ar (task, rate);
        }
        
        class HQ
        {
        public:
            typedef InputTaskUnit<SampleType,Interp::Lagrange3>     TaskType;
            typedef ResampleUnit<SampleType,Interp::Lagrange3>      ResampleType;
            typedef typename ResampleType::RateType                 RateType;
            typedef typename ResampleType::RateUnitType             RateUnitType;
            
            static UnitType ar (AudioFileReader const& file,
                                RateUnitType const& rate = Math<RateUnitType>::get1(),
                                IntVariable const& loopCount = 0,
                                const int blockSizeMultiplier = 0,
                                const int numBuffers = 16)
            {
                double fileSampleRate = file.getSampleRate();
                
                if (fileSampleRate <= 0.0)
                    fileSampleRate = file.getDefaultSampleRate();

                const DoubleVariable multiplier = blockSizeMultiplier <= 0 ?
                                                  (DoubleVariable (fileSampleRate) / SampleRate::getDefault()).ceil() * 2.0 :
                                                  DoubleVariable (blockSizeMultiplier);
                
                UnitType play = FilePlayUnit::ar (file, loopCount,
                                                  SampleType (1), SampleType (0),
                                                  true,
                                                  BlockSize::getMultipleOfDefault (multiplier));
                
                UnitType task = TaskType::ar (play, numBuffers);
                
                return ResampleType::ar (task, rate);
            }
        };

    };
};

typedef FilePlayUnit<PLONK_TYPE_DEFAULT> FilePlay;


#endif // PLONK_FILEPLAY_H

