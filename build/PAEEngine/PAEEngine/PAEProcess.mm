//
//  PAEProcessor.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEProcess ()
{
    UnitVariable _inputUnitVariable;
    Unit _patchUnit;
    FloatVariable _repatchFadeTime;
}
@end

@implementation PAEProcess

@synthesize input = _input;

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super init])
    {
        _repatchFadeTime.setValue (0.f);
        _patchUnit = Patch::ar (_inputUnitVariable, false, numInputs, _repatchFadeTime);
        _repatchFadeTime.setValue (PAERepatchFadeTime);
        self.outputUnit = _patchUnit; // pass through as default
    }
    
    return self;
}

-(PAESource*)input
{
    return _input;
}

-(void)setInput:(PAESource*)input
{
    self.inputUnit = input ? input.outputUnit : Unit::getNull();
    _input = input;
}

-(int)numInputs
{
    return _patchUnit.getNumChannels();
}

-(Unit)inputUnit
{
    return _inputUnitVariable.getValue();
}

-(void)setInputUnit:(Unit)inputUnit
{
    _inputUnitVariable.setValue (inputUnit);
}

-(Unit)patchUnit
{
    return _patchUnit;
}

-(float)repatchFadeTime
{
    return _repatchFadeTime.getValue();
}

-(void)setRepatchFadeTime:(float)repatchFadeTime
{
    _repatchFadeTime.setValue (repatchFadeTime);
}

@end
