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

#ifndef PLANK_AUDIOFILECOMMON_H
#define PLANK_AUDIOFILECOMMON_H


#define PLANKAUDIOFILE_FORMAT_INVALID                -1
#define PLANKAUDIOFILE_FORMAT_UNKNOWN                 0
#define PLANKAUDIOFILE_FORMAT_WAV                     1
#define PLANKAUDIOFILE_FORMAT_AIFF                    2
#define PLANKAUDIOFILE_FORMAT_AIFC                    3
#define PLANKAUDIOFILE_FORMAT_OGGVORBIS               4

#define PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG        1
#define PLANKAUDIOFILE_ENCODING_PCM_FLAG              2
#define PLANKAUDIOFILE_ENCODING_FLOAT_FLAG            4

#define PLANKAUDIOFILE_ENCODING_INVALID              -1
#define PLANKAUDIOFILE_ENCODING_UNKNOWN               0
#define PLANKAUDIOFILE_ENCODING_UNUSED1               1

#define PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN      2
#define PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN         3
#define PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN    4
#define PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN       5

#define PLANKAUDIOFILE_ENCODING_MIN                   PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN
#define PLANKAUDIOFILE_ENCODING_MAX                   PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN

#define PLANKAUDIOFILE_WAV_COMPRESSION_PCM            0x0001
#define PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT          0x0003
#define PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE     0xFFFE

#define PLANKAUDIOFILE_AIFC_VERSION                   0xA2805140

#if PLANK_OGGVORBIS
    //#include "oggvorbis/vorbisenc.h"
    #include "oggvorbis/codec.h"
    #include "oggvorbis/vorbisfile.h"
#endif


typedef struct PlankAudioFileFormatInfo
{
    PlankC format;
    PlankC encoding;
    
    PlankI bitsPerSample;
    PlankI bytesPerFrame;
    PlankI numChannels;
    PlankD sampleRate;

} PlankAudioFileFormatInfo;

#endif // PLANK_AUDIOFILECOMMON_H
