//
//  PAEFilter.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAESimpleGate ()
@property (strong, nonatomic) PAEFollower* follower;
@property (strong, nonatomic) PAEProcess* attackTime;
@property (strong, nonatomic) PAEProcess* releaseTime;
@property (strong, nonatomic) PAEProcess* threshold;
@property (strong, nonatomic) PAESource* control;
@end

@implementation PAESimpleGate
-(NSArray*)controls
{
    return @[self.follower,self.attackTime,self.releaseTime,self.threshold];
}

+(PAESimpleGate*)gateWithNumInputs:(int)numInputs;
{
    return [[PAESimpleGate alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.follower = [PAEFollower followerWithNumInputs:numInputs];
        self.follower.inputUnit = self.patchUnit.mix();
        
        self.attackTime        = [[PAEProcess alloc] initWithNumInputs:1];
        self.attackTime.input  = [PAEConstant constantWithValue:0.01];
        self.releaseTime       = [[PAEProcess alloc] initWithNumInputs:1];
        self.releaseTime.input = [PAEConstant constantWithValue:0.1];
        self.threshold         = [[PAEProcess alloc] initWithNumInputs:1];
        self.threshold.input   = [PAEConstant constantWithValue:0.2];

        Unit input = self.follower.outputUnit;
        Unit trigger = input > self.threshold.patchUnit;
        
        self.control = [[PAESource alloc] init];
        self.control.outputUnit = LagAsym::ar (trigger, self.attackTime.patchUnit, self.releaseTime.patchUnit);
        self.outputUnit = self.patchUnit * self.control.outputUnit;
    }
    
    return self;
}

@end

////////////////////////////////////////////////////////////////////////////////

@interface PAEGate ()
@property (strong, nonatomic) PAEFollower* follower;
@property (strong, nonatomic) PAEProcess* attackTime;
@property (strong, nonatomic) PAEProcess* releaseTime;
@property (strong, nonatomic) PAEProcess* attackThreshold;
@property (strong, nonatomic) PAEProcess* releaseThreshold;
@property (strong, nonatomic) PAESource* control;
@end

@implementation PAEGate
-(NSArray*)controls
{
    return @[self.follower,self.attackTime,self.releaseTime,self.attackThreshold,self.releaseThreshold];
}

+(PAEGate*)gateWithNumInputs:(int)numInputs;
{
    return [[PAEGate alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.follower = [PAEFollower followerWithNumInputs:numInputs];
        self.follower.inputUnit = self.patchUnit.mix();
        
        self.attackTime             = [[PAEProcess alloc] initWithNumInputs:1];
        self.attackTime.input       = [PAEConstant constantWithValue:0.01];
        self.releaseTime            = [[PAEProcess alloc] initWithNumInputs:1];
        self.releaseTime.input      = [PAEConstant constantWithValue:0.1];
        self.attackThreshold        = [[PAEProcess alloc] initWithNumInputs:1];
        self.attackThreshold.input  = [PAEConstant constantWithValue:0.21];
        self.releaseThreshold       = [[PAEProcess alloc] initWithNumInputs:1];
        self.releaseThreshold.input = [PAEConstant constantWithValue:0.19];
        
        Unit input = self.follower.outputUnit;
        Unit trigger = Schmidt::ar(input, self.releaseThreshold.patchUnit, self.attackThreshold.patchUnit);
        
        self.control = [[PAESource alloc] init];
        self.control.outputUnit = LagAsym::ar (trigger, self.attackTime.patchUnit, self.releaseTime.patchUnit);
        self.outputUnit = self.patchUnit * self.control.outputUnit;
    }
    
    return self;
}

@end

