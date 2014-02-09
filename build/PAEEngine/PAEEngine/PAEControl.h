//
//  PAEControl.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAESource.h"

/** @file */

/** A variable control. 
 These are usually used control a parameter of a PAEProcess that can be changed
 during the running of the application. */
@interface PAEControl : PAESource

/** The current value. */
@property (nonatomic) float value;

/** Create a control with a initial value of zero. */
+(PAEControl*)control;

/** Create a control with a specific initial value. */
+(PAEControl*)controlWithValue:(float)value;
@end
