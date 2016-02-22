//
//  PAEBufferPlayer.m
//  PAEEngine
//
//  Created by Martin Robinson on 05/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEBufferPlayer ()
@property (strong, nonatomic) PAEBuffer* buffer;
@property (strong, nonatomic) PAEProcess* rate;
@end

@implementation PAEBufferPlayer
{
    FloatVariable _loopFlag;
}

-(NSArray*)controls
{
    return @[self.rate];
}

+(PAEBufferPlayer*)bufferPlayerWithBuffer:(PAEBuffer*)buffer
{
    return [[PAEBufferPlayer alloc] initWithBuffer:buffer];
}

-(id)initWithBuffer:(PAEBuffer*)buffer
{
    if (!buffer)
        return nil;

    if (self = [super init])
    {
        self.buffer = buffer;
        _loopFlag.setValue (1.f);
        
        self.rate       = [[PAEProcess alloc] initWithNumInputs:1];
        self.rate.input = [PAEConstant constantWithValue:1];
                
        self.outputUnit = SignalPlay::ar (self.buffer.signal,
                                          self.rate.patchUnit,
                                          _loopFlag.kr());
    }
    
    return self;
}

-(BOOL)loop
{
    return _loopFlag.getValue() == 1.f;
}

-(void)setLoop:(BOOL)loop
{
    _loopFlag.setValue (loop ? 1.f : 0.f);
}


@end
