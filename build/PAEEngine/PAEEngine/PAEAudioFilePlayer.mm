//
//  PAEAudioFilePlayer.m
//  PAEEngine
//
//  Created by Martin Robinson on 05/02/2014.
//  Copyright (c) 2016 UWE. All rights reserved.
//

#import "PAEEngineInternal.h"

@interface PAEAudioFilePlayer ()
@property (strong, nonatomic) PAEProcess* rate;
@end

@implementation PAEAudioFilePlayer
{
    AudioFileReader _reader;
    IntVariable _loopCount;
}

-(NSArray*)controls
{
    return @[self.rate];
}

+(PAEAudioFilePlayer*)audioFilePlayerNamed:(NSString*)name
{
    FilePath path (FilePath::system (FilePath::App).child (name.UTF8String).fullpath());
    return [[PAEAudioFilePlayer alloc] initFromPath:[NSString stringWithUTF8String:path.fullpath()]];
}

+(PAEAudioFilePlayer*)audioFilePlayerFromPath:(NSString*)path
{
    return [[PAEAudioFilePlayer alloc] initFromPath:path];
}

-(id)initFromPath:(NSString*)path
{
    if (self = [super init])
    {
        NSFileManager* manager = [NSFileManager defaultManager];
        
        if (![manager fileExistsAtPath:path])
        {
            NSLog(@"PAEAudioFilePlayer path:%@ not found", path);
            return nil;
        }
        
        _reader = AudioFileReader ([path UTF8String]);
        _loopCount.setValue (0);
        
        if (!_reader.isReady())
        {
            NSLog(@"PAEAudioFilePlayer path:%@ not valid audio file", path);
            return nil;
        }
        
        self.rate       = [[PAEProcess alloc] initWithNumInputs:1];
        self.rate.input = [PAEConstant constantWithValue:1];
//        self.outputUnit = FilePlay::Simple::ar (_reader, self.rate.patchUnit, _loopCount);
        self.outputUnit = ResampleLinear::ar (FilePlay::ar (_reader, _loopCount), self.rate.patchUnit);
    }
    
    return self;
}

-(BOOL)loop
{
    return _loopCount.getValue() == 0;
}

-(void)setLoop:(BOOL)loop
{
    _loopCount.setValue (loop ? 0 : 1);
}

-(NSTimeInterval)position
{
    return _reader.getTime();
}

-(void)setPosition:(NSTimeInterval)position
{
    _reader.setTimeOnNextRead (plonk::clip (position, 0.0, self.duration));
}

-(NSTimeInterval)duration
{
    return _reader.getDuration();
}

-(PAEInt64)numFrames
{
    return _reader.getNumFrames();
}

-(double)sampleRate
{
    return _reader.getSampleRate();
}

-(NSString*)name
{
    return [NSString stringWithUTF8String:_reader.getName()];
}


@end
