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

#ifndef PLONK_SHAPEVARIABLE_H
#define PLONK_SHAPEVARIABLE_H

#include "plonk_VariableInternal.h"


template<class Type>
class ShapeVariableInternal : public VariableInternalBase<Type>
{
public:
    typedef Variable<Type>                 VariableType;
    typedef IntVariable                    StepsVariable;
    typedef Variable<Shape::ShapeType>     ShapeTypeVariable;
    typedef FloatVariable                  CurveVariable;

    ShapeVariableInternal (VariableType const& inputToUse,
                           StepsVariable const& numStepsToUse,
                           ShapeTypeVariable const& shapeToUse,
                           CurveVariable const& curveToUse) throw()
    :   input (inputToUse),
        numSteps (numStepsToUse),
        shape (shapeToUse),
        curve (curveToUse)
    {
        Memory::zero (shapeState);
        shapeState.stepsToTarget = TypeUtility<LongLong>::getTypePeak();
        shapeState.targetLevel = shapeState.currentLevel = input.getValue();
    }
    
    ~ShapeVariableInternal()
    {
    }
        
    const Type getValue() const throw()
    {
        return shapeState.currentLevel;
    }
    
    Type* getValuePtr() throw()
    {
        return &shapeState.currentLevel;
    }
    
    const Type nextValue() throw()
    {
        const Type nextValue = input.nextValue();
        
        if (nextValue != shapeState.targetLevel)
        {
            shapeState.targetLevel = nextValue;
            shapeState.shapeType = shape.nextValue();
            shapeState.curve = (shapeState.shapeType == Shape::Numerical) ? curve.nextValue() : 0.0f;
            shapeState.stepsToTarget = plonk::max (1, numSteps.nextValue());
            Shape::initShape (shapeState);
        }
        
        const Type result = Shape::next (shapeState);
        
        if (shapeState.stepsToTarget == TypeUtility<LongLong>::getTypePeak())
            this->update (Text::getEmpty(), Dynamic::getNull());
            
        return result;
    }
    
    void setValue(Type const& newValue) throw()
    {
        plonk_assertfalse;
    }
    
private:
    VariableType input;
    StepsVariable numSteps;
    ShapeTypeVariable shape;
    CurveVariable curve;
    ShapeState<Type> shapeState;
};

#endif // PLONK_SHAPEVARIABLE_H