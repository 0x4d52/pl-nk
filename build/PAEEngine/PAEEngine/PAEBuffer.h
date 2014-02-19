/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of University of the West of England, Bristol nor
 the names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF THE WEST OF ENGLAND, BRISTOL BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 This software makes use of third party libraries. See PAEEngine.h
 
 -------------------------------------------------------------------------------
 */

#import <Foundation/Foundation.h>
#import "PAEFoward.h"

/** @file */

/** A buffer stores audio data. */
@interface PAEBuffer : NSObject

/** The number of channels in the buffer. */
@property (nonatomic, readonly) int numChannels;

/** The duration of the buffer in seconds. */
@property (nonatomic, readonly) NSTimeInterval duration;

/** The duration of the buffer in frames. */
@property (nonatomic, readonly) int numFrames;

/** The sample rate of the buffer. */
@property (nonatomic, readonly) double sampleRate;

/** Create a buffer from an audio file */
+(PAEBuffer*)bufferNamed:(NSString*)name;

/** Create an empty buffer. */
+(PAEBuffer*)bufferWithSize:(int)numFrames channels:(int)numChannels sampleRate:(double)sampleRate;

/** Get a sample at a specific index on a specific channel. */
-(float)sampleAtIndex:(int)index channel:(int)channel;

/** Get a sample interpolated at a specific time on a specific channel. */
-(float)sampleAtTime:(NSTimeInterval)time channel:(int)channel;

/** Get a sample interpolated at a specific phase (time mapped to 0 to 1) on a specific channel. */
-(float)sampleAtPhase:(NSTimeInterval)phase channel:(int)channel;

/** Get a sample at a specific index on channel 0. */
-(float)sampleAtIndex:(int)index;

/** Get a sample interpolated at a specific time on channel 0. */
-(float)sampleAtTime:(NSTimeInterval)time;

/** Get a sample interpolated at a specific phase (time mapped to 0 to 1) on channel 0. */
-(float)sampleAtPhase:(NSTimeInterval)phase;

/** Get the minimum and maximum sample values between the indices. */
-(PAERange)limitsBetween:(int)startIndex end:(int)endIndex channel:(int)channel;

/** Set a sample at a specific index on a specific channel. */
-(void)setSample:(float)value index:(int)index channel:(int)channel;

@end
