//
//  PAEMixer.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEMixer ()
@end

@implementation PAEMixer

@synthesize sources = _sources;

-(NSArray*)controls
{
    return _sources;
}

+(PAEMixer*)mixerWithSources:(NSArray*)sources
{
    return [[PAEMixer alloc] initWithSources:sources];
}

-(id)initWithSources:(NSArray*)sources
{
    if (self = [super init])
    {
        Units units;
        int maxChannels = 0;
        
        for (PAESource* source in sources)
        {
            units.add (source.outputUnit);
            maxChannels = plonk::max (maxChannels, source.numChannels);
        }
        
        self.outputUnit = Mixer::ar (units, false, false, maxChannels);
        _sources = sources;
    }
    
    return self;
}

@end
