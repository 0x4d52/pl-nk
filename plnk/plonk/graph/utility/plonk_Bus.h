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

#ifndef PLONK_BUS_H
#define PLONK_BUS_H

#include "../plonk_GraphForwardDeclarations.h"
#include "../../containers/plonk_DynamicContainer.h"

#include "../utility/plonk_ProcessInfo.h"
#include "../utility/plonk_BlockSize.h"
#include "../../core/plonk_SmartPointer.h"
#include "../utility/plonk_TimeStamp.h"

template<class SampleType>
class BusBufferInternal : public SmartPointer,
                          public BlockSize::Receiver // what about samplerate changes?
{
public:    
    typedef NumericalArray<SampleType>                                          Buffer;
    typedef BusBuffer<SampleType>                                               BusType;
    typedef Dictionary<BusType>                                                 BusDictionary;
    typedef typename BinaryOpFunctionsHelper<SampleType>::BinaryOpFunctionsType BinaryOpFunctionsType;    

    BusBufferInternal() throw()
    {
        this->bufferSize.addReceiver(this);
        this->writeBlockSize.addReceiver(this);
    }
    
    BusBufferInternal(BlockSize const& bufferSizeToUse,
                      BlockSize const& writeBlockSizeToUse,
                      SampleRate const& sampleRateToUse) throw()
    :   bufferSize (bufferSizeToUse),
        writeBlockSize (writeBlockSizeToUse),
        sampleRate (sampleRateToUse),
        buffer (Buffer::newClear (bufferSize.getValue())),
        bufferStartTime (TimeStamp::getZero()),
        bufferEndTime (bufferStartTime + TimeStamp::fromSamples (bufferSize.getValue(), sampleRate)),
        latestValidTime (TimeStamp::getZero()),
        readDiff (0.0),
        firstWriteSize (0)
    {
        this->bufferSize.addReceiver(this);
        this->writeBlockSize.addReceiver(this);
    } 
    
    ~BusBufferInternal()
    {
        this->bufferSize.removeReceiver(this);
        this->writeBlockSize.removeReceiver(this);
    }
    
    void changed (BlockSize::Sender const& source, Text const& message, Dynamic const& payload) throw()
    {
        (void)message;
        (void)payload;
        
        BlockSize blockSizeSource = static_cast<BlockSize> (source);
        
        if (blockSizeSource == bufferSize)
        {
            plonk_assert (bufferSize.getValue() > buffer.length()); // code assumes the buffer will only grow at the moment
            
            buffer.setSize (bufferSize.getValue(), true); // copies existing data and zeros extra space too
            bufferEndTime = bufferStartTime + TimeStamp::fromSamples (bufferSize.getValue(), sampleRate);
        }
        else if (blockSizeSource == writeBlockSize)
        {
            // nothing
        }
    }
    
    PLONK_INLINE_LOW void growBufferSize() throw()                            { bufferSize.setValue (bufferSize.getValue() * 2); }
    PLONK_INLINE_LOW BlockSize getBufferSize() const throw()                  { return bufferSize; }
    PLONK_INLINE_LOW BlockSize getWriteBlockSize() const throw()              { return writeBlockSize; }
    PLONK_INLINE_LOW SampleRate getSampleRate() const throw()                 { return sampleRate; }
    PLONK_INLINE_LOW double getDuration() const throw()                       { return (bufferEndTime - bufferStartTime).getValue(); }
    PLONK_INLINE_LOW const TimeStamp& getLatestValidTime() const throw()      { return latestValidTime; }
    PLONK_INLINE_LOW const TimeStamp getEarliestValidTime() const throw()     { return latestValidTime - this->getDuration(); }
    Text getLabel() const throw()                                   { return identifier; }
    void setLabel (Text const& newId) throw()                       { identifier = newId; }

    SampleType getPeak() const throw()
    {
        return buffer.findMaximumAbs();
    }
    
    SampleType getMean() const throw()
    {
        return buffer.findMeanAbs();
    }
    
    SampleType getRMS() const throw()
    {
        return buffer.findRMS();
    }
    
    void write (TimeStamp writeStartTime,
                const int numWriteSamples,
                const SampleType* sourceData) throw()
    {
        const int currentBufferSize = bufferSize.getValue();
        const double currentSampleRate = sampleRate.getValue();
        
        SampleType* const bufferSamples = buffer.getArray();
        
        int numSamplesRemaining = numWriteSamples;
        
        bool shouldMix = false;
        
        if ((latestValidTime - TimeStamp::fromSamples (numWriteSamples,
                                                       currentSampleRate)) >= writeStartTime)
        {
            shouldMix = true;
            
            // if mixing on a bus the write size must match the write size set for this bus on this block
            plonk_assert (numWriteSamples == firstWriteSize);
        }
        else
        {
            // keep a record of the size used to write the first block at this time
            firstWriteSize = writeBlockSize.getValue();
        }
        
        while (numSamplesRemaining > 0)
        {            
            if (writeStartTime >= bufferEndTime)
            {
                bufferStartTime = bufferEndTime;
                bufferEndTime = bufferStartTime + TimeStamp::fromSamples (currentBufferSize,
                                                                          currentSampleRate);
            }

            const TimeStamp bufferOffsetTime = writeStartTime - bufferStartTime;
            int bufferOffsetSamples = int (bufferOffsetTime.toSamples (currentSampleRate) + 0.5);
            
            if (bufferOffsetSamples >= currentBufferSize)
                bufferOffsetSamples -= currentBufferSize;
            
            if (bufferOffsetSamples >= currentBufferSize)
            {
                bufferStartTime = writeStartTime;
                bufferEndTime = bufferStartTime + TimeStamp::fromSamples (currentBufferSize,
                                                                          currentSampleRate);
                bufferOffsetSamples = 0;
                buffer.zero();
            }
            
            const int bufferSamplesRemaining = currentBufferSize - bufferOffsetSamples;
            const int samplesThisTime = plonk::min (numSamplesRemaining,
                                                    bufferSamplesRemaining);
            
//            printf ("write-> %d samples from %d (to %d)\n", samplesThisTime, bufferOffsetSamples, bufferOffsetSamples + samplesThisTime);
            
            if (shouldMix)
            {
                NumericalArrayBinaryOp<SampleType, BinaryOpFunctionsType::addop>::calcNN (bufferSamples + bufferOffsetSamples,
                                                                                          bufferSamples + bufferOffsetSamples,
                                                                                          sourceData,
                                                                                          samplesThisTime);
            }
            else
            {
                NumericalArray<SampleType>::copyData (bufferSamples + bufferOffsetSamples,
                                                      sourceData,
                                                      samplesThisTime);
            }
            
            numSamplesRemaining -= samplesThisTime;
            sourceData += samplesThisTime;
            
            const double timestampOffset = TimeStamp::fromSamples (samplesThisTime,
                                                                   currentSampleRate);
            writeStartTime += timestampOffset;
            latestValidTime = writeStartTime;
        }
    }

    void read (TimeStamp& readStartTime,
               const int numReadSamples, 
               SampleType* destData) throw()
    {                                
        const int currentBufferSize = bufferSize.getValue();
        const double currentSampleRate = sampleRate.getValue();
        
        const TimeStamp duration = TimeStamp::fromSamples (numReadSamples, currentSampleRate);
        TimeStamp readEndTime;
        
        if (readStartTime < TimeStamp::getZero())
        {
            readEndTime = latestValidTime;
            readStartTime = readEndTime - duration;
        }
        else
        {
            readEndTime = readStartTime + duration;
        }

        const TimeStamp earliestValidTime = this->getEarliestValidTime();
        const double diff = (latestValidTime - readEndTime).getValue();
        
        if ((readStartTime >= earliestValidTime) && 
            (readStartTime < latestValidTime) && 
            (readEndTime <= latestValidTime))
        {
            int bufferOffsetSamples = int ((readStartTime - bufferStartTime).toSamples (currentSampleRate) + 0.5);
            
            if (bufferOffsetSamples < 0)
                bufferOffsetSamples += currentBufferSize - 1; // -1 due to being rounded in the wrong direction above when negative
            
            plonk_assert (bufferOffsetSamples < currentBufferSize);
                            
            SampleType* const bufferSamples = buffer.getArray();
            
//            printf ("<--read %d samples from %d (to %d)\n", numReadSamples, bufferOffsetSamples, bufferOffsetSamples + numReadSamples);
                
            int numSamplesRemaining = numReadSamples;
            
            while (numSamplesRemaining > 0)
            {
                const int bufferSamplesRemaining = currentBufferSize - bufferOffsetSamples;
                const int samplesThisTime = plonk::min (numSamplesRemaining, 
                                                        bufferSamplesRemaining);
                
                NumericalArray<SampleType>::copyData (destData, bufferSamples + bufferOffsetSamples, samplesThisTime);                    
                numSamplesRemaining -= samplesThisTime;
                
                if (numSamplesRemaining > 0)
                {
                    destData += samplesThisTime;
                    bufferOffsetSamples = 0;
                }
            }                                  
            
            readStartTime = readEndTime; // for return
        }
        else
        {
            // zero this time and check if we need to enlarge the buffer
            
            if ((diff > 0.0) && (diff >= readDiff))
                this->growBufferSize();
            
            NumericalArray<SampleType>::zeroData (destData, numReadSamples);
        }

        readDiff = diff; // keep track of the read difference to determine if we're actually getting anywhere
    }
        

private:
    BlockSize bufferSize;       // size of the circular buffer
    BlockSize writeBlockSize;   // estimated size of the write operations
    SampleRate sampleRate;      // sample rate of the audio
    Buffer buffer;              // the circular buffer
    TimeStamp bufferStartTime;  // timestamp of the first sample in the buffer
    TimeStamp bufferEndTime;    // calculated timestamp of the sample past the end of the buffer but this may not be where we have written to yet
    TimeStamp latestValidTime;  // the latest time that has been written to the bus
    double readDiff;
    int firstWriteSize;         // size of the actual first write
    Text identifier;            // named ID for the buffer
};


/** Enables the storage of globally accessible buffers to be used as signal busses.
 This stores a time stamped circular buffer such that BusRead units can read
 from the bus at "any" block size and get enough data to fill their output. 
 Of course this depends on the buffer size being large enough (although the
 buffer should automatically resize if you try to read in larger chunks
 than the current buffer size allows). */
template<class SampleType>
class BusBuffer : public SmartPointerContainer<BusBufferInternal<SampleType> >
{
public:    
    typedef BusBufferInternal<SampleType>   Internal;
    typedef SmartPointerContainer<Internal> Base;
    typedef NumericalArray<SampleType>      Buffer;    
    typedef BusBuffer<SampleType>           BusType;
    typedef Dictionary<BusType>             BusDictionary;
    
    BusBuffer() throw()
    :   Base (new Internal())
    {
    }            
        
    explicit BusBuffer (Text const& name) throw()
    :   Base (getBus (name))
    {
    }
    
    explicit BusBuffer (const int number) throw()
    :   Base (getBus (Text (number)))
    {
    }
    
    explicit BusBuffer (const char* name) throw()
    :   Base (getBus (Text (name)))
    {
    }
    
    BusBuffer (BlockSize const& bufferSize, 
               BlockSize const& writeBlockSize, 
               SampleRate const& sampleRate) throw()
    :   Base (new Internal (bufferSize, writeBlockSize, sampleRate))
    {
    }        
    
    explicit BusBuffer (Internal* internal) throw()
    :   Base (internal)
    {
    }
    
    BusBuffer (BusBuffer const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}        
            
    static const BusBuffer& getNull() throw()
	{
		static BusBuffer null;
		return null;
	}	                    
    
    PLONK_INLINE_LOW void growBufferSize() throw()                            { this->getInternal()->growBufferSize(); }
    PLONK_INLINE_LOW const BlockSize getBufferSize() const throw()            { return this->getInternal()->getBufferSize(); }
    PLONK_INLINE_LOW BlockSize getBufferSize() throw()                        { return this->getInternal()->getBufferSize(); }
    PLONK_INLINE_LOW const BlockSize getWriteBlockSize() const throw()        { return this->getInternal()->getWriteBlockSize(); }
    PLONK_INLINE_LOW BlockSize getWriteBlockSize() throw()                    { return this->getInternal()->getWriteBlockSize(); }
    PLONK_INLINE_LOW const SampleRate getSampleRate() const throw()           { return this->getInternal()->getSampleRate(); }
    PLONK_INLINE_LOW SampleRate getSampleRate() throw()                       { return this->getInternal()->getSampleRate(); }
    PLONK_INLINE_LOW double getDuration() throw()                             { return this->getInternal()->getDuration(); }
    PLONK_INLINE_LOW const TimeStamp& getLatestValidTime() const throw()      { return this->getInternal()->getLatestValidTime(); }
    PLONK_INLINE_LOW const TimeStamp  getEarliestValidTime() const throw()    { return this->getInternal()->getEarliestValidTime(); }
    PLONK_INLINE_LOW Text getLabel() const throw()                            { return this->getInternal()->getLabel(); }
    PLONK_INLINE_LOW void setLabel(Text const& newId) throw()                 { this->getInternal()->setLabel (newId); }

    /** Write data with a given time stampe start to the bus. */
    PLONK_INLINE_LOW void write (TimeStamp const& timeStamp, const int numSamples, const SampleType* sourceData) throw()
    {
        this->getInternal()->write (timeStamp, numSamples, sourceData);
    }
    
    /** Read data from the bus with a given time stamp. */
    PLONK_INLINE_LOW void read (TimeStamp& timeStamp, const int numSamples, SampleType* destData) throw()
    {
        this->getInternal()->read (timeStamp, numSamples, destData);
    }
    
    PLONK_INLINE_LOW SampleType getPeak() const throw()
    {
        return this->getInternal()->getPeak();
    }
    
    PLONK_INLINE_LOW SampleType getMean() const throw()
    {
        return this->getInternal()->getMean();
    }
    
    PLONK_INLINE_LOW SampleType getRMS() const throw()
    {
        return this->getInternal()->getRMS();
    }
    
    Text getName() const throw()
    {
        return getBusDictionary().keyForValue (*this);
    }

    static BusBuffer getBus (Text const& name) throw()
    {
        BusDictionary& dictionary = getBusDictionary();
        
        if (dictionary.getKeys().contains (name) == false)
        {
            BusBuffer newBusBuffer (BusBuffer::getDefaultBufferSize(), 
                                    BlockSize::getDefault(), 
                                    SampleRate::getDefault());
            dictionary.put (name, newBusBuffer);  
            return newBusBuffer;
        }        
        
        return dictionary[name];
    }
    
    static void addBus (Text const& name, 
                        BlockSize const& bufferSize = BusBuffer::getDefaultBufferSize(),
                        BlockSize const& writeBlockSize = BlockSize::getDefault(),
                        SampleRate const& sampleRate = SampleRate::getDefault()) throw()
    {
        BusDictionary& dictionary = getBusDictionary();
        
        plonk_assert (dictionary.getKeys().contains (name) == false);

        BusBuffer newBusBuffer (bufferSize, writeBlockSize, sampleRate);
        dictionary.put (name, newBusBuffer);
    }
    
    static void removeBus (Text const& name) throw()
    {
        getBusDictionary().remove (name);
    }
        
    static BlockSize getDefaultBufferSize() throw()
    {
        static BlockSize defaultBufferSize (4096);
        return defaultBufferSize;
    }    
    
    int getTypeCode() const throw()
    {
        return TypeUtility<BusBuffer>::getTypeCode();
    }
    
    int getSampleTypeCode() const throw()
    {
        return TypeUtility<SampleType>::getTypeCode();
    }            
    
    PLONK_OBJECTARROWOPERATOR(BusBuffer);

private:    
    static BusDictionary& getBusDictionary() throw()
    {
        static BusDictionary dictionary;
        return dictionary;
    }    
    
    BusBuffer (const double) throw();
    BusBuffer (const float) throw();
};




#endif // PLONK_BUS_H

