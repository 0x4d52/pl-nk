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

#ifndef PLANK_FFT_H
#define PLANK_FFT_H

#define PLANKFFTF_DEFAULTLENGTH 4096

PLANK_BEGIN_C_LINKAGE

/** A simple, cross-platform FFT processor for audio signals.
 This must be allocated/deallocated using pl_FFTF_Create() and pl_FFTF_Destroy(),
 a statically alloacted version is not currently available.
 
 This makes use of other underlying libraries on request. By default it uses
 FFTReal (via the Plank FFTRealInternal class). To use vDSP on Mac OS X or iOS 
 define the preprocessor macro PLANK_FFT_VDSP.
 
 @code
 PlankFFTFRef fft;
 float input[128];
 float output[128];
 int i;
 fft = pl_FFTF_Create (128); // create with FFT size 128
 
 // fill the buffer
 for (i = 0; i < 128; i++)
 {
    input[i] = i / 127.f;
 }
 
 pl_FFTF_Forward (fft, output, input); // perform the forward FFT, the result will be in output
 pl_FFTF_Destroy (fft); // destroy the FFT object
 @endcode
 
 The layout of the resulting FFT bins is in a "packed" format which omits the imaginary
 components of the DC and Nyquist bins (as these are always zero anyway). This means that the
 size of the data input and output fits in the same memory size.
 
 For the FFT size of 128 illustrated by the code snippet:
 
 - Index 0 - Bin 0 Real (DC)
 - Index 1 - Bin 1 Real
 - Index 2 - Bin 2 Real
 - ...
 - Index 62 - Bin 62 Real
 - Index 63 - Bin 63 Real
 - Index 64 - Bin 64 Real (Nyquist)
 - Index 65 - Bin 1 Imag
 - Index 66 - Bin 2 Imag
 - Index 67 - Bin 3 Imag
 - ...
 - Index 125 - Bin 61 Imag
 - Index 126 - Bin 62 Imag
 - Index 127 - Bin 63 Imag
 
 @defgroup PlankFFTFClass Plank FFTF class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank FFTF</i> object. */
typedef struct PlankFFTF* PlankFFTFRef; 

/** Create and initialise a <i>Plank FFTF</i> object and return an oqaque reference to it. 
 @return A <i>Plank FFTF</i> object as an opaque reference. */
PlankFFTFRef pl_FFTF_CreateAndInit();

/** Create a <i>Plank FFTF</i> object and return an oqaque reference to it. 
 @return A <i>Plank FFTF</i> object as an opaque reference. */
PlankFFTFRef pl_FFTF_Create();

/** Initialise a <i>Plank FFTF</i> object with a default length (PLANKFFTF_DEFAULTLENGTH). 
 @param p The <i>Plank FFTF</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_FFTF_Init (PlankFFTFRef p);

/** Initialise a <i>Plank FFTF</i> object. 
 @param p The <i>Plank FFTF</i> object. 
 @param length  The FFT size - this must be a power of 2 or less than 16 (where it will
                specify the log2 FFT size e.g., length 8 = pow(2,8) = 256
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_FFTF_InitWithLength (PlankFFTFRef p, const PlankL length);

/** Deinitialise a <i>Plank FFTF</i> object. 
 @param p The <i>Plank FFTF</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_FFTF_DeInit (PlankFFTFRef p);

/** Destroy a <i>Plank FFTF</i> object. 
 @param p The <i>Plank FFTF</i> object. 
 @return A result code which will be PlankResult_OK if the operation was completely successful. */
PlankResult pl_FFTF_Destroy (PlankFFTFRef p);

/** Apply the FFT to the input and place the result in output.
 This may be performed in-place (i.e., input and output can point to the same data). 
 @param p The <i>Plank FFTF</i> object. 
 @param output A pointer to an array of floats to store the result. 
 @param input A pointer to an array of floats holding the input data. */
void pl_FFTF_Forward (PlankFFTFRef p, float* output, const float* input);

/** Apply the inverse-FFT to the input and place the result in output.
 This may be performed in-place (i.e., input and output can point to the same data). 
 @param p The <i>Plank FFTF</i> object. 
 @param output A pointer to an array of floats to store the result. 
 @param input A pointer to an array of floats holding the input data. */
void pl_FFTF_Inverse (PlankFFTFRef p, float* output, const float* input);

/** Get the FFT size. 
 @param p The <i>Plank FFTF</i> object. 
 @return The FFT size. */
PlankL pl_FFTF_Length (PlankFFTFRef p);

/** Get half FFT size. 
 This is just as a convenience as it is already cached for efficiency. 
 @param p The <i>Plank FFTF</i> object. 
 @return The half FFT size. */
PlankL pl_FFTF_HalfLength (PlankFFTFRef p);

/** Get a pointer to the internal temporary buffer.
 This is an array of floats the size of the FFT. This could 
 be useful as a scratch space to save allocating more memory. Of
 course its contents may be invalidated by any other Plank FFTF calls. 
 @param p The <i>Plank FFTF</i> object. 
 @return A pointer to the temporary float buffer. */
float* pl_FFTF_Temp (PlankFFTFRef p);

/// @} // End group PlankFFTFClass

PLANK_END_C_LINKAGE

#endif // PLANK_FFT_H
