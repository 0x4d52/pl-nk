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
    typedef ChannelBase<SampleType>                 ChannelType;
    typedef ChannelInternalBase<SampleType>         ChannelInternalType;
    typedef NumericalArray<ChannelType>             UnitType;
    typedef NumericalArray2D<ChannelType,UnitType>  UnitArray;
    typedef NumericalArray2D<SampleType>            BufferArrayType;
    typedef NullChannelInternal<SampleType>         NullInternal;
    typedef Variable<SampleType>                    VariableType;

public:
    typedef ObjectArrayInternal<ChannelType>    Internal;    
    typedef UnitType                            Base;    
    typedef WeakPointerContainer<UnitBase>      Weak;
    typedef NumericalArray<SampleType>          Buffer;
    typedef InputDictionary                     Inputs;    
    typedef typename UnitType::InitialNumber    InitialUnit;

    /** Default constructor.
     This creates a Unit with a single null Channel. */
    UnitBase() throw()
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
    
    static const UnitBase emptyChannels(const int numChannels) throw()
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
    UnitBase (ValueType const& valueInit) throw()
    :   UnitType (ChannelType (valueInit))
    {
        this->atUnchecked (0).initChannel (0);
    }
    
    /** Multichannel Constant Unit constructor.
     This creates a Unit with a multiple constant Channel objects derived 
     from an array of numerical values. */
    template<class ValueType>
    UnitBase (NumericalArray<ValueType> const& values) throw()
    :   UnitType (UnitType::template collect<ChannelType> (values))
    {
        for (int i = 0; i < this->getNumChannels(); ++i)
            this->atUnchecked (i).initChannel (i);
    }    
    
    UnitBase (VariableType const& variable) throw()
    :   UnitType (VariableUnit<SampleType>::kr (variable))
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
    UnitBase (UnitBase const& copy) throw()
	:	UnitType (static_cast<UnitType const&> (copy))
	{
	}                
    
    
    template<template <typename> class OtherUnitType, class OtherSampleType>
    UnitBase (OtherUnitType<OtherSampleType> const& other)
    :   UnitType (TypeUnit<SampleType,OtherSampleType>::ar (other))
    {
    }
    
//    UnitBase (InitialUnit const& i00,
//              InitialUnit const& i01,
//              InitialUnit const& i02,
//              InitialUnit const& i03 = InitialUnit()) throw()
//	:	Base (i00, i01, i02, i03)
//	{
//	}
//	
//    UnitBase (InitialUnit const& i00,
//              InitialUnit const& i01,
//              InitialUnit const& i02,
//              InitialUnit const& i03,
//              InitialUnit const& i04,
//              InitialUnit const& i05 = InitialUnit(),
//              InitialUnit const& i06 = InitialUnit(),
//              InitialUnit const& i07 = InitialUnit()) throw()
//	:	Base (i00, i01, i02, i03, i04, i05, i06, i07)
//	{
//	}
        
    UnitBase (ObjectArray<ChannelType> const& copy) throw()
	:	UnitType (static_cast<UnitType const&> (copy))
	{
	}    
    
    /** Assignment operator. */
    UnitBase& operator= (UnitBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.containerCopy().getInternal());
        
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
    
    /** Primary means of creating Unit objects.
     Used with most non-proxy-owning ChannelInternal classes. */
    template<class ChannelInternalClassType>
    static inline UnitBase createFromInputs (Inputs const& inputs, 
                                             typename ChannelInternalClassType::Data const& data,
                                             BlockSize const& preferredBlockSize,
                                             SampleRate const& preferredSampleRate) throw()
    {        
        /*
         for full templating we'd need to add template params for the mul and add unit types
        */
        
        Inputs mainInputs = inputs;
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
            plonk_assert (internal->isProxyOwner() == false);

            internal->initChannel (i);
                        
            result.put (i, ChannelType (internal));
        }
        
        return applyMulAdd (result, UnitBase (mul), UnitBase (add));
    }     
    
    /** Primary means of creating Unit objects with proxies.
     Used with most proxy-owning ChannelInternal classes. */
    template<class ProxyOwnerChannelInternalClassType>
    static inline UnitBase proxiesFromInputs (Inputs const& inputs, 
                                              typename ProxyOwnerChannelInternalClassType::Data const& data,
                                              BlockSize const& preferredBlockSize,
                                              SampleRate const& preferredSampleRate) throw()
    {        
        /*
         for full templating we'd need to add template params for the mul and add unit types
         */

        Inputs mainInputs = inputs;
        const Dynamic add = mainInputs.remove (IOKey::Add);
        const Dynamic mul = mainInputs.remove (IOKey::Multiply);
                
        ProxyOwnerChannelInternalClassType* proxyOwner 
            = new ProxyOwnerChannelInternalClassType (mainInputs, 
                                                      data, 
                                                      preferredBlockSize, 
                                                      preferredSampleRate);
        
        plonk_assert (proxyOwner->isProxyOwner() == true);
        
        const int numChannels = proxyOwner->getNumChannels();
        UnitBase result (UnitBase::withSize (numChannels));
        
        for (int i = 0; i < numChannels; ++i) 
        {
            ChannelType proxy = proxyOwner->getProxy (i);
            proxy.initChannel (i);
            result.put (i, proxy);
        }
        
        return applyMulAdd (result, UnitBase (mul), UnitBase (add));
    }        
    
    /** Apply multiply and addition operators to another unit.
     Here @c mul will be ignored (not applied) if the value is a single channel 
     constant equal to 1 (or null rather than just a constant 0). And @c add 
     will be ignored (not applied) if it is a single channel constant equal to
     0 (or null). On some platforms this might be implemented as a single
     multiply-and-add operation. */
    static inline UnitBase applyMulAdd (UnitBase const& mainUnit, 
                                        UnitBase const& mul,
                                        UnitBase const& add) throw()
    {
        /*
         for full templating we'd need to add template params for the mul and add unit types
         */

        UnitBase result = mainUnit;
        
        bool hasMul = true;
        bool hasAdd = true;
                                
        if (mul.isNull() || (mul.isConstant() && (mul.getValue (0) == SampleType (1))))
            hasMul = false;
        
        if (add.isNull() || (add.isConstant() && (add.getValue (0) == SampleType (0))))
            hasAdd = false;
        
        // could apply an all-in-one muladd if we had one...
        
        if (hasMul)
            result *= mul;
        
        if (hasAdd)
            result += add;
            
        return result;        
    }
    
//    /** Apply multiply and addition operators to another unit.
//     Here @c mul will be ignored (not applied) if the value is a single channel 
//     constant equal to 1 (or null rather than just a constant 0). And @c add 
//     will be ignored (not applied) if it is a single channel constant equal to
//     0 (or null). On some platforms this might be implemented as a single
//     multiply-and-add operation. */
//    template<class MulUnitType, class AddUnitType>
//    static inline UnitBase applyMulAdd (UnitBase const& mainUnit, 
//                                        MulUnitType const& mul,
//                                        AddUnitType const& add) throw()
//    {
//        /*
//         for full templating we'd need to add template params for the mul and add unit types
//         */
//        
//        UnitBase result = mainUnit;
//        
//        bool hasMul = true;
//        bool hasAdd = true;
//        
//        if (mul.isNull() || (mul.isConstant() && (mul.getValue (0) == SampleType (1))))
//            hasMul = false;
//        
//        if (add.isNull() || (add.isConstant() && (add.getValue (0) == SampleType (0))))
//            hasAdd = false;
//        
//        // could apply an all-in-one muladd if we had one...
//        
//        if (hasMul)
//            result *= mul;
//        
//        if (hasAdd)
//            result += add;
//        
//        return result;        
//    }

    
    /** Resamples this unit to a different sample rate and/or block size. */
    inline UnitBase ar (BlockSize const& preferredBlockSize = BlockSize::getDefault(),
                        SampleRate const& preferredSampleRate = SampleRate::getDefault()) throw()
    {
        return ResampleUnit<SampleType>::ar (*this, preferredBlockSize, preferredSampleRate);
    }    
    
    /** Resamples this unit to the default control rate sample rate and block size. */
    inline UnitBase kr() throw()
    {
        return ResampleUnit<SampleType>::kr (*this);
    }
    
    /** Mixes this unit down to a single channel. */
    inline UnitBase mix() throw()
    {
        return MixerUnit<SampleType>::ar (*this, true);
    }
    
    /** Mixes this unit down to a single channel with and auto-deletion prevention barrier. 
     This prevents things like envelopes from releasing the mixer that contains this unit. */
    inline UnitBase mixBarrier() throw()
    {
        return MixerUnit<SampleType>::ar (*this, false);
    }
    
    /** Create an overlapping process from this unit's contrinuous stream. */
    inline UnitBase overlapMake (DoubleVariable const& overlap = Math<DoubleVariable>::get0_5()) throw()
    {
        return OverlapMakeUnit<SampleType>::ar (*this, overlap);
    }
    
    /** Mix down overlapping process to a continuous stream. */
    inline UnitBase overlapMix (DoubleVariable const& overlap = Math<DoubleVariable>::get0_5()) throw()
    {
        return OverlapMixUnit<SampleType>::ar (*this, overlap);
    }
    
    /** Templated binary operator Unit creator. */
    template<PLONK_BINARYOPFUNCTION(SampleType, op)>
    UnitBase binary (UnitBase const& rightOperand) const throw() 
    {        
        /*
         for full templating we'd need to add template param for the rightop unit types
         */

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
    UnitBase linlin (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linlin (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to linear mapping assuming the input is in the range -1...+1. */
    UnitBase linlin (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        const SampleType peak2peak (peak * Math<SampleType>::get2());
        return plonk::linlin2 (*this, 
                               UnitBase (-peak), UnitBase (peak2peak), 
                               outLow, (outHigh - outLow));
    }
    
    /** Linear to exponential mapping. 
     Note that the output range must not cross or meet zero. */
    UnitBase linexp (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linexp (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to exponential mapping assuming the input is in the range -1...+1. 
     Note that the output range must not cross or meet zero. */
    UnitBase linexp (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        const SampleType peak2peak (peak * Math<SampleType>::get2());
        const SampleType reciprocalInRange (plonk::reciprocal (peak2peak));
        return plonk::linexp2 (*this, 
                               UnitBase (reciprocalInRange), UnitBase (-peak * reciprocalInRange ), 
                               outLow, (outHigh / outLow));
    }
    
    /** Linear to sinusoidal mapping. */
    UnitBase linsin (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linsin (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to sinusoidal mapping assuming the input is in the range -1...+1. */
    UnitBase linsin (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        return plonk::linsin2 (*this, UnitBase (-peak), UnitBase (peak), outLow, outHigh);
    }
    
    /** Linear to Welch curve mapping. */
    UnitBase linwelch (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::linwelch (*this, inLow, inHigh, outLow, outHigh);
    }
    
    /** Linear to Welch curve mapping assuming the input is in the range -1...+1. */
    UnitBase linwelch (UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        const SampleType peak (TypeUtility<SampleType>::getTypePeak());
        return plonk::linwelch (*this, UnitBase (-peak), UnitBase (peak), outLow, outHigh);
    }
    
    /** Exponential to linear mapping. 
     Note that the intput range must not cross or meet zero. */
    UnitBase explin (UnitBase const& inLow, UnitBase const& inHigh, 
                     UnitBase const& outLow, UnitBase const& outHigh) const throw()
    {
        return plonk::explin (*this, inLow, inHigh, outLow, outHigh);
    }
        
    /** Create a array of units by concatenation. */
    const UnitArray operator<< (UnitType const& other) const throw()   { return UnitArray (*this, other); }
    
    /** Create a array of units by concatenation. */
	const UnitArray operator<< (UnitBase const& other) const throw()   { return UnitArray (*this, other); }
    
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
    inline int getNumChannels() const throw() { return this->length(); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is alos recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */
    inline UnitBase getChannel (const int index) throw() { return this->wrapAt (index).getChannel (index); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is alos recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */    
    inline UnitBase getChannel (const int index) const throw() { return this->copy().wrapAt (index).getChannel (index); }

    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is alos recursive such that the returned
     channel has in turn stripped out the other multiple channels during the process. */    
    UnitBase operator[] (const int index) throw() { return this->getChannel (index); }
    
    /** Returns a unit with the single channel specified.
     This wraps the index so that it is always in range. It is alos recursive such that the returned
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
    
    /** Get the minimum block size in the array of channels in this unit. */
    BlockSize getMinBlockSize() const throw();
    
    /** Get the maximum block size in the array of channels in this unit. */
    BlockSize getMaxBlockSize() const throw();
    
    /** Get the block size of a specific channel in this unit. */
    inline const BlockSize getBlockSize (const int index) const throw()    { return this->wrapAt (index).getBlockSize(); }
    
    /** Get the block size of a specific channel in this unit. */
    inline BlockSize getBlockSize (const int index) throw()                { return this->wrapAt (index).getBlockSize(); }

    /** Set the block size of all channels in this unit. */
    void setBlockSize (BlockSize const& newBlockSize) throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i) 
            channels[i].setBlockSize (newBlockSize);        
    }
    
    /** Get the minimum sample rate in the array of channels in this unit. */
    SampleRate getMinSampleRate() const throw();
    
    /** Get the maximum sample rate in the array of channels in this unit. */
    SampleRate getMaxSampleRate() const throw();
    
    /** Get the sample rate of a specific channel in this unit. 
     Indices out of range will be wrapped to the available channels. */
    inline const SampleRate getSampleRate (const int index) const throw()  { return this->wrapAt (index).getSampleRate(); }
    
    /** Get the sample rate of a specific channel in this unit.  
     Indices out of range will be wrapped to the available channels. */
    inline SampleRate getSampleRate (const int index) throw()              { return this->wrapAt (index).getSampleRate(); }
    
    /** Set the sample rate of all channels in this unit. */
    void setSampleRate (SampleRate const& newSampleRate) throw()
    {
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
        
        for (int i = 0; i < numChannels; ++i) 
            channels[i].setSampleRate (newSampleRate);        
    }    
    
    inline double getSampleDurationInTicks(const int index) const throw()          { return this->wrapAt (index)->getSampleDurationInTicks(); }
    inline double getBlockDurationInTicks(const int index) const throw()           { return this->wrapAt (index)->getBlockDurationInTicks(); }
    
    inline const DoubleVariable getOverlap (const int index) const throw()  { return this->wrapAt (index).getOverlap(); }
    inline DoubleVariable getOverlap (const int index) throw()              { return this->wrapAt (index).getOverlap(); }
    
    inline bool channelsHaveSameOverlap() throw()
    {
        const int numChannels = this->getNumChannels();
        plonk_assert (numChannels > 0);

        ChannelType* channels = this->getArray();
                
        DoubleVariable overlap = channels[0].getOverlap();
        
        for (int i = 1; i < numChannels; ++i)
            if (channels[i].getOverlap() != overlap)
                return false;
        
        return true;
    }
    
    /** Get the output buffer of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    inline const Buffer getOutputBuffer (const int index) const throw() 
    { 
        return this->wrapAt (index).getOutputBuffer();
    }
    
    /** Get the output buffer of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    inline Buffer getOutputBuffer (const int index) throw() 
    { 
        return this->wrapAt (index).getOutputBuffer();
    }
    
    /** Get a pointer to the raw samples of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    inline const SampleType* getOutputSamples (const int index) const throw()
    { 
        return this->wrapAt (index)->getOutputSamples(); 
    }
    
    /** Get a pointer to the raw samples of a specific channel.  
     Indices out of range will be wrapped to the available channels. */
    inline SampleType* getOutputSamples (const int index) throw()
    { 
        return this->wrapAt (index)->getOutputSamples(); 
    }
    
    /** Set the output buffer of a specific channel to use an external buffer. */
    inline void setOutputBuffer (const int index, Buffer const& externalBuffer) throw()
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
    
    /** Get the current value of a specific channel in this unit.
     Indices out of range will be wrapped to the available channels. */
    inline const SampleType& getValue (const int index) const throw()
    {
        return this->wrapAt (index).getValue();
    }
    
    inline const TimeStamp getNextTimeStamp(const int index) const throw()
    {
        return this->wrapAt (index).getNextTimeStamp();
    }
    
    inline bool isNull (const int index) const throw()
    {
        return this->wrapAt (index).isNull();
    }
    
    inline bool isNotNull (const int index) const throw()
    {
        return ! this->isNull (index);
    }
    
    inline bool isConstant (const int index) const throw()
    {
        return this->wrapAt (index).isConstant();
    }
    
    inline bool isNotConstant (const int index) const throw()
    {
        return ! this->isConstant (index);
    }    
    
    /** Returns @c true if this unit contains only a single null channel. */
    inline bool isNull() const throw()
    {
        if (this->getNumChannels() != 1)
            return false;
        else if (this->at(0).isNull())
            return true;
        else
            return false;
    }
    
    /** Returns @c false if this unit contains only a single null channel. */
    inline bool isNotNull() const throw()
    {
        return ! this->isNull();
    }
    
    /** Returns @c true if this unit contains only a single constant channel. */
    inline bool isConstant() const throw()
    {
        if (this->getNumChannels() != 1)
            return false;
        else if (this->at(0).isConstant())
            return true;
        else
            return false;
    }
    
    /** Returns @c false if this unit contains only a single constant channel. */
    inline bool isNotConstant() const throw()
    {
        return ! this->isConstant();
    }    
    
    inline void setToNull() throw()
    {
        this->setSize (1, true);
        this->put (0, ChannelType::getNull());
    }
        
    /** Process a specific channel in this unit.
     The host should prepare a ProcessInfo which is passed to this function
     for each required block of data. This is generally used by ChannelInternal
     subclasses when obtaining input data. 
     @return The buffer from the requested channel. */
    inline const Buffer process (ProcessInfo& info, const int channel) throw()
    {
        ChannelType& theChannel (this->wrapAt (channel));
        
        if (theChannel.shouldBeDeletedNow (info.getTimeStamp()))
        {
            this->setToNull();
            this->wrapAt (channel).process (info, 0); // probably unnecessary?
            return this->getOutputBuffer (0);
        }
        else
        {
            theChannel.process (info, channel);
            return this->getOutputBuffer (channel);
        }
        
    }
    
    /** Process all channels in this unit.
     The host should prepare a ProcessInfo which is passed to this function
     for each required block of data. */    
    void process (ProcessInfo& info) throw()
    {        
        const int numChannels = this->getNumChannels();
        ChannelType* channels = this->getArray();
                        
        if (numChannels > 0)
        {
            int i;
            bool didDelete = false;
            
            for (i = 0; i < numChannels; ++i) 
            {
                if (channels[i].shouldBeDeletedNow (info.getTimeStamp()))
                {
                    didDelete = true;
                    break;
                }
            }
                        
            if (didDelete == false)
                for (i = 0; i < numChannels; ++i) 
                    channels[i].process (info, i);
            else
                this->setToNull();
        }
    }
    
//    inline static const UnitBase& getZero() throw()  
//    { 
//        static const UnitBase v (Math<SampleType>::get0()); 
//        return v; 
//    }
//    
//    inline static const UnitBase& getOne() throw()   
//    {         
//        static const UnitBase v (Math<SampleType>::get1()); 
//        return v; 
//    }
//    
//    inline static const UnitBase& getTwo() throw()   
//    {         
//        static const UnitBase v (Math<SampleType>::get2()); 
//        return v; 
//    }
//    
//    inline static const UnitBase& getHalf() throw()  
//    {         
//        static const UnitBase v (Math<SampleType>::get0_5()); 
//        return v; 
//    }
//    
//    inline static const UnitBase& getPi() throw()    
//    {         
//        static const UnitBase v (Math<SampleType>::getPi()); 
//        return v; 
//    }
//    
//    inline static const UnitBase& getTwoPi() throw() 
//    {         
//        static const UnitBase v (Math<SampleType>::get2Pi()); 
//        return v; 
//    }        
    
    int getTypeCode() const throw()
    {
        return TypeUtility<UnitBase>::getTypeCode();
    }
    
    int getSampleTypeCode() const throw()
    {
        return TypeUtility<SampleType>::getTypeCode();
    }            
    
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

PLONK_BINARYOPGLOBALS_TEMPLATE(UnitBase); // declares global functions with the same name as the binary operators
PLONK_UNARYOPGLOBALS_TEMPLATE(UnitBase);  // declares global functions with the same name as the unary operators

//template<class SampleType>
//inline UnitBase<SampleType> explin (UnitBase<SampleType> const& input, 
//                                    UnitBase<SampleType> const& inLow, UnitBase<SampleType> const& inHigh, 
//                                    UnitBase<SampleType> const& outLow, UnitBase<SampleType> const& outHigh) throw()
//{
//    const UnitBase<SampleType> clipped (clip (input, inLow, inHigh));
//    return log (clipped / inLow) / log (inHigh / inLow) * (outHigh - outLow) + outLow;
//}


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
