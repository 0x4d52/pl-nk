//
//  PAEDelay.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

////////////////////////////////////////////////////////////////////////////////

@interface PAEDelay ()
@property (strong, nonatomic) PAEProcess* delayTime;
@end

@implementation PAEDelay

-(NSArray*)controls
{
    return @[self.delayTime];
}

+(PAEDelay*)delayWithNumInputs:(int)numInputs maxDelayTime:(float)maxDelayTime;
{
    return [[PAEDelay alloc] initWithNumInputs:numInputs maxDelayTime:maxDelayTime];
}

-(id)initWithNumInputs:(int)numInputs
{
    return [self initWithNumInputs:numInputs maxDelayTime:1.0];
}

-(id)initWithNumInputs:(int)numInputs maxDelayTime:(float)maxDelayTime
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.delayTime       = [[PAEProcess alloc] initWithNumInputs:numInputs];
        self.delayTime.input = [PAEConstant constantWithValue:maxDelayTime];
        self.outputUnit = Delay::ar (self.patchUnit, self.delayTime.patchUnit, maxDelayTime);
    }
    
    return self;
}

@end

////////////////////////////////////////////////////////////////////////////////

@interface PAEComb ()
@property (strong, nonatomic) PAEProcess* delayTime;
@property (strong, nonatomic) PAEProcess* feedback;
@end

@implementation PAEComb

-(NSArray*)controls
{
    return @[self.delayTime,self.feedback];
}

+(PAEComb*)combWithNumInputs:(int)numInputs maxDelayTime:(float)maxDelayTime;
{
    return [[PAEComb alloc] initWithNumInputs:numInputs maxDelayTime:maxDelayTime];
}

-(id)initWithNumInputs:(int)numInputs
{
    return [self initWithNumInputs:numInputs maxDelayTime:1.0];
}

-(id)initWithNumInputs:(int)numInputs maxDelayTime:(float)maxDelayTime
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.delayTime       = [[PAEProcess alloc] initWithNumInputs:numInputs];
        self.delayTime.input = [PAEConstant constantWithValue:maxDelayTime];
        self.feedback        = [[PAEProcess alloc] initWithNumInputs:1];
        self.feedback.input  = [PAEConstant constantWithValue:0.5];

        self.outputUnit = CombFB::ar (self.patchUnit, self.delayTime.patchUnit, self.feedback.patchUnit, maxDelayTime).dc();
    }
    
    return self;
}

@end

