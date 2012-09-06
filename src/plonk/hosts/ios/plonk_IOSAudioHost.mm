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

#ifdef PLONK_AUDIOHOST_IOS

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

END_PLONK_NAMESPACE
#include "plonk_IOSAudioHost.h"

#ifdef __OBJC__

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
    
    PLAudioHost* getPeer() const throw() { return peer; }
    
private:
    PLAudioHost* peer; // no need to retain as the Obj-C peer owns this object

    IOSAudioHostPeerBase();
};

template<class SampleType>
class IOSAudioHostPeer : public IOSAudioHostPeerBase<SampleType>
{
public:
    typedef typename IOSAudioHostBase<SampleType>::UnitType UnitType;
    
    IOSAudioHostPeer (PLAudioHost* peer)
    :   IOSAudioHostPeerBase<SampleType> (peer)
    {
    }
    
    UnitType constructGraph() throw()
    {
        PLAudioHost* const peer = this->getPeer();
        
        plonk_assert (peer.delegate != nil);
        
        // could do this with template specialisation..
        
        const int typeCode = TypeUtility<SampleType>::getTypeCode();
        
        switch (typeCode) 
        {
            case TypeCode::Float: { 
                if ([peer.delegate respondsToSelector:@selector(constructGraphFloat:)])
                    return [peer.delegate constructGraphFloat:peer];
                else if ([peer.delegate respondsToSelector:@selector(constructGraph:)])
                    return [peer.delegate constructGraph:peer];
                else
                    goto exit;
            };
            case TypeCode::Short: return [peer.delegate constructGraphShort:peer];  
            case TypeCode::Int: return [peer.delegate constructGraphInt:peer];
            case TypeCode::Double: return [peer.delegate constructGraphDouble:peer];
            default: goto exit;  
        }
        
    exit:
        plonk_assertfalse; 
        return UnitType::getNull();
    }    
};

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
    [super dealloc];
}

- (NSString*)hostName
{    
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
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->getIsRunning() ? YES : NO;
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->getIsRunning() ? YES : NO;
        default: return NO;
    }
}

//- (PLUNIT)outputUnit
//{
//    return PLPEER->getOutputUnit();
//}

- (int)numInputs
{    
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
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setNumInputs (numInputs);
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setNumInputs (numInputs);
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setNumInputs (numInputs);
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setNumInputs (numInputs);
        default: { }
    }
}

- (int)numOutputs
{
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
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setNumOutputs (numOutputs);
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setNumOutputs (numOutputs);
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setNumOutputs (numOutputs);
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setNumOutputs (numOutputs);
        default: { }
    }
}

- (int)preferredHostBlockSize
{
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
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize);
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize);
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize);
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setPreferredHostBlockSize (preferredHostBlockSize);
        default: { }
    }
}

- (int)preferredGraphBlockSize
{
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
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize);
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize);
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize);
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setPreferredGraphBlockSize (preferredGraphBlockSize);
        default: { }
    }
}

- (double)preferredHostSampleRate
{    
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
    switch ((const int)type) 
    {
        case TypeCode::Float:   static_cast< IOSAudioHostPeer<float>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate);
        case TypeCode::Short:   static_cast< IOSAudioHostPeer<short>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate);
        case TypeCode::Int:     static_cast< IOSAudioHostPeer<int>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate);
        case TypeCode::Double:  static_cast< IOSAudioHostPeer<double>* > (peer)->setPreferredHostSampleRate (preferredHostSampleRate);
        default: { }
    }
}

//- (UInt32)category
//{    
//    if (((IOAudioHostPeer*)peer)->getOtherOptions().containsKey (categoryKey))
//    {
//        return (UInt32)((IOAudioHostPeer*)peer)->getOtherOptions().at (categoryKey).asUnchecked<IntVariable>().getValue();
//    }
//    else 
//    {
//        UInt32 property;
//        UInt32 size = sizeof (property); 
//        AudioSessionGetProperty (kAudioSessionProperty_AudioCategory, &size, &property);
//        return property;
//    }
//}
//
//- (void)setCategory:(UInt32)category
//{
//    ((IOAudioHostPeer*)peer)->getOtherOptions().at (categoryKey) = IntVariable ((int)category);
//}
//
//- (UInt32)mode
//{    
//    if (((IOAudioHostPeer*)peer)->getOtherOptions().containsKey (categoryKey))
//    {
//        return (UInt32)((IOAudioHostPeer*)peer)->getOtherOptions().at (modeKey).asUnchecked<IntVariable>().getValue();
//    }
//    else 
//    {
//        UInt32 property;
//        UInt32 size = sizeof (property); 
//        AudioSessionGetProperty (kAudioSessionProperty_Mode, &size, &property);
//        return property;
//    }
//}
//
//- (void)setMode:(UInt32)mode
//{
//    ((IOAudioHostPeer*)peer)->getOtherOptions().at (modeKey) = IntVariable ((int)mode);
//}

- (void)startHost
{    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->startHost();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->startHost();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->startHost();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->startHost();
        default: { }
    }
}

- (void)stopHost
{    
    switch ((const int)type) 
    {
        case TypeCode::Float:   return static_cast< IOSAudioHostPeer<float>* > (peer)->stopHost();
        case TypeCode::Short:   return static_cast< IOSAudioHostPeer<short>* > (peer)->stopHost();
        case TypeCode::Int:     return static_cast< IOSAudioHostPeer<int>* > (peer)->stopHost();
        case TypeCode::Double:  return static_cast< IOSAudioHostPeer<double>* > (peer)->stopHost();
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
        case TypeCode::Float:   delete static_cast< IOSAudioHostPeer<float>* > (peer);   break;
        case TypeCode::Short:   delete static_cast< IOSAudioHostPeer<short>* > (peer);   break;
        case TypeCode::Int:     delete static_cast< IOSAudioHostPeer<int>* > (peer);     break;
        case TypeCode::Double:  delete static_cast< IOSAudioHostPeer<double>* > (peer);  break;
        default: { }
    }
    
    peer = nil;
    type = TypeCode::Unknown;
}

- (void)setDelegate:(id)newDelegate
{
    plonk_assert (delegate == nil || newDelegate == nil);
    
    [delegate release];
    [self deletePeer];
    
    delegate = [newDelegate retain];
    
    if (delegate)
    {
        const unsigned int isFloat = [delegate respondsToSelector:@selector(constructGraphFloat:)] || 
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
                    peer = nil; 
                    type = 0;
            }
        }
        else plonk_assertfalse;
    }
}

@end


#endif


#endif // PLONK_AUDIOHOST_IOS

