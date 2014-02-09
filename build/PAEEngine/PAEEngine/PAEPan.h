//
//  PAEPan.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAEProcess.h"

/** @file */

/** A pan processor. 
 This may have one or twp input channels and always outputs two channels. */
@interface PAEPan2 : PAEProcess

/** The pan position in the range -1 to +1.
 This Control is allocated internally and can be set only by setting the
 value of this PAEControl object. This can't be modulated by a PAESource object. 
 The value defaults to 0 which is the centre. */
@property (strong, nonatomic, readonly) PAEControl* position;

/** Create a pan process. */
+(PAEPan2*)pan2WithNumInputs:(int)numInputs;
@end
