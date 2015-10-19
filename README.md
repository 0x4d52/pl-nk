Plonk|Plink|Plank are a set of cross-platform C/C++ frameworks for audio software development.

Plonk is the main C++ framework comprising building blocks for audio processing (e.g., oscillators, filters) and ways to interconnect these processes into more complex graphs. Plonk is multi-platform, currently running on Mac OS X, iOS and Windows and the intention is to support other platforms in the future. However, this project is divided into three subprojects that may be used independently.

### Plank ###
A C API to underlying supporting libraries (e.g., for file access, threads, atomic operations, lock-free data structures, FFT processing, SIMD libraries and so on). This can be used as a standalone framework, or with Plink and/or Plonk.

### Plink ###
A C API for audio processing unit generators for 32-bit floating point audio (although this may be expanded to add 64-bit floating point support in the future). This is dependent on Plank. This will be a lightweight API collecting together useful DSP processes. Currently only a small number of processes are implemented but this can grow particularly as optimisations are sought for Plonk. Plink leverages Plank to optimise certain operations using SIMD support if available (and enables) for the runtime platform and architecture.

### Plonk ###
A C++ API for writing object oriented audio applications. This includes audio processing graph management and so on. Dependent on Plank (and provides object oriented interfaces to this underlying C API). May optionally use Plink (to use the SIMD optimised version of floating point processing code for example).

Doxygen-generated API documentation available [here](http://0x4d52.github.io/pl-nk/doc/html/).

Since pl-nk may be used as a JUCE (http://juce.com) module, the easiest way to get started with pl-nk is to use it with JUCE and add the `plnk` directory as a JUCE module. The only caveats are that: 

1.  You must disable `JUCE_USE_OGGVORBIS` in the Introjucer within the `juce_audio_formats` module (since Plank already includes Ogg Vorbis support).
2.  You must add a preprocessor definition to the Introjucer project `PLONK_AUDIOHOST_JUCE=1` to use the JUCE audio host.

You can, of course, also us pl-nk independently of JUCE, which might be more appropriate for you licensing needs.

There are two example projects: macplnk (an Xcode project for Mac OS X using PortAudio as the audio host; and, *iosplnk* (an Xcode project for iOS devices). A Windows project will be added at some point but it should be trivial to convert the *macplnk* project for this purpose as the main task is to subclass `PortAudioAudioHost`, implement the `constructGraph()` function, and create and instance of this class. Then call its `startHost()` function to start audio running.

Use github to raise issues.

Twitter [@plnkaudio](http://twitter.com/plnkaudio) **Plonk|Plink|Plank**.

