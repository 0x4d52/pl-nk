//
//  PAEBuffer.m
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@implementation PAEBufferInternal
{
    Signal _signal;
}

-(id)initFromPath:(NSString*)path
{
    if (self = [super init])
    {
        AudioFileReader reader ([path UTF8String]);
        
        if (!reader.isReady())
            return nil;
        
        const long long  size = reader.getNumFrames() * reader.getNumChannels();
        
        if (size > 0x7fffffff)
            return nil;
        
        Floats data = Floats::withSize (int (size) + reader.getNumChannels());
        IntVariable oneLoop (1);
        reader.readFrames (data, oneLoop);
        
        // for interleaving
        for (int i = 0; i < reader.getNumChannels(); ++i)
            data.put (int (size) + i, data.atUnchecked (i));
        
        data.setSize (int (size), true);
        
        _signal = Signal (data, reader.getSampleRate(), reader.getNumChannels());
    }
    
    return self;
}

-(id)initWithSize:(int)numFrames channels:(int)numChannels sampleRate:(double)sampleRate
{
    if (self = [super init])
    {
        const Floats samples = Floats::newClear (plonk::max (numFrames, 1) * plonk::max (numChannels, 1));
        _signal = Signal (samples, plonk::max (0.0, sampleRate), plonk::max (numChannels, 1));
    }
    
    return self;
}

-(Signal)signal
{
    return _signal;
}

-(int)numChannels
{
    return _signal.getNumChannels();
}

-(NSTimeInterval)duration
{
    return _signal.getDuration();
}

-(int)numFrames
{
    return _signal.getNumFrames();
}

-(double)sampleRate
{
    return _signal.getSampleRate().getValue();
}

-(int)frameStride
{
    return _signal.getFrameStride();
}

-(float*)samples:(int)channel
{
    return _signal.getSamples (channel);
}

-(float)sampleAtIndex:(int)index channel:(int)channel
{
    const float* samples = _signal.getSamples (channel);
    const int frameStride = _signal.getFrameStride();
    return *(samples + plonk::clip (index, 0, _signal.getNumFrames() - 1) * frameStride);
}

-(float)sampleAtTime:(NSTimeInterval)time channel:(int)channel
{
    const float* samples = _signal.getSamples (channel);
    const int frameStride = _signal.getFrameStride();
    const double position = time * self.sampleRate;
    
    const unsigned int sampleA (plonk::clip (int (position), 0, _signal.getNumFrames() - 1));
    const unsigned int sampleB (sampleA + 1);
    const double frac (plonk::frac (position));
    const float value = InterpLinear<float,double>::interp (samples[sampleA * frameStride],
                                                            samples[sampleB * frameStride],
                                                            frac);
    return value;
}

-(PAERange)limitsBetween:(int)startIndex end:(int)endIndex channel:(int)channel
{
    const float* samples = _signal.getSamples (channel);
    const int frameStride = _signal.getFrameStride();

    int start = plonk::min (startIndex, endIndex);
    int end   = plonk::max (startIndex, endIndex);
    start     = plonk::clip (start, 0, _signal.getNumFrames() - 1);
    end       = plonk::clip (end, 1, _signal.getNumFrames());
    
    samples += start * frameStride;
        
    PAERange range { *samples, *samples };
    
    for (int i = start + 1; i < end; ++i, samples += frameStride)
    {        
        range.minimum = plonk::min (range.minimum, *samples);
        range.maximum = plonk::max (range.maximum, *samples);
    }
    
    return range;
}

-(void)setSample:(float)value index:(int)index channel:(int)channel
{
    if (index < 0) return;
    if (index >= _signal.getNumFrames()) return;
    
    float* samples = _signal.getSamples (channel);
    const int frameStride = _signal.getFrameStride();
    *(samples + index * frameStride) = value;
}

@end

////////////////////////////////////////////////////////////////////////////////

@implementation PAEBuffer

+(PAEBuffer*)bufferNamed:(NSString*)name
{
    FilePath path (FilePath::system (FilePath::App).child (name.UTF8String).fullpath());
    return [[PAEBuffer alloc] initFromPath:[NSString stringWithUTF8String:path.fullpath()]];
}

-(id)initFromPath:(NSString*)path
{
    if (self = [super init])
    {
        self.internal = [[PAEBufferInternal alloc] initFromPath:path];
        self.localNumFrames = self.internal.numFrames;
        self.localChannel = -1;
    }
    
    return self;
}

+(PAEBuffer*)bufferWithSize:(int)numFrames channels:(int)numChannels sampleRate:(double)sampleRate
{
    return [[PAEBuffer alloc] initWithSize:numFrames
                                  channels:numChannels
                                sampleRate:sampleRate];
}

-(id)initWithSize:(int)numFrames channels:(int)numChannels sampleRate:(double)sampleRate
{
    if (self = [super init])
    {
        self.internal = [[PAEBufferInternal alloc] initWithSize:numFrames
                                                       channels:numChannels
                                                     sampleRate:sampleRate];
        self.localNumFrames = self.internal.numFrames;
        self.localChannel = -1;
    }
    
    return self;
}

-(Signal)signal
{
    return self.internal.signal;
}

-(int)numChannels
{
    return self.localChannel < 0 ? self.internal.numChannels : 1;
}

-(NSTimeInterval)duration
{
    return self.localNumFrames / self.sampleRate;
}

-(int)numFrames
{
    return self.localNumFrames;
}

-(double)sampleRate
{
    return self.internal.sampleRate;
}

-(int)frameStride
{
    return self.internal.frameStride;
}

-(float*)samples:(int)channel
{
    return [self.internal samples:self.localChannel < 0 ? channel : self.localChannel] + self.localStartOffset * self.frameStride;
}

-(float)sampleAtIndex:(int)index channel:(int)channel
{
    return [self.internal sampleAtIndex:index + self.localStartOffset
                                channel:channel];
}

-(float)sampleAtTime:(NSTimeInterval)time channel:(int)channel
{
    return [self.internal sampleAtTime:time + (self.localStartOffset / self.sampleRate)
                               channel:channel];
}

-(float)sampleAtPhase:(NSTimeInterval)phase channel:(int)channel
{
    return [self.internal sampleAtTime:phase * self.duration + (self.localStartOffset / self.sampleRate)
                               channel:channel];
}

-(float)sampleAtIndex:(int)index
{
    return [self sampleAtIndex:index
                       channel:self.localChannel < 0 ? 0 : self.localChannel];
}

-(float)sampleAtTime:(NSTimeInterval)time
{
    return [self sampleAtTime:time
                      channel:self.localChannel < 0 ? 0 : self.localChannel];
}

-(float)sampleAtPhase:(double)phase
{
    return [self sampleAtPhase:phase
                       channel:self.localChannel < 0 ? 0 : self.localChannel];
}

-(PAERange)limitsBetween:(int)startIndex end:(int)endIndex channel:(int)channel
{
    return [self.internal limitsBetween:startIndex + self.localStartOffset
                                    end:endIndex + self.localStartOffset
                                channel:self.localChannel < 0 ? channel : self.localChannel];
}

-(void)setSample:(float)value index:(int)index channel:(int)channel
{
    [self.internal setSample:value
                       index:index + self.localStartOffset
                     channel:self.localChannel < 0 ? channel : self.localChannel];
}

-(PAEBuffer*)bufferFrom:(int)startIndexOffset numFrames:(int)numFrames
{
    PAEBuffer* buffer = [PAEBuffer new];
    
    if (buffer)
    {
        buffer.internal         = self.internal;
        
        startIndexOffset        = plonk::clip (startIndexOffset, 0, self.localNumFrames);
        numFrames               = numFrames < 0 ? self.localNumFrames - startIndexOffset : numFrames;
        buffer.localStartOffset = plonk::clip (self.localStartOffset + startIndexOffset,
                                               self.localStartOffset,
                                               self.localStartOffset + self.localNumFrames);
        buffer.localNumFrames   = plonk::clip (numFrames, 0, self.localNumFrames - startIndexOffset);
        buffer.localChannel     = self.localChannel;
    }
    
    return buffer;
}

-(PAEBuffer*)bufferFromTime:(NSTimeInterval)startTimeOffset duration:(NSTimeInterval)duration
{
    return [self bufferFrom:int (startTimeOffset * self.sampleRate + 0.5)
                  numFrames:int (duration * self.sampleRate + 0.5)];
}

-(PAEBuffer*)bufferWithChannel:(int)channel
{
    PAEBuffer* buffer = [PAEBuffer new];

    if (buffer)
    {
        buffer.internal         = self.internal;
        buffer.localStartOffset = self.localStartOffset;
        buffer.localNumFrames   = self.localNumFrames;
        buffer.localChannel     = self.localChannel < 0 ? channel : self.localChannel;
    }
    
    return buffer;
}

@end






