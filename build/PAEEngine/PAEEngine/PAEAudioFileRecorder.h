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
#import "PAEProcess.h"

/** @file */

////////////////////////////////////////////////////////////////////////////////

/** A WAV audio file recorder.
 This records audio into a new WAV file until stopped or deallocated. */
@interface PAEAudioFileRecorder : PAEBufferCapture <PAEBufferCaptureDelegate>

/** The path of the file. 
 Especially useful if nil is used as the inPath: field to the creation methods
 where a randomly generated temporary path will be used. */
@property (strong, nonatomic, readonly) NSString* path;

/** Pauses or unpauses the recording process. */
@property (nonatomic) BOOL paused;

/** Create an audio file recorder with default buffer sizes. 
 Here the number of channels and the bit depth (16, 24 or 32) may be selected.
 The sample rate must match the sample rate of the audio host.
 The path may be nil (where a randomly generated path in the temporary directory
 will be used). Otherwise the path must be in a location that the app has 
 permission to write files. */
+(PAEAudioFileRecorder*)audioFileRecorderWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path;

/** Create an Ogg Vorbis recorder with default buffer sizes.
 Here the number of channels and the quality (0.0-10.0) may be selected.
 The sample rate must match the sample rate of the audio host.
 The path may be nil (where a randomly generated path in the temporary directory
 will be used). Otherwise the path must be in a location that the app has
 permission to write files. */
+(PAEAudioFileRecorder*)audioFileRecorderOggVorbisWithNumChannels:(int)numChannels quality:(float)quality sampleRate:(double)sampleRate inPath:(NSString*)path;

/** Stops recording.
 After calling this the recording cannot be restarted. */
-(void)stop;

/** Determines if the recording is stopped. */
-(BOOL)isStopped;
@end

////////////////////////////////////////////////////////////////////////////////

@interface PAEAudioFileRecorder (Advanced)

/** Create an audio file recorder with specifc buffer sizes.
 Here the number of channels and the bit depth (16, 24 or 32) may be selected.
 The sample rate must match the sample rate of the audio host.
 The path may be nil (where a randomly generated path in the temporary directory
 will be used). Otherwise the path must be in a location that the app has
 permission to write files. The bufferSize governs the number of sample
 frames buffered for each write operation. The numBuffers argument is how many
 of these buffers are used. */
+(PAEAudioFileRecorder*)audioFileRecorderWAVWithNumChannels:(int)numChannels numBits:(int)numBits sampleRate:(double)sampleRate inPath:(NSString*)path bufferSize:(int)bufferSize numBuffers:(int)numBuffers;

/** Create an Ogg Vorbis recorder with specifc buffer sizes.
 Here the number of channels and the quality (0.0-10.0) may be selected.
 The sample rate must match the sample rate of the audio host.
 The path may be nil (where a randomly generated path in the temporary directory
 will be used). Otherwise the path must be in a location that the app has
 permission to write files. The bufferSize governs the number of sample
 frames buffered for each write operation. The numBuffers argument is how many
 of these buffers are used. */
+(PAEAudioFileRecorder*)audioFileRecorderOggVorbisWithNumChannels:(int)numChannels quality:(float)quality sampleRate:(double)sampleRate inPath:(NSString*)path bufferSize:(int)bufferSize numBuffers:(int)numBuffers;

@end
