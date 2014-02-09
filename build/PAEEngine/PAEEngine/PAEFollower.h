//
//  PAEGate.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAEProcess.h"

/** @file */

/** A signal amplitude follower. */
@interface PAEFollower : PAEProcess

/** The lag time in seconds applied to the amplitude as it is rising.
 This defaults to having a constant of 0.01 patched to its input. 
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* attackTime;

/** The lag time in seconds applied to the amplitude as it is falling.
 This defaults to having a constant of 0.01 patched to its input. 
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* releaseTime;

/** Create a follower. */
+(PAEFollower*)followerWithNumInputs:(int)numInputs;
@end
