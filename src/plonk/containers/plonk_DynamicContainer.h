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

#ifndef PLONK_DYNAMICCONTAINER_H
#define PLONK_DYNAMICCONTAINER_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"

#include "../core/plonk_TypeUtility.h"


class DynamicInternal : public SmartPointer
{
public:
    typedef SmartPointerContainer<SmartPointer> GenericContainer;
    
    DynamicInternal() throw()
    :   item (static_cast<SmartPointer*> (0)),
        typeCode (0)
    {
    }
    
    ~DynamicInternal()
    {
    }
    
    template<class ContainerType>
    DynamicInternal (ContainerType const& other) throw()
    :   item (static_cast<SmartPointer*> (other.containerCopy().getInternal())),
        typeCode (TypeUtility<ContainerType>::getTypeCode())
    {
    }
    
    template<class ContainerType>
    void setItem (ContainerType const& other) throw()
    {
        typeCode = 0; // unknown
        AtomicOps::memoryBarrier();
        item = static_cast<SmartPointer*> (other.containerCopy().getInternal());
        typeCode = TypeUtility<ContainerType>::getTypeCode();
    }
    
    inline const GenericContainer& getItem() const throw()
    {
        return item;
    }
    
    inline GenericContainer& getItem() throw()
    {
        return item;
    }    
    
    inline int getTypeCode() const throw()
    {
        return typeCode;
    }
    
    inline bool isNull() const throw()
    {
        return (typeCode == 0) && (item.getInternal() == 0);
    }
    
    inline bool isNotNull() const throw()
    {
        return ! this->isNull();
    }
    
private:
    GenericContainer item;
    int typeCode;
};

/** @ingroup PlonkContainerClasses */
class Dynamic : public SmartPointerContainer<DynamicInternal>
{
public:
    typedef SmartPointerContainer<DynamicInternal>  Base;
    typedef SmartPointerContainer<SmartPointer>     GenericContainer;
    typedef WeakPointerContainer<Dynamic>           Weak;
    typedef DynamicInternal                         Internal;

    Dynamic() throw();
    
    Dynamic (Dynamic const& copy) throw();
    Dynamic& operator= (Dynamic const& other) throw();

    template<class ContainerType>
    Dynamic (ContainerType const& other) throw()
    :   Base (new DynamicInternal (other))
    {
    }
    
    explicit Dynamic (Internal* internalToUse) throw();
    static Dynamic fromWeak (Weak const& weak) throw();
        
    template<class ContainerType>
    void setItem (ContainerType const& other) throw()
    {
        this->getInternal()->setItem (other);
    }
    
    inline Dynamic containerCopy() const throw()            { return *this; }
    inline const GenericContainer& getItem() const throw()  { return this->getInternal()->getItem(); }
    inline GenericContainer& getItem() throw()              { return this->getInternal()->getItem(); }    
    inline int getTypeCode() const throw()                  { return this->getInternal()->getTypeCode(); }
#if !PLANK_APPLE_LLVM
    inline operator const Dynamic& () const throw()         { return *this; }        
    inline operator Dynamic& () throw()                     { return *this; } 
#endif
    inline bool isItemNull() const throw()                  { return this->getInternal()->isNull();}
    inline bool isItemNotNull() const throw()               { return this->getInternal()->isNotNull(); }
        
    template<class ContainerType>
    inline const ContainerType& asUnchecked() const throw()
    {              
        plonk_assert (this->isItemNotNull());
        plonk_assert (TypeUtility<ContainerType>::getTypeCode() == this->getTypeCode());
        return reinterpret_cast<const ContainerType&> (this->getItem());
    }
    
    template<class ContainerType>
    inline ContainerType& asUnchecked() throw()
    {              
        plonk_assert (this->isItemNotNull());
        plonk_assert (TypeUtility<ContainerType>::getTypeCode() == this->getTypeCode());
        return reinterpret_cast<ContainerType&> (this->getItem());
    }
    
    template<class ContainerType>
    inline ContainerType as() const throw()
    {              
        if ((this->isItemNull()) || (TypeUtility<ContainerType>::getTypeCode() != this->getTypeCode()))
            return ContainerType();
        else
            return reinterpret_cast<const ContainerType&> (this->getItem());
    }
    
    Dynamic getMaxBlockSize() const throw();    
    Dynamic getMaxSampleRate() const throw();
    int getNumChannels() const throw();
    Dynamic getChannel (const int index) throw();
    
private:
    template<class ContainerType> operator const ContainerType& () const;
    template<class ContainerType> operator ContainerType& ();
};

#endif // PLONK_DYNAMICCONTAINER_H
