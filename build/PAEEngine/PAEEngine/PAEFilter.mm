//
//  PAEFilter.m-
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterLowPass ()
@property (strong, nonatomic) PAEProcess* frequency;
@end

@implementation PAEFilterLowPass
+(PAEFilterLowPass*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterLowPass alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.outputUnit      = LPF::ar (self.patchUnit, self.frequency.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterResonantLowPass ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* q;
@end

@implementation PAEFilterResonantLowPass
+(PAEFilterResonantLowPass*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterResonantLowPass alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.q               = [[PAEProcess alloc] initWithNumInputs:1];
        self.q.input         = [PAEConstant constantWithValue:1];
        self.outputUnit      = RLPF::ar (self.patchUnit, self.frequency.patchUnit, self.q.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterHighPass ()
@property (strong, nonatomic) PAEProcess* frequency;
@end

@implementation PAEFilterHighPass
+(PAEFilterHighPass*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterHighPass alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.outputUnit      = HPF::ar (self.patchUnit, self.frequency.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@implementation PAEFilterBlockDC
+(PAEFilterBlockDC*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterBlockDC alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.outputUnit = self.patchUnit.dc();
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterLowShelf ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* slope;
@property (strong, nonatomic) PAEProcess* gain;
@end

@implementation PAEFilterLowShelf
+(PAEFilterLowShelf*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterLowShelf alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.slope           = [[PAEProcess alloc] initWithNumInputs:1];
        self.slope.input     = [PAEConstant constantWithValue:1];
        self.gain            = [[PAEProcess alloc] initWithNumInputs:1];
        self.gain.input      = [PAEConstant constantWithValue:0];
        self.outputUnit      = LowShelf::ar (self.patchUnit, self.frequency.patchUnit, self.slope.patchUnit, self.gain.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterHighShelf ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* slope;
@property (strong, nonatomic) PAEProcess* gain;
@end

@implementation PAEFilterHighShelf
+(PAEFilterHighShelf*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterHighShelf alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.slope           = [[PAEProcess alloc] initWithNumInputs:1];
        self.slope.input     = [PAEConstant constantWithValue:1];
        self.gain            = [[PAEProcess alloc] initWithNumInputs:1];
        self.gain.input      = [PAEConstant constantWithValue:0];
        self.outputUnit      = HighShelf::ar (self.patchUnit, self.frequency.patchUnit, self.slope.patchUnit, self.gain.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterNotch ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* q;
@property (strong, nonatomic) PAEProcess* gain;
@end

@implementation PAEFilterNotch
+(PAEFilterNotch*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterNotch alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.q               = [[PAEProcess alloc] initWithNumInputs:1];
        self.q.input         = [PAEConstant constantWithValue:1];
        self.gain            = [[PAEProcess alloc] initWithNumInputs:1];
        self.gain.input      = [PAEConstant constantWithValue:0];
        self.outputUnit      = Notch::ar (self.patchUnit, self.frequency.patchUnit, self.q.patchUnit, self.gain.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterBandPass ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* bandwidth;
@end

@implementation PAEFilterBandPass
+(PAEFilterBandPass*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterBandPass alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.bandwidth       = [[PAEProcess alloc] initWithNumInputs:1];
        self.bandwidth.input = [PAEConstant constantWithValue:1];
        self.outputUnit      = BPF::ar (self.patchUnit, self.frequency.patchUnit, self.bandwidth.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
@interface PAEFilterBandStop ()
@property (strong, nonatomic) PAEProcess* frequency;
@property (strong, nonatomic) PAEProcess* bandwidth;
@end

@implementation PAEFilterBandStop
+(PAEFilterBandStop*)filterWithNumInputs:(int)numInputs;
{
    return [[PAEFilterBandStop alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.frequency       = [[PAEProcess alloc] initWithNumInputs:1];
        self.frequency.input = [PAEConstant constantWithValue:1200];
        self.bandwidth       = [[PAEProcess alloc] initWithNumInputs:1];
        self.bandwidth.input = [PAEConstant constantWithValue:1];
        self.outputUnit      = BRF::ar (self.patchUnit, self.frequency.patchUnit, self.bandwidth.patchUnit);
    }
    
    return self;
}
@end
////////////////////////////////////////////////////////////////////////////////


