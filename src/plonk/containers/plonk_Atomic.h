/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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
    inline static void memoryBarrier() throw()
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
    static inline Long incrementSize() { return sizeof (Type); }
};

template<>
class AtomicBase<void*> : public AtomicOps
{
public:
    static inline Long incrementSize() { return 1; }
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
        inline ATOMIC_CLASS() throw() {\
            pl_Atomic##FUNCCODE##_Init (getAtomicRef());\
        }\
        \
        inline ATOMIC_CLASS (const Plank##TYPECODE initialValue) throw() {\
            pl_Atomic##FUNCCODE##_Init (getAtomicRef());\
            pl_Atomic##FUNCCODE##_Set (getAtomicRef(), initialValue);\
        }\
        \
        inline ATOMIC_CLASS (ATOMIC_CLASS const& copy) throw() {\
            pl_Atomic##FUNCCODE##_Init (getAtomicRef());\
            pl_Atomic##FUNCCODE##_Set (getAtomicRef(), pl_Atomic##FUNCCODE##_Get (copy.getAtomicRef()));\
        }\
        \
        inline  ~ATOMIC_CLASS() {\
            pl_Atomic##FUNCCODE##_DeInit (getAtomicRef());\
        }\
        \
        inline ATOMIC_CLASS& operator= (ATOMIC_CLASS const& other) throw() {\
            if (this != &other) pl_Atomic##FUNCCODE##_Set (getAtomicRef(), pl_Atomic##FUNCCODE##_Get (other.getAtomicRef()));\
            return *this;\
        }\
        inline ATOMIC_CLASS& operator= (const Plank##TYPECODE other) throw() {\
            pl_Atomic##FUNCCODE##_Set (getAtomicRef(), other);\
            return *this;\
        }\
        \
        template<class OtherType>\
        inline ATOMIC_CLASS& operator= (const OtherType other) throw() {\
            pl_Atomic##FUNCCODE##_Set (getAtomicRef(), static_cast<const Plank##TYPECODE> (other));\
            return *this;\
        }\
        \
        inline bool compareAndSwap (const Plank##NUMCODE oldValue, const Plank##NUMCODE newValue) throw() {\
            return pl_Atomic##FUNCCODE##_CompareAndSwap (getAtomicRef(), oldValue, newValue);\
        }\
        \
        inline bool compareAndSwap (const Plank##NUMCODE newValue) throw() {\
            return pl_Atomic##FUNCCODE##_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), newValue);\
        }\
        \
        inline Plank##NUMCODE swap (const Plank##NUMCODE newValue) throw() {\
            return pl_Atomic##FUNCCODE##_Swap (getAtomicRef(), newValue);\
        }\
        inline void swapWith (ATOMIC_CLASS& other) throw() {\
            return pl_Atomic##FUNCCODE##_SwapOther (getAtomicRef(), other.getAtomicRef());\
        }\
        \
        inline void setValue (const Plank##TYPECODE other) throw() { pl_Atomic##FUNCCODE##_Set (getAtomicRef(), other); }\
        inline Plank##TYPECODE getValue() const throw() { return pl_Atomic##FUNCCODE##_Get (getAtomicRef()); }\
        inline Plank##TYPECODE getValueUnchecked() const throw() { return pl_Atomic##FUNCCODE##_GetUnchecked (getAtomicRef()); }\
        \
        template<class OtherType> operator OtherType () const throw() { return static_cast<OtherType> (pl_Atomic##FUNCCODE##_Get (getAtomicRef())); }\
        inline operator Plank##TYPECODE () const throw() { return pl_Atomic##FUNCCODE##_Get (getAtomicRef()); }\
        inline const Plank##NUMCODE getExtra() const throw() { return pl_Atomic##FUNCCODE##_GetExtra (getAtomicRef()); }\
        inline const Plank##NUMCODE getExtraUnchecked() const throw() { return pl_Atomic##FUNCCODE##_GetExtraUnchecked (getAtomicRef()); }\
        inline const Plank##TYPECODE operator+= (const Plank##NUMCODE operand) throw() { return pl_Atomic##FUNCCODE##_Add (getAtomicRef(), operand); }\
        inline const Plank##TYPECODE operator-= (const Plank##NUMCODE operand) throw() { return pl_Atomic##FUNCCODE##_Add (getAtomicRef(), -operand); }\
        inline const Plank##TYPECODE operator++() throw() { return pl_Atomic##FUNCCODE##_Increment (getAtomicRef()); }\
        inline const Plank##TYPECODE operator--() throw() { return pl_Atomic##FUNCCODE##_Decrement (getAtomicRef()); }\
        inline const Plank##TYPECODE operator++ (int) throw() { const Plank##TYPECODE oldValue = pl_Atomic##FUNCCODE##_Get (getAtomicRef()); ++(*this); return oldValue; }\
        inline const Plank##TYPECODE operator-- (int) throw() { const Plank##TYPECODE oldValue = pl_Atomic##FUNCCODE##_Get (getAtomicRef()); --(*this); return oldValue; }\
        \
        template<class OtherType> bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == Plank##TYPECODE (other); }\
        template<class OtherType> bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != Plank##TYPECODE (other); }\
        template<class OtherType> bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  Plank##TYPECODE (other); }\
        template<class OtherType> bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= Plank##TYPECODE (other); }\
        template<class OtherType> bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  Plank##TYPECODE (other); }\
        template<class OtherType> bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= Plank##TYPECODE (other); }\
        \
        inline const void setIfLarger (const Plank##TYPECODE newValue) throw()\
        {\
            Plank##TYPECODE oldValue;\
            bool success;\
            do {\
                oldValue = getValueUnchecked();\
                success = compareAndSwap (oldValue, newValue > oldValue ? newValue : oldValue);\
            } while (!success);\
        }\
        \
        inline const void setIfSmaller (const Plank##TYPECODE newValue) throw()\
        {\
            Plank##TYPECODE oldValue;\
            bool success;\
            do {\
                oldValue = getValueUnchecked();\
                success = compareAndSwap (oldValue, newValue < oldValue ? newValue : oldValue);\
            } while (!success);\
        }\
        \
        inline AtomTypeRef getAtomicRef() { return static_cast<AtomTypeRef> (&atomic); }\
        inline const AtomTypeRef getAtomicRef() const { return const_cast<AtomTypeRef> (&atomic); }\
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
    inline AtomicValue() throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
    }

    inline AtomicValue (Type& initialValue) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (&initialValue));
    }
    
	template<class OtherType>
    inline AtomicValue (OtherType* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), static_cast<Type*> (initialPtr));
    }
    
    inline AtomicValue (Type* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), initialPtr);
    }    
    
    inline AtomicValue (AtomicValue const& copy) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), pl_AtomicP_Get (copy.getAtomicRef()));
    }
    
    inline ~AtomicValue() 
    {
        pl_AtomicP_DeInit (getAtomicRef());
    }
    
    inline AtomicValue& operator= (AtomicValue const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicP_Set (getAtomicRef(), pl_AtomicP_Get (other.getAtomicRef()));
        
        return *this;
    }
        
	template<class OtherType>
    inline AtomicValue& operator= (OtherType* other) throw() 
    {
        pl_AtomicP_Set (getAtomicRef(), static_cast<Type*> (other));
        return *this;
    }    

    inline AtomicValue& operator= (Type& other) throw() 
    {
        pl_AtomicP_Set (getAtomicRef(), static_cast<Type*> (&other));
        return *this;
    }    
    
    inline bool compareAndSwap (const Type* oldValue, const Type* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), oldValue, newValue);
    }
    
    inline bool compareAndSwap (const Type* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), newValue);
    }
    
    inline Type* swap (const Type* newValue) throw() 
    {
        return pl_AtomicP_Swap (getAtomicRef(), newValue);
    }
    
    inline void swapWith (AtomicValue& other) throw() 
    {
        return pl_AtomicP_SwapOther (getAtomicRef(), other.getAtomicRef());
    }

    inline void setValue (Type* other) throw()              { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }
    inline void setPtr (Type* other) throw()                { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }

    inline Type* getValue() const throw()                   { return static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); }
    inline Type* getPtr() const throw()                     { return static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); }
    inline Type* operator->() const throw()                 { return static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); }

    inline Type* getValueUnchecked() const throw()          { return static_cast<Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    inline Type* getPtrUnchecked() const throw()            { return static_cast<Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    inline Long getExtra() const throw()                    { return pl_AtomicP_GetExtra (getAtomicRef()); }
    inline Long getExtraUnchecked() const throw()           { return pl_AtomicP_GetExtraUnchecked (getAtomicRef()); }

    inline operator const Type* () const throw()            { return static_cast<const Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    inline operator Type* () const throw()                  { return static_cast<Type*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }

    inline void deletePtr() throw()                         { delete this->getPtrUnchecked(); }
    inline void deleteAndZeroPtr() throw()                  { delete this->getPtrUnchecked(); this->setPtr (0); }

    template<class OtherType> inline bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Type*> (other); }
    template<class OtherType> inline bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Type*> (other); }
    template<class OtherType> inline bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Type*> (other); }
    template<class OtherType> inline bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Type*> (other); }
    template<class OtherType> inline bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Type*> (other); }
    template<class OtherType> inline bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Type*> (other); }

    inline bool operator== (Type* const other) const throw() { return this->getPtrUnchecked() == other; }
    inline bool operator!= (Type* const other) const throw() { return this->getPtrUnchecked() != other; }
    inline bool operator<  (Type* const other) const throw() { return this->getPtrUnchecked() <  other; }
    inline bool operator<= (Type* const other) const throw() { return this->getPtrUnchecked() <= other; }
    inline bool operator>  (Type* const other) const throw() { return this->getPtrUnchecked() >  other; }
    inline bool operator>= (Type* const other) const throw() { return this->getPtrUnchecked() >= other; }

    inline Type* const operator+= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), operand * AtomicValue::incrementSize()); 
    }
    
    inline Type* const operator-= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), -operand * AtomicValue::incrementSize()); 
    }
    
    inline Type* const operator++() throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    inline Type* const operator--() throw() 
    { 
        return pl_AtomicP_Subtract (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    inline Type* const operator++ (int) throw() 
    { 
        Type* const oldPtr = static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); 
        ++(*this); 
        return oldPtr; 
    }
    
    inline Type* const operator-- (int) throw() 
    { 
        Type* const oldPtr = static_cast<Type*> (pl_AtomicP_Get (getAtomicRef())); 
        --(*this); 
        return oldPtr; 
    }
    
    inline PlankAtomicPRef getAtomicRef() { return static_cast<PlankAtomicPRef> (&u.atomic); }
    inline const PlankAtomicPRef getAtomicRef() const { return const_cast<PlankAtomicPRef> (&u.atomic); }
    
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
    inline AtomicValue() throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
    }
        
	template<class OtherType>
    inline AtomicValue (OtherType* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (initialPtr));
    }
    
    inline AtomicValue (void* initialPtr) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), initialPtr);
    }    
    
    inline AtomicValue (AtomicValue const& copy) throw() 
    {
        pl_AtomicP_Init (getAtomicRef());
        pl_AtomicP_Set (getAtomicRef(), pl_AtomicP_Get (copy.getAtomicRef()));
    }
    
    inline ~AtomicValue() 
    {
        pl_AtomicP_DeInit (getAtomicRef());
    }
    
    inline AtomicValue& operator= (AtomicValue const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicP_Set (getAtomicRef(), pl_AtomicP_Get (other.getAtomicRef()));
        
        return *this;
    }
    
	template<class OtherType>
    inline AtomicValue& operator= (OtherType* other) throw() 
    {
        pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other));
        return *this;
    }    
        
    inline bool compareAndSwap (void* oldValue, void* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), oldValue, newValue);
    }
    
    inline bool compareAndSwap (void* newValue) throw() 
    {
        return pl_AtomicP_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), newValue);
    }
    
    inline void* swap (void* newValue) throw() 
    {
        return pl_AtomicP_Swap (getAtomicRef(), newValue);
    }
    
    inline void swapWith (AtomicValue& other) throw() 
    {
        return pl_AtomicP_SwapOther (getAtomicRef(), other.getAtomicRef());
    }
    
    inline void setValue (void* other) throw()              { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }
    inline void setPtr (void* other) throw()                { pl_AtomicP_Set (getAtomicRef(), static_cast<void*> (other)); }
        
    inline void* getValue() const throw()                   { return pl_AtomicP_Get (getAtomicRef()); }
    inline void* getPtr() const throw()                     { return pl_AtomicP_Get (getAtomicRef()); }
    inline void* operator->() const throw()                 { return pl_AtomicP_Get (getAtomicRef()); }
    
    inline void* getValueUnchecked() const throw()          { return pl_AtomicP_GetUnchecked (getAtomicRef()); }
    inline void* getPtrUnchecked() const throw()            { return pl_AtomicP_GetUnchecked (getAtomicRef()); }
    inline Long getExtra() const throw()                    { return pl_AtomicP_GetExtra (getAtomicRef()); }
    inline Long getExtraUnchecked() const throw()           { return pl_AtomicP_GetExtraUnchecked (getAtomicRef()); }
    
    inline operator const void* () const throw()            { return static_cast<const void*> (pl_AtomicP_GetUnchecked (getAtomicRef())); }
    inline operator void* () const throw()                  { return pl_AtomicP_GetUnchecked (getAtomicRef()); }
        
    template<class OtherType> inline bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == reinterpret_cast<void*> (other); }
    template<class OtherType> inline bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != reinterpret_cast<void*> (other); }
    template<class OtherType> inline bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  reinterpret_cast<void*> (other); }
    template<class OtherType> inline bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= reinterpret_cast<void*> (other); }
    template<class OtherType> inline bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  reinterpret_cast<void*> (other); }
    template<class OtherType> inline bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= reinterpret_cast<void*> (other); }
    
    inline bool operator== (void* const other) const throw() { return this->getPtrUnchecked() == other; }
    inline bool operator!= (void* const other) const throw() { return this->getPtrUnchecked() != other; }
    inline bool operator<  (void* const other) const throw() { return this->getPtrUnchecked() <  other; }
    inline bool operator<= (void* const other) const throw() { return this->getPtrUnchecked() <= other; }
    inline bool operator>  (void* const other) const throw() { return this->getPtrUnchecked() >  other; }
    inline bool operator>= (void* const other) const throw() { return this->getPtrUnchecked() >= other; }
    
    inline void* const operator+= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), operand * AtomicValue::incrementSize()); 
    }
    
    inline void* const operator-= (const Long operand) throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), -operand * AtomicValue::incrementSize()); 
    }
    
    inline void* const operator++() throw() 
    { 
        return pl_AtomicP_Add (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    inline void* const operator--() throw() 
    { 
        return pl_AtomicP_Subtract (getAtomicRef(), AtomicValue::incrementSize()); 
    }
    
    inline void* const operator++ (int) throw() 
    { 
        void* const oldPtr = pl_AtomicP_Get (getAtomicRef()); 
        ++(*this); 
        return oldPtr; 
    }
    
    inline void* const operator-- (int) throw() 
    { 
        void* const oldPtr = pl_AtomicP_Get (getAtomicRef()); 
        --(*this); 
        return oldPtr; 
    }
    
    inline PlankAtomicPRef getAtomicRef() { return static_cast<PlankAtomicPRef> (&atomic); }
    inline const PlankAtomicPRef getAtomicRef() const { return const_cast<PlankAtomicPRef> (&atomic); }
   
private:
    PLONK_ALIGN(PLONK_WORDSIZE) 
    PlankAtomicP atomic;
};


template<class Type>
class AtomicExtended<Type*> : public AtomicBase<Type*>
{
public:
    inline AtomicExtended() throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
    }
    
    inline AtomicExtended (Type& initialValue) throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (&initialValue));
    }
    
	template<class OtherType>
    inline AtomicExtended (OtherType* const initialPtr) throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_Set (getAtomicRef(), static_cast<Type*> (initialPtr));
    }
    
    inline AtomicExtended (Type* initialPtr) throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_Set (getAtomicRef(), initialPtr);
    }    
    
    inline AtomicExtended (AtomicExtended const& copy) throw() 
    {
        pl_AtomicPX_Init (getAtomicRef());
        pl_AtomicPX_Set (getAtomicRef(), pl_AtomicPX_Get (copy.getAtomicRef()));
    }
    
    inline ~AtomicExtended() 
    {
        pl_AtomicPX_DeInit (getAtomicRef());
    }
    
    inline AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicPX_Set (getAtomicRef(), pl_AtomicPX_Get (other.getAtomicRef()));
        
        return *this;
    }
    
//    inline AtomicExtended& operator= (void* other) throw() 
//    {
//        plonk_assert (other == 0);
//        pl_AtomicPX_Set (getAtomicRef(), static_cast<Type*> (other));
//        return *this;
//    }    
//    

	template<class OtherType>
    inline AtomicExtended& operator= (OtherType* const other) throw() 
    {
        pl_AtomicPX_Set (getAtomicRef(), static_cast<Type*> (other));
        return *this;
    }    
    
//    inline AtomicExtended& operator= (Type& other) throw() 
//    {
//        pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (&other));
//        return *this;
//    }    

    inline AtomicExtended& operator= (Type* other) throw() 
    {
        pl_AtomicPX_Set (getAtomicRef(), const_cast<Type*> (other));
        return *this;
    }        
    
    inline void setAll (Type* const other, const Long extra) throw() 
    { 
        pl_AtomicPX_SetAll (getAtomicRef(), other, extra); 
    }
    
    inline bool compareAndSwap (Type* const oldValue, const Long oldExtra, Type* const newValue, const Long newExtra) throw() 
    {
        return pl_AtomicPX_CompareAndSwap (getAtomicRef(), oldValue, oldExtra, newValue, newExtra);
    }
    
    inline bool compareAndSwap (Type* const newValue, const Long newExtra) throw() 
    {
        return pl_AtomicPX_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), this->getExtraUnchecked(), newValue, newExtra);
    }
    
    inline Type* swap (Type* const newValue) throw() 
    {
        return pl_AtomicPX_Swap (getAtomicRef(), newValue);
    }
    
    inline Type* swapAll (Type* const newValue, const Long newExtra) throw() 
    {
        return pl_AtomicPX_SwapAll (getAtomicRef(), newValue, newExtra, 0);
    }
    
    inline Type* swapAll (Type* const newValue, const Long newExtra, Long& oldExtra) throw() 
    {
        return pl_AtomicPX_SwapAll (getAtomicRef(), newValue, newExtra, &oldExtra);
    }
    
    inline void swapWith (AtomicExtended& other) throw() 
    {
        return pl_AtomicPX_SwapOther (getAtomicRef(), other.getAtomicRef());
    }
    
    inline void setValue (Type* const other) throw()        { pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (other)); }
    inline void setPtr (Type* const other) throw()          { pl_AtomicPX_Set (getAtomicRef(), static_cast<void*> (other)); }
    
    inline Type* getValue() const throw()                   { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }
    inline Type* getPtr() const throw()                     { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }
    inline Type* operator->() const throw()                 { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }
    
    inline Type* getValueUnchecked() const throw()          { return static_cast<Type*> (pl_AtomicPX_GetUnchecked (getAtomicRef())); }
    inline Type* getPtrUnchecked() const throw()            { return static_cast<Type*> (pl_AtomicPX_GetUnchecked (getAtomicRef())); }

    inline operator const Type* () const throw()            { return static_cast<const Type*> (pl_AtomicPX_GetUncheckedUnchecked (getAtomicRef())); }
    inline operator Type* () throw()                        { return static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); }

    inline Long getExtra() const throw()                    { return pl_AtomicPX_GetExtra (getAtomicRef()); }
    inline Long getExtraUnchecked() const throw()           { return pl_AtomicPX_GetExtraUnchecked (getAtomicRef()); }
    
    inline void deletePtr() throw()                         { delete this->getPtrUnchecked(); }
    inline void deleteAndZeroPtr() throw()                  { delete this->getPtrUnchecked(); this->setPtr (0); }

    template<class OtherType> inline bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Type*> (other); }
    template<class OtherType> inline bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Type*> (other); }
    template<class OtherType> inline bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Type*> (other); }
    template<class OtherType> inline bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Type*> (other); }
    template<class OtherType> inline bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Type*> (other); }
    template<class OtherType> inline bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Type*> (other); }
    
    inline bool operator== (Type* const other) const throw() { return this->getPtrUnchecked() == other; }
    inline bool operator!= (Type* const other) const throw() { return this->getPtrUnchecked() != other; }
    inline bool operator<  (Type* const other) const throw() { return this->getPtrUnchecked() <  other; }
    inline bool operator<= (Type* const other) const throw() { return this->getPtrUnchecked() <= other; }
    inline bool operator>  (Type* const other) const throw() { return this->getPtrUnchecked() >  other; }
    inline bool operator>= (Type* const other) const throw() { return this->getPtrUnchecked() >= other; }

    inline Type* const operator+= (const Long operand) throw() 
    { 
        return pl_AtomicPX_Add (getAtomicRef(), operand * AtomicExtended::incrementSize()); 
    }
    
    inline Type* const operator-= (const Long operand) throw() 
    { 
        return pl_AtomicPX_Add (getAtomicRef(), -operand * AtomicExtended::incrementSize()); 
    }
    
    inline Type* const operator++() throw() 
    { 
        return pl_AtomicPX_Add (getAtomicRef(), AtomicExtended::incrementSize()); 
    }
    
    inline Type* const operator--() throw() 
    { 
        return pl_AtomicPX_Subtract (getAtomicRef(), AtomicExtended::incrementSize()); 
    }
    
    inline Type* const operator++ (int) throw() 
    { 
        Type* const oldPtr = static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); 
        ++(*this); 
        return oldPtr; 
    }
    
    inline Type* const operator-- (int) throw() 
    { 
        Type* const oldPtr = static_cast<Type*> (pl_AtomicPX_Get (getAtomicRef())); 
        --(*this); 
        return oldPtr; 
    }
    
    inline PlankAtomicPXRef getAtomicRef() { return static_cast<PlankAtomicPXRef> (&u.atomic); }
    inline const PlankAtomicPXRef getAtomicRef() const { return const_cast<PlankAtomicPXRef> (&u.atomic); }
    
private:
    struct Debug
    {
        Type* ptr;
        PlankL extra;
    };
    
    union Union
    {
        PlankAtomicPX atomic;
        Debug debug;
    };

    PLONK_ALIGN(PLONK_WIDESIZE) Union u;
};


template<>
class AtomicExtended<Long> : public AtomicBase<Long>
{
public:
    inline AtomicExtended() throw() 
    {
        pl_AtomicLX_Init (getAtomicRef());
    }
        
    inline AtomicExtended (const Long initialValue) throw() 
    {
        pl_AtomicLX_Init (getAtomicRef());
        pl_AtomicLX_Set (getAtomicRef(), initialValue);
    }    
    
    inline AtomicExtended (const Long initialValue, const Long initialExtra) throw() 
    {
        pl_AtomicLX_Init (getAtomicRef());
        pl_AtomicLX_SetAll (getAtomicRef(), initialValue, initialExtra);
    }    
    
    inline AtomicExtended (AtomicExtended const& copy) throw() 
    {
        pl_AtomicLX_Init (getAtomicRef());
        pl_AtomicLX_Set (getAtomicRef(), pl_AtomicLX_Get (copy.getAtomicRef()));
    }
    
    inline ~AtomicExtended() 
    {
        pl_AtomicLX_DeInit (getAtomicRef());
    }
    
    inline AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            pl_AtomicLX_Set (getAtomicRef(), pl_AtomicLX_Get (other.getAtomicRef()));
        
        return *this;
    }
        
    inline AtomicExtended& operator= (const Long other) throw() 
    {
        pl_AtomicLX_Set (getAtomicRef(), other);
        return *this;
    }        
    
    inline void setAll (const Long other, const Long extra) throw() 
    { 
        pl_AtomicLX_SetAll (getAtomicRef(), other, extra); 
    }
    
    inline bool compareAndSwap (const Long oldValue, const Long oldExtra, const Long newValue, const Long newExtra) throw() 
    {
        return pl_AtomicLX_CompareAndSwap (getAtomicRef(), oldValue, oldExtra, newValue, newExtra);
    }
    
    inline bool compareAndSwap (const Long newValue, const Long newExtra) throw() 
    {
        return pl_AtomicLX_CompareAndSwap (getAtomicRef(), this->getValueUnchecked(), this->getExtraUnchecked(), newValue, newExtra);
    }
    
    inline Long swap (const Long newValue) throw() 
    {
        return pl_AtomicLX_Swap (getAtomicRef(), newValue);
    }
    
    Long swapAll (const Long newValue, const Long newExtra) throw() 
    {
        return pl_AtomicLX_SwapAll (getAtomicRef(), newValue, newExtra, 0);
    }
    
    Long swapAll (const Long newValue, const Long newExtra, Long* oldExtraPtr) throw() 
    {
        return pl_AtomicLX_SwapAll (getAtomicRef(), newValue, newExtra, oldExtraPtr);
    }
    
    inline void setValue (const Long other) throw() { pl_AtomicLX_Set (getAtomicRef(), other); }
    inline Long getValue() const throw() { return pl_AtomicLX_Get (getAtomicRef()); }
    inline Long getExtra() const throw() { return pl_AtomicLX_GetExtra (getAtomicRef()); }
    inline Long getValueUnchecked() const throw() { return pl_AtomicLX_GetUnchecked (getAtomicRef()); }
    inline Long getExtraUnchecked() const throw() { return pl_AtomicLX_GetExtraUnchecked (getAtomicRef()); }

    template<class OtherType> bool operator== (OtherType const& other) const throw() { return this->getValueUnchecked() == static_cast<Long> (other); }
    template<class OtherType> bool operator!= (OtherType const& other) const throw() { return this->getValueUnchecked() != static_cast<Long> (other); }
    template<class OtherType> bool operator<  (OtherType const& other) const throw() { return this->getValueUnchecked() <  static_cast<Long> (other); }
    template<class OtherType> bool operator<= (OtherType const& other) const throw() { return this->getValueUnchecked() <= static_cast<Long> (other); }
    template<class OtherType> bool operator>  (OtherType const& other) const throw() { return this->getValueUnchecked() >  static_cast<Long> (other); }
    template<class OtherType> bool operator>= (OtherType const& other) const throw() { return this->getValueUnchecked() >= static_cast<Long> (other); }
    
    bool operator== (const Long other) const throw() { return this->getValueUnchecked() == other; }
    bool operator!= (const Long other) const throw() { return this->getValueUnchecked() != other; }
    bool operator<  (const Long other) const throw() { return this->getValueUnchecked() <  other; }
    bool operator<= (const Long other) const throw() { return this->getValueUnchecked() <= other; }
    bool operator>  (const Long other) const throw() { return this->getValueUnchecked() >  other; }
    bool operator>= (const Long other) const throw() { return this->getValueUnchecked() >= other; }

    inline Long operator+= (const Long operand) throw() 
    { 
        return pl_AtomicLX_Add (getAtomicRef(), operand); 
    }
    
    inline Long operator-= (const Long operand) throw() 
    { 
        return pl_AtomicLX_Add (getAtomicRef(), -operand); 
    }
    
    inline Long operator++() throw() 
    { 
        return pl_AtomicLX_Add (getAtomicRef(), 1); 
    }
    
    inline Long operator--() throw() 
    { 
        return pl_AtomicLX_Subtract (getAtomicRef(), 1); 
    }
    
    inline Long operator++ (int) throw() 
    { 
        const Long oldValue = pl_AtomicLX_Get (getAtomicRef()); 
        ++(*this); 
        return oldValue; 
    }
    
    inline Long operator-- (int) throw() 
    { 
        const Long oldValue = pl_AtomicLX_Get (getAtomicRef()); 
        --(*this); 
        return oldValue; 
    }
    
    inline PlankAtomicLXRef getAtomicRef() { return static_cast<PlankAtomicLXRef> (&atomic); }
    inline const PlankAtomicLXRef getAtomicRef() const { return const_cast<PlankAtomicLXRef> (&atomic); }
    
private:
    PLONK_ALIGN(PLONK_WIDESIZE) 
    PlankAtomicLX atomic;
};

template<>
class AtomicExtended<Int> : public AtomicBase<LongLong>
{
public:
    inline AtomicExtended() throw() 
    {
    }
    
    inline AtomicExtended (const Int initialValue) throw() 
    :   atom (fromParts (initialValue, 0))
    {
    }    
    
    inline AtomicExtended (AtomicExtended const& copy) throw() 
    {
        setValue (copy.getValue());
    }
    
    inline ~AtomicExtended() 
    {
        plonk_staticassert (sizeof (Separate) == sizeof (LongLong));
        plonk_staticassert (sizeof (Element) == sizeof (LongLong));
    }
    
    inline AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            setValue (other.getValue());
        
        return *this;
    }
    
    inline AtomicExtended& operator= (const Int other) throw() 
    {
        setValue (other);
        return *this;
    }        
    
    inline void setAll (const Int other, const Int extra) throw() 
    { 
        atom.setValue (fromParts (other, extra));
    }
    
    inline bool compareAndSwap (const Int oldValue, const Int oldExtra, const Int newValue, const Int newExtra) throw() 
    {
        return atom.compareAndSwap (fromParts (oldValue, oldExtra), fromParts (newValue, newExtra));
    }
    
    inline bool compareAndSwap (const Int newValue, const Int newExtra) throw() 
    {
        return atom.compareAndSwap (atom.getValueUnchecked(), fromParts (newValue, newExtra));
    }
    
    inline Int swap (const Int newValue) throw() 
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
    
    inline void setValue (const Int other) throw() 
    { 
        swap (other);
    }
    
    inline Int getValue() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.value;
    }
    
    inline Int getExtra() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.extra;
    }
    
    inline Int getValueUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.value;
    }
    
    inline Int getExtraUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.extra;
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

    inline Int operator+= (const Int operand) throw() 
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
    
    inline Int operator-= (const Int operand) throw() 
    { 
        return operator+= (-operand);
    }
    
    inline Int operator++() throw() 
    { 
        return operator+= (1);
    }
    
    inline Int operator--() throw() 
    { 
        return operator+= (-1);
    }
    
    inline Int operator++ (int) throw() 
    { 
        const Int oldValue = getValueUnchecked(); 
        ++(*this); 
        return oldValue; 
    }
    
    inline Int operator-- (int) throw() 
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
    
    static inline LongLong fromParts (const Int value, const Int extra) throw()
    {
        Element element = { { value, extra } };
        return element.whole;
    }
    
    static inline Separate fromWhole (LongLong const& whole) throw()
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
    inline AtomicExtended() throw() 
    {
    }
    
    inline AtomicExtended (const Short initialValue) throw() 
    :   atom (fromParts (initialValue, 0))
    {
    }    
    
    inline AtomicExtended (AtomicExtended const& copy) throw() 
    {
        setValue (copy.getValue());
    }
    
    inline ~AtomicExtended() 
    {
        plonk_staticassert (sizeof (Separate) == sizeof (Int));
        plonk_staticassert (sizeof (Element) == sizeof (Int));
    }
    
    inline AtomicExtended& operator= (AtomicExtended const& other) throw() 
    {
        if (this != &other) 
            setValue (other.getValue());
        
        return *this;
    }
    
    inline AtomicExtended& operator= (const Short other) throw() 
    {
        setValue (other);
        return *this;
    }        
    
    inline void setAll (const Short other, const Short extra) throw() 
    { 
        atom.setValue (fromParts (other, extra));
    }
    
    inline bool compareAndSwap (const Short oldValue, const Short oldExtra, const Short newValue, const Short newExtra) throw() 
    {
        return atom.compareAndSwap (fromParts (oldValue, oldExtra), fromParts (newValue, newExtra));
    }
    
    inline bool compareAndSwap (const Short newValue, const Short newExtra) throw() 
    {
        return atom.compareAndSwap (atom.getValueUnchecked(), fromParts (newValue, newExtra));
    }
    
    inline Short swap (const Short newValue) throw() 
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
    
    inline void setValue (const Short other) throw() 
    { 
        swap (other);
    }
    
    inline Short getValue() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.value;
    }
    
    inline Short getExtra() const throw() 
    { 
        Element element;
        element.whole = atom.getValue();
        return element.separate.extra;
    }
    
    inline Short getValueUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.value;
    }
    
    inline Short getExtraUnchecked() const throw() 
    { 
        Element element;
        element.whole = atom.getValueUnchecked();
        return element.separate.extra;
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

    inline Short operator+= (const Short operand) throw() 
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
    
    inline Short operator-= (const Short operand) throw() 
    { 
        return operator+= (-operand);
    }
    
    inline Short operator++() throw() 
    { 
        return operator+= (1);
    }
    
    inline Short operator--() throw() 
    { 
        return operator+= (-1);
    }
    
    inline Short operator++ (int) throw() 
    { 
        const Short oldValue = getValueUnchecked(); 
        ++(*this); 
        return oldValue; 
    }
    
    inline Short operator-- (int) throw() 
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
    
    static inline Int fromParts (const Short value, const Short extra) throw()
    {
        Element element = { { value, extra } };
        return element.whole;
    }
    
    static inline Separate fromWhole (const Short whole) throw()
    {
        Element element;
        element.whole = whole;
        return element.separate;
    }
    
};


#endif // PLONK_ATOMIC_H
