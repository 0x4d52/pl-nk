//
//  PAEProcessCallback.m-
//  PAEEngine
//
//  Created by Martin Robinson on 16/03/2015.
//  Copyright (c) 2015 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"


struct PAEProcessCallbackPeerData
{
    ChannelInternalCore::Data base;
    PAEProcessCallbackData functionData;
};

class PAEProcessCallbackPeerInternal : public ProxyOwnerChannelInternal<float, PAEProcessCallbackPeerData>
{
public:
    typedef PAEProcessCallbackPeerData                  Data;
    typedef ChannelBase<float>                          ChannelType;
    typedef ObjectArray<ChannelType>                    ChannelArrayType;
    typedef ProxyOwnerChannelInternal<float,Data>       Internal;
    typedef UnitBase<float>                             UnitType;
    typedef InputDictionary                             Inputs;
    typedef NumericalArray<float>                       Buffer;
    
    PAEProcessCallbackPeerInternal (Inputs const& inputs,
                                    Data const& data,
                                    BlockSize const& blockSize,
                                    SampleRate const& sampleRate,
                                    ChannelArrayType& channels) throw()
    :   Internal (data.functionData.numOutputs, inputs, data, blockSize, sampleRate, channels)
    {
    }
    
    ~PAEProcessCallbackPeerInternal()
    {
        Data& data = this->getState();
        delete [] data.functionData.outputData;
        delete [] data.functionData.inputData;
    }
    
    Text getName() const throw()
    {
        return "PAEProcessCallbackPeer";
    }
    
    IntArray getInputKeys() const throw()
    {
        const IntArray keys (IntArray::newClear (IOKey::Generic));
        return keys;
    }
    
    void initChannel (const int channel) throw()
    {
        if ((channel % this->getNumChannels()) == 0)
        {
            const UnitType& input = this->getInputAsUnit (IOKey::Generic);

            this->setBlockSize (BlockSize::decide (input.getBlockSize (0),
                                                   this->getBlockSize()));
            this->setSampleRate (SampleRate::decide (input.getSampleRate (0),
                                                     this->getSampleRate()));

            Data& data = this->getState();
            data.functionData.outputData = data.functionData.numOutputs > 0 ? new float* [data.functionData.numOutputs] : (float**)0;
            data.functionData.inputData = data.functionData.numInputs > 0 ? new const float* [data.functionData.numInputs] : (const float**)0;
        }
        
        this->initProxyValue (channel, 0.f);
    }
    
    void process (ProcessInfo& info, const int /*channel*/) throw()
    {
        Data& data = this->getState();
        
        if (data.functionData.callback)
        {
            UnitType& inputUnit (this->getInputAsUnit (IOKey::Generic));
            
            plonk_assert (inputUnit.getNumChannels() == data.functionData.numInputs);
            
            for (int i = 0; i < inputUnit.getNumChannels(); ++i)
            {
                const Buffer& inputBuffer (inputUnit.process (info, i));
                data.functionData.inputData[i] = inputBuffer.getArray();
            }
            
            plonk_assert (this->getNumChannels() == data.functionData.numOutputs);
            
            for (int i = 0; i < this->getNumChannels(); ++i)
            {
                data.functionData.outputData[i] = this->getOutputSamples (i);
            }
            
            data.functionData.numFrames = this->getOutputBuffer (0).length();
            data.functionData.sampleRate = data.base.sampleRate;
            data.functionData.callback (&data.functionData);
        }
    }
};

class PAEProcessCallbackPeer
{
public:
    typedef PAEProcessCallbackPeerData          Data;
    
    typedef ChannelBase<float>                  ChannelType;
    typedef ChannelInternal<float,Data>         Internal;
    typedef UnitBase<float>                     UnitType;
    typedef InputDictionary                     Inputs;
    
    static UnitType ar (UnitType const& input,
                        PAEProcessCallbackFunction function,
                        void* userData,
                        const int numOutputChannels,
                        BlockSize const& preferredBlockSize = BlockSize::noPreference(),
                        SampleRate const& preferredSampleRate = SampleRate::noPreference()) throw()
    {
        const int numInputChannels = input.getNumChannels();
        Data data = { { -1.0, -1.0 }, { function, -1, numOutputChannels, 0, numInputChannels, 0, 0, userData }};
        
        Inputs inputs;
        inputs.put (IOKey::Generic, input);
        
        return UnitType::template proxiesFromInputs<PAEProcessCallbackPeerInternal> (inputs,
                                                                                     data,
                                                                                     preferredBlockSize,
                                                                                     preferredSampleRate);
    }
};

@implementation PAEProcessCallback

+(PAEProcessCallback*)processCallbackWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs
                                           function:(PAEProcessCallbackFunction)function;
{
    return [[PAEProcessCallback alloc] initWithNumOutputs:numOutputs andNumInputs:numInputs function:function userData:NULL];
}

+(PAEProcessCallback*)processCallbackWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs
                                           function:(PAEProcessCallbackFunction)function userData:(void*)userData
{
    return [[PAEProcessCallback alloc] initWithNumOutputs:numOutputs andNumInputs:numInputs function:function userData:userData];
}

-(id)initWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs
               function:(PAEProcessCallbackFunction)function userData:(void*)userData
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.outputUnit = PAEProcessCallbackPeer::ar (self.patchUnit, function, userData, numOutputs);
    }
    
    return self;
}

@end