/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLONK_CHANNEL_H
#define PLONK_CHANNEL_H

#include "../plonk_GraphForwardDeclarations.h"
#include "../../containers/plonk_DynamicContainer.h"
#include "../../containers/variables/plonk_Variable.h"
#include "../../core/plonk_SmartPointer.h"
#include "../../core/plonk_WeakPointer.h"
#include "../../core/plonk_SmartPointerContainer.h"
#include "../utility/plonk_ProcessInfo.h"
#include "../utility/plonk_BlockSize.h"
#include "../utility/plonk_SampleRate.h"
#include "../utility/plonk_TimeStamp.h"
#include "../utility/plonk_InputDictionary.h"

//------------------------------------------------------------------------------

/** A single output signal.
 Channel objects may have multiple inputs but only one output.
 Multiple outputs are achieve by using arrays of Channel objects or a Unit object.
 True multiple input / multiple output Unit objects are achieved using a proxy system.
 */
template<class SampleType>
class ChannelBase : public SmartPointerContainer< ChannelInternalBase<SampleType> >
{
protected:
    typedef PLONK_UNITBASETYPE<ChannelBase>             UnitType;

public:
    typedef ChannelInternalBase<SampleType>             Internal;
    typedef SmartPointerContainer<Internal>             Base;
    typedef WeakPointerContainer<ChannelBase>           Weak;
    typedef NumericalArray<SampleType>                  Buffer;
    typedef InputDictionary                             Inputs;
    typedef Variable<SampleType>                        VariableType;
    typedef ReceiverInternal<ChannelBase>               Receiver;


    /** Default constructor.
     Creates a "null" channel. */
    ChannelBase() throw() 
    :   Base (ChannelBase::getNull().getInternal())
    { 
    }
    
    /** Construct a channel with a specific internal type. */
    ChannelBase (Internal *internal) throw()
    :   Base (internal)
    {
    }
    
    /** Get the first channel from a one-channel unit. */
    ChannelBase (UnitType const& unit) throw()
    :   Base (static_cast<Base const&> (unit.atUnchecked (0)))
    {
        plonk_assert (unit.getNumChannels() == 1);
    }
    
    /** Get the first channel from a one-channel unit. */
    ChannelBase (UnitBase<SampleType> const& unit) throw()
    :   Base (static_cast<Base const&> (unit.atUnchecked (0)))
    {
        plonk_assert (unit.getNumChannels() == 1);
    }

    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    ChannelBase (ChannelBase const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
    /** Assignment operator. */
    ChannelBase& operator= (ChannelBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
//    ChannelBase (Dynamic const& other) throw()
//    :   Base (static_cast<Internal*> (other.getItem().getInternal()))
//    {
//        if (this->getInternal() == 0)
//            operator= (ChannelBase());
//        else
//            plonk_assert (other.getTypeCode() == TypeUtility<ChannelBase>::getTypeCode());
//    }            
        
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static ChannelBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }            
        
    static const ChannelBase& getNull() throw()
	{
		static ChannelBase null (static_cast<Internal*> (new NullChannelInternal<SampleType>()));
		return null;
	}	                        
    
    /** Construct a channel that generates a constant. */
    template<class ValueType>
    ChannelBase (ValueType const& valueInit) throw()
    :   Base (new ConstantChannelInternal<SampleType> (valueInit))
    {
    }
        
    PLONK_INLINE_LOW bool isNull() const throw()                                      { return this->getInternal()->isNull(); }
    PLONK_INLINE_LOW bool isNotNull() const throw()                                   { return ! this->getInternal()->isNull(); }  
    PLONK_INLINE_LOW bool isConstant() const throw()                                  { return this->getInternal()->isConstant(); }
    PLONK_INLINE_LOW bool isNotConstant() const throw()                               { return ! this->getInternal()->isConstant(); }    
    PLONK_INLINE_LOW bool isTypeConverter() const throw()                             { return this->getInternal()->isTypeConverter(); }

    PLONK_INLINE_LOW const Text getName() const throw()                               { return this->getInternal()->getName(); }
    PLONK_INLINE_LOW const Text getLabel() const throw()                              { return this->getInternal()->getLabel(); }
    PLONK_INLINE_LOW void setLabel(Text const& newId) throw()                         { this->getInternal()->setLabel(newId); }   
    PLONK_INLINE_LOW const Text getOutputTypeName() const throw()                     { return this->getInternal()->getOutputTypeName(); }
    PLONK_INLINE_LOW const Text getInputTypeName() const throw()                      { return this->getInternal()->getInputTypeName(); }
    PLONK_INLINE_LOW int getOutputTypeCode() const throw()                            { return this->getInternal()->getOutputTypeCode(); }
    PLONK_INLINE_LOW int getInputTypeCode() const throw()                             { return this->getInternal()->getInputTypeCode(); }

    PLONK_INLINE_LOW const BlockSize& getBlockSize() const throw()                    { return this->getInternal()->getBlockSize(); }
    PLONK_INLINE_LOW BlockSize& getBlockSize() throw()                                { return this->getInternal()->getBlockSize(); }
    PLONK_INLINE_LOW void setBlockSize (BlockSize const& newBlockSize) throw()        { this->getInternal()->setBlockSize (newBlockSize); }

    PLONK_INLINE_LOW const SampleRate& getSampleRate() const throw()                  { return this->getInternal()->getSampleRate(); }
    PLONK_INLINE_LOW SampleRate& getSampleRate() throw()                              { return this->getInternal()->getSampleRate(); }
    PLONK_INLINE_LOW void setSampleRate (SampleRate const& newSampleRate) throw()     { this->getInternal()->setSampleRate (newSampleRate); }
    PLONK_INLINE_LOW double getSampleDurationInTicks() const throw()                  { return this->getInternal()->getSampleDurationInTicks(); }
    PLONK_INLINE_LOW double getBlockDurationInTicks() const throw()                   { return this->getInternal()->getBlockDurationInTicks(); }
    
    PLONK_INLINE_LOW const DoubleVariable& getOverlap() const throw()                 { return this->getInternal()->getOverlap(); }
    PLONK_INLINE_LOW DoubleVariable& getOverlap() throw()                             { return this->getInternal()->getOverlap(); }
    PLONK_INLINE_LOW void setOverlap (DoubleVariable const& newOverlap) throw()       { this->getInternal()->setOverlap (newOverlap); }
    
    PLONK_INLINE_LOW const Inputs& getInputs() const throw()                          { return this->getInternal()->getInputs(); }
    PLONK_INLINE_LOW Inputs getInputs() throw()                                       { return this->getInternal()->getInputs(); }
    
    PLONK_INLINE_LOW const Buffer& getOutputBuffer() const throw()                    { return this->getInternal()->getOutputBuffer(); }
    PLONK_INLINE_LOW Buffer& getOutputBuffer() throw()                                { return this->getInternal()->getOutputBuffer(); }
    PLONK_INLINE_LOW const SampleType* getOutputSamples() const throw()               { return this->getInternal()->getOutputSamples(); }
    PLONK_INLINE_LOW SampleType* getOutputSamples() throw()                           { return this->getInternal()->getOutputSamples(); }
    PLONK_INLINE_LOW void setOutputBuffer (Buffer const& externalBuffer) throw()      { this->getInternal()->setOutputBuffer (externalBuffer); }
    
    PLONK_INLINE_LOW int getNumChannels() const throw()                               { return this->getInternal()->getNumChannels(); }
    
    PLONK_INLINE_LOW void initValue (SampleType const& value) throw()                 { return this->getInternal()->initValue (value); }
    PLONK_INLINE_LOW void initChannel (const int index) throw()                       { this->getInternal()->initChannel (index); }
    PLONK_INLINE_LOW const SampleType& getValue() const throw()                       { return this->getInternal()->getValue(); }
    
    PLONK_INLINE_LOW ChannelBase getChannel (const int index) throw()                 { return ChannelBase (this->getInternal()->getChannel (index)); }
    PLONK_INLINE_LOW ChannelBase operator[] (const int index) throw()                 { return ChannelBase (this->getInternal()->getChannel (index)); }

    PLONK_INLINE_LOW const TimeStamp& getNextTimeStamp() const throw()                { return this->getInternal()->getNextTimeStamp(); }
    PLONK_INLINE_LOW bool shouldBeDeletedNow (TimeStamp const& time) const throw()    { return this->getInternal()->shouldBeDeletedNow (time); }
    
    PLONK_INLINE_LOW void resetIfExpired() throw()                                    { this->getInternal()->setExpiryTimeStamp (TimeStamp::getMaximum()); }
    
//    /** Returns @c true if this unit needs to process for the given timestamp. */
//    PLONK_INLINE_LOW bool needsToProcess (ProcessInfo const& info, const int channel) const throw()
//    {
//		(void)channel;
//
//        const TimeStamp& nextTimeStamp = this->getInternal()->getNextTimeStamp();
//        const TimeStamp& infoTimeStamp = info.getTimeStamp();
//
//        return (infoTimeStamp >= nextTimeStamp);
//    }

    /** Returns @c true if this unit needs to process for the given timestamp. */
    PLONK_INLINE_LOW bool needsToProcess (ProcessInfo const& info) const throw()
    {
        const TimeStamp& nextTimeStamp = this->getInternal()->getNextTimeStamp();
        const TimeStamp& infoTimeStamp = info.getTimeStamp();
        
        return (infoTimeStamp >= nextTimeStamp);
    }

    PLONK_INLINE_LOW void process (ProcessInfo& info, const int channel) throw()
    {        
        if (this->needsToProcess (info))
        {
            this->getInternal()->process (info, channel);
            this->getInternal()->setLastTimeStamp (info.getTimeStamp());
            this->getInternal()->updateTimeStamp();
            
            if (info.getShouldDelete() == true)
                this->getInternal()->setExpiryTimeStamp (this->getInternal()->getNextTimeStamp());
        }
    }
    
    int getTypeCode() const throw()
    {
        return TypeUtility<ChannelBase>::getTypeCode();
    }
    
    int getSampleTypeCode() const throw()
    {
        return TypeUtility<SampleType>::getTypeCode();
    }        
    
};



#endif // PLONK_CHANNEL_H