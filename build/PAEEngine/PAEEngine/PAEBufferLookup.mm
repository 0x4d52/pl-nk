//
//  PAEBufferPlayer.m
//  PAEEngine
//
//  Created by Martin Robinson on 05/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEBufferLookup ()
@property (strong, nonatomic) PAEBuffer* buffer;
@property (strong, nonatomic) PAEProcess* position;
@end

@implementation PAEBufferLookup

+(PAEBufferLookup*)bufferLookupWithBuffer:(PAEBuffer*)buffer
{
    return [[PAEBufferLookup alloc] initWithBuffer:buffer];
}

-(id)initWithBuffer:(PAEBuffer*)buffer
{
    if (!buffer)
        return nil;
    
    if (self = [super init])
    {
        self.buffer = buffer;
        
        double duration = buffer.duration;
        
        self.position       = [[PAEProcess alloc] initWithNumInputs:1];
        self.position.repatchFadeTime = 0;
        self.position.input = [PAESaw sawWithFrequency:1.0 / duration
                                             amplitude:duration / 2.0
                                                offset:duration / 2.0];
        
        self.outputUnit = SignalRead::ar (self.buffer.signal, self.position.patchUnit);
    }
    
    return self;
}

@end
