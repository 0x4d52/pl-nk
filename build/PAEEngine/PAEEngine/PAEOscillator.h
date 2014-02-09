//
//  PAEOscillator.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAESource.h"

/** @file */

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

/** Create a sine oscillator at a particular freuqency and amplitude. */
+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude;

/** Create a sine oscillator at a particular freuqency and amplitude with an offset. */
+(PAESine*)sineWithFrequency:(float)frequency amplitude:(float)amplitude offset:(float)offset;
@end
