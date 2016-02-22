//
//  PAEPan.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEPan2 ()
@property (strong, nonatomic) PAEControl* position;
@end

@implementation PAEPan2
{
    FloatVariable _leftLevel;
    FloatVariable _rightLevel;
}

-(NSArray*)controls
{
    return @[self.position];
}

+(PAEPan2*)pan2WithNumInputs:(int)numInputs;
{
    return [[PAEPan2 alloc] initWithNumInputs:plonk::clip(numInputs, 1, 2)];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        float initialPosition = 0;
        self.position = [PAEControl controlWithValue:initialPosition];
        [self positionChanged:initialPosition];
        [self.position addObserver:self forKeyPath:@"value" options:0 context:nil];
        
        Unit panLevels (_leftLevel.ar(), _rightLevel.ar());
        self.outputUnit = self.patchUnit * panLevels.lag (PAEControlLagTime);
    }
    
    return self;
}

-(void)dealloc
{
    [self.position removeObserver:self forKeyPath:@"value"];
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    assert (object == self.position);
    PAEControl* control = (PAEControl*)object;
    [self positionChanged:control.value];
}

-(void)positionChanged:(float)position
{
//    const FloatWavetable& table (FloatWavetable::sine512());
//    const int length = table.length();
    
    const float w = position * Math<float>::getPi_4(); // scale to +- pi/4 (+-45 degress)
    const float cosw = plonk::cos (w);
    const float sinw = plonk::sin (w);
    const float sqrt2_2 = Math<float>::getSqrt2_2();

    _leftLevel.setValue ((cosw - sinw) * sqrt2_2);
    _rightLevel.setValue ((cosw + sinw) * sqrt2_2);
}

@end

