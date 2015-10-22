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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"


int ChannelCount::fromInt (const int value) throw()
{
    if (value < -2)
        return ChannelCount::InvalidChannelCount;
    else if (value == ChannelCount::InvalidChannelCount)
        return ChannelCount::InvalidChannelCount;
    else if (value == ChannelCount::VariableChannelCount)
        return ChannelCount::VariableChannelCount;
    else
        return value;
}

Text ChannelCount::getName (const int index) throw()
{
    if (index >= 0) 
        return Text::fromValue (index);
    else if (index == ChannelCount::VariableChannelCount)
        return "Variable Channel Count";
    else
        return "Invalid";
}

int ChannelCount::numOutputInfos (const int index) throw()
{
    if (index == ChannelCount::VariableChannelCount)
        return 1;
    else if (index <= 0)
        return 0;
    else
        return index;
}

//------------------------------------------------------------------------------    

#if PLONK_ANDROID || PLONK_LINUX
static UnsignedLongLong nanbits = 0xFFFFFFFFFFFFFFFF;
double IOInfo::NoDefault = *(double*)&nanbits;
#else
double IOInfo::NoDefault = std::numeric_limits<double>::quiet_NaN();
#endif
double IOInfo::True = 1.0;
double IOInfo::False = 0.0;

IOInfo::IOInfo (const IOKey::Name nameToUse, 
                const Measure::Name measureToUse,
                const double defaultValueToUse,
                const IOLimit limitToUse) throw()
:   name (nameToUse),
    measure (measureToUse),
    defaultValue (defaultValueToUse),
    limit (limitToUse)
{
}

const IOKey::Name IOInfo::getNameKey() const throw()
{
    return name;
}

const Measure::Name IOInfo::getMeasureKey() const throw()
{
    return measure;
}

bool IOInfo::hasDefaultValue() const throw()
{
    return ! pl_IsNanD (defaultValue);
}

double IOInfo::getDefaultValue() const throw()
{
    return defaultValue;
}

const IOLimit& IOInfo::getLimit() const throw()
{
    return limit;
}

bool IOInfo::hasMeasure() const throw()
{
    return (measure != Measure::Unknown) && (measure != Measure::None);
}

const Text IOInfo::getName() const throw()
{
    return IOKey::getName (name);
}

const Text IOInfo::getMeasureName() const throw()
{
    return Measure::getName (measure);
}

const Text IOInfo::getMeasureSymbol() const throw()
{
    return Measure::getSymbol (measure);
}

 

END_PLONK_NAMESPACE
