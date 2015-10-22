/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

template<class SampleType>
static PLONK_INLINE_LOW int rtAudioCallback(void *outputBuffer, void *inputBuffer,
                                  unsigned int nBufferFrames,
                                  double streamTime,
                                  RtAudioStreamStatus status,
                                  void *userData)
{
    RTAudioAudioHostBase<SampleType>* host = static_cast<RTAudioAudioHostBase<SampleType>*> (userData);
    return host->callback ((SampleType*)outputBuffer, (const SampleType*)inputBuffer,
                           nBufferFrames, streamTime, status);
}


static void rtAudioErrorCallback (RtError::Type type, const std::string &errorText)
{
    //
}


//------------------------------------------------------------------------------

template<class SampleType>
RTAudioAudioHostBase<SampleType>::RTAudioAudioHostBase() throw() 
{
    inputParams.deviceId = dac.getDefaultInputDevice();
    outputParams.deviceId = dac.getDefaultOutputDevice();

    this->setPreferredHostBlockSize (512);
    this->setPreferredGraphBlockSize (128);
    this->setPreferredHostSampleRate (44100.0);
    this->setNumInputs (1);
    this->setNumOutputs (2);
}

template<class SampleType>
RTAudioAudioHostBase<SampleType>::~RTAudioAudioHostBase()
{
    stopHost();
}

template<class SampleType>
Text RTAudioAudioHostBase<SampleType>::getHostName() const throw()
{
    return "RTAudio (" + TypeUtility<SampleType>::getTypeName() + ")";
}

template<class SampleType>
Text RTAudioAudioHostBase<SampleType>::getNativeHostName() const throw()
{
    switch (const_cast<RtAudio&> (dac).getCurrentApi())
    {
        case RtAudio::UNSPECIFIED:      return "UNSPECIFIED";
        case RtAudio::LINUX_ALSA:       return "LINUX_ALSA";
        case RtAudio::LINUX_PULSE:      return "LINUX_PULSE";
        case RtAudio::LINUX_OSS:        return "LINUX_OSS";
        case RtAudio::UNIX_JACK:        return "UNIX_JACK";
        case RtAudio::MACOSX_CORE:      return "MACOSX_CORE";
        case RtAudio::WINDOWS_ASIO:     return "WINDOWS_ASIO";
        case RtAudio::WINDOWS_DS:       return "WINDOWS_DS";
        case RtAudio::RTAUDIO_DUMMY:    return "RTAUDIO_DUMMY";
        default:                        return "INVALID";
    }
}

template<class SampleType>
Text RTAudioAudioHostBase<SampleType>::getInputName() const throw()
{
    const RtAudio::DeviceInfo deviceInfo = const_cast<RtAudio&> (dac).getDeviceInfo (inputParams.deviceId);
    return deviceInfo.name.c_str();
}

template<class SampleType>
Text RTAudioAudioHostBase<SampleType>::getOutputName() const throw()
{
    const RtAudio::DeviceInfo deviceInfo = const_cast<RtAudio&> (dac).getDeviceInfo (outputParams.deviceId);
    return deviceInfo.name.c_str();
}

template<class SampleType>
double RTAudioAudioHostBase<SampleType>::getCpuUsage() const throw()
{
    plonk_assertfalse;
    return 0.0;
}

template<class SampleType>
void RTAudioAudioHostBase<SampleType>::stopHost() throw()
{
    dac.stopStream();
    this->setIsRunning (false);
}

template<class SampleType>
void RTAudioAudioHostBase<SampleType>::startHost() throw()
{
    RtAudioFormat sampleFormat;
    
    switch (TypeUtility<SampleType>::getTypeCode())
    {
        case TypeCode::Double: sampleFormat = RTAUDIO_FLOAT64;  break;
        case TypeCode::Float:  sampleFormat = RTAUDIO_FLOAT32;  break;
        case TypeCode::Short:  sampleFormat = RTAUDIO_SINT16;   break;
        case TypeCode::Int:    sampleFormat = RTAUDIO_SINT32;   break;
        case TypeCode::Int24:  sampleFormat = RTAUDIO_SINT24;   break;
        case TypeCode::Char:   sampleFormat = RTAUDIO_SINT8;    break;
        default: sampleFormat = 0; break;
    }
    
    int err;
    outputParams.nChannels = this->getNumOutputs();    
    inputParams.nChannels = this->getNumInputs();

    if (sampleFormat != 0)
    {
        unsigned int bufferFrames = (unsigned int)this->getPreferredHostBlockSize();
        
        RtAudio::StreamOptions streamOptions;
        streamOptions.flags |= RTAUDIO_NONINTERLEAVED;
        
        dac.openStream (outputParams.nChannels > 0 ? &outputParams : NULL,
                        inputParams.nChannels > 0 ? &inputParams : NULL,
                        sampleFormat,
                        this->getPreferredHostSampleRate(),
                        &bufferFrames,
                        rtAudioCallback<SampleType>,
                        this,
                        &streamOptions,
                        rtAudioErrorCallback);
                
        this->startHostInternal();
        dac.startStream();
    }
    else plonk_assertfalse;
}

template<class SampleType>
int RTAudioAudioHostBase<SampleType>::callback (SampleType* outputData, const SampleType* inputData,
                                                unsigned int nBufferFrames,
                                                double streamTime, RtAudioStreamStatus status) throw()
{
    (void)streamTime;
    (void)status;
    
    this->setPreferredHostBlockSize ((int)nBufferFrames);
    
    ConstBufferArray& inputs = this->getInputs();
    BufferArray& outputs = this->getOutputs();

    const int numInputs = inputs.length();
    const int numOutputs = outputs.length();
    
    int i;
    
    for (i = 0; i < numInputs; ++i)
    {
        inputs.atUnchecked (i) = inputData;
        inputData += nBufferFrames;
    }
    
    for (i = 0; i < numOutputs; ++i)
    {
        outputs.atUnchecked (i) = outputData;
        outputData += nBufferFrames;
    }
    
    this->process();
    
    return 0;
}


//#if (defined(__OBJC__) && !PLONK_IOS)|| DOXYGEN
//END_PLONK_NAMESPACE
//
//@class PLAudioHost;
//
//using namespace plonk;
//
///** A protocol for a PLAudioHost delegate.
// This class should contain a constructGraph method that returns a Unit
// conatining the audio graph to render at runtime.
// @see PLAudioHost */
//@protocol PLAudioHostDelegate <NSObject>
//@optional
//- (FloatUnit)constructGraphFloat:(PLAudioHost*)host;
//- (ShortUnit)constructGraphShort:(PLAudioHost*)host;
//- (IntUnit)constructGraphInt:(PLAudioHost*)host;
//- (DoubleUnit)constructGraphDouble:(PLAudioHost*)host;
//- (void)hostStarting:(PLAudioHost*)host;
//- (void)hostStopped:(PLAudioHost*)host;
//@end
//
//
///** An Objective-C audio host for the iOS platform.
// This is simply an adapter for the C++ IOSAudioHost. You need to set an object
// that implements the PLAudioGraph protocol in the init method of your derived
// class.
// 
// When running in the simulator the preferred sample rate and block size
// properties may be ignored and default to the hardware settings of the Mac
// hosting the simulator.
// 
// @see PLAudioGraph */
//@interface PLAudioHost : NSObject
//{
//    void* peer;
//    id<PLAudioHostDelegate> delegate;
//    int type;
//}
//
//@property (nonatomic, assign) id delegate;                  ///< The delegate that contains the constructGraph method.
//@property (nonatomic, readonly) NSString* hostName;         ///< The host name - always "iOS".
//@property (nonatomic, readonly) NSString* nativeHostName;   ///< The native host name - currently always "RemoteIO".
//@property (nonatomic, readonly) NSString* inputName;        ///< The name of the input device. May be "Default Input" on the simulator.
//@property (nonatomic, readonly) NSString* outputName;       ///< The name of the output device. May be "Default Output" on the simulator.
//@property (nonatomic, readonly) double cpuUsage;            ///< The current CPU usage of the DSP loop.
//@property (nonatomic, readonly) BOOL isRunning;             ///< Is the host running?
//@property (nonatomic, readonly) Dynamic outputUnit;         ///< The output unit of the host.
//@property (nonatomic) int numInputs;                        ///< The number of audio inputs, only set this BEFORE sending the startHost message.
//@property (nonatomic) int numOutputs;                       ///< The number of audio outputs, only set this BEFORE sending the startHost message.
//@property (nonatomic) int preferredHostBlockSize;           ///< The preferred host block size, only set this BEFORE sending the startHost message.
//@property (nonatomic) int preferredGraphBlockSize;          ///< The preferred graph block size, only set this BEFORE sending the startHost message.
//@property (nonatomic) double preferredHostSampleRate;       ///< The preferred sample rate, only set this BEFORE sending the startHost message.
//
///** Start the host running. */
//- (void)startHost;
//
///** Stop the host running. */
//- (void)stopHost;
//
//@end
//
//BEGIN_PLONK_NAMESPACE
//
//template<class SampleType>
//class RTAudioAudioHostPeerBase : public RTAudioAudioHostBase<SampleType>
//{
//public:
//    typedef typename RTAudioAudioHostBase<SampleType>::UnitType UnitType;
//    
//    RTAudioAudioHostPeerBase (PLAudioHost* p)
//    :   peer (p)
//    {
//    }
//    
//    void hostStarting() throw()
//    {
//        if ([peer.delegate respondsToSelector:@selector(hostStarting:)])
//            [peer.delegate hostStarting:peer];
//    }
//    
//    void hostStopped() throw()
//    {
//        if ([peer.delegate respondsToSelector:@selector(hostStopped:)])
//            [peer.delegate hostStopped:peer];
//    }
//        
//    PLAudioHost* getPeer() const throw() { return peer; }
//    
//private:
//    PLAudioHost* peer; // no need to retain as the Obj-C peer owns this object
//    
//    RTAudioAudioHostPeerBase();
//};
//
///** A base, actual implementations are all in template specialisations. */
//template<class SampleType>
//class RTAudioAudioHostPeer : public RTAudioAudioHostPeerBase<SampleType>
//{
//public:
//    // pure virtual constructGraph not implemented to cause a compile-time error
//    // only float, short, int and double sample types are supported
//};
//
//template<>
//class RTAudioAudioHostPeer<float> : public RTAudioAudioHostPeerBase<float>
//{
//public:
//    RTAudioAudioHostPeer (PLAudioHost* peer)
//    :   RTAudioAudioHostPeerBase<float> (peer)
//    {
//    }
//    
//    FloatUnit constructGraph() throw()
//    {
//        PLAudioHost* const peer = this->getPeer();
//        plonk_assert (peer.delegate != nil);
//        return [peer.delegate constructGraphFloat:peer];
//    }
//};
//
//template<>
//class RTAudioAudioHostPeer<short> : public RTAudioAudioHostPeerBase<short>
//{
//public:
//    RTAudioAudioHostPeer (PLAudioHost* peer)
//    :   RTAudioAudioHostPeerBase<short> (peer)
//    {
//    }
//    
//    ShortUnit constructGraph() throw()
//    {
//        PLAudioHost* const peer = this->getPeer();
//        plonk_assert (peer.delegate != nil);
//        return [peer.delegate constructGraphShort:peer];
//    }
//};
//
//template<>
//class RTAudioAudioHostPeer<int> : public RTAudioAudioHostPeerBase<int>
//{
//public:
//    RTAudioAudioHostPeer (PLAudioHost* peer)
//    :   RTAudioAudioHostPeerBase<int> (peer)
//    {
//    }
//    
//    IntUnit constructGraph() throw()
//    {
//        PLAudioHost* const peer = this->getPeer();
//        plonk_assert (peer.delegate != nil);
//        return [peer.delegate constructGraphInt:peer];
//    }
//};
//
//template<>
//class RTAudioAudioHostPeer<double> : public RTAudioAudioHostPeerBase<double>
//{
//public:
//    RTAudioAudioHostPeer (PLAudioHost* peer)
//    :   RTAudioAudioHostPeerBase<double> (peer)
//    {
//    }
//    
//    DoubleUnit constructGraph() throw()
//    {
//        PLAudioHost* const peer = this->getPeer();
//        plonk_assert (peer.delegate != nil);
//        return [peer.delegate constructGraphInt:peer];
//    }
//};
//
//#if PLONK_AUDIOHOST_PORTAUDIO_COMPILEOBJC
//
//END_PLONK_NAMESPACE
//
//using namespace plonk;
//
//@implementation PLAudioHost
//
//- (id)init
//{
//    if (self = [super init])
//    {
//        peer = nil;
//        type = TypeCode::Unknown;
//    }
//    
//    return self;
//}
//
//- (void)dealloc
//{
//    self.delegate = nil;
//    
//#if !__has_feature(objc_arc)
//    [super dealloc];
//#endif
//}
//
//- (NSString*)hostName
//{
//    if (peer == nil) return @"";
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<float>* > (peer)->getHostName().getArray()];
//            
//        case TypeCode::Short:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<short>* > (peer)->getHostName().getArray()];
//            
//        case TypeCode::Int:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<int>* > (peer)->getNativeHostName().getArray()];
//            
//        case TypeCode::Double:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<double>* > (peer)->getNativeHostName().getArray()];
//            
//        default:
//            return @"";
//    }
//}
//
//- (NSString*)nativeHostName
//{
//    if (peer == nil) return @"";
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<float>* > (peer)->getNativeHostName().getArray()];
//            
//        case TypeCode::Short:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<short>* > (peer)->getNativeHostName().getArray()];
//            
//        case TypeCode::Int:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<int>* > (peer)->getNativeHostName().getArray()];
//            
//        case TypeCode::Double:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<double>* > (peer)->getNativeHostName().getArray()];
//            
//        default:
//            return @"";
//    }
//}
//
//- (NSString*)inputName
//{
//    if (peer == nil) return @"";
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<float>* > (peer)->getInputName().getArray()];
//            
//        case TypeCode::Short:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<short>* > (peer)->getInputName().getArray()];
//            
//        case TypeCode::Int:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<int>* > (peer)->getInputName().getArray()];
//            
//        case TypeCode::Double:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<double>* > (peer)->getInputName().getArray()];
//            
//        default:
//            return @"";
//    }
//}
//
//- (NSString*)outputName
//{
//    if (peer == nil) return @"";
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<float>* > (peer)->getInputName().getArray()];
//            
//        case TypeCode::Short:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<short>* > (peer)->getInputName().getArray()];
//            
//        case TypeCode::Int:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<int>* > (peer)->getInputName().getArray()];
//            
//        case TypeCode::Double:
//            return [NSString stringWithUTF8String: static_cast< RTAudioAudioHostPeer<double>* > (peer)->getInputName().getArray()];
//            
//        default:
//            return @"";
//    }
//}
//
//- (double)cpuUsage
//{
//    if (peer == nil) return 0.0;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getCpuUsage();
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getCpuUsage();
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getCpuUsage();
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getCpuUsage();
//        default: return 0.0;
//    }
//}
//
//- (BOOL)isRunning
//{
//    if (peer == nil) return NO;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getIsRunning() ? YES : NO;
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getIsRunning() ? YES : NO;
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getIsRunning() ? YES : NO;
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getIsRunning() ? YES : NO;
//        default: return NO;
//    }
//}
//
//- (BOOL)isPaused
//{
//    if (peer == nil) return NO;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getIsPaused() ? YES : NO;
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getIsPaused() ? YES : NO;
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getIsPaused() ? YES : NO;
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getIsPaused() ? YES : NO;
//        default: return NO;
//    }
//}
//
//- (Dynamic)outputUnit
//{
//    Dynamic dyn;
//    
//    if (peer != nil)
//    {
//        switch ((const int)type)
//        {
//            case TypeCode::Float:   dyn = static_cast< RTAudioAudioHostPeer<float>* > (peer)->getOutputUnit(); break;
//            case TypeCode::Short:   dyn = static_cast< RTAudioAudioHostPeer<short>* > (peer)->getOutputUnit(); break;
//            case TypeCode::Int:     dyn = static_cast< RTAudioAudioHostPeer<int>* > (peer)->getOutputUnit(); break;
//            case TypeCode::Double:  dyn = static_cast< RTAudioAudioHostPeer<double>* > (peer)->getOutputUnit(); break;
//            default: { }
//        }
//    }
//    
//    return dyn;
//}
//
//- (int)numInputs
//{
//    if (peer == nil) return 0;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getNumInputs();
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getNumInputs();
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getNumInputs();
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getNumInputs();
//        default: return 0.0;
//    }
//}
//
//- (void)setNumInputs:(int)numInputs
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->setNumInputs (numInputs); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->setNumInputs (numInputs); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->setNumInputs (numInputs); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->setNumInputs (numInputs); break;
//        default: { }
//    }
//}
//
//- (int)numOutputs
//{
//    if (peer == nil) return 0;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getNumOutputs();
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getNumOutputs();
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getNumOutputs();
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getNumOutputs();
//        default: return 0.0;
//    }
//}
//
//- (void)setNumOutputs:(int)numOutputs
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->setNumOutputs (numOutputs); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->setNumOutputs (numOutputs); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->setNumOutputs (numOutputs); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->setNumOutputs (numOutputs); break;
//        default: { }
//    }
//}
//
//- (int)preferredHostBlockSize
//{
//    if (peer == nil) return 0;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getPreferredHostBlockSize();
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getPreferredHostBlockSize();
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getPreferredHostBlockSize();
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getPreferredHostBlockSize();
//        default: return 0;
//    }
//}
//
//- (void)setPreferredHostBlockSize:(int)preferredHostBlockSize
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
//        default: { }
//    }
//}
//
//- (int)preferredGraphBlockSize
//{
//    if (peer == nil) return 0;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getPreferredGraphBlockSize();
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getPreferredGraphBlockSize();
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getPreferredGraphBlockSize();
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getPreferredGraphBlockSize();
//        default: return 0;
//    }
//}
//
//- (void)setPreferredGraphBlockSize:(int)preferredGraphBlockSize
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
//        default: { }
//    }
//}
//
//- (double)preferredHostSampleRate
//{
//    if (peer == nil) return 0.0;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   return static_cast< RTAudioAudioHostPeer<float>* > (peer)->getPreferredHostSampleRate();
//        case TypeCode::Short:   return static_cast< RTAudioAudioHostPeer<short>* > (peer)->getPreferredHostSampleRate();
//        case TypeCode::Int:     return static_cast< RTAudioAudioHostPeer<int>* > (peer)->getPreferredHostSampleRate();
//        case TypeCode::Double:  return static_cast< RTAudioAudioHostPeer<double>* > (peer)->getPreferredHostSampleRate();
//        default: return 0.0;
//    }
//}
//
//- (void)setPreferredHostSampleRate:(double)preferredHostSampleRate
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
//        default: { }
//    }
//}
//
//- (void)startHost
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->startHost(); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->startHost(); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->startHost(); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->startHost(); break;
//        default: { }
//    }
//}
//
//- (void)stopHost
//{
//    if (peer == nil) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< RTAudioAudioHostPeer<float>* > (peer)->stopHost(); break;
//        case TypeCode::Short:   static_cast< RTAudioAudioHostPeer<short>* > (peer)->stopHost(); break;
//        case TypeCode::Int:     static_cast< RTAudioAudioHostPeer<int>* > (peer)->stopHost(); break;
//        case TypeCode::Double:  static_cast< RTAudioAudioHostPeer<double>* > (peer)->stopHost(); break;
//        default: { }
//    }
//}
//
//- (id)delegate
//{
//    return delegate;
//}
//
//- (void)deletePeer
//{
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   delete static_cast< RTAudioAudioHostPeer<float>* > (peer);   break;
//        case TypeCode::Short:   delete static_cast< RTAudioAudioHostPeer<short>* > (peer);   break;
//        case TypeCode::Int:     delete static_cast< RTAudioAudioHostPeer<int>* > (peer);     break;
//        case TypeCode::Double:  delete static_cast< RTAudioAudioHostPeer<double>* > (peer);  break;
//        default: { }
//    }
//    
//    peer = nil;
//    type = TypeCode::Unknown;
//}
//
//- (void)setDelegate:(id)newDelegate
//{
//    plonk_assert (delegate == nil || newDelegate == nil);
//    
//    [self deletePeer];
//    
//    delegate = newDelegate;
//    
//    if (delegate)
//    {
//        const unsigned int isFloat = [delegate respondsToSelector:@selector(constructGraphFloat:)] ? (1 << TypeCode::Float) : 0;
//        const unsigned int isShort = [delegate respondsToSelector:@selector(constructGraphShort:)] ? (1 << TypeCode::Short) : 0;
//        const unsigned int isInt = [delegate respondsToSelector:@selector(constructGraphInt:)] ? (1 << TypeCode::Int) : 0;
//        const unsigned int isDouble = [delegate respondsToSelector:@selector(constructGraphDouble:)] ? (1 << TypeCode::Double) : 0;
//        const unsigned int typeFlags = isFloat | isShort | isInt | isDouble;
//        
//        if (typeFlags != 0 && Bits::isPowerOf2 (typeFlags))
//        {
//            type = Bits::countTrailingZeroes (typeFlags);
//            
//            switch ((const int)type)
//            {
//                case TypeCode::Float:   peer = new RTAudioAudioHostPeer<float> (self);   break;
//                case TypeCode::Short:   peer = new RTAudioAudioHostPeer<short> (self);   break;
//                case TypeCode::Int:     peer = new RTAudioAudioHostPeer<int> (self);     break;
//                case TypeCode::Double:  peer = new RTAudioAudioHostPeer<double> (self);  break;
//                    
//                default:
//                    peer = nil;
//                    type = 0;
//            }
//        }
//        else plonk_assertfalse; // either none or more than one constructGraph.. function was implemented
//    }
//}
//
//@end
//
//BEGIN_PLONK_NAMESPACE
//#endif
//
//#endif


#endif // PLANK_INLINING_FUNCTIONS

