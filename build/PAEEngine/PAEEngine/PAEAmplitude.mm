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
@end

@implementation PAEAmplitude
+(PAEAmplitude*)amplitudeWithNumInputs:(int)numInputs;
{
    return [[PAEAmplitude alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.level        = [[PAEProcess alloc] initWithNumInputs:numInputs];
        self.level.input  = [PAEConstant constantWithValue:1];
        self.outputUnit   = self.patchUnit * self.level.patchUnit;
    }
    
    return self;
}

@end


