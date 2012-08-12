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

#ifndef PLONK_IOSAUDIOHOST_H
#define PLONK_IOSAUDIOHOST_H

#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioServices.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>

BEGIN_PLONK_NAMESPACE

/** An audio host for the iOS platform.
 @see PLAudioHost */
template<class ObjectMemoryType = ObjectMemoryDefault>
class IOSAudioHost : public AudioHostBase<float,ObjectMemoryType>
{
public:
    typedef AudioHostBase<float>::UnitType UnitType;
    
    IOSAudioHost() throw();
    ~IOSAudioHost();
    
    Text getHostName() const throw();
    Text getNativeHostName() const throw();
    Text getInputName() const throw();
    Text getOutputName() const throw();
    double getCpuUsage() const throw() { return cpuUsage; }
    
    void startHost() throw();
    void stopHost() throw();

    OSStatus renderCallback (UInt32                     inNumberFrames,
                             AudioUnitRenderActionFlags *ioActionFlags, 
                             const AudioTimeStamp 		*inTimeStamp, 
                             AudioBufferList            *ioData) throw();
    
    void propertyCallback (AudioSessionPropertyID   inID,
                           UInt32                   inDataSize,
                           const void *             inPropertyValue) throw();
    
    void interruptionCallback (UInt32 inInterruption) throw();
        
private:
    void setFormat() throw();    
    int setupRemoteIO() throw();
    void restart() throw();
    void fixAudioRouteIfSetToReceiver() throw();

    AudioStreamBasicDescription format;
	AURenderCallbackStruct		inputProc;
	Float64						hwSampleRate;
	AudioUnit					rioUnit;
    
    int							bufferSize;
	float						bufferDuration;
	double						reciprocalBufferDuration;
	float						*floatBuffer;
	UInt32						audioInputIsAvailable;
	UInt32						numInputChannels;
	UInt32						numOutputChannels;
	double						cpuUsage;
};

END_PLONK_NAMESPACE

#if defined(__OBJC__) || DOXYGEN 

/** Convenience macro for adding member variables to Objective-C classes. 
 This helps declare the appropriate type of Plonk Variable<> e.g., for 
 providing an interface between a GUI and the audio engine. You then need to 
 declare the property using PLONK_OBJC_PROPERTY_DECLARE and synthesise the 
 getter and setter methods using PLONK_OBJC_PROPERTY_SYNTH.

 For example:
 
 @code
 @interface MyClass : NSObject
 {
    PLONK_OBJC_PROPERTY_MEMBER(float,frequency);
    PLONK_OBJC_PROPERTY_MEMBER(float,amplitude);
 }
 ...
 @endcode
 
 @see PLONK_OBJC_PROPERTY_DECLARE, PLONK_OBJC_PROPERTY_SYNTH */
#define PLONK_OBJC_PROPERTY_MEMBER(Type,Name)\
    Variable<Type> Name

/** Convenience macro for declaring Objective-C properites.
 This helps make the appropriate property declaration for a Plonk
 member variable declared using PLONK_OBJC_PROPERTY_MEMBER.
 
 For example:

 @code
 @interface MyClass : NSObject
 {
    PLONK_OBJC_PROPERTY_MEMBER(float,frequency);
    PLONK_OBJC_PROPERTY_MEMBER(float,amplitude);
 }
 PLONK_OBJC_PROPERTY_DECLARE(float,frequency); // instead of @property ..etc
 PLONK_OBJC_PROPERTY_DECLARE(float,amplitude); // instead of @property ..etc
 @end
 @endcode
 
 @see PLONK_OBJC_PROPERTY_MEMBER, PLONK_OBJC_PROPERTY_SYNTH */
#define PLONK_OBJC_PROPERTY_DECLARE(Type,Name)\
    @property (nonatomic,getter=get##Name,setter=set##Name:) Type Name

/** Convenience macro for synthesising properity methods.
 This helps synthesise the getter and setter methods for properties declared
 using PLONK_OBJC_PROPERTY_MEMBER and PLONK_OBJC_PROPERTY_DECLARE. 
  
 For example:
 
 @code
 @implementation AudioHost
 
 PLONK_OBJC_PROPERTY_SYNTH (float,frequency); // instead of @synthesize frequency;
 PLONK_OBJC_PROPERTY_SYNTH (float,amplitude); // instead of @synthesize amplitude;
 ...
 @end
 @endcode
 
 @see PLONK_OBJC_PROPERTY_MEMBER, PLONK_OBJC_PROPERTY_DECLARE */
#define PLONK_OBJC_PROPERTY_SYNTH(Type,Name)\
    - (Type)get##Name {\
        return Name.getValue();\
    }\
    - (void)set##Name:(Type)value {\
        Name.setValue (value);\
    }


#define PLUNIT plonk::IOSAudioHost::UnitType

@class PLAudioHost;

/** A protocol for a PLAudioHost delegate.
 This class should contain a constructGraph method that returns a Unit 
 conatining the audio graph to render at runtime. 
 @see PLAudioHost */
@protocol PLAudioHostDelegate <NSObject>
@required
- (PLUNIT)constructGraph:(PLAudioHost*)host;
@optional
- (void)hostStarting:(PLAudioHost*)host;
- (void)hostStopped:(PLAudioHost*)host;
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
}

@property (nonatomic, retain) id delegate;                  ///< The delegat that contains the constructGraph method.
@property (nonatomic, readonly) NSString* hostName;         ///< The host name - always "iOS".
@property (nonatomic, readonly) NSString* nativeHostName;   ///< The native host name - currently always "RemoteIO".
@property (nonatomic, readonly) NSString* inputName;        ///< The name of the input device. May be "Default Input" on the simulator.
@property (nonatomic, readonly) NSString* outputName;       ///< The name of the output device. May be "Default Output" on the simulator.
@property (nonatomic, readonly) double cpuUsage;            ///< The current CPU usage of the DSP loop.
@property (nonatomic, readonly) BOOL isRunning;             ///< Is the host running?
@property (nonatomic, readonly) PLUNIT outputUnit;          ///< The output unit of the host.
@property (nonatomic) int numInputs;                        ///< The number of audio inputs, only set this BEFORE sending the startHost message.
@property (nonatomic) int numOutputs;                       ///< The number of audio outputs, only set this BEFORE sending the startHost message.
@property (nonatomic) int preferredBlockSize;               ///< The preferred block size, only set this BEFORE sending the startHost message.
@property (nonatomic) double preferredSampleRate;           ///< The preferred sample rate, only set this BEFORE sending the startHost message.
@property (nonatomic) UInt32 category;                      ///< The audio session category (kAudioSessionProperty_AudioCategory), only set this BEFORE sending the startHost message.
@property (nonatomic) UInt32 mode;                          ///< The audio session mode (kAudioSessionProperty_Mode), only set this BEFORE sending the startHost message.

/** Start the host running. */
- (void)startHost;

/** Stop the host running. */
- (void)stopHost;

@end

#endif


#endif  // PLONK_IOSAUDIOHOST_H
