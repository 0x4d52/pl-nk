//
//  PAEMidSide.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@implementation PAEMidSide
+(PAEMidSide*)midSideWithNumInputs:(int)numInputs;
{
    return [[PAEMidSide alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    int numInputsEven = (numInputs % 1) ? numInputs + 1 : numInputs;
    
    if (self = [super initWithNumInputs:numInputsEven])
    {
        int numPairs = numInputsEven / 2;
        Unit output = Unit::emptyChannels (numInputsEven);
        Unit patch = self.patchUnit;
        
        for (int i = 0; i < numPairs; ++i)
        {
            int aIndex = i * 2;
            int bIndex = i * 2 + 1;
            Unit a = patch[aIndex]; // L or M
            Unit b = patch[bIndex]; // R or S
            
            output[aIndex] = a + b;
            output[bIndex] = a - b;
        }
        
        self.outputUnit = output;
    }
    
    return self;
}

@end


