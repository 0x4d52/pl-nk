//
//  PAEAudioFileRecorder.m
//  PAEEngine
//
//  Created by Martin Robinson on 24/02/2015.
//  Copyright (c) 2015 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"
#import "PAEBufferCaptureInternal.h"

////////////////////////////////////////////////////////////////////////////////

@interface PAEAudioFileRecorder ()
@end

@implementation PAEAudioFileRecorder
{
    PlankAudioFileWriter _writer;
    void *_convertBuffer;
}

+(PAEAudioFileRecorder*)audioFileRecorderWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path
{
    return [[PAEAudioFileRecorder alloc] initWAVWithNumChannels:numChannels
                                                        numBits:numBits
                                                     sampleRate:sampleRate
                                                         inPath:path
                                                     bufferSize:512
                                                     numBuffers:64];
}

+(PAEAudioFileRecorder*)audioFileRecorderWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path
                                                 bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    return [[PAEAudioFileRecorder alloc] initWAVWithNumChannels:numChannels
                                                        numBits:numBits
                                                     sampleRate:sampleRate
                                                         inPath:path
                                                     bufferSize:bufferSize
                                                     numBuffers:numBuffers];
}

+(PAEAudioFileRecorder*)audioFileRecorderOggVorbisWithNumChannels:(int)numChannels quality:(float)quality sampleRate:(double)sampleRate inPath:(NSString*)path
{
    return [[PAEAudioFileRecorder alloc] initOggVorbisWithNumChannels:numChannels
                                                              quality:quality
                                                           sampleRate:sampleRate
                                                               inPath:path
                                                           bufferSize:512
                                                           numBuffers:64];
}

+(PAEAudioFileRecorder*)audioFileRecorderOggVorbisWithNumChannels:(int)numChannels quality:(float)quality sampleRate:(double)sampleRate inPath:(NSString*)path
                                                       bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    return [[PAEAudioFileRecorder alloc] initOggVorbisWithNumChannels:numChannels
                                                              quality:quality
                                                           sampleRate:sampleRate
                                                               inPath:path
                                                           bufferSize:bufferSize
                                                           numBuffers:numBuffers];
}

-(id)initCommonWithNumChannels:(int)numChannels sampleRate:(double)sampleRate inPath:(NSString*)path
                    bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    if ([path hasPrefix:@"."])
    {
        Text ext ([path UTF8String]);
        
        BOOL success = NO;
        NSString* randomPath;
        Text hex ("0123456789abcdef");
        
        NSFileManager* manager = [NSFileManager defaultManager];
        
        while (!success)
        {
            Text name ("temp_");
            
            for (int i = 0; i < 32; ++i)
                name += hex[rand() % 16];
            
            name += ext;
            randomPath = [NSTemporaryDirectory() stringByAppendingString:[NSString stringWithUTF8String:name.getArray()]];
            success = ![manager fileExistsAtPath:randomPath];
        }
        
        path = randomPath;
    }
    
    if (self = [super initWithNumInputs:numChannels bufferSize:bufferSize numBuffers:numBuffers])
    {
        if (sampleRate <= 0.0)
        {
            NSLog(@"PAEAudioFileRecorder sample rate is too low!");
            return nil;
        }
        
        if (numChannels < 1)
        {
            NSLog(@"PAEAudioFileRecorder numChannels must be at least 1");
            return nil;
        }
        
        if (pl_AudioFileWriter_Init (&_writer) != PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorder failed to initialise the audio file writer.");
            return nil;
        }
        
        NSFileManager* manager = [NSFileManager defaultManager];
        NSString* checkedPath = path;
        
        int suffix = 0;
        while ([manager fileExistsAtPath:checkedPath])
        {
            ++suffix;
            NSString* parent = path.stringByDeletingLastPathComponent;
            NSString* filename = [NSString stringWithFormat:@"%@-%d", path.lastPathComponent.stringByDeletingPathExtension, suffix];
            NSString* ext = path.pathExtension;
            checkedPath = [parent stringByAppendingPathComponent:[filename stringByAppendingPathExtension:ext]];
        }
        
        _path = checkedPath;
    }
    
    return self;
}

-(id)initWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path
                 bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    if (self = [self initCommonWithNumChannels:numChannels
                                    sampleRate:sampleRate
                                        inPath:path ? path : @".wav"
                                    bufferSize:bufferSize
                                    numBuffers:numBuffers])
    {
        int bitsChecked;
        
        switch (numBits)
        {
            case 16: bitsChecked = 16; break;
            case 24: bitsChecked = 24; break;
            case 32: bitsChecked = 32; break;
            default: bitsChecked = 16;
        }
        
        if (pl_AudioFileWriter_SetFormatWAV (&_writer, bitsChecked, numChannels, sampleRate, false) != PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorder failed set the file format.");
            return nil;
        }
        
        if (pl_AudioFileWriter_Open (&_writer, [self.path UTF8String]) != PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorder failed to open the file for writing: %@", self.path);
            return nil;
        }
        
        self.buffer = [PAEBuffer bufferWithSize:bufferSize channels:numChannels sampleRate:sampleRate];
        _convertBuffer = pl_Memory_AllocateBytes (pl_MemoryGlobal(), pl_AudioFileWriter_GetFormatInfoReadOnly (&_writer)->bytesPerFrame * bufferSize);
        
        self.captureDelegate = self;
        _paused = NO;
        
        NSLog(@"PAEAudioFileRecorder recording to: %@", self.path);
    }

    return self;
}

-(id)initOggVorbisWithNumChannels:(int)numChannels quality:(float)quality sampleRate:(double)sampleRate inPath:(NSString*)path
                       bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    if (self = [self initCommonWithNumChannels:numChannels
                                    sampleRate:sampleRate
                                        inPath:path ? path : @".ogg"
                                    bufferSize:bufferSize
                                    numBuffers:numBuffers])
    {
        if (pl_AudioFileWriter_SetFormatOggVorbis (&_writer, quality, numChannels, sampleRate)!= PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorder failed set the file format.");
            return nil;
        }
        
        if (pl_AudioFileWriter_Open (&_writer, [self.path UTF8String]) != PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorder failed to open the file for writing: %@", self.path);
            return nil;
        }
        
        self.buffer = [PAEBuffer bufferWithSize:bufferSize channels:numChannels sampleRate:sampleRate];
        
        self.captureDelegate = self;
        _paused = NO;
        
        NSLog(@"PAEAudioFileRecorder recording to: %@", self.path);
    }
    
    return self;
}

-(void)stop
{
    self.captureDelegate = nil;
    pl_AudioFileWriter_DeInit (&_writer);
    
    if (_convertBuffer != PLANK_NULL)
    {
        pl_Memory_Free (pl_MemoryGlobal(), _convertBuffer);
        _convertBuffer = PLANK_NULL;
    }
}

-(BOOL)isStopped
{
    return _writer.peer != NULL;
}

-(void)dealloc
{
    [self stop];
}

-(BOOL)bufferReady:(PAEBufferCapture*)bufferCapture
{
    if (_paused || bufferCapture.captureDelegate != self)
        return NO; // exit as fast as possible, don't bother to keep the excess data either
    
    void *frames = PLANK_NULL;
    
    if (_convertBuffer != PLANK_NULL)
    {
        const PlankAudioFileFormatInfo* format = pl_AudioFileWriter_GetFormatInfoReadOnly (&_writer);
        const int numItems = self.buffer.numFrames * self.buffer.numChannels;
        
        switch (format->bitsPerSample)
        {
            case 16: {
                short* destData = static_cast<short*> (_convertBuffer);
                NumericalArrayConverter<short, float>::convertScaled (destData, self.buffer.rawSamples, numItems);
            } break;
            case 24: {
                Int24* destData = static_cast<Int24*> (_convertBuffer);
                NumericalArrayConverter<Int24, float>::convertScaled (destData, self.buffer.rawSamples, numItems);
            } break;
            case 32: {
                int* destData = static_cast<int*> (_convertBuffer);
                NumericalArrayConverter<int, float>::convertScaled (destData, self.buffer.rawSamples, numItems);
            } break;
            default: plonk_assertfalse;
        }
        
        frames = _convertBuffer;
    }
    else
    {
        frames = self.buffer.rawSamples;
    }
    
    if (pl_AudioFileWriter_WriteFrames (&_writer, true, self.buffer.numFrames, frames) != PlankResult_OK)
    {
        NSLog(@"PAEAudioFileRecorder failed write frames.");
    }
    
    return YES; // don't discard other data waiting to be written
}

@end



