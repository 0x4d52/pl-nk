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

#ifndef PLONK_UNIT_H
#define PLONK_UNIT_H

#include "plonk_GraphForwardDeclarations.h"
#include "utility/plonk_ProcessInfo.h"
#include "utility/plonk_BlockSize.h"
#include "utility/plonk_SampleRate.h"
#include "info/plonk_InfoHeaders.h"

/**  A Unit is effectively an array of Channel objects. 
 A Unit effective IS a Unit graph since each Channel of each Unit may be in turn
 connected to more Unit objects via its inputs. 
 
 Inherits from ObjectArray and is specifically designed to hold an array of 
 ChannelBase objects. The is perhaps the most important class at the user end 
 of the code. The user deals with UnitBase objects and combines them into a 
 graph to produce audio processing algorithms. In fact a UnitBase object is a 
 graph. There are two primary static (factory) functions for creating UnitBase 
 objects from specific channel types: createFromDictionary() and 
 proxiesFromDictionary(). The latter will be discussed below, 
 createFromDictionary() is passed a dictionary containing the 
 desired inputs to the new Unit and it allocates a number of channels of a 
 specified ChannelInternal type. These are passed a 'data' structure to 
 initialise themselves. In many cases this might just be the default 
 ChannelInternalCore::Data where the sample rate information can be 
 transmitted up the graph. The UnitBase that is created will have a number of 
 channels equal to the the maximum number of channels found in the provided 
 inputs i.e., if a stereo unit is passed as the frequency input to the SawUnit 
 then two channels of the SawUnit will be created - at render time the 
 appropriate channel of the input unit will be passed to the appropriate 
 channel of the destination unit. There is no concept of 'wire' or 
 'connections' in Plonk - the graph connections are implied by the layering 
 of units to form a chain (and by implication the 'graph' is constructed by how 
 the channels in these layers transmit to the next layer). UnitBase objects 
 repsond to most of the standard unary and binary operators 
 (including symbols +, -, * etc). These create units that apply this operator 
 at render time.
 @see ChannelBase, ProcessInfo
*/
template<class SampleType>
class UnitBase : public NumericalArray<ChannelBase<SampleType> >
{
protected:
    typedef ChannelBase<SampleType>                     ChannelType;
    typedef ObjectArray<ChannelType>                    ChannelArrayType;          
    typedef ChannelInternalBase<SampleType>             ChannelInternalType;
    typedef NumericalArray<ChannelType>                 UnitType;
    typedef NumericalArray2D<ChannelType,UnitBase>      UnitArray;
    typedef NumericalArray2D<SampleType>                BufferArrayType;
    typedef NullChannelInternal<SampleType>             NullInternal;
    typedef Variable<SampleType>                        VariableType;
    typedef AtomicValue<SampleType>                     AtomicType;
    typedef Variable<AtomicType&>                       AtomicVariableType;
    typedef AtomicVariableUnit<SampleType>              AtomicVariableUnitType;
    typedef typename ChannelBase<SampleType>::Receiver  Receiver;

    typedef typename BinaryOpFunctionsHelper<SampleType>::BinaryOpFunctionsType BinaryOpFunctionsType;
    typedef typename UnaryOpFunctionsHelper<SampleType>::UnaryOpFunctionsType UnaryOpFunctionsType;

    typedef typename TypeUtility<SampleType>::IndexType IndexType;

public:
    typedef ObjectArrayInternal<ChannelType>    Internal;    
    typedef UnitType                            Base;    
    typedef WeakPointerContainer<UnitBase>      Weak;
    typedef NumericalArray<SampleType>          Buffer;
    typedef InputDictionary                     Inputs;    
    
    class InitialUnit
    {
	public:
		InitialUnit() throw() 
        :   value (UnitBase::getNull()), 
            valid (false) 
        { 
        }
        
		InitialUnit (UnitBase const& initialValue) throw()
		:	value (initialValue), 
            valid (true) 
		{ 
		}
        
		template<class OtherType>
		InitialUnit (OtherType const& initialValue) throw()
		:	value (initialValue), 
            valid (true) 
		{ 
		}
		
		const UnitBase value;
		const bool valid;
        
	private:
		InitialUnit& operator= (InitialUnit const&); // to prevent assignment and MSVC complaining!
	};

    
    /** Default constructor.
     This creates a Unit with a single null Channel. */
    PLONK_INLINE_LOW UnitBase() throw()
    :   Base (ChannelType())
    {
    }
        
    explicit UnitBase (Internal* internal) throw()
    :   Base (internal)
    {
    }
    
    UnitBase (Dynamic const& other) throw()
    :   Base (other.as<UnitBase>().getInternal())
    {
    }          

    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */
    static UnitBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }       
    
    static const UnitBase& getNull() throw()
	{
		static UnitBase null;
		return null;
	}	                            
    
    static const UnitBase emptyChannels (const int numChannels) throw()
    {
        return UnitBase::withSize (numChannels);
    }
    
    /** This creates a Unit with a single Channel. */
    UnitBase (ChannelType const& channel) throw()
    :   UnitType (channel)
    {
        this->atUnchecked (0).initChannel (0);
    }
    
    /** Constant Unit constructor.
     This creates a Unit with a single constant Channel. Ideally you could/should cache frequently used
     constants (e.g., 1, pi, etc). */
    template<class ValueType>
    PLONK_INLINE_LOW UnitBase (ValueType const& valueInit) throw()
    :   UnitType (ChannelType (valueInit))
    {
        this->atUnchecked (0).initChannel (0);
    }
    
    /** Multichannel Constant Unit constructor.
     This creates a Unit with a multiple constant Channel objects derived 
     from an array of numerical values. */
    template<class ValueType>
    PLONK_INLINE_LOW UnitBase (NumericalArray<ValueType> const& values) throw()
    :   UnitType (UnitType::template collect<ChannelType> (values))
    {
        for (int i = 0; i < this->getNumChannels(); ++i)
            this->atUnchecked (i).initChannel (i);
    }    
    
    PLONK_INLINE_LOW UnitBase (VariableType const& variable) throw()
    :   UnitType (ParamUnit<SampleType>::kr (variable))
    {
    }
    
    UnitBase (AtomicVariableType const& variable) throw()
    :   UnitType (AtomicVariableUnitType::kr (variable))
    {
    }
        
    UnitBase (NumericalArray<VariableType> const& variables) throw()
    :   UnitType (UnitType::withSize (variables.length()))
    {
        for (int i = 0; i < this->getNumChannels(); ++i) 
        {
            UnitBase variable = variables.atUnchecked (i);
            this->put (i, variable.atUnchecked (0));
        }
    }
    
    /** Copy constructor from the base array type. */
    PLONK_INLINE_LOW UnitBase (UnitBase const& copy) throw()
	:	UnitType (static_cast<UnitType const&> (copy))
	{
	}                
    
    
    template<template <typename> class OtherUnitType, class OtherSampleType>
    PLONK_INLINE_LOW UnitBase (OtherUnitType<OtherSampleType> const& other)
    :   UnitType (TypeUnit<SampleType,OtherSampleType>::ar (other))
    {
    }
    
    UnitBase (UnitBase const& i00,
              UnitBase const& i01) throw()
    :   Base (i00, i01)
    {
    }
    
private:
    static int countValidChannels (InitialUnit const& i00,
                                   InitialUnit const& i01,
                                   InitialUnit const& i02,
                                   InitialUnit const& i03) throw()
    {
        int numChannels = 0;
        
        if (i00.valid) numChannels += i00.value.getNumChannels();
        if (i01.valid) numChannels += i01.value.getNumChannels();
        if (i02.valid) numChannels += i02.value.getNumChannels();        
        if (i03.valid) numChannels += i03.value.getNumChannels();

        return numChannels;
    }
    
public:
    UnitBase (InitialUnit const& i00,
              InitialUnit const& i01,
              InitialUnit const& i02,
              InitialUnit const& i03 = InitialUnit()) throw()
	:	Base (NumericalArraySpec (countValidChannels (i00, i01, i02, i03), false))
	{
        int i;
        ChannelType* array = this->getArray();
        
        if (i00.valid) { for (i = 0; i < i00.value.getNumChannels(); ++i) *array++ = i00.value.atUnchecked (i); }
        if (i01.valid) { for (i = 0; i < i01.value.getNumChannels(); ++i) *array++ = i01.value.atUnchecked (i); }
        if (i02.valid) { for (i = 0; i < i02.value.getNumChannels(); ++i) *array++ = i02.value.atUnchecked (i); }
        if (i03.valid) { for (i = 0; i < i03.value.getNumChannels(); ++i) *array++ = i03.value.atUnchecked (i); }
	}
    
private:
    static int countValidChannels (InitialUnit const& i00,
                                   InitialUnit const& i01,
                                   InitialUnit const& i02,
                                   InitialUnit const& i03,
                                   InitialUnit const& i04,
                                   InitialUnit const& i05,
                                   InitialUnit const& i06,
                                   InitialUnit const& i07) throw()
    {
        int numChannels = 0;
        
        if (i00.valid) numChannels += i00.value.getNumChannels();
        if (i01.valid) numChannels += i01.value.getNumChannels();
        if (i02.valid) numChannels += i02.value.getNumChannels();        
        if (i03.valid) numChannels += i03.value.getNumChannels();
        if (i04.valid) numChannels += i04.value.getNumChannels();
        if (i05.valid) numChannels += i05.value.getNumChannels();
        if (i06.valid) numChannels += i06.value.getNumChannels();
        if (i07.valid) numChannels += i07.value.getNumChannels();
        
        return numChannels;
    }
    
public:
    UnitBase (InitialUnit const& i00,
              InitialUnit const& i01,
              InitialUnit const& i02,
              InitialUnit const& i03,
              InitialUnit const& i04,
              InitialUnit const& i05 = InitialUnit(),
              InitialUnit const& i06 = InitialUnit(),
              InitialUnit const& i07 = InitialUnit()) throw()
	:	Base (NumericalArraySpec (countValidChannels (i00, i01, i02, i03,
                                                      i04, i05, i06, i07), false))
	{
        int i;
        ChannelType* array = this->getArray();
        
        if (i00.valid) { for (i = 0; i < i00.value.getNumChannels(); ++i) *array++ = i00.value.atUnchecked (i); }
        if (i01.valid) { for (i = 0; i < i01.value.getNumChannels(); ++i) *array++ = i01.value.atUnchecked (i); }
        if (i02.valid) { for (i = 0; i < i02.value.getNumChannels(); ++i) *array++ = i02.value.atUnchecked (i); }
        if (i03.valid) { for (i = 0; i < i03.value.getNumChannels(); ++i) *array++ = i03.value.atUnchecked (i); }
        if (i04.valid) { for (i = 0; i < i04.value.getNumChannels(); ++i) *array++ = i04.value.atUnchecked (i); }
        if (i05.valid) { for (i = 0; i < i05.value.getNumChannels(); ++i) *array++ = i05.value.atUnchecked (i); }
        if (i06.valid) { for (i = 0; i < i06.value.getNumChannels(); ++i) *array++ = i06.value.atUnchecked (i); }
        if (i07.valid) { for (i = 0; i < i07.value.getNumChannels(); ++i) *array++ = i07.value.atUnchecked (i); }
	}
    
private:
    static int countValidChannels (InitialUnit const& i00,
                                   InitialUnit const& i01,
                                   InitialUnit const& i02,
                                   InitialUnit const& i03,
                                   InitialUnit const& i04,
                                   InitialUnit const& i05,
                                   InitialUnit const& i06,
                                   InitialUnit const& i07,
                                   InitialUnit const& i08,
                                   InitialUnit const& i09,
                                   InitialUnit const& i10,
                                   InitialUnit const& i11,
                                   InitialUnit const& i12,
                                   InitialUnit const& i13,
                                   InitialUnit const& i14,
                                   InitialUnit const& i15) throw()
    {
        int numChannels = 0;
        
        if (i00.valid) numChannels += i00.value.getNumChannels();
        if (i01.valid) numChannels += i01.value.getNumChannels();
        if (i02.valid) numChannels += i02.value.getNumChannels();        
        if (i03.valid) numChannels += i03.value.getNumChannels();
        if (i04.valid) numChannels += i04.value.getNumChannels();
        if (i05.valid) numChannels += i05.value.getNumChannels();
        if (i06.valid) numChannels += i06.value.getNumChannels();
        if (i07.valid) numChannels += i07.value.getNumChannels();
        if (i08.valid) numChannels += i08.value.getNumChannels();
        if (i09.valid) numChannels += i09.value.getNumChannels();
        if (i10.valid) numChannels += i10.value.getNumChannels();
        if (i11.valid) numChannels += i11.value.getNumChannels();
        if (i12.valid) numChannels += i12.value.getNumChannels();
        if (i13.valid) numChannels += i13.value.getNumChannels();
        if (i14.valid) numChannels += i14.value.getNumChannels();
        if (i15.valid) numChannels += i15.value.getNumChannels();
        
        return numChannels;
    }
    
public:
    UnitBase (InitialUnit const &i00,
              InitialUnit const &i01,
              InitialUnit const &i02,
              InitialUnit const &i03,
              InitialUnit const &i04,
              InitialUnit const &i05,
              InitialUnit const &i06,
              InitialUnit const &i07,
              InitialUnit const &i08,
              InitialUnit const &i09 = InitialUnit(),
              InitialUnit const &i10 = InitialUnit(),
              InitialUnit const &i11 = InitialUnit(),
              InitialUnit const &i12 = InitialUnit(),
              InitialUnit const &i13 = InitialUnit(),
              InitialUnit const &i14 = InitialUnit(),
              InitialUnit const &i15 = InitialUnit()) throw()
	:	Base (NumericalArraySpec (countValidChannels (i00, i01, i02, i03,
                                                      i04, i05, i06, i07,
                                                      i08, i09, i10, i11,
                                                      i12, i13, i14, i15), false))
	{
        int i;
        ChannelType* array = this->getArray();
        
        if (i00.valid) { for (i = 0; i < i00.value.getNumChannels(); ++i) *array++ = i00.value.atUnchecked (i); }
        if (i01.valid) { for (i = 0; i < i01.value.getNumChannels(); ++i) *array++ = i01.value.atUnchecked (i); }
        if (i02.valid) { for (i = 0; i < i02.value.getNumChannels(); ++i) *array++ = i02.value.atUnchecked (i); }
        if (i03.valid) { for (i = 0; i < i03.value.getNumChannels(); ++i) *array++ = i03.value.atUnchecked (i); }
        if (i04.valid) { for (i = 0; i < i04.value.getNumChannels(); ++i) *array++ = i04.value.atUnchecked (i); }
        if (i05.valid) { for (i = 0; i < i05.value.getNumChannels(); ++i) *array++ = i05.value.atUnchecked (i); }
        if (i06.valid) { for (i = 0; i < i06.value.getNumChannels(); ++i) *array++ = i06.value.atUnchecked (i); }
        if (i07.valid) { for (i = 0; i < i07.value.getNumChannels(); ++i) *array++ = i07.value.atUnchecked (i); }
        if (i08.valid) { for (i = 0; i < i08.value.getNumChannels(); ++i) *array++ = i08.value.atUnchecked (i); }
        if (i09.valid) { for (i = 0; i < i09.value.getNumChannels(); ++i) *array++ = i09.value.atUnchecked (i); }
        if (i10.valid) { for (i = 0; i < i10.value.getNumChannels(); ++i) *array++ = i10.value.atUnchecked (i); }
        if (i11.valid) { for (i = 0; i < i11.value.getNumChannels(); ++i) *array++ = i11.value.atUnchecked (i); }
        if (i12.valid) { for (i = 0; i < i12.value.getNumChannels(); ++i) *array++ = i12.value.atUnchecked (i); }
        if (i13.valid) { for (i = 0; i < i13.value.getNumChannels(); ++i) *array++ = i13.value.atUnchecked (i); }
        if (i14.valid) { for (i = 0; i < i14.value.getNumChannels(); ++i) *array++ = i14.value.atUnchecked (i); }
        if (i15.valid) { for (i = 0; i < i15.value.getNumChannels(); ++i) *array++ = i15.value.atUnchecked (i); }
    }
    
private:
    static int countValidChannels (InitialUnit const& i00,
                                   InitialUnit const& i01,
                                   InitialUnit const& i02,
                                   InitialUnit const& i03,
                                   InitialUnit const& i04,
                                   InitialUnit const& i05,
                                   InitialUnit const& i06,
                                   InitialUnit const& i07,
                                   InitialUnit const& i08,
                                   InitialUnit const& i09,
                                   InitialUnit const& i10,
                                   InitialUnit const& i11,
                                   InitialUnit const& i12,
                                   InitialUnit const& i13,
                                   InitialUnit const& i14,
                                   InitialUnit const& i15,
                                   InitialUnit const& i16,
                                   InitialUnit const& i17,
                                   InitialUnit const& i18,
                                   InitialUnit const& i19,
                                   InitialUnit const& i20,
                                   InitialUnit const& i21,
                                   InitialUnit const& i22,
                                   InitialUnit const& i23,
                                   InitialUnit const& i24,
                                   InitialUnit const& i25,
                                   InitialUnit const& i26,
                                   InitialUnit const& i27,
                                   InitialUnit const& i28,
                                   InitialUnit const& i29,
                                   InitialUnit const& i30,
                                   InitialUnit const& i31) throw()
    {
        int numChannels = 0;
        
        if (i00.valid) numChannels += i00.value.getNumChannels();
        if (i01.valid) numChannels += i01.value.getNumChannels();
        if (i02.valid) numChannels += i02.value.getNumChannels();        
        if (i03.valid) numChannels += i03.value.getNumChannels();
        if (i04.valid) numChannels += i04.value.getNumChannels();
        if (i05.valid) numChannels += i05.value.getNumChannels();
        if (i06.valid) numChannels += i06.value.getNumChannels();
        if (i07.valid) numChannels += i07.value.getNumChannels();
        if (i08.valid) numChannels += i08.value.getNumChannels();
        if (i09.valid) numChannels += i09.value.getNumChannels();
        if (i10.valid) numChannels += i10.value.getNumChannels();
        if (i11.valid) numChannels += i11.value.getNumChannels();
        if (i12.valid) numChannels += i12.value.getNumChannels();
        if (i13.valid) numChannels += i13.value.getNumChannels();
        if (i14.valid) numChannels += i14.value.getNumChannels();
        if (i15.valid) numChannels += i15.value.getNumChannels();
        if (i16.valid) numChannels += i16.value.getNumChannels();
        if (i17.valid) numChannels += i17.value.getNumChannels();
        if (i18.valid) numChannels += i18.value.getNumChannels();
        if (i19.valid) numChannels += i19.value.getNumChannels();
        if (i20.valid) numChannels += i20.value.getNumChannels();
        if (i21.valid) numChannels += i21.value.getNumChannels();
        if (i22.valid) numChannels += i22.value.getNumChannels();
        if (i23.valid) numChannels += i23.value.getNumChannels();
        if (i24.valid) numChannels += i24.value.getNumChannels();
        if (i25.valid) numChannels += i25.value.getNumChannels();
        if (i26.valid) numChannels += i26.value.getNumChannels();
        if (i27.valid) numChannels += i27.value.getNumChannels();
        if (i28.valid) numChannels += i28.value.getNumChannels();
        if (i29.valid) numChannels += i29.value.getNumChannels();
        if (i30.valid) numChannels += i30.value.getNumChannels();
        if (i31.valid) numChannels += i31.value.getNumChannels();
        
        return numChannels;
    }
    
public:
    UnitBase (InitialUnit const &i00,
              InitialUnit const &i01,
              InitialUnit const &i02,
              InitialUnit const &i03,
              InitialUnit const &i04,
              InitialUnit const &i05,
              InitialUnit const &i06,
              InitialUnit const &i07,
              InitialUnit const &i08,
              InitialUnit const &i09,
              InitialUnit const &i10,
              InitialUnit const &i11,
              InitialUnit const &i12,
              InitialUnit const &i13,
              InitialUnit const &i14,
              InitialUnit const &i15,
              InitialUnit const &i16,
              InitialUnit const &i17 = InitialUnit(),
              InitialUnit const &i18 = InitialUnit(),
              InitialUnit const &i19 = InitialUnit(),
              InitialUnit const &i20 = InitialUnit(),
              InitialUnit const &i21 = InitialUnit(),
              InitialUnit const &i22 = InitialUnit(),
              InitialUnit const &i23 = InitialUnit(),
              InitialUnit const &i24 = InitialUnit(),
              InitialUnit const &i25 = InitialUnit(),
              InitialUnit const &i26 = InitialUnit(),
              InitialUnit const &i27 = InitialUnit(),
              InitialUnit const &i28 = InitialUnit(),
              InitialUnit const &i29 = InitialUnit(),
              InitialUnit const &i30 = InitialUnit(),
              InitialUnit const &i31 = InitialUnit()) throw()
	:	Base (NumericalArraySpec (countValidChannels (i00, i01, i02, i03,
                                                      i04, i05, i06, i07,
                                                      i08, i09, i10, i11,
                                                      i12, i13, i14, i15,
                                                      i16, i17, i18, i19,
                                                      i20, i21, i22, i23,
                                                      i24, i25, i26, i27, 
                                                      i28, i29, i30, i31), false))
	{
        int i;
        ChannelType* array = this->getArray();
        
        if (i00.valid) { for (i = 0; i < i00.value.getNumChannels(); ++i) *array++ = i00.value.atUnchecked (i); }
        if (i01.valid) { for (i = 0; i < i01.value.getNumChannels(); ++i) *array++ = i01.value.atUnchecked (i); }
        if (i02.valid) { for (i = 0; i < i02.value.getNumChannels(); ++i) *array++ = i02.value.atUnchecked (i); }
        if (i03.valid) { for (i = 0; i < i03.value.getNumChannels(); ++i) *array++ = i03.value.atUnchecked (i); }
        if (i04.valid) { for (i = 0; i < i04.value.getNumChannels(); ++i) *array++ = i04.value.atUnchecked (i); }
        if (i05.valid) { for (i = 0; i < i05.value.getNumChannels(); ++i) *array++ = i05.value.atUnchecked (i); }
        if (i06.valid) { for (i = 0; i < i06.value.getNumChannels(); ++i) *array++ = i06.value.atUnchecked (i); }
        if (i07.valid) { for (i = 0; i < i07.value.getNumChannels(); ++i) *array++ = i07.value.atUnchecked (i); }
        if (i08.valid) { for (i = 0; i < i08.value.getNumChannels(); ++i) *array++ = i08.value.atUnchecked (i); }
        if (i09.valid) { for (i = 0; i < i09.value.getNumChannels(); ++i) *array++ = i09.value.atUnchecked (i); }
        if (i10.valid) { for (i = 0; i < i10.value.getNumChannels(); ++i) *array++ = i10.value.atUnchecked (i); }
        if (i11.valid) { for (i = 0; i < i11.value.getNumChannels(); ++i) *array++ = i11.value.atUnchecked (i); }
        if (i12.valid) { for (i = 0; i < i12.value.getNumChannels(); ++i) *array++ = i12.value.atUnchecked (i); }
        if (i13.valid) { for (i = 0; i < i13.value.getNumChannels(); ++i) *array++ = i13.value.atUnchecked (i); }
        if (i14.valid) { for (i = 0; i < i14.value.getNumChannels(); ++i) *array++ = i14.value.atUnchecked (i); }
        if (i15.valid) { for (i = 0; i < i15.value.getNumChannels(); ++i) *array++ = i15.value.atUnchecked (i); }
        if (i16.valid) { for (i = 0; i < i16.value.getNumChannels(); ++i) *array++ = i16.value.atUnchecked (i); }
        if (i17.valid) { for (i = 0; i < i17.value.getNumChannels(); ++i) *array++ = i17.value.atUnchecked (i); }
        if (i18.valid) { for (i = 0; i < i18.value.getNumChannels(); ++i) *array++ = i18.value.atUnchecked (i); }
        if (i19.valid) { for (i = 0; i < i19.value.getNumChannels(); ++i) *array++ = i19.value.atUnchecked (i); }
        if (i20.valid) { for (i = 0; i < i20.value.getNumChannels(); ++i) *array++ = i20.value.atUnchecked (i); }
        if (i21.valid) { for (i = 0; i < i21.value.getNumChannels(); ++i) *array++ = i21.value.atUnchecked (i); }
        if (i22.valid) { for (i = 0; i < i22.value.getNumChannels(); ++i) *array++ = i22.value.atUnchecked (i); }
        if (i23.valid) { for (i = 0; i < i23.value.getNumChannels(); ++i) *array++ = i23.value.atUnchecked (i); }
        if (i24.valid) { for (i = 0; i < i24.value.getNumChannels(); ++i) *array++ = i24.value.atUnchecked (i); }
        if (i25.valid) { for (i = 0; i < i25.value.getNumChannels(); ++i) *array++ = i25.value.atUnchecked (i); }
        if (i26.valid) { for (i = 0; i < i26.value.getNumChannels(); ++i) *array++ = i26.value.atUnchecked (i); }
        if (i27.valid) { for (i = 0; i < i27.value.getNumChannels(); ++i) *array++ = i27.value.atUnchecked (i); }
        if (i28.valid) { for (i = 0; i < i28.value.getNumChannels(); ++i) *array++ = i28.value.atUnchecked (i); }
        if (i29.valid) { for (i = 0; i < i29.value.getNumChannels(); ++i) *array++ = i29.value.atUnchecked (i); }
        if (i30.valid) { for (i = 0; i < i30.value.getNumChannels(); ++i) *array++ = i30.value.atUnchecked (i); }
        if (i31.valid) { for (i = 0; i < i31.value.getNumChannels(); ++i) *array++ = i31.value.atUnchecked (i); }
    }
    
    UnitBase (ChannelArrayType const& copy) throw()
	:	UnitType (static_cast<UnitType const&> (copy))
	{
	}    
    
    /** Assignment operator. */
    UnitBase& operator= (UnitBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    /** Assignment operator from a single channel. */
    UnitBase& operator= (ChannelType const& other) throw()
	{
		return operator= (UnitBase (other));
	}
    
    /** Assignment operator from the base array type. */
    UnitBase& operator= (UnitType const& other) throw()
	{
		return operator= (static_cast<UnitBase const&> (other));
	}
    
    /** Assignment operator from an array of channels. */
    UnitBase& operator= (ObjectArray<ChannelType> const& other) throw()
	{
		return operator= (static_cast<UnitBase const&> (other));
	}    
    
    /** Cast to a single channel. */
    PLONK_INLINE_LOW operator const ChannelType& () const throw()
    {
        plonk_assert (this->getNumChannels() == 1);
        return this->atUnchecked (0);
    }
    
    /** Cast to a single channel. */
    PLONK_INLINE_LOW operator ChannelType& () throw()
    {
        plonk_assert (this->getNumChannels() == 1);
        return this->atUnchecked (0);
    }
    
    /** Primary means of creating Unit objects.
     Used with most non-proxy-owning ChannelInternal classes. */
    template<class ChannelInternalClassType>
    static PLONK_INLINE_LOW UnitBase createFromInputs (Inputs const& inputs, 
                                             typename ChannelInternalClassType::Data const& data,
                                             BlockSize const& preferredBlockSize,
                                             SampleRate const& preferredSampleRate) throw()
    {        
        Inputs mainInputs = inputs;
        mainInputs.resetExpiredUnits();

        const Dynamic add = mainInputs.remove (IOKey::Add);
        const Dynamic mul = mainInputs.remove (IOKey::Multiply);
                        
        const int numChannels = mainInputs.getMaxNumChannels();
        UnitBase result (UnitBase::withSize (numChannels));
        
        for (int i = 0; i < numChannels; ++i) 
        {
            ChannelInternalType* internal = new ChannelInternalClassType (mainInputs, 
                                                                          data, 
                                                                          preferredBlockSize, 
                                                                          preferredSampleRate);
            
            plonk_assert ((internal->isProxyOwner() == false) || (internal->getNumChannels() == 1));

//            internal->initChannel (i);
            
            result.put (i, ChannelType (internal));
        }
        
        return applyMulAdd (result, UnitBase (mul), UnitBase (add));
    }     
    
    /** Primary means of creating Unit objects with proxies.
     Used with most proxy-owning ChannelInternal classes. */
    template<class ProxyOwnerChannelInternalClassType>
    static PLONK_INLINE_LOW UnitBase proxiesFromInputs (Inputs const& inputs, 
                                              typename ProxyOwnerChannelInternalClassType::Data const& data,
                                              BlockSize const& preferredBlockSize,
                                              SampleRate const& preferredSampleRate) throw()
    {        
        Inputs mainInputs = inputs;
        mainInputs.resetExpiredUnits();
        
        const Dynamic add = mainInputs.remove (IOKey::Add);
        const Dynamic mul = mainInputs.remove (IOKey::Multiply);
                
        ChannelArrayType channels;        
        new ProxyOwnerChannelInternalClassType (mainInputs, 
                                                data, 
                                                preferredBlockSize, 
                                                preferredSampleRate, 
                                                channels);
        const int numChannels = channels.length();
        for (int i = 0; i < numChannels; ++i)
            channels.atUnchecked (i).initChannel (i);
        
        UnitBase result (channels);        
        return applyMulAdd (result, UnitBase (mul), UnitBase (add));
    }        
    
    /** Apply multiply and addition operators to another unit.
     Here @c mul will be ignored (not applied) if the value is a single channel 
     constant equal to 1 (or null rather than just a constant 0). And @c add 
     will be ignored (not applied) if it is a single channel constant equal to
     0 (or null). On some platforms this might be implemented as a single
     multiply-and-add operation. */
    static PLONK_INLINE_LOW UnitBase applyMulAdd (UnitBase const& mainUnit, 
                                        UnitBase const& mul,
                                        UnitBase const& add) throw()
    {
        UnitBase result = mainUnit;
        
        bool hasMul = true;
        bool hasAdd = true;
                                
        if (mul.isNull() || (mul.isConstant() && (mul.getValue (0) == SampleType (1))))
            hasMul = false;
        
        if (add.isNull() || (add.isConstant() && (add.getValue (0) == SampleType (0))))
            hasAdd = false;
        
        if (hasMul && hasAdd)
            result = MulAddUnit<SampleType>::ar (result, mul, add, BlockSize::noPreference(), SampleRate::noPreference());
        else if (hasMul)
            result *= mul;
        else if (hasAdd)
            result += add;
            
        return result;        
    }
    
    /** Resamples this unit to a different sample rate and/or block size. 
     By default the default sample rate and default block size are used. */
    UnitBase ar (const Interp::TypeCode interpType,
                 BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                 SampleRate const& preferredSampleRate = SampleRate::getDefault()) const throw()
    {
        typedef UnitBase<IndexType> RateUnitType;
        const RateUnitType& rateOne (Math<RateUnitType>::get1());

        switch (interpType)
        {
            case Interp::Linear:    return ResampleUnit<SampleType,Interp::Linear>::ar (*this, rateOne, preferredBlockSize, preferredSampleRate);
            case Interp::Lagrange3: return ResampleUnit<SampleType,Interp::Lagrange3>::ar (*this, rateOne, preferredBlockSize, preferredSampleRate);
            default:                return ResampleUnit<SampleType,Interp::Linear>::ar (*this, rateOne, preferredBlockSize, preferredSampleRate);
        }
    }
    
    /** Resamples this unit to a different sample rate and/or block size.
     By default the default sample rate and default block size are used. */
    PLONK_INLINE_LOW UnitBase ar() const throw()
    {
        return ResampleUnit<SampleType,Interp::Linear>::ar (*this);
    }
    
    /** Resamples this unit to the default control rate sample rate and block size. */
    UnitBase kr (const Interp::TypeCode interpType) const throw()
    {        
        switch (interpType)
        {
            case Interp::Linear:    return ResampleUnit<SampleType,Interp::Linear>::kr (*this);
            case Interp::Lagrange3: return ResampleUnit<SampleType,Interp::Lagrange3>::kr (*this);
            default:                return ResampleUnit<SampleType,Interp::Linear>::kr (*this);
        }
    }
    
    /** Resamples this unit to the default control rate sample rate and block size. */
    PLONK_INLINE_LOW UnitBase kr() const throw()
    {
        return ResampleUnit<SampleType,Interp::Linear>::kr (*this);
    }

    PLONK_INLINE_LOW UnitBase reblock (BlockSize const& newBlockSize) const throw()
    {
        return ReblockUnit<SampleType>::ar (*this, newBlockSize);
    }
    
    PLONK_INLINE_LOW UnitBase reblock() const throw()
    {
        return ReblockUnit<SampleType>::ar (*this, BlockSize::getDefault());
    }
    
    PLONK_INLINE_LOW UnitBase lag (UnitBase const& duration) const throw()
    {
        return LagUnit<SampleType>::ar (*this, duration);
    }

    PLONK_INLINE_LOW UnitBase lag() const throw()
    {
        return LagUnit<SampleType>::ar (*this);
    }
    
    PLONK_INLINE_LOW UnitBase dc (UnitBase const& control) const throw()
    {
        return DCUnit<SampleType>::ar (*this, control);
    }
    
    PLONK_INLINE_LOW UnitBase dc() const throw()
    {
        return DCUnit<SampleType>::ar (*this);
    }

    /** Mixes this unit down to a single channel. */
    PLONK_INLINE_LOW UnitBase mix() const throw()
    {
        return MixerUnit<SampleType>::ar (*this, true);
    }
    
    /** Mixes this unit down to a single channel with and auto-deletion prevention barrier. 
     This prevents things like envelopes from releasing the mixer that contains this unit. */
    PLONK_INLINE_LOW UnitBase mixBarrier() const throw()
    {
        return MixerUnit<SampleType>::ar (*this, false);
    }
    
    PLONK_INLINE_LOW UnitBase diff() const throw()
    {
        return DiffUnit<SampleType>::ar (*this);
    }
    
    /** Create an overlapping process from this unit's contrinuous stream. */
    UnitBase overlapMake (DoubleVariable const& overlap = Math<DoubleVariable>::get0_5(), const bool zeroPad = false) const throw()
    {
        return OverlapMakeUnit<SampleType>::ar (*this, overlap, zeroPad);
    }
    
    /** Create an overlapping process from this unit's contrinuous stream, zero-padding the overlapping sections. */
    UnitBase overlapZeroPad (DoubleVariable const& overlap = Math<DoubleVariable>::get0_5()) const throw()
    {
        return OverlapMakeUnit<SampleType>::ar (*this, overlap, true);
    }
    
    /** Mix down overlapping process to a continuous stream. */
    UnitBase overlapMix (DoubleVariable const& overlap = Math<DoubleVariable>::get0_5()) const throw()
    {
        return OverlapMixUnit<SampleType>::ar (*this, overlap);
    }
    
    /** Templated binary operator Unit creator. */
    template<PLONK_BINARYOPFUNCTION(SampleType, op)>
    UnitBase binary (UnitBase const& rightOperand) const throw() 
    {        
        typedef BinaryOpChannelInternal<SampleType,op>      ChannelInternalClassType;
        typedef typename ChannelInternalClassType::Data     Data;
                
        Inputs inputs;
        inputs.put (IOKey::LeftOperand, *this);
        inputs.put (IOKey::RightOperand, rightOperand);
        
        Data data = { -1.0, -1.0 }; // dummy sample rate data
        
        return createFromInputs<ChannelInternalClassType> (inputs, 
                                                           data, 
                                                           BlockSize::noPreference(), 
                                                           SampleRate::noPreference());
    }

    /** Templated unary operator Unit creator. */
    template<PLONK_UNARYOPFUNCTION(SampleType, op)>
    UnitBase unary() const throw() 
    {        
        typedef UnaryOpChannelInternal<SampleType,op>       ChannelInternalClassType;
        typedef typename ChannelInternalClassType::Data     Data;
        
        Inputs inputs;
        inputs.put (IOKey::Generic, *this);
        
        Data data = { -1.0, -1.0 }; // dummy sample rate data
        
        return createFromInputs<ChannelInternalClassType> (inputs, 
                                                           data, 
                                                           BlockSize::noPreference(), 
                                                           SampleRate::noPreference());
    }
     
    PLONK_BINARYOPS(UnitBase);
    PLONK_UNARYOPS(UnitBase);
    
    /** Linear to linear mapping. */
    PLONK_INLINE_LOW UnitBase linlin (UnitBase const& inLow, UnitBase const& inHigh,
                            UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linlin (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to linear mapping assuming the input is in the range -1...+1. */
    PLONK_INLINE_LOW UnitBase linlin (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        const SampleType peak2peak (peak * Math<SampleType>::get2());
        return plonk::linlin2 (*this, 
                               UnitBase (-peak), UnitBase (peak2peak), 
                               outLow, (outHigh - outLow));
    }
    
    /** Linear to exponential mapping. 
     Note that the output range must not cross or meet zero. */
    PLONK_INLINE_LOW UnitBase linexp (UnitBase const& inLow, UnitBase const& inHigh,
                            UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linexp (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to exponential mapping assuming the input is in the range -1...+1. 
     Note that the output range must not cross or meet zero. */
    PLONK_INLINE_LOW UnitBase linexp (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        const SampleType peak2peak (peak * Math<SampleType>::get2());
        const SampleType reciprocalInRange (plonk::reciprocal (peak2peak));
        return plonk::linexp2 (*this, 
                               UnitBase (reciprocalInRange), UnitBase (-peak * reciprocalInRange), 
                               outLow, (outHigh / outLow));
    }
    
    /** Linear to sinusoidal mapping. */
    PLONK_INLINE_LOW UnitBase linsin (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linsin (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to sinusoidal mapping assuming the input is in the range -1...+1. */
    PLONK_INLINE_LOW UnitBase linsin (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        return plonk::linsin2 (*this, UnitBase (-peak), UnitBase (peak), outLow, outHigh);
    }
    
    /** Linear to Welch curve mapping. */
    PLONK_INLINE_LOW UnitBase linwelch (UnitBase const& inLow, UnitBase const& inHigh, 
                       UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linwelch (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to Welch curve mapping assuming the input is in the range -1...+1. */
    PLONK_INLINE_LOW UnitBase linwelch (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        return plonk::linwelch (*this, UnitBase (-peak), UnitBase (peak), outLow, outHigh);
    }
    
    /** Exponential to linear mapping. 
     Note that the intput range must not cross or meet zero. */
    PLONK_INLINE_LOW UnitBase explin (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::explin (*this, inLow, inHigh, outLow, outHigh);
    }
        
//    /** Create a array of units by concatenation. */
//    const UnitArray operator<< (UnitType const& other) const throw()   { return UnitArray (*this, other); }
//    
//    /** Create a array of units by concatenation. */
//	const UnitArray operator<< (UnitBase const& other) const throw()   { return UnitArray (*this, other); }
    
    /** Concatenate. */
    UnitBase operator, (UnitType const& other) const throw();
    UnitBase operator, (UnitBase const& other) const throw();
    
    /** Enables a custom name to be set.
     This conventiently returns this unit so it can be used in nested graph construction functions. */
    UnitBase& setLabel (Text const& unitId) throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i) 
            channels[i].setLabel (unitId);        
            
        return *this;
    }

    /** Gets the number of channels in this unit. */
    PLONK_INLINE_LOW int getNumChannels() const throw() { return this->length(); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is also recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */
    PLONK_INLINE_LOW UnitBase getChannel (const int index) throw() { return this->wrapAt (index).getChannel (index); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is also recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */    
    PLONK_INLINE_LOW UnitBase getChannel (const int index) const throw() { return this->copy().wrapAt (index).getChannel (index); }

    /** Returns a single channel object.
     This wraps the index so that it is always in range. It is also recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */
    PLONK_INLINE_LOW ChannelType getChannelObject (const int index) throw() { return this->wrapAt (index).getChannel (index); }
    
    /** Returns a single channel object.
     This wraps the index so that it is always in range. It is also recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */
    PLONK_INLINE_LOW ChannelType getChannelObject (const int index) const throw() { return this->copy().wrapAt (index).getChannel (index); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is also recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */    
    UnitBase operator[] (const int index) throw() { return this->getChannel (index); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is also recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */    
    UnitBase operator[] (const int index) const throw() { return this->getChannel (index); }
    
    /** Set a particular channel.
     Note that the input channel here must contain only one channel. */
    UnitBase& put (const int index, UnitBase const& channel) throw()
    {
        plonk_assert (channel.getNumChannels() == 1); // channel MUST be only a single channel
        plonk_assert (index >= 0);
        plonk_assert (index < this->getNumChannels());
        
        this->getArray() [index] = channel.atUnchecked (0);
        return *this;
    }
    
    UnitBase flatten() const throw()
    {
        const int numChannels = this->getNumChannels();
        UnitBase result = UnitBase::emptyChannels(numChannels);
        
        for (int i = 0; i < numChannels; ++i)
            result.put (i, this->getChannel (i));
        
        return result;
    }
    
    UnitArray group (const int groupSize) const throw()
    {
		return this->flatten().Base::group (groupSize);
    }
    
    /** Get the minimum block size in the array of channels in this unit. */
    BlockSize getMinBlockSize() const throw();
    
    /** Get the maximum block size in the array of channels in this unit. */
    BlockSize getMaxBlockSize() const throw();
        
    /** Get the block size of a specific channel in this unit. */
    PLONK_INLINE_LOW BlockSize getBlockSize (const int index) const throw()            { return this->wrapAt (index).getBlockSize(); }

    /** Set the block size of all channels in this unit. */
    void setBlockSize (BlockSize const& newBlockSize) throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i) 
            channels[i].setBlockSize (newBlockSize);        
    }
    
    /** Get an array of block sizes for the channels in the unit. */
    BlockSizes getBlockSizes() const throw()
    {
        BlockSizes result (BlockSizes::withSize (this->getNumChannels()));
        
        for (int i = 0; i < this->getNumChannels(); ++i)
            result.put (i, this->getBlockSize (i));
        
        return result;
    }
    
    /** Get the minimum sample rate in the array of channels in this unit. */
    SampleRate getMinSampleRate() const throw();
    
    /** Get the maximum sample rate in the array of channels in this unit. */
    SampleRate getMaxSampleRate() const throw();
        
    /** Get the sample rate of a specific channel in this unit.  
     Indices out of range will be wrapped to the available channels. */
    PLONK_INLINE_LOW SampleRate getSampleRate (const int index) const throw()          { return this->wrapAt (index).getSampleRate(); }
    
    /** Set the sample rate of all channels in this unit. */
    void setSampleRate (SampleRate const& newSampleRate) throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i) 
            channels[i].setSampleRate (newSampleRate);        
    }   
    
    /** Get an array of sample rates for the channels in the unit. */
    SampleRates getSampleRates() const throw()
    {
        SampleRates result (SampleRates::withSize (this->getNumChannels()));
        
        for (int i = 0; i < this->getNumChannels(); ++i)
            result.put (i, this->getSampleRate (i));
        
        return result;
    }
    
    PLONK_INLINE_LOW double getSampleDurationInTicks (const int index) const throw()         { return this->wrapAt (index)->getSampleDurationInTicks(); }
    PLONK_INLINE_LOW double getBlockDurationInTicks (const int index) const throw()          { return this->wrapAt (index)->getBlockDurationInTicks(); }
    
    PLONK_INLINE_LOW const DoubleVariable& getOverlap (const int index) const throw()        { return this->wrapAt (index).getOverlap(); }
    PLONK_INLINE_LOW DoubleVariable& getOverlap (const int index) throw()                    { return this->wrapAt (index).getOverlap(); }
    
    PLONK_INLINE_LOW bool channelsHaveSameOverlap() const throw()
    {
        const int numChannels = this->getNumChannels();
        plonk_assert (numChannels > 0);

        const ChannelType* channels = this->getArray();
        DoubleVariable overlap = channels[0].getOverlap();
        
        for (int i = 1; i < numChannels; ++i)
            if (channels[i].getOverlap() != overlap)
                return false;
        
        return true;
    }
    
    PLONK_INLINE_LOW bool channelsHaveSameBlockSize() const throw()
    {
        const int numChannels = this->getNumChannels();
        plonk_assert (numChannels > 0);
        
        const ChannelType* channels = this->getArray();
        BlockSize blockSize = channels[0].getBlockSize();
        
        for (int i = 1; i < numChannels; ++i)
            if (channels[i].getBlockSize() != blockSize)
                return false;
        
        return true;
    }
    
    PLONK_INLINE_LOW bool channelsHaveSameSampleRate() const throw()
    {
        const int numChannels = this->getNumChannels();
        plonk_assert (numChannels > 0);
        
        const ChannelType* channels = this->getArray();
        SampleRate sampleRate = channels[0].getSampleRate();
        
        for (int i = 1; i < numChannels; ++i)
            if (channels[i].getSampleRate() != sampleRate)
                return false;
        
        return true;
    }
    
    /** Get the output buffer of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    PLONK_INLINE_LOW const Buffer& getOutputBuffer (const int index) const throw() 
    { 
        return this->wrapAt (index).getOutputBuffer();
    }
    
    /** Get the output buffer of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    PLONK_INLINE_LOW Buffer& getOutputBuffer (const int index) throw() 
    { 
        return this->wrapAt (index).getOutputBuffer();
    }
    
    /** Get a pointer to the raw samples of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    PLONK_INLINE_LOW const SampleType* getOutputSamples (const int index) const throw()
    { 
        return this->wrapAt (index)->getOutputSamples(); 
    }
    
    /** Get a pointer to the raw samples of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    PLONK_INLINE_LOW SampleType* getOutputSamples (const int index) throw()
    { 
        return this->wrapAt (index)->getOutputSamples(); 
    }
    
    /** Set the output buffer of a specific channel to use an external buffer. */
    PLONK_INLINE_LOW void setOutputBuffer (const int index, Buffer const& externalBuffer) throw()
    {
        this->atUnchecked (index).setOutputBuffer (externalBuffer);
    }
    
    /** Set the output buffers of this unit's channels to use external buffers. */
    void setOutputBuffer (BufferArrayType const& externalBuffer) throw()
    {
        const int numBufferChannels = externalBuffer.length();
        const Buffer* bufferChannels = externalBuffer.getArray();
        
        for (int i = 0; i < numBufferChannels; ++i) 
            setOutputBuffer (i, bufferChannels[i]);
    }
    
    BufferArrayType getOutputBuffers() const throw()
    {
        BufferArrayType buffers;
        const int numChannels = this->getNumChannels();

        for (int i = 0; i < numChannels; ++i)
            buffers.add (this->atUnchecked (i).getOutputBuffer());
        
        return buffers;
    }
    
    /** Get the current value of a specific channel in this unit.
     Indices out of range will be wrapped to the available channels. */
    PLONK_INLINE_LOW const SampleType& getValue (const int index) const throw()
    {
        return this->wrapAt (index).getValue();
    }
    
    PLONK_INLINE_LOW const Buffer getValues() const throw()
    {
        const int numChannels = this->getNumChannels();
        Buffer result = Buffer::withSize (numChannels);
        SampleType* const resultArray = result.getArray();
        
        for (int i = 0; i < numChannels; ++i)
            resultArray[i] = this->atUnchecked (i).getValue();
        
        return result;
    }
    
    PLONK_INLINE_LOW const TimeStamp getNextTimeStamp (const int index) const throw()
    {
        return this->wrapAt (index).getNextTimeStamp();
    }
    
    PLONK_INLINE_LOW UnitBase setNull() throw()
    {
        this->operator= (UnitBase::getNull());
        return *this;
    }
    
    PLONK_INLINE_LOW bool isNull (const int index) const throw()
    {
        return this->wrapAt (index).isNull();
    }
    
    PLONK_INLINE_LOW bool isNotNull (const int index) const throw()
    {
        return ! this->isNull (index);
    }
    
    PLONK_INLINE_LOW bool isConstant (const int index) const throw()
    {
        return this->wrapAt (index).isConstant();
    }
    
    PLONK_INLINE_LOW bool isEachChannelConstant() const throw()
    {        
        for (int i = 0; i < this->getNumChannels(); ++i)
            if (! this->atUnchecked (i).isConstant())
                return false;
        
        return true;
    }
    
    PLONK_INLINE_LOW bool isNotConstant (const int index) const throw()
    {
        return ! this->isConstant (index);
    }    
    
    /** Returns @c true if this unit contains only a single null channel. */
    PLONK_INLINE_LOW bool isNull() const throw()
    {
        if (this->getNumChannels() != 1)
            return false;
        else if (this->at(0).isNull())
            return true;
        else
            return false;
    }
    
    /** Returns @c false if this unit contains only a single null channel. */
    PLONK_INLINE_LOW bool isNotNull() const throw()
    {
        return ! this->isNull();
    }
    
    /** Returns @c true if this unit contains only a single constant channel. */
    PLONK_INLINE_LOW bool isConstant() const throw()
    {
        if (this->getNumChannels() != 1)
            return false;
        else if (this->at (0).isConstant())
            return true;
        else
            return false;
    }
    
    /** Returns @c false if this unit contains only a single constant channel. */
    PLONK_INLINE_LOW bool isNotConstant() const throw()
    {
        return ! this->isConstant();
    }    
    
    PLONK_INLINE_LOW void setToNull() throw()
    {
        this->setSize (1, true);
        this->put (0, ChannelType::getNull());
    }
    
    PLONK_INLINE_LOW TextArray getNames() throw()
    {
        TextArray names;
        for (int i = 0; i < this->getNumChannels(); ++i)
            names.add (this->at (i).getName());
        return names;
    }
    
    /** Returns @c true if this unit needs to process for the given timestamp. */
    PLONK_INLINE_LOW bool needsToProcess (ProcessInfo const& info, const int channel) const throw()
    {        
        return this->wrapAt (channel).needsToProcess (info, channel);
    }
    
    /** Returns @c true if this unit (i.e., one of its channels) needs to be deleted becasue it has. */
    PLONK_INLINE_LOW bool shouldBeDeletedNow (ProcessInfo const& info) const throw()
    {
        bool flag = false;
        
        const int numChannels = this->getNumChannels();
        const ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i)
        {
            if (channels[i].shouldBeDeletedNow (info.getTimeStamp()))
            {
                flag = true;
                break;
            }
        }
        
        return flag;
    }
    
    void resetIfExpired() throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i)
            channels[i].resetIfExpired();
    }
    
    void resetTimeStamps() throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i)
            channels[i].resetTimeStamps();
    }
    
    /** Process a specific channel in this unit.
     The host should prepare a ProcessInfo which is passed to this function
     for each required block of data. This is generally used by ChannelInternal
     subclasses when obtaining input data.
     @return The buffer from the requested channel. */
    PLONK_INLINE_LOW const Buffer& process (ProcessInfo& info, const int channel) throw()
    {
        this->wrapAt (channel).process (info, channel);
        return this->getOutputBuffer (channel);
    }
    
    /** Process all channels in this unit.
     The host should prepare a ProcessInfo which is passed to this function
     for each required block of data. */
    void process (ProcessInfo& info) throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i)
            channels[i].process (info, i);
    }
    
    void process (BufferArrayType& destBuffer) throw()
    {
        resetTimeStamps();
        const int numDestChannels = destBuffer.length();

        ObjectArray<ProcessInfo> infos (ObjectArray<ProcessInfo>::withSize (numDestChannels));
        
        IntArray countDown (IntArray::withSize (numDestChannels));
        IntArray countUp (IntArray::newClear (numDestChannels));
        
        for (int i = 0; i < numDestChannels; ++i)
            countDown.put (i, destBuffer.atUnchecked (i).length());
        
        while (countDown.sum() > 0)
        {
            int numSamplesThisTime = 0;
            
            for (int i = 0; i < numDestChannels; ++i)
            {
                const Buffer& channelBuffer (this->process (infos.atUnchecked (i), i));
                const SampleType* const channelBufferSamples (channelBuffer.getArray());
                SampleType* const destBufferSamples (destBuffer.atUnchecked (i).getArray());
                
                numSamplesThisTime = plonk::min (channelBuffer.length(), countDown.atUnchecked (i));
                
                if (numSamplesThisTime > 0)
                {
                    memcpy (destBufferSamples + countUp.atUnchecked (i), channelBufferSamples, numSamplesThisTime * sizeof (SampleType));
                    countDown.atUnchecked (i) -= numSamplesThisTime;
                    countUp.atUnchecked (i)   += numSamplesThisTime;
                }
                
                infos.atUnchecked (i).offsetTimeStamp (this->getSampleRate (i).getSampleDurationInTicks() * numSamplesThisTime);
            }
        }
    }

    
    int getTypeCode() const throw()
    {
        return TypeUtility<UnitBase>::getTypeCode();
    }
    
    int getSampleTypeCode() const throw()
    {
        return TypeUtility<SampleType>::getTypeCode();
    }          
    
    typename TypeUtility<SampleType>::PeakType getTypePeak() const throw()
    {
        return TypeUtility<SampleType>::getTypePeak();
    }
    
    typename TypeUtility<SampleType>::IndexType getTypeScale() const throw()
    {
        return TypeUtility<SampleType>::getTypeScale();
    }
    
    void addReceiverToChannels (Receiver* const receiver)
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();

        for (int i = 0; i < numChannels; ++i)
            channels[i].getInternal()->addReceiver (receiver);
    }
    
    void removeReceiverFromChannels (Receiver* const receiver)
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i)
            channels[i].getInternal()->removeReceiver (receiver);
    }

    
    PLONK_OBJECTARROWOPERATOR(UnitBase);
    
private:
    UnitBase (BlockSize const& blockSize) throw()
    :   UnitType (ChannelType())
    {
        (void)blockSize;
        // you must have got your arguments mixed up, perhaps you tried to 
        // set a unit block size but missed out mul or add arguments? 
        plonk_assertfalse; 
    }
    
    UnitBase (SampleRate const& sampleRate) throw()
    :   UnitType (ChannelType())
    {
        (void)sampleRate;
        // you must have got your arguments mixed up, perhaps you tried to 
        // set a unit sample rate but missed out mul or add arguments? 
        plonk_assertfalse;
    }        
};

PLONK_BINARYOPGLOBALS_TEMPLATE(UnitBase,SampleType);         // declares global functions with the same name as the binary operators
PLONK_BINARYOPGLOBALS_LITERAL_TEMPLATE(UnitBase,SampleType); // declares global functions with the same name as the binary operators for left operators that are literals
PLONK_UNARYOPGLOBALS_TEMPLATE(UnitBase,SampleType);          // declares global functions with the same name as the unary operators

//template<class SampleType>
//PLONK_INLINE_LOW UnitBase<SampleType> explin (UnitBase<SampleType> const& input, 
//                                    UnitBase<SampleType> const& inLow, UnitBase<SampleType> const& inHigh, 
//                                    UnitBase<SampleType> const& outLow, UnitBase<SampleType> const& outHigh) throw()
//{
//    const UnitBase<SampleType> clipped (clip (input, inLow, inHigh));
//    return log (clipped / inLow) / log (inHigh / inLow) * (outHigh - outLow) + outLow;
//}

/** Resamples a unit to a different sample rate and/or block size. 
 By default the default sample rate and default block size are used. */
template<class SampleType>
UnitBase<SampleType> ar (UnitBase<SampleType> const& unit,
                         BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                         SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
{
    return unit.ar (preferredBlockSize, preferredSampleRate);
}    

/** Resamples a unit to the default control rate sample rate and block size. */
template<class SampleType>
PLONK_INLINE_LOW UnitBase<SampleType> kr (UnitBase<SampleType> const& unit) throw()
{
    return unit.kr();
}

/** Mixes a unit down to a single channel. */
template<class SampleType>
PLONK_INLINE_LOW UnitBase<SampleType> mix (UnitBase<SampleType> const& unit) throw()
{
    return unit.mix();
}


//------------------------------------------------------------------------------

template<class SampleType>
UnitBase<SampleType> UnitBase<SampleType>::operator, (UnitType const& other) const throw()
{ 
    return operator, (UnitBase (other));
}

template<class SampleType>
UnitBase<SampleType> UnitBase<SampleType>::operator, (UnitBase const& other) const throw()
{ 
    const int numChannels = getNumChannels() + other.getNumChannels();
    UnitBase result (UnitBase::withSize (numChannels));
    
    int resultIndex = 0;
    int sourceIndex;
    
    for (sourceIndex = 0; sourceIndex < getNumChannels(); ++sourceIndex) 
        result.put (resultIndex++, this->at (sourceIndex));
    
    for (sourceIndex = 0; sourceIndex < other.getNumChannels(); ++sourceIndex) 
        result.put (resultIndex++, other.at (sourceIndex));
    
    return result;        
}

template<class SampleType>
BlockSize UnitBase<SampleType>::getMinBlockSize() const throw()
{
    const int numChannels = this->getNumChannels();
    BlockSize blockSize = this->atUnchecked (0).getBlockSize();
    
    for (int i = 1; i < numChannels; ++i) 
    {
        const BlockSize& channelBlockSize = this->atUnchecked (i).getBlockSize();
        blockSize = blockSize.selectMin (channelBlockSize);        
    }
    
    return blockSize;
}

template<class SampleType>
BlockSize UnitBase<SampleType>::getMaxBlockSize() const throw()
{
    const int numChannels = this->getNumChannels();
    BlockSize blockSize = this->atUnchecked (0).getBlockSize();
    
    for (int i = 1; i < numChannels; ++i) 
    {
        const BlockSize& channelBlockSize = this->atUnchecked (i).getBlockSize();
        blockSize = blockSize.selectMax (channelBlockSize);        
    }
    
    return blockSize;
}

template<class SampleType>
SampleRate UnitBase<SampleType>::getMinSampleRate() const throw()
{
    const int numChannels = this->getNumChannels();
    SampleRate sampleRate = this->atUnchecked (0).getSampleRate();
    
    for (int i = 1; i < numChannels; ++i) 
    {
        const SampleRate& channelSampleRate = this->atUnchecked (i).getSampleRate();
        sampleRate = sampleRate.selectMin (channelSampleRate);        
    }
    
    return sampleRate;    
}

template<class SampleType>
SampleRate UnitBase<SampleType>::getMaxSampleRate() const throw()
{
    const int numChannels = this->getNumChannels();
    SampleRate sampleRate = this->atUnchecked (0).getSampleRate();
    
    for (int i = 1; i < numChannels; ++i) 
    {
        const SampleRate& channelSampleRate = this->atUnchecked (i).getSampleRate();
        sampleRate = sampleRate.selectMax (channelSampleRate);        
    }
    
    return sampleRate;        
}


#endif // PLONK_UNIT_H
