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

#ifndef PLONK_BREAKPOINTS_H
#define PLONK_BREAKPOINTS_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "plonk_ObjectArray.h"
#include "plonk_SimpleArray.h"

// allow some shapes to use double calculations
template<class ValueType>
struct ShapeCalcType
{
    union Elem
    {
        ValueType norm;
        double dbl;
    } u;
};

template<class ValueType>
struct ShapeState
{
    LongLong stepsToTarget;
    ValueType currentLevel;
    ValueType targetLevel;
    ShapeCalcType<ValueType> grow;
    ShapeCalcType<ValueType> a2;
    ShapeCalcType<ValueType> b1;
    ShapeCalcType<ValueType> y1;
    ShapeCalcType<ValueType> y2;
    int shapeType;
    float curve;
};

class Shape
{
public:
    enum ShapeType
    {
        Linear,
        Numerical,
        Sine,
        NumShapeTypes
    };
    
    Shape() throw()
    :   type (Linear),
        curve (0.0f)
    {
    }
    
    Shape (const float curveToUse) throw()
    :   type (Numerical),
        curve (curveToUse)
    {
    }
    
    Shape (ShapeType const& typeToUse) throw()
    :   type (typeToUse),
        curve (0.0f)
    {
    }

    PLONK_INLINE_HIGH ShapeType getType() const throw() { return type; }
	PLONK_INLINE_HIGH float getCurve() const throw()    { return curve; }
	void setType (ShapeType const& newType) throw()     { type = newType; }
	void setCurve (const float newCurve) throw()        { type = Numerical; curve = newCurve; }

    template<class ValueType>
    static PLONK_INLINE_MID void sustain (ShapeState<ValueType>& shapeState)
    {
        shapeState.stepsToTarget = TypeUtility<LongLong>::getTypePeak();
        shapeState.currentLevel  = shapeState.targetLevel;
        shapeState.shapeType     = Shape::Linear;
        shapeState.curve         = 0.0f;
        shapeState.grow.u.norm   = Math<ValueType>::get0();
    }
    
    template<class ValueType>
    static PLONK_INLINE_MID void initShape (ShapeState<ValueType>& shapeState)
    {
        switch (shapeState.shapeType)
        {
            case Shape::Linear:    initLinear (shapeState);    break;
            case Shape::Numerical: initNumerical (shapeState); break;
            case Shape::Sine:      initSine (shapeState);      break;
            default:               initLinear (shapeState);    break;
        }
    }
    
    template<class ValueType>
    static PLONK_INLINE_MID ValueType next (ShapeState<ValueType>& shapeState)
    {
        if (shapeState.stepsToTarget == 1)
        {
            const ValueType result = shapeState.currentLevel;
            sustain (shapeState);
            return result;
        }
        else
        {
            --shapeState.stepsToTarget;
            
            switch (shapeState.shapeType)
            {
                case Shape::Linear:    return nextLinear (shapeState);
                case Shape::Numerical: return nextNumerical (shapeState);
                case Shape::Sine:      return nextSine (shapeState);
                default:               return nextLinear (shapeState);
            }
        }
    }
    
    template<class ValueType>
    static PLONK_INLINE_LOW void processShape (ShapeState<ValueType>& shapeState, ValueType* outputSamples, const int numSamples)
    {
        plonk_assert (outputSamples != &shapeState.currentLevel);
//        plonk_assert (numSamples <= shapeState.stepsToTarget);  ///<<< this should probably be true too..
        
        int numSamplesRemaining = numSamples;
        
        while (numSamplesRemaining > 0)
        {
            const int numSamplesThisTime = int (plonk::min (LongLong (numSamples), shapeState.stepsToTarget));
        
            switch (shapeState.shapeType)
            {
                case Shape::Linear:    processLinear (shapeState, outputSamples, numSamplesThisTime);    break;
                case Shape::Numerical: processNumerical (shapeState, outputSamples, numSamplesThisTime); break;
                case Shape::Sine:      processSine (shapeState, outputSamples, numSamplesThisTime);      break;
                default:               processLinear (shapeState, outputSamples, numSamplesThisTime);    break;
            }
            
            numSamplesRemaining -= numSamplesThisTime;
            outputSamples += numSamplesThisTime;
            
            // too many samples were requested so let's flatten off at the current level
            if (numSamplesRemaining > 0)
            {
                shapeState.stepsToTarget = TypeUtility<LongLong>::getTypePeak();
                shapeState.currentLevel  = shapeState.targetLevel;
                shapeState.shapeType     = Shape::Linear;
                shapeState.grow.u.norm   = 0;
            }
        }
    }
    
private:
    template<class ValueType>
    static PLONK_INLINE_LOW void initLinear (ShapeState<ValueType>& shapeState) throw()
    {
        const ValueType diff = shapeState.targetLevel - shapeState.currentLevel;
        shapeState.grow.u.norm = diff / ValueType (shapeState.stepsToTarget);
    }
    
    template<class ValueType>
    static PLONK_INLINE_LOW void initNumerical (ShapeState<ValueType>& shapeState) throw()
    {
        const ValueType& zero = Math<ValueType>::get0();
        const ValueType diff = shapeState.targetLevel - shapeState.currentLevel;
        
        if ((plonk::abs (shapeState.curve) < 0.001f) || (diff == zero))
        {
            shapeState.shapeType = Shape::Linear;
            shapeState.curve     = 0.0f;
            initLinear (shapeState);
        }
        else
        {
            const ValueType& one   = Math<ValueType>::get1();
            const ValueType a1     = diff / (one - plonk::exp (shapeState.curve));
            shapeState.a2.u.norm   = shapeState.currentLevel + a1;
            shapeState.b1.u.norm   = a1;
            shapeState.grow.u.norm = plonk::exp (shapeState.curve / ValueType (shapeState.stepsToTarget));
        }
    }
    
    template<class ValueType>
    static PLONK_INLINE_LOW void initSine (ShapeState<ValueType>& shapeState) throw()
    {
        const double pi        = Math<double>::getPi();
        const double piOverTwo = Math<double>::getPi_2();
        const double half      = Math<double>::get0_5();
        const double two       = Math<double>::get2();
        const double sum       = shapeState.targetLevel + shapeState.currentLevel;
        const double diff      = shapeState.targetLevel - shapeState.currentLevel;
        const double w         = pi / double (shapeState.stepsToTarget);
        
        shapeState.a2.u.dbl = sum * half;
        shapeState.b1.u.dbl = two * plonk::cos (w);
        shapeState.y1.u.dbl = diff * half;
        shapeState.y2.u.dbl = shapeState.y1.u.dbl * plonk::sin (piOverTwo - w);
        
        shapeState.currentLevel = ValueType (shapeState.a2.u.dbl - shapeState.y1.u.dbl);
    }
    
    template<class ValueType>
    static PLONK_INLINE_HIGH ValueType nextLinear (ShapeState<ValueType>& shapeState) throw()
    {
        const ValueType result = shapeState.currentLevel;
        shapeState.currentLevel += shapeState.grow.u.norm;
        return result;
    }
    
    template<class ValueType>
    static PLONK_INLINE_HIGH ValueType nextNumerical (ShapeState<ValueType>& shapeState) throw()
    {
        const ValueType result  = shapeState.currentLevel;
        shapeState.b1.u.norm   *= shapeState.grow.u.norm;
        shapeState.currentLevel = shapeState.a2.u.norm - shapeState.b1.u.norm;                    
        return result;
    }
    
    template<class ValueType>
    static PLONK_INLINE_HIGH ValueType nextSine (ShapeState<ValueType>& shapeState) throw()
    {
        const ValueType result  = shapeState.currentLevel;
        const double y0         = shapeState.b1.u.dbl * shapeState.y1.u.dbl - shapeState.y2.u.dbl;
        shapeState.currentLevel = ValueType (shapeState.a2.u.dbl - y0);
        shapeState.y2.u.dbl     = shapeState.y1.u.dbl;
        shapeState.y1.u.dbl     = y0;
        return result;
    }
    
    template<class ValueType>
    static PLONK_INLINE_MID void processLinear (ShapeState<ValueType>& shapeState, ValueType* const outputSamples, const int numSamples) throw()
    {
        const ValueType& zero = Math<ValueType>::get0();
        
        if (shapeState.grow.u.norm == zero)
        {
            for (int i = 0; i < numSamples; ++i)
                outputSamples[i] = shapeState.currentLevel;
        }
        else if (numSamples == shapeState.stepsToTarget)
        {
            const int lastIndex = numSamples - 1;
            
            for (int i = 0; i < lastIndex; ++i)
                outputSamples[i] = nextLinear (shapeState);
            
            outputSamples[lastIndex] = shapeState.currentLevel = shapeState.targetLevel;
        }
        else
        {
            for (int i = 0; i < numSamples; ++i)
                outputSamples[i] = nextLinear (shapeState);
        }
        
        shapeState.stepsToTarget -= numSamples;
    }
    
    template<class ValueType>
    static PLONK_INLINE_MID void processNumerical (ShapeState<ValueType>& shapeState, ValueType* const outputSamples, const int numSamples) throw()
    {        
        if (numSamples == shapeState.stepsToTarget)
        {
            const int lastIndex = numSamples - 1;
            
            for (int i = 0; i < lastIndex; ++i)
                outputSamples[i] = nextNumerical (shapeState);
            
            outputSamples[lastIndex] = shapeState.currentLevel = shapeState.targetLevel;
        }
        else
        {
            for (int i = 0; i < numSamples; ++i)
                outputSamples[i] = nextNumerical (shapeState);
        }
        
        shapeState.stepsToTarget -= numSamples;
    }
    
    template<class ValueType>
    static PLONK_INLINE_MID void processSine (ShapeState<ValueType>& shapeState, ValueType* const outputSamples, const int numSamples) throw()
    {
        if (numSamples == shapeState.stepsToTarget)
        {
            const int lastIndex = numSamples - 1;
            
            for (int i = 0; i < lastIndex; ++i)
                outputSamples[i] = nextSine (shapeState);
                
            outputSamples[lastIndex] = shapeState.currentLevel = shapeState.targetLevel;
        }
        else
        {
            for (int i = 0; i < numSamples; ++i)
                outputSamples[i] = nextSine (shapeState);
        }
        
        shapeState.stepsToTarget -= numSamples;
    }

private:
    ShapeType type;
    float curve;
};


template<class SampleType>
class BreakpointInternal : public SenderInternal< BreakpointBase<SampleType> >
{
public:
    typedef BreakpointBase<SampleType> Container;
        
    BreakpointInternal (const SampleType targetLevel, 
                        const double targetTime,
                        Shape const& shapeToUse,
                        const int nextGateOn,
                        const int nextGateOff) throw()
    :   level (targetLevel),
        time (targetTime),
        shape (shapeToUse),
        nextOn (nextGateOn),
        nextOff (nextGateOff)
    {
    }
    
    SampleType getTargetLevel() const throw()   { return this->level; }
    double getTargetTime() const throw()        { return this->time; }
    const Shape& getShape() const throw()       { return this->shape; }
    int getNextGateOn() const throw()           { return this->nextOn; }
    int getNextGateOff() const throw()          { return this->nextOff; }

private:
    SampleType level;
    double time;
    Shape shape;
    int nextOn;
    int nextOff;
};

//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class SampleType>
class BreakpointBase : public SenderContainer< BreakpointInternal<SampleType> >
{
public:
    typedef BreakpointInternal<SampleType>         Internal;
    typedef SenderContainer<Internal>              Base;
    typedef WeakPointerContainer<BreakpointBase>   Weak;    
    
    enum NextPoint
    {
        This = -2,
        Next = -1,
        End = INT_MAX
    };    
    
	BreakpointBase(const SampleType targetLevel = SampleType (0), 
                   const double targetTime = 0.0,
                   Shape const& shape = Shape::Linear,
                   const int nextGateOn = BreakpointBase::Next,
                   const int nextGateOff = BreakpointBase::Next) throw()
	:	Base (new Internal (targetLevel,
                            targetTime,
                            shape,
                            nextGateOn,
                            nextGateOff))
	{
        plonk_assert (nextGateOn >= -2);  // must be a special code or a real index, >=0
        plonk_assert (nextGateOff >= -1); // not the only infinite loop but one of them!
	}
	
    explicit BreakpointBase (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	} 
    
    BreakpointBase (BreakpointBase const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}    
    
    BreakpointBase (Dynamic const& other) throw()
    :   Base (other.as<BreakpointBase>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    BreakpointBase& operator= (BreakpointBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static BreakpointBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
        
    static const BreakpointBase& getNull() throw()
	{
		static BreakpointBase null;        
		return null;
	}	                            
    
    PLONK_INLINE_LOW SampleType getTargetLevel() const throw()   { return this->getInternal()->getTargetLevel(); }
    PLONK_INLINE_LOW double getTargetTime() const throw()        { return this->getInternal()->getTargetTime(); }
    PLONK_INLINE_LOW const Shape& getShape() const throw()       { return this->getInternal()->getShape(); }
    PLONK_INLINE_LOW bool isSustainPoint() const throw()         { return this->getInternal()->getNextGateOn() == BreakpointBase::This; }
    PLONK_INLINE_LOW int getNextGateOn() const throw()           { return this->getInternal()->getNextGateOn(); }
    PLONK_INLINE_LOW int getNextGateOff() const throw()          { return this->getInternal()->getNextGateOff(); }
   
    PLONK_INLINE_LOW int getNext (const bool gate) const throw()
    {
        return gate ? this->getNextGateOn() : this->getNextGateOff();
    }

    PLONK_OBJECTARROWOPERATOR(BreakpointBase);

};


//------------------------------------------------------------------------------


template<class SampleType>
class BreakpointsInternal : public SenderInternal< BreakpointsBase<SampleType> >
{
public:
    typedef BreakpointsBase<SampleType>     Container;
    typedef BreakpointBase<SampleType>      BreakpointType;
    typedef ObjectArray<BreakpointType>     BreakpointArrayType;
    
    typedef NumericalArray<SampleType>      LevelsArray;
    typedef NumericalArray<double>          TimesArray;
    typedef ObjectArray<Shape>              ShapeArray;
    typedef NumericalArray<int>             NextArray;
    
	BreakpointsInternal (LevelsArray const& levels,
                         TimesArray const& times,
                         ShapeArray const& shapes,
                         NextArray const& nextGateOns,
                         NextArray const& nextGateOffs) throw()
    :   breakpoints (BreakpointArrayType::withSize (times.length()))
	{
        plonk_assert (levels.length() >= 2);
        plonk_assert (times.length() == (levels.length() - 1));
        plonk_assert (((nextGateOns.length() == 0) && (nextGateOffs.length() == 0)) || 
                     ((nextGateOns.length() == (levels.length() - 1))) && ((nextGateOffs.length() == (levels.length() - 1))));

        const SampleType* levelsArray = levels.getArray();
        const double* timesArray = times.getArray();
        const int* nextOnsArray = nextGateOns.getArray();
        const int* nextOffsArray = nextGateOffs.getArray();

        this->startLevel = *levelsArray++;
        
        const int numBreakpoints = times.length();
        
        if (nextGateOns.length() != 0)
        {
            for (int i = 0; i < numBreakpoints; ++i)
            {
                this->breakpoints.put (i, BreakpointType (levelsArray[i],
                                                          timesArray[i],
                                                          shapes.wrapAt (i),
                                                          nextOnsArray[i],
                                                          nextOffsArray[i]));
            }
        } 
        else
        {
            for (int i = 0; i < numBreakpoints; ++i)
            {
                this->breakpoints.put (i, BreakpointType (levelsArray[i],
                                                          timesArray[i],
                                                          shapes.wrapAt (i)));
            }
        }
	}
	
	~BreakpointsInternal()
	{
	}
    
    PLONK_INLINE_MID int getNumBreakpoints() const throw()
    {
        return breakpoints.length();
    }
    
    PLONK_INLINE_MID SampleType getStartLevel() const throw()
    {
        return startLevel;
    }
    
    PLONK_INLINE_MID const BreakpointArrayType& getBreakpoints() const throw()
    {
        return breakpoints; 
    }
    
    PLONK_INLINE_LOW const Container levelScale (const SampleType amount) const throw()
    {
        BreakpointsInternal* internal = new BreakpointsInternal();
        
        internal->startLevel = startLevel * amount;
        
        for (int i = 0; i < breakpoints.length(); ++i)
        {
            const BreakpointType& point = breakpoints.atUnchecked (i);
            internal->breakpoints.add (BreakpointType (point.getTargetLevel() * amount,
                                                       point.getTargetTime(),
                                                       point.getShape(),
                                                       point.getNextGateOn(),
                                                       point.getNextGateOff()));
        }
        
        return Container (internal);
    }
    
    PLONK_INLINE_LOW const Container timeScale (const SampleType amount) const throw()
    {
        BreakpointsInternal* internal = new BreakpointsInternal();
        
        internal->startLevel = startLevel;
        
        for (int i = 0; i < breakpoints.length(); ++i)
        {
            const BreakpointType& point = breakpoints.atUnchecked (i);
            internal->breakpoints.add (BreakpointType (point.getTargetLevel(),
                                                       point.getTargetTime() * amount,
                                                       point.getShape(),
                                                       point.getNextGateOn(),
                                                       point.getNextGateOff()));
        }
        
        return Container (internal);
    }
	
private:
    BreakpointsInternal() throw()
    {
    }
    
    SampleType startLevel;
    BreakpointArrayType breakpoints;
};


//------------------------------------------------------------------------------

/** @ingroup PlonkContainerClasses */
template<class SampleType>
class BreakpointsBase : public SenderContainer< BreakpointsInternal<SampleType> >
{
public:
    typedef BreakpointsInternal<SampleType>         Internal;
    typedef SenderContainer<Internal>               Base;
    typedef WeakPointerContainer<BreakpointsBase>   Weak;    
    
    typedef NumericalArray<SampleType>              LevelsArray;
    typedef NumericalArray<double>                  TimesArray;
    typedef ObjectArray<Shape>                      ShapeArray;
    typedef NumericalArray<int>                     NextArray;

    typedef NumericalArray<SampleType>              Buffer;
    
    typedef SignalBase<SampleType>                  SignalType;
    typedef BreakpointBase<SampleType>              BreakpointType;
    typedef TypeUtility<SampleType>                 SampleTypeUtility;
    
    
	BreakpointsBase (LevelsArray const& levels = LevelsArray (SampleType (0), SampleType (0)),
                     TimesArray const& times = TimesArray (0.0),
                     ShapeArray const& shapes = ShapeArray (Shape::Linear),
                     NextArray const& nextGateOns = NextArray::getNull(),
                     NextArray const& nextGateOffs = NextArray::getNull()) throw()
	:	Base (new Internal(levels, times, shapes, nextGateOns, nextGateOffs))
	{
	}
	
    explicit BreakpointsBase (Internal* internalToUse) throw() 
	:	Base (internalToUse)
	{
	} 
    
    BreakpointsBase (BreakpointsBase const& copy) throw()
	:	Base (static_cast<Base const&> (copy))
	{
	}          
    
    BreakpointsBase (Dynamic const& other) throw()
    :   Base (other.as<BreakpointsBase>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    BreakpointsBase& operator= (BreakpointsBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static BreakpointsBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    static const BreakpointsBase& getNull() throw()
	{
		static BreakpointsBase null;        
		return null;
	}	               
    
    static BreakpointsBase value (const double value,
                                  const double sustainTime) throw()
    {
        return BreakpointsBase (LevelsArray (value, value),
                                TimesArray (sustainTime),
                                ShapeArray (Shape::Linear));
    }
    
    static BreakpointsBase line (const double startLevel,
                                 const double endLevel,
                                 const double duration,
                                 Shape const& shape = Shape::Linear) throw()
    {
        return BreakpointsBase (LevelsArray (startLevel, endLevel),
                                TimesArray (duration),
                                ShapeArray (shape));
    }
    
    static BreakpointsBase lineSustain (const double startLevel,
                                        const double startTime,
                                        const double sustainLevel,
                                        const double endTime,
                                        const double endLevel,
                                        Shape const& shape = Shape::Linear) throw()
    {
        const int lastPoint = 1;
        return BreakpointsBase (LevelsArray (startLevel, sustainLevel, endLevel),
                                TimesArray (startTime, endTime),
                                ShapeArray (shape),
                                NextArray (BreakpointType::This, BreakpointType::End),
                                NextArray (lastPoint, BreakpointType::End));
    }
    
    static BreakpointsBase linen (const double attackTime, 
                                  const double sustainTime,
                                  const double decayTime,
                                  Shape const& shape = Shape::Linear) throw()
    {
        const SampleType peak = SampleTypeUtility::getTypePeak();
        return BreakpointsBase (LevelsArray (SampleType (0), peak, peak, SampleType (0)),
                                TimesArray (attackTime, sustainTime, decayTime),
                                ShapeArray (shape));
    }
    
//    static BreakpointsBase linenh (const double attackTime,
//                                   const double hold1Time,
//                                   const double sustainLevel,
//                                   const double hold2Time,
//                                   const double decayTime,
//                                   Shape const& shape = Shape::Linear) throw()
//    {
//        const SampleType peak = SampleTypeUtility::getTypePeak();
//        return BreakpointsBase (LevelsArray (SampleType (0), peak, sustainLevel, sustainLevel, SampleType (0)),
//                                TimesArray (attackTime, hold1Time, hold2Time, decayTime),
//                                ShapeArray (shape));
//    }
    
    static BreakpointsBase ahdsr (const double attackTime,
                                  const double holdTime,
                                  const double decayTime,
                                  const SampleType sustainLevel,
                                  const double releaseTime,
                                  Shape const& shape = Shape::Linear) throw()
    {
        const int lastPoint = 3;
        const SampleType peak = SampleTypeUtility::getTypePeak();
        return BreakpointsBase (LevelsArray (SampleType (0), peak, peak, sustainLevel, SampleType (0)),
                                TimesArray (attackTime, holdTime, decayTime, releaseTime),
                                ShapeArray (shape),
                                NextArray (BreakpointType::Next, BreakpointType::Next, BreakpointType::This, BreakpointType::End),
                                NextArray (lastPoint, lastPoint, lastPoint, BreakpointType::End));
    }
    
    static BreakpointsBase adsr (const double attackTime, 
                                 const double decayTime,
                                 const SampleType sustainLevel,
                                 const double releaseTime,
                                 Shape const& shape = Shape::Linear) throw()
    {
        const int lastPoint = 2;
        const SampleType peak = SampleTypeUtility::getTypePeak();
        return BreakpointsBase (LevelsArray (SampleType (0), peak, sustainLevel, SampleType (0)),
                                TimesArray (attackTime, decayTime, releaseTime),
                                ShapeArray (shape),
                                NextArray (BreakpointType::Next, BreakpointType::This, BreakpointType::End),
                                NextArray (lastPoint, lastPoint, BreakpointType::End));
    }
    
    static BreakpointsBase asr (const double attackTime, 
                                const SampleType sustainLevel,
                                const double releaseTime,
                                Shape const& shape = Shape::Linear) throw()
    {
        const int lastPoint = 1;
        return BreakpointsBase (LevelsArray (SampleType (0), sustainLevel, SampleType (0)),
                                TimesArray (attackTime, releaseTime),
                                ShapeArray (shape),
                                NextArray (BreakpointType::This, BreakpointType::End),
                                NextArray (lastPoint, BreakpointType::End));
    }

//    static BreakpointsBase asr (const double attackTime,
//                                const SampleType sustainLevel,
//                                const double releaseTime,
//                                Shape const& shape = Shape::Linear) throw()
//    {
//        return BreakpointsBase::lineSustain (0.0, attackTime, sustainLevel, releaseTime, 0.0, shape);
//    }

    static BreakpointsBase steal (const double releaseTime,
                                  Shape const& shape = Shape::Linear) throw()
    {
        const int lastPoint = 1;
        return BreakpointsBase (LevelsArray (SampleType (1), SampleType (1), SampleType (0)),
                                TimesArray (0.0, releaseTime),
                                ShapeArray (shape),
                                NextArray (BreakpointType::This, BreakpointType::End),
                                NextArray (lastPoint, BreakpointType::End));
    }
    
    PLONK_INLINE_MID int getNumBreakpoints() const throw()
    {
        return this->getInternal()->getNumBreakpoints();
    }
    
    PLONK_INLINE_MID SampleType getStartLevel() const throw()
    {
        return this->getInternal()->getStartLevel();
    }
    
    PLONK_INLINE_MID const BreakpointType& atUnchecked (const int index) const throw()
    {
        return this->getInternal()->getBreakpoints().atUnchecked (index);
    }
    
    PLONK_INLINE_MID const BreakpointsBase levelScale (const SampleType amount) const throw()
    {
        return this->getInternal()->levelScale (amount);
    }
    
    PLONK_INLINE_MID const BreakpointsBase timeScale (const SampleType amount) const throw()
    {
        return this->getInternal()->timeScale (amount);
    }
    
    PLONK_INLINE_LOW double duration() const throw()
    {
        double duration = 0.0;
        
        for (int i = 0; i < getNumBreakpoints(); ++i)
            duration += atUnchecked (i).getTargetTime();
        
        return duration;
    }
    
    SampleType lookup (const double time) const throw()
    {
        (void)time;
        plonk_assertfalse; // todo
    }
    
    void toSignal (SignalType& signal) throw()
    {
        (void)signal;
        plonk_assertfalse; // todo
    }
    
    PLONK_OBJECTARROWOPERATOR(BreakpointsBase);
};


#endif // PLONK_BREAKPOINTS_H
