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

template<class ArrayValueType, class IndexValueType>
class LookupVariableInternal : public VariableInternalBase<ArrayValueType>
{
public:
    typedef Variable<ArrayValueType>                        VariableType;
    typedef Variable<IndexValueType>                        IndexType;
    typedef NumericalArray<ArrayValueType>                  ArrayType;
    typedef InterpLinear<ArrayValueType,IndexValueType>     InterpType;


    LookupVariableInternal (ArrayType const& table, IndexType const& i) throw()
    :   array (table), index (i)
    {
    }
    
    ~LookupVariableInternal()
    {
    }
    
    const ArrayValueType getValueAtIndex (const IndexType indexValue) const
    {
        const int tableLength = array.length();
        
        if (tableLength == 0)
        {
            return ArrayValueType (0);
        }
        else if (indexValue <= IndexType (0))
        {
            return array.atUnchecked (0);
        }
        else
        {
            const int tableLength1 = tableLength - 1;
            const IndexType indexMax (tableLength1);

            if (indexValue >= indexMax)
            {
                return array.atUnchecked (tableLength1);
            }
            else
            {
                return InterpType::lookup (array.getArray(), indexValue);
            }
        }
    }
        
    const ArrayValueType getValue() const throw()
    {
        return getValueAtIndex (index.getValue());
    }
    
    ArrayValueType* getValuePtr() throw()
    {
        plonk_assertfalse;
        return nullptr;
    }
    
    const ArrayValueType nextValue() throw()
    {
        return getValueAtIndex (index.nextValue());
    }
    
    void setValue (ArrayValueType const&) throw()
    {
        plonk_assertfalse;
    }
    
private:
    ArrayType array;
    IndexType index;
};

#endif // PLONK_LOOKUPVARIABLE_H
