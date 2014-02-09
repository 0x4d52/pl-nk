//
//  PAEFilter.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAEProcess.h"

/** @file */

////////////////////////////////////////////////////////////////////////////////

/** Low-pass filter process. */
@interface PAEFilterLowPass : PAEProcess

/** The filter cut-off frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** Create a low-pass filter. */
+(PAEFilterLowPass*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** Resonant low-pass filter process. */
@interface PAEFilterResonantLowPass : PAEProcess

/** The filter cut-off frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The filter resonance expressed as a Q factor. 
 Higher values result in a narrower resonant peak.
 Low values may make the filter unstable at some frequencies.
 The default is a constant of 1.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* q;

/** Create a resonant low-pass filter. */
+(PAEFilterResonantLowPass*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** High-pass filter process. */
@interface PAEFilterHighPass : PAEProcess

/** The filter cut-off frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** Create a high-pass filter. */
+(PAEFilterHighPass*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** DC-blocking filter process. */
@interface PAEFilterBlockDC : PAEProcess

/** Create a DC-blocking filter. */
+(PAEFilterBlockDC*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** Low-shelving filter process.
 Typical "bass" EQ control. */
@interface PAEFilterLowShelf : PAEProcess

/** The filter turn-over frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The filter turn-over slope.
 The is the steepness of the change from 0dB to the gain setting.
 Higher values are steeper (like Q).
 The default is a constant of 1 and is usually sufficient.
 Lower values may make the filter unstable at some frequencies.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* slope;

/** The filter gain or attenuation below the turnover frequency in dB.
 The default is a constant of 0.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* gain;

/** Create a low-shelving filter. */
+(PAEFilterLowShelf*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** High-shelving filter process. 
 Typical "treble" EQ control. */
@interface PAEFilterHighShelf : PAEProcess

/** The filter turn-over frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The filter turn-over slope.
 The is the steepness of the change from 0dB to the gain setting.
 Higher values are steeper (like Q).
 The default is a constant of 1 and is usually sufficient.
 Lower values may make the filter unstable at some frequencies.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* slope;

/** The filter gain or attenuation above the turnover frequency in dB.
 The default is a constant of 0.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* gain;

/** Create a high-shelving filter. */
+(PAEFilterHighShelf*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** Notch filter process. 
 Typical mid-band paramteric EQ. */
@interface PAEFilterNotch : PAEProcess

/** The filter centre frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The filter resonance expressed as a Q factor.
 The default is a constant of 1.
 Low values may make the filter unstable at some frequencies.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* q;

/** The filter gain or attenuation around the centre frequency in dB.
 The default is a constant of 0.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* gain;

/** Create a notch filter. */
+(PAEFilterNotch*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** Band-pass filter process. */
@interface PAEFilterBandPass : PAEProcess

/** The filter centre frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The filter bandwidth expressed in octaves.
 The default is a constant of 1.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* bandwidth;

/** Create a band-pass filter. */
+(PAEFilterBandPass*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////

/** Band-stop filter process. */
@interface PAEFilterBandStop : PAEProcess

/** The filter centre frequency in Hz.
 The default is a constant of 1200.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* frequency;

/** The filter bandwidth expressed in octaves.
 The default is a constant of 1.
 This may be repatched with any other PAESource object as desired. */
@property (strong, nonatomic, readonly) PAEProcess* bandwidth;

/** Create a band-stop filter. */
+(PAEFilterBandStop*)filterWithNumInputs:(int)numInputs;
@end

////////////////////////////////////////////////////////////////////////////////






