//
//  PAETestTone.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAESource.h"

/** @file */

/** Mix an array of PAESource objects. */
@interface PAEMixer : PAESource

/** The array of sources to mix. 
 Note that this is a read-only property. To change a mix you need to create
 a new instance and repatch it into the desitination PAEProcess or main mix in the PAEAudioHost. */
@property (strong, nonatomic, readonly) NSArray* sources;

/** Create a mixer with an array of sources. */
+(PAEMixer*)mixerWithSources:(NSArray*)sources;
@end
