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

/** A simple noise gate. */
@interface PAESimpleGate : PAEProcess

/** The PAEFollower object used to perform the amplitude tracking. 
 To use a different side chain the input of the follower can be repatched. */
@property (strong, nonatomic, readonly) PAEFollower* follower;

/** The attack time in seconds for the envelope applied to the gate as it opens.
 The default is a constant of 0.01.
 This is independent of the follower's attack time. 
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* attackTime;

/** The release time in seconds for the envelope applied to the gate after it closes.
 The default is a constant of 0.1.
 This is independent of the follower's release time. 
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* releaseTime;

/** The gate threshold expressed in linear amplitude. 
 Below this threshold the gate will close.
 The default is a constant of 0.2.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* threshold;

/** Create a gate. */
+(PAESimpleGate*)gateWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** A noise gate with hysterisis. 
 This has two thresholds. The upper, attackThreshold is used to open the gate
 and the lowe, releaseThreshold is used to close the gate. */
@interface PAEGate : PAEProcess

/** The PAEFollower object used to perform the amplitude tracking.
 To use a different side chain the input of the follower can be repatched. */
@property (strong, nonatomic, readonly) PAEFollower* follower;

/** The attack time in seconds for the envelope applied to the gate as it opens.
 The default is a constant of 0.01.
 This is independent of the follower's attack time.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* attackTime;

/** The release time in seconds for the envelope applied to the gate after it closes.
 The default is a constant of 0.1.
 This is independent of the follower's release time.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* releaseTime;

/** The gate attack threshold expressed in linear amplitude.
 Above this threshold the gate will open.
 This should be equal to or greater than the releaseThreshold.
 The default is a constant of 0.21.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* attackThreshold;

/** The gate release threshold expressed in linear amplitude.
 Above this threshold the gate will open.
 This should be equal to or less than the attackThreshold.
 The default is a constant of 0.19.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* releaseThreshold;

/** Create a gate. */
+(PAEGate*)gateWithNumInputs:(int)numInputs;
@end
