//
//  PAEFollower.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEFollower ()
@property (strong, nonatomic) PAEProcess* attackTime;
@property (strong, nonatomic) PAEProcess* releaseTime;
@end

@implementation PAEFollower
+(PAEFollower*)followerWithNumInputs:(int)numInputs;
{
    return [[PAEFollower alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.attackTime        = [[PAEProcess alloc] initWithNumInputs:1];
        self.attackTime.input  = [PAEConstant constantWithValue:0.01];
        self.releaseTime       = [[PAEProcess alloc] initWithNumInputs:1];
        self.releaseTime.input = [PAEConstant constantWithValue:0.01];

        self.outputUnit = LagAsym::ar (self.patchUnit.abs(),
                                       self.attackTime.patchUnit,
                                       self.releaseTime.patchUnit);
    }
    
    return self;
}
@end

