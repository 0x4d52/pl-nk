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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"



IOLimit::IOLimit (const IOLimit::LimitType typeToUse, 
                        const Measure::Name measureToUse,
                        const double minimumToUse, 
                        const double maximumToUse) throw()
:   type (typeToUse),
    measure (measureToUse),
    minimum (minimumToUse),
    maximum (maximumToUse)
{
}

IOLimit IOLimit::createNone () throw()
{
    return IOLimit (IOLimit::None, Measure::None, 0.0, 0.0); // 0.0s to be ingored
}

IOLimit IOLimit::createMinimum (Measure::Name measure, const double minimum) throw()
{
    return IOLimit (IOLimit::Minimum, measure, minimum, 0.0); // 0.0 is to be ingored
}

IOLimit IOLimit::createMaximum (Measure::Name measure, const double maximum) throw()
{
    return IOLimit (IOLimit::Maximum, measure, 0.0, maximum); // 0.0 is to be ingored
}

IOLimit IOLimit::createClipped (Measure::Name measure, const double minimum, const double maximum) throw()
{
    plonk_assert(minimum < maximum);
    return IOLimit (IOLimit::Clipped, measure, minimum, maximum);
}

IOLimit::LimitType IOLimit::fromInt (const int value) throw()
{
    static const IOLimit::LimitType names[] = {
        None, 
        Minimum,
        Maximum,
        Clipped
    };
    
    if (value < 0 || value >= IOLimit::NumLimitTypes)
    {
        plonk_assertfalse;
        return IOLimit::None;
    }
    else
    {
        return names[value];    
    }
}

Text IOLimit::getName (const int keyIndex) throw()
{
    static const char* names[] = {
        "None", 
        "Minimum",
        "Maximum",
        "Clipped"        
    };
    
    if (keyIndex < 0 || keyIndex >= IOLimit::NumLimitTypes)
        return "None";
    else
        return names[keyIndex];            
}

bool IOLimit::isNone (const IOLimit::LimitType type) throw()
{
    return type == IOLimit::None;
}

bool IOLimit::isClipped (const IOLimit::LimitType type) throw()
{
    return type == IOLimit::Clipped;
}

bool IOLimit::hasMinimum (const IOLimit::LimitType type) throw()
{
    return (type == IOLimit::Minimum) || (type == IOLimit::Clipped);
}

bool IOLimit::hasMaximum (const IOLimit::LimitType type) throw()
{
    return (type == IOLimit::Maximum) || (type == IOLimit::Clipped);
}

const Text IOLimit::getName() const throw()
{
    return IOLimit::getName (type);
}

const Measure::Name IOLimit::getMeasure() const throw()
{
    return measure;
}

bool IOLimit::isNone() const throw()
{
    return IOLimit::isNone (type);
}

bool IOLimit::isClipped() const throw()
{
    return IOLimit::isClipped (type);
}

bool IOLimit::hasMinimum() const throw()
{
    return IOLimit::hasMinimum (type);
}

bool IOLimit::hasMaximum() const throw()
{
    return IOLimit::hasMaximum (type);
}

double IOLimit::getMinimum() const throw()
{
    plonk_assert(hasMinimum());
    return minimum;
}

double IOLimit::getMaximum() const throw()
{
    plonk_assert(hasMaximum());
    return maximum;
}

    

END_PLONK_NAMESPACE
