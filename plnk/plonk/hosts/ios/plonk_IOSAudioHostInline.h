/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

BEGIN_PLONK_NAMESPACE

////#ifdef __llvm__
//// #ifdef __clang__
//  #pragma clang diagnostic push
//  #pragma clang diagnostic ignored "-Wno-deprecated"
//
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//
//// #endif
////#endif

template<class SampleType>
static OSStatus Render (void                        *inRefCon,
                        AudioUnitRenderActionFlags 	*ioActionFlags,
                        const AudioTimeStamp        *inTimeStamp,
                        UInt32 						inBusNumber,
                        UInt32 						inNumberFrames,
                        AudioBufferList				*ioData)
{	
	IOSAudioHostBase<SampleType> *host = (IOSAudioHostBase<SampleType> *)inRefCon;
	const OSStatus result =  host->renderCallback (inNumberFrames, 
                                                   ioActionFlags, 
                                                   inTimeStamp, 
                                                   ioData);
	return result;
}

template<class SampleType>
static void PropertyListener (void *                  inClientData,
                              AudioSessionPropertyID  inID,
                              UInt32                  inDataSize,
                              const void *            inPropertyValue)
{	
	IOSAudioHostBase<SampleType> *host = (IOSAudioHostBase<SampleType> *)inClientData;
    host->propertyCallback (inID, inDataSize, inPropertyValue);
}

template<class SampleType>
static void InterruptionListener (void *inClientData,
                                  UInt32 inInterruption)
{	
	IOSAudioHostBase<SampleType> *host = (IOSAudioHostBase<SampleType> *)inClientData;
    host->interruptionCallback (inInterruption);
}


template<class SrcType>
static PLONK_INLINE_LOW void audioTypeToShort (const SrcType * const src, short* const dst, const unsigned int length) throw()
{
    NumericalArrayConverter<short,SrcType>::convertScaled (dst, src, length);    
}

template<class SrcType>
static PLONK_INLINE_LOW void audioTypeToShortChannels (SrcType * const src[], AudioBufferList* const dst, const unsigned int length, const unsigned int numChannels) throw()
{
	for (UInt32 channel = 0; channel < numChannels; ++channel)
	{
        AudioBuffer* const audioBuffer = dst->mBuffers + channel;
		AudioSampleType* const audioUnitSamples = (AudioSampleType*)audioBuffer->mData;
		audioTypeToShort (src[channel], audioUnitSamples, length);
	}
}

template<class DstType>
static PLONK_INLINE_LOW void audioShortToType (const short * const src, DstType* const dst, const unsigned int length) throw()
{
    NumericalArrayConverter<DstType,short>::convertScaled (dst, src, length);
}

template<class DstType>
static PLONK_INLINE_LOW void audioShortToTypeChannels (AudioBufferList* src, DstType* const dst[], const unsigned int length, const unsigned int numChannels) throw()
{
	for (UInt32 channel = 0; channel < numChannels; ++channel)
	{
		AudioSampleType* const audioUnitSamples = (AudioSampleType*)src->mBuffers[0].mData; // need this other than 0?...
		audioShortToType (audioUnitSamples, dst[channel], length);
	}	
}

//------------------------------------------------------------------------------

template<class SampleType>
IOSAudioHostBase<SampleType>::IOSAudioHostBase() throw()
:   hwSampleRate (0.0),         // let the hardware choose
    rioUnit (NULL),
    cpuUsage (0.0),
    interrupted (NO),
    customRenderCallbackRef (NULL),
    customPreRender (NULL),
    customPostRender (NULL)
{
    convertBuffer.setSize (4096 * 2, false);
    this->setPreferredGraphBlockSize (256);
}

template<class SampleType>
IOSAudioHostBase<SampleType>::~IOSAudioHostBase()
{
    this->stopHost();
    this->killRemoteIO();
}

template<class SampleType>
Text IOSAudioHostBase<SampleType>::getHostName() const throw()
{
    return "iOS (" + TypeUtility<SampleType>::getTypeName() + ")";
}

template<class SampleType>
Text IOSAudioHostBase<SampleType>::getNativeHostName() const throw()
{
    return "RemoteIO (Short)";
}

template<class SampleType>
Text IOSAudioHostBase<SampleType>::getInputName() const throw()
{
    Text result = "Default Input";
    
    CFDictionaryRef dictionary = 0;
	UInt32 propertySize = sizeof (dictionary);
    
	if (AudioSessionGetProperty (kAudioSessionProperty_AudioRouteDescription, &propertySize, &dictionary) == noErr)
	{
        CFArrayRef array = (CFArrayRef)CFDictionaryGetValue (dictionary, kAudioSession_AudioRouteKey_Inputs);
        
        if (array != nil)
        {
            for (int i = 0; i < CFArrayGetCount (array); ++i)
            {  
                CFDictionaryRef dictionary2 = (CFDictionaryRef)CFArrayGetValueAtIndex (array, i);
                NSString* name = (NSString*)CFDictionaryGetValue (dictionary2, kAudioSession_AudioRouteKey_Type);
                result = [name UTF8String];
            }
        }
        
		CFRelease (dictionary);
	}
    
    return result;
}

template<class SampleType>
Text IOSAudioHostBase<SampleType>::getOutputName() const throw()
{    
    Text result = "Default Output";
    
    CFDictionaryRef dictionary = 0;
	UInt32 propertySize = sizeof (dictionary);
    
	if (AudioSessionGetProperty (kAudioSessionProperty_AudioRouteDescription, &propertySize, &dictionary) == noErr)
	{
        CFArrayRef array = (CFArrayRef)CFDictionaryGetValue (dictionary, kAudioSession_AudioRouteKey_Outputs);
        
        if (array != nil)
        {
            for (int i = 0; i < CFArrayGetCount (array); ++i)
            {  
                CFDictionaryRef dictionary2 = (CFDictionaryRef)CFArrayGetValueAtIndex (array, i);
                NSString* name = (NSString*)CFDictionaryGetValue (dictionary2, kAudioSession_AudioRouteKey_Type);
                result = [name UTF8String];
            }
        }
        
		CFRelease (dictionary);
	}
    
    return result;
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::stopHost() throw()
{
    if (this->getIsRunning())
    {
        AudioOutputUnitStop (rioUnit);
        this->setIsRunning (false);
        this->hostStopped();
    }
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::startHost() throw()
{
    if (rioUnit != NULL)
    {
        this->setIsRunning (true);
        AudioSessionSetActive (true);
        this->hostStarting();
        restart();
    }
    else
    {
        UInt32 size;
        
        // render proc
        inputProc.inputProc = Render<SampleType>;
        inputProc.inputProcRefCon = this;
            
        // session
        AudioSessionInitialize (NULL, NULL, InterruptionListener<SampleType>, this);
        AudioSessionSetActive (true);
                
        AudioSessionAddPropertyListener (kAudioSessionProperty_AudioRouteChange, PropertyListener<SampleType>, this);
        AudioSessionAddPropertyListener (kAudioSessionProperty_InterruptionType, PropertyListener<SampleType>, this);
        AudioSessionAddPropertyListener (kAudioSessionProperty_AudioCategory, PropertyListener<SampleType>, this);
        
        fixAudioRouteIfSetToReceiver();
        
        const double preferredHostSampleRate = this->getPreferredHostSampleRate();
        size = sizeof (preferredHostSampleRate);
        
        if (preferredHostSampleRate > 0.0)
            AudioSessionSetProperty (kAudioSessionProperty_PreferredHardwareSampleRate, size, &preferredHostSampleRate);
            
        resetIO();
        
        this->setPreferredHostSampleRate (hwSampleRate);
        this->setPreferredHostBlockSize (bufferSize);
        this->startHostInternal();
        
        restart();
        
        size = sizeof (format);
        AudioUnitGetProperty (rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &format, &size);
    }    
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::pauseHost() throw()
{
    if (this->getIsRunning())
    {
        AudioOutputUnitStop (rioUnit);
    }

    this->setIsPaused (true);
    this->hostPaused();
}


template<class SampleType>
void IOSAudioHostBase<SampleType>::resumeHost() throw()
{
    this->hostResuming();
    this->setIsPaused (false);

    if (this->getIsRunning())
    {
        AudioSessionSetActive (true);
        AudioOutputUnitStart (rioUnit);
    }
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::setFormat() throw()
{
	memset (&format, 0, sizeof (AudioStreamBasicDescription));
	format.mSampleRate = hwSampleRate;
	format.mFormatID = kAudioFormatLinearPCM;
	const int sampleSize = sizeof (AudioSampleType);
	format.mFormatFlags = kAudioFormatFlagsCanonical;
	format.mBitsPerChannel = 8 * sampleSize;
	format.mChannelsPerFrame = this->getNumOutputs();
	format.mFramesPerPacket = 1;
	format.mBytesPerPacket = format.mBytesPerFrame = sampleSize;
	format.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;		
}

template<class SampleType>
int IOSAudioHostBase<SampleType>::setupRemoteIO() throw()
{	
	// Open the output unit
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	
	AudioComponent comp = AudioComponentFindNext (NULL, &desc);
	AudioComponentInstanceNew (comp, &rioUnit);

	OSStatus status;
	const UInt32 one = 1;

    if (this->getNumInputs() > 0)
        status = AudioUnitSetProperty (rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &one, sizeof (one));

    status = AudioUnitSetProperty (rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &one, sizeof (one));
    status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &format, sizeof (format));
    status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &format, sizeof (format));

    if (this->getNumInputs() > 0)
    {
        status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &inputProc, sizeof (inputProc));
    }
    else
    {
        status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, 0, &inputProc, sizeof (inputProc));
    }
    
	AudioUnitInitialize (rioUnit);
	
//    NSLog(@"setupRemoteIO complete");
    
	return 0;	
}

//template<class SampleType>
//int IOSAudioHostBase<SampleType>::setupRemoteIO() throw()
//{
//	// Open the output unit
//	AudioComponentDescription desc;
//	desc.componentType = kAudioUnitType_Output;
//	desc.componentSubType = kAudioUnitSubType_RemoteIO;
//	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
//	desc.componentFlags = 0;
//	desc.componentFlagsMask = 0;
//	
//	AudioComponent comp = AudioComponentFindNext (NULL, &desc);
//	AudioComponentInstanceNew (comp, &rioUnit);
//    
//	OSStatus status;
//	const UInt32 one = 1;
//    
//    if (this->getNumInputs() > 0)
//        status = AudioUnitSetProperty (rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &one, sizeof (one));
//        
//    status = AudioUnitSetProperty (rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &one, sizeof (one));
//    status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &format, sizeof (format));
//    status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &format, sizeof (format));
//    
//    if (this->getNumInputs() > 0)
//    {
//        status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, 0, &inputProc, sizeof (inputProc));
//    }
//    else
//    {
//        status = AudioUnitSetProperty (rioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, 0, &inputProc, sizeof (inputProc));
//    }
//    
//	AudioUnitInitialize (rioUnit);
//	
////    NSLog(@"set   upRemoteIO complete");
//    
//	return 0;	
//}

template<class SampleType>
void IOSAudioHostBase<SampleType>::killRemoteIO() throw()
{
    if (rioUnit)
    {
        AudioOutputUnitStop (rioUnit);
        AudioComponentInstanceDispose (rioUnit);
        rioUnit = NULL;
    }
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::restart() throw()
{		    
    resetIO();
    
    this->setPreferredHostSampleRate (hwSampleRate);
    this->setPreferredHostBlockSize (bufferSize);
        
    if (!rioUnit)
    {
        setFormat();
        setupRemoteIO();
	}
    
	AudioOutputUnitStart (rioUnit);
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::fixAudioRouteIfSetToReceiver() throw()
{
	CFStringRef audioRoute = 0;
	UInt32 propertySize = sizeof (audioRoute);
    
	if (AudioSessionGetProperty (kAudioSessionProperty_AudioRoute, &propertySize, &audioRoute) == noErr)
	{
		NSString* route = (PLANK_OBJC_BRIDGE NSString*)audioRoute;
        
		if ([route hasPrefix: @"Receiver"])
		{
			UInt32 audioRouteOverride = kAudioSessionOverrideAudioRoute_Speaker;
			AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, sizeof (audioRouteOverride), &audioRouteOverride);
		}
		
		CFRelease (audioRoute);
	}
}

template<class SampleType>
OSStatus IOSAudioHostBase<SampleType>::renderCallback (UInt32                     inNumberFrames,
                                                       AudioUnitRenderActionFlags *ioActionFlags, 
                                                       const AudioTimeStamp       *inTimeStamp, 
                                                       AudioBufferList            *ioData) throw()
{
    OSStatus err = 0;
    int i;
        
	double renderTime = CFAbsoluteTimeGetCurrent();
	
	if (inNumberFrames > bufferSize)
	{
		bufferSize = inNumberFrames;        
        convertBuffer.setSize (inNumberFrames * plonk::max (this->getNumInputs(), 
                                                            this->getNumOutputs()), 
                               false);
	}
	
    this->setPreferredHostBlockSize (inNumberFrames);
	
	SampleType *convertBufferData[2];
	convertBufferData[0] = convertBuffer.getArray();
	convertBufferData[1] = convertBufferData[0] + inNumberFrames;	
    
    ConstBufferArray& inputs = this->getInputs();
    BufferArray& outputs = this->getOutputs();
    
	if (audioInputIsAvailable && (inputs.length() > 0))
	{
		err = AudioUnitRender (rioUnit, ioActionFlags, inTimeStamp, 1, inNumberFrames, ioData);
        
        if (err == noErr)
            audioShortToTypeChannels (ioData, convertBufferData, inNumberFrames, numInputChannels);				
	}
	else convertBuffer.clear();  
            
    for (i = 0; i < inputs.length(); ++i)
        inputs.atUnchecked (i) = convertBufferData[i];
    
    for (i = 0; i < outputs.length(); ++i)
        outputs.atUnchecked (i) = convertBufferData[i];

    if (customPreRender)
        customPreRender (customRenderCallbackRef, inNumberFrames, ioActionFlags, inTimeStamp, ioData);
    
    this->process();
        
	audioTypeToShortChannels (convertBufferData, ioData, inNumberFrames, ioData->mNumberBuffers);
    
    if (customPostRender)
        customPostRender (customRenderCallbackRef, inNumberFrames, ioActionFlags, inTimeStamp, ioData);
    
	renderTime = CFAbsoluteTimeGetCurrent() - renderTime;
	
	const double timeRatio = renderTime * reciprocalBufferDuration;
	cpuUsage += 0.2 * (timeRatio - cpuUsage); 
	
	return err;	
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::propertyCallback (AudioSessionPropertyID inID,
                                                     UInt32                 inDataSize,
                                                     const void *           inPropertyValue) throw()
{
	
    if (inID == kAudioSessionProperty_AudioRouteChange)
    {
        if (inPropertyValue)
        {
            CFDictionaryRef routeChangeDictionary = (CFDictionaryRef)inPropertyValue;
            CFNumberRef routeChangeReasonRef = 
                (CFNumberRef)CFDictionaryGetValue (routeChangeDictionary, CFSTR (kAudioSession_AudioRouteChangeKey_Reason));
            
            SInt32 routeChangeReason;
            CFNumberGetValue (routeChangeReasonRef, kCFNumberSInt32Type, &routeChangeReason);
            
            static const char* reasons[] = {
                "Unknown",
                "NewDeviceAvailable", "OldDeviceUnavailable", "CategoryChange", "Override",
                "(Invalid)",
                "WakeFromSleep", "NoSuitableRouteForCategory"
            };
            
            CFStringRef newAudioRoute;
            UInt32 propertySize = sizeof (CFStringRef);
            AudioSessionGetProperty (kAudioSessionProperty_AudioRoute, &propertySize, &newAudioRoute);
            NSLog (@"route: %s reason = '%s' (%d)\n",
                   CFStringGetCStringPtr (newAudioRoute,CFStringGetSystemEncoding()),
                   reasons[routeChangeReason], (int)routeChangeReason);
            CFRelease(newAudioRoute);
            
            this->fixAudioRouteIfSetToReceiver();
        }
    }
    else if (inID == kAudioSessionProperty_AudioCategory)
    {
        resetIO();
    }
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::resetIO() throw()
{
    OSStatus err;
    UInt32 size;
    
    size = sizeof (hwSampleRate);
    AudioSessionGetProperty (kAudioSessionProperty_CurrentHardwareSampleRate, &size, &hwSampleRate);
    
    if (this->getPreferredHostBlockSize() > 0)
    {
        bufferDuration = (double (this->getPreferredHostBlockSize()) + 0.5) / hwSampleRate;
        AudioSessionSetProperty (kAudioSessionProperty_PreferredHardwareIOBufferDuration, sizeof (bufferDuration), &bufferDuration);
    }

    size = sizeof(UInt32);
    err = AudioSessionGetProperty (kAudioSessionProperty_CurrentHardwareInputNumberChannels, &size, &numInputChannels);
    
    if (err != noErr)
        numInputChannels = 0;
    
    err = AudioSessionGetProperty (kAudioSessionProperty_CurrentHardwareOutputNumberChannels, &size, &numOutputChannels);
    err = AudioSessionGetProperty (kAudioSessionProperty_AudioInputAvailable, &size, &audioInputIsAvailable);
    
    if (audioInputIsAvailable && this->getNumInputs() > 0)
        this->setNumInputs (numInputChannels);
        
    this->setNumOutputs (numOutputChannels);
    
    size = sizeof (bufferDuration);
    AudioSessionGetProperty (kAudioSessionProperty_CurrentHardwareIOBufferDuration, &size, &bufferDuration);
    
    reciprocalBufferDuration = 1.0 / double (bufferDuration);
    
    bufferSize = (int)(hwSampleRate * bufferDuration + 0.5);
    convertBuffer.setSize (bufferSize * plonk::max (this->getNumOutputs(),
                                                    this->getNumInputs()),
                           false);        
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::interruptionCallback (UInt32 inInterruption) throw()
{
    NSLog (@"...Interruption...");
    
    if (inInterruption == kAudioSessionBeginInterruption)
    {
        interrupted = true;
        this->hostInterruption (true);
        this->stopHost();
        
        NSLog (@"Interruption %s", inInterruption ? "begin" : "end");
    }
    else if (inInterruption == kAudioSessionEndInterruption)
    {
        this->hostInterruption (false);
        this->startHost();
        interrupted = false;

        NSLog (@"Interruption %s", inInterruption ? "begin" : "end");
	}
}

template<class SampleType>
void IOSAudioHostBase<SampleType>::setCustomRenderCallbacks (void* refCon,
                                                             RenderCallbackFunction preRender,
                                                             RenderCallbackFunction postRender) throw()
{
    customRenderCallbackRef = refCon;
    customPreRender = preRender;
    customPostRender = postRender;
}

//#ifdef __llvm__
// #ifdef __clang__
//  #pragma clang diagnostic pop
// #endif
//#endif

#pragma GCC diagnostic pop


END_PLONK_NAMESPACE

#endif

#undef kOutputBus
#undef kInputBus