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

#ifndef PLONK_AUDIOHOSTBASE_H
#define PLONK_AUDIOHOSTBASE_H

template<class SampleType>
class AudioHostBase
{
public:
    typedef NumericalArray<SampleType>      Buffer;
    typedef NumericalArray2D<SampleType>    BufferArray;
    typedef UnitBase<SampleType>            UnitType;
    typedef BusBuffer<SampleType>           BusType;
    typedef PLONK_BUSARRAYBASETYPE<BusType> BussesType;

    AudioHostBase() { }
    virtual ~AudioHostBase() { }
    
    BufferArray getInputs() const throw() { return inputs; }
    BufferArray getOutputs() const throw() { return outputs; }
    
    virtual void startHost()    = 0;
    virtual void stopHost()     = 0;
    
    virtual void hostStopped()  { }
    virtual void hostStarting() { }

    virtual UnitType constructGraph() = 0;
        
    void setup (const int numInputs, const int numOutputs)
    {
        plonk_assert (numInputs >= 0);
        
        busses.clear();
        
        int i;
        
        if (numInputs > 0)
        {
            inputs = BufferArray (numInputs);
            
            for (i = 0; i < numInputs; ++i)
            {
                inputs.atUnchecked (i).referTo (0, 0);
                
                Text busName = Text::fromValue (i);
                busses.add (busName);
            }
        }
        
        if (numOutputs > 0)
        {
            outputs = BufferArray (numOutputs);
            
            for (i = 0; i < numOutputs; ++i)
                outputs.atUnchecked (i).referTo (0, 0);
        }
        
        outputUnit = constructGraph();
        hostStarting();
    }
    
    inline void process() throw()
    {
        const int blockSize = BlockSize::getDefault().getValue();
        const int numInputs = inputs.length();
        const int numOutputs = outputs.length();
        
        int i;
        
        for (i = 0; i < numInputs; ++i)
            busses[i].write (info.getTimeStamp(), blockSize, inputs.atUnchecked (i).getArray());
        
        lock.lock();
        outputUnit.process (info);
        lock.unlock();
        
        if (outputUnit.isNotNull())
        {
            for (i = 0; i < numOutputs; ++i)
            {
                float* const output = outputs.atUnchecked (i).getArray();
                const float* const unitOutput = outputUnit.getOutputSamples (i);
                Floats::copyData (output, unitOutput, blockSize);            
            }
        }
        else if (numOutputs > 0)
        {
            float* const output = outputs.atUnchecked (0).getArray();
            for (i = 0; i < numOutputs; ++i)
                Floats::zeroData (output, blockSize);         
        }
        
        info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * blockSize);
    }
    
private:
    ProcessInfo info;
    UnitType outputUnit;  
    BussesType busses;
    BufferArray inputs, outputs;
    Lock lock;
};

#endif // PLONK_AUDIOHOSTBASE_H
