/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 The Objective-C PAEEngine
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of University of the West of England, Bristol nor
 the names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF THE WEST OF ENGLAND, BRISTOL BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 This software makes use of third party libraries. See PAEEngine.h
 
 -------------------------------------------------------------------------------
 */

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

/** The duration of the audio file in frames. */
@property (nonatomic, readonly) PAEInt64 numFrames;

/** The sample rate of the audio file. */
@property (nonatomic, readonly) double sampleRate;

/** The file's name. */
@property (nonatomic, readonly) NSString* name;

/** Create an audio file player from a named file in the applicaiton bundle.
 These are files added to the project via Xcode (similar to the way image
 files may be obtained using [UIImage imageNamed:]). */
+(PAEAudioFilePlayer*)audioFilePlayerNamed:(NSString*)name;

/** Create an audio file player from a file a particular path. */
+(PAEAudioFilePlayer*)audioFilePlayerFromPath:(NSString*)path;
@end
