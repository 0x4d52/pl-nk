//
//  PAESource.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"

/** @file */

/** The base source class.
 This is used as the base class for more specific audio and control sources. 
 You would not normally instantiate a PAESource object but instead create one
 of its subclasses (see class diagram). */
@interface PAESource : NSObject

/** The number of channels in the output of the source. */
@property (nonatomic, readonly) int numChannels;

/** The type name of the source.
 This will be the same name as the class name with the "PAE" prefix removed.
 For example PAESource is named "Source", PAEChannelStrip is named "ChannelStrip". */
@property (nonatomic, readonly) NSString* type;

/** A label for the object.
 The default name is the type name with a unique index appended (e.g., ChannelStrip3)
 This is not used for any purpose internally, but is provided to help identify objects in 
 user applications. */
@property (nonatomic, strong) NSString* label;

/** Get the current output value of a source. */
-(float)currentValueOnChannel:(int)channel;
@end
