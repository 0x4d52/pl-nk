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

#ifndef PLONK_BREAKPOINTS_H
#define PLONK_BREAKPOINTS_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "plonk_ObjectArray.h"
#include "plonk_SimpleArray.h"


template<class ValueType>
struct ShapeState
{
    LongLong stepsToTarget;
    ValueType currentLevel;
    ValueType targetLevel;
    ValueType grow;
    ValueType a2;
    ValueType b1;
    ValueType y1;
    ValueType y2;
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
        NumShapeTypes
    };
    
    Shape() throw()
    :   type (Linear),
        curve (0.f)
    {
    }
    
    Shape (const float curveToUse) throw()
    :   type (Numerical),
        curve (curveToUse)
    {
    }
    
    Shape (ShapeType const& typeToUse) throw()
    :   type (typeToUse),
        curve (0.f)
    {
    }

    ShapeType getType() const throw()				{ return type; }
	float getCurve() const throw()					{ return curve; }
	void setType (ShapeType const& newType) throw()	{ type = newType; }
	void setCurve (const float newCurve) throw()	{ type = Numerical; curve = newCurve; }
    
    template<class ValueType>
    static inline void initLinear (ShapeState<ValueType>& shapeState) throw()
    {        
        const ValueType diff = shapeState.targetLevel - shapeState.currentLevel;
        shapeState.grow = diff / shapeState.stepsToTarget;
    }

    template<class ValueType>
    static inline void initNumerical (ShapeState<ValueType>& shapeState) throw()
    {
        const ValueType& zero = Math<ValueType>::get0();
        const ValueType diff = shapeState.targetLevel - shapeState.currentLevel;
        
        if ((plonk::abs (shapeState.curve) < 0.001f) || (diff == zero))
        {
            shapeState.shapeType = Shape::Linear;
            shapeState.curve = 0.f;
            
            initLinear (shapeState);
        }
        else
        {
            const ValueType& one = Math<ValueType>::get1();
            const ValueType a1 = diff / (one - plonk::exp (shapeState.curve));
            shapeState.a2 = shapeState.currentLevel + a1;
            shapeState.b1 = a1;
            shapeState.grow = plonk::exp (shapeState.curve / ValueType (shapeState.stepsToTarget));
        }
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
    
    inline SampleType getTargetLevel() const throw()   { return this->getInternal()->getTargetLevel(); }
    inline double getTargetTime() const throw()        { return this->getInternal()->getTargetTime(); }
    inline const Shape& getShape() const throw()       { return this->getInternal()->getShape(); }
    inline bool isSustainPoint() const throw()         { return this->getInternal()->getNextGateOn() == BreakpointBase::This; }
    inline int getNextGateOn() const throw()           { return this->getInternal()->getNextGateOn(); }
    inline int getNextGateOff() const throw()          { return this->getInternal()->getNextGateOff(); }
   
    inline int getNext (const bool gate) const throw()
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
    
	BreakpointsInternal(LevelsArray const& levels,
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
    
    inline int getNumBreakpoints() const throw()
    {
        return breakpoints.length();
    }
    
    inline SampleType getStartLevel() const throw()
    {
        return startLevel;
    }
    
    inline const BreakpointArrayType& getBreakpoints() const throw()
    {
        return breakpoints; 
    }
	
private:
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
    typedef ObjectArray<Shape>              ShapeArray;
    typedef NumericalArray<int>                     NextArray;
    
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
    
    static BreakpointsBase linen (const double attackTime, 
                                  const double sustainTime,
                                  const double releaseTime,
                                  Shape const& shape = Shape::Linear) throw()
    {
        const SampleType peak = SampleTypeUtility::getTypePeak();
        return BreakpointsBase (LevelsArray (SampleType (0), peak, peak, SampleType (0)),
                                TimesArray (attackTime, sustainTime, releaseTime),
                                ShapeArray (shape));
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
    
    inline int getNumBreakpoints() const throw()
    {
        return this->getInternal()->getNumBreakpoints();
    }
    
    inline SampleType getStartLevel() const throw()
    {
        return this->getInternal()->getStartLevel();
    }
    
    inline const BreakpointType& atUnchecked (const int index) const throw()
    {
        return this->getInternal()->getBreakpoints().atUnchecked (index);
    }
    
    PLONK_OBJECTARROWOPERATOR(BreakpointsBase);
};


#endif // PLONK_BREAKPOINTS_H
