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
    
    return 0.f;
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
    
    return 0.f;
}

-(float)randomWithinInputRange
{
    switch (self.type)
    {
        case PAEMapTypeLinearToLinear:
        case PAEMapTypeLinearToExponential:
            return plonk::rand (self.inputMinimum, self.inputMaximum);
        case PAEMapTypeExponentialToLinear:
            return plonk::exprand (self.inputMinimum, self.inputMaximum);
    }
    
    return 0.f;
}

-(float)randomWithinOutputRange
{
    switch (self.type)
    {
        case PAEMapTypeLinearToLinear:
        case PAEMapTypeExponentialToLinear:
            return plonk::rand (self.outputMinimum, self.outputMaximum);
        case PAEMapTypeLinearToExponential:
            return plonk::exprand (self.outputMinimum, self.outputMaximum);
    }
    
    return 0.f;

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

-(PAERange)inputRange
{
    return PAERangeMake (self.inputMinimum, self.inputMaximum);
}

-(PAERange)outputRange
{
    return PAERangeMake (self.outputMinimum, self.outputMaximum);
}

-(void)setInputRange:(PAERange)inputRange
{
    self.inputMinimum = inputRange.minimum;
    self.inputMaximum = inputRange.maximum;
}

-(void)setOutputRange:(PAERange)outputRange
{
    self.outputMinimum = outputRange.minimum;
    self.outputMaximum = outputRange.maximum;
}

@end

////////////////////////////////////////////////////////////////////////////////


//@implementation PAEProcessMap
//{
//    PAEProcess* _mapper;
//}
//
//@synthesize map = _map;
//
//-(id)initWithNumInputs:(int)numInputs
//{
//    if (self = [super initWithNumInputs:numInputs])
//    {
//        _mapper = [[PAEProcess alloc] initWithNumInputs:numInputs];
//        _mapper.inputUnit = self.patchUnit;
//        self.outputUnit = _mapper.outputUnit;
//    }
//    
//    return self;
//}
//
//-(PAEMap*)map
//{
//    return _map;
//}
//
//-(void)setMap:(PAEMap *)map
//{
//    if (!map)
//    {
//        _mapper.inputUnit = self.patchUnit;
//    }
//    else
//    {
//        switch (map.type)
//        {
//            case PAEMapTypeLinearToLinear:
//                _mapper.inputUnit = self.patchUnit.linlin(map.inputMinimum, map.input, <#const plonk::UnitBase<float> &outLow#>, <#const plonk::UnitBase<float> &outHigh#>)
//                break;
//            case PAEMapTypeLinearToExponential:
//                break;
//            case PAEMapTypeExponentialToLinear:
//                break;
//        }
//    }
//    
//    _map = map;
//}
//
//@end
