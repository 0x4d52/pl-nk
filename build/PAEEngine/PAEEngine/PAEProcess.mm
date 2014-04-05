//
//  PAEProcessor.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

//class PAESourceHolder
//{
//public:
//    PAESourceHolder() throw() : source (nil) { }
//    PAESourceHolder (PAESource* s) throw() : source (s) { }
//    ~PAESourceHolder() { source = nil; }
//    __strong PAESource* source;
//};
//
//typedef LockFreeQueue<PAESourceHolder> PAESourceHolderQueue;

@interface PAEProcess ()
{
    UnitVariable _inputUnitVariable;
    Unit _patchUnit;
    FloatVariable _repatchFadeTime;
//    PAESourceHolderQueue _previousSources;
}
//-(void)repatchDone;
@end

//class PAEProcessPeer : public Channel::Receiver
//{
//public:
//    PAEProcessPeer (PAEProcess* p) throw()
//    :   peer (p)
//    {
//    }
//    
//    void changed (Channel const& source, Text const& message, Dynamic const& payload) throw()
//    {
//        if (message == Text::getMessagePatchEnd())
//            [peer repatchDone];
//    }
//    
//private:
//    __weak PAEProcess* peer;
//};

@implementation PAEProcess
//{
//    ScopedPointerContainer<PAEProcessPeer> _peer;
//}

@synthesize input = _input;

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super init])
    {
//        _peer = new PAEProcessPeer (self);
        _repatchFadeTime.setValue (0.f);
        _patchUnit = Patch::ar (_inputUnitVariable, false, numInputs, _repatchFadeTime);
//        _patchUnit.addReceiverToChannels (_peer.getInternal());
        _repatchFadeTime.setValue (PAERepatchFadeTime);
        self.outputUnit = _patchUnit; // pass through as default
    }
    
    return self;
}

-(void)dealloc
{
//    _patchUnit.removeReceiverFromChannels (_peer.getInternal());
}

-(PAESource*)input
{
    return _input;
}

-(void)setInput:(PAESource*)input
{
//    if (_input)
//        _previousSources.push (PAESourceHolder (_input));
    
    _input = input;
    _inputUnitVariable.setValue (input ? input.outputUnit : Unit::getNull());
}

-(int)numInputs
{
    return _patchUnit.getNumChannels();
}

-(Unit)inputUnit
{
    return _inputUnitVariable.getValue();
}

-(void)setInputUnit:(Unit)inputUnit
{
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = inputUnit;
    self.input = proxy;
}

-(Unit)patchUnit
{
    return _patchUnit;
}

-(float)repatchFadeTime
{
    return _repatchFadeTime.getValue();
}

-(void)setRepatchFadeTime:(float)repatchFadeTime
{
    _repatchFadeTime.setValue (repatchFadeTime);
}

//-(void)repatchDone
//{
//    PAESourceHolder sourceHolder = _previousSources.pop();
//    [self.processDelegate repatchDone:self previousSource:sourceHolder.source];
//    sourceHolder.source = nil;
//}

@end
