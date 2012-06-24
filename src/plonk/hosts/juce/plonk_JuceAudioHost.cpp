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

#ifdef PLONK_AUDIOHOST_JUCE

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

END_PLONK_NAMESPACE
#include "plonk_JuceAudioHost.h"
BEGIN_PLONK_NAMESPACE



JuceAudioHost::JuceAudioHost() throw()
{    
    setNumInputs (2);
    setNumOutputs (2);
}

JuceAudioHost::~JuceAudioHost()
{
    stopHost();
}

Text JuceAudioHost::getHostName() const throw()
{
    return "Juce";
}

Text JuceAudioHost::getNativeHostName() const throw()
{
    return static_cast<const char*> (audioDeviceManager.getCurrentAudioDeviceType().toUTF8());
}

Text JuceAudioHost::getInputName() const throw()
{
    Text result;
    AudioDeviceManager::AudioDeviceSetup setup;
	
    if (getIsRunning())
    {
        audioDeviceManager.getAudioDeviceSetup (setup);
        result = static_cast<const char*> (setup.inputDeviceName.toUTF8());
    }
    else
    {
        AudioDeviceManager dummyDevice;
        dummyDevice.initialise (getNumInputs(), getNumOutputs(), 0, true);
        dummyDevice.getAudioDeviceSetup (setup);
        result = static_cast<const char*> (setup.inputDeviceName.toUTF8());
    }
    
    return result;
}

Text JuceAudioHost::getOutputName() const throw()
{
    Text result;
    AudioDeviceManager::AudioDeviceSetup setup;
	
    if (getIsRunning())
    {
        audioDeviceManager.getAudioDeviceSetup (setup);
        result = static_cast<const char*> (setup.outputDeviceName.toUTF8());
    }
    else
    {
        AudioDeviceManager dummyDevice;
        dummyDevice.initialise (getNumInputs(), getNumOutputs(), 0, true);
        dummyDevice.getAudioDeviceSetup (setup);
        result = static_cast<const char*> (setup.outputDeviceName.toUTF8());
    }
    
    return result;
}

double JuceAudioHost::getCpuUsage() const throw()
{
    return audioDeviceManager.getCpuUsage();
}

void JuceAudioHost::stopHost() throw()
{
    audioDeviceManager.getAudioCallbackLock().enter();
    audioDeviceManager.removeAudioCallback(this);
    audioDeviceManager.getAudioCallbackLock().exit();
    audioDeviceManager.closeAudioDevice();
}

void JuceAudioHost::startHost() throw()
{
    String error = audioDeviceManager.initialise (getNumInputs(), 
                                                  getNumOutputs(), 
                                                  0, true);
	    
	if (error.isNotEmpty())
	{
		AlertWindow::showMessageBox (AlertWindow::WarningIcon, 
									 "JuceAudioHost", 
									 "AudioDeviceManager init failed \n\n" + error);
		
		return;
	}
	
	AudioDeviceManager::AudioDeviceSetup setup;
	audioDeviceManager.getAudioDeviceSetup (setup);
    
    if (getPreferredBlockSize() > 0)
        setup.bufferSize = getPreferredBlockSize();
    
    if (getPreferredSampleRate() > 0.0)
        setup.sampleRate = getPreferredSampleRate();
    
    audioDeviceManager.setAudioDeviceSetup (setup, false);
    
	if (error.isNotEmpty())
	{
		AlertWindow::showMessageBox (AlertWindow::WarningIcon, 
									 "JuceAudioHost", 
									 "AudioDeviceManager setup failed \n\n" + error);
		
		return;
	}
    
	audioDeviceManager.addAudioCallback (this);	    
    
}

void JuceAudioHost::audioDeviceIOCallback (const float** inputData, int numInputs, 
                                           float** outputData, int numOutputs, 
                                           int blockSize) throw()
{
    BlockSize::getDefault().setValue (blockSize);
    
    plonk_assert (numInputs == getInputs().length());
    plonk_assert (numOutputs == getOutputs().length());
    
    int i;
    
    for (i = 0; i < numInputs; ++i)
        getInputs().atUnchecked (i).referTo (blockSize, const_cast<float*> (inputData[i]));

    for (i = 0; i < numOutputs; ++i)
        getOutputs().atUnchecked (i).referTo (blockSize, outputData[i]);
    
    process();
}

void JuceAudioHost::audioDeviceAboutToStart (AudioIODevice* device) throw()
{
    SampleRate::getDefault().setValue (device->getCurrentSampleRate());
    BlockSize::getDefault().setValue (device->getCurrentBufferSizeSamples()); 
    startHostInternal();
}

void JuceAudioHost::audioDeviceStopped() throw()
{
    setIsRunning (false);
    hostStopped();
}

END_PLONK_NAMESPACE

#endif // PLONK_AUDIOHOST_JUCE
