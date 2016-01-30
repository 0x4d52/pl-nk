//
//  PAETestTone.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAETestTone ()
@end

@implementation PAETestTone

+(PAETestTone*)testTone
{
    return [[PAETestTone alloc] init];
}

-(id)init
{
    if (self = [super init])
    {
        self.outputUnit = Sine::ar (1000);
    }
    
    return self;
}

@end
