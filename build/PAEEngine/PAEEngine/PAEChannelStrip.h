/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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
#import "PAEProcess.h"

/** @file */

/** A channel strip utility. 
 This helps manage a typical channel strip in a mixer-type situation. The 
 channel strip would have a source patched to its input and the channel strip
 Inserts can be added to apply EQ (using filter
 processes), level, and pan (for example using PAEPan2). */
@interface PAEChannelStrip : PAEProcess

/** An array of inserts to apply between the input and the level control. 
 The default is nil. */
@property (strong, nonatomic) NSArray* inserts;

/** Create channel strip. */
+(PAEChannelStrip*)channelStripWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs;

/** Create channel strip with the same number of input channels as output channels. */
+(PAEChannelStrip*)channelStripWithNumChannels:(int)numChannels;
@end
