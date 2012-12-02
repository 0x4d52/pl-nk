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

#ifndef PLONK_TYPEUTILITY_H
#define PLONK_TYPEUTILITY_H

#include "plonk_CoreForwardDeclarations.h"
#include "../containers/plonk_ContainerForwardDeclarations.h"
#include "../containers/variables/plonk_VariableForwardDeclarations.h"
#include "../graph/plonk_GraphForwardDeclarations.h"
#include "../containers/plonk_Text.h"

class AudioFileReader;

/** Used to determine information about some types.
 Especially used to determine information about sample buffer types
 and the Dynamic class uses it to identify which type it currently stores. 
 This doesn't catalogue all types, only the built-in types and those
 likely to be used as channel inputs (e.g., other units, busses, etc.) 
 @ingroup PlonkOtherUserClasses */
class TypeCode
{
public:
    enum Codes
    {
    //  0
        Unknown, Dynamic,
    // built-in types (2)
        Float, Double, Int, Short, Int24, Long, LongLong, Char, Bool,
    // fixed point types
        FixI8F8, FixI8F24, FixI16F16,
    // atomic types (11)    
        AtomicFloat, AtomicDouble, 
        AtomicInt, AtomicLong, AtomicLongLong, AtomicPointer, AtomicExtendedPointer, AtomicDynamicPointer,
    // variables (19)
        FloatVariable, DoubleVariable, 
        IntVariable, ShortVariable, Int24Variable, LongVariable, CharVariable, BoolVariable,
    // atomic variables (27)
        AtomicFloatVariable, AtomicDoubleVariable, AtomicIntVariable, AtomicLongVariable, 
        AtomicPointerVariable, AtomicExtendedPointerVariable, AtomicDynamicPointerVariable,
    // arrays (34)
        FloatArray, DoubleArray, IntArray, ShortArray, Int24Array, LongArray, CharArray, BoolArray, DynamicArray,
    // other
        Text, TextArray,
    // channels (45)
        FloatChannel, DoubleChannel, IntChannel, ShortChannel, Int24Channel, LongChannel,
    // units (51)
        FloatUnit, DoubleUnit, IntUnit, ShortUnit, Int24Unit, LongUnit,
    // bus (57)
        FloatBus, DoubleBus, IntBus, ShortBus, Int24Bus, LongBus, 
    // unit arrays (63)
        FloatUnits, DoubleUnits, IntUnits, ShortUnits, Int24Units, LongUnits,
    // bus arrays (69)
        FloatBusses, DoubleBusses, IntBusses, ShortBusses, Int24Busses, LongBusses, 
    // breakpoints (75)
        FloatBreakpoints, DoubleBreakpoints, IntBreakpoints, ShortBreakpoints, Int24Breakpoints, LongBreakpoints,
    // wavetables (81)
        FloatWavetable, DoubleWavetable, IntWavetable, ShortWavetable, Int24Wavetable, LongWavetable,
    // signals (88)
        FloatSignal, DoubleSignal, IntSignal, ShortSignal, Int24Signal, LongSignal,
    // arrays
        FloatArrayVariable, DoubleArrayVariable, IntArrayVariable, ShortArrayVariable, 
        Int24ArrayVariable, LongArrayVariable, CharArrayVariable, BoolArrayVariable,
    // other
        TextVariable, TextArrayVariable,
    // channels
        FloatChannelVariable, DoubleChannelVariable, IntChannelVariable, ShortChannelVariable, Int24ChannelVariable, LongChannelVariable,
    // units
        FloatUnitVariable, DoubleUnitVariable, IntUnitVariable, ShortUnitVariable, Int24UnitVariable, LongUnitVariable,
    // bus (57)
        FloatBusVariable, DoubleBusVariable, IntBusVariable, ShortBusVariable, Int24BusVariable, LongBusVariable, 
    // unit arrays
        FloatUnitsVariable, DoubleUnitsVariable, IntUnitsVariable, ShortUnitsVariable, Int24UnitsVariable, LongUnitsVariable,
    // bus arrays
        FloatBussesVariable, DoubleBussesVariable, IntBussesVariable, ShortBussesVariable, Int24BussesVariable, LongBussesVariable, 
    // breakpoints
        FloatBreakpointsVariable, DoubleBreakpointsVariable, IntBreakpointsVariable, ShortBreakpointsVariable, Int24BreakpointsVariable, LongBreakpointsVariable,
    // wavetables
        FloatWavetableVariable, DoubleWavetableVariable, IntWavetableVariable, ShortWavetableVariable, Int24WavetableVariable, LongWavetableVariable,
    // signals
        FloatSignalVariable, DoubleSignalVariable, IntSignalVariable, ShortSignalVariable, Int24SignalVariable, LongSignalVariable,

        AudioFileReader,
        
    // count (??)
        NumTypeCodes
    };    
    
    /** Returns a type code as a text string. */
    static inline plonk::Text getName (const int code) throw()
    {
        static const char* names[] = {
            "Unknown", "Dynamic",
            
            "Float", "Double", "Int", "Short", "Int24", "Long", "LongLong", "Char", "Bool",
            
            "FixI8F8", "FixI8F24", "FixI16F16",
            
            "AtomicFloat", "AtomicDouble", 
            "AtomicInt", "AtomicLong", "AtomicLongLong", "AtomicPointer", "AtomicExtendedPointer", "AtomicDynamicPointer",
            
            "FloatVariable", "DoubleVariable", 
            "IntVariable", "ShortVariable", "Int24Variable", "LongVariable", "CharVariable", "BoolVariable",
            
            "AtomicFloatVariable", "AtomicDoubleVariable", "AtomicIntVariable", "AtomicLongVariable",
            "AtomicPointerVariable", "AtomicExtendedPointerVariable", "AtomicDynamicPointerVariable",
            
            "FloatArray", "DoubleArray", "IntArray", "ShortArray", "Int24Array", "LongArray", "CharArray", "BoolArray", "DynamicArray",
            "Text", "TextArray", 

            "FloatChannel", "DoubleChannel", "IntChannel", "ShortChannel", "Int24Channel", "LongChannel",

            "FloatUnit", "DoubleUnit", "IntUnit", "ShortUnit", "Int24Unit", "LongUnit",

            "FloatBus", "DoubleBus", "IntBus", "ShortBus", "Int24Bus", "LongBus",
            
            "FloatUnits", "DoubleUnits", "IntUnits", "ShortUnits", "Int24Units", "LongUnits",
            
            "FloatBusses", "DoubleBusses", "IntBusses", "ShortBusses", "Int24Busses", "LongBusses",
            
            "FloatBreakpoints", "DoubleBreakpoints", "IntBreakpoints", "ShortBreakpoints", "Int24Breakpoints", "LongBreakpoints",
            
            "FloatWavetable", "DoubleWavetable", "IntWavetable", "ShortWavetable", "Int24Wavetable", "LongWavetable",
            
            "FloatSignal", "DoubleSignal", "IntSignal", "ShortSignal", "Int24Signal", "LongSignal",
            
            "FloatArrayVariable", "DoubleArrayVariable", "IntArrayVariable", "ShortArrayVariable", 
            "Int24ArrayVariable", "LongArrayVariable", "CharArrayVariable", "BoolArrayVariable",

            "TextVariable", "TextArrayVariable",

            "FloatChannelVariable", "DoubleChannelVariable", "IntChannelVariable", "ShortChannelVariable", "Int24ChannelVariable", "LongChannelVariable",

            "FloatUnitVariable", "DoubleUnitVariable", "IntUnitVariable", "ShortUnitVariable", "Int24UnitVariable", "LongUnitVariable",

            "FloatBusVariable", "DoubleBusVariable", "IntBusVariable", "ShortBusVariable", "Int24BusVariable", "LongBusVariable", 

            "FloatUnitsVariable", "DoubleUnitsVariable", "IntUnitsVariable", "ShortUnitsVariable", "Int24UnitsVariable", "LongUnitsVariable",

            "FloatBussesVariable", "DoubleBussesVariable", "IntBussesVariable", "ShortBussesVariable", "Int24BussesVariable", "LongBussesVariable", 

            "FloatBreakpointsVariable", "DoubleBreakpointsVariable", "IntBreakpointsVariable", "ShortBreakpointsVariable", "Int24BreakpointsVariable", "LongBreakpointsVariable",

            "FloatWavetableVariable", "DoubleWavetableVariable", "IntWavetableVariable", "ShortWavetableVariable", "Int24WavetableVariable", "LongWavetableVariable",

            "FloatSignalVariable", "DoubleSignalVariable", "IntSignalVariable", "ShortSignalVariable", "Int24SignalVariable", "LongSignalVariable",

            "AudioFileReader"
        };
        
        if ((code >= 0) && (code < TypeCode::NumTypeCodes))
            return names[code];
        else 
            return names[0];
    }
    
    /** Used for sample types to determine the minimum / maximum peaks.
     Most types return 0 to indicate that this is not useful information. */
    static inline plonk::LongLong getPeak (const int code) throw()
    {
        static const plonk::LongLong peaks[] = {
            0, 0, // invalid
            1, 1, INT_MAX, SHRT_MAX, PLONK_INT24_MAX, LONG_MAX, LONG_LONG_MAX, CHAR_MAX, 1, // built-ins
            1, 1, 1, // fixeds
            1, 1, INT_MAX, LONG_MAX, LONG_LONG_MAX, // atomics
        };
        
        if ((code >= 0) && (code < TypeCode::AtomicLongLong))
            return peaks[code];
        else 
            return peaks[0];
    }
    
    /** Used for sample types to determine the smallest non-zero value.
     For integers this is always 1 so is most useful for floating point types. */
    static inline double getEpsilon (const int code) throw()
    {
        static const double epsilons[] = {
            0, 0, // invalid
            FLT_MIN, DBL_MIN, 1, 1, 1, 1, 1, 1, 1, // built-ins
            1.0 / (1 << 8), 1.0 / (1 << 24), 1.0 / (1 << 16), 
            FLT_MIN, DBL_MIN, 1, 1, 1 // atomics
        };
        
        if ((code >= 0) && (code < TypeCode::AtomicLongLong))
            return epsilons[code];
        else 
            return epsilons[0];
    }
    

    static inline bool isUnknown (const int code) throw()           { return (code == TypeCode::Unknown); }
    static inline bool isDynamic (const int code) throw()           { return (code == TypeCode::Dynamic); }
    
    static inline bool isBuiltIn (const int code) throw()           { return (code >= TypeCode::Float) && (code <= TypeCode::Bool) && code != TypeCode::Int24; }
    static inline bool isFixed (const int code) throw()             { return (code >= TypeCode::FixI8F8) && (code <= TypeCode::FixI16F16); }
    static inline bool isAtomic (const int code) throw()            { return (code >= TypeCode::AtomicFloat) && (code <= TypeCode::AtomicDynamicPointer); }
    static inline bool isVariable (const int code) throw()          { return (code >= TypeCode::FloatVariable) && (code <= TypeCode::BoolVariable); }
    static inline bool isAtomicVariable (const int code) throw()    { return (code >= TypeCode::AtomicFloatVariable) && (code <= TypeCode::AtomicDynamicPointerVariable); } 
    static inline bool isObjectVariable (const int code) throw()    { return (code >= TypeCode::FloatArrayVariable) && (code <= TypeCode::LongSignalVariable); } 
    static inline bool isArray (const int code) throw()             { return (code >= TypeCode::FloatArray) && (code <= TypeCode::TextArray); }
    static inline bool isChannel (const int code) throw()           { return (code >= TypeCode::FloatChannel) && (code <= TypeCode::LongChannel); }
    static inline bool isUnit (const int code) throw()              { return (code >= TypeCode::FloatUnit) && (code <= TypeCode::LongUnit); }
    static inline bool isBus (const int code) throw()               { return (code >= TypeCode::FloatBus) && (code <= TypeCode::LongBus); }
    static inline bool isUnits (const int code) throw()             { return (code >= TypeCode::FloatUnits) && (code <= TypeCode::LongUnits); }
    static inline bool isBusses (const int code) throw()            { return (code >= TypeCode::FloatBusses) && (code <= TypeCode::LongBusses); }
    static inline bool isBreakpoints (const int code) throw()       { return (code >= TypeCode::FloatBreakpoints) && (code <= TypeCode::LongBreakpoints); }
    static inline bool isWavetable (const int code) throw()         { return (code >= TypeCode::FloatWavetable) && (code <= TypeCode::LongWavetable); }
    static inline bool isSignal (const int code) throw()            { return (code >= TypeCode::FloatSignal) && (code <= TypeCode::LongSignal); }
    static inline bool isAudioFileReader (const int code) throw()   { return (code == TypeCode::AudioFileReader); }

    // could replace these later by designing the enum to be bit-mask based
    
    static inline bool isFloatType (const int code) throw()         
    { 
        return (TypeCode::getName (code).indexOf ("Float") == 0) || (TypeCode::getName (code).indexOf ("AtomicFloat") == 0); 
    }
    
    static inline bool isDoubleType (const int code) throw()        
    { 
        return (TypeCode::getName (code).indexOf ("Double") == 0) || (TypeCode::getName (code).indexOf ("AtomicDouble") == 0); 
    }
    
    static inline bool isIntType (const int code) throw()           
    { 
        return ((TypeCode::getName (code).indexOf ("Int") == 0) && (TypeCode::getName (code).indexOf ("Int24") != 0))
                || (TypeCode::getName (code).indexOf ("AtomicInt") == 0); 
    }
    
    static inline bool isShortType (const int code) throw()         { return TypeCode::getName (code).indexOf ("Short") == 0; }
    static inline bool isInt24Type (const int code) throw()         { return TypeCode::getName (code).indexOf ("Int24") == 0; }
    
    static inline bool isLongType (const int code) throw()          
    { 
        return ((TypeCode::getName (code).indexOf ("Long") == 0) && (code != TypeCode::LongLong))
                || (TypeCode::getName (code).indexOf ("AtomicLong") == 0); 
    }
    
    static inline bool isLongLongType (const int code) throw()      { return TypeCode::getName (code).indexOf ("LongLong") == 0; }
    static inline bool isCharType (const int code) throw()          { return TypeCode::getName (code).indexOf ("Char") == 0; }
    static inline bool isBoolType (const int code) throw()          { return TypeCode::getName (code).indexOf ("Bool") == 0; }
    
    static inline bool isFloatOrDoubleType (const int code) throw() { return TypeCode::isFloatType (code) || TypeCode::isDoubleType (code); }
    
    static inline bool isIntegralType (const int code) throw()
    {
        return  TypeCode::isIntType (code)   ||
                TypeCode::isShortType (code) ||
                TypeCode::isInt24Type (code) ||
                TypeCode::isLongType (code)  ||
                (code == TypeCode::LongLong) ||
                TypeCode::isCharType (code)  ||
                TypeCode::isBoolType (code);
    }
};


//------------------------------------------------------------------------------

template<class Type>
class TypeUtilityBase
{
public:
    typedef void*           TypeName;
    typedef TypeName        PeakType;
    typedef double          ScaleType;
    typedef Type            OriginalType;
    typedef Type const&     PassType;
    typedef float           IndexType;
    static inline int getTypeCode()     { return TypeCode::Unknown; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
};

template<class Type>
class TypeUtilityBase<Type&>
{
public:
    typedef void*           TypeName;
    typedef TypeName        PeakType;
    typedef double          ScaleType;
    typedef Type            OriginalType;
    typedef Type const&     PassType;
    typedef float           IndexType;
    static inline int getTypeCode()     { return TypeCode::Unknown; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
};

template<class Type>
class TypeUtilityBase<Type const&>
{
public:
    typedef void*           TypeName;
    typedef TypeName        PeakType;
    typedef double          ScaleType;
    typedef Type            OriginalType;
    typedef Type const&     PassType;
    typedef float           IndexType;
    static inline int getTypeCode()     { return TypeCode::Unknown; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
};


//------------------------------------------------------------------------------


template<>
class TypeUtilityBase<Dynamic>
{
public:
    typedef Dynamic         TypeName;
    typedef Dynamic         OriginalType;
    typedef Dynamic const&  PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Dynamic; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Dynamic>
{
public:
    typedef const Dynamic   TypeName;
    typedef Dynamic         OriginalType;
    typedef Dynamic const&  PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Dynamic; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<float>
{
public:
    typedef float           TypeName;
    typedef float           PeakType;
    typedef float           ScaleType;
    typedef float           OriginalType;
    typedef float           PassType;
    typedef float           IndexType;
    static inline int getTypeCode() { return TypeCode::Float; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
};

template<>
class TypeUtilityBase<double>
{
public:
    typedef double           TypeName;
    typedef double           PeakType;
    typedef double           ScaleType;
    typedef double           OriginalType;
    typedef double           PassType;
    typedef double           IndexType;
    static inline int getTypeCode() { return TypeCode::Double; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
};

template<>
class TypeUtilityBase<int>
{
public:
    typedef int             TypeName;
    typedef int             PeakType;
    typedef double          ScaleType;
    typedef int             OriginalType;
    typedef int             PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Int; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
    typedef unsigned int UnsignedType;
    typedef LongLong WideType;
    typedef UnsignedLongLong UnsignedWideType;

};

template<>
class TypeUtilityBase<short>
{
public:
    typedef short           TypeName;
    typedef short           PeakType;
    typedef float           ScaleType;
    typedef short           OriginalType;
    typedef short           PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Short; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
    typedef unsigned short UnsignedType;
    typedef int WideType;
    typedef unsigned int UnsignedWideType;
};

template<>
class TypeUtilityBase<Int24>
{
public:
    typedef Int24           TypeName;
    typedef int             PeakType;
    typedef float           ScaleType;
    typedef Int24           OriginalType;
    typedef Int24           PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
};

/*
template<>
class TypeUtilityBase<Long>
{
public:
    typedef Long            TypeName;
    typedef Long            OriginalType;
    typedef Long            PassType;
    typedef float           IndexType;
 static inline int  getTypeCode() { return TypeCode::Long; }
 static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
};
*/

template<>
class TypeUtilityBase<LongLong>
{
public:
    typedef LongLong        TypeName;
    typedef LongLong        PeakType;
    typedef double          ScaleType;
    typedef LongLong        OriginalType;
    typedef LongLong        PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::LongLong; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
    typedef UnsignedLongLong UnsignedType;
    typedef struct { LongLong a; LongLong b; } WideType;
    typedef struct { UnsignedLongLong a; UnsignedLongLong b; } UnsignedWideType;
};

template<>
class TypeUtilityBase<char>
{
public:
    typedef char            TypeName;
    typedef char            PeakType;
    typedef float           ScaleType;
    typedef char            OriginalType;
    typedef char            PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Char; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
    typedef unsigned char UnsignedType;
    typedef short WideType;
    typedef unsigned short UnsignedWideType;
};

template<>
class TypeUtilityBase<bool>
{
public:
    typedef bool            TypeName;
    typedef bool            PeakType;
    typedef float           ScaleType;
    typedef bool            OriginalType;
    typedef bool            PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Bool; }
    static inline const OriginalType& getNull() { static OriginalType null (0); return null; }
};

template<>
class TypeUtilityBase<FixI8F8>
{
public:
    typedef FixI8F8           TypeName;
    typedef FixI8F8           PeakType;
    typedef FixI8F8           ScaleType;
    typedef FixI8F8           OriginalType;
    typedef FixI8F8           PassType;
    typedef FixI8F8           IndexType;
    static inline int getTypeCode() { return TypeCode::FixI8F8; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
};

template<>
class TypeUtilityBase<FixI8F24>
{
public:
    typedef FixI8F24           TypeName;
    typedef FixI8F24           PeakType;
    typedef FixI8F24           ScaleType;
    typedef FixI8F24           OriginalType;
    typedef FixI8F24           PassType;
    typedef FixI8F24           IndexType;
    static inline int getTypeCode() { return TypeCode::FixI8F24; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
};

template<>
class TypeUtilityBase<FixI16F16>
{
public:
    typedef FixI16F16           TypeName;
    typedef FixI16F16           PeakType;
    typedef FixI16F16           ScaleType;
    typedef FixI16F16           OriginalType;
    typedef FixI16F16           PassType;
    typedef FixI16F16           IndexType;
    static inline int getTypeCode() { return TypeCode::FixI8F24; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
};

template<>
class TypeUtilityBase<AtomicFloat>
{
public:
    typedef AtomicFloat         TypeName;
    typedef AtomicFloat         OriginalType;
    typedef AtomicFloat const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicFloat; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicFloat>
{
public:
    typedef const AtomicFloat      TypeName;
    typedef AtomicFloat            OriginalType;
    typedef AtomicFloat const&     PassType;
    typedef float IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicFloat; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicDouble>
{
public:
    typedef AtomicDouble            TypeName;
    typedef AtomicDouble            OriginalType;
    typedef AtomicDouble const&     PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDouble; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicDouble>
{
public:
    typedef const AtomicDouble  TypeName;
    typedef AtomicDouble        OriginalType;
    typedef AtomicDouble const& PassType;
    typedef double              IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDouble; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicInt>
{
public:
    typedef AtomicInt           TypeName;
    typedef AtomicInt           OriginalType;
    typedef AtomicInt const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicInt; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicInt>
{
public:
    typedef const AtomicInt     TypeName;
    typedef AtomicInt           OriginalType;
    typedef AtomicInt const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicInt; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

/*
template<>
class TypeUtilityBase<AtomicLong>
{
public:
    typedef AtomicLong          TypeName;
    typedef AtomicLong          OriginalType;
    typedef AtomicLong const&   PassType;
    typedef float               IndexType;
 static inline int  getTypeCode() { return TypeCode::AtomicLong; }
 static inline const OriginalType& getNull() { static OriginalType null; return null; }
};

template<>
class TypeUtilityBase<const AtomicLong>
{
public:
    typedef const AtomicLong    TypeName;
    typedef AtomicLong          OriginalType;
    typedef AtomicLong const&   PassType;
    typedef float               IndexType;
 static inline int  getTypeCode() { return TypeCode::AtomicLong; }
 static inline const OriginalType& getNull() { static OriginalType null; return null; }
};
*/

template<>
class TypeUtilityBase<AtomicLongLong>
{
public:
    typedef AtomicLongLong          TypeName;
    typedef AtomicLongLong          OriginalType;
    typedef AtomicLongLong const&   PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicLongLong; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicLongLong>
{
public:
    typedef const AtomicLongLong    TypeName;
    typedef AtomicLongLong          OriginalType;
    typedef AtomicLongLong const&   PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicLongLong; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicPointer>
{
public:
    typedef AtomicPointer          TypeName;
    typedef AtomicPointer          OriginalType;
    typedef AtomicPointer const&   PassType;
    typedef int                    IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicPointer; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicPointer>
{
public:
    typedef const AtomicPointer    TypeName;
    typedef AtomicPointer          OriginalType;
    typedef AtomicPointer const&   PassType;
    typedef int                    IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicPointer; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicExtendedPointer>
{
public:
    typedef AtomicExtendedPointer          TypeName;
    typedef AtomicExtendedPointer          OriginalType;
    typedef AtomicExtendedPointer const&   PassType;
    typedef int                            IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicExtendedPointer; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicExtendedPointer>
{
public:
    typedef const AtomicExtendedPointer    TypeName;
    typedef AtomicExtendedPointer          OriginalType;
    typedef AtomicExtendedPointer const&   PassType;
    typedef int                            IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicExtendedPointer; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicDynamicPointer>
{
public:
    typedef AtomicDynamicPointer          TypeName;
    typedef AtomicDynamicPointer          OriginalType;
    typedef AtomicDynamicPointer const&   PassType;
    typedef int                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDynamicPointer; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicDynamicPointer>
{
public:
    typedef const AtomicDynamicPointer    TypeName;
    typedef AtomicDynamicPointer          OriginalType;
    typedef AtomicDynamicPointer const&   PassType;
    typedef int                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDynamicPointer; }
    static inline const OriginalType& getNull() { static OriginalType null; return null; }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatVariable>
{
public:
    typedef FloatVariable           TypeName;
    typedef FloatVariable           OriginalType;
    typedef FloatVariable const&    PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatVariable>
{
public:
    typedef const FloatVariable     TypeName;
    typedef FloatVariable           OriginalType;
    typedef FloatVariable const&    PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleVariable>
{
public:
    typedef DoubleVariable          TypeName;
    typedef DoubleVariable          OriginalType;
    typedef DoubleVariable const&   PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleVariable>
{
public:
    typedef const DoubleVariable    TypeName;
    typedef DoubleVariable          OriginalType;
    typedef DoubleVariable const&   PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntVariable>
{
public:
    typedef IntVariable         TypeName;
    typedef IntVariable         OriginalType;
    typedef IntVariable const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntVariable>
{
public:
    typedef const IntVariable   TypeName;
    typedef IntVariable         OriginalType;
    typedef IntVariable const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortVariable>
{
public:
    typedef ShortVariable           TypeName;
    typedef ShortVariable           OriginalType;
    typedef ShortVariable const&    PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortVariable>
{
public:
    typedef const ShortVariable     TypeName;
    typedef ShortVariable           OriginalType;
    typedef ShortVariable const&    PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Variable>
{
public:
    typedef Int24Variable           TypeName;
    typedef Int24Variable           OriginalType;
    typedef Int24Variable const&    PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Variable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Variable>
{
public:
    typedef const Int24Variable     TypeName;
    typedef Int24Variable           OriginalType;
    typedef Int24Variable const&    PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Variable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongVariable>
{
public:
    typedef LongVariable            TypeName;
    typedef LongVariable            OriginalType;
    typedef LongVariable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::LongVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongVariable>
{
public:
    typedef const LongVariable      TypeName;
    typedef LongVariable            OriginalType;
    typedef LongVariable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::LongVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<CharVariable>
{
public:
    typedef CharVariable            TypeName;
    typedef CharVariable            OriginalType;
    typedef CharVariable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::CharVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const CharVariable>
{
public:
    typedef const CharVariable      TypeName;
    typedef CharVariable            OriginalType;
    typedef CharVariable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::CharVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<BoolVariable>
{
public:
    typedef BoolVariable            TypeName;
    typedef BoolVariable            OriginalType;
    typedef BoolVariable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::BoolVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const BoolVariable>
{
public:
    typedef const BoolVariable      TypeName;
    typedef BoolVariable            OriginalType;
    typedef BoolVariable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::BoolVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicFloatVariable>
{
public:
    typedef AtomicFloatVariable             TypeName;
    typedef AtomicFloatVariable             OriginalType;
    typedef AtomicFloatVariable const&      PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicFloatVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicFloatVariable>
{
public:
    typedef const AtomicFloatVariable       TypeName;
    typedef AtomicFloatVariable             OriginalType;
    typedef AtomicFloatVariable const&      PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicFloatVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicDoubleVariable>
{
public:
    typedef AtomicDoubleVariable            TypeName;
    typedef AtomicDoubleVariable            OriginalType;
    typedef AtomicDoubleVariable const&     PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDoubleVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicDoubleVariable>
{
public:
    typedef const AtomicDoubleVariable      TypeName;
    typedef AtomicDoubleVariable            OriginalType;
    typedef AtomicDoubleVariable const&     PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDoubleVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicIntVariable>
{
public:
    typedef AtomicIntVariable               TypeName;
    typedef AtomicIntVariable               OriginalType;
    typedef AtomicIntVariable const&        PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicIntVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicIntVariable>
{
public:
    typedef const AtomicIntVariable         TypeName;
    typedef AtomicIntVariable               OriginalType;
    typedef AtomicIntVariable const&        PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicIntVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicLongVariable>
{
public:
    typedef AtomicLongVariable              TypeName;
    typedef AtomicLongVariable              OriginalType;
    typedef AtomicLongVariable const&       PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicLongVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicLongVariable>
{
public:
    typedef const AtomicLongVariable        TypeName;
    typedef AtomicLongVariable              OriginalType;
    typedef AtomicLongVariable const&       PassType;
    typedef float IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicLongVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicPointerVariable>
{
public:
    typedef AtomicPointerVariable              TypeName;
    typedef AtomicPointerVariable              OriginalType;
    typedef AtomicPointerVariable const&       PassType;
    typedef int                                IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicPointerVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicPointerVariable>
{
public:
    typedef const AtomicPointerVariable        TypeName;
    typedef AtomicPointerVariable              OriginalType;
    typedef AtomicPointerVariable const&       PassType;
    typedef int                                IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicPointerVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicExtendedPointerVariable>
{
public:
    typedef AtomicExtendedPointerVariable              TypeName;
    typedef AtomicExtendedPointerVariable              OriginalType;
    typedef AtomicExtendedPointerVariable const&       PassType;
    typedef int                                        IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicExtendedPointerVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicExtendedPointerVariable>
{
public:
    typedef const AtomicExtendedPointerVariable        TypeName;
    typedef AtomicExtendedPointerVariable              OriginalType;
    typedef AtomicExtendedPointerVariable const&       PassType;
    typedef int                                        IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicExtendedPointerVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AtomicDynamicPointerVariable>
{
public:
    typedef AtomicDynamicPointerVariable              TypeName;
    typedef AtomicDynamicPointerVariable              OriginalType;
    typedef AtomicDynamicPointerVariable const&       PassType;
    typedef int                                       IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDynamicPointerVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AtomicDynamicPointerVariable>
{
public:
    typedef const AtomicDynamicPointerVariable        TypeName;
    typedef AtomicDynamicPointerVariable              OriginalType;
    typedef AtomicDynamicPointerVariable const&       PassType;
    typedef int                                       IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicDynamicPointerVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

//------------------------------------------------------------------------------

template<>
class TypeUtilityBase<FloatArray>
{
public:
    typedef FloatArray              TypeName;
    typedef FloatArray              OriginalType;
    typedef FloatArray const&       PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatArray>
{
public:
    typedef const FloatArray        TypeName;
    typedef FloatArray              OriginalType;
    typedef FloatArray const&       PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleArray>
{
public:
    typedef DoubleArray             TypeName;
    typedef DoubleArray             OriginalType;
    typedef DoubleArray const&      PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleArray>
{
public:
    typedef const DoubleArray       TypeName;
    typedef DoubleArray             OriginalType;
    typedef DoubleArray const&      PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntArray>
{
public:
    typedef IntArray            TypeName;
    typedef IntArray            OriginalType;
    typedef IntArray const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntArray>
{
public:
    typedef const IntArray      TypeName;
    typedef IntArray            OriginalType;
    typedef IntArray const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortArray>
{
public:
    typedef ShortArray          TypeName;
    typedef ShortArray          OriginalType;
    typedef ShortArray const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortArray>
{
public:
    typedef const ShortArray    TypeName;
    typedef ShortArray          OriginalType;
    typedef ShortArray const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Array>
{
public:
    typedef Int24Array          TypeName;
    typedef Int24Array          OriginalType;
    typedef Int24Array const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Array; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Array>
{
public:
    typedef const Int24Array    TypeName;
    typedef Int24Array          OriginalType;
    typedef Int24Array const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Array; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongArray>
{
public:
    typedef LongArray           TypeName;
    typedef LongArray           OriginalType;
    typedef LongArray const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongArray>
{
public:
    typedef const LongArray     TypeName;
    typedef LongArray           OriginalType;
    typedef LongArray const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<CharArray>
{
public:
    typedef CharArray           TypeName;
    typedef CharArray           OriginalType;
    typedef CharArray const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::CharArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const CharArray>
{
public:
    typedef const CharArray     TypeName;
    typedef CharArray           OriginalType;
    typedef CharArray const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::CharArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<BoolArray>
{
public:
    typedef BoolArray           TypeName;
    typedef BoolArray           OriginalType;
    typedef BoolArray const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::BoolArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const BoolArray>
{
public:
    typedef const BoolArray     TypeName;
    typedef BoolArray           OriginalType;
    typedef BoolArray const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::BoolArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DynamicArray>
{
public:
    typedef DynamicArray           TypeName;
    typedef DynamicArray           OriginalType;
    typedef DynamicArray const&    PassType;
    typedef float                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DynamicArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DynamicArray>
{
public:
    typedef const DynamicArray     TypeName;
    typedef DynamicArray           OriginalType;
    typedef DynamicArray const&    PassType;
    typedef float                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DynamicArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<plonk::Text>
{
public:
    typedef plonk::Text         TypeName;
    typedef plonk::Text         OriginalType;
    typedef plonk::Text const&  PassType;
    typedef int                 IndexType;
    static inline int  getTypeCode() { return TypeCode::Text; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const plonk::Text>
{
public:
    typedef const plonk::Text   TypeName;
    typedef plonk::Text         OriginalType;
    typedef plonk::Text const&  PassType;
    typedef int                 IndexType;
    static inline int  getTypeCode() { return TypeCode::Text; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<TextArray>
{
public:
    typedef TextArray           TypeName;
    typedef TextArray           OriginalType;
    typedef TextArray const&    PassType;
    typedef int                 IndexType;
    static inline int  getTypeCode() { return TypeCode::TextArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const TextArray>
{
public:
    typedef const TextArray     TypeName;
    typedef TextArray           OriginalType;
    typedef TextArray const&    PassType;
    typedef int                 IndexType;
    static inline int  getTypeCode() { return TypeCode::TextArray; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatChannel>
{
public:
    typedef FloatChannel        TypeName;
    typedef FloatChannel        OriginalType;
    typedef FloatChannel const& PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatChannel>
{
public:
    typedef const FloatChannel  TypeName;
    typedef FloatChannel        OriginalType;
    typedef FloatChannel const& PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleChannel>
{
public:
    typedef DoubleChannel           TypeName;
    typedef DoubleChannel           OriginalType;
    typedef DoubleChannel const&    PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleChannel>
{
public:
    typedef const DoubleChannel     TypeName;
    typedef DoubleChannel           OriginalType;
    typedef DoubleChannel const&    PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntChannel>
{
public:
    typedef IntChannel              TypeName;
    typedef IntChannel              OriginalType;
    typedef IntChannel const&       PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::IntChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntChannel>
{
public:
    typedef const IntChannel        TypeName;
    typedef IntChannel              OriginalType;
    typedef IntChannel const&       PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::IntChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortChannel>
{
public:
    typedef ShortChannel            TypeName;
    typedef ShortChannel            OriginalType;
    typedef ShortChannel const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortChannel>
{
public:
    typedef const ShortChannel      TypeName;
    typedef ShortChannel            OriginalType;
    typedef ShortChannel const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Channel>
{
public:
    typedef Int24Channel            TypeName;
    typedef Int24Channel            OriginalType;
    typedef Int24Channel const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Channel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Channel>
{
public:
    typedef const Int24Channel      TypeName;
    typedef Int24Channel            OriginalType;
    typedef Int24Channel const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Channel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongChannel>
{
public:
    typedef LongChannel             TypeName;
    typedef LongChannel             OriginalType;
    typedef LongChannel const&      PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::LongChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongChannel>
{
public:
    typedef const LongChannel       TypeName;
    typedef LongChannel             OriginalType;
    typedef LongChannel const&      PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::LongChannel; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatUnit>
{
public:
    typedef FloatUnit               TypeName;
    typedef FloatUnit               OriginalType;
    typedef FloatUnit const&        PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatUnit>
{
public:
    typedef const FloatUnit         TypeName;
    typedef FloatUnit               OriginalType;
    typedef FloatUnit const&        PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleUnit>
{
public:
    typedef DoubleUnit              TypeName;
    typedef DoubleUnit              OriginalType;
    typedef DoubleUnit const&       PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleUnit>
{
public:
    typedef const DoubleUnit        TypeName;
    typedef DoubleUnit              OriginalType;
    typedef DoubleUnit const&       PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntUnit>
{
public:
    typedef IntUnit             TypeName;
    typedef IntUnit             OriginalType;
    typedef IntUnit const&      PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntUnit>
{
public:
    typedef const IntUnit       TypeName;
    typedef IntUnit             OriginalType;
    typedef IntUnit const&      PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortUnit>
{
public:
    typedef ShortUnit           TypeName;
    typedef ShortUnit           OriginalType;
    typedef ShortUnit const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortUnit>
{
public:
    typedef const ShortUnit     TypeName;
    typedef ShortUnit           OriginalType;
    typedef ShortUnit const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Unit>
{
public:
    typedef Int24Unit           TypeName;
    typedef Int24Unit           OriginalType;
    typedef Int24Unit const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Unit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Unit>
{
public:
    typedef const Int24Unit     TypeName;
    typedef Int24Unit           OriginalType;
    typedef Int24Unit const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Unit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongUnit>
{
public:
    typedef LongUnit            TypeName;
    typedef LongUnit            OriginalType;
    typedef LongUnit const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongUnit>
{
public:
    typedef const LongUnit      TypeName;
    typedef LongUnit            OriginalType;
    typedef LongUnit const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnit; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatBus>
{
public:
    typedef FloatBus            TypeName;
    typedef FloatBus            OriginalType;
    typedef FloatBus const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatBus>
{
public:
    typedef const FloatBus      TypeName;
    typedef FloatBus            OriginalType;
    typedef FloatBus const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleBus>
{
public:
    typedef DoubleBus           TypeName;
    typedef DoubleBus           OriginalType;
    typedef DoubleBus const&    PassType;
    typedef double              IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleBus>
{
public:
    typedef const DoubleBus     TypeName;
    typedef DoubleBus           OriginalType;
    typedef DoubleBus const&    PassType;
    typedef double              IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntBus>
{
public:
    typedef IntBus              TypeName;
    typedef IntBus              OriginalType;
    typedef IntBus const&       PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntBus>
{
public:
    typedef const IntBus        TypeName;
    typedef IntBus              OriginalType;
    typedef IntBus const&       PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortBus>
{
public:
    typedef ShortBus            TypeName;
    typedef ShortBus            OriginalType;
    typedef ShortBus const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortBus>
{
public:
    typedef const ShortBus      TypeName;
    typedef ShortBus            OriginalType;
    typedef ShortBus const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Bus>
{
public:
    typedef Int24Bus            TypeName;
    typedef Int24Bus            OriginalType;
    typedef Int24Bus const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Bus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Bus>
{
public:
    typedef const Int24Bus      TypeName;
    typedef Int24Bus            OriginalType;
    typedef Int24Bus const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Bus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongBus>
{
public:
    typedef LongBus             TypeName;
    typedef LongBus             OriginalType;
    typedef LongBus const&      PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongBus>
{
public:
    typedef const LongBus       TypeName;
    typedef LongBus             OriginalType;
    typedef LongBus const&      PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBus; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatUnits>
{
public:
    typedef FloatUnits          TypeName;
    typedef FloatUnits          OriginalType;
    typedef FloatUnits const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatUnits>
{
public:
    typedef const FloatUnits    TypeName;
    typedef FloatUnits          OriginalType;
    typedef FloatUnits const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleUnits>
{
public:
    typedef DoubleUnits             TypeName;
    typedef DoubleUnits             OriginalType;
    typedef DoubleUnits const&      PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleUnits>
{
public:
    typedef const DoubleUnits       TypeName;
    typedef DoubleUnits             OriginalType;
    typedef DoubleUnits const&      PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntUnits>
{
public:
    typedef IntUnits            TypeName;
    typedef IntUnits            OriginalType;
    typedef IntUnits const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntUnits>
{
public:
    typedef const IntUnits      TypeName;
    typedef IntUnits            OriginalType;
    typedef IntUnits const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortUnits>
{
public:
    typedef ShortUnits          TypeName;
    typedef ShortUnits          OriginalType;
    typedef ShortUnits const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortUnits>
{
public:
    typedef const ShortUnits    TypeName;
    typedef ShortUnits          OriginalType;
    typedef ShortUnits const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Units>
{
public:
    typedef Int24Units          TypeName;
    typedef Int24Units          OriginalType;
    typedef Int24Units const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Units; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Units>
{
public:
    typedef const Int24Units    TypeName;
    typedef Int24Units          OriginalType;
    typedef Int24Units const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Units; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongUnits>
{
public:
    typedef LongUnits           TypeName;
    typedef LongUnits           OriginalType;
    typedef LongUnits const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongUnits>
{
public:
    typedef const LongUnits     TypeName;
    typedef LongUnits           OriginalType;
    typedef LongUnits const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnits; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatBusses>
{
public:
    typedef FloatBusses         TypeName;
    typedef FloatBusses         OriginalType;
    typedef FloatBusses const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatBusses>
{
public:
    typedef const FloatBusses   TypeName;
    typedef FloatBusses         OriginalType;
    typedef FloatBusses const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleBusses>
{
public:
    typedef DoubleBusses TypeName;
    typedef DoubleBusses            OriginalType;
    typedef DoubleBusses const&     PassType;
    typedef double IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleBusses>
{
public:
    typedef const DoubleBusses      TypeName;
    typedef DoubleBusses            OriginalType;
    typedef DoubleBusses const&     PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntBusses>
{
public:
    typedef IntBusses           TypeName;
    typedef IntBusses           OriginalType;
    typedef IntBusses const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntBusses>
{
public:
    typedef const IntBusses     TypeName;
    typedef IntBusses           OriginalType;
    typedef IntBusses const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortBusses>
{
public:
    typedef ShortBusses         TypeName;
    typedef ShortBusses         OriginalType;
    typedef ShortBusses const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortBusses>
{
public:
    typedef const ShortBusses   TypeName;
    typedef ShortBusses         OriginalType;
    typedef ShortBusses const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Busses>
{
public:
    typedef Int24Busses         TypeName;
    typedef Int24Busses         OriginalType;
    typedef Int24Busses const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Busses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Busses>
{
public:
    typedef const Int24Busses   TypeName;
    typedef Int24Busses         OriginalType;
    typedef Int24Busses const&  PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Busses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongBusses>
{
public:
    typedef LongBusses          TypeName;
    typedef LongBusses          OriginalType;
    typedef LongBusses const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongBusses>
{
public:
    typedef const LongBusses    TypeName;
    typedef LongBusses          OriginalType;
    typedef LongBusses const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBusses; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatBreakpoints>
{
public:
    typedef FloatBreakpoints            TypeName;
    typedef FloatBreakpoints            OriginalType;
    typedef FloatBreakpoints const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatBreakpoints>
{
public:
    typedef const FloatBreakpoints      TypeName;
    typedef FloatBreakpoints            OriginalType;
    typedef FloatBreakpoints const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleBreakpoints>
{
public:
    typedef DoubleBreakpoints           TypeName;
    typedef DoubleBreakpoints           OriginalType;
    typedef DoubleBreakpoints const&    PassType;
    typedef double                      IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleBreakpoints>
{
public:
    typedef const DoubleBreakpoints     TypeName;
    typedef DoubleBreakpoints           OriginalType;
    typedef DoubleBreakpoints const&    PassType;
    typedef double                      IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntBreakpoints>
{
public:
    typedef IntBreakpoints              TypeName;
    typedef IntBreakpoints              OriginalType;
    typedef IntBreakpoints const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntBreakpoints>
{
public:
    typedef const IntBreakpoints        TypeName;
    typedef IntBreakpoints              OriginalType;
    typedef IntBreakpoints const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortBreakpoints>
{
public:
    typedef ShortBreakpoints            TypeName;
    typedef ShortBreakpoints            OriginalType;
    typedef ShortBreakpoints const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortBreakpoints>
{
public:
    typedef const ShortBreakpoints      TypeName;
    typedef ShortBreakpoints            OriginalType;
    typedef ShortBreakpoints const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Breakpoints>
{
public:
    typedef Int24Breakpoints            TypeName;
    typedef Int24Breakpoints            OriginalType;
    typedef Int24Breakpoints const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Breakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Breakpoints>
{
public:
    typedef const Int24Breakpoints      TypeName;
    typedef Int24Breakpoints            OriginalType;
    typedef Int24Breakpoints const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Breakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongBreakpoints>
{
public:
    typedef LongBreakpoints             TypeName;
    typedef LongBreakpoints             OriginalType;
    typedef LongBreakpoints const&      PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongBreakpoints>
{
public:
    typedef const LongBreakpoints       TypeName;
    typedef LongBreakpoints             OriginalType;
    typedef LongBreakpoints const&      PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBreakpoints; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatWavetable>
{
public:
    typedef FloatWavetable              TypeName;
    typedef FloatWavetable              OriginalType;
    typedef FloatWavetable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatWavetable>
{
public:
    typedef const FloatWavetable        TypeName;
    typedef FloatWavetable              OriginalType;
    typedef FloatWavetable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleWavetable>
{
public:
    typedef DoubleWavetable             TypeName;
    typedef DoubleWavetable             OriginalType;
    typedef DoubleWavetable const&      PassType;
    typedef double                      IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleWavetable>
{
public:
    typedef const DoubleWavetable       TypeName;
    typedef DoubleWavetable             OriginalType;
    typedef DoubleWavetable const&      PassType;
    typedef double                      IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntWavetable>
{
public:
    typedef IntWavetable            TypeName;
    typedef IntWavetable            OriginalType;
    typedef IntWavetable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::IntWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntWavetable>
{
public:
    typedef const IntWavetable      TypeName;
    typedef IntWavetable            OriginalType;
    typedef IntWavetable const&     PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::IntWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortWavetable>
{
public:
    typedef ShortWavetable              TypeName;
    typedef ShortWavetable              OriginalType;
    typedef ShortWavetable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortWavetable>
{
public:
    typedef const ShortWavetable        TypeName;
    typedef ShortWavetable              OriginalType;
    typedef ShortWavetable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Wavetable>
{
public:
    typedef Int24Wavetable              TypeName;
    typedef Int24Wavetable              OriginalType;
    typedef Int24Wavetable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Wavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Wavetable>
{
public:
    typedef const Int24Wavetable        TypeName;
    typedef Int24Wavetable              OriginalType;
    typedef Int24Wavetable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Wavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongWavetable>
{
public:
    typedef LongWavetable               TypeName;
    typedef LongWavetable               OriginalType;
    typedef LongWavetable const&        PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongWavetable>
{
public:
    typedef const LongWavetable         TypeName;
    typedef LongWavetable               OriginalType;
    typedef LongWavetable const&        PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongWavetable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatSignal>
{
public:
    typedef FloatSignal              TypeName;
    typedef FloatSignal              OriginalType;
    typedef FloatSignal const&       PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatSignal>
{
public:
    typedef const FloatSignal        TypeName;
    typedef FloatSignal              OriginalType;
    typedef FloatSignal const&       PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleSignal>
{
public:
    typedef DoubleSignal             TypeName;
    typedef DoubleSignal             OriginalType;
    typedef DoubleSignal const&      PassType;
    typedef double                   IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleSignal>
{
public:
    typedef const DoubleSignal       TypeName;
    typedef DoubleSignal             OriginalType;
    typedef DoubleSignal const&      PassType;
    typedef double                   IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntSignal>
{
public:
    typedef IntSignal            TypeName;
    typedef IntSignal            OriginalType;
    typedef IntSignal const&     PassType;
    typedef float                IndexType;
    static inline int  getTypeCode() { return TypeCode::IntSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntSignal>
{
public:
    typedef const IntSignal      TypeName;
    typedef IntSignal            OriginalType;
    typedef IntSignal const&     PassType;
    typedef float                IndexType;
    static inline int  getTypeCode() { return TypeCode::IntSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortSignal>
{
public:
    typedef ShortSignal              TypeName;
    typedef ShortSignal              OriginalType;
    typedef ShortSignal const&       PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortSignal>
{
public:
    typedef const ShortSignal        TypeName;
    typedef ShortSignal              OriginalType;
    typedef ShortSignal const&       PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24Signal>
{
public:
    typedef Int24Signal              TypeName;
    typedef Int24Signal              OriginalType;
    typedef Int24Signal const&       PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Signal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24Signal>
{
public:
    typedef const Int24Signal        TypeName;
    typedef Int24Signal              OriginalType;
    typedef Int24Signal const&       PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24Signal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongSignal>
{
public:
    typedef LongSignal               TypeName;
    typedef LongSignal               OriginalType;
    typedef LongSignal const&        PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::LongSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongSignal>
{
public:
    typedef const LongSignal         TypeName;
    typedef LongSignal               OriginalType;
    typedef LongSignal const&        PassType;
    typedef float                    IndexType;
    static inline int  getTypeCode() { return TypeCode::LongSignal; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};


//------------------------------------------------------------------------------

template<>
class TypeUtilityBase<FloatArrayVariable>
{
public:
    typedef FloatArrayVariable              TypeName;
    typedef FloatArrayVariable              OriginalType;
    typedef FloatArrayVariable const&       PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatArrayVariable>
{
public:
    typedef const FloatArrayVariable        TypeName;
    typedef FloatArrayVariable              OriginalType;
    typedef FloatArrayVariable const&       PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};


template<>
class TypeUtilityBase<DoubleArrayVariable>
{
public:
    typedef DoubleArrayVariable             TypeName;
    typedef DoubleArrayVariable             OriginalType;
    typedef DoubleArrayVariable const&      PassType;
    typedef double                  IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleArrayVariable>
{
public:
    typedef const DoubleArrayVariable       TypeName;
    typedef DoubleArrayVariable             OriginalType;
    typedef DoubleArrayVariable const&      PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntArrayVariable>
{
public:
    typedef IntArrayVariable            TypeName;
    typedef IntArrayVariable            OriginalType;
    typedef IntArrayVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntArrayVariable>
{
public:
    typedef const IntArrayVariable      TypeName;
    typedef IntArrayVariable            OriginalType;
    typedef IntArrayVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortArrayVariable>
{
public:
    typedef ShortArrayVariable          TypeName;
    typedef ShortArrayVariable          OriginalType;
    typedef ShortArrayVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortArrayVariable>
{
public:
    typedef const ShortArrayVariable    TypeName;
    typedef ShortArrayVariable          OriginalType;
    typedef ShortArrayVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24ArrayVariable>
{
public:
    typedef Int24ArrayVariable          TypeName;
    typedef Int24ArrayVariable          OriginalType;
    typedef Int24ArrayVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24ArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24ArrayVariable>
{
public:
    typedef const Int24ArrayVariable    TypeName;
    typedef Int24ArrayVariable          OriginalType;
    typedef Int24ArrayVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24ArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongArrayVariable>
{
public:
    typedef LongArrayVariable           TypeName;
    typedef LongArrayVariable           OriginalType;
    typedef LongArrayVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongArrayVariable>
{
public:
    typedef const LongArrayVariable     TypeName;
    typedef LongArrayVariable           OriginalType;
    typedef LongArrayVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<CharArrayVariable>
{
public:
    typedef CharArrayVariable           TypeName;
    typedef CharArrayVariable           OriginalType;
    typedef CharArrayVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::CharArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const CharArrayVariable>
{
public:
    typedef const CharArrayVariable     TypeName;
    typedef CharArrayVariable           OriginalType;
    typedef CharArrayVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::CharArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<BoolArrayVariable>
{
public:
    typedef BoolArrayVariable           TypeName;
    typedef BoolArrayVariable           OriginalType;
    typedef BoolArrayVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::BoolArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const BoolArrayVariable>
{
public:
    typedef const BoolArrayVariable     TypeName;
    typedef BoolArrayVariable           OriginalType;
    typedef BoolArrayVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::BoolArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<plonk::TextVariable>
{
public:
    typedef TextVariable         TypeName;
    typedef TextVariable         OriginalType;
    typedef TextVariable const&  PassType;
    typedef int                  IndexType;
    static inline int  getTypeCode() { return TypeCode::TextVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const plonk::TextVariable>
{
public:
    typedef const TextVariable   TypeName;
    typedef TextVariable         OriginalType;
    typedef TextVariable const&  PassType;
    typedef int                  IndexType;
    static inline int  getTypeCode() { return TypeCode::TextVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<TextArrayVariable>
{
public:
    typedef TextArrayVariable           TypeName;
    typedef TextArrayVariable           OriginalType;
    typedef TextArrayVariable const&    PassType;
    typedef int                         IndexType;
    static inline int  getTypeCode() { return TypeCode::TextArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const TextArrayVariable>
{
public:
    typedef const TextArrayVariable     TypeName;
    typedef TextArrayVariable           OriginalType;
    typedef TextArrayVariable const&    PassType;
    typedef int                         IndexType;
    static inline int  getTypeCode() { return TypeCode::TextArrayVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatChannelVariable>
{
public:
    typedef FloatChannelVariable        TypeName;
    typedef FloatChannelVariable        OriginalType;
    typedef FloatChannelVariable const& PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatChannelVariable>
{
public:
    typedef const FloatChannelVariable  TypeName;
    typedef FloatChannelVariable        OriginalType;
    typedef FloatChannelVariable const& PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleChannelVariable>
{
public:
    typedef DoubleChannelVariable           TypeName;
    typedef DoubleChannelVariable           OriginalType;
    typedef DoubleChannelVariable const&    PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleChannelVariable>
{
public:
    typedef const DoubleChannelVariable     TypeName;
    typedef DoubleChannelVariable           OriginalType;
    typedef DoubleChannelVariable const&    PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntChannelVariable>
{
public:
    typedef IntChannelVariable              TypeName;
    typedef IntChannelVariable              OriginalType;
    typedef IntChannelVariable const&       PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::IntChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntChannelVariable>
{
public:
    typedef const IntChannelVariable        TypeName;
    typedef IntChannelVariable              OriginalType;
    typedef IntChannelVariable const&       PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::IntChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortChannelVariable>
{
public:
    typedef ShortChannelVariable            TypeName;
    typedef ShortChannelVariable            OriginalType;
    typedef ShortChannelVariable const&     PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortChannelVariable>
{
public:
    typedef const ShortChannelVariable      TypeName;
    typedef ShortChannelVariable            OriginalType;
    typedef ShortChannelVariable const&     PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24ChannelVariable>
{
public:
    typedef Int24ChannelVariable            TypeName;
    typedef Int24ChannelVariable            OriginalType;
    typedef Int24ChannelVariable const&     PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24ChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24ChannelVariable>
{
public:
    typedef const Int24ChannelVariable      TypeName;
    typedef Int24ChannelVariable            OriginalType;
    typedef Int24ChannelVariable const&     PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24ChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongChannelVariable>
{
public:
    typedef LongChannelVariable             TypeName;
    typedef LongChannelVariable             OriginalType;
    typedef LongChannelVariable const&      PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::LongChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongChannelVariable>
{
public:
    typedef const LongChannelVariable       TypeName;
    typedef LongChannelVariable             OriginalType;
    typedef LongChannelVariable const&      PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::LongChannelVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatUnitVariable>
{
public:
    typedef FloatUnitVariable               TypeName;
    typedef FloatUnitVariable               OriginalType;
    typedef FloatUnitVariable const&        PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatUnitVariable>
{
public:
    typedef const FloatUnitVariable         TypeName;
    typedef FloatUnitVariable               OriginalType;
    typedef FloatUnitVariable const&        PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleUnitVariable>
{
public:
    typedef DoubleUnitVariable              TypeName;
    typedef DoubleUnitVariable              OriginalType;
    typedef DoubleUnitVariable const&       PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleUnitVariable>
{
public:
    typedef const DoubleUnitVariable        TypeName;
    typedef DoubleUnitVariable              OriginalType;
    typedef DoubleUnitVariable const&       PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntUnitVariable>
{
public:
    typedef IntUnitVariable             TypeName;
    typedef IntUnitVariable             OriginalType;
    typedef IntUnitVariable const&      PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntUnitVariable>
{
public:
    typedef const IntUnitVariable       TypeName;
    typedef IntUnitVariable             OriginalType;
    typedef IntUnitVariable const&      PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortUnitVariable>
{
public:
    typedef ShortUnitVariable           TypeName;
    typedef ShortUnitVariable           OriginalType;
    typedef ShortUnitVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortUnitVariable>
{
public:
    typedef const ShortUnitVariable     TypeName;
    typedef ShortUnitVariable           OriginalType;
    typedef ShortUnitVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24UnitVariable>
{
public:
    typedef Int24UnitVariable           TypeName;
    typedef Int24UnitVariable           OriginalType;
    typedef Int24UnitVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24UnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24UnitVariable>
{
public:
    typedef const Int24UnitVariable     TypeName;
    typedef Int24UnitVariable           OriginalType;
    typedef Int24UnitVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24UnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongUnitVariable>
{
public:
    typedef LongUnitVariable            TypeName;
    typedef LongUnitVariable            OriginalType;
    typedef LongUnitVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongUnitVariable>
{
public:
    typedef const LongUnitVariable      TypeName;
    typedef LongUnitVariable            OriginalType;
    typedef LongUnitVariable const&     PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnitVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatBusVariable>
{
public:
    typedef FloatBusVariable            TypeName;
    typedef FloatBusVariable            OriginalType;
    typedef FloatBusVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatBusVariable>
{
public:
    typedef const FloatBusVariable      TypeName;
    typedef FloatBusVariable            OriginalType;
    typedef FloatBusVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleBusVariable>
{
public:
    typedef DoubleBusVariable           TypeName;
    typedef DoubleBusVariable           OriginalType;
    typedef DoubleBusVariable const&    PassType;
    typedef double                      IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleBusVariable>
{
public:
    typedef const DoubleBusVariable     TypeName;
    typedef DoubleBusVariable           OriginalType;
    typedef DoubleBusVariable const&    PassType;
    typedef double                      IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntBusVariable>
{
public:
    typedef IntBusVariable              TypeName;
    typedef IntBusVariable              OriginalType;
    typedef IntBusVariable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntBusVariable>
{
public:
    typedef const IntBusVariable        TypeName;
    typedef IntBusVariable              OriginalType;
    typedef IntBusVariable const&       PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortBusVariable>
{
public:
    typedef ShortBusVariable            TypeName;
    typedef ShortBusVariable            OriginalType;
    typedef ShortBusVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortBusVariable>
{
public:
    typedef const ShortBusVariable      TypeName;
    typedef ShortBusVariable            OriginalType;
    typedef ShortBusVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24BusVariable>
{
public:
    typedef Int24BusVariable            TypeName;
    typedef Int24BusVariable            OriginalType;
    typedef Int24BusVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24BusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24BusVariable>
{
public:
    typedef const Int24BusVariable      TypeName;
    typedef Int24BusVariable            OriginalType;
    typedef Int24BusVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24BusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongBusVariable>
{
public:
    typedef LongBusVariable             TypeName;
    typedef LongBusVariable             OriginalType;
    typedef LongBusVariable const&      PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongBusVariable>
{
public:
    typedef const LongBusVariable       TypeName;
    typedef LongBusVariable             OriginalType;
    typedef LongBusVariable const&      PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBusVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatUnitsVariable>
{
public:
    typedef FloatUnitsVariable          TypeName;
    typedef FloatUnitsVariable          OriginalType;
    typedef FloatUnitsVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatUnitsVariable>
{
public:
    typedef const FloatUnitsVariable    TypeName;
    typedef FloatUnitsVariable          OriginalType;
    typedef FloatUnitsVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleUnitsVariable>
{
public:
    typedef DoubleUnitsVariable             TypeName;
    typedef DoubleUnitsVariable             OriginalType;
    typedef DoubleUnitsVariable const&      PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleUnitsVariable>
{
public:
    typedef const DoubleUnitsVariable       TypeName;
    typedef DoubleUnitsVariable             OriginalType;
    typedef DoubleUnitsVariable const&      PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntUnitsVariable>
{
public:
    typedef IntUnitsVariable            TypeName;
    typedef IntUnitsVariable            OriginalType;
    typedef IntUnitsVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntUnitsVariable>
{
public:
    typedef const IntUnitsVariable      TypeName;
    typedef IntUnitsVariable            OriginalType;
    typedef IntUnitsVariable const&     PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortUnitsVariable>
{
public:
    typedef ShortUnitsVariable          TypeName;
    typedef ShortUnitsVariable          OriginalType;
    typedef ShortUnitsVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortUnitsVariable>
{
public:
    typedef const ShortUnitsVariable    TypeName;
    typedef ShortUnitsVariable          OriginalType;
    typedef ShortUnitsVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24UnitsVariable>
{
public:
    typedef Int24UnitsVariable          TypeName;
    typedef Int24UnitsVariable          OriginalType;
    typedef Int24UnitsVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24UnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24UnitsVariable>
{
public:
    typedef const Int24UnitsVariable    TypeName;
    typedef Int24UnitsVariable          OriginalType;
    typedef Int24UnitsVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24UnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongUnitsVariable>
{
public:
    typedef LongUnitsVariable           TypeName;
    typedef LongUnitsVariable           OriginalType;
    typedef LongUnitsVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongUnitsVariable>
{
public:
    typedef const LongUnitsVariable     TypeName;
    typedef LongUnitsVariable           OriginalType;
    typedef LongUnitsVariable const&    PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongUnitsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatBussesVariable>
{
public:
    typedef FloatBussesVariable         TypeName;
    typedef FloatBussesVariable         OriginalType;
    typedef FloatBussesVariable const&  PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatBussesVariable>
{
public:
    typedef const FloatBussesVariable   TypeName;
    typedef FloatBussesVariable         OriginalType;
    typedef FloatBussesVariable const&  PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleBussesVariable>
{
public:
    typedef DoubleBussesVariable TypeName;
    typedef DoubleBussesVariable            OriginalType;
    typedef DoubleBussesVariable const&     PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleBussesVariable>
{
public:
    typedef const DoubleBussesVariable      TypeName;
    typedef DoubleBussesVariable            OriginalType;
    typedef DoubleBussesVariable const&     PassType;
    typedef double                          IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntBussesVariable>
{
public:
    typedef IntBussesVariable           TypeName;
    typedef IntBussesVariable           OriginalType;
    typedef IntBussesVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntBussesVariable>
{
public:
    typedef const IntBussesVariable     TypeName;
    typedef IntBussesVariable           OriginalType;
    typedef IntBussesVariable const&    PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortBussesVariable>
{
public:
    typedef ShortBussesVariable         TypeName;
    typedef ShortBussesVariable         OriginalType;
    typedef ShortBussesVariable const&  PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortBussesVariable>
{
public:
    typedef const ShortBussesVariable   TypeName;
    typedef ShortBussesVariable         OriginalType;
    typedef ShortBussesVariable const&  PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24BussesVariable>
{
public:
    typedef Int24BussesVariable         TypeName;
    typedef Int24BussesVariable         OriginalType;
    typedef Int24BussesVariable const&  PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24BussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24BussesVariable>
{
public:
    typedef const Int24BussesVariable   TypeName;
    typedef Int24BussesVariable         OriginalType;
    typedef Int24BussesVariable const&  PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24BussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongBussesVariable>
{
public:
    typedef LongBussesVariable          TypeName;
    typedef LongBussesVariable          OriginalType;
    typedef LongBussesVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongBussesVariable>
{
public:
    typedef const LongBussesVariable    TypeName;
    typedef LongBussesVariable          OriginalType;
    typedef LongBussesVariable const&   PassType;
    typedef float                       IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBussesVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatBreakpointsVariable>
{
public:
    typedef FloatBreakpointsVariable            TypeName;
    typedef FloatBreakpointsVariable            OriginalType;
    typedef FloatBreakpointsVariable const&     PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatBreakpointsVariable>
{
public:
    typedef const FloatBreakpointsVariable      TypeName;
    typedef FloatBreakpointsVariable            OriginalType;
    typedef FloatBreakpointsVariable const&     PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleBreakpointsVariable>
{
public:
    typedef DoubleBreakpointsVariable           TypeName;
    typedef DoubleBreakpointsVariable           OriginalType;
    typedef DoubleBreakpointsVariable const&    PassType;
    typedef double                              IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleBreakpointsVariable>
{
public:
    typedef const DoubleBreakpointsVariable     TypeName;
    typedef DoubleBreakpointsVariable           OriginalType;
    typedef DoubleBreakpointsVariable const&    PassType;
    typedef double                              IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntBreakpointsVariable>
{
public:
    typedef IntBreakpointsVariable              TypeName;
    typedef IntBreakpointsVariable              OriginalType;
    typedef IntBreakpointsVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntBreakpointsVariable>
{
public:
    typedef const IntBreakpointsVariable        TypeName;
    typedef IntBreakpointsVariable              OriginalType;
    typedef IntBreakpointsVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::IntBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortBreakpointsVariable>
{
public:
    typedef ShortBreakpointsVariable            TypeName;
    typedef ShortBreakpointsVariable            OriginalType;
    typedef ShortBreakpointsVariable const&     PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortBreakpointsVariable>
{
public:
    typedef const ShortBreakpointsVariable      TypeName;
    typedef ShortBreakpointsVariable            OriginalType;
    typedef ShortBreakpointsVariable const&     PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24BreakpointsVariable>
{
public:
    typedef Int24BreakpointsVariable            TypeName;
    typedef Int24BreakpointsVariable            OriginalType;
    typedef Int24BreakpointsVariable const&     PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24BreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24BreakpointsVariable>
{
public:
    typedef const Int24BreakpointsVariable      TypeName;
    typedef Int24BreakpointsVariable            OriginalType;
    typedef Int24BreakpointsVariable const&     PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24BreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongBreakpointsVariable>
{
public:
    typedef LongBreakpointsVariable             TypeName;
    typedef LongBreakpointsVariable             OriginalType;
    typedef LongBreakpointsVariable const&      PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongBreakpointsVariable>
{
public:
    typedef const LongBreakpointsVariable       TypeName;
    typedef LongBreakpointsVariable             OriginalType;
    typedef LongBreakpointsVariable const&      PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongBreakpointsVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatWavetableVariable>
{
public:
    typedef FloatWavetableVariable              TypeName;
    typedef FloatWavetableVariable              OriginalType;
    typedef FloatWavetableVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatWavetableVariable>
{
public:
    typedef const FloatWavetableVariable        TypeName;
    typedef FloatWavetableVariable              OriginalType;
    typedef FloatWavetableVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleWavetableVariable>
{
public:
    typedef DoubleWavetableVariable             TypeName;
    typedef DoubleWavetableVariable             OriginalType;
    typedef DoubleWavetableVariable const&      PassType;
    typedef double                              IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleWavetableVariable>
{
public:
    typedef const DoubleWavetableVariable       TypeName;
    typedef DoubleWavetableVariable             OriginalType;
    typedef DoubleWavetableVariable const&      PassType;
    typedef double                              IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntWavetableVariable>
{
public:
    typedef IntWavetableVariable            TypeName;
    typedef IntWavetableVariable            OriginalType;
    typedef IntWavetableVariable const&     PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::IntWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntWavetableVariable>
{
public:
    typedef const IntWavetableVariable      TypeName;
    typedef IntWavetableVariable            OriginalType;
    typedef IntWavetableVariable const&     PassType;
    typedef float                           IndexType;
    static inline int  getTypeCode() { return TypeCode::IntWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortWavetableVariable>
{
public:
    typedef ShortWavetableVariable              TypeName;
    typedef ShortWavetableVariable              OriginalType;
    typedef ShortWavetableVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortWavetableVariable>
{
public:
    typedef const ShortWavetableVariable        TypeName;
    typedef ShortWavetableVariable              OriginalType;
    typedef ShortWavetableVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24WavetableVariable>
{
public:
    typedef Int24WavetableVariable              TypeName;
    typedef Int24WavetableVariable              OriginalType;
    typedef Int24WavetableVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24WavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24WavetableVariable>
{
public:
    typedef const Int24WavetableVariable        TypeName;
    typedef Int24WavetableVariable              OriginalType;
    typedef Int24WavetableVariable const&       PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24WavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongWavetableVariable>
{
public:
    typedef LongWavetableVariable               TypeName;
    typedef LongWavetableVariable               OriginalType;
    typedef LongWavetableVariable const&        PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongWavetableVariable>
{
public:
    typedef const LongWavetableVariable         TypeName;
    typedef LongWavetableVariable               OriginalType;
    typedef LongWavetableVariable const&        PassType;
    typedef float                               IndexType;
    static inline int  getTypeCode() { return TypeCode::LongWavetableVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<FloatSignalVariable>
{
public:
    typedef FloatSignalVariable              TypeName;
    typedef FloatSignalVariable              OriginalType;
    typedef FloatSignalVariable const&       PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const FloatSignalVariable>
{
public:
    typedef const FloatSignalVariable        TypeName;
    typedef FloatSignalVariable              OriginalType;
    typedef FloatSignalVariable const&       PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<DoubleSignalVariable>
{
public:
    typedef DoubleSignalVariable             TypeName;
    typedef DoubleSignalVariable             OriginalType;
    typedef DoubleSignalVariable const&      PassType;
    typedef double                           IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const DoubleSignalVariable>
{
public:
    typedef const DoubleSignalVariable       TypeName;
    typedef DoubleSignalVariable             OriginalType;
    typedef DoubleSignalVariable const&      PassType;
    typedef double                           IndexType;
    static inline int  getTypeCode() { return TypeCode::DoubleSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<IntSignalVariable>
{
public:
    typedef IntSignalVariable            TypeName;
    typedef IntSignalVariable            OriginalType;
    typedef IntSignalVariable const&     PassType;
    typedef float                        IndexType;
    static inline int  getTypeCode() { return TypeCode::IntSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const IntSignalVariable>
{
public:
    typedef const IntSignalVariable      TypeName;
    typedef IntSignalVariable            OriginalType;
    typedef IntSignalVariable const&     PassType;
    typedef float                        IndexType;
    static inline int  getTypeCode() { return TypeCode::IntSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<ShortSignalVariable>
{
public:
    typedef ShortSignalVariable              TypeName;
    typedef ShortSignalVariable              OriginalType;
    typedef ShortSignalVariable const&       PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const ShortSignalVariable>
{
public:
    typedef const ShortSignalVariable        TypeName;
    typedef ShortSignalVariable              OriginalType;
    typedef ShortSignalVariable const&       PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::ShortSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<Int24SignalVariable>
{
public:
    typedef Int24SignalVariable              TypeName;
    typedef Int24SignalVariable              OriginalType;
    typedef Int24SignalVariable const&       PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24SignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const Int24SignalVariable>
{
public:
    typedef const Int24SignalVariable        TypeName;
    typedef Int24SignalVariable              OriginalType;
    typedef Int24SignalVariable const&       PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24SignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<LongSignalVariable>
{
public:
    typedef LongSignalVariable               TypeName;
    typedef LongSignalVariable               OriginalType;
    typedef LongSignalVariable const&        PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::LongSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const LongSignalVariable>
{
public:
    typedef const LongSignalVariable         TypeName;
    typedef LongSignalVariable               OriginalType;
    typedef LongSignalVariable const&        PassType;
    typedef float                            IndexType;
    static inline int  getTypeCode() { return TypeCode::LongSignalVariable; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<AudioFileReader>
{
public:
    typedef AudioFileReader               TypeName;
    typedef AudioFileReader               OriginalType;
    typedef AudioFileReader const&        PassType;
    typedef void                          IndexType;
    static inline int  getTypeCode() { return TypeCode::AudioFileReader; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};

template<>
class TypeUtilityBase<const AudioFileReader>
{
public:
    typedef const AudioFileReader         TypeName;
    typedef AudioFileReader               OriginalType;
    typedef AudioFileReader const&        PassType;
    typedef void                          IndexType;
    static inline int  getTypeCode() { return TypeCode::AudioFileReader; }
    static inline const OriginalType& getNull() { return TypeUtilityBase<const OriginalType&>::getNull(); }
    typedef int PeakType;
    typedef double ScaleType;
};


//------------------------------------------------------------------------------

/** A utility for finding out information about certain other classes and types.
 @ingroup PlonkOtherUserClasses */
template<class Type>
class TypeUtility : public TypeUtilityBase<Type>
{
public:
    typedef typename TypeUtilityBase<Type>::TypeName        TypeName;
    typedef typename TypeUtilityBase<Type>::PeakType        PeakType;
    typedef typename TypeUtilityBase<Type>::ScaleType       ScaleType;
    typedef typename TypeUtilityBase<Type>::OriginalType    OriginalType;
    typedef typename TypeUtilityBase<Type>::PassType        PassType;
    typedef typename TypeUtilityBase<Type>::IndexType       IndexType;
    
    static inline Text getTypeName()                 { return TypeCode::getName (TypeUtility<Type>::getTypeCode()); }
    static inline const PeakType& getTypePeak()      { static PeakType v = (PeakType)TypeCode::getPeak (TypeUtility<Type>::getTypeCode()); return v; }
    static inline const ScaleType& getTypeScale()    { static ScaleType v = Math<ScaleType>::get1() / ScaleType (getTypePeak()); return v; }
    static inline double getTypeEpsilon()            { return TypeCode::getEpsilon (TypeUtility<Type>::getTypeCode()); }
    static inline Text getIndexTypeName()            { return TypeCode::getName (TypeUtility<IndexType>::getTypeCode()); }

    static inline bool isUnknown() throw()           { return TypeCode::isUnknown (TypeUtility<Type>::getTypeCode()); }
    static inline bool isDynamic() throw()           { return TypeCode::isDynamic (TypeUtility<Type>::getTypeCode()); }    
    static inline bool isBuiltIn() throw()           { return TypeCode::isBuiltIn (TypeUtility<Type>::getTypeCode()); }
    static inline bool isVariable() throw()          { return TypeCode::isVariable (TypeUtility<Type>::getTypeCode()); }
    static inline bool isAtomicVariable() throw()    { return TypeCode::isAtomicVariable (TypeUtility<Type>::getTypeCode()); }
    static inline bool isArray() throw()             { return TypeCode::isArray (TypeUtility<Type>::getTypeCode()); }
    static inline bool isChannel() throw()           { return TypeCode::isChannel (TypeUtility<Type>::getTypeCode()); }
    static inline bool isUnit() throw()              { return TypeCode::isUnit (TypeUtility<Type>::getTypeCode()); }
    static inline bool isBus() throw()               { return TypeCode::isBus (TypeUtility<Type>::getTypeCode()); }
    static inline bool isUnits() throw()             { return TypeCode::isUnits (TypeUtility<Type>::getTypeCode()); }
    static inline bool isBusses() throw()            { return TypeCode::isBusses (TypeUtility<Type>::getTypeCode()); }
    static inline bool isWavetable() throw()         { return TypeCode::isWavetable (TypeUtility<Type>::getTypeCode()); }
    static inline bool isSignal() throw()            { return TypeCode::isSignal (TypeUtility<Type>::getTypeCode()); }
    static inline bool isAudioFileReader() throw()   { return TypeCode::isAudioFileReader (TypeUtility<Type>::getTypeCode()); }
    
    static inline bool isFloatType() throw()         { return TypeCode::isFloatType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isDoubleType() throw()        { return TypeCode::isDoubleType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isIntType() throw()           { return TypeCode::isIntType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isInt24Type() throw()         { return TypeCode::isInt24Type (TypeUtility<Type>::getTypeCode()); }
    static inline bool isLongType() throw()          { return TypeCode::isLongType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isShortType() throw()         { return TypeCode::isShortType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isCharType() throw()          { return TypeCode::isCharType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isBoolType() throw()          { return TypeCode::isBoolType (TypeUtility<Type>::getTypeCode()); }
    
    static inline bool isFloatOrDoubleType() throw() { return TypeCode::isFloatOrDoubleType (TypeUtility<Type>::getTypeCode()); }
    static inline bool isIntegralType() throw()      { return TypeCode::isIntegralType (TypeUtility<Type>::getTypeCode()); }
};




#endif // PLONK_TYPEUTILITY_H
