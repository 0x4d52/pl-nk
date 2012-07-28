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

#ifndef PLONK_CONTAINERFORWARDDECLARATIONS_H
#define PLONK_CONTAINERFORWARDDECLARATIONS_H

class Text;
class TextArray;
class Dynamic;
class Globals;
class Int24;
class Function;

template<class Type>                                                    class AtomicValue;
template<class Type>                                                    class AtomicExtended;
template<class ObjectType>                                              class SimpleArray;
template<class ObjectType>                                              class ObjectArray;
//template<class ObjectType, 
//         class BaseType = SenderInternal<ObjectArray<ObjectType> > >    class ObjectArrayInternal;
template<class ObjectType, 
         class BaseType = SmartPointer>                                 class ObjectArrayInternal;
template<class ArrayType, 
         class RowType = ObjectArray<ArrayType> >                       class ObjectArray2DBase;
template<class ArrayType, 
         class RowType = ObjectArray<ArrayType> >                       class ObjectArray2D;
template<class NumericalType>                                           class NumericalArray;
template<class NumericalType, 
         class RowType = NumericalArray<NumericalType> >                class NumericalArray2D;
template<class ValueType, class KeyType = Text>                         class KeyValuePair;
template<class ValueType, class KeyType = Text>                         class DictionaryInternal;
template<class ValueType, class KeyType = Text>                         class Dictionary;
template<class ValueType>                                               class LinkedList;
template<class ValueType>                                               class LinkedListElement;

template<class ValueType>                                               class LockFreeQueue;
template<class ValueType>                                               class LockFreeStack;

template<class ValueType>                                               class SimpleQueue;
template<class ValueType>                                               class SimpleStack;
template<class ValueType>                                               class SimpleLinkedList;
template<class ValueType>                                               class SimpleMap;

template<class SampleType>                                              class BreakpointInternal;
template<class SampleType>                                              class BreakpointBase;
template<class SampleType>                                              class BreakpointsInternal;
template<class SampleType>                                              class BreakpointsBase;
template<class SampleType>                                              class WavetableBase;
template<class SampleType>                                              class SignalBase;

template<class ReturnType,
         class ArgType1 = void, 
         class ArgType2 = void,
         class ArgType3 = void>                                         class FunctionContainer;
template<class ReturnType,
         class ArgType1 = void, 
         class ArgType2 = void,
         class ArgType3 = void>                                         class FunctionContainerProxy;
template<class FunctionType,
         class ReturnType,
         class ArgType1 = void, 
         class ArgType2 = void,
         class ArgType3 = void>                                         class FunctionCallerInternal;
template<class FunctionType,
         class ReturnType,
         class ArgType1 = void, 
         class ArgType2 = void,
         class ArgType3 = void>                                         class FunctionProxyCallerInternal;
template<class FunctionType,
         class ReturnType,
         class ArgType1 = void, 
         class ArgType2 = void,
         class ArgType3 = void>                                         class FunctionCallerBase;
template<class FunctionType,
         class ReturnType,
         class ArgType1 = void, 
         class ArgType2 = void,
         class ArgType3 = void>                                         class FunctionProxyCallerBase;
template<class FunctionType, 
         int argumentCount = FunctionType::argumentCount,
         bool isProxy = FunctionType::isProxy>                          class FunctionCaller;

typedef ObjectArray<Dynamic>            DynamicArray;
typedef ObjectArray<Dynamic>            Dynamics;

typedef NumericalArray<Bool>			BoolArray;
typedef NumericalArray<Char>			CharArray;
typedef NumericalArray<Short>			ShortArray;
typedef NumericalArray<Int>				IntArray;
typedef NumericalArray<Int24>			Int24Array;
typedef NumericalArray<Long>			LongArray;
typedef NumericalArray<UnsignedChar>	UnsignedCharArray;
typedef UnsignedCharArray				ByteArray;
typedef NumericalArray<UnsignedShort>	UnsignedShortArray;
typedef NumericalArray<UnsignedInt>     UnsignedIntArray;
typedef NumericalArray<UnsignedLong>	UnsignedLongArray;
typedef NumericalArray<Float>			FloatArray;
typedef NumericalArray<Double>			DoubleArray;

typedef NumericalArray<Bool>			Bools;
typedef NumericalArray<Char>			Chars;
typedef NumericalArray<Short>			Shorts;
typedef NumericalArray<Int>				Ints;
typedef NumericalArray<Int24>			Int24s;
typedef NumericalArray<Long>			Longs;
typedef NumericalArray<UnsignedChar>	Unsigneds;
typedef UnsignedCharArray				Bytes;
typedef NumericalArray<UnsignedShort>	UnsignedShorts;
typedef NumericalArray<UnsignedInt>     UnsignedInts;
typedef NumericalArray<UnsignedLong>	UnsignedLongs;
typedef NumericalArray<Float>			Floats;
typedef NumericalArray<Double>			Doubles;

typedef NumericalArray<Bool>                Bool1D;
typedef NumericalArray<Char>                Char1D;
typedef NumericalArray<Short>               Short1D;
typedef NumericalArray<Int>                 Int1D;
typedef NumericalArray<Int24>               Int241D;
typedef NumericalArray<Long>                Long1D;
typedef NumericalArray<UnsignedChar>        Unsigned1D;
typedef UnsignedCharArray                   Byte1D;
typedef NumericalArray<UnsignedShort>       UnsignedShort1D;
typedef NumericalArray<UnsignedInt>         UnsignedInt1D;
typedef NumericalArray<UnsignedLong>        UnsignedLong1D;
typedef NumericalArray<Float>               Float1D;
typedef NumericalArray<Double>              Double1D;
typedef NumericalArray<PLONK_TYPE_DEFAULT>  Buffer;

typedef NumericalArray2D<Char>				CharArray2D;
typedef NumericalArray2D<Short>				ShortArray2D;
typedef NumericalArray2D<Int>				IntArray2D;
typedef NumericalArray2D<Int24>				Int24Array2D;
typedef NumericalArray2D<Long>				LongArray2D;
typedef NumericalArray2D<UnsignedChar>		UnsignedCharArray2D;
typedef NumericalArray2D<UnsignedChar>		ByteArray2D;
typedef NumericalArray2D<UnsignedShort>     UnsignedShortArray2D;
typedef NumericalArray2D<UnsignedInt>		UnsignedIntArray2D;
typedef NumericalArray2D<UnsignedLong>		UnsignedLongArray2D;
typedef NumericalArray2D<Float>				FloatArray2D;
typedef NumericalArray2D<Double>			DoubleArray2D;

typedef NumericalArray2D<Char>				Char2D;
typedef NumericalArray2D<Short>				Short2D;
typedef NumericalArray2D<Int>				Int2D;
typedef NumericalArray2D<Int24>				Int242D;
typedef NumericalArray2D<Long>				Long2D;
typedef NumericalArray2D<UnsignedChar>		UnsignedChar2D;
typedef NumericalArray2D<UnsignedChar>		Byte2D;
typedef NumericalArray2D<UnsignedShort>     UnsignedShort2D;
typedef NumericalArray2D<UnsignedInt>		UnsignedInt2D;
typedef NumericalArray2D<UnsignedLong>		UnsignedLong2D;
typedef NumericalArray2D<Float>				Float2D;
typedef NumericalArray2D<Double>			Double2D;

typedef ObjectArray2D<Text,TextArray>       TextArray2D;
typedef ObjectArray2D<Text,TextArray>       Text2D;

typedef BreakpointBase<Float>                 FloatBreakpoint;
typedef BreakpointBase<Double>                DoubleBreakpoint;
typedef BreakpointBase<Short>                 ShortBreakpoint;
typedef BreakpointBase<Int>                   IntBreakpoint;
typedef BreakpointBase<Int24>                 Int24Breakpoint;
typedef BreakpointBase<Long>                  LongBreakpoint;
typedef BreakpointBase<PLONK_TYPE_DEFAULT>    Breakpoint;

typedef BreakpointsBase<Float>                 FloatBreakpoints;
typedef BreakpointsBase<Double>                DoubleBreakpoints;
typedef BreakpointsBase<Short>                 ShortBreakpoints;
typedef BreakpointsBase<Int>                   IntBreakpoints;
typedef BreakpointsBase<Int24>                 Int24Breakpoints;
typedef BreakpointsBase<Long>                  LongBreakpoints;
typedef BreakpointsBase<PLONK_TYPE_DEFAULT>    Breakpoints;

typedef WavetableBase<Float>                 FloatWavetable;
typedef WavetableBase<Double>                DoubleWavetable;
typedef WavetableBase<Short>                 ShortWavetable;
typedef WavetableBase<Int>                   IntWavetable;
typedef WavetableBase<Int24>                 Int24Wavetable;
typedef WavetableBase<Long>                  LongWavetable;
typedef WavetableBase<PLONK_TYPE_DEFAULT>    Wavetable;

typedef SignalBase<Float>                 FloatSignal;
typedef SignalBase<Double>                DoubleSignal;
typedef SignalBase<Short>                 ShortSignal;
typedef SignalBase<Int>                   IntSignal;
typedef SignalBase<Int24>                 Int24Signal;
typedef SignalBase<Long>                  LongSignal;
typedef SignalBase<PLONK_TYPE_DEFAULT>    Signal;

typedef AtomicValue<Int>                AtomicInt;
typedef AtomicValue<Long>               AtomicLong;
typedef AtomicValue<LongLong>           AtomicLongLong;
typedef AtomicValue<Float>              AtomicFloat;
typedef AtomicValue<Double>             AtomicDouble;
typedef AtomicValue<char*>              AtomicPointer;
typedef AtomicExtended<char*>           AtomicExtendedPointer;
typedef AtomicExtended<Dynamic*>        AtomicDynamicPointer;


#endif // PLONK_CONTAINERFORWARDDECLARATIONS_H