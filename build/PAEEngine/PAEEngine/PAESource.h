/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of University of the West of England, Bristol nor
 the names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF THE WEST OF ENGLAND, BRISTOL BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 This software makes use of third party libraries. See PAEEngine.h
 
 -------------------------------------------------------------------------------
 */

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

/** A tag for the object.
 This is not used for any purpose internally, but is provided to help identify objects in
 user applications. */
@property (nonatomic) NSInteger tag;

/** Get the current output value of a source. */
-(float)currentValueOnChannel:(int)channel;

/** Create a source by appending channels from an array of sources. */
+(PAESource*)sourceByFlatteningArray:(NSArray*)sources;

/** Get one of the channels as a new source. */
-(PAESource*)sourceWithChannel:(int)channel;

/** Get a version of this source with inverted phase. */
-(PAESource*)sourcePhaseInverted;

/** Create a source by appending channels from two sources. */
-(PAESource*)sourceWithAppendedChannels:(PAESource*)source;

/** Create a source by clipping another source to a set range. */
-(PAESource*)sourceClippedToMinimum:(float)minimum maximimum:(float)maximum;

/** Create a source by mixing down the channels in the source. */
-(PAESource*)sourceMixed;

/** Add this source to another and get the result (mix). */
-(PAESource*)addedTo:(PAESource*)source;

/** Subtract this source from another and get the result. */
-(PAESource*)subtractedFrom:(PAESource*)source;

/** Subtract another source from this source and get the result. */
-(PAESource*)subtractFrom:(PAESource*)source;

/** Multiply this source by another and get the result. */
-(PAESource*)multipliedBy:(PAESource*)source;

/** Divide this source by another and get the result. */
-(PAESource*)dividedBy:(PAESource*)source;

/** Divide this another source by this source and get the result. */
-(PAESource*)divideBy:(PAESource*)source;

/** The maximum of this and another source. */
-(PAESource*)maximumComparedTo:(PAESource*)source;

/** The minimum of this and another source. */
-(PAESource*)minimumComparedTo:(PAESource*)source;


@end
