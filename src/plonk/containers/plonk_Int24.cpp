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

#include "../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "plonk_Int24.h"

Int24::Int24() throw()
:   data (getZero().data)
{
}

Int24::Int24 (const int value) throw()
:   data (pl_ConvertIToI24 (value))
{
}

Int24::Int24 (const Internal value) throw()
:   data (value)
{
}

Int24::Int24 (Int24 const& copy) throw()
:   data (copy.data)
{
}

Int24& Int24::operator= (Int24 const& other) throw()
{
    if (this != &other)
        data = other.data;
    
    return *this;
}

Int24::operator int() const throw()
{
    return pl_ConvertI24ToI (data);
}

Int24::operator float() const throw()
{
    return float (pl_ConvertI24ToI (data));
}

Int24::operator double() const throw()
{
    return double (pl_ConvertI24ToI (data));
}

Int24::operator Internal() const throw()
{
    return data;
}

int Int24::toInt() const throw()
{
    return pl_ConvertI24ToI (data);
}

float Int24::toFloat() const throw()
{
    return float (pl_ConvertI24ToI (data));
}

double Int24::toDouble() const throw()
{
    return double (pl_ConvertI24ToI (data));
}

Int24::Internal Int24::getRaw() const throw()
{
    return data;
}

Int24 Int24::operator~() const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (~pl_ConvertI24ToI (data));
    return result;            
}

Int24 Int24::operator-() const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (-pl_ConvertI24ToI (data));
    return result;            
}

Int24& Int24::operator++() throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) + 1);
    return *this;
}

Int24& Int24::operator--() throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) - 1);
    return *this;
}

Int24 Int24::operator++ (int) throw()
{
    Int24 result;
    ++(*this);
    return result;
}

Int24 Int24::operator-- (int) throw()
{
    Int24 result;
    --(*this);
    return result;    
}

Int24 Int24::operator+ (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) + pl_ConvertI24ToI (leftOperand.data));
    return result;
}

Int24 Int24::operator- (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) - pl_ConvertI24ToI (leftOperand.data));
    return result;
}

Int24 Int24::operator* (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) * pl_ConvertI24ToI (leftOperand.data));
    return result;
}

Int24 Int24::operator/ (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) / pl_ConvertI24ToI (leftOperand.data));
    return result;
}

Int24 Int24::operator% (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) % pl_ConvertI24ToI (leftOperand.data));
    return result;
}

Int24 Int24::operator| (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) | pl_ConvertI24ToI (leftOperand.data));
    return result;    
}

Int24 Int24::operator& (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) & pl_ConvertI24ToI (leftOperand.data));
    return result;        
}

Int24 Int24::operator^ (Int24 const& leftOperand) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) ^ pl_ConvertI24ToI (leftOperand.data));
    return result;            
}

Int24& Int24::operator+= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) + pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator-= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) - pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator*= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) * pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator/= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) / pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator%= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) % pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator|= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) | pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator&= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) & pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

Int24& Int24::operator^= (Int24 const& leftOperand) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) ^ pl_ConvertI24ToI (leftOperand.data));
    return *this;
}

bool Int24::operator== (Int24 const& leftOperand) const throw()
{
    return pl_ConvertI24ToI (data) == pl_ConvertI24ToI (leftOperand.data);
}

bool Int24::operator!= (Int24 const& leftOperand) const throw()
{
    return pl_ConvertI24ToI (data) != pl_ConvertI24ToI (leftOperand.data);
}

bool Int24::operator< (Int24 const& leftOperand) const throw()
{
    return pl_ConvertI24ToI (data) < pl_ConvertI24ToI (leftOperand.data);
}

bool Int24::operator<= (Int24 const& leftOperand) const throw()
{
    return pl_ConvertI24ToI (data) <= pl_ConvertI24ToI (leftOperand.data);
}

bool Int24::operator> (Int24 const& leftOperand) const throw()
{
    return pl_ConvertI24ToI (data) > pl_ConvertI24ToI (leftOperand.data);
}

bool Int24::operator>= (Int24 const& leftOperand) const throw()
{
    return pl_ConvertI24ToI (data) >= pl_ConvertI24ToI (leftOperand.data);
}

bool Int24::operator&& (Int24 const& leftOperand) const throw()
{
    return (pl_ConvertI24ToI (data) != 0) && (pl_ConvertI24ToI (leftOperand.data) != 0);
}

bool Int24::operator|| (Int24 const& leftOperand) const throw()
{
    return (pl_ConvertI24ToI (data) != 0) || (pl_ConvertI24ToI (leftOperand.data) != 0);
}

Int24 Int24::operator<< (const int shift) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) << shift);
    return result;            
}

Int24 Int24::operator>> (const int shift) const throw()
{
    Int24 result;
    result.data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) >> shift);
    return result;                
}

Int24& Int24::operator<<= (const int shift) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) << shift);
    return *this;
}

Int24& Int24::operator>>= (const int shift) throw()
{
    data = pl_ConvertIToI24 (pl_ConvertI24ToI (data) >> shift);
    return *this;
}                             

const Int24& Int24::getZero() throw()
{
    static Int24 v (0);
    return v;
}

const Int24& Int24::getOne() throw()
{
    static Int24 v (1);
    return v;
}

const Int24& Int24::getTwo() throw()
{
    static Int24 v (2);
    return v;
}

const Int24& Int24::getHalf() throw()
{
    static Int24 v (0);
    return v;
}

const Int24& Int24::getPi() throw()
{
    static Int24 v (3);
    return v;
}

const Int24& Int24::getTwoPi() throw()
{
    static Int24 v (6);
    return v;
}

std::istream& operator>> (std::istream &inputStream, Int24& value)
{
    int intValue;
    inputStream >> intValue;
    value = Int24 (intValue);
    return inputStream;
}

std::ostream& operator<< (std::ostream &outputStream, Int24 const& value)
{
    outputStream << (int)value;
    return outputStream;
}

END_PLONK_NAMESPACE
