#ifndef JUCE_PLNK_JUCE_H_INCLUDED
#define JUCE_PLNK_JUCE_H_INCLUDED

#if JUCE_USE_OGGVORBIS 
#error You must disable JUCE_USE_OGGVORBIS in the Introjucer within the juce_audio_formats module
#endif

#if ! PLONK_AUDIOHOST_JUCE
#error You must add a preprocessor definition to the Introjucer project PLONK_AUDIOHOST_JUCE=1 to use the JUCE audio host
#endif

///** Config: PLANK_FFT_VDSP
// Use vDSP in the Accerlerate framework on Mac OS X for the FFT processing.
// */
//#ifndef PLANK_FFT_VDSP
//#define PLANK_FFT_VDSP 0
//#endif
//
///** Config: PLANK_FFT_VDSP_FLIPIMAG
// Flip thr polarity of the phase when using vDSP (this is to match the results of FFTReal).
// */
//#ifndef PLANK_FFT_VDSP_FLIPIMAG
//#define PLANK_FFT_VDSP_FLIPIMAG 0
//#endif
//
//
///** Config: PLANK_VEC_VDSP
// Use vDSP in the Accerlerate framework on Mac OS X to speed up vector maths operations.
// */
//#ifndef PLANK_VEC_VDSP
//#define PLANK_VEC_VDSP 0
//#endif


#include "plnk.h"

#endif // JUCE_PLNK_JUCE_H_INCLUDED
