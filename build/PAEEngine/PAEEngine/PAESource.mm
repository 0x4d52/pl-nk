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

-(PAESource*)sourceWithChannel:(int)channel
{
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = _outputUnitVariable.getValue() [channel];
    return proxy;
}

-(PAESource*)sourcePhaseInverted
{
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = _outputUnitVariable.getValue().neg();
    return proxy;
}

-(PAESource*)sourceWithAppendedChannels:(PAESource*)source
{
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = Unit (self.outputUnit, source.outputUnit);
    return proxy;
}

-(PAESource*)sourceByFlatteningArray:(NSArray*)sources
{
    Unit unit;
    
    for (PAESource* source in sources)
    {
        if (unit.isNull())
        {
            unit = source.outputUnit;
        }
        else
        {
            unit = Unit (unit, source.outputUnit);
        }
    }
    
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = unit;
    return proxy;
}

-(PAESource*)sourceClippedToMinimum:(float)minimum maximimum:(float)maximum
{
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = _outputUnitVariable.getValue().max (minimum).min (maximum);
    return proxy;
}

-(PAESource*)sourceMixed
{
    PAESource* proxy = [[PAESource alloc] init];
    proxy.outputUnit = _outputUnitVariable.getValue().mix();
    return proxy;
}

@end
