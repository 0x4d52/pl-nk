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

#ifndef PLONK_AUDIOFILE_H
#define PLONK_AUDIOFILE_H

/** Some audio file common constants. */
class AudioFile
{
public:
    enum Format
    {
        FormatInvalid               = PLANKAUDIOFILE_FORMAT_INVALID,
        FormatUnknown               = PLANKAUDIOFILE_FORMAT_UNKNOWN,
        FormatWAV                   = PLANKAUDIOFILE_FORMAT_WAV,
        FormatAIFF                  = PLANKAUDIOFILE_FORMAT_AIFF,
        FormatAIFC                  = PLANKAUDIOFILE_FORMAT_AIFC,
        FormatOggVorbis             = PLANKAUDIOFILE_FORMAT_OGGVORBIS,
        FormatOpus                  = PLANKAUDIOFILE_FORMAT_OPUS,
        FormatCAF                   = PLANKAUDIOFILE_FORMAT_CAF
    };
    
    enum EncodingFlag
    {
        EncodingFlagBigEndian       = PLANKAUDIOFILE_ENCODING_BIGENDIAN_FLAG,
        EncodingFlagPCM             = PLANKAUDIOFILE_ENCODING_PCM_FLAG,
        EncodingFlagFloat           = PLANKAUDIOFILE_ENCODING_FLOAT_FLAG,
        EncodingFlagNonIntervleaved = PLANKAUDIOFILE_ENCODING_NONINTERLEAVED_FLAG
    };
    
    enum Encoding
    {
        EncodingInvalid                         = PLANKAUDIOFILE_ENCODING_INVALID,
        EncodingUnknown                         = PLANKAUDIOFILE_ENCODING_UNKNOWN,
        EncodingUnused1                         = PLANKAUDIOFILE_ENCODING_UNUSED1,
                
        EncodingPCMLittleEndian                 = PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN,
        EncodingPCMBigEndian                    = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN,
        EncodingFloatLittleEndian               = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN,
        EncodingFloatBigEndian                  = PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN,
        
        EncodingPCMLittleEndianNonInterleaved   = PLANKAUDIOFILE_ENCODING_PCM_LITTLEENDIAN_NONINTERLEAVED,
        EncodingPCMBigEndianNonInterleaved      = PLANKAUDIOFILE_ENCODING_PCM_BIGENDIAN_NONINTERLEAVED,
        EncodingFloatLittleEndianNonInterleaved = PLANKAUDIOFILE_ENCODING_FLOAT_LITTLEENDIAN_NONINTERLEAVED,
        EncodingFloatBigEndianNonInterleaved    = PLANKAUDIOFILE_ENCODING_FLOAT_BIGENDIAN_NONINTERLEAVED,
                
        EncodingMin                             = PLANKAUDIOFILE_ENCODING_MIN,
        EncodingMax                             = PLANKAUDIOFILE_ENCODING_MAX
    };
    
    enum WAVOption
    {
        CompressionPCM              = PLANKAUDIOFILE_WAV_COMPRESSION_PCM,
        CompressionFloat            = PLANKAUDIOFILE_WAV_COMPRESSION_FLOAT,
        CompressionExtensible       = PLANKAUDIOFILE_WAV_COMPRESSION_EXTENSIBLE
    };
    
    enum AIFFOption
    {
        AIFCVersion                 = PLANKAUDIOFILE_AIFC_VERSION
    };
    
    enum SampleType
    {
        Invalid,
        Short,
        Int24,
        Int,
        Char,
        Float,
        Double,
        NumSampleTypes
    };
    
    enum Defaults
    {
        DefaultBufferSize = 32768
    };

};

#endif // PLONK_AUDIOFILE_H
