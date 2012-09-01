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

#ifdef PLONK_AUDIOHOST_PORTAUDIO

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

END_PLONK_NAMESPACE
#include "plonk_PortAudioAudioHost.h"
BEGIN_PLONK_NAMESPACE

//------------------------------------------------------------------------------

static inline int paCallback (const void *input, void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData)
{
    PortAudioAudioHost* host = (PortAudioAudioHost*)userData;
    return host->callback ((const float**)input, (float**)output, 
                           frameCount, timeInfo, statusFlags);
}

static inline void paCheckError (PaError err)
{
    if (err != paNoError)
        printf ("paerror '%s' (%d)\n", Pa_GetErrorText (err), err);
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

PortAudioAudioHost::PortAudioAudioHost (ObjectMemoryBase* omb) throw() 
:   AudioHostBase (omb),
    stream (0)
{
    PaError err;
	err = Pa_Initialize();
    paCheckError (err);
    
    setNumInputs (1);
    setNumOutputs (2);
    setPreferredHostBlockSize (512);
    setPreferredHostSampleRate (44100.0);    
}

PortAudioAudioHost::~PortAudioAudioHost()
{
    if (stream)
        stopHost();
    
    PaError err;
    err = Pa_Terminate();
    paCheckError (err);
}

Text PortAudioAudioHost::getHostName() const throw()
{
    return "PortAudio";
}

Text PortAudioAudioHost::getNativeHostName() const throw()
{
    const PaHostApiIndex paHostIndex = Pa_GetDefaultHostApi();
    const PaHostApiInfo* info = Pa_GetHostApiInfo (paHostIndex);
    return info->name;
}

Text PortAudioAudioHost::getInputName() const throw()
{
    const PaDeviceIndex deviceId = Pa_GetDefaultInputDevice();    
    const PaDeviceInfo* info = Pa_GetDeviceInfo (deviceId);
    return info->name;
}

Text PortAudioAudioHost::getOutputName() const throw()
{
    const PaDeviceIndex deviceId = Pa_GetDefaultOutputDevice();    
    const PaDeviceInfo* info = Pa_GetDeviceInfo (deviceId);
    return info->name;
}

double PortAudioAudioHost::getCpuUsage() const throw()
{
    if (!stream) return 0.0;
    return Pa_GetStreamCpuLoad (stream);
}

void PortAudioAudioHost::stopHost() throw()
{
    PaError err;
    
    err = Pa_StopStream (stream);
    paCheckError (err);
    
    err = Pa_CloseStream (stream);
    paCheckError (err);
    
    stream = NULL;
    setIsRunning (false);
}

void PortAudioAudioHost::startHost() throw()
{        
    PaError err;
    err = Pa_OpenDefaultStream (&stream, 
                                getNumInputs(), getNumOutputs(),
                                paFloat32 | paNonInterleaved,
                                getPreferredHostSampleRate(), getPreferredHostBlockSize(),
                                paCallback, 
                                this);
    paCheckError (err);
    
    SampleRate::getDefault().setValue (getPreferredHostSampleRate());
    BlockSize::getDefault().setValue (getPreferredHostBlockSize()); 
    startHostInternal();
    
    err = Pa_StartStream (stream);
    paCheckError (err);    
}

int PortAudioAudioHost::callback (const float **inputData, float **outputData,
                                  unsigned long frameCount,
                                  const PaStreamCallbackTimeInfo* timeInfo,
                                  PaStreamCallbackFlags statusFlags) throw()
{
    (void)timeInfo;
    (void)statusFlags;
    
    BlockSize::getDefault().setValue (frameCount);
    
    ConstBufferArray& inputs = getInputs();
    BufferArray& outputs = getOutputs();

    const int numInputs = inputs.length();
    const int numOutputs = outputs.length();
    
    int i;
    
    for (i = 0; i < numInputs; ++i)
        inputs.atUnchecked (i) = inputData[i];//.referTo (frameCount, const_cast<float*> (inputData[i]));

    for (i = 0; i < numOutputs; ++i)
        outputs.atUnchecked (i) = outputData[i];//.referTo (frameCount, outputData[i]);
    
    process();
    
    return paContinue;
}

END_PLONK_NAMESPACE

#endif // PLONK_AUDIOHOST_PORTAUDIO
