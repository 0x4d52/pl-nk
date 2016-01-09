/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
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

#ifndef PLONK_MEASURE_H
#define PLONK_MEASURE_H

#include "plonk_InfoHeaders.h"



class Measure
{
public:
    enum Name
    {
        Unknown = 0,            ///< Not known.
        None,                   ///< None i.e., not applicable
        Bool,                   ///< Boolean: true/false, on/off.
        NormalisedUnipolar,     ///< Normalised (e.g., for floating point types 0 ... 1).
        NormalisedBipolar,      ///< Normalised (e.g., for floating point types -1 ... 1).
        Factor,                 ///< A multiplication factor.
        Percent,                ///< A percentage.
        Seconds,                ///< Time in seconds.
        Samples,                ///< A number of audio samples.
        Cents,                  ///< Pitch in cents (1200 per octave).
        Semitones,              ///< Pitch in semitones, also used to denote MIDI note numbers.
        Octaves,                ///< Pitch in octaves.
        Hertz,                  ///< Frequency in Hertz.
        SampleRateRatio,        ///< Frequencty as a ratio of a sample rate (e.g., the relevant channel's sample rate).
        Q,                      ///< Q factor for filters.
        Slope,                  ///< S factor for filters.
        Decibels,               ///< Decibels.
        Radians,                ///< Angle in radians.
        Degrees,                ///< Angle in degrees.
        FFTUnpacked,            ///< Complex FFT data in packed format, real/imag in the same channel, imag for DC/Nyquist removed
        FFTPacked,              ///< Complex FFT data unpacked with real data in even channels, imag data in odd channels
        Real,
        Imaginary,
        Coeffs,
        Count,
        
        NumNames
    };
    
    static Measure::Name fromInt (const int value) throw();
    
    static bool isNotUnknownOrNone (const int index) throw();
    
    static Text getName (const int index) throw();
    static Text getSymbol (const int index) throw();

    static TextArray collectNames (IntArray const& keys) throw();
    static TextArray collectSymbols (IntArray const& keys) throw();

};



#endif // PLONK_MEASURE_H