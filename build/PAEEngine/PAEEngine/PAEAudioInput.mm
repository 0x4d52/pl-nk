//
//  PAEAudioInput.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"


@implementation PAEAudioInput

+(PAEAudioInput*)audioInputWithNumChannels:(int)numChannels
{
    return [[PAEAudioInput alloc] initWithNumChannels:numChannels];
}

-(id)init
{
    return nil;
}

-(id)initWithNumChannels:(int)numChannels
{
    if (numChannels < 1)
        return nil;
    
    if (self = [super init])
    {
        Busses busses;
        
        for (int i = 0; i < numChannels; ++i)
            busses.add (Bus (i));
        
        self.outputUnit = BusRead::ar (busses);
    }
    
    return self;
}

@end
