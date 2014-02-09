//
//  PAEConstant.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAESource.h"

/** @file */

/** A constant value source.
 These are usually used to set a property of PAEProcess subclass to a constant 
 value for the entire runtime of the application. */
@interface PAEConstant : PAESource

/** Create a constant value source with a particular value. */
+(PAEConstant*)constantWithValue:(float)value;
@end
