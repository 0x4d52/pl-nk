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


/** @page PlonkCoreConcepts
    @ingroup PlonkCoreConcepts

 @section Introduction Introduction

 @section FundamentalTypes Fundamental types
 Plonk generally makes use of the standard C++ types, @c char, @c int, @c float, @c double etc. 
 To keep consistent types on all platforms these are @c typedef 'd in the plonk namespace as follows:
 - @c Char (generally built-in @c char) always an 8-bit signed integer.
 - @c Short (generally built-in @c short) always a 16-bit signed integer.
 - @c Int (generally built-in @c int_ always a 32-bit signed integer.
 - @c Long - a 32-bit signed integer on 32-bit architectures and 64-bits on 64-bit architectures. 
   NB this is therefore not the same as a @c long on Windows under Visual Studio.
 - @c LongLong - a 64-bit signed integer.
 - @c UnsignedChar (generally built-in @c unsigned @c char) always an 8-bit unsigned integer.
 - @c UnsignedShort (generally built-in @c unsigned @c short) always a 16-bit unsigned integer.
 - @c UnsignedInt (generally built-in @c unsigned @c int) always a 32-bit unsigned integer.
 - @c UnsignedLong - a 32-bit unsigned integer on 32-bit architectures and 64-bits on 64-bit architectures.
 - @c UnsignedLongLong - a 64-bit unsigned integer.
 - @c Float (generally built-in @c float) always a 32-bit floating point type.
 - @c Double (generally built-in @c double) always a 64-bit floating point type.
 - @c Bool - built-in @c bool.
 
 There is also an Int24 class to assist with manipulating 24-bit audio data.
 
 @section CoreClasses Core classes
 Plonk is designed to fully support a range of audio sample types in the future. Currently,
 only 32-bit float support is extensively tested but 64-bit support should work (and the
 infrastructure is in place to make integer and maybe fixed-point processing a possibility 
 in the future). To help support this many Plonk classes are C++ template classes. For example,
 The Variable class is a template. Essentially, this allows you to store another value or object 
 in a rerefence counted container (Variable has other useful features too which are outlined below).
 To store an @c int in a Variable you would use a Variable<Int>, for convenience (and to avoid
 many nested angle brackets that can result from templated code) commonly used types are
 @c typedef 'd. For example, IntVariable is equivalent to Variable<Int> (and this naming scheme
 is used throughout Plonk for similar @c typedef s

 Plonk classes are divided into "user" classes and "internal" (i.e., implementation) classes.
 Most internal classes are denoted by the @c Internal suffix added to the class name 
 (should you come across them in the documentation).  Generally, Plonk classes come in pairs 
 (e.g., @c SomeClass and @c SomeClassInternal). As a user you will generally deal with the 
 user classes and almost never the internal classes directly (unless of course you write your
 own extensions using the same pattern). Almost all Plonk classes like this are built on a 
 custom reference counting base class (which incidentally is thread safe and lock free) to
 help manage object lifetimes. You should never need to deal with the reference counting system, 
 simply create user classes on the stack, store them by value in your own classes, and pass around 
 by value (or by reference if you're careful).

 For example, to use the Variable class to store and manipulate integers you could do something like:
 
 @code
 IntVariable a (1); // 'a' initialised to 1
 IntVariable b (2); // 'b' initialised to 2
 IntVariable c (b); // 'c' shares same state as 'b'
 
 a = 3; // 'a' now set to 3 - "a.setValue(3)" is equivalent
 a = b; // 'a' now shares 'b' (the original internal from 'a' is destroyed as it's no longer referenced) 
 b = 4; // 'a', 'b' and 'c' all store 4 since they all share the same state
 @endcode
 
 This technique should mean that you can avoid the use of explicit memory allocation
 in your own user code. You should rarely need to use @c new, @c delete, @c malloc
 or @c free (etc). One use of the Variable class is to store values you wish to use in
 an audio process that you wish to manipulate from elsewhere (e.g., from A GUI).
 
 As mentioned previously, the Variable class has other useful features. Instances of
 Variable can be combined into expressions that are evaluated at a later time. 
 For example:
 
 @code
 IntVariable a (1);
 IntVariable b (2);
 IntVariable c = a + b; // creates a variable that will evaluate "a + b" at a later time;
 
 int t1 = a; // is 1 - implies "t1 = a.getValue()"
 int t2 = b; // is 2
 int t3 = c; // is 3 - evaluates "a.getValue() + b.getValue()"
 
 a = 100;
 
 int t4 = c; // is 102
 @endcode
  
 Variable supports many unary and binary arithmetic operators in this way including 
 @c +, @c -, @c *, @c /, @c pow, and so on. The Variable documentation page provides a full list.
 
 @subsection Arrays Arrays

 Plonk provides its own dynamic array class ObjectArray (which is also a template class and
 employs the same reference counted lifetime management as the Variable class). ObjectArray
 is very generalised. Please note that while assignment of a Plonk array is thread safe, 
 manipulations of the array are not. To store and manipulate numerical data there is the 
 specialised class NumericalArray (which inherits from ObjectArray). 
 
 As with the Variable class there are @c typedef s for many of the common types.
 One convention in Plonk is to use the plural form to imply an array e.g., Ints is a 
 NumericalArray<Int>. It is appreciated that this style may not suit all users and there are
 alternative forms using the word 'Array', thus IntArray is equivalent to Ints (both are
 NumericalArray<Int>). There is nothing to prevent you from using an ObjectArray<Int> but the
 NumericalArray has added arithmetic functionality.
 
 @code
 Ints a;              // empty array
 Ints b (1);          // an array containing a single element '1'
 Ints c (1, 2, 3, 4); // initialise with array {1, 2, 3, 4}, up to 32 items can be used to initialise this way
 
 int cn = c.length(); // is 4, the number of items stored in 'c'
 int cs = c.size();   // is also 4, 'size' and 'length' are only different for null terminated arrays (see Text below)
 
 b.add (10);          // 'b' is now {1, 10}
 b.add (100);         // 'b' is now {1, 10, 100}
 
 c[1] = 20;           // 'c' is now {1, 20, 3, 4}
 b[2] = c[1];         // 'b' is now {1, 10, 20}
 a = b;               // 'a' now shares 'b'
 
 a.remove (0);        // remove at index 0, 'a' and 'b' are now both {10, 20}
 c.removeItem (20);   // removes item '20', 'c' is now { 1, 3, 4}
 c.removeItem (100);  // has no effect as 'c' doesn't contain '100'
 cn = c.length();     // is now 3
 @endcode
 
 By default Plonk checks inidices to ensure they are not out of bounds. If you
 wish to avoid these checks and are sure that array indices are within the bounds
 you can call faster versions:
 
 @code
 Ints a (1, 2, 3);
 int b = a[0]; // is 1
 int c = a[1]; // is 2
 int d = a[2]; // is 3
 int e = a[3]; // index out-of-bounds, returns zero (or a 'null' object if this is not a NumericalArray)
 int f = a.atUnchecked (0); // is 1
 int g = a.atUnchecked (1); // is 2
 int h = a.atUnchecked (2); // is 3
 int i = a.atUnchecked (3); // returns garbage or may crash, raises an assertion though in a Debug build
 
 @endcode
 
 As dicsussed above, arithmetic can be performed on NumericalArray types. For example:
 
 @code
 Ints a (1, 2, 3);          
 Ints b (4, 5, 6);
 Ints c (a + b);            // 'c' is {5, 7, 9}
 Ints d (1, 10, 100, 1000); 
 Ints e (c * d);            // 'e' is {5, 70, 900, 5000} i.e., the shorter array 'loops' to complete the pattern
 Ints f (e);                // 'f' shares 'e'
 e *= 10;                   // 'e' is now {50, 700, 9000, 50000} but 'f' is still {5, 70, 900, 5000}
 @endcode
 
 These arithmetic operations are evaluated on assignment unlike the Variable class 
 (although you could wrap a NumericalArray in a Variable if you wish, to 
 leverage the deferred evaluation capabiltiies of Variable).

 Numerical arrays can be filled with commonly required data e.g., sine wave tables, random values.
 For example:
 
 @code
 Floats a (Floats::newClear (10));        // array with 10 items all zero
 Floats b (Floats::sineTable (8));        // sine wave shape using 8 items (bipolar -1...+1) starting at 0
 Floats c (Floats::cosineTable (8));      // cosine wave shape using 8 items (bipolar -1...+1) starting at 1
 Floats d (Floats::cosineWindow (8));     // cosine wave shape using 8 items (unipolar 0...+1) starting at 0
 Floats e (Floats::rand (4, 10, 100));    // array of 4 random values 10...100 uniformly distributed
 Floats f (Floats::exprand (4, 10, 100)); // array of 4 random values 10...100 exponentially distributed
 @endcode
 
 Text is a specially adapted Chars (CharArray or NumericalArray<Char>) for dealing with text strings.
 
 @code
 Text a ("Hello");
 Text b ("World!");
 Text c (a + " " + b); // is "Hello World!" as '+' is overidden to do concatenation
 int an = a.length();  // is 5
 int bn = b.length();  // is 6
 int cn = c.length();  // is 12
 int as = a.size();    // is 6  (size is one larger to hold the null terminating character
 int bs = b.size();    // is 7
 int cs = c.size();    // is 13
 @endcode
 
 If you need to access the raw C++ array stored in the Plonk array you can use getArray() or cast the 
 Plonk array to a pointer to the array's value type:
 
 @code
 Ints a (1, 2, 3);
 
 // all of these are equivalent
 int* ptr1 = a.getArray();           // explicit getArray()
 int* ptr2 = static_cast<int*> (a);  // explicit cast, calls getArray() internally 
 int* ptr3 (a);                      // implicit cast, calls getArray() internally
 int* ptr4 = a;                      // implicit cast, calls getArray() internally
 int* ptr5 = (int*)a;                // should work, but not C casts are not recommended in C++
 @endcode
 
 It is best not to store any of the pointers obtained this way as the array could be
 reallocated if it is modified (e.g., by adding items). If you are iterating through 
 an array this may be faster than repeated calls to atUnchecked() or the [] operator.
 
 @subsection ChannelsUnitsAndGraphs Channels, units and graphs
 Channels, units and graphs are concepts used to represent audio processing 
 configurations in Plonk.
 - channel: a single channel of audio (i.e, mono)
 - unit: an array of channels
 - graph: an interconnected collection of units
 
 In fact in Plonk there is no separate "graph" object, rather graphs are built by
 passing units to other units during construction. There are channel objects 
 (the ChannelBase<SampleType> class) although you rarely deal with these directly
 in user code. Most user code will create and manipulate unit objects 
 (via the UnitBase<SampleType> class). By default Plonk is setup to deal with 32-bit 
 floating point data throughout its graphs. This is achieved by the macro
 PLONK_TYPE_DEFAULT which is set to 'float'. This macro is used to set the default
 type for the UnitBase class and the unit factory classes. The convention is for the
 base types (ChannelBase, UnitBase) to become simply 'Channel' and 'Unit' for the
 default sample type. For factory classes, these are generally names like 
 SomethingUnit, which becomes simply 'Something' for the default sample type.
 
 For example, the SineUnit factory class is used to create wavetable-based sine
 wave oscillators. To create one using the default sample type you would do this:
 
 @code
 Unit u = Sine::ar (1000, 0.1);
 @endcode
 
 This creates a single channel 1kHz sine oscillator unit at an amplitude of 0.1 
 and stores it in the variable 'u' (of type Unit). The 'ar' factory function
 name is common throughout the factory classes and denotes 'audio rate' in a similar way 
 to other Music-N languages (Csound, SuperCollider etc). This means it will output one
 sample for each sample required by the audio hardware. It is possible to create 'control rate'
 units too although this will be discussed later (in fact there isn't really a global
 notion of 'control rate' as there is in other Music-N langauges as Plonk channels
 can run at arbitrary sample rates and block sizes).
 
 The arguments, or 'inputs', for the factory functions are documented in the factory class
 documentation. For example, SineUnit documentation states:
 
 <table><tr><td>
 Factory functions:
 - ar (frequency=440, mul=1, add=0, preferredBlockSize=default, preferredSampleRate=default)
 - kr (frequency=440, mul=1, add=0) 
 
 Inputs:
 - frequency: (unit, multi) the frequency of the oscillator in Hz
 - mul: (unit, multi) the multiplier applied to the output
 - add: (unit, multi) the offset aded to the output
 - preferredBlockSize: the preferred output block size (for advanced usage, leave on default if unsure)
 - preferredSampleRate: the preferred output sample rate (for advanced usage, leave on default if unsure)
 </td></tr></table>
 
 The 'mul' and 'add' inputs are common to many factory classes these are applied
 after the output of the unit at full ampltitude. The default for float and double sample types
 is ±1. Thus Sine::ar(1000) would output a full scale sine wave at 1kHz. While the 'add'
 input can be used to mix units, this is not its primary purpose nor the most convenient
 way of achieving this. The 'add' input is more useful for creating modulators. E.g.,
 
 @code
 Unit u = Sine::ar (Sine::ar (5, 100, 1000), 0.1);
 @endcode
 
 Here the 5Hz sine is used to modulate another sine. This could be rewritten a follows:
 
 @code
 Unit m = Sine::ar (5, 100, 1000);
 Unit u = Sine::ar (m, 0.1);
 @endcode
 
 (Both of these versions produce identical graphs, the processing required to render
 either graph will be exactly the same. The latter version <em>may</em> be a fraction
 slower to construct but most compilers should be able to optimise.)

 The 'mul' input for 'm' is 100, this is effectively the modulation depth. The 'add'
 input is 1000, this is effectively the centre value for the modulation. Thus the modulator
 will have a centre value of 1000 and deviate by ±100 (i.e., 900...1100). The unit 'm'
 is then used as the frequency input for the unit 'u'. Any input marked as 'unit' can
 be any other unit. Units can be created explicitly using the factory classes, or in some cases
 implicitly from constants, Variable types, NumericalArray types and so on. Inputs marked
 as 'multi' can be used to create multichannel units by passing in a multichannel unit to 
 this input. For example, the following code creates a two-channel unit, both with
 amplitude 0.1 but one with frequency 995Hz and the other 1005Hz:
 
 @code
 Unit u = Sine::ar (Floats (995, 1005), 0.1);
 @endcode
 
 Or alternatively:

 @code
 Unit u = Sine::ar (Unit (995, 1005), 0.1);
 @endcode

 This is equivalent to:
 
 @code
 Unit u = Unit (Sine::ar (995, 0.1), Sine::ar (1005, 0.1));
 @endcode
 
 Similarly, multichannel units can be passed into all inputs labelled 'multi'. The
 following code creates two sines with frequencies as above but with different
 amplitudes too:
 
 @code
 Unit u = Sine::ar (Floats (995, 1005), Floats (0.09, 0.11));
 @endcode

 This is equivalent to:
 
 @code
 Unit u = Unit (Sine::ar (995, 0.09), Sine::ar (1005, 0.11));
 @endcode

 @subsection ImplicitUnaryAndBinaryOperatorUnits Implicit unary and binary operator units
 
 Arithmetic can be performed on units in an intuitive fashion. For example, the following
 lines are all equivalent to each other:
 
 @code
 Unit u = Sine::ar (Floats (995, 1005), 0.1);
 Unit u = Sine::ar (Floats (995, 1005)) * 0.1;
 Unit u = Sine::ar (Floats (995, 1005)) * Unit (0.1);
 @endcode
 
 Similarly, the following lines are also equivalent to each other:
 
 @code
 Unit u = Sine::ar (Floats (995, 1005), Floats (0.09, 0.11));
 Unit u = Sine::ar (Floats (995, 1005)) * Floats (0.09, 0.11);
 Unit u = Sine::ar (Floats (995, 1005)) * Unit (0.1, 0.11);
 @endcode

 Mixing can be performed using the addition operator:
 
 @code
 Unit u = Sine::ar (200, 0.1) +
          Sine::ar (400, 0.1 / 2) +
          Sine::ar (600, 0.1 / 3) +
          Sine::ar (800, 0.1 / 4);
 @endcode
 
 (The creates the first four harmonics of a sawtooth wave.) In most cases it
 is more straightforward to use the MixerUnit to achieve the same result:
 
 @code
 Unit u = Mixer::ar (Sine::ar (Floats (200, 400, 600, 800), 
                               Floats (0.1, 0.1/2, 0.1/3, 0.1/4)));
 @endcode
 
 Or more clearly:
 
 @code
 Unit s = Sine::ar (Floats (200, 400, 600, 800), 
                    Floats (0.1, 0.1/2, 0.1/3, 0.1/4));
 Unit u = Mixer::ar (s);
 @endcode
 
 Or:
 
 @code
 Unit u = Sine::ar (Floats (200, 400, 600, 800), 
                    Floats (0.1, 0.1/2, 0.1/3, 0.1/4)).mix();
 @endcode

 Thus Mixer and Unit::mix() mix down multiple channels to a single channel.
 Mixing multiple channels is as straightforward but requires the use of arrays
 or units i.e. a Units or UnitArray. Take the following example:
 
 @code
 Unit a = Sine::ar (Floats (200, 600), Floats (0.1, 0.1/3));
 Unit b = Sine::ar (Floats (400, 800), Floats (0.1/2, 0.1/4));
 Unit u = a + b;
 @endcode
 
 Here 200Hz and 400Hz are mixed in one channel and 600Hz and 800Hz are mixed in 
 another channel. Using Mixer this could be:
 
 @code
 Unit a = Sine::ar (Floats (200, 600), Floats (0.1, 0.1/3));
 Unit b = Sine::ar (Floats (400, 800), Floats (0.1/2, 0.1/4));
 Units us = Units (a, b);
 Unit u = Mixer::ar (us);
 @endcode

 Clearly in this case this results in <em>more</em> code but with larger numbers
 of units to mix this can be more convenient.
 
 An equivalent would be:
 
 @code
 Units us;
 us.add (Sine::ar (Floats (200, 600), Floats (0.1, 0.1/3)));
 us.add (Sine::ar (Floats (400, 800), Floats (0.1/2, 0.1/4)));
 Unit u = Mixer::ar (us);
 @endcode

 This technique allows you to build arrays of units using conventional C++ loops:
 
 @code
 Units us;
 
 for (int i = 0; i < 4; ++i)
 {
    us.add (Sine::ar (Floats::exprand (2, 100, 1000), 
                      Floats::rand (2, 0.05, 0.1)));
 }
 
 Unit u = Mixer::ar (us);
 @endcode

    
 
 
*/




