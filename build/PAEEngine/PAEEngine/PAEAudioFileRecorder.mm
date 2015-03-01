//
//  PAEAudioFileRecorder.m
//  PAEEngine
//
//  Created by Martin Robinson on 24/02/2015.
//  Copyright (c) 2015 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"
#import "PAEBufferCaptureInternal.h"

@interface PAEAudioFileRecorderWAV ()
@end

@implementation PAEAudioFileRecorderWAV
{
    PlankAudioFileWriter _writer;
    void *_convertBuffer;
}

+(PAEAudioFileRecorderWAV*)audioFileRecorderWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path
{
    return [[PAEAudioFileRecorderWAV alloc] initWithNumChannels:numChannels
                                                        numBits:numBits
                                                     sampleRate:sampleRate
                                                         inPath:path
                                                     bufferSize:512
                                                     numBuffers:64];
}

+(PAEAudioFileRecorderWAV*)audioFileRecorderWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path
                                                    bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    return [[PAEAudioFileRecorderWAV alloc] initWithNumChannels:numChannels
                                                        numBits:numBits
                                                     sampleRate:sampleRate
                                                         inPath:path
                                                     bufferSize:bufferSize
                                                     numBuffers:numBuffers];
}

-(id)initWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path bufferSize:(int)bufferSize numBuffers:(int)numBuffers
{
    _convertBuffer = 0;
    
    if (!path)
    {
        BOOL success = NO;
        NSString* randomPath;
        Text hex ("0123456789abcdef");
        
        NSFileManager* manager = [NSFileManager defaultManager];
        
        while (!success)
        {
            Text name ("temp_");
            
            for (int i = 0; i < 32; ++i)
                name += hex[rand() % 16];
            
            name += ".wav";
            randomPath = [NSTemporaryDirectory() stringByAppendingString:[NSString stringWithUTF8String:name.getArray()]];
            success = ![manager fileExistsAtPath:randomPath];
        }
        
        path = randomPath;
    }
    
    if (self = [super initWithNumInputs:numChannels bufferSize:bufferSize numBuffers:numBuffers])
    {
        if (sampleRate <= 0.0)
        {
            NSLog(@"PAEAudioFileRecorderWAV sample rate is too low!");
            return nil;
        }
        
        if (numChannels < 1)
        {
            NSLog(@"PAEAudioFileRecorderWAV numChannels must be at least 1");
            return nil;
        }

        if (pl_AudioFileWriter_Init (&_writer) != PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorderWAV failed to initialise the audio file writer.");
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
            NSLog(@"PAEAudioFileRecorderWAV failed set the file format.");
            return nil;
        }
        
        if (pl_AudioFileWriter_Open (&_writer, [checkedPath UTF8String]) != PlankResult_OK)
        {
            NSLog(@"PAEAudioFileRecorderWAV failed to open the file for writing: %@", checkedPath);
            return nil;
        }
        
        
        const int bufferSize = BlockSize::getDefault().getValue();;
        self.buffer = [PAEBuffer bufferWithSize:bufferSize channels:numChannels sampleRate:sampleRate];
        _convertBuffer = pl_Memory_AllocateBytes (pl_MemoryGlobal(), pl_AudioFileWriter_GetFormatInfoReadOnly (&_writer)->bytesPerFrame * bufferSize);
        
        _path = checkedPath;

        self.captureDelegate = self;
        _paused = NO;

        NSLog(@"PAEAudioFileRecorderWAV recording to: %@", checkedPath);
    }
    
    return self;
}

-(void)stop
{
    self.captureDelegate = nil;
    pl_AudioFileWriter_DeInit (&_writer);
    pl_Memory_Free (pl_MemoryGlobal(), _convertBuffer);
}

-(void)dealloc
{
    [self stop];
}

-(BOOL)bufferReady:(PAEBufferCapture*)bufferCapture
{
    if (_paused || bufferCapture.captureDelegate != self)
        return NO; // exit as fast as possible, don't bother to keep the excess data either
    
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
    
    if (pl_AudioFileWriter_WriteFrames (&_writer, true, self.buffer.numFrames, _convertBuffer) != PlankResult_OK)
    {
        NSLog(@"PAEAudioFileRecorderWAV failed write frames.");
    }
    
    return YES; // don't discard other data waiting to be written
}

@end