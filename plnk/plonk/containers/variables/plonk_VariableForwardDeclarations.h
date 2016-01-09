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

#ifndef PLONK_VARIABLEFORWARDDECLARATIONS_H
#define PLONK_VARIABLEFORWARDDECLARATIONS_H

#include "../../maths/plonk_InlineBinaryOps.h"
#include "../../maths/plonk_InlineUnaryOps.h"
#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../plonk_ContainerForwardDeclarations.h"
#include "../plonk_Atomic.h"

template<class Type>                                        class Variable;
template<class Type>                                        class VariableInternalBase;
template<class Type>                                        class VariableInternal;

template<class Type, PLONK_BINARYOPFUNCTION(Type, op)>      class BinaryOpVariableInternal;
template<class Type, PLONK_UNARYOPFUNCTION(Type, op)>       class UnaryOpVariableInternal;
template<class Type, class OtherType>                       class TypeVariableInternal;
template<class Type>                                        class ShapeVariableInternal;
template<class Type>                                        class PatternVariableInternal;

typedef Variable<float>     FloatVariable;
typedef Variable<double>    DoubleVariable;
typedef Variable<int>       IntVariable;
typedef Variable<short>     ShortVariable;
typedef Variable<Int24>     Int24Variable;
typedef Variable<Long>      LongVariable;
typedef Variable<LongLong>  LongLongVariable;
typedef Variable<char>      CharVariable;
typedef Variable<bool>      BoolVariable;

// these don't really store references, they're just used to manipulate the compiler
typedef Variable<AtomicFloat&>                      AtomicFloatVariable;
typedef Variable<AtomicDouble&>                     AtomicDoubleVariable;
typedef Variable<AtomicInt&>                        AtomicIntVariable;
typedef Variable<AtomicLong&>                       AtomicLongVariable;
typedef Variable<AtomicLongLong&>                   AtomicLongLongVariable;
typedef Variable<AtomicPointer&>                    AtomicPointerVariable;
typedef Variable<AtomicExtendedPointer&>            AtomicExtendedPointerVariable;
typedef Variable<AtomicDynamicPointer&>             AtomicDynamicPointerVariable;

typedef Variable<FloatArray&>                       FloatArrayVariable;
typedef Variable<DoubleArray&>                      DoubleArrayVariable;
typedef Variable<IntArray&>                         IntArrayVariable;
typedef Variable<ShortArray&>                       ShortArrayVariable;
typedef Variable<Int24Array&>                       Int24ArrayVariable;
typedef Variable<LongArray&>                        LongArrayVariable;
typedef Variable<CharArray&>                        CharArrayVariable;
typedef Variable<BoolArray&>                        BoolArrayVariable;
typedef Variable<Buffer&>                           BufferVariable;

typedef Variable<Text&>                             TextVariable;
typedef Variable<TextArray&>                        TextArrayVariable;

typedef Variable<FloatBreakpoint&>                  FloatBreakpointVariable;
typedef Variable<DoubleBreakpoint&>                 DoubleBreakpointVariable;
typedef Variable<IntBreakpoint&>                    IntBreakpointVariable;
typedef Variable<ShortBreakpoint&>                  ShortBreakpointVariable;
typedef Variable<Int24Breakpoint&>                  Int24BreakpointVariable;
typedef Variable<LongBreakpoint&>                   LongBreakpointVariable;
typedef Variable<Breakpoint&>                       BreakpointVariable;

typedef Variable<FloatBreakpoints&>                 FloatBreakpointsVariable;
typedef Variable<DoubleBreakpoints&>                DoubleBreakpointsVariable;
typedef Variable<IntBreakpoints&>                   IntBreakpointsVariable;
typedef Variable<ShortBreakpoints&>                 ShortBreakpointsVariable;
typedef Variable<Int24Breakpoints&>                 Int24BreakpointsVariable;
typedef Variable<LongBreakpoints&>                  LongBreakpointsVariable;
typedef Variable<Breakpoints&>                      BreakpointsVariable;

typedef Variable<FloatWavetable&>                   FloatWavetableVariable;
typedef Variable<DoubleWavetable&>                  DoubleWavetableVariable;
typedef Variable<IntWavetable&>                     IntWavetableVariable;
typedef Variable<ShortWavetable&>                   ShortWavetableVariable;
typedef Variable<Int24Wavetable&>                   Int24WavetableVariable;
typedef Variable<LongWavetable&>                    LongWavetableVariable;
typedef Variable<Wavetable&>                        WavetableVariable;

typedef Variable<FloatSignal&>                      FloatSignalVariable;
typedef Variable<DoubleSignal&>                     DoubleSignalVariable;
typedef Variable<IntSignal&>                        IntSignalVariable;
typedef Variable<ShortSignal&>                      ShortSignalVariable;
typedef Variable<Int24Signal&>                      Int24SignalVariable;
typedef Variable<LongSignal&>                       LongSignalVariable;
typedef Variable<Signal&>                           SignalVariable;

typedef NumericalArray< Variable<float> >           FloatVariableArray;
typedef NumericalArray< Variable<double> >          DoubleVariableArray;
typedef NumericalArray< Variable<int> >             IntVariableArray;
typedef NumericalArray< Variable<short> >           ShortVariableArray;
typedef NumericalArray< Variable<Int24> >           Int24VariableArray;
typedef NumericalArray< Variable<Long> >            LongVariableArray;
typedef NumericalArray< Variable<LongLong> >        LongLongVariableArray;
typedef NumericalArray< Variable<char> >            CharVariableArray;
typedef NumericalArray< Variable<bool> >            BoolVariableArray;

typedef NumericalArray< Variable<float> >           FloatVariables;
typedef NumericalArray< Variable<double> >          DoubleVariables;
typedef NumericalArray< Variable<int> >             IntVariables;
typedef NumericalArray< Variable<short> >           ShortVariables;
typedef NumericalArray< Variable<Int24> >           Int24Variables;
typedef NumericalArray< Variable<Long> >            LongVariables;
typedef NumericalArray< Variable<LongLong> >        LongLongVariables;
typedef NumericalArray< Variable<char> >            CharVariables;
typedef NumericalArray< Variable<bool> >            BoolVariables;

typedef Variable< FloatFFTBuffers& >                FloatFFTBuffersVariable;
typedef Variable< FFTBuffers& >                     FFTBuffersVariable;


#endif // PLONK_VARIABLEFORWARDDECLARATIONS_H