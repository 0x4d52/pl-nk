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


    /** Default constructor.
     Creates a "null" channel. */
    ChannelBase() throw() 
    :   Base (ChannelBase::getNull().containerCopy().getInternal()) 
    { 
    }
    
    /** Construct a channel with a specific internal type. */
    ChannelBase (Internal *internal) throw()
    :   Base (internal)
    {
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
            this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
//    ChannelBase (Dynamic const& other) throw()
//    :   Base (static_cast<Internal*> (other.containerCopy().getItem().getInternal()))
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
        
    inline bool isNull() const throw()                                      { return this->getInternal()->isNull(); }
    inline bool isNotNull() const throw()                                   { return ! this->getInternal()->isNull(); }  
    inline bool isConstant() const throw()                                  { return this->getInternal()->isConstant(); }
    inline bool isNotConstant() const throw()                               { return ! this->getInternal()->isConstant(); }    
    inline bool isTypeConverter() const throw()                             { return this->getInternal()->isTypeConverter(); }

    inline const Text getName() const throw()                               { return this->getInternal()->getName(); }
    inline const Text getLabel() const throw()                              { return this->getInternal()->getLabel(); }
    inline void setLabel(Text const& newId) throw()                         { this->getInternal()->setLabel(newId); }   
    inline const Text getOutputTypeName() const throw()                     { return this->getInternal()->getOutputTypeName(); }
    inline const Text getInputTypeName() const throw()                      { return this->getInternal()->getInputTypeName(); }
    inline int getOutputTypeCode() const throw()                            { return this->getInternal()->getOutputTypeCode(); }
    inline int getInputTypeCode() const throw()                             { return this->getInternal()->getInputTypeCode(); }

    inline BlockSize getBlockSize() const throw()                           { return this->getInternal()->getBlockSize(); }
    inline void setBlockSize (BlockSize const& newBlockSize) throw()        { this->getInternal()->setBlockSize (newBlockSize); }

    inline SampleRate getSampleRate() const throw()                         { return this->getInternal()->getSampleRate(); }
    inline void setSampleRate (SampleRate const& newSampleRate) throw()     { this->getInternal()->setSampleRate (newSampleRate); }
    inline double getSampleDurationInTicks() const throw()                  { return this->getInternal()->getSampleDurationInTicks(); }
    inline double getBlockDurationInTicks() const throw()                   { return this->getInternal()->getBlockDurationInTicks(); }
    
    inline DoubleVariable getOverlap() const throw()                        { return this->getInternal()->getOverlap(); }
    inline void setOverlap (DoubleVariable const& newOverlap) throw()       { this->getInternal()->setOverlap (newOverlap); }
    
    inline const Inputs getInputs() const throw()                           { return this->getInternal()->getInputs(); }
    
    inline const Buffer getOutputBuffer() const throw()                     { return this->getInternal()->getOutputBuffer(); }
    inline const SampleType* getOutputSamples() const throw()               { return this->getInternal()->getOutputSamples(); }
    inline SampleType* getOutputSamples() throw()                           { return this->getInternal()->getOutputSamples(); }
    inline void setOutputBuffer (Buffer const& externalBuffer) throw()      { this->getInternal()->setOutputBuffer (externalBuffer); }
    
    inline void initValue (SampleType const& value) throw()                 { return this->getInternal()->initValue(value); }
    inline void initChannel (const int index) throw()                       { this->getInternal()->initChannel (index); }
    inline const SampleType& getValue() const throw()                       { return this->getInternal()->getValue(); }
    
    inline ChannelBase getChannel (const int index) throw()                 { return ChannelBase (this->getInternal()->getChannel (index)); }
    inline ChannelBase operator[] (const int index) throw()                 { return ChannelBase (this->getInternal()->getChannel (index)); }

    inline const TimeStamp getNextTimeStamp() const throw()                 { return this->getInternal()->getNextTimeStamp(); }
    inline bool shouldBeDeletedNow (TimeStamp const& time) const throw()    { return this->getInternal()->shouldBeDeletedNow (time); }
    
    void process (ProcessInfo& info, const int channel) throw()
    {
        const TimeStamp& nextTimeStamp = this->getInternal()->getNextTimeStamp();
        const TimeStamp& infoTimeStamp = info.getTimeStamp();
        
        if (infoTimeStamp >= nextTimeStamp)
        {
            this->getInternal()->process (info, channel);
            this->getInternal()->setLastTimeStamp (infoTimeStamp);
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