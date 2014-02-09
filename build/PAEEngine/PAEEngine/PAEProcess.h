//
//  PAEProcess.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAESource.h"

/** @file */

/** The base process class.
 This is used as the base class audio and control processing. It has a single
 source as its input but this may be multichannel. 
 
 You would not normally instantiate a PAEProcess object but instead create one
 of its subclasses (see class diagram). */
@interface PAEProcess : PAESource

/** The input source. */
@property (strong, nonatomic) PAESource* input;
@end
