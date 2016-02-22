/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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
#import "PAESource.h"

/** @file */

////////////////////////////////////////////////////////////////////////////////

/** A sine oscillator. */
@interface PAESine : PAESource

/** The oscillator frequency in Hz.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The oscillator linear amplitude (peak).
 Defaults to nil (meaning a fixed amplitude of 1) unless the object is 
 initialised with sineWithFrequency:amplitude: or sineWithFrequency:amplitude:offset: 
 If not nil, this may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* amplitude;

/** The oscillator linear amplitude (peak).
 Defaults to nil (meaning a fixed offset of 0) unless the object is
 initialised with sineWithFrequency:amplitude:offset:
 If not nil, this may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* offset;

/** Create a sine oscillator at a particular freuqency with fixed amplitude 1. */
+(PAESine*)sineWithFrequency:(float)frequency;

/** Create a sine oscillator at a particular initial freuqency and amplitude. */
+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude;

/** Create a sine oscillator at a particular freuqency and amplitude with an offset. */
+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset;
@end

////////////////////////////////////////////////////////////////////////////////

/** A sawtooth oscillator. 
 @note The osciallator starts at 0 phase and rises in value if frequency is positive. */
@interface PAESaw : PAESource

/** The oscillator frequency in Hz.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The oscillator linear amplitude (peak).
 Defaults to nil (meaning a fixed amplitude of 1) unless the object is
 initialised with sawWithFrequency:amplitude: or sineWithFrequency:amplitude:offset:
 If not nil, this may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* amplitude;

/** The oscillator linear amplitude (peak).
 Defaults to nil (meaning a fixed offset of 0) unless the object is
 initialised with sawWithFrequency:amplitude:offset:
 If not nil, this may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* offset;

/** Create a saw oscillator at a particular freuqency with fixed amplitude 1. */
+(PAESaw*)sawWithFrequency:(float)frequency;

/** Create a saw oscillator at a particular freuqency and amplitude. */
+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude;

/** Create a saw oscillator at a particular freuqency and amplitude with an offset. */
+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset;

/** Create a saw oscillator at a particular freuqency, amplitude with an offset, and initial value 
 @note Useful if you need to set the intial value as a phasor e.g., for PAEBufferLookup */
+(PAESaw*)sawWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset initialValue:(float)value;


@end
