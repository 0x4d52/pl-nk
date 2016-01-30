//
//  PAEConvolve.m-
//  PAEEngine
//
//  Created by Martin Robinson on 29/01/2016.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

static const int defaultFFTSize = 512;
static const int minFFTSize     = 64;
static const int maxFFTSize     = 16384;
static const int numFadeSamples = 256;

@interface PAEConvolve ()
{
    FFTBuffersVariable _fftBuffers;
    PAEBuffer* _ir;
    int _fftSize;
    FloatVariable _gain;
    BOOL _usingAutoGain;
}
@end

@implementation PAEConvolve
+(PAEConvolve*)convolveWithNumInputs:(int)numInputs;
{
    return [[PAEConvolve alloc] initWithNumInputs:numInputs];
}

-(id)initWithNumInputs:(int)numInputs
{
    if (self = [super initWithNumInputs:numInputs])
    {
        self.fftSize       = defaultFFTSize;
        self.gain          = 1.0f;
        self.usingAutoGain = YES;
        
        Buffer identityChannel (Buffer::newClear (self.fftSize / 2));
        identityChannel.atUnchecked (0) = 1.0f;
        Buffers identity;
        
        for (int i = 0; i < numInputs; ++i)
            identity.add (identityChannel);
        
        self.ir         = [[PAEBuffer alloc] initWithSignal:Signal (identity)];
        self.outputUnit = Convolve::ar (self.patchUnit, _fftBuffers, true, numInputs, maxFFTSize, numFadeSamples) * _gain.ar();
    }
    
    return self;
}

-(void)updateIR
{
    if (! _ir)
        return;

    _fftBuffers = FFTBuffers (self.fftSize, _ir.signal);
}

-(PAEBuffer*)ir
{
    return _ir;
}

-(void)setIr:(PAEBuffer*)ir
{
    if (_ir != ir)
    {
        _ir = ir;
        
        [self updateGain];
        [self updateIR];
    }
}

-(int)fftSize
{
    return _fftSize;
}

-(void)setFftSize:(int)fftSize
{
    fftSize = plonk::clip (fftSize, minFFTSize, maxFFTSize);
    
    if (_fftSize != fftSize)
    {
        _fftSize = fftSize;
        [self updateIR];
    }
}

-(float)gain
{
    return _gain.getValue();
}

-(void)setGain:(float)gain
{
    _gain.setValue (gain);
}

-(void)updateGain
{
    if (_usingAutoGain && self.ir.numFrames > 0)
    {
        self.gain = 1.0f / self.ir.maxAbsSum;
    }
}

-(BOOL)isUsingAutoGain
{
    return _usingAutoGain;
}

-(void)setUsingAutoGain:(BOOL)usingAutoGain
{
    if (_usingAutoGain != usingAutoGain)
    {
        _usingAutoGain = usingAutoGain;
        [self updateGain];
    }
}

@end


