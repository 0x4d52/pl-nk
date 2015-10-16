#ifndef JUCE_PLNK_JUCE_H_INCLUDED
#define JUCE_PLNK_JUCE_H_INCLUDED

#if JUCE_USE_OGGVORBIS 
#error You must disable JUCE_USE_OGGVORBIS in the Introjucer within the juce_audio_formats module
#endif

#if ! PLONK_AUDIOHOST_JUCE
#error You must add a preprocessor definition to the Introjucer project PLONK_AUDIOHOST_JUCE=1 to use the JUCE audio host
#endif

#include "plnk.h"

#endif // JUCE_PLNK_JUCE_H_INCLUDED
