//
//  PAEControl.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEControl ()
{
    FloatVariable _value;
    FloatVariable _lagTime;
}
@end

@implementation PAEControl

@synthesize map = _map;

+(PAEControl*)control
{
    return [[PAEControl alloc] initWithValue:0.f];
}

+(PAEControl*)controlWithValue:(float)value
{
    return [[PAEControl alloc] initWithValue:value];
}

-(id)init
{
    return nil;
}

-(id)initWithValue:(float)value
{
    if (self = [super init])
    {
        _lagTime = PAEControlLagTime;
        self.outputUnit = _value.ar().lag (_lagTime);
        _value.setValue (value);
    }
    
    return self;
}

-(float)value
{
    const float currentOutputValue = _value.getValue();    
    return self.map ? [self.map mapOutputToInput:currentOutputValue] : currentOutputValue;
}

-(void)setValue:(float)value
{
    const float newOutputValue = self.map ? [self.map mapInputToOutput:value] : value;
    _value.setValue (newOutputValue);
}

-(PAEMap*)map
{
    return _map;
}

-(void)setMap:(PAEMap *)map
{
    const float oldValue = self.value; // may unmap
    _map = map;
    self.value = oldValue; // may remap
}

-(float)mappedValue
{
    return _value.getValue();
}

-(float)lagTime
{
    return _lagTime.getValue();
}

-(void)setLagTime:(float)lagTime
{
    _lagTime.setValue (plonk::max(0.f, lagTime));
}

@end

