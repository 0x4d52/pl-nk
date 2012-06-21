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

#include "NSIOSAudioHost.h"

#define NUM_CHANNELS 2
//#define DEFAULT_BUFFERSIZE 2048


static OSStatus	Render(void							*inRefCon, 
					   AudioUnitRenderActionFlags 	*ioActionFlags, 
					   const AudioTimeStamp 		*inTimeStamp, 
					   UInt32 						inBusNumber, 
					   UInt32 						inNumberFrames, 
					   AudioBufferList				*ioData)
{	
	IOSAudioHost *host = (IOSAudioHost *)inRefCon;
	const OSStatus result =  [host renderCallback:inNumberFrames 
								  withActionFlags:ioActionFlags 
									  atTimeStamp:inTimeStamp
									  withBuffers:ioData];
	return result;
	
}

void PropertyListener(void *                  inClientData,
					  AudioSessionPropertyID  inID,
					  UInt32                  inDataSize,
					  const void *            inPropertyValue)
{	
	IOSAudioHost *host = (IOSAudioHost *)inClientData;
	[host propertyCallback:inID 
				  withSize:inDataSize 
			  withProperty:inPropertyValue];;
}

void InterruptionListener(void *inClientData, UInt32 inInterruption)
{	
	IOSAudioHost *host = (IOSAudioHost *)inClientData;
	[host interruptionCallback:inInterruption];
}

@implementation NSIOSAudioHost

@synthesize audioCategory, preferredBufferSize, preferredSampleRate;

- (id)init
{
	if (self = [super init])
	{
		preferredBufferSize = 1024;
        preferredSampleRate = 0.0;  // let the hardware choose
		hwSampleRate = 0.0;         // let the hardware choose
		cpuUsage = 0.0;
        audioCategory = kAudioSessionCategory_PlayAndRecord;
	}
    
	return self;
}

- (void)setFormat
{
	memset(&format, 0, sizeof(AudioStreamBasicDescription));
	format.mSampleRate = hwSampleRate;
	format.mFormatID = kAudioFormatLinearPCM;
	int sampleSize = sizeof(AudioSampleType);
	format.mFormatFlags = kAudioFormatFlagsCanonical;
	format.mBitsPerChannel = 8 * sampleSize;
	format.mChannelsPerFrame = NUM_CHANNELS;
	format.mFramesPerPacket = 1;
	format.mBytesPerPacket = format.mBytesPerFrame = sampleSize;
	format.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;		
}

- (int)setupRemoteIO
{	
	// Open the output unit
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	
	AudioComponent comp = AudioComponentFindNext(NULL, &desc);
	AudioComponentInstanceNew(comp, &rioUnit);
	
	const UInt32 one = 1;
	AudioUnitSetProperty(rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &one, sizeof(one));	
	AudioUnitSetProperty(rioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &inputProc, sizeof(inputProc));
	
	AudioUnitSetProperty(rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &format, sizeof(format));
	AudioUnitSetProperty(rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &format, sizeof(format));
		
	AudioUnitInitialize(rioUnit);
	
	return 0;	
}

#pragma mark float<->short conversion routines

static inline void audioFloatToShort(float *src, short* dst, unsigned int length)
{
	static const float scale = 32767.f;
    pl_VectorMulF_N1N (src, scale, src, length);
    pl_VectorConvertF2S_NN (dst, src, length);
}

static inline void audioFloatToShortChannels(float *src[], AudioBufferList* dst, unsigned int length, unsigned int numChannels)
{
	for (UInt32 channel = 0; channel < numChannels; channel++)
	{
		AudioSampleType *audioUnitBuffer = (AudioSampleType*)dst->mBuffers[channel].mData;		
		audioFloatToShort (src[channel], audioUnitBuffer, length);
	}
}

static inline void audioShortToFloat(short *src, float* dst, unsigned int length)
{
	static const float scale = 1.f / 32767.f;	
	pl_VectorConvertS2F_NN (dst, src, length);
    pl_VectorMulF_N1N (dst, scale, dst, length);
}

static inline void audioShortToFloatChannels(AudioBufferList* src, float* dst[], unsigned int length, unsigned int numChannels)
{
	for (UInt32 channel = 0; channel < numChannels; channel++)
	{
		AudioSampleType *audioUnitBuffer = (AudioSampleType*)src->mBuffers[0].mData; // need this other than 0?...		
		audioShortToFloat (audioUnitBuffer, dst[channel], length);
	}	
}

#pragma mark callbacks

- (OSStatus)renderCallback:(UInt32)inNumberFrames 
		   withActionFlags:(AudioUnitRenderActionFlags*)ioActionFlags
			   atTimeStamp:(const AudioTimeStamp*)inTimeStamp 
			   withBuffers:(AudioBufferList*)ioData
{
	OSStatus err = 0;
		
	double renderTime = CFAbsoluteTimeGetCurrent();
	
	if(inNumberFrames > bufferSize)
	{
		delete [] floatBuffer;
		bufferSize = inNumberFrames;
		
		floatBuffer = new float[inNumberFrames * NUM_CHANNELS];
	}
	
    BlockSize::getDefault().setValue (inNumberFrames);
	
	float *floatBufferData[2];
	floatBufferData[0] = floatBuffer;
	floatBufferData[1] = floatBufferData[0] + inNumberFrames;	
		
	if(audioInputIsAvailable)
	{
		err = AudioUnitRender(rioUnit, ioActionFlags, inTimeStamp, 1, inNumberFrames, ioData);
		
        if (err) 
        { 
            printf("renderCallback: error %d %s\n", (int)err, (err == -10863) ? "(harmless)" : ""); 
            return err; 
        }
		
		audioShortToFloatChannels(ioData, floatBufferData, inNumberFrames, numInputChannels);				
	}
	else
	{
		memset(floatBuffer, 0, numInputChannels * inNumberFrames * sizeof(float));
	}
	
    int i;
    
    for (i = 0; i < numInputChannels; ++i)
        busses[i].write (info.getTimeStamp(), inNumberFrames, floatBufferData[i]);
    
    lock.lock();
    outputUnit.process (info);
    lock.unlock();
    
    if (outputUnit.isNotNull())
    {
        for (i = 0; i < numOutputChannels; ++i)
        {
            float* const output = floatBufferData[i];
            const float* const unitOutput = outputUnit.getOutputSamples (i);
            Floats::copyData (output, unitOutput, inNumberFrames);            
        }
    }
    else
    {
        float* const output = floatBufferData[0];
        for (i = 0; i < numOutputChannels; ++i)
            Floats::zeroData (output, inNumberFrames);         
    }
    
    info.offsetTimeStamp (SampleRate::getDefault().getSampleDurationInTicks() * inNumberFrames);
	audioFloatToShortChannels (floatBufferData, ioData, inNumberFrames, ioData->mNumberBuffers);
			
	renderTime = CFAbsoluteTimeGetCurrent() - renderTime;
	
	const float timeRatio = renderTime * reciprocalBufferDuration;
	cpuUsage += 0.2f * (timeRatio - cpuUsage); 
	
	return err;	
}	

- (void)propertyCallback:(AudioSessionPropertyID)inID
				withSize:(UInt32)inDataSize	
			withProperty:(const void *)inPropertyValue
{	
	if(!isRunning) return;
	
	if(inPropertyValue)
	{
		CFDictionaryRef routeChangeDictionary  = (CFDictionaryRef)inPropertyValue;
		CFNumberRef     routeChangeReasonRef   = 
		(CFNumberRef)CFDictionaryGetValue (routeChangeDictionary, 
										   CFSTR (kAudioSession_AudioRouteChangeKey_Reason));
		
		SInt32 routeChangeReason;
		CFNumberGetValue(routeChangeReasonRef, kCFNumberSInt32Type, &routeChangeReason);
		
		CFStringRef newAudioRoute;
		UInt32 propertySize = sizeof (CFStringRef);
		AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &propertySize, &newAudioRoute);
		
		printf("route=%s\n", CFStringGetCStringPtr(newAudioRoute, CFStringGetSystemEncoding()));
    }
	
    [self restart];
}

- (void)restart
{
    UInt32 size = sizeof(UInt32);
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareInputNumberChannels, &size, &numInputChannels);
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareOutputNumberChannels, &size, &numOutputChannels);
	AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &size, &audioInputIsAvailable);
	
	printf("inputs=%d outputs=%d audioInputIsAvailable=%d\n", (int)numInputChannels, (int)numOutputChannels, (int)audioInputIsAvailable);
	
	if(rioUnit)	
        AudioComponentInstanceDispose(rioUnit);
	
	rioUnit = NULL;
	
	[self setFormat];
	[self setupRemoteIO];
	
	AudioSessionSetActive(true);
	AudioOutputUnitStart(rioUnit);
}


- (void)fixAudioRouteIfSetToReceiver
{
	CFStringRef audioRoute = 0;
	UInt32 propertySize = sizeof (audioRoute);
	if (AudioSessionGetProperty (kAudioSessionProperty_AudioRoute, &propertySize, &audioRoute) == noErr)
	{
		NSString* route = (NSString*) audioRoute;
				
		if ([route hasPrefix: @"Receiver"])
		{
			UInt32 audioRouteOverride = kAudioSessionOverrideAudioRoute_Speaker;
			AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, sizeof (audioRouteOverride), &audioRouteOverride);
		}
		
		CFRelease (audioRoute);
	}
}

- (void)interruptionCallback:(UInt32)inInterruption
{
	if (inInterruption == kAudioSessionEndInterruption) {
		// make sure we are again the active session
		//AudioSessionSetActive(false);
		AudioSessionSetActive(true);
		isRunning = true;
		AudioOutputUnitStart(rioUnit);
	}
}

#pragma mark other functions

-(void)start
{	
    UInt32 size;

	// render proc
	inputProc.inputProc = Render;
	inputProc.inputProcRefCon = self;
	
//	printf("IOSAudioHost: SR=%f\n", hwSampleRate);
	
	// session
	AudioSessionInitialize(NULL, NULL, InterruptionListener, self);
	AudioSessionSetActive(true);
	
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory);
	AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, PropertyListener, self);
	
	[self fixAudioRouteIfSetToReceiver];
	
    size = sizeof(preferredSampleRate);

	if(hwSampleRate > 0.0)
	{
		AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareSampleRate, size, &preferredSampleRate);
	}		

    size = sizeof(hwSampleRate);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, &size, &hwSampleRate);
	
	bufferDuration = preferredBufferSize / hwSampleRate;
	AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, sizeof(bufferDuration), &bufferDuration);
	
	size = sizeof (UInt32);
	AudioSessionGetProperty (kAudioSessionProperty_CurrentHardwareInputNumberChannels, &size, &numInputChannels);
	AudioSessionGetProperty (kAudioSessionProperty_CurrentHardwareOutputNumberChannels, &size, &numOutputChannels);
	AudioSessionGetProperty (kAudioSessionProperty_AudioInputAvailable, &size, &audioInputIsAvailable);
			
    SampleRate::getDefault().setValue (hwSampleRate);
    BlockSize::getDefault().setValue (preferredBufferSize);    
    busses = Busses (Bus("0"), Bus("1"));
    outputUnit = [self constructGraph];    	
	
	rioUnit = NULL;
	isRunning = true;
	[self restart];
	
	size = sizeof(format);
	AudioUnitGetProperty(rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &format, &size);
	
	size = sizeof(bufferDuration);
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareIOBufferDuration, &size, &bufferDuration);
	
	reciprocalBufferDuration = 1.f / bufferDuration; 
	
	bufferSize = (int)(hwSampleRate*bufferDuration+0.5);
	floatBuffer = new float[bufferSize * NUM_CHANNELS];
		
	printf("IOSAudioHost: SR=%f buffer=%fs (%d samples)\n", hwSampleRate, bufferDuration, bufferSize);
}

- (id)initAudio
{	
    [self start];
	return self;
}

- (Unit)constructGraph
{
	return Unit::getNull();
}

- (float)getCpuUsage
{
	return cpuUsage;
}

- (void)lock
{
	lock.lock();
}

- (void)unlock
{
	lock.unlock();
}

- (BOOL)tryLock
{
	return lock.tryLock();
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	printf("buttonIndex=%d\n", buttonIndex);
	
	if(buttonIndex == 0)
	{
		// resume
		isRunning = true;
		[self restart];
	}
	
	[alertView release];
}


- (void)isAboutToShutdown
{
	// override this to do something just before shutdown
}


- (void)shutdown
{
	[self isAboutToShutdown];
	
	const double pause = 0.25;
	[NSThread sleepForTimeInterval: pause];
    
    if (rioUnit)
    {
        AudioComponentInstanceDispose(rioUnit);
        rioUnit = NULL;
    }
	
    outputUnit = Unit();
}


-(void) dealloc
{
	delete [] floatBuffer;
	[super dealloc];
}
@end

