/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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
static PLONK_INLINE_LOW int paCallback (const void *input, void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData)
{
    PortAudioAudioHostBase<SampleType>* host = static_cast<PortAudioAudioHostBase<SampleType>*> (userData);
    return host->callback ((const SampleType**)input, (SampleType**)output, 
                           frameCount, timeInfo, statusFlags);
}

static PLONK_INLINE_LOW bool paCheckError (PaError err)
{
    if (err != paNoError)
    {
        printf ("paerror '%s' (%d)\n", Pa_GetErrorText (err), err);
        return true;
    }
    
    return false; 
}

//static PLONK_INLINE_LOW void paGetInfo()
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
PortAudioAudioHostBase<SampleType>::PortAudioAudioHostBase() throw() 
:   stream (0)
{
    PaError err;
	err = Pa_Initialize();
    paCheckError (err);
    
    this->setPreferredHostBlockSize (512);
    this->setPreferredGraphBlockSize (128);
    this->setPreferredHostSampleRate (44100.0);
    this->setNumInputs (1);
    this->setNumOutputs (2);
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
        case TypeCode::Short: sampleFormat = paInt16;   break;
        case TypeCode::Int:   sampleFormat = paInt32;   break;
        case TypeCode::Int24: sampleFormat = paInt24;   break;
        case TypeCode::Char:  sampleFormat = paInt8;    break;
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
    
    this->setPreferredHostBlockSize ((int)frameCount);
    
    ConstBufferArray& inputs = this->getInputs();
    BufferArray& outputs = this->getOutputs();

    const int numInputs = inputs.length();
    const int numOutputs = outputs.length();
    
    int i;
    
    for (i = 0; i < numInputs; ++i)
    {
        const SampleType* const inputChannel = inputData[i];
        inputs.atUnchecked (i) = inputChannel;
    }
    
    for (i = 0; i < numOutputs; ++i)
    {
        SampleType* const outputChannel = outputData[i];
        outputs.atUnchecked (i) = outputChannel;
    }
    
    this->process();
    
    return paContinue;
}


#if (defined(__OBJC__) && !PLONK_IOS)|| DOXYGEN
END_PLONK_NAMESPACE

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
- (void)hostStarting:(PLAudioHost*)host;
- (void)hostStopped:(PLAudioHost*)host;
@end


/** An Objective-C audio host for the iOS platform.
 This is simply an adapter for the C++ IOSAudioHost. You need to set an object
 that implements the PLAudioGraph protocol in the init method of your derived
 class.
 
 @see PLAudioHostDelegate */
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

@end

BEGIN_PLONK_NAMESPACE

template<class SampleType>
class PortAudioAudioHostPeerBase : public PortAudioAudioHostBase<SampleType>
{
public:
    typedef typename PortAudioAudioHostBase<SampleType>::UnitType UnitType;
    
    PortAudioAudioHostPeerBase (PLAudioHost* p)
    :   peer (p)
    {
    }
    
    void hostStarting() throw()
    {
        if ([peer.delegate respondsToSelector:@selector(hostStarting:)])
            [peer.delegate hostStarting:peer];
    }
    
    void hostStopped() throw()
    {
        if ([peer.delegate respondsToSelector:@selector(hostStopped:)])
            [peer.delegate hostStopped:peer];
    }
        
    PLAudioHost* getPeer() const throw() { return peer; }
    
private:
    PLAudioHost* peer; // no need to retain as the Obj-C peer owns this object
    
    PortAudioAudioHostPeerBase();
};

/** A base, actual implementations are all in template specialisations. */
template<class SampleType>
class PortAudioAudioHostPeer : public PortAudioAudioHostPeerBase<SampleType>
{
public:
    // pure virtual constructGraph not implemented to cause a compile-time error
    // only float, short, int and double sample types are supported
};

template<>
class PortAudioAudioHostPeer<float> : public PortAudioAudioHostPeerBase<float>
{
public:
    PortAudioAudioHostPeer (PLAudioHost* peer)
    :   PortAudioAudioHostPeerBase<float> (peer)
    {
    }
    
    FloatUnit constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        plonk_assert (peer.delegate != nil);
        return [peer.delegate constructGraphFloat:peer];
    }
};

template<>
class PortAudioAudioHostPeer<short> : public PortAudioAudioHostPeerBase<short>
{
public:
    PortAudioAudioHostPeer (PLAudioHost* peer)
    :   PortAudioAudioHostPeerBase<short> (peer)
    {
    }
    
    ShortUnit constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        plonk_assert (peer.delegate != nil);
        return [peer.delegate constructGraphShort:peer];
    }
};

template<>
class PortAudioAudioHostPeer<int> : public PortAudioAudioHostPeerBase<int>
{
public:
    PortAudioAudioHostPeer (PLAudioHost* peer)
    :   PortAudioAudioHostPeerBase<int> (peer)
    {
    }
    
    IntUnit constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        plonk_assert (peer.delegate != nil);
        return [peer.delegate constructGraphInt:peer];
    }
};

template<>
class PortAudioAudioHostPeer<double> : public PortAudioAudioHostPeerBase<double>
{
public:
    PortAudioAudioHostPeer (PLAudioHost* peer)
    :   PortAudioAudioHostPeerBase<double> (peer)
    {
    }
    
    DoubleUnit constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        plonk_assert (peer.delegate != nil);
        return [peer.delegate constructGraphInt:peer];
    }
};

#if PLONK_AUDIOHOST_PORTAUDIO_COMPILEOBJC

END_PLONK_NAMESPACE

using namespace plonk;

@implementation PLAudioHost

- (id)init
{
    if (self = [super init])
    {
        peer = nil;
        type = TypeCode::Unknown;
    }
    
    return self;
}

- (void)dealloc
{
    self.delegate = nil;
    
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

- (NSString*)hostName
{
    if (peer == nil) return @"";
    
    switch ((const int)type)
    {
        case TypeCode::Float:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<float>* > (peer)->getHostName().getArray()];
            
        case TypeCode::Short:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<short>* > (peer)->getHostName().getArray()];
            
        case TypeCode::Int:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<int>* > (peer)->getNativeHostName().getArray()];
            
        case TypeCode::Double:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<double>* > (peer)->getNativeHostName().getArray()];
            
        default:
            return @"";
    }
}

- (NSString*)nativeHostName
{
    if (peer == nil) return @"";
    
    switch ((const int)type)
    {
        case TypeCode::Float:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<float>* > (peer)->getNativeHostName().getArray()];
            
        case TypeCode::Short:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<short>* > (peer)->getNativeHostName().getArray()];
            
        case TypeCode::Int:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<int>* > (peer)->getNativeHostName().getArray()];
            
        case TypeCode::Double:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<double>* > (peer)->getNativeHostName().getArray()];
            
        default:
            return @"";
    }
}

- (NSString*)inputName
{
    if (peer == nil) return @"";
    
    switch ((const int)type)
    {
        case TypeCode::Float:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<float>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Short:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<short>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Int:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<int>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Double:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<double>* > (peer)->getInputName().getArray()];
            
        default:
            return @"";
    }
}

- (NSString*)outputName
{
    if (peer == nil) return @"";
    
    switch ((const int)type)
    {
        case TypeCode::Float:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<float>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Short:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<short>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Int:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<int>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Double:
            return [NSString stringWithUTF8String: static_cast< PortAudioAudioHostPeer<double>* > (peer)->getInputName().getArray()];
            
        default:
            return @"";
    }
}

- (double)cpuUsage
{
    if (peer == nil) return 0.0;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getCpuUsage();
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getCpuUsage();
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getCpuUsage();
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getCpuUsage();
        default: return 0.0;
    }
}

- (BOOL)isRunning
{
    if (peer == nil) return NO;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getIsRunning() ? YES : NO;
        default: return NO;
    }
}

- (BOOL)isPaused
{
    if (peer == nil) return NO;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getIsPaused() ? YES : NO;
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getIsPaused() ? YES : NO;
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getIsPaused() ? YES : NO;
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getIsPaused() ? YES : NO;
        default: return NO;
    }
}

- (Dynamic)outputUnit
{
    Dynamic dyn;
    
    if (peer != nil)
    {
        switch ((const int)type)
        {
            case TypeCode::Float:   dyn = static_cast< PortAudioAudioHostPeer<float>* > (peer)->getOutputUnit(); break;
            case TypeCode::Short:   dyn = static_cast< PortAudioAudioHostPeer<short>* > (peer)->getOutputUnit(); break;
            case TypeCode::Int:     dyn = static_cast< PortAudioAudioHostPeer<int>* > (peer)->getOutputUnit(); break;
            case TypeCode::Double:  dyn = static_cast< PortAudioAudioHostPeer<double>* > (peer)->getOutputUnit(); break;
            default: { }
        }
    }
    
    return dyn;
}

- (int)numInputs
{
    if (peer == nil) return 0;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getNumInputs();
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getNumInputs();
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getNumInputs();
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getNumInputs();
        default: return 0.0;
    }
}

- (void)setNumInputs:(int)numInputs
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->setNumInputs (numInputs); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->setNumInputs (numInputs); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->setNumInputs (numInputs); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->setNumInputs (numInputs); break;
        default: { }
    }
}

- (int)numOutputs
{
    if (peer == nil) return 0;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getNumOutputs();
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getNumOutputs();
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getNumOutputs();
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getNumOutputs();
        default: return 0.0;
    }
}

- (void)setNumOutputs:(int)numOutputs
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->setNumOutputs (numOutputs); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->setNumOutputs (numOutputs); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->setNumOutputs (numOutputs); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->setNumOutputs (numOutputs); break;
        default: { }
    }
}

- (int)preferredHostBlockSize
{
    if (peer == nil) return 0;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getPreferredHostBlockSize();
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getPreferredHostBlockSize();
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getPreferredHostBlockSize();
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getPreferredHostBlockSize();
        default: return 0;
    }
}

- (void)setPreferredHostBlockSize:(int)preferredHostBlockSize
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        default: { }
    }
}

- (int)preferredGraphBlockSize
{
    if (peer == nil) return 0;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getPreferredGraphBlockSize();
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getPreferredGraphBlockSize();
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getPreferredGraphBlockSize();
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getPreferredGraphBlockSize();
        default: return 0;
    }
}

- (void)setPreferredGraphBlockSize:(int)preferredGraphBlockSize
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        default: { }
    }
}

- (double)preferredHostSampleRate
{
    if (peer == nil) return 0.0;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< PortAudioAudioHostPeer<float>* > (peer)->getPreferredHostSampleRate();
        case TypeCode::Short:   return static_cast< PortAudioAudioHostPeer<short>* > (peer)->getPreferredHostSampleRate();
        case TypeCode::Int:     return static_cast< PortAudioAudioHostPeer<int>* > (peer)->getPreferredHostSampleRate();
        case TypeCode::Double:  return static_cast< PortAudioAudioHostPeer<double>* > (peer)->getPreferredHostSampleRate();
        default: return 0.0;
    }
}

- (void)setPreferredHostSampleRate:(double)preferredHostSampleRate
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        default: { }
    }
}

- (void)startHost
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->startHost(); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->startHost(); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->startHost(); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->startHost(); break;
        default: { }
    }
}

- (void)stopHost
{
    if (peer == nil) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< PortAudioAudioHostPeer<float>* > (peer)->stopHost(); break;
        case TypeCode::Short:   static_cast< PortAudioAudioHostPeer<short>* > (peer)->stopHost(); break;
        case TypeCode::Int:     static_cast< PortAudioAudioHostPeer<int>* > (peer)->stopHost(); break;
        case TypeCode::Double:  static_cast< PortAudioAudioHostPeer<double>* > (peer)->stopHost(); break;
        default: { }
    }
}

- (id)delegate
{
    return delegate;
}

- (void)deletePeer
{
    switch ((const int)type)
    {
        case TypeCode::Float:   delete static_cast< PortAudioAudioHostPeer<float>* > (peer);   break;
        case TypeCode::Short:   delete static_cast< PortAudioAudioHostPeer<short>* > (peer);   break;
        case TypeCode::Int:     delete static_cast< PortAudioAudioHostPeer<int>* > (peer);     break;
        case TypeCode::Double:  delete static_cast< PortAudioAudioHostPeer<double>* > (peer);  break;
        default: { }
    }
    
    peer = nil;
    type = TypeCode::Unknown;
}

- (void)setDelegate:(id)newDelegate
{
    plonk_assert (delegate == nil || newDelegate == nil);
    
    [self deletePeer];
    
    delegate = newDelegate;
    
    if (delegate)
    {
        const unsigned int isFloat = [delegate respondsToSelector:@selector(constructGraphFloat:)] ? (1 << TypeCode::Float) : 0;
        const unsigned int isShort = [delegate respondsToSelector:@selector(constructGraphShort:)] ? (1 << TypeCode::Short) : 0;
        const unsigned int isInt = [delegate respondsToSelector:@selector(constructGraphInt:)] ? (1 << TypeCode::Int) : 0;
        const unsigned int isDouble = [delegate respondsToSelector:@selector(constructGraphDouble:)] ? (1 << TypeCode::Double) : 0;
        const unsigned int typeFlags = isFloat | isShort | isInt | isDouble;
        
        if (typeFlags != 0 && Bits::isPowerOf2 (typeFlags))
        {
            type = Bits::countTrailingZeroes (typeFlags);
            
            switch ((const int)type)
            {
                case TypeCode::Float:   peer = new PortAudioAudioHostPeer<float> (self);   break;
                case TypeCode::Short:   peer = new PortAudioAudioHostPeer<short> (self);   break;
                case TypeCode::Int:     peer = new PortAudioAudioHostPeer<int> (self);     break;
                case TypeCode::Double:  peer = new PortAudioAudioHostPeer<double> (self);  break;
                    
                default:
                    peer = nil;
                    type = 0;
            }
        }
        else plonk_assertfalse; // either none or more than one constructGraph.. function was implemented
    }
}

@end

BEGIN_PLONK_NAMESPACE
#endif

#endif


#endif // PLANK_INLINING_FUNCTIONS

