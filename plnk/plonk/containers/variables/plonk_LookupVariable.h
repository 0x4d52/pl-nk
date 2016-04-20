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

#ifndef PLONK_LOOKUPVARIABLE_H
#define PLONK_LOOKUPVARIABLE_H

#include "plonk_VariableInternal.h"

//==============================================================================

template<class ArrayValueType, class IndexValueType, class InterpType>
class LookupVariableInternalBase : public VariableInternalBase<ArrayValueType>
{
public:
    typedef Variable<ArrayValueType>                        VariableType;
    typedef Variable<IndexValueType>                        IndexType;
    typedef NumericalArray<ArrayValueType>                  ArrayType;
    
    LookupVariableInternalBase (ArrayType const& table, IndexType const& i) throw()
    :   array (table), index (i)
    {
        plonk_assert (table.length() >= (InterpType::getExtension() + InterpType::getOffset()));
    }
    
    ~LookupVariableInternalBase()
    {
    }
    
    const ArrayValueType getValueAtIndex (const IndexValueType indexValue) const
    {
        const int tableLength = array.length();
        
        if (tableLength == 0)
        {
            return ArrayValueType (0);
        }
        else
        {
            const int extension            = InterpType::getExtension();
            const int offset               = InterpType::getOffset();
            const ArrayValueType* rawArray = array.getArray();
            const int tempLength           = extension + offset + 1;
            int iIndex                     = (int) (indexValue + tableLength) - offset - tableLength; // so negative rounds correctly
            const IndexValueType tempIndex = plonk::clip (indexValue - iIndex, IndexValueType (0), IndexValueType (tempLength - 1));
            
            ArrayValueType temp[tempLength];
            
            for (int i = 0; i < tempLength; ++i, ++iIndex)
                temp[i] = rawArray[plonk::clip (iIndex, 0, tableLength - 1)];
            
            return InterpType::lookup (temp, tempIndex);
        }
    }
    
    ArrayValueType* getValuePtr() throw()
    {
        plonk_assertfalse;
        return nullptr;
    }
    
    void setValue (ArrayValueType const&) throw()
    {
        plonk_assertfalse;
    }
    
protected:
    ArrayType array;
    IndexType index;
};

//==============================================================================

template<class ArrayValueType, class IndexValueType, class InterpType>
class LookupMapVariableInternal : public LookupVariableInternalBase<ArrayValueType,IndexValueType,InterpType>
{
public:
    typedef LookupVariableInternalBase<ArrayValueType,IndexValueType,InterpType>    Base;
    typedef Variable<ArrayValueType>                                                VariableType;
    typedef Variable<IndexValueType>                                                IndexType;
    typedef NumericalArray<ArrayValueType>                                          ArrayType;

    LookupMapVariableInternal (ArrayType const& table, IndexType const& i) throw()
    :   Base (table, i)
    {
    }
    
    ~LookupMapVariableInternal()
    {
    }
    
    const ArrayValueType getValue() const throw()
    {
        return this->getValueAtIndex (this->index.getValue());
    }
    
    const ArrayValueType nextValue() throw()
    {
        return this->getValueAtIndex (this->index.nextValue());
    }
};

///=============================================================================

template<class ArrayValueType, class IndexValueType, class InterpType>
class LookupWaveVariableInternal : public LookupVariableInternalBase<ArrayValueType,IndexValueType,InterpType>
{
public:
    typedef LookupVariableInternalBase<ArrayValueType,IndexValueType,InterpType>    Base;
    typedef Variable<ArrayValueType>                                                VariableType;
    typedef Variable<IndexValueType>                                                IndexType;
    typedef NumericalArray<ArrayValueType>                                          ArrayType;
    
    LookupWaveVariableInternal (ArrayType const& table, IndexType const& i) throw()
    :   Base (table, i)
    {
    }
    
    ~LookupWaveVariableInternal()
    {
    }
    
    const ArrayValueType getValueAtWarppedIndex (IndexValueType index) const
    {
        const IndexValueType tableLength = (IndexValueType) this->array.length();
        
        while (index < IndexValueType (0))
            index += tableLength;
        
        while (index >= tableLength)
            index -= tableLength;
        
        return this->getValueAtIndex (index);
    }
    
    const ArrayValueType getValue() const throw()
    {
        return getValueAtWarppedIndex (this->index.getValue());
    }
    
    const ArrayValueType nextValue() throw()
    {
        return getValueAtWarppedIndex (this->index.nextValue());
    }
    
};



#endif // PLONK_LOOKUPVARIABLE_H
