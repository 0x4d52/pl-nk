//
//  PAEControl.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEControl ()
{
    FloatVariable _value;
}
@end

@implementation PAEControl

+(PAEControl*)control
{
    return [[PAEControl alloc] initWithValue:0.f];
}

+(PAEControl*)controlWithValue:(float)value
{
    return [[PAEControl alloc] initWithValue:value];
}

-(id)init
{
    return nil;
}

-(id)initWithValue:(float)value
{
    if (self = [super init])
    {
        self.outputUnit = _value.ar().lag (PAEControlLagTime);
        _value.setValue (value);
    }
    
    return self;
}

-(float)value
{
    return _value.getValue();
}

-(void)setValue:(float)value
{
    _value.setValue (value);
}


@end
