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
 infrastructure is in place to make integer or maybe fixed-point processing a possibility 
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
 
 These arithmetic operations are evaluated on assignment unlike the Variable class 
 (although you could wrap a NumericalArray in a Variable if you wish).
 
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
 Ints a (1, 2, 3); // array length aL
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
 int as = a.size();    // is 6
 int bs = b.size();    // is 7
 int cs = c.size();    // is 13
 @endcode
 
 If you need to access the raw C++ array stored in the Plonk array you can do use getArray() or cast the 
 Plonk array to a pointer to the array type:
 
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
 reallocated if it is modified (e.g., by adding items).
 
*/




