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
    // atomic types (11)    
        AtomicFloat, AtomicDouble, AtomicInt, AtomicLong, AtomicLongLong, AtomicPointer, AtomicExtendedPointer,
    // variables (18)
        FloatVariable, DoubleVariable, 
        IntVariable, ShortVariable, Int24Variable, LongVariable, CharVariable, BoolVariable,
    // atomic variables (26)
        AtomicFloatVariable, AtomicDoubleVariable, AtomicIntVariable, AtomicLongVariable,
    // arrays (30)
        FloatArray, DoubleArray, IntArray, ShortArray, Int24Array, LongArray, CharArray, BoolArray,
    // other
        Text, TextArray,
    // channels (40)
        FloatChannel, DoubleChannel, IntChannel, ShortChannel, Int24Channel, LongChannel,
    // units (46)
        FloatUnit, DoubleUnit, IntUnit, ShortUnit, Int24Unit, LongUnit,
    // bus (52)
        FloatBus, DoubleBus, IntBus, ShortBus, Int24Bus, LongBus, 
    // unit arrays (58)
        FloatUnits, DoubleUnits, IntUnits, ShortUnits, Int24Units, LongUnits,
    // bus arrays (64)
        FloatBusses, DoubleBusses, IntBusses, ShortBusses, Int24Busses, LongBusses, 
    // breakpoints (70)
        FloatBreakpoints, DoubleBreakpoints, IntBreakpoints, ShortBreakpoints, Int24Breakpoints, LongBreakpoints,
    // breakpoints (76)
        FloatWavetable, DoubleWavetable, IntWavetable, ShortWavetable, Int24Wavetable, LongWavetable,
    // count (82)
        NumTypeCodes
    };    
    
    /** Returns a type code as a text string. */
    static inline plonk::Text getName (const int code) throw()
    {
        static const char* names[] = {
            "Unknown", "Dynamic",
            
            "Float", "Double", "Int", "Short", "Int24", "Long", "LongLong", "Char", "Bool",
            
            "AtomicFloat", "AtomicDouble", "AtomicInt", "AtomicLong", "AtomicLongLong", "AtomicPointer", "AtomicExtendedPointer",
            
            "FloatVariable", "DoubleVariable", 
            "IntVariable", "ShortVariable", "Int24Variable", "LongVariable", "CharVariable", "BoolVariable",
            
            "AtomicFloatVariable", "AtomicDoubleVariable", "AtomicIntVariable", "AtomicLongVariable",
            
            "FloatArray", "DoubleArray", "IntArray", "ShortArray", "Int24Array", "LongArray", "CharArray", "BoolArray",
            "Text", "TextArray", 

            "FloatChannel", "DoubleChannel", "IntChannel", "ShortChannel", "Int24Channel", "LongChannel",

            "FloatUnit", "DoubleUnit", "IntUnit", "ShortUnit", "Int24Unit", "LongUnit",

            "FloatBus", "DoubleBus", "IntBus", "ShortBus", "Int24Bus", "LongBus",
            
            "FloatUnits", "DoubleUnits", "IntUnits", "ShortUnits", "Int24Units", "LongUnits",
            
            "FloatBusses", "DoubleBusses", "IntBusses", "ShortBusses", "Int24Busses", "LongBusses",
            
            "FloatBreakpoints", "DoubleBreakpoints", "IntBreakpoints", "ShortBreakpoints", "Int24Breakpoints", "LongBreakpoints",
            
            "FloatWavetable", "DoubleWavetable", "IntWavetable", "ShortWavetable", "Int24Wavetable", "LongWavetable"
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
    static inline bool isAtomic (const int code) throw()            { return (code >= TypeCode::AtomicFloat) && (code <= TypeCode::AtomicExtendedPointer); }
    static inline bool isVariable (const int code) throw()          { return (code >= TypeCode::FloatVariable) && (code <= TypeCode::BoolVariable); }
    static inline bool isAtomicVariable (const int code) throw()    { return (code >= TypeCode::AtomicFloatVariable) && (code <= TypeCode::AtomicLongVariable); }
    static inline bool isArray (const int code) throw()             { return (code >= TypeCode::FloatArray) && (code <= TypeCode::TextArray); }
    static inline bool isChannel (const int code) throw()           { return (code >= TypeCode::FloatChannel) && (code <= TypeCode::LongChannel); }
    static inline bool isUnit (const int code) throw()              { return (code >= TypeCode::FloatUnit) && (code <= TypeCode::LongUnit); }
    static inline bool isBus (const int code) throw()               { return (code >= TypeCode::FloatBus) && (code <= TypeCode::LongBus); }
    static inline bool isUnits (const int code) throw()             { return (code >= TypeCode::FloatUnits) && (code <= TypeCode::LongUnits); }
    static inline bool isBusses (const int code) throw()            { return (code >= TypeCode::FloatBusses) && (code <= TypeCode::LongBusses); }
    static inline bool isBreakpoints (const int code) throw()       { return (code >= TypeCode::FloatBreakpoints) && (code <= TypeCode::LongBreakpoints); }
    static inline bool isWavetable (const int code) throw()         { return (code >= TypeCode::FloatWavetable) && (code <= TypeCode::LongWavetable); }

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
    typedef Type            OriginalType;
    typedef Type const&     PassType;
    typedef float           IndexType;
    static inline int getTypeCode() { return TypeCode::Unknown; }
};

template<class Type>
class TypeUtilityBase<Type&>
{
public:
    typedef void*           TypeName;
    typedef Type            OriginalType;
    typedef Type const&     PassType;
    typedef float           IndexType;
    static inline int getTypeCode() { return TypeCode::Unknown; }
};

template<class Type>
class TypeUtilityBase<Type const&>
{
public:
    typedef void*           TypeName;
    typedef Type            OriginalType;
    typedef Type const&     PassType;
    typedef float           IndexType;
    static inline int getTypeCode() { return TypeCode::Unknown; }
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
};

template<>
class TypeUtilityBase<float>
{
public:
    typedef float           TypeName;
    typedef float           OriginalType;
    typedef float           PassType;
    typedef float           IndexType;
    static inline int getTypeCode() { return TypeCode::Float; }
};

template<>
class TypeUtilityBase<double>
{
public:
    typedef double           TypeName;
    typedef double           OriginalType;
    typedef double           PassType;
    typedef double           IndexType;
    static inline int getTypeCode() { return TypeCode::Double; }
};

template<>
class TypeUtilityBase<int>
{
public:
    typedef int             TypeName;
    typedef int             OriginalType;
    typedef int             PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Int; }
};

template<>
class TypeUtilityBase<short>
{
public:
    typedef short           TypeName;
    typedef short           OriginalType;
    typedef short           PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Short; }
};

template<>
class TypeUtilityBase<Int24>
{
public:
    typedef Int24           TypeName;
    typedef Int24           OriginalType;
    typedef Int24           PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Int24; }
};

template<>
class TypeUtilityBase<Long>
{
public:
    typedef Long            TypeName;
    typedef Long            OriginalType;
    typedef Long            PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Long; }
};

template<>
class TypeUtilityBase<LongLong>
{
public:
    typedef LongLong        TypeName;
    typedef LongLong        OriginalType;
    typedef LongLong        PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::LongLong; }
};

template<>
class TypeUtilityBase<char>
{
public:
    typedef char            TypeName;
    typedef char            OriginalType;
    typedef char            PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Char; }
};

template<>
class TypeUtilityBase<bool>
{
public:
    typedef bool            TypeName;
    typedef bool            OriginalType;
    typedef bool            PassType;
    typedef float           IndexType;
    static inline int  getTypeCode() { return TypeCode::Bool; }
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
};

template<>
class TypeUtilityBase<AtomicLong>
{
public:
    typedef AtomicLong          TypeName;
    typedef AtomicLong          OriginalType;
    typedef AtomicLong const&   PassType;
    typedef float               IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicLong; }
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
};

template<>
class TypeUtilityBase<AtomicLongLong>
{
public:
    typedef AtomicLongLong          TypeName;
    typedef AtomicLongLong          OriginalType;
    typedef AtomicLongLong const&   PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::AtomicLongLong; }
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
};

template<>
class TypeUtilityBase<FloatArray>
{
public:
    typedef FloatArray              TypeName;
    typedef FloatArray              OriginalType;
    typedef FloatArray const&       PassType;
    typedef float                   IndexType;
    static inline int  getTypeCode() { return TypeCode::FloatArray; }
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
};


//------------------------------------------------------------------------------

/** A utility for finding out information about certain other classes and types.
 @ingroup PlonkOtherUserClasses */
template<class Type>
class TypeUtility : public TypeUtilityBase<Type>
{
public:
    typedef typename TypeUtilityBase<Type>::TypeName        TypeName;
    typedef typename TypeUtilityBase<Type>::OriginalType    OriginalType;
    typedef typename TypeUtilityBase<Type>::PassType        PassType;
    typedef typename TypeUtilityBase<Type>::IndexType       IndexType;
    
    static inline Text getTypeName()                 { return TypeCode::getName (TypeUtility<Type>::getTypeCode()); }
    static inline TypeName getTypePeak()             { return TypeCode::getPeak (TypeUtility<Type>::getTypeCode()); }
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

template<class TypeA, class TypeB>
class BinaryOpTypeUtility
{
public:
    typedef double CalcType;
};

#define PLONK_BINARYOPTYPEUTILITY_DEFINE(TYPEA,TYPEB,CALCTYPE)\
    template<> class BinaryOpTypeUtility<TYPEA,TYPEB>\
    {\
    public:\
        typedef CALCTYPE CalcType;\
    }

PLONK_BINARYOPTYPEUTILITY_DEFINE(float,float,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(float,int,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(int,float,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(float,short,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(short,float,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(float,char,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(char,float,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(float,Long,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(Long,float,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(float,LongLong,float);
PLONK_BINARYOPTYPEUTILITY_DEFINE(LongLong,float,float);

PLONK_BINARYOPTYPEUTILITY_DEFINE(double,double,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(double,int,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(int,double,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(double,short,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(short,double,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(double,char,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(char,double,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(double,Long,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(Long,double,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(double,LongLong,double);
PLONK_BINARYOPTYPEUTILITY_DEFINE(LongLong,double,double);


PLONK_BINARYOPTYPEUTILITY_DEFINE(char,char,short);
PLONK_BINARYOPTYPEUTILITY_DEFINE(char,short,int);
PLONK_BINARYOPTYPEUTILITY_DEFINE(short,char,int);
PLONK_BINARYOPTYPEUTILITY_DEFINE(char,int,LongLong);
PLONK_BINARYOPTYPEUTILITY_DEFINE(int,char,LongLong);

PLONK_BINARYOPTYPEUTILITY_DEFINE(short,short,int);
PLONK_BINARYOPTYPEUTILITY_DEFINE(short,int,LongLong);
PLONK_BINARYOPTYPEUTILITY_DEFINE(int,short,LongLong);

PLONK_BINARYOPTYPEUTILITY_DEFINE(int,int,LongLong);
PLONK_BINARYOPTYPEUTILITY_DEFINE(int,Long,LongLong);
PLONK_BINARYOPTYPEUTILITY_DEFINE(int,LongLong,LongLong);
PLONK_BINARYOPTYPEUTILITY_DEFINE(Long,int,LongLong);
PLONK_BINARYOPTYPEUTILITY_DEFINE(LongLong,int,LongLong);


#endif // PLONK_TYPEUTILITY_H
