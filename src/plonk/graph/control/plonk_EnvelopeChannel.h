/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

PLONK_CHANNELDATA_DECLARE(EnvelopeChannelInternal,SampleType)
{    
    ChannelInternalCore::Data base;
    
    ShapeState<SampleType> shapeState;
    int targetPointIndex;

    bool prevGate:1;
    bool done:1;
    bool deleteWhenDone:1;
};      



/** Envelope channel performs a series of breakpoints. */
template<class SampleType>
class EnvelopeChannelInternal 
:   public ChannelInternal<SampleType, PLONK_CHANNELDATA_NAME(EnvelopeChannelInternal,SampleType)>
{
public:
    typedef PLONK_CHANNELDATA_NAME(EnvelopeChannelInternal,SampleType)  Data;
    typedef ChannelBase<SampleType>                                     ChannelType;
    typedef EnvelopeChannelInternal<SampleType>                         EnvelopeInternal;
    typedef ChannelInternal<SampleType,Data>                            Internal;
    typedef ChannelInternalBase<SampleType>                             InternalBase;
    typedef UnitBase<SampleType>                                        UnitType;
    typedef InputDictionary                                             Inputs;
    typedef BreakpointsBase<SampleType>                                 BreakpointsType;
    typedef BreakpointBase<SampleType>                                  BreakpointType;

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
        const IntArray keys (IOKey::Breakpoints, IOKey::Gate);
        return keys;
    }    
    
    InternalBase* getChannel (const int /*index*/) throw()
    {
        return this;
    }        
    
    void initChannel (const int channel) throw()
    {
        Data& data = this->getState();
        const BreakpointsType& breakpoints = this->getInputAsBreakpoints (IOKey::Breakpoints);
        
//        const UnitType& gate = this->getInputAsUnit (IOKey::Gate);
        plonk_assert (this->getInputAsUnit (IOKey::Gate).getOverlap (channel) == Math<DoubleVariable>::get1());

        data.shapeState.currentLevel = breakpoints.getStartLevel();
        this->initValue (data.shapeState.currentLevel);
        
        this->setTargetPoint (-1, 0);
    }    
    
    void setTargetPoint (const int index, const int samplesRemaining) throw()
    {
        Data& data = this->getState();
        
        if ((index != -1) && (index == data.targetPointIndex)) // ?? ugh can't be right
        {
            // sustain
            data.shapeState.grow.u.norm = 0;
            data.shapeState.stepsToTarget = samplesRemaining;
        }
        else 
        {
            const BreakpointsType& breakpoints = this->getInputAsBreakpoints (IOKey::Breakpoints);
            
            if (index < 0)
            {
                // start
                data.targetPointIndex = -1;
                data.shapeState.shapeType = Shape::Linear;
                data.shapeState.curve = 0.f;
                data.shapeState.grow.u.norm = SampleType (0);
                data.shapeState.stepsToTarget = samplesRemaining;
            }
            else if (index >= breakpoints.getNumBreakpoints())
            {
                // finished...
                data.targetPointIndex = breakpoints.getNumBreakpoints();                
                Shape::sustain (data.shapeState);
                
                data.done = true;
                
                this->update (Text::getMessageDone(), Dynamic::getNull());
            }
            else
            {
                // configure next transition
                data.targetPointIndex = index;
                const BreakpointType& targetBreakpoint = breakpoints.atUnchecked (index);
                
                const Shape& shape = targetBreakpoint.getShape();
                data.shapeState.shapeType = shape.getType();
                data.shapeState.curve = shape.getCurve();
                data.shapeState.targetLevel = targetBreakpoint.getTargetLevel();
                data.shapeState.stepsToTarget = targetBreakpoint.getTargetTime() * data.base.sampleRate;
                Shape::initShape (data.shapeState);
            }
        }        
    }
    
    inline void nextTargetPoint (const bool gate, const int samplesRemaining) throw()
    {
        const Data& data = this->getState();
        const BreakpointsType& breakpoints = this->getInputAsBreakpoints (IOKey::Breakpoints);
        
        const int next = (data.targetPointIndex < 0) ? 0 : breakpoints.atUnchecked (data.targetPointIndex).getNext (gate);
        
        if (next == BreakpointType::This)
            this->setTargetPoint (data.targetPointIndex, samplesRemaining);
        else if (next == BreakpointType::Next)
            this->setTargetPoint (data.targetPointIndex + 1, samplesRemaining);
        else 
            this->setTargetPoint (next, samplesRemaining);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        Data& data = this->getState();
        
        SampleType* outputSamples = this->getOutputSamples();
        const int outputBufferLength = this->getOutputBuffer().length();

        UnitType& gate (this->getInputAsUnit (IOKey::Gate));
        const Buffer& gateBuffer (gate.process (info, 0));
        const SampleType* const gateSamples = gateBuffer.getArray();
        const int gateBufferLength = gateBuffer.length();        
        
        bool currGate;
        
        if (gateBufferLength == 1) // most efficient
        {
            int samplesRemaining = outputBufferLength;
            currGate = gateSamples[0] >= SampleType (0.5);
            
            if (currGate != data.prevGate)
                this->nextTargetPoint (currGate, samplesRemaining);
            
            while (samplesRemaining > 0)
            {
                const int samplesThisTime = int (plonk::min (LongLong (samplesRemaining), data.shapeState.stepsToTarget));
                
                if (samplesThisTime > 0)
                {
                    Shape::processShape (data.shapeState, outputSamples, samplesThisTime);
                                        
                    outputSamples += samplesThisTime;
                    samplesRemaining -= samplesThisTime;
                }
                
                if (data.shapeState.stepsToTarget == 0)
                    this->nextTargetPoint (currGate, samplesRemaining);
            }
            
            data.prevGate = currGate;
        }
        else if (gateBufferLength == outputBufferLength)
        {
            for (int i = 0; i < outputBufferLength; ++i)
            {
                currGate = gateSamples[i] >= SampleType (0.5);
                
                if (currGate != data.prevGate)
                    this->nextTargetPoint (currGate, 1);
                    
                Shape::processShape (data.shapeState, outputSamples++, 1);
                
                if (data.shapeState.stepsToTarget == 0)
                    this->nextTargetPoint (currGate, 1);
                
                data.prevGate = currGate;
            }
        }
        else
        {            
            double gatePosition = 0.0;
            const double gateIncrement = double (gateBufferLength) / double (outputBufferLength);
            
            for (int i = 0; i < outputBufferLength; ++i)
            {
                currGate = gateSamples[int (gatePosition)] >= SampleType (0.5);
                
                if (currGate != data.prevGate)
                    this->nextTargetPoint (currGate, 1);
                
                Shape::processShape (data.shapeState, outputSamples++, 1);
                    
                if (data.shapeState.stepsToTarget == 0)
                    this->nextTargetPoint (currGate, 1);
                
                data.prevGate = currGate;
                gatePosition += gateIncrement;
            }
        }
        
        if (data.done && data.deleteWhenDone)
            info.setShouldDelete();
    }

private:
};



//------------------------------------------------------------------------------

/** Envelope unit performs a series of breakpoints. 
 
 [args-todo]
 - breakpoints: (breakpoints) the series of breakpoints to perform.
 - gate: (unit) controls the envelope
 - allowAutoDelete: (bool) whether this unit can be casued to be deleted by the unit it contains
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)

 
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
                         IOKey::Gate,           Measure::NormalisedBipolar,     0.0,            IOLimit::Clipped, Measure::NormalisedBipolar,  -1.0, 1.0,
                         IOKey::AutoDeleteFlag, Measure::Bool,                  IOInfo::True,   IOLimit::None,
                         IOKey::BlockSize,      Measure::Samples,               blockSize,      IOLimit::Minimum, Measure::Samples,             1.0,
                         IOKey::SampleRate,     Measure::Hertz,                 sampleRate,     IOLimit::Minimum, Measure::Hertz,               0.0,
                         IOKey::End);
    }    
    
    /** Create an audio rate envelope generator. */
    static UnitType ar (BreakpointsType const& breakpoints,
                        UnitType const& gate = SampleType (1),
                        const bool deleteWhenDone = true,
                        BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {        
        plonk_assert (gate.getNumChannels() == 1);
        
        Inputs inputs;
        inputs.put (IOKey::Breakpoints, breakpoints);
        inputs.put (IOKey::Gate, gate);
        
        Data data = { { -1.0, -1.0 },
            { 0, 0, 0, 0,
              0, 0, 0, 0, // coeffs
              Shape::Linear, 0.f },
            -1,
            false, false, deleteWhenDone };
        
        return UnitType::template createFromInputs<EnvelopeInternal> (inputs, 
                                                                      data, 
                                                                      preferredBlockSize, 
                                                                      preferredSampleRate);
    }
    
    /** Create a control rate envelope generator. */
    static inline UnitType kr (BreakpointsType const& breakpoints,
                               UnitType const& gate = SampleType (1),
                               const bool deleteWhenDone = true) throw()
    {
        return ar (breakpoints, gate, deleteWhenDone,
                   BlockSize::getControlRateBlockSize(), 
                   SampleRate::getControlRate());
    }

};

typedef EnvelopeUnit<PLONK_TYPE_DEFAULT> Envelope;




#endif // PLONK_ENVELOPECHANNEL_H


