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

    AudioHostBase() throw()
    :   preferredSampleRate (0.0),
        preferredBlockSize (0)
    { 
    }
    
    virtual ~AudioHostBase() { }
        
    inline const UnitType& getOutputUnit() const throw() { return outputUnit; }
    
    inline int getNumInputs() const throw()  { return this->inputs.length(); }
    inline int getNumOutputs() const throw() { return this->outputs.length(); }
    
    inline double getPreferredSampleRate() const throw() { return preferredSampleRate; }
    inline int getPreferredBlockSize() const throw() { return preferredBlockSize; }
    
    inline void setPreferredSampleRate (const double newRate) throw() { preferredSampleRate = newRate; }
    inline void setPreferredBlockSize (const int newSize) throw() {  preferredBlockSize = newSize; }
    
    void setNumInputs (const int numInputs) throw()
    {
        plonk_assert (numInputs >= 0);
        
        if (numInputs != this->getNumInputs())
        {
            this->busses.clear();

            if (numInputs != 0)
            {
                inputs = BufferArray (numInputs);
                
                for (int i = 0; i < numInputs; ++i)
                {
                    this->inputs.atUnchecked (i).referTo (0, 0);
                    
                    Text busName = Text::fromValue (i);
                    this->busses.add (busName);
                }
            }
            else this->inputs.clear();
        }
    }
    
    void setNumOutputs (const int numOutputs) throw()
    {
        plonk_assert (numOutputs >= 0);
        
        if (numOutputs != this->getNumOutputs())
        {            
            if (numOutputs != 0)
            {
                outputs = BufferArray (numOutputs);
                
                for (int i = 0; i < numOutputs; ++i)
                    this->outputs.atUnchecked (i).referTo (0, 0);
            }
            else this->outputs.clear();
        }
    }
                
protected:
    inline BufferArray getInputs() const throw() { return this->inputs; }
    inline BufferArray getOutputs() const throw() { return this->outputs; }

    virtual void startHost()    = 0;
    virtual void stopHost()     = 0;
    
    virtual void hostStopped() throw()  { }
    virtual void hostStarting() throw() { }

    virtual UnitType constructGraph() = 0;

    inline void process() throw()
    {
        const int blockSize = BlockSize::getDefault().getValue();
        const int numInputs = this->inputs.length();
        const int numOutputs = this->outputs.length();
        
        int i;
        
        for (i = 0; i < numInputs; ++i)
            this->busses[i].write (this->info.getTimeStamp(), 
                                   blockSize, 
                                   this->inputs.atUnchecked (i).getArray());
        
        this->lock.lock();
        this->outputUnit.process (info);
        this->lock.unlock();
        
        if (this->outputUnit.isNotNull())
        {
            for (i = 0; i < numOutputs; ++i)
            {
                float* const output = this->outputs.atUnchecked (i).getArray();
                const float* const unitOutput = this->outputUnit.getOutputSamples (i);
                Floats::copyData (output, unitOutput, blockSize);            
            }
        }
        else if (numOutputs > 0)
        {
            float* const output = this->outputs.atUnchecked (0).getArray();
            for (i = 0; i < numOutputs; ++i)
                Floats::zeroData (output, blockSize);         
        }
        
        this->info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * blockSize);
    }
    
    void startHostInternal() throw()
    {
        outputUnit = constructGraph();
        hostStarting();
    }

    
private:
    double preferredSampleRate;
    int preferredBlockSize;

    ProcessInfo info;
    UnitType outputUnit;  
    BussesType busses;
    BufferArray inputs, outputs;
    Lock lock;
};

#endif // PLONK_AUDIOHOSTBASE_H
