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

#ifndef PLONK_ATOMIC_H
#define PLONK_ATOMIC_H

/** Type-independent atomic operations. */
class AtomicOps //: public PlonkBase // need to reorganise headers etc
{
public:
    PLONK_INLINE_MID static void memoryBarrier() throw()
    {
        pl_AtomicMemoryBarrier();
    }
};

template<class Type>
class AtomicBase : public AtomicOps
{
};

template<class Type>
class AtomicBase<Type*> : public AtomicOps
{
public:
    static PLONK_INLINE_MID Long incrementSize() { return sizeof (Type); }
};

template<>
class AtomicBase<void*> : public AtomicOps
{
public:
    static PLONK_INLINE_MID Long incrementSize() { return 1; }
};


/** Enables atomic operations on a range of types.
 @tparam This must be one of: int, long, LongLong, float, double or pointer types.
 @ingroup PlonkContainerClasses */
template<class Type>
class AtomicValue : public AtomicBase<Type>
{
public:
};

/** Enables atomic operations on a pointer with additional security.
 This stores an additional atomically accesses counter to make the liklihood of the ABA
 problem (etc) much lower.
 @tparam This must be a pointer type.
 @ingroup PlonkContainerClasses */
template<class Type>
class AtomicExtended : public AtomicBase<Type>
{
};


#define PLONK_ATOMIC_DEFINE(ATOMIC_CLASS,TYPECODE,FUNCCODE,NUMCODE,ALIGN) \
    template<>\
    class ATOMIC_CLASS<Plank##TYPECODE> : public AtomicBase<Plank##TYPECODE>\
    {\
    public:\
        typedef PlankAtomic##FUNCCODE AtomType;\
        typedef PlankAtomic##FUNCCODE##Ref AtomTypeRef;\
        \
        PLONK_INLINE_MID ATOMIC_CLASS() throw() {\
            pl_Atomic##FUNCCODE##_Init (getAtomicRef());\
        }\
        \
        PLONK_INLINE_MID ATOMIC_CLASS (const Plank##TYPECODE initialValue) throw() {\
            pl_Atomic##FUNCCODE##_Init (getAtomicRef());\
            pl_Atomic##FUNCCODE##_SetUnchecked (getAtomicRef(), initialValue);\
        }\
        \
        PLONK_INLINE_MID ATOMIC_CLASS (ATOMIC_CLASS const& copy) throw() {\
            pl_Atomic##FUNCCODE##_Init (getAtomicRef());\
            pl_Atomic##FUNCCODE##_SetUnchecked (getAtomicRef(), pl_Atomic##FUNCCODE##_Get (copy.getAtomicRef()));\
        }\
        \
        PLONK_INLINE_MID  ~ATOMIC_CLASS() {\
            pl_Atomic##FUNCCODE##_DeInit (getAtomicRef());\
        }\
        \
        PLONK_INLINE_MID ATOMIC_CLASS& operator= (ATOMIC_CLASS const& other) throw() {\
            if (this != &other) pl_Atomic##FUNCCODE##_Set (getAtomicRef(), pl_Atomic##FUNCCODE##_Get (other.getAtomicRef()));\
            return *this;\
        }\
        PLONK_INLINE_MID ATOMIC_CLASS& operator= (const Plank##TYPECODE other) throw() {\
            pl_Atomic##FUNCCODE##_Set (getAtomicRef(), other);\
            return *this;\
        }\
        \
        template<class OtherType>\
        PLONK_INLINE_MID ATOMIC_CLASS& operator= (const OtherType other) throw() {\
            pl_Atomic##FUNCCODE##_Set (getAtomicRef(), static_cast<const Plank##TYPECODE> (other));\
            return *this;\
        }\
        \
        PLONK_INLINE_MID bool compareAndSwap (const Plank##NUMCODE oldValue, const Plank##NUMCODE newValue) throw() {\
            return pl_Atomic##FUNCCODE##_CompareAndSwap (getAtomicRef(), oldValue, newValue);\
        }\
        \
        PLONK_INLINE_MID bool compareAndSwap (const Plank##NUMCODE newValue) throw() {\
            return pl_Atomic##FUNCCODE##_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), newValue);\
        }\
        \
        PLONK_INLINE_MID Plank##NUMCODE swap (const Plank##NUMCODE newValue) throw() {\
            return pl_Atomic##FUNCCODE##_Swap (getAtomicRef(), newValue);\
        }\
        PLONK_INLINE_MID void swapWith (ATOMIC_CLASS& other) throw() {\
            return pl_Atomic##FUNCCODE##_SwapOther (getAtomicRef(), other.getAtomicRef());\
        }\
        \
        PLONK_INLINE_MID void setValue (const Plank##TYPECODE other) throw() { pl_Atomic##FUNCCODE##_Set (getAtomicRef(), other); }\
        PLONK_INLINE_MID Plank##TYPECODE getValue() const throw() { return pl_Atomic##FUNCCODE##_Get (getAtomicRef()); }\
        PLONK_INLINE_MID Plank##TYPECODE getValueUnchecked() const throw() { return pl_Atomic##FUNCCODE##_GetUnchecked (getAtomicRef()); }\
        \
        template<class OtherType> operator OtherType () const throw() { return static_cast<OtherType> (pl_Atomic##FUNCCODE##_Get (getAtomicRef())); }\
        PLONK_INLINE_MID operator Plank##TYPECODE () const throw() { return pl_Atomic##FUNCCODE##_Get (getAtomicRef()); }\
        PLONK_INLINE_MID const Plank##NUMCODE getExtra() const throw() { return pl_Atomic##FUNCCODE##_GetExtra (getAtomicRef()); }\
        PLONK_INLINE_MID const Plank##NUMCODE getExtraUnchecked() const throw() { return pl_Atomic##FUNCCODE##_GetExtraUnchecked (getAtomicRef()); }\
        PLONK_INLINE_MID const Plank##TYPECODE operator+= (const Plank##NUMCODE operand) throw() { return pl_Atomic##FUNCCODE##_Add (getAtomicRef(), operand); }\
        PLONK_INLINE_MID const Plank##TYPECODE operator-= (const Plank##NUMCODE operand) throw() { return pl_Atomic##FUNCCODE##_Add (getAtomicRef(), -operand); }\
        PLONK_INLINE_MID const Plank##TYPECODE operator++() throw() { return pl_Atomic##FUNCCODE##_Increment (getAtomicRef()); }\
        PLONK_INLINE_MID const Plank##TYPECODE operator--() throw() { return pl_Atomic##FUNCCODE##_Decrement (getAtomicRef()); }\
        PLONK_INLINE_MID const Plank##TYPECODE operator++ (int) throw() { const Plank##TYPECODE oldValue = pl_Atomic##FUNCCODE##_Get (getAtomicRef()); ++(*this); return oldValue; }\
        PLONK_INLINE_MID const Plank##TYPECODE operator-- (int) throw() { const Plank##TYPECODE oldValue = pl_Atomic##FUNCCODE##_Get (getAtomicRef()); --(*this); return oldValue; }\
        \
        template<class OtherType> bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == Plank##TYPECODE (other); }\
        template<class OtherType> bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != Plank##TYPECODE (other); }\
        template<class OtherType> bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  Plank##TYPECODE (other); }\
        template<class OtherType> bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= Plank##TYPECODE (other); }\
        template<class OtherType> bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  Plank##TYPECODE (other); }\
        template<class OtherType> bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= Plank##TYPECODE (other); }\
        \
        PLONK_INLINE_MID const void setIfLarger (const Plank##TYPECODE newValue) throw()\
        {\
            Plank##TYPECODE oldValue;\
            bool success;\
            do {\
                oldValue = getValueUnchecked();\
                success = compareAndSwap (oldValue, newValue > oldValue ? newValue : oldValue);\
            } while (!success);\
        }\
        \
        PLONK_INLINE_MID const void setIfSmaller (const Plank##TYPECODE newValue) throw()\
        {\
            Plank##TYPECODE oldValue;\
            bool success;\
            do {\
                oldValue = getValueUnchecked();\
                success = compareAndSwap (oldValue, newValue < oldValue ? newValue : oldValue);\
            } while (!success);\
        }\
        \
        PLONK_INLINE_MID AtomTypeRef getAtomicRef() { return static_cast<AtomTypeRef> (&atomic); }\
        PLONK_INLINE_MID const AtomTypeRef getAtomicRef() const { return const_cast<AtomTypeRef> (&atomic); }\
    private:\
        \
        PLONK_ALIGN(ALIGN)\
        AtomType atomic;\
    };

#define PLONK_ATOMIC_DEFINE_SIMPLE(TYPECODE,ALIGN) PLONK_ATOMIC_DEFINE(AtomicValue,TYPECODE,TYPECODE,TYPECODE,ALIGN)

PLONK_ATOMIC_DEFINE_SIMPLE(I,4);
PLONK_ATOMIC_DEFINE(AtomicValue,UI,I,I,4);
PLONK_ATOMIC_DEFINE_SIMPLE(L,PLONK_WORDSIZE);
PLONK_ATOMIC_DEFINE(AtomicValue,UL,L,L,PLONK_WORDSIZE);

#if !(PLANK_WIN && PLANK_64BIT) || DOXYGEN
PLONK_ATOMIC_DEFINE_SIMPLE(LL,8);
PLONK_ATOMIC_DEFINE(AtomicValue,ULL,LL,LL,8);
#endif

PLONK_ATOMIC_DEFINE_SIMPLE(F,4);
PLONK_ATOMIC_DEFINE_SIMPLE(D,8);

template<class Type>
class AtomicValue<Type*> : public AtomicBase<Type*>
{
public:
    PLONK_INLINE_MID AtomicValue() throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
    }

    PLONK_INLINE_MID AtomicValue (Type& initialValue) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), &initialValue);
    }
    
	template<class OtherType>
    PLONK_INLINE_MID AtomicValue (OtherType* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), static_cast<Type*> (initialPtr));
    }
    
    PLONK_INLINE_MID AtomicValue (Type* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), initialPtr);
    }
    
    PLONK_INLINE_MID AtomicValue (AtomicValue const& copy) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), pl_AtomicP_Get (copy.getAtomicRef()));
    }
    
    PLONK_INLINE_MID ~AtomicValue() 
    {
        pl_AtomicP_DeInit (getAtomicRef());
    }
    
    PLONK_INLINE_MID AtomicValue& operator= (AtomicValue const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicP_Set (getAtomicRef(), pl_AtomicP_Get (other.getAtomicRef()));
        
        return *this;
    }
        
	template<class OtherType>
    PLONK_INLINE_MID AtomicValue& operator= (OtherType* other) throw() 
    {
        pl_AtomicP_Set (getAtomicRef(), static_cast<Type*> (other));
        return *this;
    }    

    PLONK_INLINE_MID AtomicValue& operator= (Type& other) throw() 
    {
        pl_AtomicP_Set (getAtomicRef(), static_cast<Type*> (&other));
        return *this;
    }    
    
    PLONK_INLINE_MID bool compareAndSwap (const Type* oldValue, const Type* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), oldValue, newValue);
    }
    
    PLONK_INLINE_MID bool compareAndSwap (const Type* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), newValue);
    }
    
    PLONK_INLINE_MID Type* swap (const Type* newValue) throw() 
    {
        return pl_AtomicP_Swap (getAtomicRef(), newValue);
    }
    
    PLONK_INLINE_MID void swapWith (AtomicValue& other) throw() 
    {
        return pl_AtomicP_SwapOther (getAtomicRef(), other.getAtomicRef());
    }

    PLONK_INLINE_MID void setValue (Type* other) throw()              { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }
    PLONK_INLINE_MID void setPtr (Type* other) throw()                { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }

    PLONK_INLINE_MID Type* getValue() const throw()                   { return static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); }
    PLONK_INLINE_MID Type* getPtr() const throw()                     { return static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); }
    PLONK_INLINE_MID Type* operator->() const throw()                 { return static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); }

    PLONK_INLINE_MID Type* getValueUnchecked() const throw()          { return static_cast<Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    PLONK_INLINE_MID Type* getPtrUnchecked() const throw()            { return static_cast<Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    PLONK_INLINE_MID Long getExtra() const throw()                    { return pl_AtomicP_GetExtra (getAtomicRef()); }
    PLONK_INLINE_MID Long getExtraUnchecked() const throw()           { return pl_AtomicP_GetExtraUnchecked (getAtomicRef()); }

    PLONK_INLINE_MID operator const Type* () const throw()            { return static_cast<const Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    PLONK_INLINE_MID operator Type* () const throw()                  { return static_cast<Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }

    PLONK_INLINE_MID void deletePtr() throw()                         { delete this->getPtrUnchecked(); }
    PLONK_INLINE_MID void deleteAndZeroPtr() throw()                  { delete this->swap (0); }

    template<class OtherType> PLONK_INLINE_MID bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Type*> (other); }

    PLONK_INLINE_MID bool operator== (Type* const other) const throw() { return this->getPtrUnchecked() == other; }
    PLONK_INLINE_MID bool operator!= (Type* const other) const throw() { return this->getPtrUnchecked() != other; }
    PLONK_INLINE_MID bool operator<  (Type* const other) const throw() { return this->getPtrUnchecked() <  other; }
    PLONK_INLINE_MID bool operator<= (Type* const other) const throw() { return this->getPtrUnchecked() <= other; }
    PLONK_INLINE_MID bool operator>  (Type* const other) const throw() { return this->getPtrUnchecked() >  other; }
    PLONK_INLINE_MID bool operator>= (Type* const other) const throw() { return this->getPtrUnchecked() >= other; }

    PLONK_INLINE_MID Type* const operator+= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), operand * AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID Type* const operator-= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), -operand * AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID Type* const operator++() throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID Type* const operator--() throw() 
    { 
        return pl_AtomicP_Subtract (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID Type* const operator++ (int) throw() 
    { 
        Type* const oldPtr = static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); 
        ++(*this); 
        return oldPtr; 
    }
    
    PLONK_INLINE_MID Type* const operator-- (int) throw() 
    { 
        Type* const oldPtr = static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); 
        --(*this); 
        return oldPtr; 
    }
    
    PLONK_INLINE_MID PlankAtomicPRef getAtomicRef() { return static_cast<PlankAtomicPRef> (&u.atomic); }
    PLONK_INLINE_MID const PlankAtomicPRef getAtomicRef() const { return const_cast<PlankAtomicPRef> (&u.atomic); }
    
private:
    union Union
    {
        PlankAtomicP atomic;
        Type* ptr;
    };
    
    PLONK_ALIGN(PLONK_WORDSIZE) Union u;
};

template<>
class AtomicValue<void*> : public AtomicBase<void*>
{
public:
    PLONK_INLINE_MID AtomicValue() throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
    }
        
	template<class OtherType>
    PLONK_INLINE_MID AtomicValue (OtherType* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), static_cast<void*> (initialPtr));
    }
    
    PLONK_INLINE_MID AtomicValue (void* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), initialPtr);
    }
    
    PLONK_INLINE_MID AtomicValue (AtomicValue const& copy) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_SetUnchecked (getAtomicRef(), pl_AtomicP_Get (copy.getAtomicRef()));
    }
    
    PLONK_INLINE_MID ~AtomicValue() 
    {
        pl_AtomicP_DeInit (getAtomicRef());
    }
    
    PLONK_INLINE_MID AtomicValue& operator= (AtomicValue const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicP_Set (getAtomicRef(), pl_AtomicP_Get (other.getAtomicRef()));
        
        return *this;
    }
    
	template<class OtherType>
    PLONK_INLINE_MID AtomicValue& operator= (OtherType* other) throw() 
    {
        pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other));
        return *this;
    }    
        
    PLONK_INLINE_MID bool compareAndSwap (void* oldValue, void* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), oldValue, newValue);
    }
    
    PLONK_INLINE_MID bool compareAndSwap (void* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), newValue);
    }
    
    PLONK_INLINE_MID void* swap (void* newValue) throw() 
    {
        return pl_AtomicP_Swap (getAtomicRef(), newValue);
    }
    
    PLONK_INLINE_MID void swapWith (AtomicValue& other) throw() 
    {
        return pl_AtomicP_SwapOther (getAtomicRef(), other.getAtomicRef());
    }
    
    PLONK_INLINE_MID void setValue (void* other) throw()              { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }
    PLONK_INLINE_MID void setPtr (void* other) throw()                { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }
        
    PLONK_INLINE_MID void* getValue() const throw()                   { return pl_AtomicP_Get (getAtomicRef()); }
    PLONK_INLINE_MID void* getPtr() const throw()                     { return pl_AtomicP_Get (getAtomicRef()); }
    PLONK_INLINE_MID void* operator->() const throw()                 { return pl_AtomicP_Get (getAtomicRef()); }
    
    PLONK_INLINE_MID void* getValueUnchecked() const throw()          { return pl_AtomicP_GetUnchecked (getAtomicRef()); }
    PLONK_INLINE_MID void* getPtrUnchecked() const throw()            { return pl_AtomicP_GetUnchecked (getAtomicRef()); }
    PLONK_INLINE_MID UnsignedLong getExtra() const throw()                    { return pl_AtomicP_GetExtra (getAtomicRef()); }
    PLONK_INLINE_MID UnsignedLong getExtraUnchecked() const throw()           { return pl_AtomicP_GetExtraUnchecked (getAtomicRef()); }
    
    PLONK_INLINE_MID operator const void* () const throw()            { return static_cast<const void*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    PLONK_INLINE_MID operator void* () const throw()                  { return pl_AtomicP_GetUnchecked (getAtomicRef()); }
        
    template<class OtherType> PLONK_INLINE_MID bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == reinterpret_cast<void*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != reinterpret_cast<void*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  reinterpret_cast<void*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= reinterpret_cast<void*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  reinterpret_cast<void*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= reinterpret_cast<void*> (other); }
    
    PLONK_INLINE_MID bool operator== (void* const other) const throw() { return this->getPtrUnchecked() == other; }
    PLONK_INLINE_MID bool operator!= (void* const other) const throw() { return this->getPtrUnchecked() != other; }
    PLONK_INLINE_MID bool operator<  (void* const other) const throw() { return this->getPtrUnchecked() <  other; }
    PLONK_INLINE_MID bool operator<= (void* const other) const throw() { return this->getPtrUnchecked() <= other; }
    PLONK_INLINE_MID bool operator>  (void* const other) const throw() { return this->getPtrUnchecked() >  other; }
    PLONK_INLINE_MID bool operator>= (void* const other) const throw() { return this->getPtrUnchecked() >= other; }
    
    PLONK_INLINE_MID void* const operator+= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), operand * AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID void* const operator-= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), -operand * AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID void* const operator++() throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID void* const operator--() throw() 
    { 
        return pl_AtomicP_Subtract (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    PLONK_INLINE_MID void* const operator++ (int) throw() 
    { 
        void* const oldPtr = pl_AtomicP_Get (getAtomicRef()); 
        ++(*this); 
        return oldPtr; 
    }
    
    PLONK_INLINE_MID void* const operator-- (int) throw() 
    { 
        void* const oldPtr = pl_AtomicP_Get (getAtomicRef()); 
        --(*this); 
        return oldPtr; 
    }
    
    PLONK_INLINE_MID PlankAtomicPRef getAtomicRef() { return static_cast<PlankAtomicPRef> (&atomic); }
    PLONK_INLINE_MID const PlankAtomicPRef getAtomicRef() const { return const_cast<PlankAtomicPRef> (&atomic); }
   
private:
    PLONK_ALIGN(PLONK_WORDSIZE) 
    PlankAtomicP atomic;
};


template<class Type>
class AtomicExtended<Type*> : public AtomicBase<Type*>
{
public:
    
    template<class OtherType>
    static PLONK_INLINE_MID bool ptrUsesValidBits (OtherType* ptr) throw()
    {
        union Utility
        {
            OtherType* ptr;
            UnsignedLong bits;
        };

        Utility u;
        u.ptr = ptr;
        return (u.bits & PLANK_ATOMIC_PMASK) == u.bits;
    }
    
    PLONK_INLINE_MID AtomicExtended() throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
    }
    
    PLONK_INLINE_MID AtomicExtended (Type& initialValue) throw() 
    {
        plonk_assert (ptrUsesValidBits (&initialValue));
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_SetAllUnchecked (getAtomicRef(), static_cast<void*> (&initialValue), 0);
    }
    
	template<class OtherType>
    PLONK_INLINE_MID AtomicExtended (OtherType* const initialPtr) throw() 
    {
        plonk_assert (ptrUsesValidBits (initialPtr));
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_SetAllUnchecked (getAtomicRef(), static_cast<Type*> (initialPtr), 0);
    }
    
    PLONK_INLINE_MID AtomicExtended (Type* initialPtr) throw() 
    {
        plonk_assert (ptrUsesValidBits (initialPtr));
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_SetAllUnchecked (getAtomicRef(), initialPtr, 0);
    }
    
    PLONK_INLINE_MID AtomicExtended (Type* initialPtr, const UnsignedLong extra) throw()
    {
        plonk_assert (ptrUsesValidBits (initialPtr));
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_SetAllUnchecked (getAtomicRef(), initialPtr, extra);
    }
    
    PLONK_INLINE_MID AtomicExtended (AtomicExtended const& copy) throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_SetAllUnchecked (getAtomicRef(), pl_AtomicPX_Get (copy.getAtomicRef()), 0);
    }
    
    PLONK_INLINE_MID ~AtomicExtended() 
    {
        pl_AtomicPX_DeInit (getAtomicRef());
    }
    
    PLONK_INLINE_MID AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicPX_Set (getAtomicRef(), pl_AtomicPX_Get (other.getAtomicRef()));
        
        return *this;
    }
    
//    PLONK_INLINE_MID AtomicExtended& operator= (void* other) throw() 
//    {
//        plonk_assert (other == 0);
//        pl_AtomicPX_Set (getAtomicRef(), static_cast<Type*> (other));
//        return *this;
//    }    
//    

	template<class OtherType>
    PLONK_INLINE_MID AtomicExtended& operator= (OtherType* const other) throw() 
    {
        plonk_assert (ptrUsesValidBits (other));
        pl_AtomicPX_Set (getAtomicRef(), static_cast<Type*> (other));
        return *this;
    }    
    
//    PLONK_INLINE_MID AtomicExtended& operator= (Type& other) throw() 
//    {
//        pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (&other));
//        return *this;
//    }    

    PLONK_INLINE_MID AtomicExtended& operator= (Type* other) throw() 
    {
        plonk_assert (ptrUsesValidBits (other));
        pl_AtomicPX_Set (getAtomicRef(), const_cast<Type*> (other));
        return *this;
    }        
    
    PLONK_INLINE_MID void setAll (Type* const other, const UnsignedLong extra) throw()
    {
        plonk_assert (ptrUsesValidBits (other));
        pl_AtomicPX_SetAll (getAtomicRef(), other, extra);
    }
    
    PLONK_INLINE_MID bool compareAndSwap (Type* const oldValue, const UnsignedLong oldExtra, Type* const newValue, const UnsignedLong newExtra) throw()
    {
        plonk_assert (ptrUsesValidBits (newValue));
        return pl_AtomicPX_CompareAndSwap (getAtomicRef(), oldValue, oldExtra, newValue, newExtra);
    }
    
    PLONK_INLINE_MID bool compareAndSwap (Type* const newValue, const UnsignedLong newExtra) throw()
    {
        plonk_assert (ptrUsesValidBits (newValue));
        return pl_AtomicPX_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), this->getExtraUnchecked(), newValue, newExtra);
    }
    
    PLONK_INLINE_MID Type* swap (Type* const newValue) throw() 
    {
        plonk_assert (ptrUsesValidBits (newValue));
        return pl_AtomicPX_Swap (getAtomicRef(), newValue);
    }
    
    PLONK_INLINE_MID Type* swapAll (Type* const newValue, const UnsignedLong newExtra) throw()
    {
        plonk_assert (ptrUsesValidBits (newValue));
        return pl_AtomicPX_SwapAll (getAtomicRef(), newValue, newExtra, 0);
    }
    
    PLONK_INLINE_MID Type* swapAll (Type* const newValue, const UnsignedLong newExtra, UnsignedLong& oldExtra) throw()
    {
        plonk_assert (ptrUsesValidBits (newValue));
        return pl_AtomicPX_SwapAll (getAtomicRef(), newValue, newExtra, &oldExtra);
    }
    
    PLONK_INLINE_MID void swapWith (AtomicExtended& other) throw() 
    {
        return pl_AtomicPX_SwapOther (getAtomicRef(), other.getAtomicRef());
    }
    
    PLONK_INLINE_MID void setValue (Type* const other) throw()        { plonk_assert (ptrUsesValidBits (other)); pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (other)); }
    PLONK_INLINE_MID void setPtr (Type* const other) throw()          { plonk_assert (ptrUsesValidBits (other)); pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (other)); }
    
    PLONK_INLINE_MID Type* getValue() const throw()                   { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }
    PLONK_INLINE_MID Type* getPtr() const throw()                     { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }
    PLONK_INLINE_MID Type* operator->() const throw()                 { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }
    
    PLONK_INLINE_MID Type* getValueUnchecked() const throw()          { return static_cast<Type*> (pl_AtomicPX_GetUnchecked (getAtomicRef())); }
    PLONK_INLINE_MID Type* getPtrUnchecked() const throw()            { return static_cast<Type*> (pl_AtomicPX_GetUnchecked (getAtomicRef())); }

    PLONK_INLINE_MID operator const Type* () const throw()            { return static_cast<const Type*> (pl_AtomicPX_GetUnchecked (getAtomicRef())); }
    PLONK_INLINE_MID operator Type* () throw()                        { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }

    PLONK_INLINE_MID UnsignedLong getExtra() const throw()            { return pl_AtomicPX_GetExtra (getAtomicRef()); }
    PLONK_INLINE_MID UnsignedLong getExtraUnchecked() const throw()   { return pl_AtomicPX_GetExtraUnchecked (getAtomicRef()); }
    
    PLONK_INLINE_MID void deletePtr() throw()                         { delete this->getPtrUnchecked(); }
    PLONK_INLINE_MID void deleteAndZeroPtr() throw()                  { delete this->getPtrUnchecked(); this->setPtr (0); }

    template<class OtherType> PLONK_INLINE_MID bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Type*> (other); }
    template<class OtherType> PLONK_INLINE_MID bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Type*> (other); }
    
    PLONK_INLINE_MID bool operator== (Type* const other) const throw() { return this->getPtrUnchecked() == other; }
    PLONK_INLINE_MID bool operator!= (Type* const other) const throw() { return this->getPtrUnchecked() != other; }
    PLONK_INLINE_MID bool operator<  (Type* const other) const throw() { return this->getPtrUnchecked() <  other; }
    PLONK_INLINE_MID bool operator<= (Type* const other) const throw() { return this->getPtrUnchecked() <= other; }
    PLONK_INLINE_MID bool operator>  (Type* const other) const throw() { return this->getPtrUnchecked() >  other; }
    PLONK_INLINE_MID bool operator>= (Type* const other) const throw() { return this->getPtrUnchecked() >= other; }

//    PLONK_INLINE_MID Type* const operator+= (const Long operand) throw() 
//    { 
//        return pl_AtomicPX_Add (getAtomicRef(), operand * AtomicExtended::incrementSize()); 
//    }
//    
//    PLONK_INLINE_MID Type* const operator-= (const Long operand) throw() 
//    { 
//        return pl_AtomicPX_Add (getAtomicRef(), -operand * AtomicExtended::incrementSize()); 
//    }
//    
//    PLONK_INLINE_MID Type* const operator++() throw() 
//    { 
//        return pl_AtomicPX_Add (getAtomicRef(), AtomicExtended::incrementSize()); 
//    }
//    
//    PLONK_INLINE_MID Type* const operator--() throw() 
//    { 
//        return pl_AtomicPX_Subtract (getAtomicRef(), AtomicExtended::incrementSize()); 
//    }
//    
//    PLONK_INLINE_MID Type* const operator++ (int) throw() 
//    { 
//        Type* const oldPtr = static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); 
//        ++(*this); 
//        return oldPtr; 
//    }
//    
//    PLONK_INLINE_MID Type* const operator-- (int) throw() 
//    { 
//        Type* const oldPtr = static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); 
//        --(*this); 
//        return oldPtr; 
//    }
    
    PLONK_INLINE_MID PlankAtomicPXRef getAtomicRef() { return static_cast<PlankAtomicPXRef> (&atomic); }
    PLONK_INLINE_MID const PlankAtomicPXRef getAtomicRef() const { return const_cast<PlankAtomicPXRef> (&atomic); }
    
private:
    PLONK_ALIGN(PLONK_WIDESIZE) PlankAtomicPX atomic;
};

template<>
class AtomicExtended<Int> : public AtomicBase<LongLong>
{
public:
    PLONK_INLINE_MID AtomicExtended() throw() 
    {
    }
    
    PLONK_INLINE_MID AtomicExtended (const Int initialValue) throw() 
    :   atom (fromParts (initialValue, 0))
    {
    }    
    
    PLONK_INLINE_MID AtomicExtended (AtomicExtended const& copy) throw() 
    {
        setValue (copy.getValue());
    }
    
    PLONK_INLINE_MID ~AtomicExtended() 
    {
        plonk_staticassert (sizeof (Separate) == sizeof (LongLong));
        plonk_staticassert (sizeof (Element) == sizeof (LongLong));
    }
    
    PLONK_INLINE_MID AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            setValue (other.getValue());
        
        return *this;
    }
    
    PLONK_INLINE_MID AtomicExtended& operator= (const Int other) throw() 
    {
        setValue (other);
        return *this;
    }        
    
    PLONK_INLINE_MID void setAll (const Int other, const Int extra) throw() 
    { 
        atom.setValue (fromParts (other, extra));
    }
    
    PLONK_INLINE_MID bool compareAndSwap (const Int oldValue, const Int oldExtra, const Int newValue, const Int newExtra) throw() 
    {
        return atom.compareAndSwap (fromParts (oldValue, oldExtra), fromParts (newValue, newExtra));
    }
    
    PLONK_INLINE_MID bool compareAndSwap (const Int newValue, const Int newExtra) throw() 
    {
        return atom.compareAndSwap (atom.getValueUnchecked(), fromParts (newValue, newExtra));
    }
    
    PLONK_INLINE_MID Int swap (const Int newValue) throw() 
    {
        bool success;
        Element oldAll, newAll;
        
        do 
        {
            oldAll.whole = atom.getValueUnchecked();
            newAll.separate.value = newValue;
            newAll.separate.extra = oldAll.separate.extra + 1;
            success = atom.compareAndSwap (oldAll.whole, newAll.whole);
        } while (!success);
        
        return oldAll.separate.value;
    }
    
    Int swapAll (const Int newValue, const Int newExtra) throw() 
    {
        return swapAll (newValue, newExtra, 0);
    }
    
    Int swapAll (const Int newValue, const Int newExtra, Int* oldExtraPtr) throw() 
    {
        bool success;
        Element oldAll;
        
        do 
        {
            oldAll.whole = atom.getValueUnchecked();
            success = atom.compareAndSwap (oldAll.whole, fromParts (newValue, newExtra));
        } while (!success);
        
        if (oldExtraPtr != 0)
            *oldExtraPtr = oldAll.separate.extra;
        
        return oldAll.separate.value;
    }
    
    PLONK_INLINE_MID void setValue (const Int other) throw() 
    { 
        swap (other);
    }
    
    PLONK_INLINE_MID Int getValue() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.value;
    }
    
    PLONK_INLINE_MID Int getExtra() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.extra;
    }
    
    PLONK_INLINE_MID Int getValueUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.value;
    }
    
    PLONK_INLINE_MID Int getExtraUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.extra;
    }
    
    PLONK_INLINE_MID LongLong getAllUnchecked() const throw()
    {
        return atom.getValueUnchecked();
    }
    
    template<class OtherType> bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Int> (other); }
    template<class OtherType> bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Int> (other); }
    template<class OtherType> bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Int> (other); }
    template<class OtherType> bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Int> (other); }
    template<class OtherType> bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Int> (other); }
    template<class OtherType> bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Int> (other); }
    
    bool operator== (const Int other) const throw() { return this->getValueUnchecked() == other; }
    bool operator!= (const Int other) const throw() { return this->getValueUnchecked() != other; }
    bool operator<  (const Int other) const throw() { return this->getValueUnchecked() <  other; }
    bool operator<= (const Int other) const throw() { return this->getValueUnchecked() <= other; }
    bool operator>  (const Int other) const throw() { return this->getValueUnchecked() >  other; }
    bool operator>= (const Int other) const throw() { return this->getValueUnchecked() >= other; }

    PLONK_INLINE_MID Int operator+= (const Int operand) throw() 
    { 
        bool success;
        Element oldAll, newAll;
        
        do 
        {
            oldAll.whole = atom.getValueUnchecked();
            newAll.separate.value = oldAll.separate.value + operand;
            newAll.separate.extra = oldAll.separate.extra + 1;
            success = atom.compareAndSwap (oldAll.whole, newAll.whole);
        } while (!success);
        
        return newAll.separate.value;
    }
    
    PLONK_INLINE_MID Int operator-= (const Int operand) throw() 
    { 
        return operator+= (-operand);
    }
    
    PLONK_INLINE_MID Int operator++() throw() 
    { 
        return operator+= (1);
    }
    
    PLONK_INLINE_MID Int operator--() throw() 
    { 
        return operator+= (-1);
    }
    
    PLONK_INLINE_MID Int operator++ (int) throw() 
    { 
        const Int oldValue = getValueUnchecked(); 
        ++(*this); 
        return oldValue; 
    }
    
    PLONK_INLINE_MID Int operator-- (int) throw() 
    { 
        const Int oldValue = getValueUnchecked(); 
        --(*this); 
        return oldValue; 
    }
    
private:    
    AtomicValue<LongLong> atom;
    
    struct Separate
    {
        Int value;
        Int extra;
    };
        
    union Element
    {
        Separate separate;
        LongLong whole;
    };
    
    static PLONK_INLINE_MID LongLong fromParts (const Int value, const Int extra) throw()
    {
        Element element;
        element.separate.value = value;
        element.separate.extra = extra;
        return element.whole;
    }
    
    static PLONK_INLINE_MID Separate fromWhole (LongLong const& whole) throw()
    {
        Element element;
        element.whole = whole;
        return element.separate;
    }
    
};

template<>
class AtomicExtended<Short> : public AtomicBase<Int>
{
public:
    PLONK_INLINE_MID AtomicExtended() throw() 
    {
    }
    
    PLONK_INLINE_MID AtomicExtended (const Short initialValue) throw() 
    :   atom (fromParts (initialValue, 0))
    {
    }    
    
    PLONK_INLINE_MID AtomicExtended (AtomicExtended const& copy) throw() 
    {
        setValue (copy.getValue());
    }
    
    PLONK_INLINE_MID ~AtomicExtended() 
    {
        plonk_staticassert (sizeof (Separate) == sizeof (Int));
        plonk_staticassert (sizeof (Element) == sizeof (Int));
    }
    
    PLONK_INLINE_MID AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            setValue (other.getValue());
        
        return *this;
    }
    
    PLONK_INLINE_MID AtomicExtended& operator= (const Short other) throw() 
    {
        setValue (other);
        return *this;
    }        
    
    PLONK_INLINE_MID void setAll (const Short other, const Short extra) throw() 
    { 
        atom.setValue (fromParts (other, extra));
    }
    
    PLONK_INLINE_MID bool compareAndSwap (const Short oldValue, const Short oldExtra, const Short newValue, const Short newExtra) throw() 
    {
        return atom.compareAndSwap (fromParts (oldValue, oldExtra), fromParts (newValue, newExtra));
    }
    
    PLONK_INLINE_MID bool compareAndSwap (const Short newValue, const Short newExtra) throw() 
    {
        return atom.compareAndSwap (atom.getValueUnchecked(), fromParts (newValue, newExtra));
    }
    
    PLONK_INLINE_MID Short swap (const Short newValue) throw() 
    {
        bool success;
        Element oldAll, newAll;
        
        do 
        {
            oldAll.whole = atom.getValueUnchecked();
            newAll.separate.value = newValue;
            newAll.separate.extra = oldAll.separate.extra + 1;
            success = atom.compareAndSwap (oldAll.whole, newAll.whole);
        } while (!success);
        
        return oldAll.separate.value;
    }
    
    Short swapAll (const Short newValue, const Short newExtra) throw() 
    {
        return swapAll (newValue, newExtra, 0);
    }
    
    Short swapAll (const Short newValue, const Short newExtra, Short* oldExtraPtr) throw() 
    {
        bool success;
        Element oldAll;
        
        do 
        {
            oldAll.whole = atom.getValueUnchecked();
            success = atom.compareAndSwap (oldAll.whole, fromParts (newValue, newExtra));
        } while (!success);
        
        if (oldExtraPtr != 0)
            *oldExtraPtr = oldAll.separate.extra;
        
        return oldAll.separate.value;
    }
    
    PLONK_INLINE_MID void setValue (const Short other) throw() 
    { 
        swap (other);
    }
    
    PLONK_INLINE_MID Short getValue() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.value;
    }
    
    PLONK_INLINE_MID Short getExtra() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.extra;
    }
    
    PLONK_INLINE_MID Short getValueUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.value;
    }
    
    PLONK_INLINE_MID Short getExtraUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.extra;
    }
    
    PLONK_INLINE_MID Int getAllUnchecked() const throw()
    {
        return atom.getValueUnchecked();
    }
    
    template<class OtherType> bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Short> (other); }
    template<class OtherType> bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Short> (other); }
    template<class OtherType> bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Short> (other); }
    template<class OtherType> bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Short> (other); }
    template<class OtherType> bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Short> (other); }
    template<class OtherType> bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Short> (other); }
    
    bool operator== (const Short other) const throw() { return this->getValueUnchecked() == other; }
    bool operator!= (const Short other) const throw() { return this->getValueUnchecked() != other; }
    bool operator<  (const Short other) const throw() { return this->getValueUnchecked() <  other; }
    bool operator<= (const Short other) const throw() { return this->getValueUnchecked() <= other; }
    bool operator>  (const Short other) const throw() { return this->getValueUnchecked() >  other; }
    bool operator>= (const Short other) const throw() { return this->getValueUnchecked() >= other; }

    PLONK_INLINE_MID Short operator+= (const Short operand) throw() 
    { 
        bool success;
        Element oldAll, newAll;
        
        do 
        {
            oldAll.whole = atom.getValueUnchecked();
            newAll.separate.value = oldAll.separate.value + operand;
            newAll.separate.extra = oldAll.separate.extra + 1;
            success = atom.compareAndSwap (oldAll.whole, newAll.whole);
        } while (!success);
        
        return newAll.separate.value;
    }
    
    PLONK_INLINE_MID Short operator-= (const Short operand) throw() 
    { 
        return operator+= (-operand);
    }
    
    PLONK_INLINE_MID Short operator++() throw() 
    { 
        return operator+= (1);
    }
    
    PLONK_INLINE_MID Short operator--() throw() 
    { 
        return operator+= (-1);
    }
    
    PLONK_INLINE_MID Short operator++ (int) throw() 
    { 
        const Short oldValue = getValueUnchecked(); 
        ++(*this); 
        return oldValue; 
    }
    
    PLONK_INLINE_MID Short operator-- (int) throw() 
    { 
        const Short oldValue = getValueUnchecked(); 
        --(*this); 
        return oldValue; 
    }
    
private:    
    AtomicValue<Int> atom;
    
    struct Separate
    {
        Short value;
        Short extra;
    };
    
    union Element
    {
        Separate separate;
        Int whole;
    };
    
    static PLONK_INLINE_MID Int fromParts (const Short value, const Short extra) throw()
    {
        Element element;
        element.separate.value = value;
        element.separate.extra = extra;
        return element.whole;
    }
    
    static PLONK_INLINE_MID Separate fromWhole (const Short whole) throw()
    {
        Element element;
        element.whole = whole;
        return element.separate;
    }
    
};


#endif // PLONK_ATOMIC_H
