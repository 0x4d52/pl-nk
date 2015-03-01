//
//  PAEBufferInternal.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#include "plonk.h"

@interface PAEBufferInternal : NSObject
@property (nonatomic, readonly) int numChannels;
@property (nonatomic, readonly) NSTimeInterval duration;
@property (nonatomic, readonly) int numFrames;
@property (nonatomic, readonly) double sampleRate;
@property (nonatomic) Signal signal;
@property (nonatomic, readonly) int frameStride;
-(id)initFromPath:(NSString*)path;
-(id)initWithSize:(int)numFrames channels:(int)numChannels sampleRate:(double)sampleRate;
-(float)sampleAtIndex:(int)index channel:(int)channel;
-(float)sampleAtTime:(NSTimeInterval)time channel:(int)channel;
-(PAERange)limitsBetween:(int)startIndex end:(int)endIndex channel:(int)channel;
-(void)setSample:(float)value index:(int)index channel:(int)channel;
-(float*)samples:(int)channel;
@end

@interface PAEBuffer ()
@property (strong, nonatomic) PAEBufferInternal* internal;
@property (nonatomic, readonly) Signal signal;
-(float*)samples:(int)channel;
@end




