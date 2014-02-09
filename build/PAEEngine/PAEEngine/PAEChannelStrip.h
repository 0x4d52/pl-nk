//
//  PAEChannelStrip.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAEProcess.h"

/** @file */

/** A channel strip utility. 
 This helps manage a typical channel strip in a mixer-type situation. The 
 channel strip would have a source patched to its input and the channel strip
 has a single level control. Inserts can be added to apply EQ (using filter
 processes) and pan (for example using PAEPan2).
 Aux sends are not yet implemented. */
@interface PAEChannelStrip : PAEProcess

/** The level control for the channel strip expressed as linear amplitude. 
 The default is a constant of 1.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* level;

/** An array of inserts to apply between the input and the level control. 
 The default is nil. */
@property (strong, nonatomic) NSArray* inserts;

/** Create channel strip. */
+(PAEChannelStrip*)channelStripWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs;

/** Create channel strip with the same number of input channels as output channels. */
+(PAEChannelStrip*)channelStripWithNumChannels:(int)numChannels;
@end
