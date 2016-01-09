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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

InputDictionary::InputDictionary() throw()
:   Base (8)
{
}

InputDictionary::InputDictionary (InputDictionary const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}            

const BlockSize InputDictionary::getMaxBlockSize() const throw()
{
    const DynamicArray& items = this->getValues();
    const int numItems = items.length();
    
    if (numItems == 0)
        return 0;
        
    BlockSize blockSize; // 0
    
    for (int i = 0; i < numItems; ++i)
    {
        const Dynamic& item = items.atUnchecked (i);
        
        if (TypeCode::isUnit (item.getTypeCode()))
        {
            BlockSize unitBlockSize = item.getMaxBlockSize().as<BlockSize>();
            blockSize = blockSize.selectMax (unitBlockSize);
        }
    }
    
    return blockSize;
}

const SampleRate InputDictionary::getMaxSampleRate() const throw()
{
    const DynamicArray& items = this->getValues();
    const int numItems = items.length();
    
    if (numItems == 0)
        return 0;
        
    SampleRate sampleRate; // 0
    
    for (int i = 0; i < numItems; ++i)
    {
        const Dynamic& item = items.atUnchecked (i);
        
        if (TypeCode::isUnit (item.getTypeCode()))
        {
            SampleRate unitSampleRate = item.getMaxSampleRate().as<SampleRate>();
            sampleRate = sampleRate.selectMax (unitSampleRate);
        }
    }
    
    return sampleRate;
}

InputDictionary InputDictionary::getChannel (const int index) throw()
{
    InputDictionary result;
    
    const IntArray& keys = this->getKeys();
    const int numKeys = keys.length();
    
    plonk_assert (numKeys == this->length());
    
    for (int i = 0; i < numKeys; ++i)
    {
        Dynamic& item = this->atIndexUnchecked (i);
        const int type = item.getTypeCode();
        
        if (TypeCode::isUnit (type)     || 
            TypeCode::isUnits (type)    ||
            TypeCode::isBusses (type))  
            result.put (keys.atUnchecked (i), item.getChannel (index));
        else
            result.put (keys.atUnchecked (i), item);
    }
    
    return result;
}

int InputDictionary::getMaxNumChannels() const throw()
{
    const DynamicArray& items = this->getValues();
    const int numItems = items.length();
    
    if (numItems == 0)
        return 0;
        
    int numChannels = 0;
    
    for (int i = 0; i < numItems; ++i)
    {
        const Dynamic& item = items.atUnchecked (i);
        const int unitNumChannels = item.getNumChannels();
        numChannels = plonk::max (numChannels, unitNumChannels);
    }
    
    return numChannels;        
}

void InputDictionary::resetExpiredUnits() throw()
{
    DynamicArray items = this->getValues();
    const int numItems = items.length();
    
    for (int i = 0; i < numItems; ++i)
    {
        Dynamic& item = items.atUnchecked (i);
        const int type = item.getTypeCode();

        switch (type)
        {
            case TypeCode::FloatUnit:  item.asUnchecked<FloatUnit>().resetIfExpired();  break;
            case TypeCode::DoubleUnit: item.asUnchecked<DoubleUnit>().resetIfExpired(); break;
            case TypeCode::IntUnit:    item.asUnchecked<IntUnit>().resetIfExpired();    break;
            case TypeCode::ShortUnit:  item.asUnchecked<ShortUnit>().resetIfExpired();  break;
            case TypeCode::Int24Unit:  item.asUnchecked<Int24Unit>().resetIfExpired();  break;
            case TypeCode::LongUnit:   item.asUnchecked<LongUnit>().resetIfExpired();   break;
        }
    }
}


END_PLONK_NAMESPACE
