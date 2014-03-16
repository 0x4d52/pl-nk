//
//  PAEFilter.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEAmplitude ()
@property (strong, nonatomic) PAEProcess* level;
@property (nonatomic) BOOL pauseInputOnZero;
@end

@implementation PAEAmplitude

@synthesize pauseInputOnZero = _pauseInputOnZero;

+(PAEAmplitude*)amplitudeWithNumInputs:(int)numInputs;
{
    return [[PAEAmplitude alloc] initWithNumInputs:numInputs pauseInputOnZero:NO];
}

+(PAEAmplitude*)amplitudeWithNumInputs:(int)numInputs pauseInputOnZero:(BOOL)pauseOnZero
{
    return [[PAEAmplitude alloc] initWithNumInputs:numInputs pauseInputOnZero:pauseOnZero];
}

-(id)initWithNumInputs:(int)numInputs pauseInputOnZero:(BOOL)pauseOnZero
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.pauseInputOnZero = pauseOnZero;
        self.level            = [[PAEProcess alloc] initWithNumInputs:numInputs];
        self.level.input      = [PAEConstant constantWithValue:1];
        
        if (self.pauseInputOnZero)
        {
            Unit levelUnit = self.level.patchUnit;
            self.outputUnit   = Pause::ar (self.patchUnit, levelUnit.kr().isEqualTo (0.f)) * levelUnit;
        }
        else
        {
            self.outputUnit   = self.patchUnit * self.level.patchUnit;
        }
        
    }
    
    return self;
}

-(BOOL)pauseInputOnZero
{
    return _pauseInputOnZero;
}

@end


