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

#if PLANK_INLINING_FUNCTIONS

template<class SampleType>
static inline int paCallback (const void *input, void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData)
{
    PortAudioAudioHostBase<SampleType>* host = static_cast<PortAudioAudioHostBase<SampleType>*> (userData);
    return host->callback ((const SampleType**)input, (SampleType**)output, 
                           frameCount, timeInfo, statusFlags);
}

static inline bool paCheckError (PaError err)
{
    if (err != paNoError)
    {
        printf ("paerror '%s' (%d)\n", Pa_GetErrorText (err), err);
        return true;
    }
    
    return false; 
}

//static inline void paGetInfo()
//{
//    const PaDeviceIndex inputDeviceId = Pa_GetDefaultInputDevice();
//    const PaDeviceIndex outputDeviceId = Pa_GetDefaultOutputDevice();
//    
//    const PaDeviceInfo* inputInfo = Pa_GetDeviceInfo (inputDeviceId);
//    const PaDeviceInfo* outputInfo = Pa_GetDeviceInfo (outputDeviceId);
//    
//
//}

//------------------------------------------------------------------------------

template<class SampleType>
PortAudioAudioHostBase<SampleType>::PortAudioAudioHostBase (ObjectMemoryBase* omb) throw() 
:   AudioHostBase<SampleType> (omb),
    stream (0)
{
    PaError err;
	err = Pa_Initialize();
    paCheckError (err);
    
    this->setNumInputs (1);
    this->setNumOutputs (2);
    this->setPreferredHostBlockSize (512);
    this->setPreferredGraphBlockSize (128);
    this->setPreferredHostSampleRate (44100.0);    
}

template<class SampleType>
PortAudioAudioHostBase<SampleType>::~PortAudioAudioHostBase()
{
    if (stream)
        this->stopHost();
    
    PaError err;
    err = Pa_Terminate();
    paCheckError (err);
}

template<class SampleType>
Text PortAudioAudioHostBase<SampleType>::getHostName() const throw()
{
    return "PortAudio (" + TypeUtility<SampleType>::getTypeName() + ")";
}

template<class SampleType>
Text PortAudioAudioHostBase<SampleType>::getNativeHostName() const throw()
{
    const PaHostApiIndex paHostIndex = Pa_GetDefaultHostApi();
    const PaHostApiInfo* info = Pa_GetHostApiInfo (paHostIndex);
    return info->name;
}

template<class SampleType>
Text PortAudioAudioHostBase<SampleType>::getInputName() const throw()
{
    const PaDeviceIndex deviceId = Pa_GetDefaultInputDevice();    
    const PaDeviceInfo* info = Pa_GetDeviceInfo (deviceId);
    return info->name;
}

template<class SampleType>
Text PortAudioAudioHostBase<SampleType>::getOutputName() const throw()
{
    const PaDeviceIndex deviceId = Pa_GetDefaultOutputDevice();    
    const PaDeviceInfo* info = Pa_GetDeviceInfo (deviceId);
    return info->name;
}

template<class SampleType>
double PortAudioAudioHostBase<SampleType>::getCpuUsage() const throw()
{
    if (!stream) return 0.0;
    return Pa_GetStreamCpuLoad (stream);
}

template<class SampleType>
void PortAudioAudioHostBase<SampleType>::stopHost() throw()
{
    PaError err;
    
    err = Pa_StopStream (stream);
    paCheckError (err);
    
    err = Pa_CloseStream (stream);
    paCheckError (err);
    
    stream = NULL;
    this->setIsRunning (false);
}

template<class SampleType>
void PortAudioAudioHostBase<SampleType>::startHost() throw()
{        
    PaError err;
    
    PaSampleFormat sampleFormat;
    
    switch (TypeUtility<SampleType>::getTypeCode()) 
    {
        case TypeCode::Float: sampleFormat = paFloat32; break;
        case TypeCode::Short: sampleFormat = paInt16; break;
        case TypeCode::Int: sampleFormat = paInt32; break;
        case TypeCode::Int24: sampleFormat = paInt24; break;
        case TypeCode::Char: sampleFormat = paInt8; break;
        default: sampleFormat = 0; break;
    }
        
    if (sampleFormat != 0)
    {
        err = Pa_OpenDefaultStream (&stream, 
                                    this->getNumInputs(), this->getNumOutputs(),
                                    sampleFormat | paNonInterleaved,
                                    this->getPreferredHostSampleRate(), this->getPreferredHostBlockSize(),
                                    paCallback<SampleType>, 
                                    this);
        
        // if our preferred sample rate and block size are invalid hopefully we get an error
        if (paCheckError (err)) 
            return;
        
        this->startHostInternal();
        
        err = Pa_StartStream (stream);
        paCheckError (err); 
    }
    else plonk_assertfalse;
}

template<class SampleType>
int PortAudioAudioHostBase<SampleType>::callback (const SampleType **inputData, SampleType **outputData,
                                                  unsigned long frameCount,
                                                  const PaStreamCallbackTimeInfo* timeInfo,
                                                  PaStreamCallbackFlags statusFlags) throw()
{
    (void)timeInfo;
    (void)statusFlags;
    
    this->setPreferredHostBlockSize (frameCount);
    
    ConstBufferArray& inputs = this->getInputs();
    BufferArray& outputs = this->getOutputs();

    const int numInputs = inputs.length();
    const int numOutputs = outputs.length();
    
    int i;
    
    for (i = 0; i < numInputs; ++i)
        inputs.atUnchecked (i) = inputData[i];

    for (i = 0; i < numOutputs; ++i)
        outputs.atUnchecked (i) = outputData[i];
    
    this->process();
    
    return paContinue;
}

#endif // PLANK_INLINING_FUNCTIONS

