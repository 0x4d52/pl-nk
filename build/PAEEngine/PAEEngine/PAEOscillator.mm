//
//  PAEOscillator.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAESine ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* amplitude;
@property (strong, nonatomic) PAEProcess* offset;
@end

@implementation PAESine

+(PAESine*)sineWithFrequency:(float)frequency
{
    return [[PAESine alloc] initWithFrequency:frequency];
}

+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude
{
    return [[PAESine alloc] initWithFrequency:frequency amplitude:amplitude];
}

+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset
{
    return [[PAESine alloc] initWithFrequency:frequency amplitude:amplitude offset:offset];
}

-(id)initWithFrequency:(float)frequency
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.outputUnit      = Sine::ar (self.frequency.patchUnit);
    }
    
    return self;
}

-(id)initWithFrequency:(float)frequency amplitude:(float)amplitude
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = [PAEConstant constantWithValue:amplitude];
        self.outputUnit      = Sine::ar (self.frequency.patchUnit, self.amplitude.patchUnit);
    }
    
    return self;
}

-(id)initWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = [PAEConstant constantWithValue:amplitude];
        self.offset          = [[PAEProcess alloc] initWithNumInputs:1];
        self.offset.input    = [PAEConstant constantWithValue:offset];
        self.outputUnit      = Sine::ar (self.frequency.patchUnit, self.amplitude.patchUnit, self.offset.patchUnit);
    }
    
    return self;
}

@end

////////////////////////////////////////////////////////////////////////////////

@interface PAESaw ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* amplitude;
@property (strong, nonatomic) PAEProcess* offset;
@end

@implementation PAESaw

+(PAESaw*)sawWithFrequency:(float)frequency
{
    return [[PAESaw alloc] initWithFrequency:frequency];
}

+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude
{
    return [[PAESaw alloc] initWithFrequency:frequency amplitude:amplitude];
}

+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset
{
    return [[PAESaw alloc] initWithFrequency:frequency amplitude:amplitude offset:offset];
}

-(id)initWithFrequency:(float)frequency
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.outputUnit      = Saw::ar (self.frequency.patchUnit);
    }
    
    return self;
}

-(id)initWithFrequency:(float)frequency amplitude:(float)amplitude
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = [PAEConstant constantWithValue:amplitude];
        self.outputUnit      = Saw::ar (self.frequency.patchUnit, self.amplitude.patchUnit);
    }
    
    return self;
}

-(id)initWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = [PAEConstant constantWithValue:amplitude];
        self.offset          = [[PAEProcess alloc] initWithNumInputs:1];
        self.offset.input    = [PAEConstant constantWithValue:offset];
        self.outputUnit      = Saw::ar (self.frequency.patchUnit, self.amplitude.patchUnit, self.offset.patchUnit);
    }
    
    return self;
}

@end
