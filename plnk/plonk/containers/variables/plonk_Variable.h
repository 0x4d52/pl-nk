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

#ifndef PLONK_VARIABLE_H
#define PLONK_VARIABLE_H

#include "plonk_VariableForwardDeclarations.h"
#include "../plonk_DynamicContainer.h"

#include "../../core/plonk_SmartPointerContainer.h"
#include "../../core/plonk_WeakPointerContainer.h"
#include "../../core/plonk_SenderContainer.h"

#include "../../maths/plonk_InlineUnaryOps.h"
#include "../../maths/plonk_InlineBinaryOps.h"
#include "../../maths/plonk_InlineMiscOps.h"

#include "../../random/plonk_RNG.h"
#include "../plonk_Text.h"

/** A template class for storing any variable type. 
 The main reason for this class is that it is the simplest kind of Sender. 
 Thus you can inhereit from Variable<Type>::Receiver and register this with a 
 Variable<Type> object for updates when it changes. Variables can be constructed 
 into graphs using binary and unary operators and type conversions from one 
 variable types to another. Within a Variable graph each node registers itself 
 as Receiver from its dependent variables such that it can be notified if these 
 change. This changes is trasnmitted all the way through the graph to the root.
 @ingroup PlonkContainerClasses
 */
template<class Type>
class Variable : public SenderContainer< VariableInternalBase<Type> >
{
public:
    typedef VariableInternalBase<Type>      Internal;
    typedef VariableInternal<Type>          VariableInternalType;
    typedef SenderContainer<Internal>       Base;
    typedef ReceiverInternal<Variable>      Receiver;
    typedef Variable                        Sender;
    typedef Variable                        VariableType;
    typedef Variable<VariableType>          Meta;

    typedef WeakPointerContainer<Variable>  Weak;
    typedef UnitBase<Type>                  UnitType;


    
    typedef typename BinaryOpFunctionsHelper<Type>::BinaryOpFunctionsType BinaryOpFunctionsType;
    typedef typename UnaryOpFunctionsHelper<Type>::UnaryOpFunctionsType UnaryOpFunctionsType;

    typedef PatternVariableInternal<Type>   PatternVariableInternalType;
    typedef NumericalArray<Variable>        PatternType;
    
    /** Default constructor. */
    PLONK_INLINE_LOW Variable() throw()
    :   Base (new VariableInternalType (Type()))
    {
    }
    
    /** Initialised with a specific internal. */
    explicit Variable (Internal* internal) throw()
    :   Base (internal)
    {
    }   
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    PLONK_INLINE_LOW Variable (Variable const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    Variable (Dynamic const& other) throw()
    :   Base (other.as<Variable>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    Variable& operator= (Variable const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */
    static Variable fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }
            
    static const Variable& getNull() throw()
	{
		static Variable null;
		return null;
	}	                
    
    /** Initialised with a specific value. */
    PLONK_INLINE_LOW Variable (Type const& initValue) throw()
    :   Base (new VariableInternalType (initValue))
    {
    }
    
    /** Initialised with a pattern/sequence. */
    Variable (PatternType const& pat) throw()
    :   Base (new PatternVariableInternalType (pat))
    {
    }
    
    /** Initialised with a pattern/sequence. */
    template<class OtherType>
    Variable (NumericalArray< Variable<OtherType> > const& pat) throw()
    :   Base (new TypeVariableInternal<Type,OtherType> (Variable<OtherType> (static_cast< VariableInternalBase<OtherType>*> (new PatternVariableInternal<OtherType> (pat)))))
    {
        // what a nasty expression!
    }
    
    /** Effectively a Variable< Variable<Type> >.
     This allows for dynamic repatching of variables at run time. This is really useful for
     audio parameters, especially where you want to change custom mappings or modulators while audio is runnin.
     The ParamUnit in the audio signal path will then smooth out any chnages as necessary. */
    static Variable meta (Meta const& other) throw()
    {
        return Variable (static_cast<Internal*> (new MetaVariableInternal<Type> (other)));
    }
    
    /** Use a variable as an index to lookup a value in an array with no interpolation
     The index is clipped at the bounds of the array
     This is useful for mapping parameter values from one range to another without using trig functions. */
    template<class IndexValueType>
    static Variable map (NumericalArray<Type> const& table, Variable<IndexValueType> const& index) throw()
    {
        typedef InterpNone<Type, IndexValueType> InterpType;
        return Variable (static_cast<Internal*> (new LookupMapVariableInternal<Type, IndexValueType, InterpType> (table, index)));
    }
    
    /** Use a variable as an index to lookup a value in an array with linear interpolation
     The index is clipped at the bounds of the array (taking into account the interpolation scheme).
     This is useful for mapping parameter values from one range to another without using trig functions. */
    template<class IndexValueType>
    static Variable mapLinear (NumericalArray<Type> const& table, Variable<IndexValueType> const& index) throw()
    {
        typedef InterpLinear<Type, IndexValueType> InterpType;
        return Variable (static_cast<Internal*> (new LookupMapVariableInternal<Type, IndexValueType, InterpType> (table, index)));
    }
    
    /** Use a variable as an index to lookup a value in an array with Lagrange 3 interpolation
     The index is clipped at the bounds of the array (taking into account the interpolation scheme).
     This is useful for mapping parameter values from one range to another without using trig functions. */
    template<class IndexValueType>
    static Variable mapLagrange3 (NumericalArray<Type> const& table, Variable<IndexValueType> const& index) throw()
    {
        typedef InterpLagrange3<Type, IndexValueType> InterpType;
        return Variable (static_cast<Internal*> (new LookupMapVariableInternal<Type, IndexValueType, InterpType> (table, index)));
    }
    
    template<class IndexValueType>
    static Variable lookup (NumericalArray<Type> const& table, Variable<IndexValueType> const& index) throw()
    {
        typedef InterpNone<Type, IndexValueType> InterpType;
        return Variable (static_cast<Internal*> (new LookupWaveVariableInternal<Type, IndexValueType, InterpType> (table, index)));
    }
    
    /** Use a variable as an index to lookup a value in an array with linear interpolation
     The index is wrapped at the bounds of the array.
     This is useful for looking up wavetables. */
    template<class IndexValueType>
    static Variable lookupLinear (NumericalArray<Type> const& table, Variable<IndexValueType> const& index) throw()
    {
        typedef InterpLinear<Type, IndexValueType> InterpType;
        return Variable (static_cast<Internal*> (new LookupWaveVariableInternal<Type, IndexValueType, InterpType> (table, index)));
    }
    
    template<class IndexValueType>
    static Variable lookupLagrange3 (NumericalArray<Type> const& table, Variable<IndexValueType> const& index) throw()
    {
        typedef InterpLagrange3<Type, IndexValueType> InterpType;
        return Variable (static_cast<Internal*> (new LookupWaveVariableInternal<Type, IndexValueType, InterpType> (table, index)));
    }


    static Variable clip (Variable const& source, Type minimumValue, Type maximumValue)
    {
        return Variable (static_cast<Internal*> (new ClipVariableInternal<Type> (source, minimumValue, maximumValue)));
    }

    Variable clip (Type minimumValue, Type maximumValue)
    {
        return Variable::clip (*this, minimumValue, maximumValue);
    }
    
    /** Create a Variable that converts from one type to another. */
    template<class OtherType>
    PLONK_INLINE_LOW Variable (OtherType const& other) throw()
    :   Base (new TypeVariableInternal<Type,OtherType> (other))
    {
    }
    
    /** A convenience for setValue(). */
    template<class ValueType>
    Variable& operator= (ValueType const& newValue) throw()
    {
        this->setValue (newValue);
        return *this;
    }
    
    /** Returns the current value. */
    PLONK_INLINE_LOW const Type getValue() const throw()
    {
        return this->getInternal()->getValue();
    }
    
    /** Returns the current value. */
    PLONK_INLINE_LOW Type* getValuePtr() throw()
    {
        return this->getInternal()->getValuePtr();
    }
    
    /** Returns the current value. */
    PLONK_INLINE_LOW operator Type () const throw()
    {
        return this->getInternal()->getValue();
    }
    
    /** Returns the next value. 
     By default this returns the current value. More complex Variable subclasses
     might generate sequences or random numbers (for example).*/
    PLONK_INLINE_LOW const Type nextValue() throw()
    {
        return this->getInternal()->nextValue();
    }
    
    /** Sets the current value. */
    PLONK_INLINE_LOW void setValue (Type const& newValue) throw()
    {
        this->getInternal()->setValue (newValue);
    }    
    
    PLONK_INLINE_LOW void swapValues (Type& other) throw()
    {
        this->getInternal()->swapValues (other);
    }
                    
    /** Builds a binary operation from two source Variables. */
    template<PLONK_BINARYOPFUNCTION(Type, op)>
    Variable binary (Variable const& rightOperand) const throw()
    {
        Internal* internal = new BinaryOpVariableInternal<Type,op> (*this, rightOperand);
        return Variable (internal);
    }
    
    /** Builds a unary operation from a source Variable. */
    template<PLONK_UNARYOPFUNCTION(Type, op)>
    Variable unary() const throw()
    {
        Internal* internal = new UnaryOpVariableInternal<Type,op> (*this);
        return Variable (internal);
    }
    
    PLONK_BINARYOPS(Variable)
    PLONK_UNARYOPS(Variable)
    
    Variable linlin (Variable const& inLow, Variable const& inHigh, Variable const& outLow, Variable const& outHigh) const throw()
    {
        return plonk::linlin (*this, inLow, inHigh, outLow, outHigh);
    }
    
    Variable linexp (Variable const& inLow, Variable const& inHigh, Variable const& outLow, Variable const& outHigh) const throw()
    {
        return plonk::linexp (*this, inLow, inHigh, outLow, outHigh);
    }
    
    Variable explin (Variable const& inLow, Variable const& inHigh, Variable const& outLow, Variable const& outHigh) const throw()
    {
        return plonk::explin (*this, inLow, inHigh, outLow, outHigh);
    }
    
    Variable linwelch (Variable const& inLow, Variable const& inHigh, Variable const& outLow, Variable const& outHigh) const throw()
    {
        return plonk::linwelch (*this, inLow, inHigh, outLow, outHigh);
    }

    
    typedef ShapeVariableInternal<Type>     ShapeVariableInternalType;
    typedef IntVariable                     StepsVariable;
    typedef Variable<Shape::ShapeType>      ShapeTypeVariable;
    typedef FloatVariable                   CurveVariable;
    
    /** Create a shape variable that can transition in different ways to new values. */
    Variable shape (StepsVariable const& numSteps) const throw()
    {
        Internal* internal = new ShapeVariableInternalType (*this, numSteps, Shape::Linear, 0.0f);
        return Variable (internal);
    }
    
    /** Create a shape variable that can transition in different ways to new values. */
    Variable shape (StepsVariable const& numSteps,
                    ShapeTypeVariable const& shapeType) const throw()
    {
        Internal* internal = new ShapeVariableInternalType (*this, numSteps, shapeType, 0.0f);
        return Variable (internal);
    }
    
    /** Create a shape variable that can transition in different ways to new values. */
    Variable shape (StepsVariable const& numSteps,
                    ShapeTypeVariable const& shapeType,
                    CurveVariable const& curve) const throw()
    {
        Internal* internal = new ShapeVariableInternalType (*this, numSteps, shapeType, curve);
        return Variable (internal);
    }
    
    Variable selectMin (Variable const& other) const throw()
    {
        return (this->getValue() > other.getValue()) ? other : *this;
    }    
    
    Variable selectMax (Variable const& other) const throw()
    {
        return (this->getValue() < other.getValue()) ? other : *this;
    }
            
    int getTypeCode() const throw()
    {
        return TypeUtility<Variable>::getTypeCode();
    }
    
    int getValueTypeCode() const throw()
    {
        return TypeUtility<Type>::getTypeCode();
    }        
    
    PLONK_INLINE_LOW UnitType ar() const throw()
    {
        return UnitType (*this).ar();
//        return ParamUnit<Type>::ar (*this);
    }
    
    PLONK_INLINE_LOW UnitType kr() const throw()
    {
        return UnitType (*this).kr();
//        return ParamUnit<Type>::kr (*this);
    }
    
    PLONK_INLINE_LOW UnitType ar (const Interp::TypeCode interpType) const throw()
    {
        return UnitType (*this).ar (interpType);
    }
    
    PLONK_INLINE_LOW UnitType kr (const Interp::TypeCode interpType) const throw()
    {
        return UnitType (*this).kr (interpType);
    }
};

PLONK_BINARYOPGLOBALS_TEMPLATE(Variable,Type);               // declares global functions with the same name as the binary operators
PLONK_BINARYOPGLOBALS_LITERAL_TEMPLATE(Variable,SampleType); // declares global functions with the same name as the binary operators for left operators that are literals
PLONK_UNARYOPGLOBALS_TEMPLATE(Variable,Type);                // declares global functions with the same name as the unary operators

template<class Type>
PLONK_INLINE_LOW UnitBase<Type> ar (Variable<Type> v) throw()
{
    return UnitBase<Type> (v).ar();
}

template<class Type>
PLONK_INLINE_LOW UnitBase<Type> kr (Variable<Type> v) throw()
{
    return UnitBase<Type> (v).kr();
}

#if !PLONK_ANDROID
template<class Type>
std::istream& operator>> (std::istream &inputStream, Variable<Type>& variable)
{
    Type value;
    inputStream >> value;
    
    variable = Variable<Type> (value);
    
    return inputStream;
}

template<class Type>
std::ostream& operator<< (std::ostream &outputStream, Variable<Type> const& variable)
{
    outputStream << variable.getValue();
    return outputStream;
}
#endif
    
//BINARYOPGLOBALS(Variable) // declares global functions with the same name as the binary operators
//UNARYOPGLOBALS(Variable)  // declares global functions with the same name as the unary operators

/** A variable container for objects.
 This is designed to store subclasses of SmartPointerContainer which covers most
 objects in Plonk (e.g., units, arrays). This makes it possible to store object 
 variables that need to be swapped atomically to assist with multithreaded 
 access. */
template<class Type>
class Variable<Type&> : public SmartPointerContainer< VariableInternal<Type&> >
{
public:
    typedef VariableInternal<Type&>         Internal;
    typedef SmartPointerContainer<Internal> Base;
    
    Variable()
    :   Base (new Internal (Type()))
    {
    }
    
    Variable (Variable const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}        
    
    Variable (Type& initValue) throw()
    :   Base (new Internal (initValue))
    {
    }
    
    template<class ValueType>
    Variable (ValueType const& initValue) throw()
    :   Base (new Internal (initValue))
    {
    }    
            
    template<class ValueType>
    PLONK_INLINE_LOW Variable& operator= (ValueType const& newValue) throw()
    {
        Type temp (newValue);
        this->swapValues (temp);
        return *this;
    }    
    
    PLONK_INLINE_LOW Variable& operator= (Variable const& other) throw()
    {
        Variable temp (other.getValue());
        this->swapValues (temp);
        return *this;
    }    

    PLONK_INLINE_LOW Type& getValue() throw()
    {
        return this->getInternal()->getValue();
    }
    
    PLONK_INLINE_LOW const Type& getValue() const throw()
    {
        return this->getInternal()->getValue();
    }
    
    PLONK_INLINE_LOW operator Type& () throw()
    {
        return this->getInternal()->getValue();
    }
    
    template<class ValueType>
    PLONK_INLINE_LOW operator ValueType() throw()
    {
        return ValueType (this->getInternal()->getValue());
    }
    
    template<class ValueType>
    PLONK_INLINE_LOW void setValue (ValueType const& newValue) throw()
    {
        this->getInternal()->setValue (newValue);
    }    
    
    PLONK_INLINE_LOW void swapValues (Type& other) throw()
    {
        this->getInternal()->swapValues (other);
    }
    
    PLONK_INLINE_LOW void swapValues (Variable& other) throw()
    {
        this->getInternal()->swapValues (other.getValue());
    }
    
    PLONK_INLINE_LOW bool isValueNull() const throw()
    {
        return this->getInternal()->getValue().isNull();
    }

    PLONK_INLINE_LOW bool isValueNotNull() const throw()
    {
        return this->getInternal()->getValue().isNotNull();
    }
    
    PLONK_OBJECTARROWOPERATOR(Variable);
};


#endif // PLONK_VARIABLE_H