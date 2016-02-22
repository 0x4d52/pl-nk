//
//  PAEOscillator.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAESine ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* amplitude;
@property (strong, nonatomic) PAEProcess* offset;
@end

@implementation PAESine

-(NSArray*)controls
{
    return @[self.frequency,self.amplitude,self.offset];
}

+(PAESine*)sineWithFrequency:(float)frequency
{
    return [[PAESine alloc] initWithFrequency:frequency];
}

+(PAESine*)sineWithFrequencySource:(PAESource*)frequency
{
    return [[PAESine alloc] initWithFrequencySource:frequency];
}

+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude
{
    return [[PAESine alloc] initWithFrequency:frequency amplitude:amplitude];
}

+(PAESine*)sineWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude
{
    return [[PAESine alloc] initWithFrequencySource:frequency amplitudeSource:amplitude];
}

+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset
{
    return [[PAESine alloc] initWithFrequency:frequency amplitude:amplitude offset:offset];
}

+(PAESine*)sineWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude offsetSource:(PAESource*)offset
{
    return [[PAESine alloc] initWithFrequencySource:frequency amplitudeSource:amplitude offsetSource:offset];
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

-(id)initWithFrequencySource:(PAESource*)frequency
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = frequency;
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

-(id)initWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = frequency;
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = amplitude;
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

-(id)initWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude offsetSource:(PAESource*)offset
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = frequency;
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = amplitude;
        self.offset          = [[PAEProcess alloc] initWithNumInputs:1];
        self.offset.input    = offset;
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

-(NSArray*)controls
{
    return @[self.frequency,self.amplitude,self.offset];
}

+(PAESaw*)sawWithFrequency:(float)frequency
{
    return [[PAESaw alloc] initWithFrequency:frequency];
}

+(PAESaw*)sawWithFrequencySource:(PAESource*)frequency
{
    return [[PAESaw alloc] initWithFrequencySource:frequency];
}

+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude
{
    return [[PAESaw alloc] initWithFrequency:frequency amplitude:amplitude];
}

+(PAESaw*)sawWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude
{
    return [[PAESaw alloc] initWithFrequencySource:frequency amplitudeSource:amplitude];
}

+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset
{
    return [[PAESaw alloc] initWithFrequency:frequency amplitude:amplitude offset:offset];
}

+(PAESaw*)sawWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude offsetSource:(PAESource*)offset
{
    return [[PAESaw alloc] initWithFrequencySource:frequency amplitudeSource:amplitude offsetSource:offset];
}

+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset initialValue:(float)value
{
    return [[PAESaw alloc] initWithFrequency:frequency amplitude:amplitude offset:offset initialValue:value];
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

-(id)initWithFrequencySource:(PAESource*)frequency
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = frequency;
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

-(id)initWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = frequency;
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = amplitude;
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

-(id)initWithFrequencySource:(PAESource*)frequency amplitudeSource:(PAESource*)amplitude offsetSource:(PAESource*)offset
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = frequency;
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = amplitude;
        self.offset          = [[PAEProcess alloc] initWithNumInputs:1];
        self.offset.input    = offset;
        self.outputUnit      = Saw::ar (self.frequency.patchUnit, self.amplitude.patchUnit, self.offset.patchUnit);
    }
    
    return self;
}

-(id)initWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset initialValue:(float)value
{
    if (self = [super init])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:frequency];
        self.amplitude       = [[PAEProcess alloc] initWithNumInputs:1];
        self.amplitude.input = [PAEConstant constantWithValue:amplitude];
        self.offset          = [[PAEProcess alloc] initWithNumInputs:1];
        self.offset.input    = [PAEConstant constantWithValue:offset];
        self.outputUnit      = Saw::ar (self.frequency.patchUnit, self.amplitude.patchUnit, self.offset.patchUnit, value);
    }
    
    return self;
}

@end
