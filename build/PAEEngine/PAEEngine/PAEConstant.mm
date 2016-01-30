//
//  PAEConstant.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@implementation PAEConstant

+(PAEConstant*)constantWithValue:(float)value;
{
    return [[PAEConstant alloc] initWithValue:value];
}

-(id)init
{
    return nil;
}

-(id)initWithValue:(float)value
{
    if (self = [super init])
    {
        self.outputUnit = Unit (value);
    }
    
    return self;
}

@end
