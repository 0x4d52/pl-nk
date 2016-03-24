#ifndef JUCE_PLNK_JUCE_H_INCLUDED
#define JUCE_PLNK_JUCE_H_INCLUDED


/** Config: PLONK_WARN_ABOUT_MISSING_PLONK_AUDIOHOST_JUCE_DEFINE
 This is used to diable the compiler warning if you don't need the JUCE audio host.
 Otherwise you should leave this enabled and define PLONK_AUDIOHOST_JUCE=1 in the preprocessor definitions.
 */
#ifndef PLONK_WARN_ABOUT_MISSING_PLONK_AUDIOHOST_JUCE_DEFINE
#define PLONK_WARN_ABOUT_MISSING_PLONK_AUDIOHOST_JUCE_DEFINE 1
#endif


#if JUCE_USE_OGGVORBIS && PLANK_OGGVORBIS
#error You must disable JUCE_USE_OGGVORBIS in the Introjucer within the juce_audio_formats module if using PLANK_OGGVORBIS
#endif

#if PLONK_WARN_ABOUT_MISSING_PLONK_AUDIOHOST_JUCE_DEFINE && ! PLONK_AUDIOHOST_JUCE
#warning You must add a preprocessor definition to the Introjucer project PLONK_AUDIOHOST_JUCE=1 to use the JUCE audio host
#endif


#include "plnk.h"

#endif // JUCE_PLNK_JUCE_H_INCLUDED
