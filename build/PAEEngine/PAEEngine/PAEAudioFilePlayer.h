//
//  PAEAudioFilePlayer.h
//  PAEEngine
//
//  Created by Martin Robinson on 05/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEFoward.h"
#import "PAESource.h"

/** @file */

/** An audio file player. 
 This supports the following formats:
 - WAVE
 - AIFF (and AIFC)
 - WAV64
 - CAF (PCM data only)
 - Ogg Vorbis
 - Opus 
 
 It does NOT support MP3 or AAC.  Files in the user's music library will need to
 be converted to one of these formats and copied to the application sandbox. */
@interface PAEAudioFilePlayer : PAESource

/** The playback rate.
 The default is a constant of 1.
 This may be repatched with any other PAESource object as desired. 
 Drastic modulation is not recommended. */
@property (strong, nonatomic, readonly) PAEProcess* rate;

/** Whether the file will loop playback. 
 The default is YES. */
@property (nonatomic) BOOL loop;

/** The file's current playback position in seconds. */
@property (nonatomic) NSTimeInterval position;

/** The file's duration in seconds. */
@property (nonatomic, readonly) NSTimeInterval duration;

/** The file's name. */
@property (nonatomic, readonly) NSString* name;

/** Create an audio file player from a named file in the applicaiton bundle.
 These are files added to the project via Xcode (similar to the way image
 files may be obtained using [UIImage imageNamed:]). */
+(PAEAudioFilePlayer*)audioFilePlayerNamed:(NSString*)name;

///** Create an audio file player a file path. */
//+(PAEAudioFilePlayer*)audioFilePlayerFromPath:(NSString*)path;
@end
