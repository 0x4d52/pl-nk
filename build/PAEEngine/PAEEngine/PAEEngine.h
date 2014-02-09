//
//  PAEEngine.h
//  PAEEngine
//
//  Created by Martin Robinson on 03/02/2014.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>

// core
#import "PAEAudioHost.h"
#import "PAESource.h"
#import "PAEProcess.h"
#import "PAEChannelStrip.h"
#import "PAEMap.h"

// sources
#import "PAEConstant.h"
#import "PAEControl.h"
#import "PAETestTone.h"
#import "PAEOscillator.h"
#import "PAEAudioFilePlayer.h"
#import "PAEMixer.h"
#import "PAEFilter.h"
#import "PAEFollower.h"
#import "PAEGate.h"
#import "PAEPan.h"

/**
@mainpage Documentation
 
 PAEEngine is an Objective-C API for simple audio patching on iOS.
 
 By Martin Robinson.
 
 <p>
 This project is built on the cross-platform audio framwork pl-nk (Plonk|Plink|Plank). 
 http://code.google.com/p/pl-nk/
 
 To get started simply create a PAEAudioHost and patch a PAESource to its mainMix property.
 
 Complex graphs of processing may be created using chains of processing and mixed 
 using PAEMixer.
*/