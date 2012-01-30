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

#ifndef PLONK_ENVELOPECHANNEL_H
#define PLONK_ENVELOPECHANNEL_H

#include "../channel/plonk_ChannelInternalCore.h"
#include "../plonk_GraphForwardDeclarations.h"


template<class SampleType> class EnvelopeChannelInternal;

CHANNELDATA_DECLARE(EnvelopeChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    
    LongLong samplesUntilTarget;
    SampleType currentLevel;
    SampleType grow;
    int targetPointIndex;
    
    bool done:1;
    bool deleteWhenDone:1;
};      



/** Envelope channel performs a series of breakpoints. */
template<class SampleType>
class EnvelopeChannelInternal 
:   public ChannelInternal<SampleType, CHANNELDATA_NAME(EnvelopeChannelInternal,SampleType)>
{
public:
    typedef CHANNELDATA_NAME(EnvelopeChannelInternal,SampleType)    Data;
    typedef ChannelBase<SampleType>                                 ChannelType;
    typedef EnvelopeChannelInternal<SampleType>                     EnvelopeInternal;
    typedef ChannelInternal<SampleType,Data>                        Internal;
    typedef ChannelInternalBase<SampleType>                         InternalBase;
    typedef UnitBase<SampleType>                                    UnitType;
    typedef InputDictionary                                         Inputs;
    typedef BreakpointsBase<SampleType>                             BreakpointsType;
    typedef BreakpointBase<SampleType>                              BreakpointType;

    EnvelopeChannelInternal (Inputs const& inputs, 
                             Data const& data, 
                             BlockSize const& blockSize,
                             SampleRate const& sampleRate) throw()
    :   Internal (inputs, data, blockSize, sampleRate)
    {
    }
    
    Text getName() const throw()
    {        
        return "Envelope";
    }        
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IOKey::Breakpoints);
        return keys;
    }    
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }        
    
    void initChannel (const int /*channel*/) throw()
    {
        Data& data = this->getState();
        const BreakpointsType& breakpoints = this->getInputAsBreakpoints (IOKey::Breakpoints);
        
        data.currentLevel = breakpoints.getStartLevel();
        this->initValue (data.currentLevel);
        
        this->setTargetPoint (0, 0);
    }    
    
    void setTargetPoint (const int index, const int samplesRemaining) throw()
    {
        Data& data = this->getState();
        
        if (index == data.targetPointIndex)
        {
            // sustain
            data.grow = SampleType (0);
            data.samplesUntilTarget = samplesRemaining;
        }
        else 
        {
            const BreakpointsType& breakpoints = this->getInputAsBreakpoints (IOKey::Breakpoints);

            plonk_assert (index >= 0);
            
            if(index >= breakpoints.getNumBreakpoints())
            {
                // finished...
                data.targetPointIndex = breakpoints.getNumBreakpoints();
                data.grow = SampleType (0);
                data.samplesUntilTarget = TypeUtility<LongLong>::getTypePeak();
                data.done = true;
            }
            else
            {
                // configure next transition
                data.targetPointIndex = index;
                const BreakpointType& targetBreakpoint = breakpoints.atUnchecked (index);
                
                const SampleType diff = targetBreakpoint.getTargetLevel() - data.currentLevel;
                data.samplesUntilTarget = targetBreakpoint.getTargetTime() * this->getSampleRate();
                data.grow = diff / data.samplesUntilTarget;
            }
        }
    }
    
    inline void nextTargetPoint(const bool gate, const int samplesRemaining) throw()
    {
        const Data& data = this->getState();
        const BreakpointsType& breakpoints = this->getInputAsBreakpoints (IOKey::Breakpoints);
        const BreakpointType& targetBreakpoint = breakpoints.atUnchecked (data.targetPointIndex);
        
        const int next = targetBreakpoint.getNext (gate);
        
        if (next == BreakpointType::This)
            this->setTargetPoint (data.targetPointIndex, samplesRemaining);
        else if (next == BreakpointType::Next)
            this->setTargetPoint (data.targetPointIndex + 1, samplesRemaining);
        else 
            this->setTargetPoint (next, samplesRemaining);
    }
            
    inline void processLinear (SampleType* const outputSamples, const int numSamples) throw()
    {
        Data& data = this->getState();

        for (int i = 0; i < numSamples; ++i)
        {
            outputSamples[i] = data.currentLevel;
            data.currentLevel += data.grow;
        }
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {                
        Data& data = this->getState();
        
        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();

        UnitType& gate (this->getInputAsUnit (IOKey::Control));
        const Buffer gateBuffer (gate.process (info, 0));
        const SampleType* gateSamples = gateBuffer.getArray();
        const int gateBufferLength = gateBuffer.length();        
        
        int samplesRemaining = outputBufferLength;
        
        if (gateBufferLength == 1)
        {
            while (samplesRemaining > 0)
            {
                const int samplesThisTime = int (plonk::min (LongLong (samplesRemaining), data.samplesUntilTarget));
                
                if (samplesThisTime > 0)
                {
                    processLinear (outputSamples, samplesThisTime);
                    
                    outputSamples += samplesThisTime;
                    samplesRemaining -= samplesThisTime;
                    data.samplesUntilTarget -= samplesThisTime;
                }
                else
                {           
                    const bool gate = *gateSamples >= SampleType (0.5);
                    this->nextTargetPoint (gate, samplesRemaining);
                }
            }
        }
        else if (gateBufferLength == outputBufferLength)
        {
            while (samplesRemaining > 0)
            {
                const int samplesThisTime = int (plonk::min (LongLong (samplesRemaining), data.samplesUntilTarget));
                
                if (samplesThisTime > 0)
                {
                    processLinear (outputSamples, samplesThisTime);
                    
                    outputSamples += samplesThisTime;
                    gateSamples += samplesThisTime;
                    samplesRemaining -= samplesThisTime;
                    data.samplesUntilTarget -= samplesThisTime;
                }
                else
                {    
                    const bool gate = *gateSamples >= SampleType (0.5);
                    
                    // find how many samples the gate stays in the same state
                    int count = 1;
                    while ((gate == (gateSamples[count] >= SampleType (0.5))) && (count <  samplesThisTime))
                        count++;
                    
                    this->nextTargetPoint (gate, count);
                }
            }            
        }
        else
        {
            double gatePosition = 0.0;
            const double gateIncrement = double (gateBufferLength) / double (outputBufferLength);
            
            while (samplesRemaining > 0)
            {
                const int samplesThisTime = int (plonk::min (LongLong (samplesRemaining), data.samplesUntilTarget));
                
                if (samplesThisTime > 0)
                {
                    processLinear (outputSamples, samplesThisTime);
                    
                    outputSamples += samplesThisTime;
                    gatePosition += gateIncrement * samplesThisTime;
                    samplesRemaining -= samplesThisTime;
                    data.samplesUntilTarget -= samplesThisTime;
                }
                else
                {                  
                    const bool gate = gateSamples[int (gatePosition)] >= SampleType (0.5);
                    
                    int count = 1;
                    double gatePositionCount = gatePosition + gateIncrement;
                    
                    while ((gate == (gateSamples[int (gatePositionCount)] >= SampleType (0.5))) && (count <  samplesThisTime))
                    {
                        count++;
                        gatePositionCount += gateIncrement;
                    }
                    
                    this->nextTargetPoint (gate, count);
                }
            }            
        }
        
        if (data.done && data.deleteWhenDone)
            info.setShouldDelete();
    }

private:
};



//------------------------------------------------------------------------------

/** Envelope unit performs a series of breakpoints. 
 @ingroup EnvelopeUnits */
template<class SampleType>
class EnvelopeUnit
{
public:    
    typedef EnvelopeChannelInternal<SampleType>     EnvelopeInternal;
    typedef typename EnvelopeInternal::Data         Data;
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternal<SampleType,Data>        Internal;
    typedef ChannelInternalBase<SampleType>         InternaBase;
    typedef UnitBase<SampleType>                    UnitType;
    typedef InputDictionary                         Inputs;
    typedef BreakpointsBase<SampleType>             BreakpointsType;

    static inline UnitInfos getInfo() throw()
    {
        const double blockSize = (double)BlockSize::getDefault().getValue();
        const double sampleRate = SampleRate::getDefault().getValue();

        return UnitInfo ("Envelope", "Performs a series of breakpoints.",
                         
                         // output
                         1, 
                         IOKey::Generic,     Measure::None,      IOInfo::NoDefault,  IOLimit::None, 
                         IOKey::End,
                         
                         // inputs
                         IOKey::Breakpoints,    Measure::None,
                         IOKey::Control,        Measure::NormalisedBipolar,     0.0,            IOLimit::Clipped, Measure::NormalisedBipolar,  -1.0, 1.0, 
                         IOKey::AutoDeleteFlag, Measure::Bool,                  IOInfo::True,   IOLimit::None,
                         IOKey::BlockSize,      Measure::Samples,               blockSize,      IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::SampleRate,     Measure::Hertz,                 sampleRate,     IOLimit::Minimum, Measure::Hertz,               0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate envelope generator. */
    static  UnitType ar (BreakpointsType const& breakpoints,
                         UnitType const& gate = SampleType (0),
                         const bool deleteWhenDone = true,
                         BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                         SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        plonk_assert (gate.getNumChannels() == 1);
        
        Inputs inputs;
        inputs.put (IOKey::Breakpoints, breakpoints);
        inputs.put (IOKey::Control, gate);
        
        Data data = { { -1.0, -1.0 }, 0, 0, 0, -1, false, deleteWhenDone };
        
        return UnitType::template createFromInputs<EnvelopeInternal> (inputs, 
                                                                      data, 
                                                                      preferredBlockSize, 
                                                                      preferredSampleRate);
    }
};

typedef EnvelopeUnit<PLONK_TYPE_DEFAULT> Envelope;




#endif // PLONK_ENVELOPECHANNEL_H


