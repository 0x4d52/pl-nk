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

#ifndef PLONK_IOKEY_H
#define PLONK_IOKEY_H

#include "plonk_InfoHeaders.h"



class IOKey
{
public:
    enum IOType
    {
        TypeUnknown = 0,
        TypeNumerical,
        TypeVariable,
        TypeAtomicVariable,
        TypeUnit,
        TypeUnits,
        TypeBus,
        TypeBusses,
        TypeBuffer,
        TypeWavetable,
        TypeSignal,
        TypeBreakpoints,
        TypeUnitVariable,
        TypeUnitsVariable,
        TypeBusVariable,
        TypeBussesVariable,
        TypeBufferVariable,
        TypeWavetableVariable,
        TypeSignalVariable,
        TypeBreakpointsVariable,
        TypeAudioFileReader,
        TypeBlockSize,
        TypeSampleRate,
//        TypeBlockSizes,
//        TypeSampleRates,
        TypeBool,
        
        NumIOTypes
    };
    
    
    enum Name
    {
        End = -2,       ///< Used to terminate a list of IOKeys.
        Invalid = -1,   ///< Not valid, used to help catch errors.
        Unknown = 0,
        Constant,       ///< A constant value.
        Variable,       ///< A variable value.
        AtomicVariable, ///< An atomic variable.
        Multiply,       ///< A multiplier.
        Add,            ///< An offset.
        Generic,        ///< A generic audio (commonly used where there is only one input or output).
        Control,        ///< A generic control (e.g., used for the gate input of envelope).
        LeftOperand,    ///< Left operand of a binary operation or stereo signal.
        RightOperand,   ///< Right operand of a binary operation or stereo signal.
        Position,       ///< Position, e.g., pan position.
        Frequency,      ///< Frequency e.g., for oscillators, filters or pitch detectors.
        Rate,           ///< Relative speed where 1 = normal
        Q,              ///< Q factor for filters.
        S,              ///< S factor for filters.
        Gain,           ///< Gain specifically (usually in dB as opposed to Multiply).
        Bandwidth,      ///< Bandwidth (could be in Semitones, Octaves or Hertz).
        Duration,       ///< Duration in seconds.
        Decay,          ///< Decay time in seconds
        Feedback,       ///< A feedback amount
        Feedforward,    ///< A feedforward amount
        Coeffs,         ///< Filter coefficients
        FFT,            ///< FFT data
        Real,
        Imaginary,
        
        BlockSize,          ///< Audio processing block size.
        SampleRate,         ///< Audio processing sample rate.
        FilterSampleRate,   ///< Sample rate of a filter, need to change this to FilterSampleRates and have it type SampleRates
        OverlapMake,
        OverlapMix,
        
        Units,          ///< An array of units.
        Bus,            ///< A bus.
        Busses,         ///< An array of busses.
        Buffer,         ///< A single buffer.
        Wavetable,      ///< A wavetable
        Signal,         ///< A stored signal buffer
        Breakpoints,    ///< Breakpoints (i.e., an envelope specification)
        
        UnitVariable,           ///< A variable unit
        UnitsVariable,          ///< An variable array of units.
        BusVariable,            ///< A variable bus.
        BussesVariable,         ///< An variable array of busses.
        BufferVariable,         ///< A variable single buffer.
        WavetableVariable,      ///< A variable wavetable
        SignalVariable,         ///< A variable signal buffer
        BreakpointsVariable,    ///< A variable Breakpoints object (i.e., an envelope specification)
        
        AudioFileReader,        ///< An AudioFileReader

        AutoDeleteFlag, ///< To control the auto deletion
        PurgeNullUnitsFlag,
        HarmonicCount,
        BufferCount,
        MaximumDuration,
        PreferredNumChannels,

        NumNames
    };

    static IOKey::Name fromInt (const int value) throw();

    static Text getName (const int keyIndex) throw();
    static IOKey::IOType getType (const int keyIndex) throw();
    static Text getTypeName (const int keyIndex) throw();
    
    static TextArray collectNames (IntArray const& keys) throw();
    static IntArray collectTypes (IntArray const& keys) throw();
    static TextArray collectTypeNames (IntArray const& keys) throw();
        
private:
};



#endif // PLONK_IOKEY_H