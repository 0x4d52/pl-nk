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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../core/plonk_Headers.h"

Dynamic::Dynamic() throw()
:   Base (new DynamicInternal())
{
}

Dynamic::Dynamic (Dynamic const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}

Dynamic& Dynamic::operator= (Dynamic const& other) throw()
{
    if (this != &other)
        this->setInternal (other.getInternal());
    
    return *this;
}

Dynamic::Dynamic (Internal* internalToUse) throw() 
:   Base (internalToUse)
{
}

Dynamic Dynamic::fromWeak (Weak const& weak) throw()
{
    return weak.fromWeak();
}    

Dynamic Dynamic::getMaxBlockSize() const throw()
{
    plonk_assert (this->isItemNotNull());
    const int typeCode = this->getTypeCode();
    
    switch (typeCode) 
    {
        case TypeCode::FloatUnit:   return reinterpret_cast<const FloatUnit&>  (this->getItem()).getMaxBlockSize();
        case TypeCode::ShortUnit:   return reinterpret_cast<const ShortUnit&>  (this->getItem()).getMaxBlockSize();
        case TypeCode::DoubleUnit:  return reinterpret_cast<const DoubleUnit&> (this->getItem()).getMaxBlockSize();
        case TypeCode::IntUnit:     return reinterpret_cast<const IntUnit&>    (this->getItem()).getMaxBlockSize();
        case TypeCode::Int24Unit:   return reinterpret_cast<const Int24Unit&>  (this->getItem()).getMaxBlockSize();
        case TypeCode::LongUnit:    return reinterpret_cast<const LongUnit&>   (this->getItem()).getMaxBlockSize();
        default:
            plonk_assertfalse;
            return Math<BlockSize>::get0();//return BlockSize::getZero();//0;
    }    
}


Dynamic Dynamic::getMaxSampleRate() const throw()
{
    plonk_assert (this->isItemNotNull());
    const int typeCode = this->getTypeCode();
    
    switch (typeCode) 
    {
        case TypeCode::FloatUnit:   return reinterpret_cast<const FloatUnit&>  (this->getItem()).getMaxSampleRate();
        case TypeCode::ShortUnit:   return reinterpret_cast<const ShortUnit&>  (this->getItem()).getMaxSampleRate();
        case TypeCode::DoubleUnit:  return reinterpret_cast<const DoubleUnit&> (this->getItem()).getMaxSampleRate();
        case TypeCode::IntUnit:     return reinterpret_cast<const IntUnit&>    (this->getItem()).getMaxSampleRate();
        case TypeCode::Int24Unit:   return reinterpret_cast<const Int24Unit&>  (this->getItem()).getMaxSampleRate();
        case TypeCode::LongUnit:    return reinterpret_cast<const LongUnit&>   (this->getItem()).getMaxSampleRate();
        default:
            plonk_assertfalse;
            return Math<SampleRate>::get0();//return SampleRate::getZero();//0;
    }    
}


int Dynamic::getNumChannels() const throw()
{
    plonk_assert (this->isItemNotNull());
    const int typeCode = this->getTypeCode();
    
    switch (typeCode) 
    {
        case TypeCode::FloatUnit:           return reinterpret_cast<const FloatUnit&>           (this->getItem()).getNumChannels();
        case TypeCode::FloatUnits:          return reinterpret_cast<const FloatUnits&>          (this->getItem()).numColumns();
        case TypeCode::FloatBusses:         return reinterpret_cast<const FloatBusses&>         (this->getItem()).length();  
        case TypeCode::FloatSignal:         return reinterpret_cast<const FloatSignal&>         (this->getItem()).getNumChannels();
        case TypeCode::FloatUnitVariable:   return reinterpret_cast<const FloatUnitVariable&>   (this->getItem()).getValue().getNumChannels();
            
        case TypeCode::ShortUnit:       return reinterpret_cast<const ShortUnit&>       (this->getItem()).getNumChannels();
        case TypeCode::ShortUnits:      return reinterpret_cast<const ShortUnits&>      (this->getItem()).numColumns();
        case TypeCode::ShortBusses:     return reinterpret_cast<const ShortBusses&>     (this->getItem()).length();
        case TypeCode::ShortSignal:     return reinterpret_cast<const ShortSignal&>     (this->getItem()).getNumChannels();

        case TypeCode::DoubleUnit:      return reinterpret_cast<const DoubleUnit&>      (this->getItem()).getNumChannels();
        case TypeCode::DoubleUnits:     return reinterpret_cast<const DoubleUnits&>     (this->getItem()).numColumns();
        case TypeCode::DoubleBusses:    return reinterpret_cast<const DoubleBusses&>    (this->getItem()).length();
        case TypeCode::DoubleSignal:    return reinterpret_cast<const DoubleSignal&>    (this->getItem()).getNumChannels();

        case TypeCode::IntUnit:         return reinterpret_cast<const IntUnit&>         (this->getItem()).getNumChannels();
        case TypeCode::IntUnits:        return reinterpret_cast<const IntUnits&>        (this->getItem()).numColumns();
        case TypeCode::IntBusses:       return reinterpret_cast<const IntBusses&>       (this->getItem()).length();
        case TypeCode::IntSignal:       return reinterpret_cast<const IntSignal&>       (this->getItem()).getNumChannels();

        case TypeCode::Int24Unit:       return reinterpret_cast<const Int24Unit&>       (this->getItem()).getNumChannels();
        case TypeCode::Int24Units:      return reinterpret_cast<const Int24Units&>      (this->getItem()).numColumns();
        case TypeCode::Int24Busses:     return reinterpret_cast<const Int24Busses&>     (this->getItem()).length();
        case TypeCode::Int24Signal:     return reinterpret_cast<const Int24Signal&>     (this->getItem()).getNumChannels();

        case TypeCode::LongUnit:        return reinterpret_cast<const LongUnit&>        (this->getItem()).getNumChannels();
        case TypeCode::LongUnits:       return reinterpret_cast<const LongUnits&>       (this->getItem()).numColumns();
        case TypeCode::LongBusses:      return reinterpret_cast<const LongBusses&>      (this->getItem()).length();
        case TypeCode::LongSignal:      return reinterpret_cast<const LongSignal&>      (this->getItem()).getNumChannels();
            
        case TypeCode::FloatFFTBuffers: return reinterpret_cast<const FloatFFTBuffers&> (this->getItem()).getNumChannels();
            
            
        // all singular...
        case TypeCode::Float:
        case TypeCode::Double:
        case TypeCode::Int: 
        case TypeCode::Int24: 
        case TypeCode::Short: 
        case TypeCode::Long: 
        case TypeCode::Char:
        case TypeCode::Bool:
            
        case TypeCode::FloatVariable:
        case TypeCode::DoubleVariable:
        case TypeCode::IntVariable: 
        case TypeCode::Int24Variable: 
        case TypeCode::ShortVariable: 
        case TypeCode::LongVariable: 
        case TypeCode::CharVariable:
        case TypeCode::BoolVariable:

        case TypeCode::AtomicFloatVariable:
        case TypeCode::AtomicDoubleVariable:
        case TypeCode::AtomicIntVariable: 
        case TypeCode::AtomicLongVariable: 
        case TypeCode::AtomicPointerVariable:
        case TypeCode::AtomicExtendedPointerVariable: 
        case TypeCode::AtomicDynamicPointerVariable:
            
        case TypeCode::FloatArray:
        case TypeCode::DoubleArray:
        case TypeCode::IntArray: 
        case TypeCode::Int24Array: 
        case TypeCode::ShortArray: 
        case TypeCode::LongArray: 
        case TypeCode::CharArray:
        case TypeCode::BoolArray:
            
        case TypeCode::FloatBus:
        case TypeCode::ShortBus:
        case TypeCode::DoubleBus:
        case TypeCode::IntBus:
        case TypeCode::Int24Bus:
        case TypeCode::LongBus:
            
        case TypeCode::FloatBreakpoints:
        case TypeCode::ShortBreakpoints:
        case TypeCode::DoubleBreakpoints:
        case TypeCode::IntBreakpoints:
        case TypeCode::Int24Breakpoints:
        case TypeCode::LongBreakpoints:
            
        case TypeCode::FloatChannel:
        case TypeCode::ShortChannel:
        case TypeCode::DoubleChannel:
        case TypeCode::IntChannel:
        case TypeCode::Int24Channel:
        case TypeCode::LongChannel:  
            
        case TypeCode::FloatWavetable:
        case TypeCode::ShortWavetable:
        case TypeCode::DoubleWavetable:
        case TypeCode::IntWavetable:
        case TypeCode::Int24Wavetable:
        case TypeCode::LongWavetable:

        case TypeCode::FloatUnitQueue:
        case TypeCode::ShortUnitQueue:
        case TypeCode::DoubleUnitQueue:
        case TypeCode::IntUnitQueue:
        case TypeCode::Int24UnitQueue:
        case TypeCode::LongUnitQueue:
            
        case TypeCode::FloatBufferQueue:
        case TypeCode::ShortBufferQueue:
        case TypeCode::DoubleBufferQueue:
        case TypeCode::IntBufferQueue:
        case TypeCode::Int24BufferQueue:
        case TypeCode::LongBufferQueue:   return 1;
            
        default:
            plonk_assertfalse;
            return 0;
    }    
}


Dynamic Dynamic::getChannel (const int index) throw()
{
    plonk_assert (this->isItemNotNull());
    const int typeCode = this->getTypeCode();
    
    switch (typeCode) 
    {
        case TypeCode::FloatUnit:       return reinterpret_cast<FloatUnit&> (this->getItem()).getChannel (index);
        case TypeCode::FloatUnits:      return FloatUnits (reinterpret_cast<FloatUnits&> (this->getItem()).columnWrapAt (index));
        case TypeCode::FloatBusses:     return FloatBusses (reinterpret_cast<FloatBusses&> (this->getItem()).wrapAt (index));
            
        case TypeCode::ShortUnit:       return reinterpret_cast<ShortUnit&> (this->getItem()).getChannel (index);
        case TypeCode::ShortUnits:      return ShortUnits (reinterpret_cast<ShortUnits&> (this->getItem()).columnWrapAt (index));
        case TypeCode::ShortBusses:     return ShortBusses (reinterpret_cast<ShortBusses&> (this->getItem()).wrapAt (index));
            
        case TypeCode::DoubleUnit:      return reinterpret_cast<DoubleUnit&> (this->getItem()).getChannel (index);
        case TypeCode::DoubleUnits:     return DoubleUnits (reinterpret_cast<DoubleUnits&> (this->getItem()).columnWrapAt (index));
        case TypeCode::DoubleBusses:    return DoubleBusses (reinterpret_cast<DoubleBusses&> (this->getItem()).wrapAt (index));
            
        case TypeCode::IntUnit:         return reinterpret_cast<IntUnit&> (this->getItem()).getChannel (index);
        case TypeCode::IntUnits:        return IntUnits (reinterpret_cast<IntUnits&> (this->getItem()).columnWrapAt (index));
        case TypeCode::IntBusses:       return IntBusses (reinterpret_cast<IntBusses&> (this->getItem()).wrapAt (index));

        case TypeCode::Int24Unit:       return reinterpret_cast<Int24Unit&> (this->getItem()).getChannel (index);
        case TypeCode::Int24Units:      return Int24Units (reinterpret_cast<Int24Units&> (this->getItem()).columnWrapAt (index));
        case TypeCode::Int24Busses:     return Int24Busses (reinterpret_cast<Int24Busses&> (this->getItem()).wrapAt (index));            
            
        case TypeCode::LongUnit:        return reinterpret_cast<LongUnit&> (this->getItem()).getChannel (index);
        case TypeCode::LongUnits:       return LongUnits (reinterpret_cast<LongUnits&> (this->getItem()).columnWrapAt (index));
        case TypeCode::LongBusses:      return LongBusses (reinterpret_cast<LongBusses&> (this->getItem()).wrapAt (index));

        case TypeCode::FloatFFTBuffers: return FloatFFTBuffers (reinterpret_cast<FloatFFTBuffers&> (this->getItem()).getChannel (index));
            
        default:
            plonk_assertfalse;
            return Dynamic();
    }    
}

END_PLONK_NAMESPACE

