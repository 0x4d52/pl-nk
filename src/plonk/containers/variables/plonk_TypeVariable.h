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

#ifndef PLONK_TYPEVARIABLE_H
#define PLONK_TYPEVARIABLE_H

#include "plonk_VariableInternal.h"


template<class Type, class OtherType>
class TypeVariableInternal : public VariableInternalBase<Type>,
                             public Variable<OtherType>::Receiver
{
public:
    TypeVariableInternal(Variable<OtherType> const& input) throw()
    :   operand (input),
        cachedValue (Type (input.getValue()))
    {
        operand.addReceiver (this);
    }
    
    ~TypeVariableInternal()
    {
        operand.removeReceiver (this);
    }
    
    void changed (typename Variable<OtherType>::Sender const& sender, Dynamic const& message) throw()
    {
        (void)sender;
        (void)message;
        this->update (Dynamic::getNull()); // transmit the change down the chain..
    }    
    
    const Type getValue() const throw()
    {
        this->cachedValue = Type (operand.getValue());
        return this->cachedValue;
    }
    
    const Type nextValue() throw()
    {
        const Type result = Type (operand.nextValue());

        if (result != this->cachedValue)
        {
            this->cachedValue = result;
            this->update (Dynamic::getNull());
        }
        
        return result;
    }
    
    void setValue(Type const& newValue) throw()
    {
        const OtherType currentValue = operand.getValue();
        const OtherType otherNewValue = OtherType (newValue);
        
        if (otherNewValue != currentValue)
        {
            cachedValue = newValue;
            operand.setValue (otherNewValue);
        }
    }    
    
private:
    Variable<OtherType> operand; 
    mutable Type cachedValue;
};

#endif // PLONK_TYPEVARIABLE_H