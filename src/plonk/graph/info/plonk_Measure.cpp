/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
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
 -------------------------------------------------------------------------------
 */

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"


Measure::Name Measure::fromInt (const int value) throw()
{
    static const Measure::Name names[] = {
        Measure::Unknown,
        Measure::None,
        Measure::Bool,
        Measure::NormalisedUnipolar,
        Measure::NormalisedBipolar,
        Measure::Factor,
        Measure::Percent,
        Measure::Seconds,
        Measure::Samples,
        Measure::Cents,
        Measure::Semitones,
        Measure::Octaves,
        Measure::Hertz,
        Measure::SampleRateRatio,
        Measure::Q,
        Measure::Slope,
        Measure::Decibels,
        Measure::Radians,
        Measure::Degrees,
        Measure::Real,
        Measure::Imaginary,
        Measure::Coeffs
    };
    
    if (value < 0 || value >= Measure::NumNames)
    {
        plonk_assertfalse;
        return Measure::Unknown;
    }
    else
    {
        return names[value];
    }
}

bool Measure::isNotUnknownOrNone (const int index) throw()
{
    return (index != Measure::Unknown) && (index != Measure::None);
}

Text Measure::getName (const int index) throw()
{
    static const char* names[] = {
        "Unknown",
        "None",
        "Bool",
        "Normalised Unipolar",
        "Normalised Bipolar",
        "Factor",
        "Percent",
        "Seconds",
        "Samples",
        "Cents",
        "Semitones",
        "Octaves",
        "Hertz",
        "Sample Rate Ratio",
        "Q",
        "Slope",
        "Decibels",       
        "Radians",
        "Degrees",
        "Real",
        "Imaginary",
        "Coeffs"
    };
    
    if (index < 0 || index >= Measure::NumNames)
        return "Unknown";
    else
        return names[index];                
}

Text Measure::getSymbol (const int index) throw()
{
    static const char* symbols[] = {
        "",         //"Unknown",
        "",         //"None",
        "",         //"Bool"
        "",         //"Normalised Unipolar",
        "",         //"Normalised Bipolar",
        "",         //"Factor"
        "%",        //"Percent"
        "s",        //"Seconds",
        "samps",    //"Samples",
        "cents",    //"Cents"
        "",         //"Semitone"
        "oct",      //"Octaves"
        "Hz",       //"Hertz",
        "",         //"Sample Rate Ratio",
        "Q",        //"Q",
        "S",
        "dB",       //"Decibels",  
        "",         //"Radians"
        "o",        //"Degrees" maybe do this in Unicode?
        "",         //"Real"
        "i",        //"Imaginary"
        ""
    };
    
    if (index < 0 || index >= Measure::NumNames)
        return "";
    else
        return symbols[index];                    
}

TextArray Measure::collectNames (IntArray const& keys) throw()
{
    const int numKeys = keys.length();
    TextArray names (TextArray::withSize (numKeys));
    
    for (int i = 0; i < numKeys; ++i)
        names.put (i, Measure::getName (keys.atUnchecked (i)));
    
    return names;    
}

TextArray Measure::collectSymbols (IntArray const& keys) throw()
{
    const int numKeys = keys.length();
    TextArray names (TextArray::withSize (numKeys));
    
    for (int i = 0; i < numKeys; ++i)
        names.put (i, Measure::getSymbol (keys.atUnchecked (i)));
    
    return names;    
}



END_PLONK_NAMESPACE
