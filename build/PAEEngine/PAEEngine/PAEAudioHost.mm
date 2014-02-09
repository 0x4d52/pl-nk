//
//  PAEAudioHost.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEAudioHost.h"
#import "PAESourceInternal.h"

#include "plonk.h"
#include "plonk_IOSAudioHost.h"

const float PAERepatchFadeTime = 0.005f;
const float PAEControlLagTime = 0.02f;

/** Audio host implementation.
 @internal */
@interface PAEAudioHostInternal : PLAudioHost<PLAudioHostDelegate>
@end

/**@internal */
@implementation PAEAudioHostInternal
@end

@interface PAEAudioHost ()
{
    PAEAudioHostInternal* _internal;
    UnitVariable _mainOutputVariable;
    int _numOutputs;
    int _numInputs;
}
-(void)process:(Unit)unit;
@end

OSStatus preRenderCallbackFunction (void*                      refCon,
                                    UInt32                     inNumberFrames,
                                    AudioUnitRenderActionFlags *ioActionFlags,
                                    const AudioTimeStamp 	   *inTimeStamp,
                                    AudioBufferList            *ioData)
{
    PAEAudioHost* SELF = (__bridge PAEAudioHost*)refCon;
    
    for (PAESource* source in SELF.additionalRenderSources)
    {
        [SELF process:source.outputUnit];
    }
    
    return noErr;
}

@implementation PAEAudioHost

@synthesize mainMix = _mainMix;
@synthesize additionalRenderSources = _additionalRenderSources;

+(PAEAudioHost*)audioHostWithNumOutputs:(int)numOutputs
{
    return [[PAEAudioHost alloc] initWithNumOutputs:numOutputs andNumInputs:0];
}

+(PAEAudioHost*)audioHostWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs
{
    return [[PAEAudioHost alloc] initWithNumOutputs:numOutputs andNumInputs:numInputs];
}

-(id)initWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs
{
    if (self = [super init])
    {
        _internal = [[PAEAudioHostInternal alloc] init];
        _internal.delegate = self;

        _numOutputs = plonk::max (numOutputs, 1);
        _numInputs = plonk::max (numInputs, 0);
        
        _internal.numOutputs = _numOutputs;
        _internal.numInputs = _numInputs;
//        _internal.preferredGraphBlockSize = 512;
//        _internal.preferredHostBlockSize = 512;
    }
    
    return self;
}

-(id)init
{
    return [self initWithNumOutputs:2 andNumInputs:0];
}

-(void)start
{
    [_internal startHost];
}

-(void)stop
{
    [_internal stopHost];
}

- (FloatUnit)constructGraphFloat:(PLAudioHost*)host
{
    return Patch::ar (_mainOutputVariable, false, _numOutputs, PAERepatchFadeTime);
}

-(PAESource*)mainMix
{
    return _mainMix;
}

-(void)setMainMix:(PAESource*)mainMix
{
    _mainOutputVariable.setValue (mainMix ? mainMix.outputUnit : Unit::getNull());
    _mainMix = mainMix;
}

-(void)process:(Unit)unit
{
    [_internal process:unit];
}

-(NSArray*)additionalRenderSources
{
    return _additionalRenderSources;
}

-(void)setAdditionalRenderSources:(NSArray *)additionalRenderSources
{
    RenderCallbackFunction callback = NULL;
    
    if (additionalRenderSources)
        callback = preRenderCallbackFunction;
    
    _additionalRenderSources = additionalRenderSources;
    
    [_internal setCustomRenderCallbacksWithRef:(__bridge void*)self
                                           pre:callback
                                          post:NULL];

}

@end
