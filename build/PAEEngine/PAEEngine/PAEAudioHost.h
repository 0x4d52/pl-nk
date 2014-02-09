//
//  PAEAudioHost.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

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

/** Create an audio host for output only. */
+(PAEAudioHost*)audioHostWithNumOutputs:(int)numOutputs;

/** Create an audio host for input and output. */
+(PAEAudioHost*)audioHostWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs;

/** Start the audio host. */
-(void)start;

/** Stop the audio host. */
-(void)stop;

@end
