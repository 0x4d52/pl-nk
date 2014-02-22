/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

/** A range of values.
 Like NSRange but using floating point valeus. */
typedef struct PAERange
{
    float minimum;
    float maximum;
} PAERange;

/** Convenience function to create a PAERange. */
static inline PAERange PAERangeMake (float minimum, float maximum)
{
    PAERange r = { minimum, maximum };
    return r;
}

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

/** The input range as a PAERange. */
@property (nonatomic) PAERange inputRange;

/** The output range as a PAERange. */
@property (nonatomic) PAERange outputRange;

/** Create mapping object. */
+(PAEMap*)mapWithType:(PAEMapType)type;

/** Map a value from the input range to output range. */
-(float)mapInputToOutput:(float)input;

/** Map a value from the output range to the input range. */
-(float)mapOutputToInput:(float)output;

/** Create a random value within the input range. */
-(float)randomWithinInputRange;

/** Create a random value within the output range. */
-(float)randomWithinOutputRange;

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

/** Source value mapping.
 This maps the output value of a source*/
@interface PAEProcessMap : PAEProcess
@property (nonatomic, strong) PAEMap* map;
+(PAEProcessMap*)processMapWithNumInputs:(int)numInputs;
@end

