//
//  PAEAudioHost.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEAudioHost.h"
#import "PAESourceInternal.h"

#include "plonk.h"
#include "plonk_IOSAudioHost.h"

NSString* fourCharNSStringForFourCharCode (UInt32 aCode)
{
    const char fourChar[5] = {
        (const char)((aCode >> 24) & 0xFF),
        (const char)((aCode >> 16) & 0xFF),
        (const char)((aCode >> 8) & 0xFF),
        (const char)(aCode & 0xFF),
        0
    };
    NSString *fourCharString = [NSString stringWithCString:fourChar encoding:NSUTF8StringEncoding];
    return fourCharString;
}



static BOOL initialised = NO;
const float PAERepatchFadeTime = 0.005f;
const float PAEControlLagTime = 0.02f;

/** Audio host implementation.
 @internal */
@interface PAEAudioHostInternal : PLAudioHost<PLAudioHostDelegate>
@end

/**@internal */
@implementation PAEAudioHostInternal
@end

struct PAEAudioHostData
{
    void* peer;
    Units additionalRenderSources;
};

@interface PAEAudioHost ()
{
    PAEAudioHostInternal* _internal;
    UnitVariable _mainOutputVariable;
    int _numOutputs;
    int _numInputs;
    PAEAudioHostData _data;
}
@end

//OSStatus preRenderCallbackFunction (void*                      refCon,
//                                    UInt32                     inNumberFrames,
//                                    AudioUnitRenderActionFlags *ioActionFlags,
//                                    const AudioTimeStamp 	   *inTimeStamp,
//                                    AudioBufferList            *ioData)
//{
//    PAEAudioHost* SELF = (__bridge PAEAudioHost*)refCon;
//    
//    for (PAESource* source in SELF.additionalRenderSources)
//    {
//        [SELF process:source.outputUnit];
//    }
//    
//    return noErr;
//}

OSStatus preRenderCallbackFunction (void*                      refCon,
                                    UInt32                     inNumberFrames,
                                    AudioUnitRenderActionFlags *ioActionFlags,
                                    const AudioTimeStamp 	   *inTimeStamp,
                                    AudioBufferList            *ioData)
{
    PAEAudioHostData* data = (PAEAudioHostData*)refCon;
    Units& additionalRenderSources = data->additionalRenderSources;
    AudioHostBase<float>* hostBase = static_cast<AudioHostBase<float>*> (data->peer);
    
    if (hostBase != NULL)
    {
        for (int i = 0; i < additionalRenderSources.size(); ++i)
        {
            Unit& source = additionalRenderSources.atUnchecked (i);
            hostBase->process (source);
        }
    }
    
    return noErr;
}

@implementation PAEAudioHost

@synthesize mainMix = _mainMix;

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
    if (!initialised)
    {
        initialised = YES;
        NSLog (@"PAEEngine v%s.%08x", PAEENGINE_VERSION, PAEENGINE_BUILD);
    }
    
    if (self = [super init])
    {
        _internal = [[PAEAudioHostInternal alloc] init];
        _internal.delegate = self;
        _data.peer = _internal.peer; // after setDelegate:

        _numOutputs = plonk::max (numOutputs, 1);
        _numInputs = plonk::max (numInputs, 0);
        
        _internal.numOutputs = _numOutputs;
        _internal.numInputs = _numInputs;
        _internal.preferredGraphBlockSize = 64;
    }
    
    return self;
}

-(id)init
{
    return [self initWithNumOutputs:2 andNumInputs:0];
}

-(int)numOutputs
{
    return _internal.numOutputs;
}

-(void)setNumOutputs:(int)numOutputs
{
    if (_internal.isRunning)
    {
        NSString* propname = @"numOutputs";
        NSLog(@"PAEAudioHost readonly setter for property %@ when audio is running", propname);
        return;
    }
    
    _internal.numOutputs = numOutputs;
}

-(int)numInputs
{
    return _internal.numInputs;
}

-(void)setNumInputs:(int)numInputs
{
    if (_internal.isRunning)
    {
        NSString* propname = @"numInputs";
        NSLog(@"PAEAudioHost readonly setter for property %@ when audio is running", propname);
        return;
    }
    
    _internal.numInputs = numInputs;
}

-(int)hardwareBlockSize
{
    return _internal.preferredHostBlockSize;
}

-(void)setHardwareBlockSize:(int)hardwareBlockSize
{
    if (_internal.isRunning)
    {
        NSString* propname = @"hardwareBlockSize";
        NSLog(@"PAEAudioHost readonly setter for property %@ when audio is running", propname);
        return;
    }
    
    _internal.preferredHostBlockSize = hardwareBlockSize;
}

-(int)processBlockSize
{
    return _internal.preferredGraphBlockSize;
}

-(void)setProcessBlockSize:(int)processBlockSize
{
    if (_internal.isRunning)
    {
        NSString* propname = @"processBlockSize";
        NSLog(@"PAEAudioHost readonly setter for property %@ when audio is running", propname);
        return;
    }
    
    _internal.preferredGraphBlockSize = processBlockSize;
}

-(double)sampleRate
{
    return _internal.preferredHostSampleRate;
}

-(void)setSampleRate:(double)sampleRate
{
    if (_internal.isRunning)
    {
        NSString* propname = @"sampleRate";
        NSLog(@"PAEAudioHost readonly setter for property %@ when audio is running", propname);
        return;
    }
    
    _internal.preferredHostSampleRate = sampleRate;
}

-(double)cpuUsage
{
    return _internal.cpuUsage;
}

-(void)start
{
    if (_internal.numInputs > 0)
    {
        UInt32 propertySize = sizeof (UInt32);
        UInt32 category;
        AudioSessionGetProperty (kAudioSessionProperty_AudioCategory, &propertySize, &category);
        
        if (! ((category == kAudioSessionCategory_PlayAndRecord) || (category == kAudioSessionCategory_RecordAudio)))
        {
            NSLog(@"WARNING: PAEAudioHost has audio inputs but the AudioSession category is set incorrectly to %@", fourCharNSStringForFourCharCode (category));
        }
    }
    
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

-(NSArray*)additionalRenderSources
{
    Units renderSources = _data.additionalRenderSources; // cache
    
    NSMutableArray* additionalRenderSources = [[NSMutableArray alloc] initWithCapacity:renderSources.size()];
    
    for (int i = 0; i < renderSources.size(); ++i)
    {
        PAESource* proxy = [[PAESource alloc] init];
        proxy.outputUnit = renderSources.atUnchecked (i);
        [additionalRenderSources addObject:proxy];
    }
    
    return [NSArray arrayWithArray:additionalRenderSources];
}

-(void)setAdditionalRenderSources:(NSArray *)additionalRenderSources
{
    RenderCallbackFunction callback = NULL;
    Units renderSources;

    if (additionalRenderSources)
    {
        callback = preRenderCallbackFunction;
    
        for (PAESource* source in additionalRenderSources)
            renderSources.add (source.outputUnit);
    }
    
    _data.additionalRenderSources = renderSources;
    
    [_internal setCustomRenderCallbacksWithRef:&_data
                                           pre:callback
                                          post:NULL];

}

@end
