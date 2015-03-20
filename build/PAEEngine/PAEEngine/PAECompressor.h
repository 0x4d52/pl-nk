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
#import "PAEProcess.h"

/** @file */

/** A compressor. */
@interface PAECompressor : PAEProcess

/** The PAEFollower object used to perform the amplitude tracking. 
 To use a different side chain the input of the follower can be repatched. */
@property (strong, nonatomic, readonly) PAEFollower* follower;

/** The attack time in seconds for the envelope applied to the compressor.
 The default is a constant of 0.01.
 This is independent of the follower's attack time. 
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* attackTime;

/** The release time in seconds for the envelope applied to the compressor.
 The default is a constant of 0.1.
 This is independent of the follower's release time. 
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* releaseTime;

/** The compressor threshold expressed in linear amplitude. 
 The default is a constant of 0.2.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* threshold;

/** The compressor ratio, n:1
 The default is a constant of 0.5, ie. 2:1
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* ratio;

/** Access to the internal compressor control state. */
@property (strong, nonatomic, readonly) PAESource* control;

/** Create a compressor. */
+(PAECompressor*)compressorWithNumInputs:(int)numInputs;
@end

