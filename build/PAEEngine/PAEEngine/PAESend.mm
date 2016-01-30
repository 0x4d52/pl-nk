//
//  PAEFilter.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAESend ()
@property (strong, nonatomic) PAEAmplitude* send;
@end

@implementation PAESend
+(PAESend*)sendWithNumInputs:(int)numInputs;
{
    return [[PAESend alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {        
        self.send = [PAEAmplitude amplitudeWithNumInputs:numInputs];
        self.send.inputUnit = self.patchUnit;
        self.outputUnit = self.patchUnit;
    }
    
    return self;
}

@end


