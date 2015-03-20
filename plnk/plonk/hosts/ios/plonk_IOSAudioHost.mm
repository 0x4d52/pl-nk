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

#ifdef PLONK_AUDIOHOST_IOS

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

END_PLONK_NAMESPACE
#include "plonk_IOSAudioHost.h"

#ifdef __OBJC__

Text IOSUtilities::pathInDirectory (Locations location, const char *filename) throw()
{
    plonk_assert (filename != 0);
        
    switch (location)
    {
        case Bundle:    return FilePath::system (FilePath::App).child (filename);
        case Documents: return FilePath::system (FilePath::AppData).child (filename);
        case Temporary: return FilePath::system (FilePath::Temp).child (filename);
        default: return Text::getEmpty();
    }
}

BEGIN_PLONK_NAMESPACE

template<class SampleType>
class IOSAudioHostPeerBase : public IOSAudioHostBase<SampleType>
{
public:
    typedef typename IOSAudioHostBase<SampleType>::UnitType UnitType;
        
    IOSAudioHostPeerBase (PLAudioHost* p)
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
    
    void hostPaused() throw()
    {
        if ([peer.delegate respondsToSelector:@selector(hostPaused:)])
            [peer.delegate hostPaused:peer];
    }

    void hostResuming() throw()
    {
        if ([peer.delegate respondsToSelector:@selector(hostResuming:)])
            [peer.delegate hostResuming:peer];
    }
    
    void hostInterruption (const bool flag) throw()
    {
        if ([peer.delegate respondsToSelector:@selector(hostInterruption:withFlag:)])
            [peer.delegate hostInterruption:peer withFlag:flag];
    }
    
    PLAudioHost* getPeer() const throw() { return peer; }
    
private:
    PLAudioHost* peer; // no need to retain as the Obj-C peer owns this object

    IOSAudioHostPeerBase();
};

/** A base, actual implementations are all in template specialisations. */
template<class SampleType>
class IOSAudioHostPeer : public IOSAudioHostPeerBase<SampleType>
{
public:
    // pure virtual constructGraph not implemented to cause a compile-time error
    // only float, short, int and double sample types are supported
};

template<>
class IOSAudioHostPeer<float> : public IOSAudioHostPeerBase<float>
{
public:    
    IOSAudioHostPeer (PLAudioHost* peer)
    :   IOSAudioHostPeerBase<float> (peer)
    {
    }
    
    FloatUnit constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        plonk_assert (peer.delegate != nil);
        
        if ([peer.delegate respondsToSelector:@selector(constructGraphFloat:)])
            return [peer.delegate constructGraphFloat:peer];
        else if ([peer.delegate respondsToSelector:@selector(constructGraph:)])
            return [peer.delegate constructGraph:peer];

        plonk_assertfalse; 
        return UnitType::getNull();
    }    
};

template<>
class IOSAudioHostPeer<short> : public IOSAudioHostPeerBase<short>
{
public:    
    IOSAudioHostPeer (PLAudioHost* peer)
    :   IOSAudioHostPeerBase<short> (peer)
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
class IOSAudioHostPeer<int> : public IOSAudioHostPeerBase<int>
{
public:    
    IOSAudioHostPeer (PLAudioHost* peer)
    :   IOSAudioHostPeerBase<int> (peer)
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
class IOSAudioHostPeer<double> : public IOSAudioHostPeerBase<double>
{
public:    
    IOSAudioHostPeer (PLAudioHost* peer)
    :   IOSAudioHostPeerBase<double> (peer)
    {
    }
    
    DoubleUnit constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        plonk_assert (peer.delegate != nil);
        return [peer.delegate constructGraphInt:peer];  
    }    
};

END_PLONK_NAMESPACE

using namespace plonk;

@implementation PLAudioHost

- (id)init
{
    if (self = [super init])
    {
        peer = NULL;
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
    if (peer == NULL) return @"";
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<float>* > (peer)->getHostName().getArray()];
            
        case TypeCode::Short:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<short>* > (peer)->getHostName().getArray()];
            
        case TypeCode::Int:     
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<int>* > (peer)->getNativeHostName().getArray()];
            
        case TypeCode::Double:  
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<double>* > (peer)->getNativeHostName().getArray()];
            
        default: 
            return @"";
    }
}

- (NSString*)nativeHostName
{
    if (peer == NULL) return @"";
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<float>* > (peer)->getNativeHostName().getArray()];
        
        case TypeCode::Short:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<short>* > (peer)->getNativeHostName().getArray()];
        
        case TypeCode::Int:     
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<int>* > (peer)->getNativeHostName().getArray()];
        
        case TypeCode::Double:  
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<double>* > (peer)->getNativeHostName().getArray()];
            
        default: 
            return @"";
    }
}

- (NSString*)inputName
{
    if (peer == NULL) return @"";
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<float>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Short:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<short>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Int:     
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<int>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Double:  
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<double>* > (peer)->getInputName().getArray()];
            
        default: 
            return @"";
    }
}

- (NSString*)outputName
{
    if (peer == NULL) return @"";
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<float>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Short:   
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<short>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Int:     
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<int>* > (peer)->getInputName().getArray()];
            
        case TypeCode::Double:  
            return [NSString stringWithUTF8String: static_cast< IOSAudioHostPeer<double>* > (peer)->getInputName().getArray()];
            
        default: 
            return @"";
    }
}

- (double)cpuUsage
{
    if (peer == NULL) return 0.0;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getCpuUsage();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getCpuUsage();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getCpuUsage();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getCpuUsage();
        default: return 0.0;
    }
}

- (BOOL)isRunning
{    
    if (peer == NULL) return NO;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getIsRunning() ? YES : NO;
        default: return NO;
    }
}

- (BOOL)isPaused
{
    if (peer == NULL) return NO;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getIsPaused() ? YES : NO;
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getIsPaused() ? YES : NO;
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getIsPaused() ? YES : NO;
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getIsPaused() ? YES : NO;
        default: return NO;
    }
}

- (BOOL)wasInterrupted
{
    if (peer == NULL) return NO;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->wasInterrupted() ? YES : NO;
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->wasInterrupted() ? YES : NO;
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->wasInterrupted() ? YES : NO;
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->wasInterrupted() ? YES : NO;
        default: return NO;
    }
}

- (Dynamic)outputUnit
{
    Dynamic dyn;
    
    if (peer != NULL)
    {
        switch ((const int)type) 
        {
            case TypeCode::Float:   dyn = static_cast< IOSAudioHostPeer<float>* > (peer)->getOutputUnit(); break;
            case TypeCode::Short:   dyn = static_cast< IOSAudioHostPeer<short>* > (peer)->getOutputUnit(); break;
            case TypeCode::Int:     dyn = static_cast< IOSAudioHostPeer<int>* > (peer)->getOutputUnit(); break;
            case TypeCode::Double:  dyn = static_cast< IOSAudioHostPeer<double>* > (peer)->getOutputUnit(); break;
            default: { }
        }
    }
    
    return dyn;
}

- (int)numInputs
{    
    if (peer == NULL) return 0;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getNumInputs();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getNumInputs();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getNumInputs();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getNumInputs();
        default: return 0.0;
    }
}

- (void)setNumInputs:(int)numInputs
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setNumInputs (numInputs); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setNumInputs (numInputs); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setNumInputs (numInputs); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setNumInputs (numInputs); break;
        default: { }
    }
}

- (int)numOutputs
{
    if (peer == NULL) return 0;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getNumOutputs();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getNumOutputs();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getNumOutputs();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getNumOutputs();
        default: return 0.0;
    }
}

- (void)setNumOutputs:(int)numOutputs
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setNumOutputs (numOutputs); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setNumOutputs (numOutputs); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setNumOutputs (numOutputs); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setNumOutputs (numOutputs); break;
        default: { }
    }
}

- (int)preferredHostBlockSize
{
    if (peer == NULL) return 0;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getPreferredHostBlockSize();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getPreferredHostBlockSize();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getPreferredHostBlockSize();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getPreferredHostBlockSize();
        default: return 0;
    }
}

- (void)setPreferredHostBlockSize:(int)preferredHostBlockSize
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize); break;
        default: { }
    }
}

- (int)preferredGraphBlockSize
{
    if (peer == NULL) return 0;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getPreferredGraphBlockSize();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getPreferredGraphBlockSize();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getPreferredGraphBlockSize();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getPreferredGraphBlockSize();
        default: return 0;
    }
}

- (void)setPreferredGraphBlockSize:(int)preferredGraphBlockSize
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize); break;
        default: { }
    }
}

- (double)preferredHostSampleRate
{    
    if (peer == NULL) return 0.0;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getPreferredHostSampleRate();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getPreferredHostSampleRate();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getPreferredHostSampleRate();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getPreferredHostSampleRate();
        default: return 0.0;
    }
}

- (void)setPreferredHostSampleRate:(double)preferredHostSampleRate
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate); break;
        default: { }
    }
}

- (void)startHost
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->startHost(); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->startHost(); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->startHost(); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->startHost(); break;
        default: { }
    }
}

- (void)stopHost
{    
    if (peer == NULL) return;
    
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->stopHost(); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->stopHost(); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->stopHost(); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->stopHost(); break;
        default: { }
    }
}

- (void)pauseHost
{
    if (peer == NULL) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->pauseHost(); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->pauseHost(); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->pauseHost(); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->pauseHost(); break;
        default: { }
    }
}

- (void)resumeHost
{
    if (peer == NULL) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->resumeHost(); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->resumeHost(); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->resumeHost(); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->resumeHost(); break;
        default: { }
    }
}

- (void)setCustomRenderCallbacksWithRef:(void*)refCon pre:(RenderCallbackFunction)preFunc post:(RenderCallbackFunction)postFunc
{
    if (peer == NULL) return;
    
    switch ((const int)type)
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setCustomRenderCallbacks (refCon, preFunc, postFunc); break;
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setCustomRenderCallbacks (refCon, preFunc, postFunc); break;
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setCustomRenderCallbacks (refCon, preFunc, postFunc); break;
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setCustomRenderCallbacks (refCon, preFunc, postFunc); break;
        default: { }
    }
}

//- (void)process:(Dynamic)unit
//{
//    if (peer == NULL) return;
//    
//    switch ((const int)type)
//    {
//        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->process (unit.asUnchecked<FloatUnit>()); break;
//        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->process (unit.asUnchecked<ShortUnit>()); break;
//        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->process (unit.asUnchecked<IntUnit>()); break;
//        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->process (unit.asUnchecked<DoubleUnit>()); break;
//        default: { }
//    }
//}

- (id)delegate
{
    return delegate;
}

- (void*)peer
{
    return peer;
}

- (void)deletePeer
{
    switch ((const int)type) 
    {
        case TypeCode::Float:   delete static_cast< IOSAudioHostPeer<float>* > (peer);   break;
        case TypeCode::Short:   delete static_cast< IOSAudioHostPeer<short>* > (peer);   break;
        case TypeCode::Int:     delete static_cast< IOSAudioHostPeer<int>* > (peer);     break;
        case TypeCode::Double:  delete static_cast< IOSAudioHostPeer<double>* > (peer);  break;
        default: { }
    }
    
    peer = NULL;
    type = TypeCode::Unknown;
}

- (void)setDelegate:(id)newDelegate
{
    plonk_assert (delegate == nil || newDelegate == nil);
    
    [self deletePeer];
    
    delegate = newDelegate;
    
    if (delegate)
    {
        const unsigned int isFloat = [delegate respondsToSelector:@selector(constructGraphFloat:)] ^ 
                                     [delegate respondsToSelector:@selector(constructGraph:)]
                                     ? (1 << TypeCode::Float) : 0;
        const unsigned int isShort = [delegate respondsToSelector:@selector(constructGraphShort:)] ? (1 << TypeCode::Short) : 0;
        const unsigned int isInt = [delegate respondsToSelector:@selector(constructGraphInt:)] ? (1 << TypeCode::Int) : 0;
        const unsigned int isDouble = [delegate respondsToSelector:@selector(constructGraphDouble:)] ? (1 << TypeCode::Double) : 0;
        const unsigned int typeFlags = isFloat | isShort | isInt | isDouble;
            
        if (typeFlags != 0 && Bits::isPowerOf2 (typeFlags))
        {
            type = Bits::countTrailingZeroes (typeFlags);
            
            switch ((const int)type) 
            {
                case TypeCode::Float:   peer = new IOSAudioHostPeer<float> (self);   break;
                case TypeCode::Short:   peer = new IOSAudioHostPeer<short> (self);   break;
                case TypeCode::Int:     peer = new IOSAudioHostPeer<int> (self);     break;
                case TypeCode::Double:  peer = new IOSAudioHostPeer<double> (self);  break;
                
                default: 
                    peer = NULL;
                    type = 0;
            }
        }
        else plonk_assertfalse; // either none or more than one constructGraph.. function was implemented
    }
}

@end


#endif


#endif // PLONK_AUDIOHOST_IOS

