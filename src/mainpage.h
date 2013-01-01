/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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
 
 This software makes use of third party libraries. For more information see:
 doc/license.txt included in the distribution.
 -------------------------------------------------------------------------------
 */

/** 
 @mainpage Plonk|Plink|Plank Documentation
 
 @section Introduction Introduction
 Plonk|Plink|Plank implements a set of resuable functions and DSP for audio software development.
 
 @section Installation Installation
 Currently is currently an example project for iOS and an example project for Mac OS X using PortAudio.
 
 <em>Plonk|Plink|Plank</em> are easy to include in a project and are not tied to a particular
 host environment. Just add all the files in the @c /src directory (recursively) to a project. 
 To use Plonk just include @c plonk/plonk.h (this automatically includes @c plank/plank.h 
 as Plonk is dependent on Plank).
 
 @section GettingStarted Getting started
 There are a few compile options that can be configured using preprocessor macros. First, you 
 should ensure that DEBUG=1 is defined for a <em>Debug</em> build and NDEBUG=1 is defined for a 
 <em>Release</em> build.
 
 <strong>Plank</strong> (these options also affect the behaviour of Plink and Plonk)
 - PLANK_VEC_VDSP=1 : Use Apple's Accelerate vDSP library for vector processing where possible 
                      (the default is to use scalar code). You must also link to the 
                      Accelerate framework.
 - PLANK_FFT_VDSP=1 : Use Apple's Accelerate vDSP library for FFT processing 
                      (the default is to use FFTReal which is included in the source tree).
                      You must also link to the Accelerate framework.
 - PLANK_OGGVORBIS=1 : Enable Ogg Vorbis support. You must include the files in ext/vorbis in your project too.
 
 <strong>Plonk</strong>
 - PLONK_USEPLINK=1 : Use the Plink library where possible. This effectively enables optimised versions of
                      some processing units since Plink may be leveraging vectorised code directly (rather
                      than relying of compiler optimisations via the template code in Plonk).
 - PLONK_AUDIOHOST_PORTAUDIO=1 : Use the PortAudio audio host. You must also link to the PortAudio library.
 - PLONK_AUDIOHOST_IOS=1 : Use the Apple iOS audio host. You must also link to the CoreAudio, AudioUnit and 
                           AudioToolbox frameworks.
 - PLONK_AUDIOHOST_JUCE=1 : Use the Juce audio host. You must also link to Juce (either the static library
                            or otherwise using the Introjucer to generate the project).
 
 <strong>Plink</strong>
 <em>None</em>

 You should read the section @link PlonkCoreConcepts core concepts in Plonk @endlink then use the macplnk 
 (requires Xcode 3.2.6 or later) or iosplnk (requires Xcode 4.3.3 or later) projects as a starting point. 
 (Windows project - winplnk - coming soon.)
 
 */

 /*
 The following assume the use of standard 32-bit floating point processing.
  
 The following class should act as pseudocode for the required operations needed to get
 basic synthesis working.
 
 @code
 #include "plonk/plonk.h"
 
 class TestPlonk
 {
 public:
    TestPlonk()
    {
    }
 
    // set up sample rate and block size and construct process graph
    void init (const double sampleRate, const int blockSize)
    {
        SampleRate::getDefault().setValue (sampleRate);
        BlockSize::getDefault().setValue (blockSize);
 
        graph = constructGraph();
    }
 
    // construct the graph of processing objects
    Unit constructGraph()
    {
        // just a 1kHz test tone at amplitude 0.25 (-12dB)
        return Sine::ar (1000, 0.25);
    }
 
    // assumes outputs is an array of pointers to output channel buffers
    void render (float** outputs, int numOutputs, int blockSize)
    {
        // update block size just in case it changed
        BlockSize::getDefault().setValue (blockSize);
 
        // process the graph 
        graph.process (info);
        
        // pull the data out of the root of the graph 
        for (i = 0; i < numOutputs; ++i)
        {
            float* const output = outputs[i];
            const float* const graphOutput = graph.getOutputSamples (i);
            Float1D::copyData (output, graphOutput, blockSize);            
        }
    }
 
 private:
    Unit graph;
    ProcessInfo info;
 };
 @endcode
 
 The following assumes you then have an instance of this @c TestPlonk class accessible
 to your code i.e., 
 
 @code
 ...
 TestPlonk testPlonk;
 ...
 @endcode
 
 Once you know the sample rate and processing block size for the system you're using you
 need to set these as the defaults in Plonk. E.g., if the sample rate is 44.1kHz using
 a block size of 512 samples you would need to use something like the following code
 before audio rendering starts (this is commonly available before starting an audio
 device driver running).
 
 @code
 ...
 testPlonk.init (44100.0, 512);
 ...
 @endcode
 
 Then you'd need to call the @c render() function for each block of audio requested
 by the audio device.
 
 @code
 ...
 testPlonk.render (outputs, numOutputs, blockSize);
 ...
 @endcode
 
 This assumes @c outputs is an array of pointers to floating point arrays of samples
 that are all @c blockSize in length. There should be @c numOutputs pointers in the
 @c outputs array.

*/


/*
 cd libogg-1.3.0 
 
 ./configure 
 make clean
 make
 sudo make install
 
 cp /usr/local/lib/libogg.a libogg-x86_64.a
 cp /usr/local/lib/libogg.0.dylib libogg-x86_64.dylib 
 
 make clean
 make CC="gcc -m32"
 sudo make install
 
 cp /usr/local/lib/libogg.a libogg-i386.a
 cp /usr/local/lib/libogg.0.dylib libogg-i386.dylib 
 
 sudo rm /usr/local/lib/libogg.a
 sudo rm /usr/local/lib/libogg.0.dylib
 sudo lipo -create libogg-i386.a libogg-x86_64.a -output /usr/local/lib/libogg.a
 sudo lipo -create libogg-i386.dylib libogg-x86_64.dylib -output /usr/local/lib/libogg.0.dylib
 */

/*
 cd libvorbis-1.3.3 
 ./configure
 make clean
 make CC="clang -arch x86_64"
 sudo make install
 
 cp /usr/local/lib/libvorbisenc.2.dylib libvorbisenc-x86_64.dylib 
 cp /usr/local/lib/libvorbisfile.3.dylib libvorbisfile-x86_64.dylib 
 cp /usr/local/lib/libvorbis.0.dylib libvorbis-x86_64.dylib 
 
 make clean
 make CC="clang -arch i386"
 sudo make install
 
 cp /usr/local/lib/libvorbisenc.2.dylib libvorbisenc-i386.dylib 
 cp /usr/local/lib/libvorbisfile.3.dylib libvorbisfile-i386.dylib 
 cp /usr/local/lib/libvorbis.0.dylib libvorbis-i386.dylib 
 
 sudo rm /usr/local/lib/libvorbisenc.2.dylib
 sudo rm /usr/local/lib/libvorbisfile.3.dylib
 sudo rm /usr/local/lib/libvorbis.0.dylib
 
 sudo lipo -create libvorbisenc-i386.dylib libvorbisenc-x86_64.dylib -output /usr/local/lib/libvorbisenc.2.dylib
 sudo lipo -create libvorbisfile-i386.dylib libvorbisfile-x86_64.dylib -output /usr/local/lib/libvorbisfile.3.dylib
 sudo lipo -create libvorbis-i386.dylib libvorbis-x86_64.dylib -output /usr/local/lib/libvorbis.0.dylib
*/