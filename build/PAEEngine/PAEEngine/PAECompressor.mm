
//
//  PAECompressor.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAECompressor ()
@property (strong, nonatomic) PAEFollower* follower;
@property (strong, nonatomic) PAEProcess* attackTime;
@property (strong, nonatomic) PAEProcess* releaseTime;
@property (strong, nonatomic) PAEProcess* threshold;
@property (strong, nonatomic) PAEProcess* ratio;
@property (strong, nonatomic) PAESource* control;
@end

@implementation PAECompressor
+(PAECompressor*)compressorWithNumInputs:(int)numInputs;
{
    return [[PAECompressor alloc] initWithNumInputs:numInputs];
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
        self.ratio             = [[PAEProcess alloc] initWithNumInputs:1];
        self.ratio.input       = [PAEConstant constantWithValue:0.5];
        
        static const Unit& zero = Math<Unit>::get0();
        static const Unit& one  = Math<Unit>::get1();
        static const Unit& ten  = Math<Unit>::get10();
        
        Unit input      = self.follower.outputUnit;
        input          -= self.threshold.patchUnit;
        Unit control    = input.max (zero);
        control         = LagAsym::ar (control, self.attackTime.patchUnit, self.releaseTime.patchUnit);
        
        self.control = [[PAESource alloc] init];
        self.control.outputUnit = one - pow (ten, log10 (control) * self.ratio.patchUnit);
        
        self.outputUnit = self.patchUnit * self.control.outputUnit;
    }
    
    return self;
}

@end

