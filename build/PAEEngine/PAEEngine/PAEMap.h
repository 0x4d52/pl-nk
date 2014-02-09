//
//  PAEMap.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"

/** @file */

/** To store mapping types. */
typedef int PAEMapType;

/** Types of value range mapping. */
enum
{
    /** A linear to linear range mapping type. */
    PAEMapTypeLinearToLinear = 0,
    
    /** A linear to exponential range mapping type. */
    PAEMapTypeLinearToExponential = 1,
    
    /** A exponential to linear range mapping type. */
    PAEMapTypeExponentialToLinear = 2
};

/** Value mapping. 
 Instances of this class can map from one range to another. */
@interface PAEMap : NSObject

/** The mapping type. */
@property (nonatomic) PAEMapType type;

/** The lower bound of the input range. 
 For the PAEMapTypeExponentialToLinear type this value must not equal zero and
 the inputMaximum property must have the same sign (i.e., both positive or both negative). */
@property (nonatomic) float inputMinimum;

/** The upper bound of the input range. 
 For the PAEMapTypeExponentialToLinear type this value must not equal zero and
 the inputMinimum property must have the same sign (i.e., both positive or both negative). */
@property (nonatomic) float inputMaximum;

/** The lower bound of the output range.
 For the PAEMapTypeLinearToExponential type this value must not equal zero and
 the outputMaximum property must have the same sign (i.e., both positive or both negative). */
@property (nonatomic) float outputMinimum;

/** The upper bound of the input range. 
 For the PAEMapTypeLinearToExponential type this value must not equal zero and
 the outputMinimum property must have the same sign (i.e., both positive or both negative). */
@property (nonatomic) float outputMaximum;

/** Create mapping object. */
+(PAEMap*)mapWithType:(PAEMapType)type;

/** Map a value from the input range to output range. */
-(float)mapInputToOutput:(float)input;

/** Map a value from the output range to the input range. */
-(float)mapOutputToInput:(float)output;

/** Utility method to convert MIDI note values to frequency in Hz. */
+(float)midiNoteToFrequency:(float)midiNote;

/** Utility method to convert frequency in Hz to MIDI note values. */
+(float)frequencyToMidiNote:(float)frequency;

/** Utility method to convert linear amplitude to decibels. */
+(float)ampltudeToDecibels:(float)amplitude;

/** Utility method to convert decibels to linear amplitude. */
+(float)decibelsToAmplitude:(float)decibels;

/** Utility method to convert a filter Q to bandwidth in octaves. */
+(float)qToBandwidth:(float)q;

/** Utility method to convert bandwidth in octaves to a filter Q factor. */
+(float)bandwidthToQ:(float)bandwidth;

@end
