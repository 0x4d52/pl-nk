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

class IOAudioHostPeer : public IOSAudioHost
{
public:
    typedef IOSAudioHost::UnitType UnitType;
    
    IOAudioHostPeer (PLAudioHost* peerToUse)
    :   peer (peerToUse)
    {
    }
    
    UnitType constructGraph() throw()
    {
        plonk_assert (peer.delegate != nil);
        return [peer.delegate constructGraph:peer];
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
    
private:
    PLAudioHost* peer; // no need to retain as the Obj-C peer owns this object
    
    IOAudioHostPeer();
};

END_PLONK_NAMESPACE

using namespace plonk;

#define PLPEERTYPE IOAudioHostPeer
#define PLPEER ((PLPEERTYPE*)peer)

@implementation PLAudioHost

@synthesize delegate;

- (id)init
{
    if (self = [super init])
    {
        peer = new PLPEERTYPE (self);
    }
    
    return self;
}

- (void)dealloc
{
    delete PLPEER;
    [super dealloc];
}

- (NSString*)hostName
{
    return [NSString stringWithUTF8String:PLPEER->getHostName().getArray()];
}

- (NSString*)nativeHostName
{
    return [NSString stringWithUTF8String:PLPEER->getNativeHostName().getArray()];
}

- (NSString*)inputName
{
    return [NSString stringWithUTF8String:PLPEER->getInputName().getArray()];
}

- (NSString*)outputName
{
    return [NSString stringWithUTF8String:PLPEER->getOutputName().getArray()];
}

- (double)cpuUsage
{
    return PLPEER->getCpuUsage();
}

- (BOOL)isRunning
{
    return PLPEER->getIsRunning() ? YES : NO;
}

- (PLUNIT)outputUnit
{
    return PLPEER->getOutputUnit();
}

- (int)numInputs
{
    return PLPEER->getNumInputs();
}

- (void)setNumInputs:(int)numInputs
{
    PLPEER->setNumInputs (numInputs);
}

- (int)numOutputs
{
    return PLPEER->getNumOutputs();
}

- (void)setNumOutputs:(int)numOutputs
{
    PLPEER->setNumOutputs (numOutputs);
}

- (int)preferredHostBlockSize
{
    return PLPEER->getPreferredHostBlockSize();
}

- (void)setPreferredHostBlockSize:(int)preferredHostBlockSize
{
    PLPEER->setPreferredHostBlockSize (preferredHostBlockSize);
}

- (double)preferredHostSampleRate
{
    return PLPEER->getPreferredHostSampleRate();
}

- (void)setPreferredHostSampleRate:(double)preferredHostSampleRate
{
    PLPEER->setPreferredHostSampleRate (preferredHostSampleRate);
}

- (UInt32)category
{    
    if (PLPEER->getOtherOptions().containsKey (categoryKey))
    {
        return (UInt32)PLPEER->getOtherOptions().at (categoryKey).asUnchecked<IntVariable>().getValue();
    }
    else 
    {
        UInt32 property;
        UInt32 size = sizeof (property); 
        AudioSessionGetProperty (kAudioSessionProperty_AudioCategory, &size, &property);
        return property;
    }
}

- (void)setCategory:(UInt32)category
{
    PLPEER->getOtherOptions().at (categoryKey) = IntVariable ((int)category);
}

- (UInt32)mode
{    
    if (PLPEER->getOtherOptions().containsKey (categoryKey))
    {
        return (UInt32)PLPEER->getOtherOptions().at (modeKey).asUnchecked<IntVariable>().getValue();
    }
    else 
    {
        UInt32 property;
        UInt32 size = sizeof (property); 
        AudioSessionGetProperty (kAudioSessionProperty_Mode, &size, &property);
        return property;
    }
}

- (void)setMode:(UInt32)mode
{
    PLPEER->getOtherOptions().at (modeKey) = IntVariable ((int)mode);
}

- (void)startHost
{
    PLPEER->startHost();
}

- (void)stopHost
{
    PLPEER->stopHost();
}

@end

#undef PLPEERTYPE
#undef PLPEER

#endif


#endif // PLONK_AUDIOHOST_IOS

