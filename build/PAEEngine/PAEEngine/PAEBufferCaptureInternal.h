//
//  PAEBufferCaptureInternal.h
//  PAEEngine
//
//  Created by Martin Robinson on 28/02/2015.
//  Copyright (c) 2014 UWE. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PAEBufferCapture.h"

@interface PAEBufferCapture (Private)
-(id)initWithNumInputs:(int)numInputs bufferSize:(int)bufferSize numBuffers:(int)numBuffers;
@end
