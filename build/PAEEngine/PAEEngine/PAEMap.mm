//
//  PAEMap.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@implementation PAEMap

+(PAEMap*)mapWithType:(PAEMapType)type
{
    return [[PAEMap alloc] initWithType:type];
}

-(id)initWithType:(PAEMapType)type
{
    if (self = [super init])
    {
        self.type = type;
        
        switch(type)
        {
            case PAEMapTypeLinearToLinear:
                self.inputMinimum = 0;
                self.inputMaximum = 1;
                self.outputMinimum = 0;
                self.outputMinimum = 100;
                break;
            case PAEMapTypeLinearToExponential:
                self.inputMinimum = 0;
                self.inputMaximum = 1;
                self.outputMinimum = 100;
                self.outputMinimum = 1000;
                break;
            case PAEMapTypeExponentialToLinear:
                self.inputMinimum = 100;
                self.inputMaximum = 1000;
                self.outputMinimum = 0;
                self.outputMinimum = 1;
                break;
        }
    }
    
    return self;
}

-(float)mapInputToOutput:(float)input
{
    switch (self.type)
    {
        case PAEMapTypeLinearToLinear:
            return plonk::linlin (input, self.inputMinimum, self.inputMaximum, self.outputMinimum, self.outputMaximum);
        case PAEMapTypeLinearToExponential:
            return plonk::linexp (input, self.inputMinimum, self.inputMaximum, self.outputMinimum, self.outputMaximum);
        case PAEMapTypeExponentialToLinear:
            return plonk::explin (input, self.inputMinimum, self.inputMaximum, self.outputMinimum, self.outputMaximum);
    }
}

-(float)mapOutputToInput:(float)output
{
    switch (self.type)
    {
        case PAEMapTypeLinearToLinear:
            return plonk::linlin (output, self.outputMinimum, self.outputMaximum, self.inputMinimum, self.inputMaximum);
        case PAEMapTypeLinearToExponential:
            return plonk::explin (output, self.outputMinimum, self.outputMaximum, self.inputMinimum, self.inputMaximum);
        case PAEMapTypeExponentialToLinear:
            return plonk::linexp (output, self.outputMinimum, self.outputMaximum, self.inputMinimum, self.inputMaximum);
    }
}

+(float)midiNoteToFrequency:(float)midiNote
{
    return plonk::m2f (midiNote);
}

+(float)frequencyToMidiNote:(float)frequency
{
    return plonk::f2m (frequency);
}

+(float)ampltudeToDecibels:(float)amplitude
{
    return plonk::a2dB (amplitude);
}

+(float)decibelsToAmplitude:(float)decibels
{
    return plonk::dB2a (decibels);
}

+(float)qToBandwidth:(float)q
{
    return plonk::q2bw (q);
}

+(float)bandwidthToQ:(float)bandwidth
{
    return plonk::bw2Q (bandwidth);
}



@end
