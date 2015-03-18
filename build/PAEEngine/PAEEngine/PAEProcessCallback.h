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

/** The type of the function callback.
 This mean you need to write a function like this:
 @code
 void myCallbackFunction (PAEProcessCallbackData* data)
 {
 //... process the data in here...
 }
 @endcode
 
 You pass the function name when the PAEProcessCallback object is created. This
 function will be called each time the PAEEngine needs a buffer of samples
 to be processed. */
typedef void (*PAEProcessCallbackFunction)(struct PAEProcessCallbackData*);

/** A structure that contains data passed to your PAEProcessCallbackFunction function.
 You would normally fill the buffers in the outputData array with your process's output.
 To do this you would use the inputData buffers (if you expect them) and your own state
 (that you provied access to via the userData field).
 
 You MAY change the userData field to point within your callback to a different 
 location if you wish although you wouldn't normally need to do this and is an
 advanced technique.
 
 You MAY change the callback field within your callback function so that a different
 callback is used on the next callback. This is also an advanced technique.
 
 All other fields MUST be regarded as read-only.
 */
typedef struct PAEProcessCallbackData
{
    PAEProcessCallbackFunction callback;    ///< The callback function.
    double sampleRate;                      ///< The sample rate of the process (readonly).
    int numOutputs;                         ///< The number of output channels and the size of the outputData array (readonly).
    float** outputData;                     ///< An array of pointers to the output data buffers.
    int numInputs;                          ///< The number of input channels and the size of the inputData array (readonly).
    const float** inputData;                ///< An array of pointers to the input data buffers (readonly).
    int numFrames;                          ///< The number of samples in each buffer in the inputData and outputData buffers (readonly).
    void* userData;                         ///< This will be the pointer you passed when creating the PAEProcessCallback.
} PAEProcessCallbackData;

/** A process with a user defined callback for processing.
 This allows you to write your own low level plugins for the PAEEngine. */
@interface PAEProcessCallback : PAEProcess

/** Create custom process callback.
 You must specify the number of inputs and outputs your callback will process and 
 provide your callback function. The userData of the PAEProcessCallbackData structure 
 will be zero in your callback. */
+(PAEProcessCallback*)processCallbackWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs function:(PAEProcessCallbackFunction)function;

/** Create custom process callback.
 You must specify the number of inputs and outputs your callback will process and
 provide your callback function. Provide a pointer to some useful data in the userData: 
 argument so you can keep track of the state of your prcoessing in your callback. */
+(PAEProcessCallback*)processCallbackWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs function:(PAEProcessCallbackFunction)function userData:(void*)userData;

/** Initialise a custom process callback.
 You must specify the number of inputs and outputs your callback will process and
 provide your callback function. Provide a pointer to some useful data in the userData:
 argument so you can keep track of the state of your prcoessing in your callback or
 pass NULL if you don't need to maintain any state information.
 
 This is useful if you create a subclass of PAEProcessCallback; for example you might 
 then write your own init method something like this:
 @code
 -(id)init
 {
    if (self = [super initWithNumOutputs:1 andNumInputs:1 function:myFunction userData:NULL])
    {
        //...etc
    }
    return self;
 }
 @endcode */
-(id)initWithNumOutputs:(int)numOutputs andNumInputs:(int)numInputs function:(PAEProcessCallbackFunction)function userData:(void*)userData;

@end

