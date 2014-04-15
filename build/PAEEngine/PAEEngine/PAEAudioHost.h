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

/** The main audio host "engine".
 To use this:
 - Create one of these objects and store it (e.g., in you app delegate).
 - Set the mainMix property to the source to play.
 - use the start message to start the processing. */
@interface PAEAudioHost : NSObject

/** The main mix to play.
 For more complex playback use PAEMixer and PAEChannelStrip objects to
 construct mixer patching graphs. */
@property (strong, nonatomic) PAESource* mainMix;

/** An array of sources to render before the main mix.
 Any sources not part of the main mix graph will not be rendered at runtime. 
 For example, a PAEFollower can be used to track the amplitude of a PAESource
 but for it to generate the values it needs to be in the main mix graph (e.g., 
 as in the PAEGate) or placed in this array for separate rendering. */
@property (strong, nonatomic) NSArray* additionalRenderSources;

/** The number of audio output channels.
 This is readonly once the host has started. */
@property (nonatomic) int numOutputs;

/** The number of audio input channels.
 This is readonly once the host has started.  */
@property (nonatomic) int numInputs;

/** Harware block size.
 This is readonly once the host has started.  */
@property (nonatomic) int hardwareBlockSize;

/** Process block size.
 This is readonly once the host has started.  */
@property (nonatomic) int processBlockSize;

/** Sample rate.
 This is readonly once the host has started.  */
@property (nonatomic) double sampleRate;

/** CPU Usage between 0.0 - 1.0. */
@property (nonatomic, readonly) double cpuUsage;

/** Create an audio host for output only. */
+(PAEAudioHost*)audioHostWithNumOutputs:(int)numOutputs;

/** Create an audio host for input and output. */
+(PAEAudioHost*)audioHostWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs;

/** Start the audio host. */
-(void)start;

/** Stop the audio host. */
-(void)stop;



@end
