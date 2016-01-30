//
//  PAEChannelStrip.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEChannelStrip ()
{
    UnitVariable _insertGraphVariable;
}
@property (strong, nonatomic) PAEProcess* level;
@end

@implementation PAEChannelStrip

@synthesize inserts = _inserts;

+(PAEChannelStrip*)channelStripWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs;
{
    return [[PAEChannelStrip alloc] initWithNumOutputs:numOutputs andNumInputs:numInputs];
}

+(PAEChannelStrip*)channelStripWithNumChannels:(int)numChannels
{
    return [[PAEChannelStrip alloc] initWithNumOutputs:numChannels andNumInputs:numChannels];
}

-(id)init
{
    return nil;
}

-(id)initWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs;
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.outputUnit  = Patch::ar (_insertGraphVariable, false, numOutputs, PAERepatchFadeTime);        
        _insertGraphVariable.setValue (self.patchUnit);
    }
    
    return self;
}

-(NSArray*)inserts
{
    return _inserts;
}

-(void)setInserts:(NSArray*)inserts
{
    Unit graph = self.patchUnit;
    
    for (PAEProcess* insert in inserts)
    {
        insert.inputUnit = graph;
        graph = insert.outputUnit;
    }
    
    _insertGraphVariable = graph;
}

@end
