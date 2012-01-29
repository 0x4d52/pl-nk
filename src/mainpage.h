/** 
 @mainpage Plonk|Plink|Plank Documentation
 
 @section Introduction Introduction
 Plonk|Plink|Plank implements a set of resuable functions and DSP for audio software development.
 
 @section Installation Installation
 Currently there are no example projects, these are comming soon.
 
 In the meantime <em>Plonk|Plink|Plank</em> are easy to include in a project. Just add all the files
 in the @c /src directory (recursively) to a project. To use Plonk just include
 @c plonk/plonk.h (this automatically includes @c plank/plank.h as Plonk is dependent
 on Plank).
 
 @section GettingStarted Getting started
 <em>Plonk|Plink|Plank</em> are designed to be used with a wide range of audio I/O systems. It
 has been tested primarily with 32-bit floating point audio I/O although should work at double precision
 and even integer-based formats too. The following assume the use of standard 32-bit floating point 
 processing.
 

 The following class should act as pseudocode for the required operations need to get
 basic synthesis working.
 
 @code
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