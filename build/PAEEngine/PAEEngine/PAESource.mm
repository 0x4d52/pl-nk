//
//  PAESource.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@implementation PAESource
{
    UnitVariable _outputUnitVariable;
}

+(NSMutableDictionary*)countDictionary
{    
    static dispatch_once_t once;
    static NSMutableDictionary *countDictionary;
    dispatch_once(&once, ^ { countDictionary = [[NSMutableDictionary alloc] init]; });
    return countDictionary;
}

-(id)init
{
    if (self = [super init])
    {
        NSMutableDictionary *countDictionary = [PAESource countDictionary];
        NSString *type = self.type;
        NSNumber* index;
        
        @synchronized(countDictionary)
        {
            index = countDictionary[type];
            index = index ? [NSNumber numberWithInt:index.intValue + 1] : [NSNumber numberWithInt:0];
            countDictionary[type] = index;
        }
        
        self.label = [type stringByAppendingFormat:@"%d", index.intValue];
    }

    return self;
}

-(Unit)outputUnit
{
    return _outputUnitVariable.getValue();
}

-(void)setOutputUnit:(Unit)outputUnit
{
    _outputUnitVariable.setValue (outputUnit);
}

-(int)numChannels
{
    return _outputUnitVariable.getValue().getNumChannels();
}

-(NSString*)type
{
    return [NSStringFromClass (self.class) substringFromIndex:3]; // strip off "PAE"
}

-(float)currentValueOnChannel:(int)channel;
{
    return _outputUnitVariable.getValue().getValue (channel);
}

@end
