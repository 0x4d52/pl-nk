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
    typedef WeakPointerContainer<Variable>  Weak;
    
    /** Default constructor. */
    Variable() throw()
    :   Base (new VariableInternalType (Type (0)))
    {
    }
    
    /** Initialised with a specific internal. */
    explicit Variable (Internal* internal) throw()
    :   Base (internal)
    {
    }   
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    Variable (Variable const& copy) throw()
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
            this->setInternal (other.containerCopy().getInternal());
        
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
    Variable (Type const& initValue) throw()
    :   Base (new VariableInternalType (initValue))
    {
    }
    
    /** Create a Variable that converts from one type to another. */
    template<class OtherType>
    Variable (OtherType const& other) throw()
    :   Base (new TypeVariableInternal<Type,OtherType> (other))
    {
    }
    
    /** A convenience for setValue(). */
    Variable& operator= (Type const& newValue) throw()
    {
        this->setValue (newValue);
        return *this;
    }
    
    /** Returns the current value. */
    const Type getValue() const throw()
    {
        return this->getInternal()->getValue();
    }
    
    /** Returns the current value. */
    operator Type () const throw()
    {
        return this->getInternal()->getValue();
    }
    
    /** Returns the next value. 
     By default this returns the current value. More complex Variable subclasses
     might generate sequences or random numbers (for example).*/
    const Type nextValue() throw()
    {
        return this->getInternal()->nextValue();
    }
    
    /** Sets the current value. */
    void setValue(Type const& newValue) throw()
    {
        this->getInternal()->setValue (newValue);
    }    
                
    /** Builds a binary operation from two source Variables. */
    template<BINARYOPFUNCTION(Type, op)>
    const Variable binary (Variable const& rightOperand) const throw()
    {
        Internal* internal = new BinaryOpVariableInternal<Type,op> (*this, 
                                                                    rightOperand);
        return Variable (internal);
    }
    
    /** Builds a unary operation from a source Variable. */
    template<UNARYOPFUNCTION(Type, op)>
    const Variable unary() const throw()
    {
        Internal* internal = new UnaryOpVariableInternal<Type,op> (*this);
        return Variable (internal);
    }
    
    BINARYOPS(Variable)
    UNARYOPS(Variable)    
    
    const Variable selectMin (Variable const& other) const throw()
    {
        return (this->getValue() > other.getValue()) ? other : *this;
    }    
    
    const Variable selectMax (Variable const& other) const throw()
    {
        return (this->getValue() < other.getValue()) ? other : *this;
    }
        
    inline static const Variable& getZero() throw()  
    { 
        static const Variable v (Math<Type>::get0() ); 
        return v; 
    }
    
    inline static const Variable& getOne() throw()   
    {         
        static const Variable v (Math<Type>::get1() ); 
        return v; 
    }
    
    inline static const Variable& getTwo() throw()   
    {         
        static const Variable v (Math<Type>::get2() ); 
        return v; 
    }
    
    inline static const Variable& getHalf() throw()  
    {         
        static const Variable v (Math<Type>::get0_5()); 
        return v; 
    }
    
    inline static const Variable& getPi() throw()    
    {         
        static const Variable v (Math<Type>::getPi()); 
        return v; 
    }
    
    inline static const Variable& getTwoPi() throw() 
    {         
        static const Variable v (Math<Type>::get2Pi() ); 
        return v; 
    }    
    
    int getTypeCode() const throw()
    {
        return TypeUtility<Variable>::getTypeCode();
    }
    
    int getValueTypeCode() const throw()
    {
        return TypeUtility<Type>::getTypeCode();
    }        
};

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

//BINARYOPGLOBALS(Variable) // declares global functions with the same name as the binary operators
//UNARYOPGLOBALS(Variable)  // declares global functions with the same name as the unary operators

template<class Type>
class Variable<Type&> : public SmartPointerContainer< VariableInternal<Type&> >
{
public:
    typedef VariableInternal<Type&>         Internal;
    typedef SmartPointerContainer<Internal> Base;
    
    Variable()
    :   Base (new Internal (0))
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
        
//    inline Variable& operator= (Type& newValue) throw()
//    {
//        this->setValue (newValue);
//        return *this;
//    }
    
    template<class ValueType>
    inline Variable& operator= (ValueType const& newValue) throw()
    {
        this->setValue (newValue);
        return *this;
    }    
    
    inline Type& getValue() throw()
    {
        return this->getInternal()->getValue();
    }
    
    inline operator Type& () throw()
    {
        return this->getInternal()->getValue();
    }
    
    template<class ValueType>
    inline operator ValueType() throw()
    {
        return ValueType (this->getInternal()->getValue());
    }
    
    template<class ValueType>
    inline void setValue(ValueType const& newValue) throw()
    {
        this->getInternal()->setValue (newValue);
    }    
    
};


#endif // PLONK_VARIABLE_H