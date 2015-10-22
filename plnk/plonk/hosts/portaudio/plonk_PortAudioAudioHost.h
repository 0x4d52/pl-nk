/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
 by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLONK_PORTAUDIOAUDIOHOST_H
#define PLONK_PORTAUDIOAUDIOHOST_H

#include "../../../ext/portaudio/portaudio/portaudio.h"

BEGIN_PLONK_NAMESPACE

template<class SampleType>
class PortAudioAudioHostBase : public AudioHostBase<SampleType>
{
public:
    typedef NumericalArray<SampleType*>            BufferArray;
    typedef NumericalArray<const SampleType*>      ConstBufferArray;

    /** Default constructor. */
    PortAudioAudioHostBase() throw();
    ~PortAudioAudioHostBase();
    
    Text getHostName() const throw();
    Text getNativeHostName() const throw();
    Text getInputName() const throw();
    Text getOutputName() const throw();
    double getCpuUsage() const throw();

    void startHost() throw();
    void stopHost() throw();
    
    int callback (const SampleType** inputData, SampleType** outputData,
                  unsigned long frameCount,
                  const PaStreamCallbackTimeInfo* timeInfo,
                  PaStreamCallbackFlags statusFlags) throw();
                
private:
    PaStream* stream;    
};

class PortAudioAudioHost : public PortAudioAudioHostBase<float>
{
public:
    PortAudioAudioHost() throw();
    ~PortAudioAudioHost();
};

#define PLANK_INLINING_FUNCTIONS 1
#include "plonk_PortAudioAudioHostInline.h"
#undef PLANK_INLINING_FUNCTIONS

END_PLONK_NAMESPACE

#endif  // PLONK_PORTAUDIOAUDIOHOST_H
