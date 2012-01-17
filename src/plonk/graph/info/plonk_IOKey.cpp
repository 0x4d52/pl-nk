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


IOKey::Name IOKey::fromInt (const int value) throw()
{
    static const IOKey::Name names[] = {
        IOKey::Unknown,
        IOKey::Constant,
        IOKey::Variable,
        IOKey::AtomicVariable,
        IOKey::Multiply,
        IOKey::Add,
        IOKey::Signal,
        IOKey::Control,
        IOKey::LeftOperand,
        IOKey::RightOperand,
        IOKey::Position,
        IOKey::Frequency,
        IOKey::Q,
        IOKey::S,
        IOKey::Gain,
        IOKey::Bandwidth,
        IOKey::Duration,
        IOKey::Coeffs,
        IOKey::FFT,
        IOKey::Real,
        IOKey::Imaginary,
        IOKey::BlockSize,
        IOKey::SampleRate,
        IOKey::FilterSampleRate,
        IOKey::OverlapMake,
        IOKey::OverlapMix,
        IOKey::Units,
        IOKey::Bus,
        IOKey::Busses,
        IOKey::Buffer,
        IOKey::Wavetable,
        IOKey::Breakpoints,
        IOKey::AutoDeleteFlag,
        IOKey::HarmonicCount
    };
    
    if (value == IOKey::End)   
    {
        return IOKey::End;
    }
    else if (value == IOKey::Invalid)             
    {
        plonk_assertfalse;
        return IOKey::Invalid;
    }
    else if (value < 0 || value >= IOKey::NumNames)
    {
        plonk_assertfalse;
        return IOKey::Invalid;
    }
    else
    {
        return names[value];
    }
}

Text IOKey::getName (const int keyIndex) throw()
{
    static const char* names[] = {
        "Unknown",
        "Constant",
        "Variable",
        "Atomic Variable",
        "Multiply",
        "Add",
        "Signal",
        "Control",
        "Left Operand",
        "Right Operand",
        "Position",
        "Frequency",
        "Q", 
        "S",
        "Gain",
        "Bandwidth",
        "Duration",
        "Coeffs",
        
        "FFT",
        "Real",
        "Imaginary",
        
        "Block Size",
        "Sample Rate",
        "Filter Sample Rate",
        "Overlap Make",
        "Overlap Mix",
        
        "Units",
        "Bus",
        "Busses",
        "Buffer",
        "Wavetable",
        "Breakpoints",
        
        "Auto Delete Flag",
        "Harmonic Count"
    };
    
    if (keyIndex < 0 || keyIndex >= NumNames)
        return "Invalid";
    else
        return names[keyIndex];
}

IOKey::IOType IOKey::getType (const int keyIndex) throw()
{
    static const IOKey::IOType typeCodes[] = {
        IOKey::TypeUnknown,
        IOKey::TypeNumerical,       //"Constant",
        IOKey::TypeVariable,        //"Variable"
        IOKey::TypeAtomicVariable,  //"AtomicVariable"
        IOKey::TypeUnit,            //"Multiply",
        IOKey::TypeUnit,            //"Add",
        IOKey::TypeUnit,            //"Signal",
        IOKey::TypeUnit,            //"Control"
        IOKey::TypeUnit,            //"LeftOperand",
        IOKey::TypeUnit,            //"RightOperand",
        IOKey::TypeUnit,            //"Position",
        IOKey::TypeUnit,            //"Frequency",
        IOKey::TypeUnit,            //"Q",
        IOKey::TypeUnit,            //"S",
        IOKey::TypeUnit,            //"Gain",
        IOKey::TypeUnit,            //"Bandwidth",
        IOKey::TypeUnit,            //"Duration"
        IOKey::TypeUnit,            //"Coeffs",
        IOKey::TypeUnit,            //"FFT",
        IOKey::TypeUnit,            //"Real",
        IOKey::TypeUnit,            //"Imaginary",
        
        IOKey::TypeBlockSize,       //"BlockSize"
        IOKey::TypeSampleRate,      //"SampleRate"
        IOKey::TypeSampleRate,      //"FilterSampleRate"
        IOKey::TypeVariable,        //"Overlap Make"
        IOKey::TypeVariable,        //"Overlap Mix"

        IOKey::TypeUnits,           //"Units",
        IOKey::TypeBus,             //"Bus",
        IOKey::TypeBusses,          //"Busses",
        IOKey::TypeBuffer,          //"Buffer",
        IOKey::TypeWavetable,       //"Wavetable",
        IOKey::TypeBreakpoints,     //"Breakpoints"
        
        IOKey::TypeBool,            //"Auto Delete Flag"
        IOKey::TypeNumerical        //"Harmonic Count"
    };
    
    if (keyIndex < 0 || keyIndex >= IOKey::NumNames)
        return IOKey::TypeUnknown;
    else
        return typeCodes[keyIndex];        
}

Text IOKey::getTypeName (const int keyIndex) throw()
{
    static const char* typeCodes[] = {
        "Unknown",
        "Numerical",        //"Constant",
        "Variable",         //"Variable"
        "AtomicVariable",   //"AtomicVariable"
        "Unit",             //"Multiply",
        "Unit",             //"Add",
        "Unit",             //"Signal",
        "Unit",             //"Control"
        "Unit",             //"LeftOperand",
        "Unit",             //"RightOperand",
        "Unit",             //"Position",
        "Unit",             //"Frequency",
        "Unit",             //"Q",
        "Unit",             //"S",
        "Unit",             //"Gain",
        "Unit",             //"Bandwidth",
        "Unit",             //"Duration",
        "Unit",             //"Coeffs",
        "Unit",             //"FFT",
        "Unit",             //"Real",
        "Unit",             //"Imaginary",
        
        "BlockSize",        //"BlockSize"
        "SampleRate",       //"SampleRate"
        "SampleRate",       //"FilterSampleRate"
        "DoubleVariable",   //"Overlap Make"
        "DoubleVariable",   //"Overlap Mix"
        
        "Units",            //"Units",
        "Bus",              //"Bus",
        "Busses",           //"Busses",
        "Buffer",           //"Buffer",
        "Wavetable",        //"Wavetable",
        "Breakpoints",      //"Breakpoints"
        
        "Bool",             //"Auto Delete Flag"
        "Numerical"         //"Harmonic Count"
    };
    
    if (keyIndex < 0 || keyIndex >= IOKey::NumNames)
        return "Unknown";
    else
        return typeCodes[keyIndex];            
}

TextArray IOKey::collectNames (IntArray const& keys) throw()
{
    const int numKeys = keys.length();
    TextArray names (TextArray::withSize (numKeys));
    
    for (int i = 0; i < numKeys; ++i)
        names.put (i, IOKey::getName (keys.atUnchecked (i)));
    
    return names;
}

IntArray IOKey::collectTypes (IntArray const& keys) throw()
{
    const int numKeys = keys.length();
    IntArray types (IntArray::withSize (numKeys));
    
    for (int i = 0; i < numKeys; ++i)
        types.put (i, IOKey::getType (keys.atUnchecked (i)));
    
    return types;
}

TextArray IOKey::collectTypeNames (IntArray const& keys) throw()
{
    const int numKeys = keys.length();
    TextArray names (TextArray::withSize (numKeys));
    
    for (int i = 0; i < numKeys; ++i)
        names.put (i, IOKey::getTypeName (keys.atUnchecked (i)));
    
    return names;
}
    

END_PLONK_NAMESPACE
