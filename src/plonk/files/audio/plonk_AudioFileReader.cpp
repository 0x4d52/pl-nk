/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-12
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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

AudioFileReaderInternal::AudioFileReaderInternal() throw()
{
    pl_AudioFileReader_Init (getPeerRef());
}

AudioFileReaderInternal::AudioFileReaderInternal (Text const& path, const int bufferSize) throw()
:   readBuffer (Chars::withSize ((bufferSize > 0) ? bufferSize : 32768))
{
    plonk_assert (bufferSize > 0);
    
    pl_AudioFileReader_Init (getPeerRef());
    ResultCode result = pl_AudioFileReader_Open (getPeerRef(), path.getArray());
    plonk_assert (result == PlankResult_OK);
    
    numFramesPerBuffer = readBuffer.length() / getBytesPerFrame();

    
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

AudioFileReaderInternal::~AudioFileReaderInternal()
{
    pl_AudioFileReader_DeInit (getPeerRef());
}

int AudioFileReaderInternal::getBitsPerSample() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetBitsPerSample (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

int AudioFileReaderInternal::getBytesPerFrame() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetBytesPerFrame (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

int AudioFileReaderInternal::getNumChannels() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetNumChannels (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

double AudioFileReaderInternal::getSampleRate() const throw()
{
    double value;
    ResultCode result = pl_AudioFileReader_GetSampleRate (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

int AudioFileReaderInternal::getNumFrames() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetNumFrames (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

int AudioFileReaderInternal::getFramePosition() const throw()
{
    int value;
    ResultCode result = pl_AudioFileReader_GetFramePosition (getPeerRef(), &value);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
    return value;
}

void AudioFileReaderInternal::setFramePosition (const int position) throw()
{
    ResultCode result = pl_AudioFileReader_SetFramePosition (getPeerRef(), position);
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
}

void AudioFileReaderInternal::resetFramePosition() throw()
{
    ResultCode result = pl_AudioFileReader_ResetFramePosition (getPeerRef());
    plonk_assert (result == PlankResult_OK);
#ifndef PLONK_DEBUG
    (void)result;
#endif
}



END_PLONK_NAMESPACE
