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

#ifndef PLONK_IOSAUDIOHOST_H
#define PLONK_IOSAUDIOHOST_H

#include <AudioToolbox/AudioServices.h>
#include <AudioUnit/AudioUnit.h>

BEGIN_PLONK_NAMESPACE

/** Some utilties for the iOS platform. */
class IOSUtilities
{
public:
    /** @deprecated Use the FilePath class instead. */
    enum Locations
    {
        Bundle,		///< In the \<MyApp\>.app/ bundle, files can be placed here during compilation in Xcode. 
        Documents,	///< The application's Documents directory, files here will be backed up during iTunes sync.
        Temporary	///< The application's tmp directory, NB this is cleared when the device is restarted.
    };
    
    /** @deprecated Use the FilePath class instead.
     IOSUtilities::pathInDirectory(Bundle, filename) becomes FilePath::system(FilePath::App).child(filename).fullpath()
     IOSUtilities::pathInDirectory(Documents, filename) becomes FilePath::system(FilePath::AppData).child(filename).fullpath()
     IOSUtilities::pathInDirectory(Temporary, filename) becomes FilePath::system(FilePath::Temp).child(filename).fullpath()
     */
    static Text pathInDirectory (Locations location, const char *filename) throw();
};

typedef OSStatus (*RenderCallbackFunction)(void*                      refCon,
                                           UInt32                     inNumberFrames,
                                           AudioUnitRenderActionFlags *ioActionFlags,
                                           const AudioTimeStamp 	  *inTimeStamp,
                                           AudioBufferList            *ioData);

/** An audio host for the iOS platform.
 @see PLAudioHost */
template<class SampleType>
class IOSAudioHostBase : public AudioHostBase<SampleType>
{
public:
    typedef typename AudioHostBase<SampleType>::BufferArray      BufferArray;
    typedef typename AudioHostBase<SampleType>::ConstBufferArray ConstBufferArray;
    typedef typename AudioHostBase<SampleType>::UnitType         UnitType;
    typedef NumericalArray<SampleType>                           Buffer;
    
    IOSAudioHostBase (ObjectMemoryBase* omb = ObjectMemory<ObjectMemoryDefault>::create()) throw();
    ~IOSAudioHostBase();
    
    Text getHostName() const throw();
    Text getNativeHostName() const throw();
    Text getInputName() const throw();
    Text getOutputName() const throw();
    double getCpuUsage() const throw() { return cpuUsage; }
    
    void startHost() throw();
    void stopHost() throw();
    void pauseHost() throw();
    void resumeHost() throw();
    
    virtual void hostInterruption (const bool flag) throw() { }
    inline bool wasInterrupted() throw() { return interrupted.getValue() != 0; }
    
    OSStatus renderCallback (UInt32                     inNumberFrames,
                             AudioUnitRenderActionFlags *ioActionFlags, 
                             const AudioTimeStamp 		*inTimeStamp, 
                             AudioBufferList            *ioData) throw();
    
    void propertyCallback (AudioSessionPropertyID   inID,
                           UInt32                   inDataSize,
                           const void *             inPropertyValue) throw();
    
    void interruptionCallback (UInt32 inInterruption) throw();
    
    void setCustomRenderCallbacks (void* refCon, RenderCallbackFunction preRender, RenderCallbackFunction postRender) throw();

        
private:
    void setFormat() throw();    
    int setupRemoteIO() throw();
    void killRemoteIO() throw();
    void resetIO() throw();
    void restart() throw();
    void fixAudioRouteIfSetToReceiver() throw();

    AudioStreamBasicDescription format;
	AURenderCallbackStruct		inputProc;
	Float64						hwSampleRate;
	AudioUnit					rioUnit;
    
    int							bufferSize;
	float						bufferDuration;
	double						reciprocalBufferDuration;
    Buffer                      convertBuffer;
	UInt32						audioInputIsAvailable;
	UInt32						numInputChannels;
	UInt32						numOutputChannels;
	double						cpuUsage;

    AtomicInt                   interrupted;
    
    void*                       customRenderCallbackRef;
    RenderCallbackFunction      customPreRender;
    RenderCallbackFunction      customPostRender;
};


// depracated but for backwards compatibility.
class IOSAudioHost : public IOSAudioHostBase<float>
{
public:
    IOSAudioHost (ObjectMemoryBase* omb = ObjectMemory<ObjectMemoryDefault>::create()) throw()
    :   IOSAudioHostBase<float> (omb)
    {
    }
};

END_PLONK_NAMESPACE

#if defined(__OBJC__) || DOXYGEN 

@class PLAudioHost;

using namespace plonk;

/** A protocol for a PLAudioHost delegate.
 This class should contain a constructGraph method that returns a Unit 
 conatining the audio graph to render at runtime. 
 @see PLAudioHost */
@protocol PLAudioHostDelegate <NSObject>
@optional
- (FloatUnit)constructGraphFloat:(PLAudioHost*)host;
- (ShortUnit)constructGraphShort:(PLAudioHost*)host;
- (IntUnit)constructGraphInt:(PLAudioHost*)host;
- (DoubleUnit)constructGraphDouble:(PLAudioHost*)host;
- (Unit)constructGraph:(PLAudioHost*)host DEPRECATED_ATTRIBUTE;
- (void)hostStarting:(PLAudioHost*)host;
- (void)hostStopped:(PLAudioHost*)host;
- (void)hostPaused:(PLAudioHost*)host;
- (void)hostResuming:(PLAudioHost*)host;
- (void)hostInterruption:(PLAudioHost*)host withFlag:(BOOL)flag;
@end


/** An Objective-C audio host for the iOS platform.
 This is simply an adapter for the C++ IOSAudioHost. You need to set an object
 that implements the PLAudioGraph protocol in the init method of your derived 
 class.
 
 When running in the simulator the preferred sample rate and block size
 properties may be ignored and default to the hardware settings of the Mac
 hosting the simulator.
 
 @see PLAudioGraph */
@interface PLAudioHost : NSObject  
{
    void* peer;
    id<PLAudioHostDelegate> delegate;
    int type;
}

@property (nonatomic, assign) id delegate;                  ///< The delegate that contains the constructGraph method.
@property (nonatomic, readonly) NSString* hostName;         ///< The host name - always "iOS".
@property (nonatomic, readonly) NSString* nativeHostName;   ///< The native host name - currently always "RemoteIO".
@property (nonatomic, readonly) NSString* inputName;        ///< The name of the input device. May be "Default Input" on the simulator.
@property (nonatomic, readonly) NSString* outputName;       ///< The name of the output device. May be "Default Output" on the simulator.
@property (nonatomic, readonly) double cpuUsage;            ///< The current CPU usage of the DSP loop.
@property (nonatomic, readonly) BOOL isRunning;             ///< Is the host running?
@property (nonatomic, readonly) BOOL isPaused;              ///< Is the host paused?
@property (nonatomic, readonly) BOOL wasInterrupted;        ///< Was the host interrupted?
@property (nonatomic, readonly) Dynamic outputUnit;         ///< The output unit of the host.
@property (nonatomic) int numInputs;                        ///< The number of audio inputs, only set this BEFORE sending the startHost message.
@property (nonatomic) int numOutputs;                       ///< The number of audio outputs, only set this BEFORE sending the startHost message.
@property (nonatomic) int preferredHostBlockSize;           ///< The preferred host block size, only set this BEFORE sending the startHost message.
@property (nonatomic) int preferredGraphBlockSize;          ///< The preferred graph block size, only set this BEFORE sending the startHost message.
@property (nonatomic) double preferredHostSampleRate;       ///< The preferred sample rate, only set this BEFORE sending the startHost message.

/** Start the host running. */
- (void)startHost;

/** Stop the host running. */
- (void)stopHost;

/** Pause the host. */
- (void)pauseHost;

/** Resume the host. */
- (void)resumeHost;

- (void)setCustomRenderCallbacksWithRef:(void*)refCon pre:(RenderCallbackFunction)preFunc post:(RenderCallbackFunction)postFunc;

@end

#endif

#define PLANK_INLINING_FUNCTIONS 1
#include "plonk_IOSAudioHostInline.h"
#undef PLANK_INLINING_FUNCTIONS

#endif  // PLONK_IOSAUDIOHOST_H
